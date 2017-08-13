#include "count_down_latch.h"
#include <unistd.h>
#include <stdio.h>

using namespace util;

CountDownLatch c(100);

void *Foo(void *arg) {
  sleep(2);
  c.CountDown();
  int i = *((int *) arg);
  printf("thread[%d] exit\n", i);
  return NULL;
}

int main() {
  pthread_t ts[100];
  for (int i = 0; i < 100; ++i) {
    int *p = new int(i);
    pthread_create(ts+i, NULL, Foo, p);
  }
  c.Wait();
  printf("all thread ok\n");
}
