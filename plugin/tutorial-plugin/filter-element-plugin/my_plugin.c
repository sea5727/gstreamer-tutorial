#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/rtp/gstrtpbuffer.h>
#include "my_plugin.h"


G_DEFINE_TYPE (GstMyPlugin, gst_my_plugin, GST_TYPE_ELEMENT);

GST_DEBUG_CATEGORY_STATIC (gst_my_plugin_debug);

enum {
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_SILENT
};




static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

// set_property
// get_property
// sink_event
// chain

static void 
gst_my_plugin_set_property (GObject        *object,
                            guint           property_id,
                            const GValue   *value,
                            GParamSpec     *pspec);

static void
gst_my_plugin_get_property (GObject        *object,
                            guint           property_id,
                            GValue         *value,
                            GParamSpec     *pspec);


static gboolean
gst_my_plugin_sink_event (  GstPad *pad, 
                            GstObject *parent,
                            GstEvent *event);

static GstFlowReturn
gst_my_plugin_chain (   GstPad *pad, 
                        GstObject *parent,
                        GstBuffer *buffer);

static void
gst_my_plugin_class_init(GstMyPluginClass * klass) {
    GObjectClass * gobject_class = G_OBJECT_CLASS (klass);
    GstElementClass * gstelement_class = GST_ELEMENT_CLASS (klass);

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

    gst_element_class_set_details_simple (
        gstelement_class,
        "MyPlugin",
        "Classification?",
        "This is Description",
        "YangSangHo"
    );


    gst_element_class_add_pad_template(
        gstelement_class, 
        gst_static_pad_template_get(&src_factory)
    );
    
    gst_element_class_add_pad_template(
        gstelement_class,
        gst_static_pad_template_get(&sink_factory)
    );
}

static void
gst_my_plugin_init (GstMyPlugin * self) {
    self->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

    gst_pad_set_event_function (
        self->sinkpad, 
        GST_DEBUG_FUNCPTR (gst_my_plugin_sink_event)
    );

    gst_pad_set_chain_function (
        self->sinkpad,
        GST_DEBUG_FUNCPTR (gst_my_plugin_chain)
    );

    GST_PAD_SET_PROXY_CAPS (self->sinkpad);
    gst_element_add_pad(GST_ELEMENT(self), self->sinkpad);

    self->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    GST_PAD_SET_PROXY_CAPS (self->srcpad);
    gst_element_add_pad(GST_ELEMENT (self), self->srcpad);

}

// class init
// init


static void 
gst_my_plugin_set_property (
    GObject        *object,
    guint           property_id, 
    const GValue   *value,
    GParamSpec     *pspec) {
    
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
gst_my_plugin_sink_event (  
    GstPad *pad, 
    GstObject *parent,
    GstEvent *event) {

    GstCaps * caps;
    gboolean ret = FALSE;
    GstMyPlugin * self = GST_MY_PLUGIN(parent);
    GST_LOG_OBJECT (self, 
        "[TESTDEBUG][sink_event]This is Test log for debug.. %s, event:%" GST_PTR_FORMAT, 
        GST_EVENT_TYPE_NAME (event),
        event);
    
    g_print("[TESTDEBUG][sink_event] This is Test log for debug.. %s, event:%" GST_PTR_FORMAT "\n", 
        GST_EVENT_TYPE_NAME (event),
        event);
        
    switch(GST_EVENT_TYPE(event)){
        case GST_EVENT_CAPS:
            gst_event_parse_caps(event, &caps);
            // todo
            ret = gst_pad_event_default (pad, parent, event);
            break;
        case GST_EVENT_STREAM_START:
        case GST_EVENT_SEGMENT:
        default:
            ret = gst_pad_event_default(pad, parent, event);
            break;
    }

    return ret;
    
}

static GstFlowReturn
gst_my_plugin_chain (   
    GstPad *pad, 
    GstObject *parent,
    GstBuffer *buffer) {

    GstMyPlugin * self = GST_MY_PLUGIN(parent);

    if(self->silent == FALSE){
        GST_LOG_OBJECT (self, 
            "[TESTDEBUG][gst_my_plugin_chain] silent:%d, This is Test log for debug.. %s, pad:%" GST_PTR_FORMAT, 
            self->silent,
            GST_PAD_NAME(pad),
            pad);
        
        g_print("[TESTDEBUG][gst_my_plugin_chain] silent:%d, This is Test log for debug.. %s, pad:%" GST_PTR_FORMAT "\n", 
            self->silent,
            GST_PAD_NAME(pad),
            pad);
        self->silent = TRUE;
    }

    return gst_pad_push (self->srcpad, buffer);

}

static gboolean
gst_my_plugin_plugin_init (GstPlugin * plugin) {
    GST_DEBUG_CATEGORY_INIT (gst_my_plugin_debug, "myplugin", 0, "myplugin!!");

    return gst_element_register(plugin, "myplugin", GST_RANK_NONE, GST_TYPE_MY_PLUGIN);
}


#ifndef PACKAGE
#define PACKAGE "myplugin"
#endif

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myplugin,
    "This is description, my_plugin",
    gst_my_plugin_plugin_init,
    "1.16.1.0",
    "LGPL",
    "GStreamer template Plug-ins",
    "https://sea5727.github.io"
)