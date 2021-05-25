#include <gst/gst.h>
#include <gst/rtp/rtp.h>
#include <unistd.h>


typedef struct _rtpbin_data
{
    GstBus * bus;
    GMainLoop *main_loop;

    GstElement * pipeline;
    GstElement * file_src;
    GstElement * qtdemux;

    GstBin *rtpbin;

    GstBin *audio_bin;
    GstElement *audio_queue;
    GstElement * rtpmp4gpay;
    GstElement * rtpsink_audio;
    GstElement * rtcpsink_audio;
    GstElement * rtcpsrc_audio;
    GstElement * identity_audio;


    GstBin *video_bin;
    GstElement *video_queue;
    GstElement *h264parse;
    GstElement *rtph264pay;
    GstElement *rtpsink_video;
    GstElement *rtcpsink_video;
    GstElement *rtcpsrc_video;
    GstElement *identity_video;


}rtpbin_data;

typedef struct _mydata
{
    GstBus * bus;
    GMainLoop *main_loop;

    GstElement * pipeline;
    GstElement * file_src;
    GstElement * qtdemux;
    GstElement * file_sink;
    
    GstElement *audio_queue;
    GstElement *rtpmp4gpay;
    GstElement *audio_udpsink;

    GstElement *video_queue;
    GstElement *rtph264pay;
    GstElement *video_udpsink;
}mydata;

int count = 0;

