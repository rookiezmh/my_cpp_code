#pragma once

#include <deque>
#include "mutex.h"
#include "cond.h"

namespace util {

template<typename T>
class BlockingQueue {
public:
  BlockingQueue() : mutex_(), empty_cond_(mutex_), queue_() { }
  void Put(const T &t) {
    MutexGuard g(mutex_);
    queue_.push_back(t);
    empty_cond_.Notify();
  }
  void Take(T &t) {
    MutexGuard g(mutex_);
    while (queue_.empty()) {
      empty_cond_.Wait();
    }
    t = queue_.front();
    queue_.pop_front();
  }
  T Take() {
    MutexGuard g(mutex_);
    while (queue_.empty()) {
      empty_cond_.Wait();
    }
    T t = queue_.front();
    queue_.pop_front();
    return t;
  }
  size_t Size() const {
    MutexGuard g(mutex_);
    return queue_.size();
  }

private:
  BlockingQueue(const BlockingQueue &);
  BlockingQueue &operator =(const BlockingQueue &);
  mutable Mutex mutex_;
  Cond empty_cond_;
  std::deque<T> queue_;
};


} // namespace util
