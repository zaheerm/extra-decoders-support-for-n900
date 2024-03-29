/* Generic video mixer plugin
 * Copyright (C) 2008 Wim Taymans <wim@fluendo.com>
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
 
#ifndef __GST_VIDEO_MIXER_H__
#define __GST_VIDEO_MIXER_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#include "videomixerpad.h"

GST_DEBUG_CATEGORY_EXTERN (gst_videomixer_debug);
#define GST_CAT_DEFAULT gst_videomixer_debug


G_BEGIN_DECLS

#define GST_TYPE_VIDEO_MIXER (gst_videomixer_get_type())
#define GST_VIDEO_MIXER(obj) \
        (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VIDEO_MIXER, GstVideoMixer))
#define GST_VIDEO_MIXER_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VIDEO_MIXER, GstVideoMixerClass))
#define GST_IS_VIDEO_MIXER(obj) \
        (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VIDEO_MIXER))
#define GST_IS_VIDEO_MIXER_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VIDEO_MIXER))

typedef struct _GstVideoMixer GstVideoMixer;
typedef struct _GstVideoMixerClass GstVideoMixerClass;

/**
 * GstVideoMixerBackground:
 * @VIDEO_MIXER_BACKGROUND_CHECKER: checker pattern background
 * @VIDEO_MIXER_BACKGROUND_BLACK: solid color black background
 * @VIDEO_MIXER_BACKGROUND_WHITE: solid color white background
 *
 * The different backgrounds videomixer can blend over.
 */
typedef enum
{
  VIDEO_MIXER_BACKGROUND_CHECKER,
  VIDEO_MIXER_BACKGROUND_BLACK,
  VIDEO_MIXER_BACKGROUND_WHITE
}
GstVideoMixerBackground;

/**
 * GstVideoMixer:
 *
 * The opaque #GstVideoMixer structure.
 */
struct _GstVideoMixer
{
  GstElement element;

  /* pad */
  GstPad *srcpad;

  /* Lock to prevent the state to change while blending */
  GMutex *state_lock;
  /* Sink pads using Collect Pads from core's base library */
  GstCollectPads *collect;
  /* sinkpads, a GSList of GstVideoMixerPads */
  GSList *sinkpads;

  gint numpads;

  GstClockTime last_ts;

  /* the master pad */
  GstVideoMixerPad *master;

  GstVideoFormat fmt;

  gint in_width, in_height;
  gint out_width, out_height;
  gboolean setcaps;
  gboolean sendseg;

  GstVideoMixerBackground background;

  gint fps_n;
  gint fps_d;
  
  /* Next available sinkpad index */
  gint next_sinkpad;

  /* sink event handling */
  GstPadEventFunction collect_event;
  guint64	segment_position;
  gdouble	segment_rate;

  void (*blend) (guint8 * src, gint xpos, gint ypos, gint src_width, gint src_height, gdouble src_alpha,
                 guint8 * dest, gint dest_width, gint dest_height);
  void (*fill_checker) (guint8 * dest, gint width, gint height);

  void (*fill_color) (guint8 * dest, gint width, gint height, gint colY, gint colU, gint colV);
};

struct _GstVideoMixerClass
{
  GstElementClass parent_class;
};

G_END_DECLS
#endif /* __GST_VIDEO_MIXER_H__ */
