/* -*- Mode: C; tab-width: 2; indent-tabs-mode: t; c-basic-offset: 2 -*- */
/* Copyright 2005 Jan Schmidt <thaytan@mad.scientist.com>
 * Copyright 2002,2003 Scott Wheeler <wheeler@kde.org> (portions from taglib)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <gst/tag/tag.h>

#include "id3tags.h"

GST_DEBUG_CATEGORY_EXTERN (id3demux_debug);
#define GST_CAT_DEFAULT (id3demux_debug)

#define HANDLE_INVALID_SYNCSAFE
static ID3TagsResult
id3demux_id3v2_frames_to_tag_list (ID3TagsWorking * work, guint size);

guint
read_synch_uint (const guint8 * data, guint size)
{
  gint i;
  guint result = 0;
  gint invalid = 0;

  g_assert (size <= 4);

  size--;
  for (i = 0; i <= size; i++) {
    invalid |= data[i] & 0x80;
    result |= (data[i] & 0x7f) << ((size - i) * 7);
  }

#ifdef HANDLE_INVALID_SYNCSAFE
  if (invalid) {
    GST_WARNING ("Invalid synch-safe integer in ID3v2 frame "
        "- using the actual value instead");
    result = 0;
    for (i = 0; i <= size; i++) {
      result |= data[i] << ((size - i) * 8);
    }
  }
#endif
  return result;
}

guint
id3demux_calc_id3v2_tag_size (GstBuffer * buf)
{
  guint8 *data, flags;
  guint size;

  g_assert (buf != NULL);
  g_assert (GST_BUFFER_SIZE (buf) >= ID3V2_HDR_SIZE);

  data = GST_BUFFER_DATA (buf);

  /* Check for 'ID3' string at start of buffer */
  if (data[0] != 'I' || data[1] != 'D' || data[2] != '3') {
    GST_DEBUG ("No ID3v2 tag in data");
    return 0;
  }

  /* Read the flags */
  flags = data[5];

  /* Read the size from the header */
  size = read_synch_uint (data + 6, 4);
  if (size == 0)
    return ID3V2_HDR_SIZE;

  size += ID3V2_HDR_SIZE;

  /* Expand the read size to include a footer if there is one */
  if ((flags & ID3V2_HDR_FLAG_FOOTER))
    size += 10;

  GST_DEBUG ("ID3v2 tag, size: %u bytes", size);
  return size;
}

guint8 *
id3demux_ununsync_data (const guint8 * unsync_data, guint32 * size)
{
  const guint8 *end;
  guint8 *out, *uu;
  guint out_size;

  uu = out = g_malloc (*size);

  for (end = unsync_data + *size; unsync_data < end - 1; ++unsync_data, ++uu) {
    *uu = *unsync_data;
    if (G_UNLIKELY (*unsync_data == 0xff && *(unsync_data + 1) == 0x00))
      ++unsync_data;
  }

  /* take care of last byte (if last two bytes weren't 0xff 0x00) */
  if (unsync_data < end) {
    *uu = *unsync_data;
    ++uu;
  }

  out_size = uu - out;
  GST_DEBUG ("size after un-unsyncing: %u (before: %u)", out_size, *size);

  *size = out_size;
  return out;
}

