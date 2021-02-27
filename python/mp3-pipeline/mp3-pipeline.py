#!/usr/bin/python3

import os
import gi
gi.require_version('Gst', '1.0')

from gi.repository import Gst, GObject

class Mp3Pipeline(object):
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

    def run(self):

        self.player = Gst.Pipeline.new('player')
        filesrc = Gst.ElementFactory.make('filesrc', 'filesrc')
        mpegaudioparse = Gst.ElementFactory.make('mpegaudioparse', 'mpegaudioparse')
        
        avdec_mp3 = Gst.ElementFactory.make('avdec_mp3', 'avdec_mp3')
        audioconvert = Gst.ElementFactory.make('audioconvert', 'audioconvert')
        alsasink = Gst.ElementFactory.make('alsasink', 'alsasink')
        
        self.player.add(filesrc)
        self.player.add(mpegaudioparse)
        self.player.add(avdec_mp3)
        self.player.add(audioconvert)
        self.player.add(alsasink)
        
        filesrc.link(mpegaudioparse)
        mpegaudioparse.link(avdec_mp3)
        avdec_mp3.link(audioconvert)
        audioconvert.link(alsasink)

        bus = self.player.get_bus()
        bus.add_signal_watch()
        bus.connect('message', self.on_message)

        filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/file_example_MP3_2MG.mp3'
        if os.path.isfile(filepath):
            self.player.get_by_name('filesrc').set_property('location', filepath)
            self.player.set_state(Gst.State.PLAYING)
            
        else :
            print('file no exist')
            self.player.set_state(Gst.State.NULL)

if __name__ == '__main__':
    Gst.init(None)
    
    Mp3Pipeline().run()
    GObject.threads_init()
    loop = GObject.MainLoop()
    loop.run()