#include <uv.h>
#include <node.h>
#include <nan.h>
#include <stdlib.h>
#include <node_buffer.h>
#include <node_version.h>

int log_enable();

#define LOGGER
#ifdef LOGGER
  #ifdef WIN32
    #define LOG(fmt, ...) \
    do { \
      if (!log_enable()) break; \
      char _fmtstr[1024] = { "[SDK WRAPPER] ====> " }; \
      strcat(_fmtstr, fmt); \
      strcat(_fmtstr, "\n"); \
      fprintf(stderr, _fmtstr, __VA_ARGS__); \
    } while (0);
  #else
    #define LOG(fmt, args...) \
    do { \
      if (!log_enable()) break; \
      char _fmtstr[1024] = { "[SDK WRAPPER] ====> " }; \
      strcat(_fmtstr, fmt); \
      strcat(_fmtstr, "\n"); \
      fprintf(stderr, _fmtstr, ##args); \
    } while (0);
  #endif
#else
  #define LOG
#endif


typedef void (*V8_CALLBACK)(void *);

void Callback_Invoke(void* data);
void Callback_Initialize(v8::Handle<v8::Object> target);
void Callback_setV8Callback(V8_CALLBACK cb);

class ThreadedCallbackInvokation {
  public:
    ThreadedCallbackInvokation(void* data);
    ~ThreadedCallbackInvokation();

    void SignalDoneExecuting();
    void WaitForExecution();

    void* _data;

  private:
    uv_cond_t m_cond;
    uv_mutex_t m_mutex;
};
