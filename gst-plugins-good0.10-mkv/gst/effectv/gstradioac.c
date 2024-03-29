/* GStreamer
 * Cradioacyright (C) <2009> Sebastian Dröge <sebastian.droege@collabora.co.uk>
 *
 * EffecTV - Realtime Digital Video Effector
 * Cradioacyright (C) 2001-2006 FUKUCHI Kentaro
 *
 * RadioacTV - motion-enlightment effect.
 * Cradioacyright (C) 2001-2002 FUKUCHI Kentaro
 *
 * EffecTV is free software. This library is free software;
 * you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your radioaction) any later version.
 *
 * This library is distributed in the hradioace that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a cradioacy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-radioactv
 *
 * RadioacTV does *NOT* detect a radioactivity. It detects a difference
 * from previous frame and blurs it.
 * 
 * RadioacTV has 4 mode, normal, strobe1, strobe2 and trigger.
 * In trigger mode, effect appears only when the trigger property is %TRUE.
 *
 * strobe1 and strobe2 mode drops some frames. strobe1 mode uses the difference between
 * current frame and previous frame dropped, while strobe2 mode uses the difference from
 * previous frame displayed. The effect of strobe2 is stronger than strobe1.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v videotestsrc ! radioactv ! ffmpegcolorspace ! autovideosink
 * ]| This pipeline shows the effect of radioactv on a test stream.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include <string.h>

#include "gstradioac.h"
#include "gsteffectv.h"

#include <gst/video/video.h>
#include <gst/controller/gstcontroller.h>

enum
{
  RADIOAC_NORMAL = 0,
  RADIOAC_STROBE,
  RADIOAC_STROBE2,
  RADIOAC_TRIGGER
};

enum
{
  COLOR_RED = 0,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_WHITE
};

#define GST_TYPE_RADIOACTV_MODE (gst_radioactv_mode_get_type())
static GType
gst_radioactv_mode_get_type (void)
{
  static GType type = 0;

  static const GEnumValue enumvalue[] = {
    {RADIOAC_NORMAL, "Normal", "normal"},
    {RADIOAC_STROBE, "Strobe 1", "strobe1"},
    {RADIOAC_STROBE2, "Strobe 2", "strobe2"},
    {RADIOAC_TRIGGER, "Trigger", "trigger"},
    {0, NULL, NULL},
  };

  if (!type) {
    type = g_enum_register_static ("GstRadioacTVMode", enumvalue);
  }
  return type;
}

#define GST_TYPE_RADIOACTV_COLOR (gst_radioactv_color_get_type())
static GType
gst_radioactv_color_get_type (void)
{
  static GType type = 0;

  static const GEnumValue enumvalue[] = {
    {COLOR_RED, "Red", "red"},
    {COLOR_GREEN, "Green", "green"},
    {COLOR_BLUE, "Blue", "blue"},
    {COLOR_WHITE, "White", "white"},
    {0, NULL, NULL},
  };

  if (!type) {
    type = g_enum_register_static ("GstRadioacTVColor", enumvalue);
  }
  return type;
}

#define DEFAULT_MODE RADIOAC_NORMAL
#define DEFAULT_COLOR COLOR_WHITE
#define DEFAULT_INTERVAL 3
#define DEFAULT_TRIGGER FALSE

enum
{
  PROP_0,
  PROP_MODE,
  PROP_COLOR,
  PROP_INTERVAL,
  PROP_TRIGGER
};

#define COLORS 32
#define PATTERN 4
#define MAGIC_THRESHOLD 40
#define RATIO 0.95

static guint32 palettes[COLORS * PATTERN];

GST_BOILERPLATE (GstRadioacTV, gst_radioactv, GstVideoFilter,
    GST_TYPE_VIDEO_FILTER);

static GstStaticPadTemplate gst_radioactv_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
    GST_STATIC_CAPS (GST_VIDEO_CAPS_RGBx)
#else
    GST_STATIC_CAPS (GST_VIDEO_CAPS_xBGR)
#endif
    );

static GstStaticPadTemplate gst_radioactv_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
    GST_STATIC_CAPS (GST_VIDEO_CAPS_RGBx)
#else
    GST_STATIC_CAPS (GST_VIDEO_CAPS_xBGR)
#endif
    );

static void
makePalette (void)
{
  gint i;

#define DELTA (255/(COLORS/2-1))

  for (i = 0; i < COLORS / 2; i++) {
    palettes[i] = i * DELTA;
    palettes[COLORS + i] = (i * DELTA) << 8;
    palettes[COLORS * 2 + i] = (i * DELTA) << 16;
  }
  for (i = 0; i < COLORS / 2; i++) {
    palettes[+i + COLORS / 2] = 255 | (i * DELTA) << 16 | (i * DELTA) << 8;
    palettes[COLORS + i + COLORS / 2] =
        (255 << 8) | (i * DELTA) << 16 | i * DELTA;
    palettes[COLORS * 2 + i + COLORS / 2] =
        (255 << 16) | (i * DELTA) << 8 | i * DELTA;
  }
  for (i = 0; i < COLORS; i++) {
    palettes[COLORS * 3 + i] = (255 * i / COLORS) * 0x10101;
  }
  for (i = 0; i < COLORS * PATTERN; i++) {
    palettes[i] = palettes[i] & 0xfefeff;
  }
#undef DELTA
}

#define VIDEO_HWIDTH (filter->buf_width/2)
#define VIDEO_HHEIGHT (filter->buf_height/2)

/* this table assumes that video_width is times of 32 */
static void
setTable (GstRadioacTV * filter)
{
  guint bits;
  gint x, y, tx, ty, xx;
  gint ptr, prevptr;

  prevptr = (gint) (0.5 + RATIO * (-VIDEO_HWIDTH) + VIDEO_HWIDTH);
  for (xx = 0; xx < (filter->buf_width_blocks); xx++) {
    bits = 0;
    for (x = 0; x < 32; x++) {
      ptr = (gint) (0.5 + RATIO * (xx * 32 + x - VIDEO_HWIDTH) + VIDEO_HWIDTH);
      bits = bits >> 1;
      if (ptr != prevptr)
        bits |= 0x80000000;
      prevptr = ptr;
    }
    filter->blurzoomx[xx] = bits;
  }

  ty = (gint) (0.5 + RATIO * (-VIDEO_HHEIGHT) + VIDEO_HHEIGHT);
  tx = (gint) (0.5 + RATIO * (-VIDEO_HWIDTH) + VIDEO_HWIDTH);
  xx = (gint) (0.5 + RATIO * (filter->buf_width - 1 - VIDEO_HWIDTH) +
      VIDEO_HWIDTH);
  filter->blurzoomy[0] = ty * filter->buf_width + tx;
  prevptr = ty * filter->buf_width + xx;
  for (y = 1; y < filter->buf_height; y++) {
    ty = (gint) (0.5 + RATIO * (y - VIDEO_HHEIGHT) + VIDEO_HHEIGHT);
    filter->blurzoomy[y] = ty * filter->buf_width + tx - prevptr;
    prevptr = ty * filter->buf_width + xx;
  }
}

