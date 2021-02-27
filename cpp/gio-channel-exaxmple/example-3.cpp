#include <glib.h>
#include <unistd.h>
#include <stdio.h>

int fd[2];

void *thread_function(void *data)
{
	// call sleep so that the main loop source is not ready immediately
	sleep(10);
	
	write(fd[1],"GIOChannel main loop example",29);
	return NULL;
}

gboolean my_callback(GIOChannel *source,GIOCondition condition,gpointer data)
{
	char buf[100];	
	
	read(fd[0],buf,sizeof(buf));
	g_print("%s",buf);
	
	getchar();
	
	g_main_loop_quit((GMainLoop *)data);
	
	return FALSE;
}

int main()
{
	GMainLoop *loop;
	GIOChannel *channel;
	
	pipe(fd);
	
	channel = g_io_channel_unix_new(fd[0]);
	
	g_thread_init(NULL);
	g_thread_create(thread_function,NULL,TRUE,NULL);
	
	loop = g_main_loop_new(NULL,FALSE);
	g_io_add_watch(channel,G_IO_IN | G_IO_HUP | G_IO_ERR,(GIOFunc)my_callback,loop);
	g_main_loop_run(loop);
	
	g_io_channel_shutdown(channel,TRUE,NULL);

	return 0;
}