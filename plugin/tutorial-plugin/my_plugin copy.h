/**
 * gstreamer plugin : my_plugin
 */
#pragma once

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_MY_PLUGIN (gst_my_plugin_get_type())

G_DECLARE_FINAL_TYPE (GstMyPlugin, gst_my_plugin, GST, MY_PLUGIN, GstElement)

struct _GstMyPlugin {
    GstElement element;
    GstPad *sinkpad, *srcpad;
    gboolean silent;
};

G_END_DECLS
