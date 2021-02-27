# Capabilities

Capabilities, Gst.Caps, is a container where you may store information that you may pass on to a Gst.PadTemplate. When you set the pipeline state to either playing or paused the elements pads negotiates what caps to use for the stream. Now the following pipeline works perfectly:

```
gst-launch-1.0  videotestsrc ! video/x-raw, width=320, height=240 ! xvimagesink
```

But if you try to switch out the xvimagesink for an ximagesink you will notice that it wouldn't work. That's because ximagesink can't handle video/x-raw so you must put in an element BEFORE in the pipeline that does.

```
gst-launch-1.0  videotestsrc ! video/x-raw, width=320, height=240 ! videoconvert ! ximagesink
```
And as ximagesink does not support hardware scaling you have to throw in a videoscale element too if you want software scaling.

```
gst-launch-1.0  videotestsrc ! video/x-raw, width=320, height=240 ! videoscale ! videoconvert ! ximagesink
```

To put the above examples in code you have to put the caps in a capsfilter element.