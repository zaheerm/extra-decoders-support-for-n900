
% includes
% prototypes

static void gst_replace_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_replace_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_replace_dispose (GObject * object);
static void gst_replace_finalize (GObject * object);

% declare-class
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
% set-methods
  gobject_class->set_property = gst_replace_set_property;
  gobject_class->get_property = gst_replace_get_property;
  gobject_class->dispose = gst_replace_dispose;
  gobject_class->finalize = gst_replace_finalize;
% methods

void
gst_replace_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstReplace *replace;

  g_return_if_fail (GST_IS_REPLACE (object));
  replace = GST_REPLACE (object);

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_replace_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstReplace *replace;

  g_return_if_fail (GST_IS_REPLACE (object));
  replace = GST_REPLACE (object);

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_replace_dispose (GObject * object)
{
  GstReplace *replace;

  g_return_if_fail (GST_IS_REPLACE (object));
  replace = GST_REPLACE (object);

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

void
gst_replace_finalize (GObject * object)
{
  GstReplace *replace;

  g_return_if_fail (GST_IS_REPLACE (object));
  replace = GST_REPLACE (object);

  /* clean up object here */

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

% end

