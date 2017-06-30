#ifndef GALAXY_THREAD_POOL_H_
#define GALAXY_THREAD_POOL_H_

#include <pthread.h>

namespace util {

class ThreadPool {

public:

  typedef enum {
    kImmediateShutDown = 1,
    kGracefulShutDown = 2,
  } ShutDownWay;

  typedef enum {
    kPoolShutDown = 1,
    kTimedout = 2,
  } AddError;

  struct Task {
    Task();
    Task(void (*j)(void *), void *a);
    void (*job)(void *);
    void *arg;
  }; 

public:

  static ThreadPool *Create(int thread_num, int task_queue_size);

  bool Destroy(int flag);

  bool Start();

  int AddTask(const Task &task, int timeout);

  int AddTask(void (*job)(void *), void *arg, int timeout);

private:
  // No copying allowed
  ThreadPool(const ThreadPool &);
  ThreadPool &operator=(const ThreadPool &);
  // Heap allocated only, and should not used as a base class
  ThreadPool(int thread_num, int task_queue_size);
  ~ThreadPool();

private:

  bool Init();

  void Free();

  static void *ThreadRoutine(void *arg);

private:

  int thread_num_;

  int task_queue_size_;

  int head_;

  int tail_;

  int count_;

  int started_;

  int shutdown_;

  pthread_t *threads_;

  pthread_cond_t add_notify_;

  pthread_cond_t done_notify_;

  pthread_mutex_t mutex_;

  Task *tasks_;

};

}

#endif // GALAXY_THREAD_POOL_H_
