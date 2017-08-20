#include "max_heap.h"
#include <assert.h>

size_t MaxHeap::Size() const {
  assert(vec.size() >= 1);
  return vec.size() - 1;
}

bool MaxHeap::Empty() const {
  return Size() == 0;
}

void MaxHeap::Insert(int i) {
  vec.push_back(i);
  Swim(Size());
} 

void MaxHeap::Swim(size_t hole) {
  int t = vec[hole];
  while (hole > 1 && t > vec[hole / 2]) {
    vec[hole] = vec[hole / 2];
    hole /= 2;
  }
  vec[hole] = t;
}

void MaxHeap::Sink(size_t hole) {
  int t = vec[hole];
  while (2 * hole <= Size()) {
    size_t max_child = hole * 2;
    if (max_child < Size() && vec[max_child] < vec[max_child+1]) {
      ++max_child;
    }
    if (t > vec[max_child]) {
      break;
    } else {
      vec[hole] = vec[max_child];
      hole = max_child;
    }
  }
  vec[hole] = t;
}

MaxHeap::MaxHeap() : vec(1) {}

void MaxHeap::DelMax() {
  int t = vec.back();
  vec.pop_back();
  vec[1] = t;
  Sink(1);
}

MaxHeap::iterator MaxHeap::begin() {
  return ++vec.begin();
}

MaxHeap::iterator MaxHeap::end() {
  return vec.end();
}
