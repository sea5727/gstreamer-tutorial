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
        Date: Mon, 22 Mar 2021 13:31:15 GMT

C->S:   SETUP rtsp://sanghgotest.iptime.org/test/stream=1 RTSP/1.0
        CSeq:5
        User-AGent: LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)
        Transport: RTP/AVP;unicast;client_port=60518-60519
        Sesion: jLGJXzv3eUw3ccSl
S->C:   RTSP/1.0 200 OK
        CSeq:4
        Transport: RTP/AVP;unicast;client_port=60518-60519;server_port=43570-43571;ssrc=346C724;mode="PLAY"
        Server: GStreamer RTSP server
        Session: jLGXzv3eUwRccSl
        Date: Mon, 22 Mar 2021 13:31:15 GMT

 ```

 ### PLAY
 - 미디어 스트림을 재생한다. 범위를 지정하지 않으면 처음부터 끝까지 재생, 일시 중지된 상황이라면 중지도니 지점부터 이어서 재생

 ```
 C->S:  PLAY rtsp://sanghotest.iptime.org:12345/test/ RTSP/1.0
        CSeq:6
        User-Agent:LibVLC/3.0.8 (LIVE555 Streaming Media v2016.11.28)
        Session: -SPyTmrMF_o-2JB
        Range: npt=0.000-
또는 pause 후 재생시 Rage: npt=324.667
 S->:   RTSP/1.0 200 OK
        CSeq:6
        RTP-Info: url=rtsp://sanghotest.iptime.org:12345/test/stream=0;seq=22423;rtptime=2551474179, url=rtsp://sanghotest.iptime.org:12345/test/stream=1;seq=23462;rtptime=1385494271
        Range: npt=0-3146.002
        Server: GStreamer RTSP server
        Session: .-SPyTmRMF_o-2JB
        Date: Mon, 22 Mar 2021 14:28:57 GMT
 ```
 ### TEARDOWN

 ### PAUSE


