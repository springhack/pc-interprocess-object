#include "allocator.h"


using namespace std;

namespace dosk {
  allocator::allocator(string& hash, void* pool, size_t size): hash(hash), size(size) {
    this->heap = tlsf_create_with_pool(pool, size);
  } 

  void* allocator::get_heap() {
    return this->heap;
  }

  size_t allocator::get_heap_size() {
    return this->size;
  }

  void* allocator::malloc(size_t size) {
    return tlsf_malloc(this->heap, size);
  }
  void* allocator::align(size_t align, size_t bytes) {
    return tlsf_memalign(this->heap, align, bytes);
  }

  void* allocator::realloc(void* ptr, size_t size) {
    return tlsf_realloc(this->heap, ptr, size);
  }

  void allocator::free(void* ptr) {
    return tlsf_free(this->heap, ptr);
  }

  allocator::~allocator() {
    tlsf_destroy(this->heap);
  }
}
