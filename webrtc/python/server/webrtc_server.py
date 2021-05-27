#!/bin/python3

import asyncio
import argparse
import ssl
import websockets
import sys
import threading
import json
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
        print('__init__ thread:', threading.get_ident())
        self.address = address
        self.port = port
        self.server = 'wss://{address}:{port}'
    def pipeline(self):
        '''
        '''
        print('pipeline thread:', threading.get_ident())
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

        # self.webrtcbin.connect('on-negotiation-needed', self.on_negotiation_needed, None)
        self.webrtcbin.connect('on-ice-candidate', self.on_ice_candidate, None)
        self.webrtcbin.connect('pad-added', self.on_incoming_stream, None)
        self.pipe.set_state(Gst.State.PLAYING)

    def on_negotiation_needed(self, element, mydata):
        '''
        '''
        print('on_negotiation_needed thread:', threading.get_ident())
        promise = Gst.Promise.new_with_change_func(self.on_offer_created, element, None)
        element.emit('create-offer', None, promise)

    def on_ice_candidate(self, element, sdpMLineIndex, candidate, _):
        '''
        '''
        print(f'on_ice_candidate thread: {threading.get_ident()}')
        

    def on_incoming_stream(self, param1):
        '''
        '''
        print('on_incoming_stream thread:', threading.get_ident())

    def on_offer_created(self, promise, element, _):
        '''
        '''
        print(f'on_offer_created thread: {threading.get_ident()}, element:{type(element)}')
        promise.wait()
        reply = promise.get_reply()
        offer = reply.get_value('offer')
        promise = Gst.Promise.new_with_change_func(self.on_set_local_description, element, None)
        element.emit('set-local-description', offer, promise)
        self.send_offer(offer)
    
    def on_set_local_description(self, promise, element, _):
        '''
        '''
        print(f'on_set_local_description thread: {threading.get_ident()}, element:{type(element)}')
        promise.wait()



    def send_offer(self, offer):
        '''
        '''
        print(f'send_offer:{offer}, thread: {threading.get_ident()}')
        text = offer.sdp.as_text()
        print('sdp:', text)
        

class WebRTCServer:
    _id = 0
    def __init__(self, websocket):
        '''
        need a mutex?
        '''
        self.id = WebRTCServer._id
        WebRTCServer._id += 1
        self.websocket = websocket

        self.pipe = Gst.parse_launch(PIPELINE_DESC)
        self.webrtc = self.pipe.get_by_name('sendrecv')
        self.webrtc.connect('on-ice-candidate', self.onIceCandidate, self.websocket)
        self.webrtc.connect('pad-added', self.onPadAdded)

        self.pipe.set_state(Gst.State.PLAYING)
    def startPipeline(self):
        '''
        '''


    def onIceCandidate(self, element, sdpMLineIndex, candidate, conn):
        '''
        '''
        print('onIceCandidate thread:', threading.get_ident())

        icemsg = json.dumps({'event': {'type': 'ice', 'candidate': candidate, 'sdpMLineIndex': sdpMLineIndex}})
        # conn.send(icemsg)

    def onPadAdded(self, param1):
        '''
        '''
        print('onPadAdded thread:', threading.get_ident())
    
    def onSetLocalDescription(self, promise, param1):
        '''
        need to send answer
        '''
        print('onSetLocalDescription thread:', threading.get_ident())

        promise.wait()
        reply = promise.get_reply()
        

    def setRemoteDescription(self, offer_sdp):
        '''
        '''
        print('setRemoteDescription thread:', threading.get_ident())
        result, offer = GstSdp.SDPMessage.new()
        assert(result == GstSdp.SDPResult.OK)
        GstSdp.sdp_message_parse_buffer(offer_sdp.encode(), offer)
        description = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.OFFER, offer)
        self.webrtc.emit('set-remote-description', description, None)

        promise = Gst.Promise.new()
        self.webrtc.emit('create-answer', None, promise)
        promise.wait()
        reply = promise.get_reply()
        answer = reply.get_value('answer')
        # promise = Gst.Promise.new_with_change_func(self.onSetLocalDescription, 'myaatga')
        promise = Gst.Promise.new()
        self.webrtc.emit('set-local-description', answer, promise)
        promise.wait()

        return answer.sdp.as_text()
    
    def addIceCandidate(self, sdpmlineindex, candidate):
        '''
        '''
        self.webrtc.emit('add-ice-candidate', sdpmlineindex, candidate)


class WebSocketServer:
    def __init__(self, host, port, ssl_context):
        '''
        '''
        self.host = host
        self.port = port
        self.ssl_context = ssl_context
    
    def start(self):
        self.server = websockets.serve(self.accept, self.host, self.port, ssl=self.ssl_context)
        return self.server

    async def accept(self, conn, path):
        '''
        '''
        webrtc = None
        while True:
            message = await conn.recv()
            print('websocket accept thread:', threading.get_ident())
            jsondata = json.loads(message)
            if 'request' in jsondata:
                assert(jsondata['request']['type'] == 'offer')
                offer_sdp = jsondata['request']['sdp']
                
                webrtc = WebRTCServer(conn)
                answer_sdp = webrtc.setRemoteDescription(offer_sdp)

                res_msg = json.dumps({
                    'response' : {
                        'type' : 'answer',
                        'sdp' : answer_sdp,
                        }
                    }
                )
                await conn.send(res_msg)
            
            elif 'event' in jsondata:
                assert('type' in jsondata['event'])
                assert('candidate' in jsondata['event'])

                if jsondata['event']['type'] == 'ice':
                    candidate = jsondata['event']['candidate']
                    sdpMLineIndex = jsondata['event']['sdpMLineIndex']
                    webrtc.addIceCandidate(sdpMLineIndex, candidate)
                    


if __name__ == '__main__':
    print('main start thread:', threading.get_ident())
    Gst.init(None)
    if not check_plugins():
        sys.exit(1)
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--listen', help='listening address default:0.0.0.0')
    parser.add_argument('--port', help='listening port default:8999', default=8999)
    args = parser.parse_args()

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(certfile='./cert.pem', keyfile='./key.pem')
    server = WebSocketServer(args.listen, args.port, ssl_context)
    websock_server = server.start()

    # peer = WebRTCPeer(args.listen, args.port)
    # peer.pipeline()

    loop = asyncio.get_event_loop()
    loop.run_until_complete(websock_server)
    loop.run_forever()

    sys.exit()




