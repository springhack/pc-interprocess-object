const Allocator = require('malloc');
const binding = require('./binding');

class MallocAllocator {
  constructor(heap) {
    this.heap = heap;
    this.allocator = new Allocator(heap.heap);
  }
  malloc(size, calloc = false) {
    const offset = this.allocator[calloc ? 'calloc' : 'alloc'](size);
    const buffer = binding.fromOffset(this.heap.heap, offset, size);
    buffer.nativeOffset = offset;
    return buffer;
  }
  free(buffer) {
    this.allocator.free(buffer.nativeOffset);
  }
}

module.exports = MallocAllocator;
