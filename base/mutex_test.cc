#include "mutex.h"
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string>

int64_t count = 0;
std::string s;
util::Mutex c_mutex;

void *Inc(void *arg) {
  util::MutexGuard g(c_mutex);
  s += std::string("haha");
  delete (int *)arg;
  return NULL;
}

int main() {
  pthread_t pids[100];
  for (int i = 0; i < 100; ++i) {
    int *p = new int(i);
    pthread_create(pids+i, NULL, Inc, p);
  }
  for (int i = 0; i < 100; ++i) {
    pthread_join(pids[i], NULL);
  }
  printf("%s\n", s.c_str());
}
