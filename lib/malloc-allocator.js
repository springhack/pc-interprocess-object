/*
        Author: SpringHack - springhack@live.cn
        Last modified: 2018-10-31 15:15:04
        Filename: lib/malloc-allocator.js
        Description: Created by SpringHack using vim automatically.
 */
const binding = require('./binding');

class MallocAllocator {
  constructor(heap) {
    this.heap = heap;
  }
  malloc(size) {
    const [offset, buffer] = binding.malloc(this.heap.hash, size);
    buffer.heapOffset = offset;
    buffer.heapHash = this.heap.hash;
    return buffer;
  }
}

module.exports = MallocAllocator;
