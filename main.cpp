#include<stdio.h>
#include <gst/gst.h>


typedef struct
{
    GstElement *pipeline;
    GstElement *source;
    GstElement *demux;
    GstElement *audio_queue;
    GstElement *audio_rtppay;
    GstElement *rtpbin;
    GstElement *audio_sink;
} AppData;



static void
qtdemux_pad_added_cb (GstElement * element, GstPad * new_pad, gpointer app)
{
    AppData* appData = (AppData *)app;
    GstPad *sinkpad, *srcpad;

    g_print ("new payload on pad: %s\n", GST_PAD_NAME (new_pad));

    if (strcmp(GST_PAD_NAME(new_pad), "audio_0") == 0)
    {
        g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        sinkpad = gst_element_get_static_pad(appData->audio_queue, "sink");
        g_assert(sinkpad);

        GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(sinkpad);
    }
    else
    {
        g_print("else ... add pads..: %s\n", GST_PAD_NAME(new_pad));
    }

}

int main (int argc, char *argv[]){
    gboolean silent = FALSE;
    gint session_count = 1;
    gchar *savefile = NULL;
    gchar *filepath = NULL;
    GOptionContext *ctx;
    GError *err = NULL;
    GOptionEntry entries[] = {
        {"file-path", 'f', 0, G_OPTION_ARG_STRING, &filepath, "mp4 file path(audio and video is streamed)", "/path/your/file.mp4"},
        {"session-count", 'c', 0, G_OPTION_ARG_INT, &session_count, "rtp stream count", "0~max"},
        {NULL}};

    ctx = g_option_context_new("- Your application");
    g_option_context_add_main_entries(ctx, entries, NULL);
    g_option_context_add_group(ctx, gst_init_get_option_group());
    if (!g_option_context_parse(ctx, &argc, &argv, &err))
    {
        g_print("Failed to initialize: %s\n", err->message);
        g_clear_error(&err);
        g_option_context_free(ctx);
        return 1;
    }
    
    g_option_context_free (ctx);

    g_print("file-path : %s\n", filepath);
    g_print("session-count : %d\n", session_count);

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    // "rtpbin name=rtpbin filesrc location=/home/ysh8361/Videos/SampleVideo_1280x720_5mb.mp4 ! qtdemux name=demux  demux.audio_0 ! queue ! rtpmp4gpay! rtpbin.send_rtp_sink_0 rtpbin.send_rtp_src_0 ! udpsink host=192.168.0.30 port=5002 demux.video_0 ! queue ! h264parse ! rtph264pay ! rtpbin.send_rtp_sink_1 rtpbin.send_rtp_src_1 ! udpsink host=192.168.0.30 port=5000"

    AppData *app = (AppData *)malloc(sizeof(AppData));
    app->pipeline = gst_pipeline_new(NULL);
    app->source = gst_element_factory_make("filesrc", "src");
    app->demux = gst_element_factory_make("qtdemux", "demux");
    app->audio_queue = gst_element_factory_make("queue", "audio_queue");
    app->audio_rtppay = gst_element_factory_make("rtpmp4gpay", "audio_rtppay");
    app->rtpbin = gst_element_factory_make("rtpbin", "rtpbin");
    app->audio_sink = gst_element_factory_make("udpsink", "audio_sink");
    GstPad *sinkpad, *srcpad;
    if (!app->pipeline || !app->source || !app->demux | !app->audio_rtppay || !app->audio_rtppay || !app->rtpbin || !app->audio_sink)
    {
        throw "Not all elements could be created.\n";
    }

    gst_bin_add_many(GST_BIN(app->pipeline), app->source, app->demux, app->audio_queue, app->audio_rtppay , app->rtpbin, app->audio_sink, NULL);


    g_object_set(app->source, "location", "/home/jdin/SampleVideo_1280x720_5mb.mp4", NULL);
    g_object_set(app->audio_sink, "host", "192.168.0.30", NULL);
    g_object_set(app->audio_sink, "port", 5002, NULL);


    if (!gst_element_link_many(app->source, app->demux, NULL))
    {
        g_error("Failed to link audiosrc, audioconv, audioresample, "
                "audio encoder and audio payloader");
    }


    g_signal_connect(app->demux, "pad-added", (GCallback)qtdemux_pad_added_cb, app);

        if (!gst_element_link_many(app->audio_queue, app->audio_rtppay, NULL))
    {
        g_error("Failed to link audiosrc, audioconv, audioresample, "
                "audio encoder and audio payloader");
    }



    sinkpad = gst_element_get_request_pad(app->rtpbin, "send_rtp_sink_0");
    srcpad = gst_element_get_static_pad(app->audio_rtppay, "src");
    if (gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK)
    {
        // g_error ("Failed to link audio payloader to rtpbin");
        throw "Failed to link audio payloader to rtpbin";
    }
    // gst_object_unref(sinkpad);
    // gst_object_unref(srcpad);

    srcpad = gst_element_get_static_pad(app->rtpbin, "send_rtp_src_0");
    sinkpad = gst_element_get_static_pad(app->audio_sink, "sink");
    if (gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK)
    {
        g_print("rtpbin:%p, video_sink:%p, send_rtp_src_0:%p, video_sink:%p\n", app->rtpbin, app->audio_sink, srcpad, sinkpad);
        //g_error ("Failed to link rtpbin to rtpsink");
        throw "Failed to link rtpbin to rtpsink";
    }

    // gst_object_unref(srcpad);
    // gst_object_unref(sinkpad);

    /* set the pipeline to playing */
    g_print("starting sender pipeline\n");
    gst_element_set_state(app->pipeline, GST_STATE_PLAYING);

    /* we need to run a GLib main loop to get the messages */
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);


    return 0;
}

