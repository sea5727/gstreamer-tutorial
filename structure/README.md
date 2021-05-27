# gststructure

`key-value` 형태의 `container` 입니다. 내부적으로 ~~로 구현되어있나 궁금하다.  

키는 GQuarks 로 표현되며 값은 모든 GType이 될 수 있습니다.

`gststructure`는 `name`을 가집니다.

## serialization format (직렬화 포맷)

"<stcuture-name>, <key1>=<value1>, <key2>=<value2> ... " 형태로 작성할 수 있습니다.

## gquark
GQuark 는 `string` <-> `integer` 간에 식별자입니다.

`string` 으로 `integer`를 할당하여 사용할 수 있고, `integer`로 `string`을 얻을 수 있습니다. 
내부적으로 `hashmap`으로 strinrg과 integer간에 매핑관계를 유지하고 있습니다.

