/*
        Author: SpringHack - springhack@live.cn
        Last modified: 2018-05-30 19:49:09
        Filename: src/threaded_callback_invokation.cc
        Description: Created by SpringHack using vim automatically.
 */
#include <node.h>
#include "callback_info.h"

ThreadedCallbackInvokation::ThreadedCallbackInvokation(void* data) {
  _data = data;

  uv_mutex_init(&m_mutex);
  uv_mutex_lock(&m_mutex);
  uv_cond_init(&m_cond);
}

ThreadedCallbackInvokation::~ThreadedCallbackInvokation() {
  uv_mutex_unlock(&m_mutex);
  uv_cond_destroy(&m_cond);
  uv_mutex_destroy(&m_mutex);
}

void ThreadedCallbackInvokation::SignalDoneExecuting() {
  uv_mutex_lock(&m_mutex);
  uv_cond_signal(&m_cond);
  uv_mutex_unlock(&m_mutex);
}

void ThreadedCallbackInvokation::WaitForExecution() {
  uv_cond_wait(&m_cond, &m_mutex);
}
