#include <glib.h>
#include <gst/gst.h>



struct event_queue
{
  GMutex lock;
  GCond cond;
  GThread *thread;
  GMainContext *main_context;
  GMainLoop *main_loop;
  gpointer user_data;
};

// void
// test_reply(){
//     GstPromise * r = gst_promise_new();
//     gst_promise_reply(r, NULL);
//     g_assert_true(gst_promise_wait(r) == GST_PROMISE_RESULT_REPLIED);
//     gst_promise_unref(r);
// }

// void
// test_reply_data(){
//     GstPromise * r = gst_promise_new();
//     // GstStructure * s = NULL;
//     const GstStructure * ret;
//     GstStructure *s = gst_structure_new("promise", "test", G_TYPE_INT, 1, NULL);
//     GstStructure *s2 = gst_structure_new("promise", "test", G_TYPE_INT, 1, NULL);
//     gst_promise_reply(r, gst_structure_copy (s));

//     g_assert_true(gst_promise_wait(r) == GST_PROMISE_RESULT_REPLIED);

    
//     ret = gst_promise_get_reply(r);

//     gst_promise_unref(r);
//     gst_structure_free(s);
// }

struct stress_item
{
  struct event_queue *q;
  GstPromise *promise;
  GstPromiseResult result;
};


struct stress_queues
{
  GAsyncQueue *push_queue;
  GAsyncQueue *wait_queue;
  guint64 push_count;
};


static gboolean
_wait_promise (struct event_queue *q)
{
  pthread_t self = pthread_self();
  
  struct stress_queues *s_q = q->user_data;
  struct stress_item *item;
  g_print("%s/%lu]==_wait_promise start and try g_async_queue_pop\n", __func__, self);
  item = g_async_queue_pop (s_q->wait_queue);
  g_print("pop??\n");
  g_print("%s/%lu]==item:%p\n", __func__, self, item);

  if (item == (void *) 1){
    return G_SOURCE_REMOVE;
  }
    

  GstPromiseResult ret = gst_promise_wait (item->promise);
  g_print("%s/%lu]==gst_promise_wait?%d\n", __func__, self, ret);
  g_assert_true (ret == item->result);

  gst_promise_unref (item->promise);
  g_free (item);

  return G_SOURCE_CONTINUE;
}


static void
stress_reply (struct stress_item *item)
{
  switch (item->result) {
    case GST_PROMISE_RESULT_REPLIED:
      gst_promise_reply (item->promise, NULL);
      break;
    case GST_PROMISE_RESULT_INTERRUPTED:
      gst_promise_interrupt (item->promise);
      break;
    case GST_PROMISE_RESULT_EXPIRED:
      gst_promise_expire (item->promise);
      break;
    default:
      g_assert_not_reached ();
  }
}

static gboolean
_pop_promise (struct event_queue *q)
{
  g_print("_pop_promise start\n");
  struct stress_queues *s_q = q->user_data;
  struct stress_item *item;

  item = g_async_queue_pop (s_q->push_queue);

  if (item == (void *) 1)
    return G_SOURCE_REMOVE;

  stress_reply (item);

  return G_SOURCE_CONTINUE;
}


static void
event_queue_stop (struct event_queue *q)
{
  g_mutex_lock (&q->lock);
  if (q->main_loop)
    g_main_loop_quit (q->main_loop);
  g_mutex_unlock (&q->lock);
}

static void
event_queue_stop_wait (struct event_queue *q)
{
  g_mutex_lock (&q->lock);
  while (q->main_loop) {
    g_main_loop_quit (q->main_loop);
    g_cond_wait (&q->cond, &q->lock);
  }
  g_mutex_unlock (&q->lock);

  g_thread_unref (q->thread);
}

static void
event_queue_free (struct event_queue *q)
{
  event_queue_stop_wait (q);

  g_mutex_clear (&q->lock);
  g_cond_clear (&q->cond);

  GST_LOG ("stopped event queue %p", q);

  g_free (q);
}
static gboolean
_unlock_thread (GMutex * lock)
{
  pthread_t self = pthread_self();
  g_print("%lu] _unlock_thread start\n", self);
  g_mutex_unlock (lock);
  return G_SOURCE_REMOVE;
}

// // static gboolean
// // test_timeout_callback2 (gpointer data){
// //   pthread_t self = pthread_self();
// //   g_print("%s/%lu] test_timeout_callback2 start\n", __func__, self);

