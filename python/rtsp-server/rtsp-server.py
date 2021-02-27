import sys
import gi
gi.require_version('Gst', '1.0')
gi.require_version('GstRtspServer', '1.0')
# gi.require_version('GObject', '3.0')
from gi.repository import Gst, GstRtspServer, GObject

class Factory(GstRtspServer.RTSPMediaFactory):
    def __init__(self):
        GstRtspServer.RTSPMediaFactory.__init__(self)
    
    
    def do_create_element(self, url):
        file = '/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/480p.mp4'
        spec = f' filesrc location={file} ! qtdemux name=demux ' \
                'demux.video_0 ! queue ! rtph264pay pt=96 name=pay0 ' \
                'demux.audio_0 ! queue ! rtpmp4apay pt=97 name=pay1 ' 
        return Gst.parse_launch(spec)


class RtspServer(object):
    def __init__(self):
        '''
        '''
        self.server = GstRtspServer.RTSPServer()
        self.server.set_service('12345')

        factory = Factory()
        factory.set_shared(True)
        m = self.server.get_mount_points()
        m.add_factory('/test', factory)
        self.server.attach(None)

        

if __name__ == '__main__':

    loop = GObject.MainLoop()
    GObject.threads_init()
    Gst.init(None)

    rtspserver = RtspServer()

    loop.run()