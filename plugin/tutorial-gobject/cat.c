#include "cat.h"

struct _TestCat {
    TestAnimal parent;
};

G_DEFINE_TYPE (TestCat, test_cat, TEST_TYPE_ANIMAL)

void
test_cat_real_make_sound (TestCat * self);

void
test_cat_real_move (TestCat * self, gint x, gint y);

TestCat *
test_cat_new(){
    return g_object_new(TEST_TYPE_CAT, NULL);
}

static void
test_cat_init(TestCat * self){

}

static void
test_cat_class_init(TestCatClass * klass){
    TestAnimalClass * animal_class = TEST_ANIMAL_CLASS(klass);
    animal_class->make_sound = test_cat_real_make_sound;
    animal_class->move = test_cat_real_move;
}

void
test_cat_real_make_sound (TestCat * self) {
    g_print("The cat say \"meew\" and \"rurururu\"\n");
}

void
test_cat_real_move (TestCat * self, gint x, gint y) {
    g_print("The cat silently moves to (%i, %i). \n", x, y);
}