/* caller must pass buffer with full ID3 tag */
ID3TagsResult
id3demux_read_id3v2_tag (GstBuffer * buffer, guint * id3v2_size,
    GstTagList ** tags)
{
  guint8 *data, *uu_data = NULL;
  guint read_size;
  ID3TagsWorking work;
  guint8 flags;
  ID3TagsResult result;
  guint16 version;

  read_size = id3demux_calc_id3v2_tag_size (buffer);

  if (id3v2_size)
    *id3v2_size = read_size;

  /* Ignore tag if it has no frames attached, but skip the header then */
  if (read_size <= ID3V2_HDR_SIZE)
    return ID3TAGS_BROKEN_TAG;

  data = GST_BUFFER_DATA (buffer);

  /* Read the version */
  version = GST_READ_UINT16_BE (data + 3);

  /* Read the flags */
  flags = data[5];

  /* Validate the version. At the moment, we only support up to 2.4.0 */
  if (ID3V2_VER_MAJOR (version) > 4 || ID3V2_VER_MINOR (version) > 0) {
    GST_WARNING ("ID3v2 tag is from revision 2.%d.%d, "
        "but decoder only supports 2.%d.%d. Ignoring as per spec.",
        version >> 8, version & 0xff, ID3V2_VERSION >> 8, ID3V2_VERSION & 0xff);
    return ID3TAGS_READ_TAG;
  }

  GST_DEBUG ("ID3v2 header flags: %s %s %s %s",
      (flags & ID3V2_HDR_FLAG_UNSYNC) ? "UNSYNC" : "",
      (flags & ID3V2_HDR_FLAG_EXTHDR) ? "EXTENDED_HEADER" : "",
      (flags & ID3V2_HDR_FLAG_EXPERIMENTAL) ? "EXPERIMENTAL" : "",
      (flags & ID3V2_HDR_FLAG_FOOTER) ? "FOOTER" : "");

  /* This shouldn't really happen! Caller should have checked first */
  if (GST_BUFFER_SIZE (buffer) < read_size) {
    GST_DEBUG
        ("Found ID3v2 tag with revision 2.%d.%d - need %u more bytes to read",
        version >> 8, version & 0xff,
        (guint) (read_size - GST_BUFFER_SIZE (buffer)));
    return ID3TAGS_MORE_DATA;   /* Need more data to decode with */
  }

  GST_DEBUG ("Reading ID3v2 tag with revision 2.%d.%d of size %u", version >> 8,
      version & 0xff, read_size);

  g_return_val_if_fail (tags != NULL, ID3TAGS_READ_TAG);

  GST_MEMDUMP ("ID3v2 tag", GST_BUFFER_DATA (buffer), read_size);

  memset (&work, 0, sizeof (ID3TagsWorking));
  work.buffer = buffer;
  work.hdr.version = version;
  work.hdr.size = read_size;
  work.hdr.flags = flags;
  work.hdr.frame_data = GST_BUFFER_DATA (buffer) + ID3V2_HDR_SIZE;
  if (flags & ID3V2_HDR_FLAG_FOOTER)
    work.hdr.frame_data_size = read_size - ID3V2_HDR_SIZE - 10;
  else
    work.hdr.frame_data_size = read_size - ID3V2_HDR_SIZE;

  /* in v2.3 the frame sizes are not syncsafe, so the entire tag had to be
   * unsynced. In v2.4 the frame sizes are syncsafe so it's just the frame
   * data that needs un-unsyncing, but not the frame headers. */
  if ((flags & ID3V2_HDR_FLAG_UNSYNC) != 0 && ID3V2_VER_MAJOR (version) <= 3) {
    GST_DEBUG ("Un-unsyncing entire tag");
    uu_data = id3demux_ununsync_data (work.hdr.frame_data,
        &work.hdr.frame_data_size);
    work.hdr.frame_data = uu_data;
    GST_MEMDUMP ("ID3v2 tag (un-unsyced)", uu_data, work.hdr.frame_data_size);
  }

  result = id3demux_id3v2_frames_to_tag_list (&work, work.hdr.frame_data_size);

  *tags = work.tags;

  g_free (uu_data);

  return result;
}

static guint
id3demux_id3v2_frame_hdr_size (guint id3v2ver)
{
  /* ID3v2 < 2.3.0 only had 6 byte header */
  switch (ID3V2_VER_MAJOR (id3v2ver)) {
    case 0:
    case 1:
    case 2:
      return 6;
    case 3:
    case 4:
    default:
      return 10;
  }
}

static const gchar *obsolete_frame_ids[] = {
  "CRM", "EQU", "LNK", "RVA", "TIM", "TSI",     /* From 2.2 */
  "EQUA", "RVAD", "TIME", "TRDA", "TSIZ",       /* From 2.3 */
  NULL
};

