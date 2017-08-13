#include "blocking_queue.h"
#include <thread>
#include <stdio.h>

using namespace util;
using namespace std;

BlockingQueue<int> q;

void Prod() {
  for (int i = 0; i < 100; ++i)
    q.Put(i);
}

void Cons() {
  int i;
  q.Take(i);
  printf("Take %d from queue\n", i);
}

int main() {
  thread t[100];
  for (int i = 0; i < 100; ++i) {
    t[i] = std::move(thread(Cons));
  }
  thread t1(Prod);
  t1.join();
  for (int i = 0; i < 100; ++i) {
    t[i].join();
  }
}
