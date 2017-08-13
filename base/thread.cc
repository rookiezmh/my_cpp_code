#include "thread.h"

namespace util {


namespace detail {
static pid_t gettid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

static void AfterFork() {
  current_thread::t_cached_tid = 0;
  current_thread::t_thread_name = "main";
  current_thread::Tid();
}

class ThreadNameInitializer {
public:
  ThreadNameInitializer() {
    current_thread::t_thread_name = "main";
    current_thread::Tid();
    pthread_atfork(NULL, NULL, &AfterFork);
  }
};

ThreadNameInitializer init;

struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func;
  std::string name;
};

} // namespace detail

namespace current_thread {
  __thread int t_cached_tid = 0;
  __thread char t_tid_string[32];
  __thread int t_tid_string_length = 6;
  __thread const char *t_thread_name = "Unknown";
  void CacheTid() {
    if (t_cached_tid == 0) {
      t_cached_tid = gettid();
      t_tid_string_length = snprintf(t_tid_string, sizeof(t_tid_string), "%5d ", t_cached_tid);
    }
  }

  bool IsMainThread() {
    return Tid() == ::getpid();
  }

}

} // namespace util