#undef VIDEO_HWIDTH
#undef VIDEO_HHEIGHT

static void
blur (GstRadioacTV * filter)
{
  gint x, y;
  gint width;
  guint8 *p, *q;
  guint8 v;

  width = filter->buf_width;
  p = filter->blurzoombuf + filter->width + 1;
  q = p + filter->buf_area;

  for (y = filter->buf_height - 2; y > 0; y--) {
    for (x = width - 2; x > 0; x--) {
      v = (*(p - width) + *(p - 1) + *(p + 1) + *(p + width)) / 4 - 1;
      if (v == 255)
        v = 0;
      *q = v;
      p++;
      q++;
    }
    p += 2;
    q += 2;
  }
}

static void
zoom (GstRadioacTV * filter)
{
  gint b, x, y;
  guint8 *p, *q;
  gint blocks, height;
  gint dx;

  p = filter->blurzoombuf + filter->buf_area;
  q = filter->blurzoombuf;
  height = filter->buf_height;
  blocks = filter->buf_width_blocks;

  for (y = 0; y < height; y++) {
    p += filter->blurzoomy[y];
    for (b = 0; b < blocks; b++) {
      dx = filter->blurzoomx[b];
      for (x = 0; x < 32; x++) {
        p += (dx & 1);
        *q++ = *p;
        dx = dx >> 1;
      }
    }
  }
}

static void
blurzoomcore (GstRadioacTV * filter)
{
  blur (filter);
  zoom (filter);
}

/* Background image is refreshed every frame */
static void
image_bgsubtract_update_y (guint32 * src, guint32 * background, guint8 * diff,
    gint video_area, gint y_threshold)
{
  gint i;
  gint R, G, B;
  guint32 *p;
  gint16 *q;
  guint8 *r;
  gint v;

  p = src;
  q = (gint16 *) background;
  r = diff;
  for (i = 0; i < video_area; i++) {
    R = ((*p) & 0xff0000) >> (16 - 1);
    G = ((*p) & 0xff00) >> (8 - 2);
    B = (*p) & 0xff;
    v = (R + G + B) - (gint) (*q);
    *q = (gint16) (R + G + B);
    *r = ((v + y_threshold) >> 24) | ((y_threshold - v) >> 24);

    p++;
    q++;
    r++;
  }
}

