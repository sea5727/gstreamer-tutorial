/* GStreamer
 * Copyright (C) 2013 Collabora Ltd.
 *   @author Torrie Fischer <torrie.fischer@collabora.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <gst/gst.h>
#include <glib.h>
#include <gst/rtp/rtp.h>

/*
 * An RTP server
 *  creates two sessions and streams audio on one, video on the other, with RTCP
 *  on both sessions. The destination is 127.0.0.1.
 *
 *  In both sessions, we set "rtprtxsend" as the session's "aux" element
 *  in rtpbin, which enables RFC4588 retransmission for that session.
 *
 *  .-------.    .-------.    .-------.      .------------.       .-------.
 *  |audiots|    |alawenc|    |pcmapay|      | rtpbin     |       |udpsink|
 *  |      src->sink    src->sink    src->send_rtp_0 send_rtp_0->sink     |
 *  '-------'    '-------'    '-------'      |            |       '-------'
 *                                           |            |     
 *  .-------.    .---------.    .---------.  |            |       .-------.
 *  |audiots|    |theoraenc|    |theorapay|  |            |       |udpsink|
 *  |      src->sink      src->sink  src->send_rtp_1 send_rtp_1->sink     |
 *  '-------'    '---------'    '---------'  |            |       '-------'
 *                                           |            |
 *                               .------.    |            |
 *                               |udpsrc|    |            |       .-------.
 *                               |     src->recv_rtcp_0   |       |udpsink|
 *                               '------'    |       send_rtcp_0->sink    |
 *                                           |            |       '-------'
 *                               .------.    |            |
 *                               |udpsrc|    |            |       .-------.
 *                               |     src->recv_rtcp_1   |       |udpsink|
 *                               '------'    |       send_rtcp_1->sink    |
 *                                           '------------'       '-------'
 *
 * To keep the set of ports consistent across both this server and the
 * corresponding client, a SessionData struct maps a rtpbin session number to
 * a GstBin and is used to create the corresponding udp sinks with correct
 * ports.
 */

struct CallData{
  GstBin *audioBin;
  GstBin *videoBin;
  GstElement *video_queue;
  GstElement *audio_queue;  
};

static GstBin *audioBin;
static GstBin *videoBin;
static GstElement *video_queue;
static GstElement *audio_queue;

static void
qtdemux_pad_added_cb (GstElement * element, GstPad * new_pad, gpointer app)
{
  CallData *call = (CallData*)app;
    GstPad *sinkpad, *srcpad;

    g_print ("new payload on pad: %s\n", GST_PAD_NAME (new_pad));

    if (strcmp(GST_PAD_NAME(new_pad), "audio_0") == 0)
    {
        // g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        // sinkpad = gst_element_get_static_pad(GST_ELEMENT(call->audioBin), "sink");
        // g_assert(sinkpad);

        // GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        // g_print("lres:%d\n", lres);
        // g_assert(lres == GST_PAD_LINK_OK);
        // gst_object_unref(sinkpad);
    }
    else
    {
        g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        sinkpad = gst_element_get_static_pad(GST_ELEMENT(call->videoBin), "sink");
        g_assert(sinkpad);

        GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        g_print("lres:%d\n", lres);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(sinkpad);
    }

}

static void
qtdemux_pad_added_cb2 (GstElement * element, GstPad * new_pad, gpointer app)
{
    GstElement* appData = (GstElement *)app;
    GstPad *sinkpad, *srcpad;

    g_print ("new payload on pad: %s\n", GST_PAD_NAME (new_pad));

    if (strcmp(GST_PAD_NAME(new_pad), "audio_0") == 0)
    {
        // g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        // sinkpad = gst_element_get_static_pad(appData, "sink");
        // g_assert(sinkpad);

        // GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        // g_assert(lres == GST_PAD_LINK_OK);
        // gst_object_unref(sinkpad);
    }
    else
    {
        g_print("add pads..: %s\n", GST_PAD_NAME(new_pad));
        sinkpad = gst_element_get_static_pad(appData, "sink");
        g_assert(sinkpad);

        GstPadLinkReturn lres = gst_pad_link(new_pad, sinkpad);
        g_assert(lres == GST_PAD_LINK_OK);
        gst_object_unref(sinkpad);
    }

}


