#include "animal.h"
#include "cat.h"
#include "dog.h"

int main(int argc, char * argv[]){
    TestAnimal * a = test_animal_new();
    TestCat * b = test_cat_new();
    TestDog * c = test_dog_new();

    test_animal_make_sound(a);
    test_animal_move(a, 5, 6);

    test_animal_make_sound(b);
    test_animal_move(b, 55, 65);

    test_animal_make_sound(TEST_ANIMAL(b));
    test_animal_move(TEST_ANIMAL(b), 5, 6);

    test_animal_make_sound(c);
    test_animal_move(c, 100, 1000);

    g_object_unref(a);
    g_object_unref(b);
    g_object_unref(c);

    return 0;
}
