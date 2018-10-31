#include "tlsf.h"
#include <string>

using namespace std;

namespace dosk {
  class allocator {
    private:
      string hash;
      tlsf_t heap;
      size_t size;
    public:
      allocator(string& hash, void* pool, size_t size);
      void* get_heap();
      size_t get_heap_size();
      void* malloc(size_t size);
      void* align(size_t align, size_t bytes);
      void* realloc(void* ptr, size_t size);
      void free(void* ptr);
      ~allocator();
  };
}
