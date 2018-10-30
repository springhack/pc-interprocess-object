/*
        Author: SpringHack - springhack@live.cn
        Last modified: 2018-05-30 19:50:04
        Filename: src/callback_info.cc
        Description: Created by SpringHack using vim automatically.
 */
// https://raw.githubusercontent.com/ghostoy/node-ffi/master/src/callback_info.cc
// Reference:
//   http://www.bufferoverflow.ch/cgi-bin/dwww/usr/share/doc/libffi5/html/The-Closure-API.html

#include "callback_info.h"

#if !(NODE_VERSION_AT_LEAST(0, 11, 15))
  #ifdef WIN32
    int uv_thread_equal(const uv_thread_t* t1, const uv_thread_t* t2) {
      return *t1 == *t2;
    }
  #else
    #include <pthread.h>
    int uv_thread_equal(const uv_thread_t* t1, const uv_thread_t* t2) {
      return pthread_equal(*t1, *t2);
    }
  #endif
#endif

#ifdef WIN32
DWORD g_threadID;
#else
uv_thread_t g_mainthread;
#endif
uv_mutex_t    g_queue_mutex;
std::queue<ThreadedCallbackInvokation *> g_queue;
uv_async_t         g_async;

V8_CALLBACK doCallback = NULL;


int log_enable() {
  static int got = 0;
  static int enable = 0;
  if (got) return enable;
  got = 1;
  char* env = getenv("SDKW");
  if (env) enable = 1;
  return enable;
}


void DispatchToV8(void* data) {
  if (doCallback) {
    doCallback(data);
  }
}

void WatcherCallback(uv_async_t *w, int revents) {

  LOG("WatcherCallback got run");

  uv_mutex_lock(&g_queue_mutex);

  while (!g_queue.empty()) {
    ThreadedCallbackInvokation *inv = g_queue.front();
    g_queue.pop();

    DispatchToV8(inv->_data);
    inv->SignalDoneExecuting();
  }

  uv_mutex_unlock(&g_queue_mutex);
}


void Callback_Invoke(void* data) {

  LOG("Callback_Invoke");

  // are we executing from another thread?
#ifdef WIN32
  if (g_threadID == GetCurrentThreadId()) {
#else
  uv_thread_t self_thread = (uv_thread_t) uv_thread_self();
  if (uv_thread_equal(&self_thread, &g_mainthread)) {
#endif
    DispatchToV8(data);
    LOG("Callback from main");
  } else {
    // hold the event loop open while this is executing
#if NODE_VERSION_AT_LEAST(0, 7, 9)
    uv_ref((uv_handle_t *)&g_async);
#else
    uv_ref(uv_default_loop());
#endif

    // create a temporary storage area for our invokation parameters
    ThreadedCallbackInvokation *inv = new ThreadedCallbackInvokation(data);

    LOG("Callback from other");
    // push it to the queue -- threadsafe
    uv_mutex_lock(&g_queue_mutex);
    g_queue.push(inv);
    LOG("Queue adding callback");
    uv_mutex_unlock(&g_queue_mutex);
    LOG("Queue added callback");

    // send a message to our main thread to wake up the WatchCallback loop
    uv_async_send(&g_async);
    LOG("Waiting for call");
    // wait for signal from calling thread
    inv->WaitForExecution();
    LOG("Callback called");

#if NODE_VERSION_AT_LEAST(0, 7, 9)
    uv_unref((uv_handle_t *)&g_async);
#else
    uv_unref(uv_default_loop());
#endif
    delete inv;
  }
}

/*
 * Init stuff.
 */


void Callback_setV8Callback(V8_CALLBACK cb) {
  doCallback = cb;
}


void Callback_Initialize(v8::Handle<v8::Object> target) {
  Nan::HandleScope scope;

  // initialize our threaded invokation stuff
#ifdef WIN32
  g_threadID = GetCurrentThreadId();
#else
  g_mainthread = (uv_thread_t) uv_thread_self();
#endif
  uv_async_init(uv_default_loop(), &g_async, (uv_async_cb) WatcherCallback);
  uv_mutex_init(&g_queue_mutex);

  // allow the event loop to exit while this is running
#if NODE_VERSION_AT_LEAST(0, 7, 9)
  uv_unref((uv_handle_t *)&g_async);
#else
  uv_unref(uv_default_loop());
#endif
}