static gboolean
my_bus_callback(GstBus* bus, GstMessage * message, gpointer data)
{
    mydata* my = (mydata *)data;

    switch(GST_MESSAGE_TYPE(message))
    {
        case GST_MESSAGE_ERROR:
            // g_print("Got %s message.. data : %p\n", GST_MESSAGE_TYPE_NAME(message), data);
            break;
        case GST_MESSAGE_EOS:
            {
                // g_print("Got %s message.. data : %p\n", GST_MESSAGE_TYPE_NAME(message), data);
                if(count != 0) break;
                gst_element_set_state(my->pipeline, GST_STATE_NULL);
                
                // g_print("mypipeline state is null?\n" );


                count += 1 ;
                gchar *location;
                g_object_get (G_OBJECT (my->file_src), "location", &location, NULL);
                g_print("location : %s\n", location);
                g_object_set(my->file_src, "location", "/home/ysh8361/kotlin-compiler-1.3.72.zip", NULL);
                g_object_set(my->file_sink, "location", "/home/ysh8361/new.zip", NULL);

                
                gst_element_set_state(my->pipeline, GST_STATE_NULL);
                break;
            }
        case GST_MESSAGE_STATE_CHANGED:
        {
            GstState pending;
            GstState old_state;
            GstState new_state;
            
            // GstElement *element = (GstElement *)data;
            
            gst_message_parse_state_changed(message, &old_state, &new_state, &pending);           

            // g_print("Got %s message.. element : %s... old:%s, new:%s, pending:%s\n",             
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_ELEMENT_NAME(GST_ELEMENT(message->src)),
            //     gst_element_state_get_name (old_state),
            //     gst_element_state_get_name (new_state),
            //     gst_element_state_get_name (pending));
            break;
        }
        case GST_MESSAGE_STREAM_STATUS:
        {
            GstStreamStatusType type;
            GstElement *owner;
            gst_message_parse_stream_status(message, &type, &owner);
            
            // g_print("Got %s message.. element : %s.. type:%d\n",
            //     GST_MESSAGE_TYPE_NAME(message), 
            //     GST_ELEMENT_NAME(GST_ELEMENT(owner)),
            //     type);
            
            break;
        }
        case GST_MESSAGE_STREAM_START:
        {
            // g_print("Got %s message... element : %s.. \n" , 
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_ELEMENT_NAME(GST_ELEMENT(message->src)));
            break;
        }
        case GST_MESSAGE_ASYNC_DONE:
        {
            GstClockTime clock_time;
            gst_message_parse_async_done(message, &clock_time);

            // g_print("Got %s message.. %" GST_TIME_FORMAT  "\n", 
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_TIME_ARGS(clock_time));
                
            break;
        }
        case GST_MESSAGE_NEW_CLOCK:
        {
            GstClock *clock;
            gst_message_parse_new_clock(message, &clock);
            GstClockTime time;
            time = gst_clock_get_time (clock);
            // g_print("Got %s message.. time : %" GST_TIME_FORMAT "\n", 
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_TIME_ARGS (time));
            break;
        }
        case  GST_MESSAGE_TAG:
        {
            // GstTagList *taglist;
            // gst_message_parse_tag(message, &taglist);
            // g_print("Got %s message.. %d..\n", 
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_MESSAGE_TYPE(message));
            break;
        }
        case GST_MESSAGE_DURATION_CHANGED:
        {
            // g_print("Got %s message.. %d..\n", 
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_MESSAGE_TYPE(message));
            break;
        }
        default:
        {
            // g_print("Got %s message.. default error.. %d..\n", 
            //     GST_MESSAGE_TYPE_NAME(message),
            //     GST_MESSAGE_TYPE(message));
            break;
        }

    }
    return TRUE;
}
static void
qtdemux_callback2(GstElement * element, GstPad * new_pad, gpointer app)
{
    // g_print ("padded cb2.. new payload on pad: %s\n", GST_PAD_NAME (new_pad));
    rtpbin_data *my = (rtpbin_data*)app;
    // GstPad *sinkpad;
    
    if (strcmp(GST_PAD_NAME(new_pad), "audio_0") == 0)
    {
        // g_print("add pads.. audio??: %s\n", GST_PAD_NAME(new_pad));
        GstPad * audio_bin_sink = gst_element_get_static_pad(GST_ELEMENT(my->audio_bin), "sink");
        g_assert(audio_bin_sink);
        GstPadLinkReturn lres = gst_pad_link(new_pad, audio_bin_sink);
        // g_print("lres:%d\n", lres);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(audio_bin_sink);        
    }
    else
    {
        // g_print("add pads.. video??: %s\n", GST_PAD_NAME(new_pad));
        GstPad * video_bin_sink = gst_element_get_static_pad(GST_ELEMENT(my->video_bin), "sink");
        g_assert(video_bin_sink);
        GstPadLinkReturn lres = gst_pad_link(new_pad, video_bin_sink);
        // g_print("lres:%d\n", lres);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(video_bin_sink);
    }
}
static void
qtdemux_callback(GstElement * element, GstPad * new_pad, gpointer app)
{
    g_print ("padded cb3.. new payload on pad: %s\n", GST_PAD_NAME (new_pad));
    mydata *my = (mydata*)app;
    // GstPad *sinkpad;
    
    if (strcmp(GST_PAD_NAME(new_pad), "audio_0") == 0)
    {
    
        g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        
  
        GstPad* audio_queue = gst_element_get_static_pad(my->audio_queue, "sink");
        g_assert(audio_queue);


        GstPadLinkReturn lres = gst_pad_link(new_pad, audio_queue);
        g_print("lres:%d\n", lres);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(audio_queue);
    }
    else
    {
        g_print("add pads.. video??: %s\n", GST_PAD_NAME(new_pad));
        GstPad * rtph264pay_pad = gst_element_get_static_pad(my->rtph264pay, "sink");
        g_assert(rtph264pay_pad);
        GstPadLinkReturn lres = gst_pad_link(new_pad, rtph264pay_pad);
        g_print("lres:%d\n", lres);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(rtph264pay_pad);
    }
}

