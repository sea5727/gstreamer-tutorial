#!/usr/bin/python3

import os
import gi
gi.require_version('Gst', '1.0')

from gi.repository import Gst, GObject

class AudioPlayer(object):
    def __init__(self):
        '''
        '''
    def on_message(self, bus, message):
        '''
        '''
        t = message.type
        if t == Gst.MessageType.EOS:
            self.playbin.set_state(Gst.State.NULL)
        elif t == Gst.MessageType.ERROR:
            self.playbin.set_state(Gst.State.NULL)
            err, debug = message.parse_error()
            print(f'Error : {err}, {debug}')

    def run(self):
        filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/file_example_MP3_2MG.mp3'
        if os.path.isfile(filepath):
            playbin = Gst.ElementFactory.make("playbin", "player")
            fakesink = Gst.ElementFactory.make("fakesink", "fakesink")
            playbin.set_property('video-sink', fakesink)
            playbin.set_property('uri', f'file://{filepath}')
            playbin.set_state(Gst.State.PLAYING)
        else :
            print('file no exist')
            playbin.set_state(Gst.State.NULL)

if __name__ == '__main__':
    Gst.init(None)
    
    AudioPlayer().run()
    GObject.threads_init()
    loop = GObject.MainLoop()
    loop.run()