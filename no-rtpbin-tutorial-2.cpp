#include <gst/gst.h>
#include <unistd.h>

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
            g_print("Got %s message.. data : %p\n", GST_MESSAGE_TYPE_NAME(message), data);
            break;
        case GST_MESSAGE_EOS:
            {
                g_print("Got %s message.. data : %p\n", GST_MESSAGE_TYPE_NAME(message), data);
                if(count != 0) break;
                gst_element_set_state(my->pipeline, GST_STATE_NULL);
                
                g_print("mypipeline state is null?\n" );


                count += 1 ;
                gchar *location;
                g_object_get (G_OBJECT (my->file_src), "location", &location, NULL);
                g_print("location : %s\n", location);
                g_object_set(my->file_src, "location", "/home/ysh8361/kotlin-compiler-1.3.72.zip", NULL);
                g_object_set(my->file_sink, "location", "/home/ysh8361/new.zip", NULL);

                
                gst_element_set_state(my->pipeline, GST_STATE_PLAYING);
                break;
            }
        case GST_MESSAGE_STATE_CHANGED:
        {
            GstState pending;
            GstState old_state;
            GstState new_state;
            
            // GstElement *element = (GstElement *)data;
            
            gst_message_parse_state_changed(message, &old_state, &new_state, &pending);           

            g_print("Got %s message.. element : %s... old:%s, new:%s, pending:%s\n",             
                GST_MESSAGE_TYPE_NAME(message),
                GST_ELEMENT_NAME(GST_ELEMENT(message->src)),
                gst_element_state_get_name (old_state),
                gst_element_state_get_name (new_state),
                gst_element_state_get_name (pending));
            break;
        }
        case GST_MESSAGE_STREAM_STATUS:
        {
            GstStreamStatusType type;
            GstElement *owner;
            gst_message_parse_stream_status(message, &type, &owner);
            
            g_print("Got %s message.. element : %s.. type:%d\n",
                GST_MESSAGE_TYPE_NAME(message), 
                GST_ELEMENT_NAME(GST_ELEMENT(owner)),
                type);
            
            break;
        }
        case GST_MESSAGE_STREAM_START:
        {
            g_print("Got %s message... element : %s.. \n" , 
                GST_MESSAGE_TYPE_NAME(message),
                GST_ELEMENT_NAME(GST_ELEMENT(message->src)));
            break;
        }
        case GST_MESSAGE_ASYNC_DONE:
        {
            GstClockTime clock_time;
            gst_message_parse_async_done(message, &clock_time);

            g_print("Got %s message.. %" GST_TIME_FORMAT  "\n", 
                GST_MESSAGE_TYPE_NAME(message),
                GST_TIME_ARGS(clock_time));
                
            break;
        }
        case GST_MESSAGE_NEW_CLOCK:
        {
            GstClock *clock;
            gst_message_parse_new_clock(message, &clock);
            GstClockTime time;
            time = gst_clock_get_time (clock);
            g_print("Got %s message.. time : %" GST_TIME_FORMAT "\n", 
                GST_MESSAGE_TYPE_NAME(message),
                GST_TIME_ARGS (time));
            break;
        }
        case  GST_MESSAGE_TAG:
        {
            // GstTagList *taglist;
            // gst_message_parse_tag(message, &taglist);
            g_print("Got %s message.. %d..\n", 
                GST_MESSAGE_TYPE_NAME(message),
                GST_MESSAGE_TYPE(message));
            break;
        }
        default:
            g_print("Got %s message.. default error.. %d..\n", 
                GST_MESSAGE_TYPE_NAME(message),
                GST_MESSAGE_TYPE(message));
    }
    return TRUE;
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
        g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        GstPad* video_queue = gst_element_get_static_pad(my->video_queue, "sink");
        g_assert(video_queue);


        GstPadLinkReturn lres = gst_pad_link(new_pad, video_queue);
        g_print("lres:%d\n", lres);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(video_queue);
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

    g_object_set(newdata->video_udpsink, "host", host, NULL);
    g_object_set(newdata->video_udpsink, "port", port - 4, NULL);

    // gboolean sync;
    // g_object_set(newdata->audio_udpsink, "sync", FALSE, NULL);
    // g_object_get(newdata->audio_udpsink, "sync", &sync, NULL);
    // g_print("sync is : %d\n", sync);


    // gboolean async;
    // g_object_set(newdata->audio_udpsink, "async", FALSE, NULL);
    // g_object_get(newdata->audio_udpsink, "async", &async, NULL);
    // g_print("async is : %d\n", async);

    g_object_set(newdata->rtpmp4gpay, "pt", 98, NULL);
    g_object_set(newdata->rtph264pay, "pt", 96, NULL);

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

    // gst_bin_add_many (GST_BIN (newdata->pipeline), 
    //                 //   newdata->file_src, 
    //                 //   newdata->qtdemux, 
    //                 //   newdata->audio_queue, 
    //                 //   newdata->rtpmp4gpay, 
    //                 //   newdata->audio_udpsink,
    //                   newdata->video_queue,
    //                   newdata->rtph264pay,
    //                   newdata->video_udpsink,
    //                   NULL);
    if(!gst_element_link_many(newdata->video_queue, newdata->rtph264pay, newdata->video_udpsink, NULL))
    {
        g_warning ("Failed to link elements!");
        return -1;
    }
    
    g_signal_connect(newdata->qtdemux, "pad-added", G_CALLBACK(qtdemux_callback), newdata);
    gst_element_set_state(newdata->pipeline, GST_STATE_PLAYING);

    return 0;
}
int main(int argc, char *argv[])
{
    g_print("start main\n");

    mydata my;


    gst_init(&argc, &argv);

    my.main_loop = g_main_loop_new(NULL, FALSE);


    // my.pipeline = gst_pipeline_new("main_pipeline");
    // my.bus = gst_pipeline_get_bus(GST_PIPELINE(my.pipeline));
    // gst_bus_add_watch(my.bus, my_bus_callback, &my);
    // gst_object_unref(my.bus);

    

    // my.file_src = gst_element_factory_make("filesrc", NULL);
    // my.file_sink = gst_element_factory_make("filesink", NULL);
    // my.qtdemux = gst_element_factory_make("qtdemux", NULL);
    // my.rtpmp4gpay = gst_element_factory_make("rtpmp4gpay", NULL);
    // my.audio_udpsink = gst_element_factory_make("udpsink", "udpsink_audio");


    // my.rtph264pay =  gst_element_factory_make("rtph264pay", NULL);


    // g_object_set(my.audio_udpsink, "host", "192.168.0.4", NULL);
    // g_object_set(my.audio_udpsink, "port", 5004, NULL);


    // g_object_set(my.rtpmp4gpay, "pt", 98, NULL);

    // g_object_set(my.file_src, "location", "/home/ysh8361/SampleVideo_1280x720_5mb.mp4", NULL);
    // g_object_set(my.file_sink, "location", "/home/ysh8361/new.mp4", NULL);

    // gchar *filesrcname;
    // g_object_get (G_OBJECT (my.file_src), "name", &filesrcname, NULL);
    // g_print("filesrcname : %s\n", filesrcname);

    // gchar *get_path;
    // g_object_get (G_OBJECT (my.file_src), "location", &get_path, NULL);
    // g_print("path : %s\n", get_path);

    
    // gst_bin_add_many (GST_BIN (my.pipeline), my.file_src, my.qtdemux, my.rtpmp4gpay, my.audio_udpsink, NULL);

    // if (!gst_element_link_many (my.file_src, my.qtdemux, NULL)) {
    //     g_warning ("Failed to link elements!");
    //     return -1;
    // }

    // if(!gst_element_link_many(my.rtpmp4gpay, my.audio_udpsink, NULL))
    // {
    //     g_warning ("Failed to link elements!");
    //     return -1;
    // }
    
    // g_signal_connect(my.qtdemux, "pad-added", G_CALLBACK(qtdemux_callback), &my);




    // gst_element_set_state(my.pipeline, GST_STATE_PLAYING);



    
    make_pipeline("first", "192.168.0.4", 5004);
    // make_pipeline("second", "192.168.0.3", 6004);
    // make_pipeline("1", "192.168.0.3", 7004);
    // make_pipeline("2", "192.168.0.3", 8004);
    // make_pipeline("3", "192.168.0.3", 9004);
    // make_pipeline("4", "192.168.0.3", 10004);
    // make_pipeline("5", "192.168.0.3", 11004);
    // make_pipeline("6", "192.168.0.3", 13004);
    // make_pipeline("7", "192.168.0.3", 14004);
    // make_pipeline("8", "192.168.0.3", 15004);
    // make_pipeline("9", "192.168.0.3", 16004);
    // make_pipeline("10", "192.168.0.3", 17004);
    // make_pipeline("11", "192.168.0.3", 18004);
    // make_pipeline("12", "192.168.0.3", 19004);
    // make_pipeline("13", "192.168.0.3", 20004);
    // make_pipeline("14", "192.168.0.3", 21004);
    // make_pipeline("15", "192.168.0.3", 22004);
    // make_pipeline("16", "192.168.0.3", 23004);
    // make_pipeline("17", "192.168.0.3", 24004);


    GST_INFO("[TESTDEBUG] start plyaing..\n");
    // gst_element_set_state (file_src, GST_STATE_PLAYING);
    // gst_element_set_state (file_sink, GST_STATE_PLAYING);
    // g_print("path_string : %s\n", gst_object_get_path_string (GST_OBJECT(file_src)));
    // g_print("path_string : %s\n", gst_object_get_path_string (GST_OBJECT(file_sink)));

    // g_print("file_src->numpads : %hd\n", my.file_src->numpads);
    // g_print("file_src->numsrcpads : %hd\n", my.file_src->numsrcpads);
    // g_print("file_src->numsinkpads : %hd\n", my.file_src->numsinkpads);

    // GstPad *gp = my.file_src->srcpads->data;
    // gchar *srcpadname;
    // g_object_get (G_OBJECT (gp), "name", &srcpadname, NULL);
    // g_print("srcpadname : %s\n", srcpadname);

    // GstTask *gt = gp->task;
    // gchar *gtname;
    // g_object_get (G_OBJECT (gt), "name", &gtname, NULL);
    // g_print("srcpadname : %s\n", gtname);


    g_main_loop_run(my.main_loop);

    // g_object_unref(GST_OBJECT(my.file_src));



    return 0;
}


