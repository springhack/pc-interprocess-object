const { Heap, MallocAllocator } = require('.');

const heap = new Heap('/tmp/mmap', 1024 * 10);
const allocator = new MallocAllocator(heap);
const buffer = allocator.malloc(100);

for (let i = 0; i < 100; ++i) {
  buffer[i] = i;
}

for (let i = 0; i < 100; ++i) {
  console.log(buffer[i], heap.getBuffer()[buffer.nativeOffset + i]);
}
