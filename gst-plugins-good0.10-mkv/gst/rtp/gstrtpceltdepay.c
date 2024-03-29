/* GStreamer
 * Copyright (C) <2009> Wim Taymans <wim.taymans@gmail.com>
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
#  include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <gst/rtp/gstrtpbuffer.h>

#include "gstrtpceltdepay.h"

/* elementfactory information */
static const GstElementDetails gst_rtp_celtdepay_details =
GST_ELEMENT_DETAILS ("RTP CELT depayloader",
    "Codec/Depayloader/Network",
    "Extracts CELT audio from RTP packets",
    "Wim Taymans <wim.taymans@gmail.com>");

/* RtpCELTDepay signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  ARG_0
};

static GstStaticPadTemplate gst_rtp_celt_depay_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-rtp, "
        "media = (string) \"audio\", "
        "payload = (int) " GST_RTP_PAYLOAD_DYNAMIC_STRING ", "
        "clock-rate =  (int) [32000, 48000], "
        "encoding-name = (string) \"CELT\"")
    );

static GstStaticPadTemplate gst_rtp_celt_depay_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-celt")
    );

static GstBuffer *gst_rtp_celt_depay_process (GstBaseRTPDepayload * depayload,
    GstBuffer * buf);
static gboolean gst_rtp_celt_depay_setcaps (GstBaseRTPDepayload * depayload,
    GstCaps * caps);

GST_BOILERPLATE (GstRtpCELTDepay, gst_rtp_celt_depay, GstBaseRTPDepayload,
    GST_TYPE_BASE_RTP_DEPAYLOAD);

static void
gst_rtp_celt_depay_base_init (gpointer klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_rtp_celt_depay_src_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_rtp_celt_depay_sink_template));
  gst_element_class_set_details (element_class, &gst_rtp_celtdepay_details);
}

static void
gst_rtp_celt_depay_class_init (GstRtpCELTDepayClass * klass)
{
  GstBaseRTPDepayloadClass *gstbasertpdepayload_class;

  gstbasertpdepayload_class = (GstBaseRTPDepayloadClass *) klass;

  gstbasertpdepayload_class->process = gst_rtp_celt_depay_process;
  gstbasertpdepayload_class->set_caps = gst_rtp_celt_depay_setcaps;
}

static void
gst_rtp_celt_depay_init (GstRtpCELTDepay * rtpceltdepay,
    GstRtpCELTDepayClass * klass)
{
}

/* len 4 bytes LE,
 * vendor string (len bytes),
 * user_len 4 (0) bytes LE
 */
static const gchar gst_rtp_celt_comment[] =
    "\045\0\0\0Depayloaded with GStreamer celtdepay\0\0\0\0";

static gboolean
gst_rtp_celt_depay_setcaps (GstBaseRTPDepayload * depayload, GstCaps * caps)
{
  GstStructure *structure;
  GstRtpCELTDepay *rtpceltdepay;
  gint clock_rate, nb_channels, frame_size;
  GstBuffer *buf;
  guint8 *data;
  const gchar *params;
  GstCaps *srccaps;
  gboolean res;

  rtpceltdepay = GST_RTP_CELT_DEPAY (depayload);

  structure = gst_caps_get_structure (caps, 0);

  if (!gst_structure_get_int (structure, "clock-rate", &clock_rate))
    goto no_clockrate;
  depayload->clock_rate = clock_rate;

  if (!(params = gst_structure_get_string (structure, "encoding-params")))
    nb_channels = 1;
  else {
    nb_channels = atoi (params);
  }

  if (!(params = gst_structure_get_string (structure, "frame-size")))
    frame_size = 480;
  else {
    frame_size = atoi (params);
  }

  /* construct minimal header and comment packet for the decoder */
  buf = gst_buffer_new_and_alloc (60);
  data = GST_BUFFER_DATA (buf);
  memcpy (data, "CELT    ", 8);
  data += 8;
  memcpy (data, "1.1.12", 7);
  data += 20;
  GST_WRITE_UINT32_LE (data, 0x80000006);       /* version */
  data += 4;
  GST_WRITE_UINT32_LE (data, 56);       /* header_size */
  data += 4;
  GST_WRITE_UINT32_LE (data, clock_rate);       /* rate */
  data += 4;
  GST_WRITE_UINT32_LE (data, nb_channels);      /* channels */
  data += 4;
  GST_WRITE_UINT32_LE (data, frame_size);       /* frame-size */
  data += 4;
  GST_WRITE_UINT32_LE (data, -1);       /* overlap */
  data += 4;
  GST_WRITE_UINT32_LE (data, -1);       /* bytes_per_packet */
  data += 4;
  GST_WRITE_UINT32_LE (data, 0);        /* extra headers */

  srccaps = gst_caps_new_simple ("audio/x-celt", NULL);
  res = gst_pad_set_caps (depayload->srcpad, srccaps);
  gst_caps_unref (srccaps);

  gst_buffer_set_caps (buf, GST_PAD_CAPS (depayload->srcpad));
  gst_base_rtp_depayload_push (GST_BASE_RTP_DEPAYLOAD (rtpceltdepay), buf);

  buf = gst_buffer_new_and_alloc (sizeof (gst_rtp_celt_comment));
  memcpy (GST_BUFFER_DATA (buf), gst_rtp_celt_comment,
      sizeof (gst_rtp_celt_comment));

  gst_buffer_set_caps (buf, GST_PAD_CAPS (depayload->srcpad));
  gst_base_rtp_depayload_push (GST_BASE_RTP_DEPAYLOAD (rtpceltdepay), buf);

  return res;

  /* ERRORS */
no_clockrate:
  {
    GST_DEBUG_OBJECT (depayload, "no clock-rate specified");
    return FALSE;
  }
}

static GstBuffer *
gst_rtp_celt_depay_process (GstBaseRTPDepayload * depayload, GstBuffer * buf)
{
  GstBuffer *outbuf = NULL;
  guint8 *payload;
  guint offset, pos, payload_len, total_size, size;
  guint8 s;

  GST_DEBUG ("process : got %d bytes, mark %d ts %u seqn %d",
      GST_BUFFER_SIZE (buf),
      gst_rtp_buffer_get_marker (buf),
      gst_rtp_buffer_get_timestamp (buf), gst_rtp_buffer_get_seq (buf));

  payload = gst_rtp_buffer_get_payload (buf);
  payload_len = gst_rtp_buffer_get_payload_len (buf);

  /* first count how many bytes are consumed by the size headers and make offset
   * point to the first data byte */
  total_size = 0;
  offset = 0;
  while (total_size < payload_len) {
    do {
      s = payload[offset++];
      total_size += s + 1;
    } while (s == 0xff);
  }

  /* offset is now pointing to the payload */
  total_size = 0;
  pos = 0;
  while (total_size < payload_len) {
    size = 0;
    do {
      s = payload[pos++];
      size += s;
      total_size += size + 1;
    } while (s == 0xff);

    outbuf = gst_rtp_buffer_get_payload_subbuffer (buf, offset, size);
    offset += size;

    gst_base_rtp_depayload_push (depayload, outbuf);
  }
  return NULL;
}

gboolean
gst_rtp_celt_depay_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "rtpceltdepay",
      GST_RANK_MARGINAL, GST_TYPE_RTP_CELT_DEPAY);
}
