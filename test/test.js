/*
        Author: SpringHack - springhack@live.cn
        Last modified: 2018-10-31 15:19:20
        Filename: test/test.js
        Description: Created by SpringHack using vim automatically.
 */
const { Heap, MallocAllocator } = require('..');

const heap = new Heap('/tmp/mmap', 1024 * 10);
const allocator = new MallocAllocator(heap);

console.log(`New heap hash(${heap.hash})`);

const buffer = allocator.malloc(100);
const heapBuffer = heap.getBuffer();

console.log(`Native allocator header offset(${buffer.heapOffset})`);

for (let i = 0; i < 100; ++i) {
  buffer[i] = i;
}

let same = true;
for (let i = 0; i < 100; ++i) {
  same = same && (buffer[i] === heapBuffer[buffer.heapOffset + i]);
}

console.log(`Malloc memory same(${same}), from heap hash(${buffer.heapHash}), ${JSON.stringify(buffer, null, 2)}`);
