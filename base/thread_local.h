#pragma once

#include "mutex.h"
#include <pthread.h>

namespace util {

template<typename T>
class ThreadLocal {
public:
  ThreadLocal() {
    pthread_key_create(&pkey_, &ThreadLocal::Destructor);
  }
  ~ThreadLocal() {
    pthread_key_delete(pkey_);
  }

  T *Ptr() {
    T *p = static_cast<T *>(pthread_getspecific(pkey_));
    if (!p) {
      T *n = new T();
      pthread_setspecific(pkey_, n);
      p = n;
    }
    return p;
  }

  T &Value() {
    T *p = Ptr();
    return *p;
  }

private:
  static void Destructor(void *x) {
    T *p = static_cast<T *>(x);
    typedef char T_must_be_complete_type[sizeof(T)==0 ? -1 : 1];
    T_must_be_complete_type dummy; (void) dummy;
    delete p;
  }

private:
  pthread_key_t pkey_;
};

} // namespace util
