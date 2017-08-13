#pragma once

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <string>

#include "atomic.h"

namespace util {

namespace current_thread {

extern __thread int t_cached_tid;
extern __thread char t_tid_string[];
extern __thread int t_tid_string_length;
extern __thread const char *t_thread_name;

void CacheTid();

inline int Tid() {
  if (t_cached_tid == 0) {
    CacheTid();
  }
  return t_cached_tid;
}

inline const char *TidString() {
  return t_tid_string;
}

int TidStringLength() {
  return t_tid_string_length;
}

const char *Name() {
  return t_thread_name;  
}

bool IsMainThread();

void SleepUsec(int64_t usec);

}

class Thread {
public:
  typedef boost::function<void()> ThreadFunc;

  explicit Thread(const ThreadFunc &, const std::string &name = std::string());
  ~Thread();

  void Start();
  int Join();
  bool Started() const { return started_; }
  pid_t Tid() const { return *tid_; }
  const std::string &Name() const { return name_; }

  static int NumCreated() { return num_created_.Get(); }

private:
  static AtomicUint32 num_created_;
  void SetDefaultName();

  bool started_;
  bool joined_;
  pthread_t pthread_id;
  pid_t *tid_;
  ThreadFunc func_;
  std::string name_;
};

}
