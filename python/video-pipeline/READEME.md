# Video Pipeline

## qtdemux 
mp4 컨테이너를 audio 와 video로 분리  
connect 에 add-paded 핸들러로 dynamic pad가 생성되고, 다음 pipeline의 element에 연결해야 한다.  
* audio 는 queue가 없으면 나오지 않았다.  

* template 명은
아래처럼 %u로 로 나온다.
```
SRC template: 'video_%u'
   Availability: Sometimes
   Capabilities:
     ANY

 SRC template: 'audio_%u'
   Availability: Sometimes
   Capabilities:
     ANY

 SRC template: 'subtitle_%u'
   Availability: Sometimes
   Capabilities:
     ANY
```