const struct ID3v2FrameIDConvert
{
  gchar *orig;
  gchar *new;
} frame_id_conversions[] = {
  /* 2.3.x frames */
  {
  "TORY", "TDOR"}, {
  "TYER", "TDRC"},
      /* 2.2.x frames */
  {
  "BUF", "RBUF"}, {
  "CNT", "PCNT"}, {
  "COM", "COMM"}, {
  "CRA", "AENC"}, {
  "ETC", "ETCO"}, {
  "GEO", "GEOB"}, {
  "IPL", "TIPL"}, {
  "MCI", "MCDI"}, {
  "MLL", "MLLT"}, {
  "PIC", "APIC"}, {
  "POP", "POPM"}, {
  "REV", "RVRB"}, {
  "SLT", "SYLT"}, {
  "STC", "SYTC"}, {
  "TAL", "TALB"}, {
  "TBP", "TBPM"}, {
  "TCM", "TCOM"}, {
  "TCO", "TCON"}, {
  "TCR", "TCOP"}, {
  "TDA", "TDAT"}, {             /* obsolete, but we need to parse it anyway */
  "TDY", "TDLY"}, {
  "TEN", "TENC"}, {
  "TFT", "TFLT"}, {
  "TKE", "TKEY"}, {
  "TLA", "TLAN"}, {
  "TLE", "TLEN"}, {
  "TMT", "TMED"}, {
  "TOA", "TOAL"}, {
  "TOF", "TOFN"}, {
  "TOL", "TOLY"}, {
  "TOR", "TDOR"}, {
  "TOT", "TOAL"}, {
  "TP1", "TPE1"}, {
  "TP2", "TPE2"}, {
  "TP3", "TPE3"}, {
  "TP4", "TPE4"}, {
  "TPA", "TPOS"}, {
  "TPB", "TPUB"}, {
  "TRC", "TSRC"}, {
  "TRD", "TDRC"}, {
  "TRK", "TRCK"}, {
  "TSS", "TSSE"}, {
  "TT1", "TIT1"}, {
  "TT2", "TIT2"}, {
  "TT3", "TIT3"}, {
  "TXT", "TOLY"}, {
  "TXX", "TXXX"}, {
  "TYE", "TDRC"}, {
  "UFI", "UFID"}, {
  "ULT", "USLT"}, {
  "WAF", "WOAF"}, {
  "WAR", "WOAR"}, {
  "WAS", "WOAS"}, {
  "WCM", "WCOM"}, {
  "WCP", "WCOP"}, {
  "WPB", "WPUB"}, {
  "WXX", "WXXX"}, {
  NULL, NULL}
};

static gboolean
convert_fid_to_v240 (gchar * frame_id)
{
  gint i = 0;

  while (obsolete_frame_ids[i] != NULL) {
    if (strncmp (frame_id, obsolete_frame_ids[i], 5) == 0)
      return TRUE;
    i++;
  }

  i = 0;
  while (frame_id_conversions[i].orig != NULL) {
    if (strncmp (frame_id, frame_id_conversions[i].orig, 5) == 0) {
      strcpy (frame_id, frame_id_conversions[i].new);
      return FALSE;
    }
    i++;
  }
  return FALSE;
}