static int 
make_pipeline(char * name, char *host, int port)
{
    mydata *newdata = (mydata *)malloc(sizeof(mydata));
    newdata->pipeline = gst_pipeline_new(name);
    newdata->bus = gst_pipeline_get_bus(GST_PIPELINE(newdata->pipeline));
    gst_bus_add_watch(newdata->bus, my_bus_callback, newdata);
    gst_object_unref(newdata->bus);

    newdata->main_loop = g_main_loop_new(NULL, FALSE);

    newdata->file_src = gst_element_factory_make("filesrc", NULL);
    newdata->file_sink = gst_element_factory_make("filesink", NULL);
    newdata->qtdemux = gst_element_factory_make("qtdemux", NULL);
    
    newdata->audio_queue = gst_element_factory_make("queue", NULL);
    newdata->rtpmp4gpay = gst_element_factory_make("rtpmp4gpay", NULL);
    newdata->audio_udpsink = gst_element_factory_make("udpsink", "udpsink_audio");


    newdata->video_queue = gst_element_factory_make("queue", NULL);
    newdata->rtph264pay =  gst_element_factory_make("rtph264pay", NULL);
    newdata->video_udpsink = gst_element_factory_make("udpsink", "udpsink_video");

    g_object_set(newdata->audio_udpsink, "host", host, NULL);
    g_object_set(newdata->audio_udpsink, "port", port, NULL);

    // gboolean sync;
    // g_object_set(newdata->audio_udpsink, "sync", FALSE, NULL);
    // g_object_get(newdata->audio_udpsink, "sync", &sync, NULL);
    // g_print("sync is : %d\n", sync);


    // gboolean async;
    // g_object_set(newdata->audio_udpsink, "async", FALSE, NULL);
    // g_object_get(newdata->audio_udpsink, "async", &async, NULL);
    // g_print("async is : %d\n", async);

    g_object_set(newdata->rtpmp4gpay, "pt", 98, NULL);

    g_object_set(newdata->file_src, "location", "/home/ysh8361/SampleVideo_1280x720_5mb.mp4", NULL);
    g_object_set(newdata->file_sink, "location", "/home/ysh8361/new.mp4", NULL);

    gchar *filesrcname;
    g_object_get (G_OBJECT (newdata->file_src), "name", &filesrcname, NULL);
    g_print("filesrcname : %s\n", filesrcname);

    gchar *get_path;
    g_object_get (G_OBJECT (newdata->file_src), "location", &get_path, NULL);
    g_print("path : %s\n", get_path);

    
    gst_bin_add_many (GST_BIN (newdata->pipeline), 
                      newdata->file_src, 
                      newdata->qtdemux, 
                      newdata->audio_queue, 
                      newdata->rtpmp4gpay, 
                      newdata->audio_udpsink,
                      newdata->video_queue,
                      newdata->rtph264pay,
                      newdata->video_udpsink,
                      NULL);

    if (!gst_element_link_many (newdata->file_src, newdata->qtdemux, NULL)) {
        g_warning ("Failed to link elements!");
        return -1;
    }

    if(!gst_element_link_many(newdata->audio_queue, newdata->rtpmp4gpay, newdata->audio_udpsink, NULL))
    {
        g_warning ("Failed to link elements!");
        return -1;
    }
    if(!gst_element_link_many(newdata->video_queue, newdata->rtph264pay, newdata->video_udpsink, NULL))
    {
        g_warning ("Failed to link elements!");
        return -1;
    }
    
    g_signal_connect(newdata->qtdemux, "pad-added", G_CALLBACK(qtdemux_callback), newdata);
    gst_element_set_state(newdata->pipeline, GST_STATE_PLAYING);

    return 0;
}
int make_rtp_bins(const char *ip, int port, int video_seqq, int audio_seq)
{
    rtpbin_data *myrtp = (rtpbin_data *)malloc(sizeof(rtpbin_data));
    myrtp->pipeline = gst_pipeline_new(NULL);
    myrtp->bus = gst_element_get_bus(GST_ELEMENT(myrtp->pipeline));
    gst_bus_add_watch(myrtp->bus, my_bus_callback, myrtp);
    gst_object_unref(myrtp->bus);

    myrtp->rtpbin = GST_BIN(gst_element_factory_make("rtpbin", "myrtpbin"));
    g_object_set(myrtp->rtpbin, "rtp-profile", GST_RTP_PROFILE_AVPF, NULL);

    gst_bin_add(GST_BIN(myrtp->pipeline), GST_ELEMENT(myrtp->rtpbin));

    myrtp->video_bin = GST_BIN(gst_bin_new(NULL));

    myrtp->video_queue = gst_element_factory_make("queue", "video_queue");
    myrtp->h264parse = gst_element_factory_make("h264parse", NULL);
    myrtp->rtph264pay = gst_element_factory_make("rtph264pay", NULL);

    gst_bin_add_many(myrtp->video_bin, 
                      myrtp->video_queue, 
                      myrtp->h264parse, 
                      myrtp->rtph264pay, 
                      NULL);

    gst_element_link_many(myrtp->video_queue, myrtp->h264parse, myrtp->rtph264pay, NULL);
    
    g_object_set(myrtp->rtph264pay, "pt", 96, NULL);
    
    GstPad *ghost_src_pad = gst_element_get_static_pad(myrtp->rtph264pay, "src");
    GstPad *ghost_bin_pad = gst_ghost_pad_new("src", ghost_src_pad);
    gst_element_add_pad(GST_ELEMENT(myrtp->video_bin), ghost_bin_pad);


    GstPad *queue_sink_pad = gst_element_get_static_pad(myrtp->video_queue, "sink");
    GstPad *ghost_pad = gst_ghost_pad_new("sink", queue_sink_pad);
    gst_pad_set_active(ghost_pad, TRUE);
    gst_element_add_pad(GST_ELEMENT(myrtp->video_bin), ghost_pad);
    gst_object_unref(queue_sink_pad);

    gchar *pad_name;

    myrtp->rtpsink_video = gst_element_factory_make("udpsink", "rtpsink_video");
    myrtp->rtcpsink_video = gst_element_factory_make("udpsink", "rtcpsink_video");
    myrtp->rtcpsrc_video = gst_element_factory_make("udpsrc", "rtcpsrc_video");
    myrtp->identity_video = gst_element_factory_make("identity", "identity_video");



    gst_bin_add_many(GST_BIN(myrtp->pipeline), 
                     myrtp->rtpsink_video,
                     myrtp->rtcpsink_video,
                     myrtp->rtcpsrc_video,
                     myrtp->identity_video,
                     GST_ELEMENT(myrtp->video_bin),
                     NULL);
    g_object_set(myrtp->rtpsink_video, "port", port, "host", ip, NULL);
    g_object_set(myrtp->rtcpsink_video, "port", port + 1, "host", ip, NULL, "sync", FALSE, "async", FALSE, NULL);
    g_object_set(myrtp->rtcpsrc_video, "port", port + 2, NULL);

    
    pad_name = g_strdup_printf("send_rtp_sink_%u", video_seqq);
    gst_element_link_pads(GST_ELEMENT(myrtp->video_bin), "src", GST_ELEMENT(myrtp->rtpbin), pad_name);
    g_free(pad_name);

    pad_name = g_strdup_printf("send_rtp_src_%u", video_seqq);
    // gst_element_link_pads(GST_ELEMENT(myrtp->rtpbin), pad_name, myrtp->identity_video, "sink");
    gst_element_link_pads(GST_ELEMENT(myrtp->rtpbin), pad_name, myrtp->identity_video, "sink");
    gst_element_link(myrtp->identity_video, myrtp->rtpsink_video);
    g_free(pad_name);

    pad_name = g_strdup_printf("send_rtcp_src_%u", video_seqq);
    gst_element_link_pads(GST_ELEMENT(myrtp->rtpbin), pad_name, myrtp->rtcpsink_video, "sink");
    g_free(pad_name);

    pad_name = g_strdup_printf("recv_rtcp_sink_%u", video_seqq);
    gst_element_link_pads(myrtp->rtcpsrc_video, "src", GST_ELEMENT(myrtp->rtpbin), pad_name);
    g_free(pad_name);

    g_print("New RTP stream on %i/%i/%i\n", port, port+1, port+2);



    myrtp->audio_bin = GST_BIN(gst_bin_new(NULL));
    myrtp->audio_queue = gst_element_factory_make("queue", "audio_queue");
    myrtp->rtpmp4gpay = gst_element_factory_make("rtpmp4gpay", NULL);

    gst_bin_add_many(myrtp->audio_bin, myrtp->audio_queue, myrtp->rtpmp4gpay, NULL);

    gst_element_link_many(myrtp->audio_queue, myrtp->rtpmp4gpay, NULL);

    g_object_set(myrtp->rtpmp4gpay, "pt", 98, NULL);

    GstPad *ghost_src_pad_audio = gst_element_get_static_pad(myrtp->rtpmp4gpay, "src");
    GstPad *ghost_bin_pad_audio = gst_ghost_pad_new("src", ghost_src_pad_audio);
    gst_element_add_pad(GST_ELEMENT(myrtp->audio_bin), ghost_bin_pad_audio);

    GstPad * ghost_sink_pad_audio = gst_element_get_static_pad(myrtp->audio_queue, "sink");
    GstPad * ghost_bin_sink_pad_audio = gst_ghost_pad_new("sink", ghost_sink_pad_audio);
    gst_pad_set_active(ghost_bin_sink_pad_audio, TRUE);
    gst_element_add_pad(GST_ELEMENT(myrtp->audio_bin), ghost_bin_sink_pad_audio);
    gst_object_unref(ghost_sink_pad_audio);

    
    myrtp->rtpsink_audio = gst_element_factory_make("udpsink", "udpsink_rtp_audio");
    myrtp->rtcpsink_audio = gst_element_factory_make("udpsink", "udpsink_rtcp_audio");
    myrtp->rtcpsrc_audio = gst_element_factory_make("udpsrc", "udpsrc_rtcp_audio");
    myrtp->identity_audio = gst_element_factory_make ("identity", NULL);

    gst_bin_add_many(GST_BIN(myrtp->pipeline), 
                     myrtp->rtpsink_audio,
                     myrtp->rtcpsink_audio,
                     myrtp->rtcpsrc_audio,
                    //  myrtp->identity_audio,
                     GST_ELEMENT(myrtp->audio_bin),
                     NULL);
    g_object_set(myrtp->rtpsink_audio, "port", port+ 4, "host", ip, NULL);
    g_object_set(myrtp->rtcpsink_audio, "port", port + 5, "host", ip, NULL, "sync", FALSE, "async", FALSE, NULL);
    g_object_set(myrtp->rtcpsrc_audio, "port", port + 6, NULL);

    pad_name = g_strdup_printf("send_rtp_sink_%u", audio_seq);
    gst_element_link_pads(GST_ELEMENT(myrtp->audio_bin), "src", GST_ELEMENT(myrtp->rtpbin), pad_name);
    g_free(pad_name);

    pad_name = g_strdup_printf("send_rtp_src_%u", audio_seq);
    gst_element_link_pads(GST_ELEMENT(myrtp->rtpbin), pad_name, myrtp->rtpsink_audio, "sink");
    // gst_element_link(myrtp->identity_audio, myrtp->rtpsink_audio);
    g_free(pad_name);

    pad_name = g_strdup_printf("send_rtcp_src_%u", audio_seq);
    gst_element_link_pads(GST_ELEMENT(myrtp->rtpbin), pad_name, myrtp->rtcpsink_audio, "sink");
    g_free(pad_name);

    pad_name = g_strdup_printf("recv_rtcp_sink_%u", audio_seq);
    gst_element_link_pads(myrtp->rtcpsrc_audio, "src", GST_ELEMENT(myrtp->rtpbin), pad_name);
    g_free(pad_name);

    g_print ("New RTP stream on %i/%i/%i\n", port + 4, port + 5, port + 6);




    myrtp->file_src = gst_element_factory_make("filesrc", NULL);
    myrtp->qtdemux = gst_element_factory_make("qtdemux", NULL);
    
    g_object_set(myrtp->file_src, "location", "/home/jdin/SampleVideo_1280x720_5mb.mp4", NULL);
    gst_bin_add_many(GST_BIN(myrtp->pipeline), myrtp->file_src, myrtp->qtdemux, NULL);
    gst_element_link(myrtp->file_src, myrtp->qtdemux);
    g_signal_connect(myrtp->qtdemux, "pad-added", G_CALLBACK(qtdemux_callback2), myrtp);

    // g_print("start server pipeline\n");
    gst_element_set_state(GST_ELEMENT(myrtp->pipeline), GST_STATE_PLAYING);
}
int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        g_print("input <your_ip> <video_port> <max_count> <my_index>\n");
        return -1;
    }

    g_print("%s\n", argv[1]);
    g_print("%s\n", argv[2]);
    g_print("%s\n", argv[3]);
    g_print("%s\n", argv[4]);

    int port = atoi(argv[2]);
    int count = atoi(argv[3]);
    int myidx = atoi(argv[4]);

    g_print("start main\n");

    mydata my;


    gst_init(&argc, &argv);

    my.main_loop = g_main_loop_new(NULL, FALSE);

    // make_pipeline("first", "192.168.0.4", 5004);
    int seq = 0;

    
    int garbage_port = 5000;

    for(int i = 0 ; i < count ; i++)
    {
        if(i == myidx)
        {
            make_rtp_bins(argv[1], port, seq, seq+1); seq += 2;
        } 
        else 
        {
            make_rtp_bins("192.168.0.192", garbage_port, seq, seq+1); seq += 2;
            garbage_port += 8;
        }
    }


    GST_INFO("[TESTDEBUG] start plyaing..\n");


    g_main_loop_run(my.main_loop);



    return 0;
}



