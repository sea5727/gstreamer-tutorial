#include <glib.h>
#include <gst/gst.h>
#include <stdio.h>

// audiomixer name=mix ! opusenc ! rtpopuspay pt=96 ! udpsink host=192.168.0.4 port=3000 
// audiotestsrc wave=8 ! audioconvert ! mix. 
// audiotestsrc wave=3 ! audioconvert ! mix.
int main(int argc, char *argv[]) {

    gst_init (&argc, &argv);

    auto pipeline = gst_pipeline_new(NULL);

    auto mix = gst_element_factory_make("audiomixer", "mix");
    auto opusenc = gst_element_factory_make("opusenc", "opusenc");
    auto rtpopuspay = gst_element_factory_make("rtpopuspay", "rtpopuspay");
    auto udpsink = gst_element_factory_make("udpsink", "udpsink");
    
    g_object_set(rtpopuspay, "pt", 96, NULL);
    g_object_set(udpsink, "host", "192.168.0.4", "port", 3000, NULL);
    
    gst_bin_add_many(GST_BIN(pipeline), mix, opusenc, rtpopuspay, udpsink, NULL);
    gst_element_link_many(mix, opusenc, rtpopuspay, udpsink, NULL);

    auto audiotestsrc0 = gst_element_factory_make("audiotestsrc", "audiotestsrc0");
    auto audioconvert0 = gst_element_factory_make("audioconvert", "audioconvert0");
    g_object_set(audiotestsrc0, "wave", 8, NULL);
    gst_bin_add_many(GST_BIN(pipeline), audiotestsrc0, audioconvert0, NULL);
    gst_element_link_many(audiotestsrc0, audioconvert0, NULL);

    auto audiotestsrc1 = gst_element_factory_make("audiotestsrc", "audiotestsrc1");
    auto audioconvert1 = gst_element_factory_make("audioconvert", "audioconvert1");
    g_object_set(audiotestsrc1, "wave", 3, NULL);
    gst_bin_add_many(GST_BIN(pipeline), audiotestsrc1, audioconvert1, NULL);
    gst_element_link_many(audiotestsrc1, audioconvert1, NULL);

    auto mix_sink_pad0 = gst_element_get_request_pad(mix, "sink_%u");
    auto cnvt_src_pad0 = gst_element_get_static_pad(audioconvert0, "src");
    if(gst_pad_link (cnvt_src_pad0, mix_sink_pad0) != GST_PAD_LINK_OK ){
        g_printerr("gst_pad_link (mix_sink_pad, cnvt_src_pad)\n");
        return 0;
    }
    auto mix_sink_pad1 = gst_element_get_request_pad(mix, "sink_%u");
    auto cnvt_src_pad1 = gst_element_get_static_pad(audioconvert1, "src");
    if(gst_pad_link (cnvt_src_pad1, mix_sink_pad1) != GST_PAD_LINK_OK ){
        g_printerr("gst_pad_link (mix_sink_pad, cnvt_src_pad)\n");
        return 0;
    }


    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* we need to run a GLib main loop to get the messages */
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

	return 0;
}
 