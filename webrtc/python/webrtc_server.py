#!/bin/python3

import asyncio
import argparse
import sys
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst
gi.require_version('GstWebRTC', '1.0')
from gi.repository import GstWebRTC
gi.require_version('GstSdp', '1.0')
from gi.repository import GstSdp

def check_plugins():
    needed = ["opus", "vpx", "nice", "webrtc", "dtls", "srtp", "rtp", "rtpmanager", "videotestsrc", "audiotestsrc"]
    missing = list(filter(lambda p: Gst.Registry.get().find_plugin(p) is None, needed))
    if len(missing):
        print('Missing gstreamer plugins:', missing)
        return False
    return True

PIPELINE_DESC = '''
webrtcbin name=sendrecv bundle-policy=max-bundle stun-server=stun://stun.l.google.com:19302
 videotestsrc is-live=true pattern=ball ! videoconvert ! queue ! vp8enc deadline=1 ! rtpvp8pay !
 queue ! application/x-rtp,media=video,encoding-name=VP8,payload=97 ! sendrecv.
 audiotestsrc is-live=true wave=red-noise ! audioconvert ! audioresample ! queue ! opusenc ! rtpopuspay !
 queue ! application/x-rtp,media=audio,encoding-name=OPUS,payload=96 ! sendrecv.
'''


class WebRTCPeer:
    def __init__(self, address, port):
        '''
        '''
        self.address = address
        self.port = port
        self.server = 'wss://{address}:{port}'
    def pipeline(self):
        '''
        '''
        self.pipe = Gst.parse_launch(PIPELINE_DESC)
        self.webrtcbin = self.pipe.get_by_name('sendrecv')
        # self.webrtcbin.connect('create-offer')
        # self.webrtcbin.connect('create-answer')
        # self.webrtcbin.connect('set-local-description')
        # self.webrtcbin.connect('set-remote-description')
        # self.webrtcbin.connect('add-ice-candidate')
        # self.webrtcbin.connect('get-stats')
        # self.webrtcbin.connect('on-negotiation-needed')
        # self.webrtcbin.connect('on-ice-candidate')
        # self.webrtcbin.connect('on-new-transceiver')
        # self.webrtcbin.connect('on-data-channel')
        # self.webrtcbin.connect('add-transceiver')
        # self.webrtcbin.connect('get-transceivers')
        # self.webrtcbin.connect('get-transceiver')
        # self.webrtcbin.connect('add-turn-server')
        # self.webrtcbin.connect('create-data-channel')

        self.webrtcbin.connect('on-negotiation-needed', self.on_negotiation_needed, 'mydata')
        self.webrtcbin.connect('on-ice-candidate', self.on_ice_candidate, 'mydata')
        self.webrtcbin.connect('pad-added', self.on_incoming_stream, 'mydata')
        self.pipe.set_state(Gst.State.PLAYING)

    def on_negotiation_needed(self, element, mydata):
        '''
        '''
        print('on_negotiation_needed')
        promise = Gst.Promise.new_with_change_func(self.on_offer_created, element, None)
        element.emit('create-offer', None, promise)

    def on_ice_candidate(self, param1):
        '''
        '''
        print('on_ice_candidate')

    def on_incoming_stream(self, param1):
        '''
        '''
        print('on_incoming_stream')

    def on_offer_created(self, promise, element, _):
        '''
        '''
        print('on_offer_created')
        




if __name__ == '__main__':
    Gst.init(None)
    if not check_plugins():
        sys.exit(1)
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--listen', help='listening address default:0.0.0.0')
    parser.add_argument('--port', help='listening port default:8999', default=8999)
    args = parser.parse_args()

    peer = WebRTCPeer(args.listen, args.port)
    peer.pipeline()

    loop = asyncio.get_event_loop()
    loop.run_forever()

    sys.exit()




