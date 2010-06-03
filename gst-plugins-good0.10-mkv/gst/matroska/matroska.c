/* GStreamer Matroska muxer/demuxer
 * (c) 2003 Ronald Bultje <rbultje@ronald.bitfreak.net>
 *
 * matroska.c: plugin loader
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

#include "matroska-demux.h"
#include "matroska-mux.h"
#include "matroska-ids.h"

static gboolean
ebml_check_header (GstTypeFind * tf, const gchar * doctype, int doctype_len)
{
  /* 4 bytes for EBML ID, 1 byte for header length identifier */
  guint8 *data = gst_type_find_peek (tf, 0, 4 + 1);
  gint len_mask = 0x80, size = 1, n = 1, total;

  if (!data)
    return FALSE;

  /* ebml header? */
  if (data[0] != 0x1A || data[1] != 0x45 || data[2] != 0xDF || data[3] != 0xA3)
    return FALSE;

  /* length of header */
  total = data[4];
  while (size <= 8 && !(total & len_mask)) {
    size++;
    len_mask >>= 1;
  }
  if (size > 8)
    return FALSE;
  total &= (len_mask - 1);
  while (n < size)
    total = (total << 8) | data[4 + n++];

  /* get new data for full header, 4 bytes for EBML ID,
 *    * EBML length tag and the actual header */
  data = gst_type_find_peek (tf, 0, 4 + size + total);
  if (!data)
    return FALSE;

  /* the header must contain the doctype. For now, we don't parse the
 *    * whole header but simply check for the availability of that array
 *       * of characters inside the header. Not fully fool-proof, but good
 *          * enough. */
  for (n = 4 + size; n <= 4 + size + total - doctype_len; n++)
    if (!memcmp (&data[n], doctype, doctype_len))
      return TRUE;

  return FALSE;
}

/*** video/webm ***/
static GstStaticCaps webm_caps = GST_STATIC_CAPS ("video/webm");

#define WEBM_CAPS (gst_static_caps_get(&webm_caps))
static void
webm_type_find (GstTypeFind * tf, gpointer ununsed)
{
  if (ebml_check_header (tf, "webm", 4))
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, WEBM_CAPS);
}

#define TYPE_FIND_REGISTER(plugin,name,rank,func,ext,caps,priv,notify) \
G_BEGIN_DECLS{\
  if (!gst_type_find_register (plugin, name, rank, func, (char **) ext, caps, priv, notify))\
    return FALSE; \
}G_END_DECLS

static gboolean
plugin_init (GstPlugin * plugin)
{
  static const gchar *webm_exts[] = { "webm", "weba", "webv", NULL };
  TYPE_FIND_REGISTER (plugin, "video/webm", GST_RANK_PRIMARY,
      webm_type_find, webm_exts, WEBM_CAPS, NULL, NULL);
  gst_matroska_register_tags ();
  return gst_matroska_demux_plugin_init (plugin) &&
      gst_matroska_mux_plugin_init (plugin);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "matroska",
    "Matroska stream handling",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
