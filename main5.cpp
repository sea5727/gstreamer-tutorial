#include<stdio.h>
#include <gst/gst.h>


typedef struct
{
    GstElement *pipeline;
    GstElement *source;
    GstElement *demux;
    GstElement *audio_queue;
    GstElement *audio_rtppay;
    GstElement* audiosink;



    
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
    app->pipeline = gst_pipeline_new(NULL);
    app->source = gst_element_factory_make("filesrc", "src");
    app->demux = gst_element_factory_make("qtdemux", "demux");
    app->audio_queue = gst_element_factory_make("queue", "audio_queue");
    app->audio_rtppay = gst_element_factory_make("rtpmp4gpay", "audio_rtppay");
    app->audiosink = gst_element_factory_make("udpsink", "audiosink");

    g_object_set (app->source, "location", "/home/jdin/SampleVideo_1280x720_5mb.mp4", NULL);
    g_object_set (app->audiosink, "host", "192.168.0.204", NULL);
    g_object_set (app->audiosink, "port", 5002, NULL);
  
    GstPad *sinkpad, *srcpad;

    gst_bin_add_many(
        GST_BIN(app->pipeline), 
        app->source, 
        app->demux, 
        app->audio_queue,
        app->audio_rtppay,
        app->audiosink,
        NULL);


    if( !app->pipeline || 
        !app->source || 
        !app->demux || 
        !app->audio_queue ||
        !app->audio_rtppay || 
        !app->audiosink
        ){
        g_error("null");            
        }
    if (!gst_element_link_many(app->source, app->demux, NULL)) {
        g_error("gst_element_link_many(app->source, app->demux) fail\n");            
    }  
    g_signal_connect (app->demux, "pad-added", G_CALLBACK (qtdemux_pad_added_cb), app);

    if(!gst_element_link_many(
        app->audio_queue,
        app->audio_rtppay,
        app->audiosink,
        NULL)){
        g_error("gst_element_link_many(app->source, app->demux) etc fail\n");
    }
    g_print("starting sender pipeline\n");
    gst_element_set_state(app->pipeline, GST_STATE_PLAYING);

    /* we need to run a GLib main loop to get the messages */
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

    
    return 0;
}

