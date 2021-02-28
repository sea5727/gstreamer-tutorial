#include <iostream>
#include <gio/gnetworking.h>

int main(int argc, char * argv[]){

    auto io_stdin = g_io_channel_unix_new(fileno(stdin));

    g_io_channel_set_flags(io_stdin, G_IO_FLAG_NONBLOCK, NULL);

    gchar *line = NULL;
    while (1) {
        GIOStatus s = g_io_channel_read_line (io_stdin, &line, NULL, NULL, NULL);
        if (s == G_IO_STATUS_AGAIN){
            g_usleep (100000); // 100ms
        } else if (s == G_IO_STATUS_NORMAL) {
            g_print("line:%s\n", line);
            g_free (line);
            fflush (stdout);
        } else {
            g_print("s:%d\n", s);
        }
    }
    return 0;


}