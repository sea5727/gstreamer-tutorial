# Gstreamer

## mad

### From the GStreamer's 1.12 Changelog on plugins removal:

```
The mad mp1/mp2/mp3 decoder plugin was removed from gst-plugins-ugly, as libmad is GPL licensed, has been unmaintained for a very long time, and there are better alternatives available. Use the mpg123audiodec element from the mpg123 plugin in gst-plugins-ugly instead, or avdec_mp3 from the gst-libav module which wraps the ffmpeg library. We expect that we will be able to move mp3 decoding to gst-plugins-good in the next cycle seeing that most patents around mp3 have expired recently or are about to expire.
```
mad mp1/mp2/mp3 디코더 플러그인은 GPL 라이센스이기 으로 오랜기간 유지되지 않았기때문에 gst-plugins-ugly에서 지워졌습니다.  그리고 더나은 대안이 있습니다. 대신에 gst-plugins-ugly에 mpg123 plugin의 mpg123audiodec 엘리먼트를 사용하세요, 또는 ffmpeg library로 wrap된 gst-libav 모듈의 avdec_mp3을 사용하세요.
...

mad element가 제거되었다. avdev_mp3 을 사용하면 된다.

```
gst-launch-1.0 filesrc location="/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/file_example_MP3_2MG.mp3" ! mpegaudioparse ! avdec_mp3 ! audioconvert ! alsasink
```




