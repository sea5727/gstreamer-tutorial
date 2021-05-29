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
webrtcbin name=sendrecv bundle-policy=max-bundle 
 videotestsrc is-live=true pattern=ball ! videoconvert ! queue name=q1 ! vp8enc deadline=1 ! rtpvp8pay !
 queue name=q2 ! application/x-rtp,media=video,encoding-name=VP8,payload=96 ! sendrecv.
 audiotestsrc is-live=true wave=red-noise ! audioconvert ! audioresample ! queue name=q4 ! opusenc ! rtpopuspay !
 queue name=q3 ! application/x-rtp,media=audio,encoding-name=OPUS,payload=111 ! sendrecv.
'''

# PIPELINE_DESC = '''
# webrtcbin name=sendrecv bundle-policy=max-bundle stun-server=stun://sanghotest.iptime.org
#  videotestsrc is-live=true pattern=ball ! videoconvert ! queue name=q1 ! vp8enc deadline=1 ! rtpvp8pay !
#  queue name=q2 ! application/x-rtp,media=video,encoding-name=VP8,payload=97 ! sendrecv.
#  audiotestsrc is-live=true wave=red-noise ! audioconvert ! audioresample ! queue name=q4 ! opusenc ! rtpopuspay !
#  queue name=q3 ! application/x-rtp,media=audio,encoding-name=OPUS,payload=96 ! sendrecv.
# '''
    
mainloop = None