// //   return FALSE;
// // }
// // static gboolean
// // test_timeout_callback (gpointer data){
// //   pthread_t self = pthread_self();
// //   g_print("%s/%lu] test_timeout_callback start\n", __func__, self);

// //   g_main_context_invoke (NULL, (GSourceFunc) test_timeout_callback2, NULL);
// //   return FALSE;
// // }
static gpointer
_promise_thread (struct event_queue *q)
{
  pthread_t self = pthread_self();
  g_print("%lu] _promise_thread start\n", self);
  g_mutex_lock (&q->lock);
  q->main_context = g_main_context_new ();
  q->main_loop = g_main_loop_new (q->main_context, FALSE);

  g_cond_broadcast (&q->cond);
  g_main_context_invoke (q->main_context, (GSourceFunc) _unlock_thread, &q->lock);

  g_print("%lu] _promise_thread run..\n", self);
  g_main_loop_run (q->main_loop);

  g_mutex_lock (&q->lock);
  g_main_context_unref (q->main_context);
  q->main_context = NULL;
  g_main_loop_unref (q->main_loop);
  q->main_loop = NULL;
  g_cond_broadcast (&q->cond);
  g_mutex_unlock (&q->lock);
  
  g_print("%s/%lu] _promise_thread end\n", __func__, self);
  return NULL;
}

static void
event_queue_start (struct event_queue *q)
{
  pthread_t self = pthread_self();
  g_print("%lu] event_queue_start start.. \n", self);
  g_mutex_lock (&q->lock);
  q->thread = g_thread_new ("promise-thread", (GThreadFunc) _promise_thread, q);

  while (!q->main_loop){
    g_cond_wait (&q->cond, &q->lock);
  }
  
  g_mutex_unlock (&q->lock);
}
static struct event_queue *
event_queue_new (void)
{
  struct event_queue *q = g_new0 (struct event_queue, 1);

  GST_LOG ("starting event queue %p", q);

  g_mutex_init (&q->lock);
  g_cond_init (&q->cond);
  event_queue_start (q);

  return q;
}

static void
_enqueue_task (struct event_queue *q, GSourceFunc func, gpointer data, GDestroyNotify notify)
{
  pthread_t self = pthread_self();
  g_print("%lu] _enqueue_task start\n", self);
  GSource *source;

  source = g_idle_source_new ();
  g_source_set_priority (source, G_PRIORITY_DEFAULT);
  g_source_set_callback (source, (GSourceFunc) func, data, notify);
  g_print("%lu]_enqueue_task g_source_attach??\n", self);
  g_source_attach (source, q->main_context);
  g_print("%lu]_enqueue_task g_source_attach end\n", self);
  g_source_unref (source);
}


static void
_push_stop_promise (struct event_queue *q)
{
  g_print("_push_stop_promise start\n");
  struct stress_queues *s_q = q->user_data;
  gpointer item = GINT_TO_POINTER (1);

  g_async_queue_push (s_q->wait_queue, item);
  g_async_queue_push (s_q->push_queue, item);
}

static gboolean
_push_random_promise (struct event_queue *q)
{
  pthread_t self = pthread_self();
  g_print("%lu] _push_random_promise start\n", self);
  struct stress_queues *s_q = q->user_data;
  struct stress_item *item;

  item = g_new0 (struct stress_item, 1);
  item->promise = gst_promise_new ();
  
  while (item->result == GST_PROMISE_RESULT_PENDING){
    item->result = g_random_int () % 4;
  }
    
  g_print("g_async_queue_push 1?\n");
  g_async_queue_push (s_q->wait_queue, item);
  g_print("g_async_queue_push 2?\n");
  g_async_queue_push (s_q->push_queue, item);
  g_print("g_async_queue_push end?\n");

  s_q->push_count++;

  g_print("%lu] _push_random_promise G_SOURCE_CONTINUE\n", self);
  return G_SOURCE_CONTINUE;
}