/* add unknown or unhandled ID3v2 frames to the taglist as binary blobs */
static void
id3demux_add_id3v2_frame_blob_to_taglist (ID3TagsWorking * work, guint size)
{
  GstBuffer *blob;
  GstCaps *caps;
  guint8 *frame_data;
  gchar *media_type;
  guint frame_size, header_size;

  switch (ID3V2_VER_MAJOR (work->hdr.version)) {
    case 1:
    case 2:
      header_size = 3 + 3;
      break;
    case 3:
    case 4:
      header_size = 4 + 4 + 2;
      break;
    default:
      g_return_if_reached ();
  }

  frame_data = work->hdr.frame_data - header_size;
  frame_size = size + header_size;

  blob = gst_buffer_new_and_alloc (frame_size);
  memcpy (GST_BUFFER_DATA (blob), frame_data, frame_size);

  media_type = g_strdup_printf ("application/x-gst-id3v2-%c%c%c%c-frame",
      g_ascii_tolower (frame_data[0]), g_ascii_tolower (frame_data[1]),
      g_ascii_tolower (frame_data[2]), g_ascii_tolower (frame_data[3]));
  caps = gst_caps_new_simple (media_type, "version", G_TYPE_INT,
      (gint) ID3V2_VER_MAJOR (work->hdr.version), NULL);
  gst_buffer_set_caps (blob, caps);
  gst_caps_unref (caps);
  g_free (media_type);

  /* gst_util_dump_mem (GST_BUFFER_DATA (blob), GST_BUFFER_SIZE (blob)); */

  gst_tag_list_add (work->tags, GST_TAG_MERGE_APPEND,
      GST_ID3_DEMUX_TAG_ID3V2_FRAME, blob, NULL);
  gst_buffer_unref (blob);
}

