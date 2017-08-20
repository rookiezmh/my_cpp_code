#include "max_heap.h"
#include <stdio.h>
#include <algorithm>
#include <assert.h>

void Print(int i) {
  printf("%d ", i);
}

int main() {
  MaxHeap mh;
  for (int i = 0; i < 100; ++i) {
    mh.Insert(i);
    assert(*mh.begin() == i);
  }
  for (int i = 99; i > -1; --i) {
    assert(*mh.begin() == i);
    mh.DelMax();
  }
  assert(mh.Empty());
}
