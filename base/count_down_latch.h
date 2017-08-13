#pragma once

#include "mutex.h"
#include "cond.h"

namespace util {

class CountDownLatch {
public:
  explicit CountDownLatch(int count) : mutex_(), cond_(mutex_), count_(count) { }
  void Wait() {
    MutexGuard g(mutex_);
    while (count_ != 0) {
      cond_.Wait();
    }
  }

  void CountDown() {
    MutexGuard g(mutex_);
    // Use <= to avoid call CountDown twice
    if (--count_ <= 0) {
      cond_.NotifyAll();
    }
  }

  int GetCount() const {
    MutexGuard g(mutex_);
    return count_;
  }

private:
  CountDownLatch(const CountDownLatch &);
  CountDownLatch &operator =(const CountDownLatch &);
  mutable Mutex mutex_;
  Cond cond_;
  int count_;
};

} // namespace util
