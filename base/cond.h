#pragma once 

#include <pthread.h>
#include "mutex.h"

namespace util {
class Cond {
public:
  explicit Cond(Mutex &mutex) : mutex_(mutex) {
    pthread_cond_init(&cond_, NULL);
  }
  ~Cond() {
    pthread_cond_destroy(&cond_);
  }
  int Wait() {
    return pthread_cond_wait(&cond_, mutex_.Get());
  }
  int Notify() {
    return pthread_cond_signal(&cond_);
  }
  int NotifyAll() {
    return pthread_cond_broadcast(&cond_);
  }
private:
  Cond(const Cond &);
  Cond &operator =(const Cond &);
  Mutex &mutex_;
  pthread_cond_t cond_;
};
}
