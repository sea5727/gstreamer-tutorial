### SRTP

## client
```
gst-launch-1.0 filesrc location=/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/file_example_MP3_2MG.mp3 ! mpegaudioparse ! rtpmpapay ! "application/x-rtp, payload=(int)96, ssrc=(uint)1356955624" ! srtpenc key=987654321001234567890123456789012345678901234567890123456789 rtp-cipher=aes-128-icm rtp-auth=hmac-sha1-80 rtcp-cipher=aes-128-icm rtcp-auth=hmac-sha1-80 ! udpsink host=127.0.0.1 port=5000
```

## server
```
gst-launch-1.0 udpsrc port=5000 caps="application/x-srtp, encoding-name=(string)MPA, payload=(int)96, ssrc=(uint)1356955624, srtp-key=(buffer)987654321001234567890123456789012345678901234567890123456789, srtp-cipher=(string)aes-128-icm, srtp-auth=(string)hmac-sha1-80, srtcp-cipher=(string)aes-128-icm, srtcp-auth=(string)hmac-sha1-80, roc=(uint)0" ! srtpdec ! rtpjitterbuffer ! rtpmpadepay ! decodebin ! audioconvert ! alsasink
```
98:76:54:32:10:01:23:45:67:89:01:23:45:67:89:01:23:45:67:89:01:23:45:67:89:01:23:45:67:89
DC:12:D4:78:7A:64:D7:E8:50:0C:FF:3C:7C:CF:0F:AA:1A:D0:09:AD:88:2F:B8:BD:18:6C:A9:BF:BD:31:E0:1E
FF:FF:FF:B1:3F:82:18:3B:54:02:12:DF:3E:5D:49:6B:19:E5:7C:AB
sha-1 
B9:0F:89:EE:BD:1F:B1:C4:86:B6:D7:5C:25:88:53:F4:02:F4:F5:91
5A:70:05:55:C1:5A:82:51:02:D3:00:A3:BF:E7:EF:62:DF:29:EB:F2:9F:5F:51:58:12:D9:4C:AA:41:36:86:13