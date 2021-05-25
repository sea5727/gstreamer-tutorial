#include<stdio.h>
#include <gst/gst.h>


typedef struct
{
    GstElement *pipeline;

    GstBin *audiobin;
    GstBin *videobin;

    GstElement *rtpbin;
    
    GstElement *source;
    GstElement *demux;


    GstElement *audio_queue;
    GstElement *audio_rtppay;
    GstElement* audio_rtp_sink;
    GstElement* audio_rtcp_src;
    GstElement* audio_rtcp_sink;
    

    GstElement *video_parse;
    GstElement *video_rtppay;
    GstElement* video_rtp_sink;
    GstElement* video_rtcp_src;
    GstElement* video_rtcp_sink;
    
    
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
        sinkpad = gst_element_get_static_pad(GST_ELEMENT(appData->audiobin), "sink");
        g_assert(sinkpad);

        GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(sinkpad);
    }
    else
    {
        // g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        // sinkpad = gst_element_get_static_pad(GST_ELEMENT(appData->videobin), "sink");
        // g_assert(sinkpad);

        // GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        // g_assert(lres == GST_PAD_LINK_OK);
        // gst_object_unref(sinkpad);
    }

}

// static void
// cb_newpad (GstElement *decodebin, GstPad *pad, gpointer data)
// {
//     AppData *appData = (AppData *)data;
//   GstCaps *caps;
//   GstStructure *str;
//   GstPad *audiopad;

//   g_print("cb_newpad\n");
//   /* only link once */
//   audiopad = gst_element_get_static_pad (appData->audioconvert, "sink");
//   if (GST_PAD_IS_LINKED (audiopad)) {
//     g_object_unref (audiopad);
//     return;
//   }

//   /* check media type */
//   caps = gst_pad_query_caps (pad, NULL);
//   str = gst_caps_get_structure (caps, 0);
//   g_print("cb_newpad name:%s\n", gst_structure_get_name (str));
//   if (!g_strrstr (gst_structure_get_name (str), "audio/x-raw")) {
//       g_print("!cb_newpad name:%s\n", gst_structure_get_name (str));
//     gst_caps_unref (caps);
//     gst_object_unref (audiopad);
//     return;
//   }
//   g_print("!link!! %s\n", gst_structure_get_name (str));
//   gst_caps_unref (caps);

//   /* link'n'play */
//   gst_pad_link (pad, audiopad);

//   g_object_unref (audiopad);
// }


