/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
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


#ifndef __GST_MEDIAN_H__
#define __GST_MEDIAN_H__


#include <gst/gst.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GST_TYPE_MEDIAN \
  (gst_median_get_type())
#define GST_MEDIAN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MEDIAN,GstMedian))
#define GST_MEDIAN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MEDIAN,GstMedianClass))
#define GST_IS_MEDIAN(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MEDIAN))
#define GST_IS_MEDIAN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MEDIAN))

typedef struct _GstMedian GstMedian;
typedef struct _GstMedianClass GstMedianClass;

struct _GstMedian {
  GstElement element;

  int format;
  int width;
  int height;

  int filtersize;

  gboolean active;
  gboolean lum_only;

  GstPad *sinkpad,*srcpad;
};

struct _GstMedianClass {
  GstElementClass parent_class;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GST_MEDIAN_H__ */
