#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>

#include "my_plugin.h"

G_DEFINE_TYPE (GstMyPlugin, gst_my_plugin, GST_TYPE_BASE_SRC);

GST_DEBUG_CATEGORY_STATIC (gst_my_plugin_debug);

enum {
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_SILENT
};

static GstStaticPadTemplate src_factory = 
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
);



// #define GST_CAT_DEFAULT gst_fake_src_debug

// // GST_ELEMENT_REGISTER_DEFINE (fakesrc, "fakesrc", GST_RANK_NONE, GST_TYPE_FAKE_SRC);

static void
gst_my_plugin_set_property(
    GObject         *object,
    guint           property_id,
    const GValue   *value,
    GParamSpec     *pspec);

static void
gst_my_plugin_get_property(
    GObject        *object,
    guint           property_id,
    GValue         *value,
    GParamSpec     *pspec
);
static void
gst_my_plugin_finalize(
    GObject        *object
);

static gboolean gst_my_plugin_is_seekable(GstBaseSrc *src);
static gboolean gst_my_plugin_start(GstBaseSrc *src);
static gboolean gst_my_plugin_stop(GstBaseSrc *src);
static gboolean gst_my_plugin_event_handler(GstBaseSrc *src, GstEvent *event);
static void gst_my_plugin_get_times(GstBaseSrc *src, GstBuffer *buffer, GstClockTime *start, GstClockTime *end);
static GstFlowReturn gst_my_plugin_create(GstBaseSrc *src, guint64 offset, guint size, GstBuffer **buf);

static void
gst_my_plugin_class_init (GstMyPluginClass * klass) {
    GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass * gstelement_class = GST_ELEMENT_CLASS(klass);
    GstBaseSrcClass * gstbase_src_class = GST_BASE_SRC_CLASS(klass);

    gobject_class->finalize = gst_my_plugin_finalize;
    gobject_class->set_property = gst_my_plugin_set_property;
    gobject_class->get_property = gst_my_plugin_get_property;

    GParamSpec* silent = 
    g_param_spec_boolean (
        "silent", 
        "Silent", 
        "Produce verbose output ?", 
        FALSE, 
        G_PARAM_READWRITE);

    g_object_class_install_property (gobject_class, PROP_SILENT, silent);

    //factory details
    gst_element_class_set_static_metadata (
        gstelement_class,
        "Myplugin Source Element",
        "Source",
        "Test buffer",
        "Desc.."
    );

    gst_element_class_add_pad_template(
        gstelement_class, 
        gst_static_pad_template_get(&src_factory)
    );

    gstbase_src_class->is_seekable = GST_DEBUG_FUNCPTR (gst_my_plugin_is_seekable);
    gstbase_src_class->start = GST_DEBUG_FUNCPTR (gst_my_plugin_start);
    gstbase_src_class->stop = GST_DEBUG_FUNCPTR (gst_my_plugin_stop);
    gstbase_src_class->event = GST_DEBUG_FUNCPTR (gst_my_plugin_event_handler);
    gstbase_src_class->get_times = GST_DEBUG_FUNCPTR (gst_my_plugin_get_times);
    gstbase_src_class->create = GST_DEBUG_FUNCPTR (gst_my_plugin_create);
    
}


static void
gst_my_plugin_init (GstMyPlugin * self) {
    g_print("gst_my_plugin_init\n");
    self->silent = FALSE;
}

static void
gst_my_plugin_finalize(
    GObject        *object) {
    g_print("gst_my_plugin_finalize\n");
}

static void 
gst_my_plugin_set_property (
    GObject        *object,
    guint           property_id, 
    const GValue   *value,
    GParamSpec     *pspec) {
    g_print("gst_my_plugin_set_property\n");
    GstMyPlugin * self = GST_MY_PLUGIN (object);

    switch (property_id) {
        case PROP_SILENT:
            self->silent = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void
gst_my_plugin_get_property (
    GObject        *object,
    guint           property_id,
    GValue         *value,
    GParamSpec     *pspec) {
    
    // g_print("gst_my_plugin_get_property\n");

    GstMyPlugin * self = GST_MY_PLUGIN(object);

    switch(property_id){
        case PROP_SILENT:
            g_value_set_boolean(value, self->silent);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static gboolean 
gst_my_plugin_is_seekable(GstBaseSrc *src) {
    g_print("gst_my_plugin_is_seekable\n");
    return TRUE;
}

static gboolean gst_my_plugin_start(GstBaseSrc *src) {
    g_print("gst_my_plugin_start\n");
    return TRUE;
}
static gboolean gst_my_plugin_stop(GstBaseSrc *src) {
    g_print("gst_my_plugin_stop\n");
    return TRUE;
}
static gboolean gst_my_plugin_event_handler(GstBaseSrc *src, GstEvent *event) {
    g_print("gst_my_plugin_event_handler\n");
    return TRUE;
}
static void gst_my_plugin_get_times(GstBaseSrc *src, GstBuffer *buffer, GstClockTime *start, GstClockTime *end) {
    // g_print("gst_my_plugin_get_times\n");
}
static GstFlowReturn gst_my_plugin_create(GstBaseSrc *src, guint64 offset, guint size, GstBuffer **buf) {
    // g_print("gst_my_plugin_create\n");

    GstBuffer *buffer;

    buffer = gst_buffer_new ();
    GstMemory * memory = gst_allocator_alloc (NULL, 100000000 , NULL);
    gst_buffer_insert_memory (buffer, -1, memory);
    GST_BUFFER_OFFSET (buffer) = offset;
    GST_BUFFER_DTS (buffer) = GST_CLOCK_TIME_NONE;
    GST_BUFFER_PTS (buffer) = GST_CLOCK_TIME_NONE;

    *buf = buffer;

    return GST_FLOW_OK;
}


static gboolean
gst_my_plugin_plugin_init (GstPlugin * plugin) {
    GST_DEBUG_CATEGORY_INIT (gst_my_plugin_debug, "mysourceplugin", 0, "mysourceplugin!!");

    return gst_element_register(plugin, "mysourceplugin", GST_RANK_NONE, GST_TYPE_MY_PLUGIN);
}


#ifndef PACKAGE
#define PACKAGE "mysourceplugin"
#endif

// plugin detials 
GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mysourceplugin,
    "This is description, mysourceplugin",
    gst_my_plugin_plugin_init,
    "1.16.1.0",
    "LGPL",
    "GStreamer template Plug-ins",
    "https://sea5727.github.io"
)