int main (int argc, char *argv[]){
    // gboolean silent = FALSE;
    // gint session_count = 1;
    // gchar *savefile = NULL;
    // gchar *filepath = NULL;
    // GOptionContext *ctx;
    // GError *err = NULL;
    // GOptionEntry entries[] = {
    //     {"file-path", 'f', 0, G_OPTION_ARG_STRING, &filepath, "mp4 file path(audio and video is streamed)", "/path/your/file.mp4"},
    //     {"session-count", 'c', 0, G_OPTION_ARG_INT, &session_count, "rtp stream count", "0~max"},
    //     {NULL}};

    // ctx = g_option_context_new("- Your application");
    // g_option_context_add_main_entries(ctx, entries, NULL);
    // g_option_context_add_group(ctx, gst_init_get_option_group());
    // if (!g_option_context_parse(ctx, &argc, &argv, &err))
    // {
    //     g_print("Failed to initialize: %s\n", err->message);
    //     g_clear_error(&err);
    //     g_option_context_free(ctx);
    //     return 1;
    // }
    
    // g_option_context_free (ctx);

    // g_print("file-path : %s\n", filepath);
    // g_print("session-count : %d\n", session_count);

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    // "rtpbin name=rtpbin filesrc location=/home/ysh8361/Videos/SampleVideo_1280x720_5mb.mp4 ! qtdemux name=demux  demux.audio_0 ! queue ! rtpmp4gpay! rtpbin.send_rtp_sink_0 rtpbin.send_rtp_src_0 ! udpsink host=192.168.0.30 port=5002 demux.video_0 ! queue ! h264parse ! rtph264pay ! rtpbin.send_rtp_sink_1 rtpbin.send_rtp_src_1 ! udpsink host=192.168.0.30 port=5000"

    AppData *app = (AppData *)malloc(sizeof(AppData));
    app->pipeline = gst_pipeline_new("my_pipeline");
    app->audiobin = GST_BIN (gst_bin_new ("my_audio_bin"));
    app->videobin = GST_BIN (gst_bin_new ("my_vedio_bin"));
    
    app->rtpbin = gst_element_factory_make("rtpbin", "rtpbin");
    app->source = gst_element_factory_make("filesrc", "src");
    app->demux = gst_element_factory_make("qtdemux", "demux");
    app->audio_queue = gst_element_factory_make("queue", "audio_queue");
    app->audio_rtppay = gst_element_factory_make("rtpmp4gpay", "audio_rtppay");
    app->audio_rtp_sink = gst_element_factory_make("udpsink", "audio_rtp_sink");
    app->audio_rtcp_src = gst_element_factory_make("udpsrc", "audio_rtcp_src");
    app->audio_rtcp_sink = gst_element_factory_make("udpsink", "audio_rtcp_sink");

    g_object_set (app->source, "location", "/home/ysh8361/Videos/SampleVideo_1280x720_5mb.mp4", NULL);
    g_object_set (app->audio_rtp_sink, "host", "192.168.0.30", NULL);
    g_object_set (app->audio_rtp_sink, "port", 5002, NULL);
    g_object_set (app->audio_rtcp_src, "port", 5002 + 5, NULL);
    g_object_set (app->audio_rtcp_sink, "host", "192.168.0.30", NULL);
    g_object_set (app->audio_rtcp_sink, "port", 5002 + 1, NULL);
  
    app->video_parse = gst_element_factory_make("h264parse", "video_parse");
    app->video_rtppay = gst_element_factory_make("rtph264pay", "video_rtppay");
    app->video_rtp_sink = gst_element_factory_make("udpsink", "video_rtp_sink");
    app->video_rtcp_src = gst_element_factory_make("udpsrc", "video_rtcp_src");
    app->video_rtcp_sink = gst_element_factory_make("udpsink", "video_rtcp_sink");
    g_object_set (app->video_rtp_sink, "host", "192.168.0.204", NULL);
    g_object_set (app->video_rtp_sink, "port", 5000, NULL);
    g_object_set (app->video_rtcp_src, "port", 5000 + 5, NULL);
    g_object_set (app->video_rtcp_sink, "host", "192.168.0.30", NULL);
    g_object_set (app->video_rtcp_sink, "port", 5000 + 1, NULL);

    GstPad *sinkpad, *srcpad;

    gst_bin_add_many(GST_BIN(app->pipeline), app->rtpbin, app->source, app->demux, NULL);

    gst_bin_add_many(
        app->audiobin, 
        app->audio_queue,
        app->audio_rtppay,
        NULL);
    gst_bin_add_many(
        app->videobin, 
        app->video_parse,
        app->video_rtppay,
        NULL);




    if (!gst_element_link_many(app->source, app->demux, NULL)) {
        g_error("gst_element_link_many(app->source, app->demux) fail\n");            
    }  

    g_signal_connect (app->demux, "pad-added", G_CALLBACK (qtdemux_pad_added_cb), app);


    if(!gst_element_link_many(
        app->audio_queue,
        app->audio_rtppay,
        NULL)){
        g_error("gst_element_link_many(app->source, app->demux) etc fail\n");
    }

    GstPad *srcPad = gst_element_get_static_pad (app->audio_queue, "sink");
    GstPad *binPad = gst_ghost_pad_new ("sink", srcPad);
    gst_element_add_pad (GST_ELEMENT (app->audiobin), binPad);

    srcPad = gst_element_get_static_pad (app->audio_rtppay, "src");
    binPad = gst_ghost_pad_new ("src", srcPad);
    gst_element_add_pad (GST_ELEMENT (app->audiobin), binPad);


    if(!gst_element_link_many(
        app->video_parse,
        app->video_rtppay,
        NULL)){
        g_error("gst_element_link_many(app->source, app->demux) etc fail\n");
    }

    srcPad = gst_element_get_static_pad (app->video_parse, "sink");
    binPad = gst_ghost_pad_new ("sink", srcPad);
    gst_element_add_pad (GST_ELEMENT (app->videobin), binPad);

    srcPad = gst_element_get_static_pad (app->video_rtppay, "src");
    binPad = gst_ghost_pad_new ("src", srcPad);
    gst_element_add_pad (GST_ELEMENT (app->videobin), binPad);


    gst_bin_add_many(GST_BIN(app->pipeline), 
        app->audio_rtp_sink, 
        app->audio_rtcp_sink, 
        app->audio_rtcp_src, 
        GST_ELEMENT (app->audiobin), NULL);



      /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
    sinkpad = gst_element_get_request_pad (app->rtpbin, "send_rtp_sink_0");
    srcpad = gst_element_get_static_pad (GST_ELEMENT (app->audiobin), "src");
    if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
        g_error ("Failed to link audio payloader to rtpbin!!");
    gst_object_unref (srcpad);




    /* get the RTP srcpad that was created when we requested the sinkpad above and
    * link it to the rtpsink sinkpad*/
    srcpad = gst_element_get_static_pad (app->rtpbin, "send_rtp_src_0");
    sinkpad = gst_element_get_static_pad (app->audio_rtp_sink, "sink");
    if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
        g_error ("send_rtp_src_0 Failed to link rtpbin to rtpsink???");
    gst_object_unref (srcpad);
    gst_object_unref (sinkpad);


     /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
    sinkpad = gst_element_get_request_pad (app->rtpbin, "recv_rtcp_sink_0");
    srcpad = gst_element_get_static_pad (GST_ELEMENT (app->audio_rtcp_src), "src");
    if(!sinkpad) g_error("sinkpad\n");
    if(!srcpad) g_error("srcpad\n");
    if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
        g_error ("send_rtcp_sink_0 Failed to link audio payloader to rtpbin");
    gst_object_unref (srcpad);

    /* get the RTP srcpad that was created when we requested the sinkpad above and
    * link it to the rtpsink sinkpad*/
    srcpad = gst_element_get_request_pad (app->rtpbin, "send_rtcp_src_0");
    sinkpad = gst_element_get_static_pad (app->audio_rtcp_sink, "sink");
    if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
        g_error ("send_rtcp_src_0 Failed to link rtpbin to rtpsink");
    gst_object_unref (srcpad);
    gst_object_unref (sinkpad);


    // gst_bin_add_many(GST_BIN(app->pipeline), 
    //     app->video_rtp_sink, 
    //     app->video_rtcp_sink, 
    //     app->video_rtcp_src, 
    //     GST_ELEMENT (app->videobin), NULL);



    //   /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
    // sinkpad = gst_element_get_request_pad (app->rtpbin, "send_rtp_sink_1");
    // srcpad = gst_element_get_static_pad (GST_ELEMENT (app->videobin), "src");
    // if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
    //     g_error ("Failed to link video payloader to rtpbin!!");
    // gst_object_unref (srcpad);




    // /* get the RTP srcpad that was created when we requested the sinkpad above and
    // * link it to the rtpsink sinkpad*/
    // srcpad = gst_element_get_static_pad (app->rtpbin, "send_rtp_src_1");
    // sinkpad = gst_element_get_static_pad (app->video_rtp_sink, "sink");
    // if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
    //     g_error ("send_rtp_src_1 Failed to link rtpbin to rtpsink???");
    // gst_object_unref (srcpad);
    // gst_object_unref (sinkpad);


    //  /* now link all to the rtpbin, start by getting an RTP sinkpad for session 0 */
    // sinkpad = gst_element_get_request_pad (app->rtpbin, "recv_rtcp_sink_1");
    // srcpad = gst_element_get_static_pad (GST_ELEMENT (app->video_rtcp_src), "src");
    // if(!sinkpad) g_error("sinkpad\n");
    // if(!srcpad) g_error("srcpad\n");
    // if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
    //     g_error ("send_rtcp_sink_0 Failed to link video payloader to rtpbin");
    // gst_object_unref (srcpad);

    // /* get the RTP srcpad that was created when we requested the sinkpad above and
    // * link it to the rtpsink sinkpad*/
    // srcpad = gst_element_get_request_pad (app->rtpbin, "send_rtcp_src_1");
    // sinkpad = gst_element_get_static_pad (app->video_rtcp_sink, "sink");
    // if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
    //     g_error ("send_rtcp_src_0 Failed to link rtpbin to rtpsink");
    // gst_object_unref (srcpad);
    // gst_object_unref (sinkpad);



    g_print("starting sender pipeline\n");
    gst_element_set_state(app->pipeline, GST_STATE_PLAYING);

    /* we need to run a GLib main loop to get the messages */
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

    
    return 0;
}