typedef struct _SessionData
{
  int ref;
  guint sessionNum;
  GstElement *input;
} SessionData;

static SessionData *
session_ref (SessionData * data)
{
  g_atomic_int_inc (&data->ref);
  return data;
}

static void
session_unref (gpointer data)
{
  SessionData *session = (SessionData *) data;
  if (g_atomic_int_dec_and_test (&session->ref)) {
    g_free (session);
  }
}

static SessionData *
session_new (guint sessionNum)
{
  SessionData *ret = g_new0 (SessionData, 1);
  ret->sessionNum = sessionNum;
  return session_ref (ret);
}

/*
 * Used to generate informative messages during pipeline startup
 */
static void
cb_state (GstBus * bus, GstMessage * message, gpointer data)
{
  GstObject *pipe = GST_OBJECT (data);
  GstState old, new_, pending;
  gst_message_parse_state_changed (message, &old, &new_, &pending);
  if (message->src == pipe) {
    g_print ("Pipeline %s changed state from %s to %s\n",
        GST_OBJECT_NAME (message->src),
        gst_element_state_get_name (old), gst_element_state_get_name (new_));
  }
}

/*
 * Creates a GstGhostPad named "src" on the given bin, pointed at the "src" pad
 * of the given element
 */
static void
setup_ghost (GstElement * src, GstBin * bin)
{
  GstPad *srcPad = gst_element_get_static_pad (src, "src");
  GstPad *binPad = gst_ghost_pad_new ("src", srcPad);
  gst_element_add_pad (GST_ELEMENT (bin), binPad);
}


static SessionData *
make_audio_session2 (guint sessionNum, CallData *call)
{
  SessionData *session;
  call->audioBin = GST_BIN (gst_bin_new (NULL));
  // GstElement *audioSrc = gst_element_factory_make ("filesrc", NULL);
  // GstElement *demux = gst_element_factory_make("qtdemux", NULL);
  call->audio_queue = gst_element_factory_make("queue", "audio_queue");
  GstElement *audio_rtppay = gst_element_factory_make("rtpmp4gpay", "audio_rtppay");

  // g_object_set (audioSrc, "location", "/home/ysh8361/Videos/SampleVideo_1280x720_5mb.mp4", NULL);

  // gst_bin_add_many (audioBin, audioSrc, demux, audio_queue, audio_rtppay, NULL);
  gst_bin_add_many (call->audioBin, call->audio_queue, audio_rtppay, NULL);

  // gst_element_link_many (audioSrc, demux, NULL);
  gst_element_link_many (call->audio_queue, audio_rtppay, NULL);

  // g_signal_connect (demux, "pad-added", G_CALLBACK (qtdemux_pad_added_cb), audio_queue);

  setup_ghost (audio_rtppay, call->audioBin);
  GstPad* pad = gst_element_get_static_pad (call->audio_queue, "sink");
  GstPad* ghost_pad = gst_ghost_pad_new("sink", pad);
  gst_pad_set_active (ghost_pad, TRUE);
  gst_element_add_pad (GST_ELEMENT(call->audioBin), ghost_pad);
  gst_object_unref (pad);

  session = session_new (sessionNum);
  session->input = GST_ELEMENT (call->audioBin);

  return session;
}


