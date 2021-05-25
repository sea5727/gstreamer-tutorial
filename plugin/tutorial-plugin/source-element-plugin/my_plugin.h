/**
 * gstreamer plugin : my_plugin
 */
#pragma once

#include "animal.h"
#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>

G_BEGIN_DECLS

#define GST_TYPE_MY_PLUGIN gst_my_plugin_get_type()
G_DECLARE_FINAL_TYPE (GstMyPlugin, gst_my_plugin, GST, MY_PLUGIN, GstBaseSrc)

struct _GstMyPlugin {
    GstBaseSrc basesrc;
    gboolean silent;
};

struct _GstMyPluginClass {
    GstBaseSrcClass parent;
};

G_END_DECLS
