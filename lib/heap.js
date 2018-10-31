/*
        Author: SpringHack - springhack@live.cn
        Last modified: 2018-10-31 14:25:14
        Filename: heap.js
        Description: Created by SpringHack using vim automatically.
 */
const binding = require('./binding');

class Heap {
  constructor(path, size) {
    const instance = binding.allocHeap(path, size);
    [this.hash, this.heap] = instance;
    this.have = true;
  }
  free() {
    // Totally useless, just in case
    binding.freeHeap(this.hash);
    this.have = false;
  }
  getBuffer() {
    return this.heap;
  }
}

module.exports = Heap;