static void 
test_stress(){
  pthread_t self = pthread_self();
  g_print("%s/%lu] test_stress start \n", __func__, self);

    #define N_QUEUES 3
  struct event_queue *pushers[N_QUEUES];
  struct event_queue *poppers[N_QUEUES];
  struct event_queue *waiters[N_QUEUES];
  struct stress_queues s_q = { 0, };
  int i;

  s_q.push_queue = g_async_queue_new ();
  s_q.wait_queue = g_async_queue_new ();

  for (i = 0; i < N_QUEUES; i++) {
    pushers[i] = event_queue_new ();
    pushers[i]->user_data = &s_q;
    g_print("%lu] _enqueue_task -> _push_random_promise\n", self);
    
    _enqueue_task (pushers[i], (GSourceFunc) _push_random_promise, pushers[i], NULL);

    waiters[i] = event_queue_new ();
    waiters[i]->user_data = &s_q;
    // _enqueue_task (waiters[i], (GSourceFunc) _wait_promise, waiters[i], NULL);

        return;
    // poppers[i] = event_queue_new ();
    // poppers[i]->user_data = &s_q;
    // _enqueue_task (poppers[i], (GSourceFunc) _pop_promise, poppers[i], NULL);
  }

  GST_INFO ("all set up, waiting.");
  g_usleep (100000);
  GST_INFO ("wait done, cleaning up the test.");

  {
    struct stress_item *item;
    int push_size;

    for (i = 0; i < N_QUEUES; i++) {
      event_queue_stop (pushers[i]);
      event_queue_stop (poppers[i]);
      event_queue_stop (waiters[i]);
      _push_stop_promise (pushers[i]);
    }

    for (i = 0; i < N_QUEUES; i++) {
      event_queue_free (pushers[i]);
      event_queue_free (poppers[i]);
    }

    push_size = g_async_queue_length (s_q.push_queue);

    /* push through all the promises so all the waits will complete */
    while ((item = g_async_queue_try_pop (s_q.push_queue))) {
      if (item == (void *) 1)
        continue;
      stress_reply (item);
    }

    for (i = 0; i < N_QUEUES; i++)
      event_queue_free (waiters[i]);

    GST_INFO ("pushed %" G_GUINT64_FORMAT ", %d leftover in push queue, "
        "%d leftover in wait queue", s_q.push_count, push_size,
        g_async_queue_length (s_q.wait_queue));

    while ((item = g_async_queue_try_pop (s_q.wait_queue))) {
      if (item == (void *) 1)
        continue;

        
      g_assert_true (gst_promise_wait (item->promise) == item->result);

      gst_promise_unref (item->promise);
      g_free (item);
    }
  }

  g_async_queue_unref (s_q.push_queue);
  g_async_queue_unref (s_q.wait_queue);
}
// void
// test_reply_immutable(){
//     GstPromise * r;
//     GstStructure * s, * ret;

//     r = gst_promise_new();

//     s = gst_structure_new ("promise", "test", G_TYPE_INT, 1, NULL);
//     gst_promise_reply(r, s);
//     ret = (GstStructure *)gst_promise_get_reply(r);
//     gst_structure_set(ret, "foo", G_TYPE_STRING, "bar", NULL);
//     const gchar * v = gst_structure_get_string(ret, "foo");

//     g_print("foo:%s\n", v);

//     gst_promise_unref(r);
// }

// void
// test_expire(){
//     GstPromise * r;
//     r = gst_promise_new();
//     gst_promise_expire(r);

//     GstPromiseResult ret = gst_promise_wait(r);
//     if(ret == GST_PROMISE_RESULT_EXPIRED){
//         g_print("success\n");
//     }
    

//     gst_promise_ref(r);
// }


// struct change_data {
//     int change_count;
//     GstPromiseResult result;
// };

// static void
// on_change(GstPromise * promise, gpointer user_data){
//     g_print("on_change start\n");
//     struct change_data * res = user_data;

//     res->result = gst_promise_wait(promise);
//     res->change_count += 1;
//     g_print("on_change count:%d\n", res->change_count);
// }

// void
// test_change_func(){
//     GstPromise * r;
//     struct change_data data = {0, };

//     r = gst_promise_new_with_change_func(on_change, &data, NULL);
//     gst_promise_reply (r, NULL);
    
//     g_print("test_change_func end..data.result:%d, count:%d\n", 
//         data.result, data.change_count);
// }
int main(int argc, char * argv[]){
    pthread_t self = pthread_self();
    g_print("%s/%lu] main start \n", __func__, self);


    // gst_init(argc, &argv);

    // test_reply();
    // test_reply_data();
    // test_reply_immutable();
    // test_expire();
    // test_change_func();

    test_stress();

    g_print("main loop start\n");

    // g_main_loop_run(loop);

    g_usleep(1000 * 1000 * 30);

    g_print("main end\n");

    return 0;
}