#!/usr/bin/python3

import os
import gi
gi.require_version('Gst', '1.0')

from gi.repository import Gst, GObject

if __name__ == '__main__':
    Gst.init(None)

    pipeline  = Gst.parse_launch("filesrc location=\"/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/480p.mp4\" ! qtdemux name=demux demux.audio_0 ! queue ! decodebin ! audioconvert ! audioresample ! autoaudiosink demux.video_0 ! queue ! decodebin ! videoscale ! videoconvert ! autovideosink")
    pipeline.set_state (Gst.State.PLAYING);
    
    GObject.threads_init()
    loop = GObject.MainLoop()
    loop.run()