static GstFlowReturn
gst_radioactv_transform (GstBaseTransform * trans, GstBuffer * in,
    GstBuffer * out)
{
  GstRadioacTV *filter = GST_RADIOACTV (trans);
  guint32 *src, *dest;
  GstFlowReturn ret = GST_FLOW_OK;
  GstClockTime timestamp, stream_time;
  gint x, y;
  guint32 a, b;
  guint8 *diff, *p;
  guint32 *palette;

  palette = &palettes[COLORS * filter->color];

  timestamp = GST_BUFFER_TIMESTAMP (in);
  stream_time =
      gst_segment_to_stream_time (&trans->segment, GST_FORMAT_TIME, timestamp);

  GST_DEBUG_OBJECT (filter, "sync to %" GST_TIME_FORMAT,
      GST_TIME_ARGS (timestamp));

  if (GST_CLOCK_TIME_IS_VALID (stream_time))
    gst_object_sync_values (G_OBJECT (filter), stream_time);

  src = (guint32 *) GST_BUFFER_DATA (in);
  dest = (guint32 *) GST_BUFFER_DATA (out);

  diff = filter->diff;

  if (filter->mode == 3 && filter->trigger)
    filter->snaptime = 0;
  else if (filter->mode == 3 && !filter->trigger)
    filter->snaptime = 1;

  if (filter->mode != 2 || filter->snaptime <= 0) {
    image_bgsubtract_update_y (src, filter->background, diff,
        filter->width * filter->height, MAGIC_THRESHOLD * 7);
    if (filter->mode == 0 || filter->snaptime <= 0) {
      diff += filter->buf_margin_left;
      p = filter->blurzoombuf;
      for (y = 0; y < filter->buf_height; y++) {
        for (x = 0; x < filter->buf_width; x++) {
          p[x] |= diff[x] >> 3;
        }
        diff += filter->width;
        p += filter->buf_width;
      }
      if (filter->mode == 1 || filter->mode == 2) {
        memcpy (filter->snapframe, src, filter->width * filter->height * 4);
      }
    }
  }
  blurzoomcore (filter);

  if (filter->mode == 1 || filter->mode == 2) {
    src = filter->snapframe;
  }
  p = filter->blurzoombuf;
  for (y = 0; y < filter->height; y++) {
    for (x = 0; x < filter->buf_margin_left; x++) {
      *dest++ = *src++;
    }
    for (x = 0; x < filter->buf_width; x++) {
      a = *src++ & 0xfefeff;
      b = palette[*p++];
      a += b;
      b = a & 0x1010100;
      *dest++ = a | (b - (b >> 8));
    }
    for (x = 0; x < filter->buf_margin_right; x++) {
      *dest++ = *src++;
    }
  }

  if (filter->mode == 1 || filter->mode == 2) {
    filter->snaptime--;
    if (filter->snaptime < 0) {
      filter->snaptime = filter->interval;
    }
  }

  return ret;
}

static gboolean
gst_radioactv_set_caps (GstBaseTransform * btrans, GstCaps * incaps,
    GstCaps * outcaps)
{
  GstRadioacTV *filter = GST_RADIOACTV (btrans);
  GstStructure *structure;
  gboolean ret = FALSE;

  structure = gst_caps_get_structure (incaps, 0);

  if (gst_structure_get_int (structure, "width", &filter->width) &&
      gst_structure_get_int (structure, "height", &filter->height)) {
    filter->buf_width_blocks = filter->width / 32;
    if (filter->buf_width_blocks > 255)
      return FALSE;

    filter->buf_width = filter->buf_width_blocks * 32;
    filter->buf_height = filter->height;
    filter->buf_area = filter->buf_height * filter->buf_width;
    filter->buf_margin_left = (filter->width - filter->buf_width) / 2;
    filter->buf_margin_right =
        filter->height - filter->buf_width - filter->buf_margin_left;

    if (filter->blurzoombuf)
      g_free (filter->blurzoombuf);
    filter->blurzoombuf = g_new0 (guint8, filter->buf_area * 2);

    if (filter->blurzoomx)
      g_free (filter->blurzoomx);
    filter->blurzoomx = g_new0 (gint, filter->buf_width);

    if (filter->blurzoomy)
      g_free (filter->blurzoomy);
    filter->blurzoomy = g_new0 (gint, filter->buf_height);

    if (filter->snapframe)
      g_free (filter->snapframe);
    filter->snapframe = g_new (guint32, filter->width * filter->height);

    if (filter->diff)
      g_free (filter->diff);
    filter->diff = g_new (guint8, filter->width * filter->height);

    if (filter->background)
      g_free (filter->background);
    filter->background = g_new (guint32, filter->width * filter->height);

    setTable (filter);

    ret = TRUE;
  }

  return ret;
}

