#pragma once 

#include <pthread.h>

namespace util {

class MutexGuard;

class Mutex {

friend class MutexGuard;

public:
  Mutex() {
    pthread_mutex_init(&mutex_, NULL);
  }
  ~Mutex() {
    pthread_mutex_destroy(&mutex_);
  }
  // 不好的接口，暴露了内部使用pthread实现
  pthread_mutex_t *Get() {
    return &mutex_;
  }

private:
  void Lock() {
    pthread_mutex_lock(&mutex_);
  }
  void Unlock() {
    pthread_mutex_unlock(&mutex_);
  }

private:
  Mutex(const Mutex &);
  Mutex &operator =(const Mutex &);

private:
  pthread_mutex_t mutex_;
};

class MutexGuard {
public:
  explicit MutexGuard(Mutex &m) : mutex_(m) {
    mutex_.Lock();
  }
  ~MutexGuard() {
    mutex_.Unlock();
  }

private:
  MutexGuard(const MutexGuard &);
  MutexGuard &operator =(const MutexGuard &);

private:
  Mutex &mutex_;
};

} // namespace util
