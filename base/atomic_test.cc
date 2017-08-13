#include "atomic.h"
#include <thread>

util::AtomicUint32 count;

void Inc() {
  count++;
}

int main() {
  std::thread ts[100];
  for (int i = 0; i < 100; ++i) {
    ts[i] = std::thread(Inc);
  }
  for (int i = 0; i < 100; ++i) {
    ts[i].join();
  }
  printf("%d\n", (int32_t) count);
}