class WebRTCServer:
    _id = 0
    def __init__(self, websocket, type):
        '''
        need a mutex?
        '''
        self.id = WebRTCServer._id
        WebRTCServer._id += 1
        self.websocket = websocket
        if type == 'sender':
            '''
            '''
            print('try sender.. ')
            self.pipe = Gst.parse_launch(PIPELINE_DESC)
            self.webrtc = self.pipe.get_by_name('sendrecv')
            self.webrtc.connect('on-negotiation-needed', self.onNegotiationNeeded, self.websocket)
            self.webrtc.connect('on-ice-candidate', self.onIceCandidate, self.websocket)
            self.webrtc.connect('pad-added', self.onPadAdded)
            self.pipe.set_state(Gst.State.PLAYING)
            print('try sender end..')
        elif type == 'receiver':
            self.pipe = Gst.parse_launch(PIPELINE_DESC)
            self.webrtc = self.pipe.get_by_name('sendrecv')
            self.webrtc.connect('on-ice-candidate', self.onIceCandidate, self.websocket)
            self.webrtc.connect('pad-added', self.onPadAdded)

            self.pipe.set_state(Gst.State.PLAYING)
    def startPipeline(self):
        '''
        '''
    async def testfunction(self, param):
        '''
        '''
        print('testfunction thread:', threading.get_ident(), 'param:', param)
        
    def onCreateOffer(self, promise, conn, _):
        '''
        '''
        print('onCreateOffer thread:', threading.get_ident())
        exit(0)
        promise.wait()
        
        reply = promise.get_reply()
        offer = reply.get_value('offer')
        
        promise = Gst.Promise.new()
        print('offer:', offer.sdp.as_text())
        self.webrtc.emit('set-local-description', offer, promise)
        
        promise.interrupt()

        offer_msg = json.dumps({
            'request' : {
                'type' : 'offer',
                'sdp' : offer.sdp.as_text(),
                }
            }
        )
        print('send offer_msg:', offer_msg)
        asyncio.run_coroutine_threadsafe(conn.send(offer_msg), loop=mainloop)
    def onNegotiationNeeded(self, element, conn):
        '''
        '''
        promise = Gst.Promise.new_with_change_func(self.onCreateOffer, conn, None)
        self.webrtc.emit('create-offer', None, promise)

        
        
    def onIceCandidate(self, element, sdpMLineIndex, candidate, conn):
        '''
        '''
        print('onIceCandidate thread:', threading.get_ident(), 'candidate:', candidate, 'sdpMLineIndex:', sdpMLineIndex)

        icemsg = json.dumps({'event': {'type': 'ice', 'candidate': { 'candidate': candidate, 'sdpMLineIndex': sdpMLineIndex}}})
        asyncio.run_coroutine_threadsafe(conn.send(icemsg), loop=mainloop)

    def onDecodebinPadAdded(self, _, pad):
        '''
        '''
        if not pad.has_current_caps():
            print (pad, 'has no caps, ignoring')
            return
        
        caps = pad.get_current_caps()

        for i in range(caps.get_size()):
            structure = caps.get_structure(i)
            name = structure.get_name()
            print('name:', name)
        
            if name == 'video/x-raw':
                print('on_incoming_decodebin_stream -> video')
                q = Gst.ElementFactory.make('queue')
                q.set_property('name', 'q5')
                conv = Gst.ElementFactory.make('videoconvert')
                sink = Gst.ElementFactory.make('fakesink')
                # sink = Gst.ElementFactory.make('autovideosink')
                self.pipe.add(q)
                self.pipe.add(conv)
                self.pipe.add(sink)
                self.pipe.sync_children_states()
                pad.link(q.get_static_pad('sink'))
                q.link(conv)
                conv.link(sink)

            elif name == 'audio/x-raw':
                print('on_incoming_decodebin_stream -> audio')
                q = Gst.ElementFactory.make('queue')
                conv = Gst.ElementFactory.make('audioconvert')
                resample = Gst.ElementFactory.make('audioresample')
                sink = Gst.ElementFactory.make('fakesink')
                # sink = Gst.ElementFactory.make('autoaudiosink')
                self.pipe.add(q)
                self.pipe.add(conv)
                self.pipe.add(resample)
                self.pipe.add(sink)
                self.pipe.sync_children_states()
                pad.link(q.get_static_pad('sink'))
                q.link(conv)
                conv.link(resample)
                resample.link(sink)
  

    def onPadAdded(self, element, pad):
        '''
        '''
        print('onPadAdded thread:', threading.get_ident())

        decodebin = Gst.ElementFactory.make('decodebin')
        decodebin.connect('pad-added', self.onDecodebinPadAdded)
        self.pipe.add(decodebin)
        self.webrtc.link(decodebin)
        decodebin.sync_state_with_parent()

    
    def onSetLocalDescription(self, promise, param1):
        '''
        need to send answer
        '''
        print('onSetLocalDescription thread:', threading.get_ident())

        promise.wait()
        reply = promise.get_reply()
        
    def setAnswer(self, sdp):
        '''
        '''
        res, sdpmsg = GstSdp.SDPMessage.new()
        GstSdp.sdp_message_parse_buffer(bytes(sdp.encode()), sdpmsg)
        answer = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.ANSWER, sdpmsg)
        promise = Gst.Promise.new()
        self.webrtc.emit('set-remote-description', answer, promise)
        promise.interrupt()
        print('set-remote-description end..')

    def setReceiver(self, offer_sdp):
        '''
        '''
        print('setReceiver thread:', threading.get_ident())
        result, offer = GstSdp.SDPMessage.new()
        assert(result == GstSdp.SDPResult.OK)
        GstSdp.sdp_message_parse_buffer(offer_sdp.encode(), offer)
        description = GstWebRTC.WebRTCSessionDescription.new(GstWebRTC.WebRTCSDPType.OFFER, offer)
        promise = Gst.Promise.new()
        self.webrtc.emit('set-remote-description', description, promise)
        promise.interrupt()


        direction_audio = GstWebRTC.WebRTCRTPTransceiverDirection.SENDRECV
        caps_audio = Gst.caps_from_string("application/x-rtp,media=audio,encoding-name=OPUS,payload=111")
        direction_video = GstWebRTC.WebRTCRTPTransceiverDirection.SENDRECV
        caps_vedio = Gst.caps_from_string("application/x-rtp,media=video,encoding-name=VP8,payload=96")
        
        self.webrtc.emit('add-transceiver', direction_audio, caps_audio)
        self.webrtc.emit('add-transceiver', direction_video, caps_vedio)


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
        print('addIceCandidate thread:', threading.get_ident())
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
                assert('type' in jsondata['request'])
                if jsondata['request']['type'] == 'offer':
                    offer_sdp = jsondata['request']['sdp']
                    print('[recv] offer:', offer_sdp)
                    
                    webrtc = WebRTCServer(conn, 'receiver')
                    answer_sdp = webrtc.setReceiver(offer_sdp)

                    res_msg = json.dumps({
                        'response' : {
                            'type' : 'answer',
                            'sdp' : answer_sdp,
                            }
                        }
                    )
                    print(f'send ==> answer sdp:\n{answer_sdp}')
                    await conn.send(res_msg)
                elif jsondata['request']['type'] == 'needCall':
                    '''
                    '''

                    webrtc = WebRTCServer(conn, 'sender')
                    asyncio.ensure_future(webrtc.testfunction('1'), loop=mainloop)
                    # webrtc.setLocalDescription()
            elif 'response' in jsondata:
                assert('type' in jsondata['response'])
                
                if jsondata['response']['type'] == 'answer':
                    answer_sdp = jsondata['response']['sdp']
                    print(f'recv <== answer sdp :' , answer_sdp)

                    webrtc.setAnswer(answer_sdp)



                    
            elif 'event' in jsondata:
                assert('type' in jsondata['event'])
                assert('candidate' in jsondata['event'])

                if jsondata['event']['type'] == 'ice':
                    candidate = jsondata['event']['candidate']
                    sdpMLineIndex = jsondata['event']['sdpMLineIndex']
                    print(f'recv ==> sdpMLineIndex: {sdpMLineIndex} candidate: {candidate}')
                    webrtc.addIceCandidate(sdpMLineIndex, candidate)
                    


if __name__ == '__main__':
    print('main start thread:', threading.get_ident())
    Gst.init(None)

    # my_pipeline = Gst.Pipeline.new("testpipeline")
    # audiotestsrc = Gst.ElementFactory.make('audiotestsrc')
    # udpsink = Gst.ElementFactory.make('udpsink')
    # udpsink.set_property("host", '192.168.0.192')
    # udpsink.set_property('port', 12345)
    # my_pipeline.add(audiotestsrc)
    # my_pipeline.add(udpsink)
    # audiotestsrc.link(udpsink)
    # my_pipeline.set_state(Gst.State.PLAYING)


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

    mainloop = asyncio.get_event_loop()
    mainloop.run_until_complete(websock_server)
    mainloop.run_forever()

    sys.exit()




