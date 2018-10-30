const binding = require('./binding');

class Heap {
  constructor(path, size) {
    const instance = binding.allocHeap(path, size);
    [this.hash, this.heap] = instance;
    this.have = true;
  }
  free() {
    binding.freeHeap(this.hash);
    this.have = false;
  }
  getBuffer() {
    return this.heap;
  }
}

module.exports = Heap;
