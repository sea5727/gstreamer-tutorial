# RTSP 

RTSP : Real Time Streaming Protocol
 - 스트리밍 미디어 서버를 컨트롤 하기 위해 고안된 프로토콜 ( RFC2326)
  - PLAY, PAUSE 와 같은 동작으로 서버에 접근.
  - 실제 미디어는 RTP로 전송한다 ( TCP )


### OPTIONS

OPTIONS 요청은 서버가 응답할 타입을 반환한다 ( PLAY, PAUSE 등) 반환한다.
```
1.  c -> S OPTIONS rtsp://sanghotest.iptime.org:12345/test
    CSeq : 1

2.  S -> C RTSP/1.0 200OK
    CSeq : 1
    Public : OPTIONS, DESCRIBE, ANNOUNCE, GET_PARAMETER, PAUSE, PLAY, RECORD, SETUP, SET_PARAMETER, TEARDOWN
```


### DESCRIBE 
DESCRIBE 요청은 URL을 보내 sdp를 수신받는다.


```
C->S: DESCRIBE rtsp://sanghotest.iptime.org/test RTSP/1.0
      CSeq: 3
      User-Agent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)
      Accept: application/sdp

S->C: RTSP/1.0 200 OK
      CSeq: 3
      Content-Type: application/sdp
      Content-Base: rtsp://sanghotest.iptime.org/test
      Server: GStreamer RTSP server 
      Date: Mon, 22 Mar 2021 13:31:14 GMT
      Content-Length:661
      

      v=0
      o=- 119 75779673297454952 1 IN IP4 192.168.0.34
      s=Session stream with GStreamer
      i=rtsp-server
      t=0 0
      a=tool:GStreamer
      a=type:broadcast
      a=control:*
      a=range:npt=0-3146.002
      m=video 0 RTP/AVP 96
      c=IN IP4 0.0.0.0 
      b=AS:118
      a=rtpmap:96 H264/90000 
      a=framerate:30 
      a=fmtp:96 packetization-mode=1;profile-level-id=4d401f;sprop-parameter-sets=Z01AH+iAbB7zeAtQEBAUAAADAAQAAAM8DxgxEg=,aOvvIA==
      a=control:stream=0
      a:ts-refclk:locak
      a=mediaclk:sender
      m=audio 0 RTP/AVP 97
      c=IN IP4 0.0.0.0
      b=AS:127
      a=rtpmap:97 MP4A-LATM/44100
      a=fmtp:97 cpresent=0;config=400024200000000000000000000000000000
      a=control:stream=1
      a=ts-reflck:local
      a=mediaclk:sender
```

### SETUP
 - 이 요청이 수행된 후에야 PLAY 요청을 전달 할 수 있다.
 - 스트림이 어떻게 전송되어야 하는지를 규정

 ```
 C->S:  SETUP rtsp://sanghotest.iptime.org/test/stream=0 RTSP/1.0
        CSeq:4
        User-Agent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)
        Transport: RTP/AVP;unicast;client_port=60516-60517
 S->C:  RTSP/1.0 200 OK
        CSeq:4
        Transport: RTP/AVP;unicast;client_port=60516-60517;server_port=47588-47589;ssrc=8A201DD0;mode="PLAY"
        Server: GStreamer RTSP server
        Session: jLGXzv3eUwRccSl
        Date: Mon, 22 Mar 2021 13:31:15  GMT
C->S:   SETUP rtsp://sanghgotest.iptime.org/test/stream=1 RTSP/1.0
        CSeq:5
        User-AGent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)
        Transport: RTP/AVP;unicast;client_port=60518-60519
        Sesion: jLGJXzv3eUw3ccSl


 ```

 
