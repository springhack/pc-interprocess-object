/*
        Author: SpringHack - springhack@live.cn
        Last modified: 2018-10-31 15:07:13
        Filename: src/main.cc
        Description: Created by SpringHack using vim automatically.
 */
#include "main.h"

using namespace Nan;
using rw_mmap = mio::basic_mmap<mio::access_mode::write, char>;

// static std::map<std::string, std::uint32_t> lockMap;
static std::map<std::string, rw_mmap *> memMap;
static std::map<std::string, dosk::allocator *> allocatorMap;

void allocate_file(const std::string& path, const int size) {
  std::ofstream file(path, std::ios::binary | std::ios::out);
  file.seekp(size - 1);
  file.write("", 1);
}

void none_free_callback(char* data, void* hint) {
  (void)data;
  (void)hint;
}

void malloc_free_callback(char* data, void* hint) {
  dosk::allocator* allocator = (dosk::allocator *)hint;
  allocator->free(data);
}

void heap_free_callback(char* data, void* hint) {
  std::string hash((char *)hint);
  free(hint);
  rw_mmap* mmap = memMap[hash];
  if (!mmap) return;
  if (allocatorMap[hash]) {
    auto allocator = allocatorMap[hash];
    allocatorMap.erase(hash);
    delete allocator;
  }
  memMap.erase(hash);
  mmap->unmap();
  delete mmap;
}

NAN_METHOD(AllocHeap) {
  std::string path(*(Nan::Utf8String(info[0])));
  size_t size = info[1]->Uint32Value();
  allocate_file(path, size);
  auto hash = md5(path);
  auto hint = strdup(hash.c_str());
  rw_mmap* mmap = new rw_mmap(path);
  memMap[hash] = mmap;
  v8::Local<v8::Array> ret = New<v8::Array>();
  Nan::Set(
    ret,
    Nan::New<v8::Uint32>(0),
    Nan::New<v8::String>(hash.c_str()).ToLocalChecked()
  );
  Nan::Set(
    ret,
    Nan::New<v8::Uint32>(1),
    Nan::NewBuffer(mmap->data<mio::access_mode::write, char *>(), size, heap_free_callback, hint).ToLocalChecked()
  );
  info.GetReturnValue().Set(ret);
}

NAN_METHOD(FreeHeap) {
  std::string hash(*(Nan::Utf8String(info[0])));
  rw_mmap* mmap = memMap[hash];
  if (!mmap) return;
  memMap.erase(hash);
  if (allocatorMap[hash]) {
    auto allocator = allocatorMap[hash];
    allocatorMap.erase(hash);
    delete allocator;
  }
  mmap->unmap();
  delete mmap;
}

NAN_METHOD(FromOffset) {
  v8::Local<v8::Object> buffer = info[0]->ToObject();
  size_t offset = info[1]->Uint32Value();
  size_t length = info[2]->Uint32Value();
  v8::Local<v8::Object> ret = Nan::NewBuffer(node::Buffer::Data(buffer) + offset, length, none_free_callback, nullptr).ToLocalChecked();
  info.GetReturnValue().Set(ret);
}

NAN_METHOD(Malloc) {
  std::string hash(*(Nan::Utf8String(info[0])));
  size_t size = info[1]->Uint32Value();
  dosk::allocator* allocator = allocatorMap[hash];
  auto mmap = memMap[hash];
  auto pool = mmap->data();
  if (!allocator) {
    allocator = new dosk::allocator(hash, pool, mmap->size());
    allocatorMap[hash] = allocator;
  }
  void* buffer = allocator->malloc(size);
  v8::Local<v8::Object> ret_buffer = Nan::NewBuffer((char *)buffer, size, malloc_free_callback, allocator).ToLocalChecked();
  v8::Local<v8::Array> ret = New<v8::Array>();
  Nan::Set(
    ret,
    Nan::New<v8::Uint32>(0),
    Nan::New<v8::Uint32>((uint32_t)((size_t)buffer - (size_t)pool))
  );
  Nan::Set(
    ret,
    Nan::New<v8::Uint32>(1),
    ret_buffer
  );
  info.GetReturnValue().Set(ret);
}

NAN_METHOD(Lock) {
  // std::string hash(*(Nan::Utf8String(info[0])));
  // size_t begin = info[1]->Uint32Value();
  // size_t end = info[2]->Uint32Value();
  // std::string lock_hash = hash + "_" + std::to_string(begin) + "_" + std::to_string(end);
  // CubicleSoft::Sync::Mutex* mutex = new CubicleSoft::Sync::Mutex();
  // mutex->Lock(INFINITE);
  // info.GetReturnValue().Set(Nan::New<v8::String>(lock_hash.c_str()).ToLocalChecked());
}

NAN_METHOD(Unlock) {
  // std::string lock_hash(*(Nan::Utf8String(info[0])));
  // CubicleSoft::Sync::Mutex* mutex = new CubicleSoft::Sync::Mutex();
  // mutex->Unlock();
}

NAN_MODULE_INIT(Init) {
  Nan::Set(
    target,
    Nan::New<v8::String>("allocHeap").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(AllocHeap)->GetFunction()
  );
  Nan::Set(
    target,
    Nan::New<v8::String>("freeHeap").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(FreeHeap)->GetFunction()
  );
  Nan::Set(
    target,
    Nan::New<v8::String>("malloc").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(Malloc)->GetFunction()
  );
  Nan::Set(
    target,
    Nan::New<v8::String>("fromOffset").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(FromOffset)->GetFunction()
  );
  Nan::Set(
    target,
    Nan::New<v8::String>("lock").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(Lock)->GetFunction()
  );
  Nan::Set(
    target,
    Nan::New<v8::String>("unlock").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(Unlock)->GetFunction()
  );
}

NODE_MODULE(interprocess_object, Init)
