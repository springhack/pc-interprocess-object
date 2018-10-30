#include "main.h"

using namespace Nan;
using rw_mmap = mio::basic_mmap<mio::access_mode::write, char>;

static std::map<std::string, std::uint32_t> lockMap;
static std::map<std::string, rw_mmap *> memMap;

void allocate_file(const std::string& path, const int size) {
  std::ofstream file(path);
  std::string s(size, '0');
  file << s;
}

void none_free_callback(char* data, void* hint) {
  (void)data;
  (void)hint;
}

NAN_METHOD(Alloc) {
  std::string path(*(Nan::Utf8String(info[0])));
  size_t size = info[1]->Uint32Value();
  allocate_file(path, size);
  std::string hash = md5(path);
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
    Nan::NewBuffer(mmap->data<mio::access_mode::write, char *>(), size, none_free_callback, nullptr).ToLocalChecked()
  );
  info.GetReturnValue().Set(ret);
}

NAN_METHOD(Free) {
  std::string hash(*(Nan::Utf8String(info[0])));
  rw_mmap* mmap = memMap[hash];
  mmap->unmap();
  delete mmap;
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
    Nan::New<v8::String>("alloc").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(Alloc)->GetFunction()
  );
  Nan::Set(
    target,
    Nan::New<v8::String>("free").ToLocalChecked(),
    Nan::New<v8::FunctionTemplate>(Free)->GetFunction()
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