static SessionData *
make_video_session2 (guint sessionNum, CallData *call)
{
  call->videoBin = GST_BIN (gst_bin_new (NULL));
  // GstElement *videoSrc = gst_element_factory_make ("filesrc", NULL);
  // GstElement *demux = gst_element_factory_make("qtdemux", NULL);

  call->video_queue = gst_element_factory_make("queue", NULL);
  GstElement *video_parse = gst_element_factory_make("h264parse", NULL);
  GstElement *video_rtppay = gst_element_factory_make("rtph264pay", NULL);

  GstCaps *videoCaps;
  SessionData *session;
  // g_object_set (videoSrc, "location", "/home/ysh8361/Videos/SampleVideo_1280x720_5mb.mp4", NULL);

  // gst_bin_add_many (videoBin, videoSrc, demux, video_queue, video_parse, video_rtppay, NULL);
  gst_bin_add_many (call->videoBin, call->video_queue, video_parse, video_rtppay, NULL);

  // g_signal_connect (demux, "pad-added", G_CALLBACK (qtdemux_pad_added_cb2), video_queue);
  
  // gst_element_link (videoSrc, demux);
  gst_element_link_many (call->video_queue, video_parse, video_rtppay, NULL);

  setup_ghost (video_rtppay, call->videoBin);
  
  GstPad* pad = gst_element_get_static_pad (call->video_queue, "sink");
  GstPad* ghost_pad = gst_ghost_pad_new("sink", pad);
  gst_pad_set_active (ghost_pad, TRUE);
  gst_element_add_pad (GST_ELEMENT(call->videoBin), ghost_pad);
  gst_object_unref (pad);



  session = session_new (sessionNum);
  session->input = GST_ELEMENT (call->videoBin);

  return session;
}


static GstElement *
request_aux_sender (GstElement * rtpbin, guint sessid, SessionData * session)
{
  GstElement *rtx, *bin;
  GstPad *pad;
  gchar *name;
  GstStructure *pt_map;

  GST_INFO ("creating AUX sender");
  bin = gst_bin_new (NULL);
  rtx = gst_element_factory_make ("rtprtxsend", NULL);
  pt_map = gst_structure_new ("application/x-rtp-pt-map",
      "8", G_TYPE_UINT, 98, "96", G_TYPE_UINT, 99, NULL);
  g_object_set (rtx, "payload-type-map", pt_map, NULL);
  gst_structure_free (pt_map);
  gst_bin_add (GST_BIN (bin), rtx);

  pad = gst_element_get_static_pad (rtx, "src");
  name = g_strdup_printf ("src_%u", sessid);
  gst_element_add_pad (bin, gst_ghost_pad_new (name, pad));
  g_free (name);
  gst_object_unref (pad);

  pad = gst_element_get_static_pad (rtx, "sink");
  name = g_strdup_printf ("sink_%u", sessid);
  gst_element_add_pad (bin, gst_ghost_pad_new (name, pad));
  g_free (name);
  gst_object_unref (pad);

  return bin;
}

/*
 * This function sets up the UDP sinks and sources for RTP/RTCP, adds the
 * given session's bin into the pipeline, and links it to the properly numbered
 * pads on the rtpbin
 */