static ID3TagsResult
id3demux_id3v2_frames_to_tag_list (ID3TagsWorking * work, guint size)
{
  guint frame_hdr_size;
  guint8 *start;

  /* Extended header if present */
  if (work->hdr.flags & ID3V2_HDR_FLAG_EXTHDR) {
    work->hdr.ext_hdr_size = read_synch_uint (work->hdr.frame_data, 4);
    if (work->hdr.ext_hdr_size < 6 ||
        (work->hdr.ext_hdr_size) > work->hdr.frame_data_size) {
      GST_DEBUG ("Invalid extended header. Broken tag");
      return ID3TAGS_BROKEN_TAG;
    }
    work->hdr.ext_flag_bytes = work->hdr.frame_data[4];
    if (5 + work->hdr.ext_flag_bytes > work->hdr.frame_data_size) {
      GST_DEBUG
          ("Tag claims extended header, but doesn't have enough bytes. Broken tag");
      return ID3TAGS_BROKEN_TAG;
    }

    work->hdr.ext_flag_data = work->hdr.frame_data + 5;
    work->hdr.frame_data += work->hdr.ext_hdr_size;
    work->hdr.frame_data_size -= work->hdr.ext_hdr_size;
  }

  start = GST_BUFFER_DATA (work->buffer);
  frame_hdr_size = id3demux_id3v2_frame_hdr_size (work->hdr.version);
  if (work->hdr.frame_data_size <= frame_hdr_size) {
    GST_DEBUG ("Tag has no data frames. Broken tag");
    return ID3TAGS_BROKEN_TAG;  /* Must have at least one frame */
  }

  work->tags = gst_tag_list_new ();
  g_return_val_if_fail (work->tags != NULL, ID3TAGS_READ_TAG);

  while (work->hdr.frame_data_size > frame_hdr_size) {
    guint frame_size = 0;
    gchar frame_id[5] = "";
    guint16 frame_flags = 0x0;
    gboolean obsolete_id = FALSE;
    gboolean read_synch_size = TRUE;

    /* Read the header */
    switch (ID3V2_VER_MAJOR (work->hdr.version)) {
      case 0:
      case 1:
      case 2:
        frame_id[0] = work->hdr.frame_data[0];
        frame_id[1] = work->hdr.frame_data[1];
        frame_id[2] = work->hdr.frame_data[2];
        frame_id[3] = 0;
        frame_id[4] = 0;
        obsolete_id = convert_fid_to_v240 (frame_id);

        /* 3 byte non-synchsafe size */
        frame_size = work->hdr.frame_data[3] << 16 |
            work->hdr.frame_data[4] << 8 | work->hdr.frame_data[5];
        frame_flags = 0;
        break;
      case 3:
        read_synch_size = FALSE;        /* 2.3 frame size is not synch-safe */
      case 4:
      default:
        frame_id[0] = work->hdr.frame_data[0];
        frame_id[1] = work->hdr.frame_data[1];
        frame_id[2] = work->hdr.frame_data[2];
        frame_id[3] = work->hdr.frame_data[3];
        frame_id[4] = 0;
        if (read_synch_size)
          frame_size = read_synch_uint (work->hdr.frame_data + 4, 4);
        else
          frame_size = GST_READ_UINT32_BE (work->hdr.frame_data + 4);

        frame_flags = GST_READ_UINT16_BE (work->hdr.frame_data + 8);

        if (ID3V2_VER_MAJOR (work->hdr.version) == 3) {
          frame_flags &= ID3V2_3_FRAME_FLAGS_MASK;
          obsolete_id = convert_fid_to_v240 (frame_id);
          if (obsolete_id)
            GST_DEBUG ("Ignoring v2.3 frame %s", frame_id);
        }
        break;
    }

    work->hdr.frame_data += frame_hdr_size;
    work->hdr.frame_data_size -= frame_hdr_size;

    if (frame_size > work->hdr.frame_data_size || strcmp (frame_id, "") == 0)
      break;                    /* No more frames to read */

#if 1
    GST_LOG
        ("Frame @ %d (0x%02x) id %s size %d, next=%d (0x%02x) obsolete=%d",
        work->hdr.frame_data - start, work->hdr.frame_data - start, frame_id,
        frame_size, work->hdr.frame_data + frame_hdr_size + frame_size - start,
        work->hdr.frame_data + frame_hdr_size + frame_size - start,
        obsolete_id);
#define flag_string(flag,str) \
        ((frame_flags & (flag)) ? (str) : "")
    GST_LOG ("Frame header flags: 0x%04x %s %s %s %s %s %s %s", frame_flags,
        flag_string (ID3V2_FRAME_STATUS_FRAME_ALTER_PRESERVE, "ALTER_PRESERVE"),
        flag_string (ID3V2_FRAME_STATUS_READONLY, "READONLY"),
        flag_string (ID3V2_FRAME_FORMAT_GROUPING_ID, "GROUPING_ID"),
        flag_string (ID3V2_FRAME_FORMAT_COMPRESSION, "COMPRESSION"),
        flag_string (ID3V2_FRAME_FORMAT_ENCRYPTION, "ENCRYPTION"),
        flag_string (ID3V2_FRAME_FORMAT_UNSYNCHRONISATION, "UNSYNC"),
        flag_string (ID3V2_FRAME_FORMAT_DATA_LENGTH_INDICATOR, "LENGTH_IND"));
#undef flag_str
#endif

    if (!obsolete_id) {
      /* Now, read, decompress etc the contents of the frame
       * into a TagList entry */
      work->cur_frame_size = frame_size;
      work->frame_id = frame_id;
      work->frame_flags = frame_flags;

      if (id3demux_id3v2_parse_frame (work)) {
        GST_LOG ("Extracted frame with id %s", frame_id);
      } else {
        GST_LOG ("Failed to extract frame with id %s", frame_id);
        id3demux_add_id3v2_frame_blob_to_taglist (work, frame_size);
      }
    }
    work->hdr.frame_data += frame_size;
    work->hdr.frame_data_size -= frame_size;
  }

  if (gst_structure_n_fields (GST_STRUCTURE (work->tags)) == 0) {
    GST_DEBUG ("Could not extract any frames from tag. Broken or empty tag");
    gst_tag_list_free (work->tags);
    work->tags = NULL;
    return ID3TAGS_BROKEN_TAG;
  }

  /* Set day/month now if they were in a separate (obsolete) TDAT frame */
  if (work->pending_day != 0 && work->pending_month != 0) {
    GDate *date = NULL;

    if (gst_tag_list_get_date (work->tags, GST_TAG_DATE, &date)) {
      g_date_set_day (date, work->pending_day);
      g_date_set_month (date, work->pending_month);
      gst_tag_list_add (work->tags, GST_TAG_MERGE_REPLACE, GST_TAG_DATE,
          date, NULL);
      g_date_free (date);
    }
  }

  return ID3TAGS_READ_TAG;
}
