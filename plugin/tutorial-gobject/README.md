# gobject tutorial

https://developer.gnome.org/gobject/stable/gobject-Type-Information.html

G_DECLARE_FINAL_TYPE : 마지막으로 상속될 유형의 헤더파일에 선언하는 편리한 매크로
G_DECLARE_DERIVABLE_TYPE : 상속될수 있는 유형의 헤더파일에 선언하는 편리한 매크로
G_DECLARE_INTERFACE : 인터페이스 유형의 헤더파일에 선언하는 편리한 매크로
G_DEFINE_TYPE : 클래스 초기함수, 인스턴스 초기 함수 및 부모 클래스를 가리키는 정적 변수를 구현하는 편리한 매크로



TestAnimal 객체와
TestAnimalClass 클래스를 만드는 샘플입니다. 

객체는 ~~ 클래스는 ~~ 입니다.
``` c
typedef struct _TestAnimal TestAnimal;
typedef struct _TestAnimalClass TestAnimalClass;

#define TEST_TYPE_ANIMAL test_animal_get_type()
G_DECLARE_DERIVABLE_TYPE (TestAnimal,  , TEST, ANIMAL, GObject)
```

## G_DECLARE_DERIVABLE_TYPE

``` c
#define             G_DECLARE_DERIVABLE_TYPE(ModuleObjName, module_obj_name, MODULE, OBJ_NAME, ParentName)
```
`G_DECLARE_DERIVABLE_TYPE` 은 여러개의 `define`으로 이루어진 `파생 변수`들을 생성합니다.
`G_DECLARE_DERIVABLE_TYPE`문이 생성하는 파생 변수들은 아래 5가지 입니다.

``` c
#define TEST_ANIMAL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_ANIMAL, TestAnimal))
#define TEST_IS_ANIMAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_ANIMAL))
#define TEST_ANIMAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_ANIMAL, TestAnimalClass))
#define TEST_ANIMAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_ANIMAL))
#define TEST_ANIMAL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_ANIMAL, TestAnimalClass))

struct _TestAnimal {
    GObject padding_instance;
    gpointer padding[8];
};
```



### 주의점.

TestAnimal : 대문자 객체명
test_animal : 소문자 객체명
TEST : prefix,
ANIMAL : 이름값, 
GObject : 부모클래스


