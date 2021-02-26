#!/usr/bin/python3

import os
import gi
gi.require_version('Gst', '1.0')

from gi.repository import Gst, GObject

def on_message(bus, message):
    '''
    '''
def frun():
    filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/gstreamer-hello/file_example_MP3_2MG.mp3'
    if os.path.isfile(filepath):
        playbin = Gst.ElementFactory.make("playbin", "player")
        fakesink = Gst.ElementFactory.make("fakesink", "fakesink")
        playbin.set_property('video-sink', fakesink)
        playbin.set_property('uri', f'file://{filepath}')

        bus = playbin.get_bus()
        bus.add_signal_watch()
        bus.connect("message", on_message)

        playbin.set_state(Gst.State.PLAYING)
    else :
        print('file no exist')
        playbin.set_state(Gst.State.NULL)

class AudioPlayer(object):
    def __init__(self):
        '''
        '''
    def on_message(self, bus, message):
        '''
        '''
        t = message.type
        if t == Gst.MessageType.EOS:
            playbin.set_state(Gst.State.NULL)
        elif t == Gst.MessageType.ERROR:
            playbin.set_state(Gst.State.NULL)
            err, debug = message.parse_error()
            print("Error: %s" % err, debug)

    def run(self):
        filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/gstreamer-hello/file_example_MP3_2MG.mp3'
        if os.path.isfile(filepath):
            playbin = Gst.ElementFactory.make("playbin", "player")
            fakesink = Gst.ElementFactory.make("fakesink", "fakesink")
            playbin.set_property('video-sink', fakesink)
            playbin.set_property('uri', f'file://{filepath}')


            playbin.set_state(Gst.State.PLAYING)
        else :
            print('file no exist')
            playbin.set_state(Gst.State.NULL)

        # player = Gst.ElementFactory.make("playbin", "player")
        # fakesink = Gst.ElementFactory.make("fakesink", "fakesink")
        # player.set_property("video-sink", fakesink)
        # # bus = player.get_bus()
        # # bus.add_signal_watch()
        # # bus.connect("message", on_message)

        # filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/gstreamer-hello/file_example_MP3_2MG.mp3'

        # print('filepath:', filepath)
        # filepath = os.path.realpath(filepath)
        # print('filepath:', filepath)
        # # self.button.set_label("Stop")
        # player.set_property("uri", "file://" + filepath)
        # player.set_state(Gst.State.PLAYING)

if __name__ == '__main__':
    Gst.init(None)
    
    # audioplayer = AudioPlayer()
    # audioplayer.run()
    frun()

    GObject.threads_init()
    loop = GObject.MainLoop()
    loop.run()