static void
add_stream (GstPipeline * pipe, GstElement * rtpBin, SessionData * session)
{
  GstElement *rtpSink = gst_element_factory_make ("udpsink", NULL);
  GstElement *rtcpSink = gst_element_factory_make ("udpsink", NULL);
  GstElement *rtcpSrc = gst_element_factory_make ("udpsrc", NULL);
  int basePort;
  gchar *padName;

  basePort = 5000 + (session->sessionNum * 6);

  gst_bin_add_many (GST_BIN (pipe), rtpSink, rtcpSink, rtcpSrc, session->input, NULL);

  /* enable retransmission by setting rtprtxsend as the "aux" element of rtpbin */
  g_signal_connect (rtpBin, "request-aux-sender",
      (GCallback) request_aux_sender, session);

  g_object_set (rtpSink, "port", basePort, "host", "192.168.0.30", NULL);
  g_object_set (rtcpSink, "port", basePort + 1, "host", "192.168.0.30", "sync",
      FALSE, "async", FALSE, NULL);
  g_object_set (rtcpSrc, "port", basePort + 5, NULL);

  /* this is just to drop some rtp packets at random, to demonstrate
   * that rtprtxsend actually works */
//   g_object_set (identity, "drop-probability", 0.01, NULL);

  padName = g_strdup_printf ("send_rtp_sink_%u", session->sessionNum);
  gst_element_link_pads (session->input, "src", rtpBin, padName);
  g_free (padName);

  /* link rtpbin to udpsink directly here if you don't want
   * artificial packet loss */
//   padName = g_strdup_printf ("send_rtp_src_%u", session->sessionNum);
//   gst_element_link_pads (rtpBin, padName, identity, "sink");
//   gst_element_link (identity, rtpSink);
//   g_free (padName);

  padName = g_strdup_printf ("send_rtcp_src_%u", session->sessionNum);
  gst_element_link_pads (rtpBin, padName, rtcpSink, "sink");
  g_free (padName);

  padName = g_strdup_printf ("recv_rtcp_sink_%u", session->sessionNum);
  gst_element_link_pads (rtcpSrc, "src", rtpBin, padName);
  g_free (padName);

  g_print ("New RTP stream on %i/%i/%i\n", basePort, basePort + 1,
      basePort + 5);

  session_unref (session);
}

int
main (int argc, char **argv)
{
  
  char *filepath = NULL;
  GError *error = NULL;
  int session_count = 1;
  GOptionEntry entries[] = {
    {"session-count", 's', 0, G_OPTION_ARG_INT, &session_count, "session count", "1"},
    {NULL}
  };


  GOptionContext *context = g_option_context_new("App");
  g_option_context_set_help_enabled(context, TRUE);
  g_option_context_add_main_entries(context, entries, "test");
  if(!g_option_context_parse(context, &argc, &argv, &error)){
    g_print("option parsing failed : %s\n", error->message);
    exit(1);
  }

  g_print("session_count:%d\n", session_count);

  GstPipeline *pipe;
  GstBus *bus;
  SessionData *videoSession;
  SessionData *audioSession;
  GstElement *rtpBin;
  GMainLoop *loop;

  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  for (int i = 0; i < session_count; i++)
  {
    CallData *call = new CallData();
    pipe = GST_PIPELINE(gst_pipeline_new(NULL));
    bus = gst_element_get_bus(GST_ELEMENT(pipe));
    g_signal_connect(bus, "message::state-changed", G_CALLBACK(cb_state), pipe);
    gst_bus_add_signal_watch(bus);
    gst_object_unref(bus);

    rtpBin = gst_element_factory_make("rtpbin", NULL);
    g_object_set(rtpBin, "rtp-profile", GST_RTP_PROFILE_AVPF, NULL);

    gst_bin_add_many(GST_BIN(pipe), rtpBin, NULL);

    videoSession = make_video_session2(0 + (i * 2), call);
    // audioSession = make_audio_session2(1 + (i * 2), call);
    add_stream(pipe, rtpBin, videoSession);
    // add_stream(pipe, rtpBin, audioSession);

    GstElement *videoSrc = gst_element_factory_make("filesrc", NULL);
    GstElement *demux = gst_element_factory_make("qtdemux", NULL);

    g_object_set(videoSrc, "location", "/home/jdin/SampleVideo_1280x720_5mb.mp4", NULL);
    gst_bin_add_many(GST_BIN(pipe), videoSrc, demux, NULL);
    gst_element_link(videoSrc, demux);
    g_signal_connect(demux, "pad-added", G_CALLBACK(qtdemux_pad_added_cb), call);

    g_print("starting server pipeline\n");
    gst_element_set_state(GST_ELEMENT(pipe), GST_STATE_PLAYING);
  }

  g_main_loop_run (loop);

  g_print ("stopping server pipeline\n");
  gst_element_set_state (GST_ELEMENT (pipe), GST_STATE_NULL);

  gst_object_unref (pipe);
  g_main_loop_unref (loop);

  return 0;
}