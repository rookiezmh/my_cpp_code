#pragma once

#include <stdint.h>

namespace util {

template<typename T>
class AtomicIntegerT {
public:
  AtomicIntegerT() : val_(0) { }
  template<typename U>
  explicit AtomicIntegerT(U u) : val_(u) { }
  T Get() {
    return __sync_val_compare_and_swap(&val_, 0, 0);
  }
  T FetchAndAdd(T x) {
    return __sync_fetch_and_add(&val_, x);
  }
  T AddAndFetch(T x) {
    return __sync_add_and_fetch(&val_, x);
  }
  T IncrementAndFetch() {
    return AddAndFetch(1);
  }
  T DecrementAndFetch() {
    return AddAndFetch(-1);
  }
  void Add(T x) {
    FetchAndAdd(x);  
  }
  void Increment() {
    Add(1);
  }
  void Decrement() {
    Add(-1);
  }
  T GetAndSet(T n) {
    return __sync_lock_test_and_set(&val_, n);
  }
  operator T() {
    return Get();
  }
  // 不返回引用，返回一个简单的T
  T operator ++() {
    return AddAndFetch(1);
  }
  T operator ++(int) {
    return FetchAndAdd(1);
  }
  T operator --() {
    return AddAndFetch(-1);
  }
  T operator --(int) {
    return FetchAndAdd(-1);
  }

private:
  volatile T val_;
};

typedef AtomicIntegerT<int8_t>  AtomicInt8;
typedef AtomicIntegerT<int16_t> AtomicInt16;
typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;
typedef AtomicIntegerT<uint32_t> AtomicUint32;
typedef AtomicIntegerT<uint64_t> AtomicUint64;

} // namespace util
