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

    def demuxer_callback(self, demuxer, pad):
        '''
        '''
        print(pad.get_property("template").name_template)
        if pad.get_property("template").name_template == "video_%u":
            pad.link(self.queue1.get_static_pad('sink'))
        elif pad.get_property("template").name_template == "audio_%u":
            pad.link(self.queue2.get_static_pad('sink'))

    def run(self):
        'gst-launch-1.0 filesrc location="/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/480p.mp4" ! qtdemux name=demux demux.audio_0 ! queue ! decodebin ! audioconvert ! audioresample ! autoaudiosink demux.video_0 ! queue ! decodebin ! videoconvert ! autovideosink'

        self.player = Gst.Pipeline.new('player')
        self.filesrc = Gst.ElementFactory.make('filesrc', 'filesrc')
        self.qtdemux = Gst.ElementFactory.make('qtdemux', 'qtdemux')

        self.queue1 = Gst.ElementFactory.make('queue', 'queue1')
        self.avdec_h264 = Gst.ElementFactory.make('avdec_h264', 'avdec_h264')
        self.videoconvert = Gst.ElementFactory.make('videoconvert', 'videoconvert')
        self.autovideosink = Gst.ElementFactory.make('autovideosink', 'autovideosink')

        self.queue2 = Gst.ElementFactory.make('queue', 'queue2')
        self.avdec_aac = Gst.ElementFactory.make('avdec_aac', 'avdec_aac')
        self.audioconvert = Gst.ElementFactory.make('audioconvert', 'audioconvert')
        self.audioresample = Gst.ElementFactory.make('audioresample', 'audioresample')
        self.autoaudiosink = Gst.ElementFactory.make('autoaudiosink', 'autoaudiosink')        
        
        self.player.add(self.filesrc)
        self.player.add(self.qtdemux)

        self.player.add(self.queue1)
        self.player.add(self.avdec_h264)
        self.player.add(self.videoconvert)
        self.player.add(self.autovideosink)

        self.player.add(self.queue2)
        self.player.add(self.avdec_aac)
        self.player.add(self.audioconvert)
        self.player.add(self.audioresample)
        self.player.add(self.autoaudiosink)
        
        self.filesrc.link(self.qtdemux)
        self.qtdemux.connect('pad-added', self.demuxer_callback)

        self.queue1.link(self.avdec_h264)
        self.avdec_h264.link(self.videoconvert)
        self.videoconvert.link(self.autovideosink)

        self.queue2.link(self.avdec_aac)
        self.avdec_aac.link(self.audioconvert)
        self.audioconvert.link(self.audioresample)
        self.audioresample.link(self.autoaudiosink)

        bus = self.player.get_bus()
        bus.add_signal_watch()
        bus.connect('message', self.on_message)

        filepath = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/480p.mp4'
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