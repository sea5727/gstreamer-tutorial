
`gobject.h` 파일에 있는 `_GObjectClass` 객체의 메소드들을 초기화 해주어야 한다.

`set_property`, `get_property`


----

`gstpad.h` 파일에 있는 `gst_pad_set_chain_function` 를 사용하여 pad의 이벤트를 등록해주어야 한다.


### GST_PLUGIN_DEFINE
이 플러그인이 다른 응용 프로그램이 사용할 수 있도록 내보내는 매크로
``` c
#define GST_PLUGIN_DEFINE(major,minor,name,description,init,version,license,package,origin)
```
name : library 파일이름과 동일해야합니다.
libgstmyplugin.so 라면 name은 myplugin 이어야 외부에서 인식을 합니다.

예 : 
``` c
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    plugin,
    "Template plugin",
    plugin_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)

```