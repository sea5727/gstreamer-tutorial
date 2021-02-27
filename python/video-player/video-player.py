#!/usr/bin/python3

import os
import gi
gi.require_version('Gst', '1.0')

from gi.repository import Gst, GObject

class VideoPlayer(object):
    def __init__(self):
        '''
        '''
    def on_message(self, bus, message):
        '''
        '''
        t = message.type
        if t == Gst.MessageType.EOS:
            self.player.set_state(Gst.State.NULL)
        elif t == Gst.MessageType.ERROR:
            self.player.set_state(Gst.State.NULL)
            err, debug = message.parse_error()
            print(f'Error : {err}, {debug}')


    def on_sync_message(self, bus, message):
        print('on_sync_message??')
        if message.get_structure().get_name() == 'prepare-window-handle':
            imagesink = message.src
            imagesink.set_property("force-aspect-ratio", True)
            imagesink.set_window_handle(self.movie_window.get_property('window').get_xid())


    def run(self):
        filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/480p.mp4'
        if os.path.isfile(filepath):
            self.player = Gst.ElementFactory.make("playbin", "player")
            self.player.set_property('uri', f'file://{filepath}')

            bus = self.player.get_bus()
            bus.add_signal_watch()
            bus.enable_sync_message_emission()
            bus.connect("message", self.on_message)
            bus.connect("sync-message::element", self.on_sync_message)


            self.player.set_state(Gst.State.PLAYING)
        else :
            print('file no exist')
            self.player.set_state(Gst.State.NULL)

if __name__ == '__main__':
    Gst.init(None)
    
    VideoPlayer().run()
    GObject.threads_init()
    loop = GObject.MainLoop()
    loop.run()