static gboolean
gst_radioactv_start (GstBaseTransform * trans)
{
  GstRadioacTV *filter = GST_RADIOACTV (trans);

  filter->snaptime = 0;

  return TRUE;
}

static void
gst_radioactv_finalize (GObject * object)
{
  GstRadioacTV *filter = GST_RADIOACTV (object);

  if (filter->snapframe)
    g_free (filter->snapframe);
  filter->snapframe = NULL;

  if (filter->blurzoombuf)
    g_free (filter->blurzoombuf);
  filter->blurzoombuf = NULL;

  if (filter->diff)
    g_free (filter->diff);
  filter->diff = NULL;

  if (filter->background)
    g_free (filter->background);
  filter->background = NULL;

  if (filter->blurzoomx)
    g_free (filter->blurzoomx);
  filter->blurzoomx = NULL;

  if (filter->blurzoomy)
    g_free (filter->blurzoomy);
  filter->blurzoomy = NULL;

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_radioactv_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstRadioacTV *filter = GST_RADIOACTV (object);

  switch (prop_id) {
    case PROP_MODE:
      filter->mode = g_value_get_enum (value);
      if (filter->mode == 3)
        filter->snaptime = 1;
      break;
    case PROP_COLOR:
      filter->color = g_value_get_enum (value);
      break;
    case PROP_INTERVAL:
      filter->interval = g_value_get_uint (value);
      break;
    case PROP_TRIGGER:
      filter->trigger = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_radioactv_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstRadioacTV *filter = GST_RADIOACTV (object);

  switch (prop_id) {
    case PROP_MODE:
      g_value_set_enum (value, filter->mode);
      break;
    case PROP_COLOR:
      g_value_set_enum (value, filter->color);
      break;
    case PROP_INTERVAL:
      g_value_set_uint (value, filter->interval);
      break;
    case PROP_TRIGGER:
      g_value_set_boolean (value, filter->trigger);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_radioactv_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_set_details_simple (element_class, "RadioacTV effect",
      "Filter/Effect/Video",
      "motion-enlightment effect",
      "FUKUCHI, Kentarou <fukuchi@users.sourceforge.net>, "
      "Sebastian Dröge <sebastian.droege@collabora.co.uk>");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_radioactv_sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_radioactv_src_template));
}

static void
gst_radioactv_class_init (GstRadioacTVClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstBaseTransformClass *trans_class = (GstBaseTransformClass *) klass;

  gobject_class->set_property = gst_radioactv_set_property;
  gobject_class->get_property = gst_radioactv_get_property;

  gobject_class->finalize = gst_radioactv_finalize;

  g_object_class_install_property (gobject_class, PROP_MODE,
      g_param_spec_enum ("mode", "Mode",
          "Mode", GST_TYPE_RADIOACTV_MODE, DEFAULT_MODE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_COLOR,
      g_param_spec_enum ("color", "Color",
          "Color", GST_TYPE_RADIOACTV_COLOR, DEFAULT_COLOR,
          GST_PARAM_CONTROLLABLE | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_INTERVAL,
      g_param_spec_uint ("interval", "Interval",
          "Snapshot interval (in strobe mode)", 0, G_MAXINT, DEFAULT_INTERVAL,
          GST_PARAM_CONTROLLABLE | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_TRIGGER,
      g_param_spec_boolean ("trigger", "Trigger",
          "Trigger (in trigger mode)", DEFAULT_TRIGGER,
          GST_PARAM_CONTROLLABLE | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  trans_class->set_caps = GST_DEBUG_FUNCPTR (gst_radioactv_set_caps);
  trans_class->transform = GST_DEBUG_FUNCPTR (gst_radioactv_transform);
  trans_class->start = GST_DEBUG_FUNCPTR (gst_radioactv_start);

  makePalette ();
}

static void
gst_radioactv_init (GstRadioacTV * filter, GstRadioacTVClass * klass)
{
  filter->mode = DEFAULT_MODE;
  filter->color = DEFAULT_COLOR;
  filter->interval = DEFAULT_INTERVAL;
  filter->trigger = DEFAULT_TRIGGER;

  gst_pad_use_fixed_caps (GST_BASE_TRANSFORM_SRC_PAD (filter));
  gst_pad_use_fixed_caps (GST_BASE_TRANSFORM_SINK_PAD (filter));
}
