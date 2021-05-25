#include "dog.h"

struct _TestDog{
    TestAnimal parent;
};

G_DEFINE_TYPE (TestDog, test_dog, TEST_TYPE_ANIMAL)

void
test_dog_real_make_sound (TestDog * self);

void
test_dog_real_move (TestDog * self, gint x, gint y);

TestDog *
test_dog_new() {
    return g_object_new(TEST_TYPE_DOG, NULL);
}

void
test_dog_init(TestDog * self){
    
}

void
test_dog_class_init(TestDogClass * klass){
    TestAnimalClass * animal_class = TEST_ANIMAL_CLASS(klass);
    animal_class->make_sound = test_dog_real_make_sound;
    animal_class->move = test_dog_real_move;
}

void
test_dog_real_make_sound (TestDog * self) {
    g_print ("The Dog says \"awoo!\" \n");
}

void
test_dog_real_move (TestDog * self, gint x, gint y) {
    g_print ("The Dog runs around a bit and randomly ends up at (%i, %i)\n", x, y);
}
//dog make sound
//dog move

// dog init
// dog class init
