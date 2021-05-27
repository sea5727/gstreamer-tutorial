#include <glib.h>
#include <gst/gst.h>

void test_from_string_int(){
    const gchar *strings[] = {
        "structure1, bar1 = (int) 123456",
        "structure2, bar2 = (int) -123456",
        "structure3, bar3 = (int) 123456",
        "structure4, bar4 = (int) 123456",
        "structure5, bar5 = (int) 123456",
        "structure6, bar6 = (int) 123456",
        "structure7, bar7 = (int) 123456",
    };

    gint results[] = {
        123456,
        -123456,
        0xFFFF,
        0xFFFF,
        0x7FFFFFFF,
        (gint) 0x80000000,
        (gint) 0xFF000000,
        (gint) 0xFF000000,
    };

    GstStructure * structure;
    for(int i = 0 ; i < G_N_ELEMENTS(strings); ++i){
        const gchar * s = strings[i];
        const gchar * name;
        gint value;

        structure  = gst_structure_from_string(s, NULL);
        if(structure == NULL){
            g_print("Could not get structure frmo string %s\n", s);
        }
        const gchar * st_name = gst_structure_get_name(structure);
        name = gst_structure_nth_field_name(structure, 0);
        g_print("name:%s\n", name);
        gboolean ret = gst_structure_get_int(structure, name, &value);
        g_print("get_int ret:%d\n", ret);
        if(value != results[i]){
            g_print("Value %d is not expected result %d for string %s\n", value, results[i], s);
        }
        const GValue * value = gst_structure_get_value(structure, "bar7");
        gint v = g_value_get_int(value);
        g_print("v:%d\n", v);

    }
}


int main(int argc, char * argv[]){

    gst_init(&argc, &argv);
    test_from_string_int();

    return 0;
}