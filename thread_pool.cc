#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include "thread_pool.h"

using util::ThreadPool;

ThreadPool::Task::Task() : job(NULL), arg(NULL) { }

ThreadPool::Task::Task(void (*j)(void *), void *a) : job(j), arg(a) { }

ThreadPool* ThreadPool::Create(int thread_num, int task_queue_size) {
  ThreadPool *p = new ThreadPool(thread_num, task_queue_size);
  if (p->Init()) {
    return p;
  } else {
    return NULL;
  }
}

ThreadPool::ThreadPool(int thread_num, int task_queue_size) : 
  thread_num_(thread_num),
  task_queue_size_(task_queue_size),
  shutdown_(0),
  head_(0),
  tail_(0),
  count_(0),
  started_(0),
  threads_(NULL),
  tasks_(NULL) { }

bool ThreadPool::Init() {
  if (pthread_mutex_init(&mutex_, NULL) != 0) {
    return false;
  }
  if (pthread_cond_init(&add_notify_, NULL) != 0) {
    pthread_mutex_destroy(&mutex_);
    return false;
  }
  if (pthread_cond_init(&done_notify_, NULL) != 0) {
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&add_notify_);
    return false;
  }

  threads_ = static_cast<pthread_t *>(calloc(thread_num_, sizeof(pthread_t)));
  if (threads_ == NULL) {
    Free();
    return false;
  }

  tasks_ = static_cast<Task *>(calloc(task_queue_size_, sizeof(Task)));
  if (tasks_ == NULL) {
    Free();
    return false;
  }
  return true;
}

void ThreadPool::Free() {
  pthread_mutex_lock(&mutex_);
  pthread_mutex_destroy(&mutex_);
  pthread_cond_destroy(&add_notify_);
  pthread_cond_destroy(&done_notify_);
  if (threads_) free(threads_);
  if (tasks_) free(tasks_);
}

bool ThreadPool::Destroy(int flag) {
  pthread_mutex_lock(&mutex_);
  bool err = false;
  do {
    if (shutdown_) {
      err = true;
      break;
    } 
    shutdown_ = (flag & kImmediateShutDown) ? kImmediateShutDown : kGracefulShutDown;

    pthread_cond_broadcast(&add_notify_);
    pthread_cond_broadcast(&done_notify_);
    pthread_mutex_unlock(&mutex_);
    for (int i = 0; i < thread_num_; ++i) {
      if (pthread_join(threads_[i], NULL) != 0) {
        err = true;
        break;
      }
    }
  } while (0);

  if (!err) {
    Free();
  }
  return !err;
}

bool ThreadPool::Start() {
  for (int i = 0; i < thread_num_; ++i) {
    if (pthread_create(&threads_[i], NULL, ThreadRoutine, this) != 0) {
      Free();
      return false;
    }
    ++started_;
  }
  return started_ == thread_num_;
}

void *ThreadPool::ThreadRoutine(void *arg) {
  ThreadPool *p = static_cast<ThreadPool *>(arg);
  while (1) {
    pthread_mutex_lock(&p->mutex_);
    while (p->count_ == 0 && !p->shutdown_) {
      pthread_cond_wait(&p->add_notify_, &p->mutex_);
    }
    if (p->shutdown_ == kImmediateShutDown || 
        (p->shutdown_ == kGracefulShutDown && p->count_ == 0)) {
      break;
    }

    const Task &t = p->tasks_[p->head_];
    p->head_ = (p->head_ + 1) % p->task_queue_size_;
    --p->count_;
    pthread_cond_signal(&p->done_notify_);
    pthread_mutex_unlock(&p->mutex_);
    t.job(t.arg);
  }
  --p->started_;
  pthread_mutex_unlock(&p->mutex_);
  return NULL;
}

int ThreadPool::AddTask(const Task &task, int timeout) {
  struct timeval tp;
	struct timespec ts;
  if (timeout != -1) {
    gettimeofday(&tp, NULL);
    ts.tv_sec  = tp.tv_sec;
    ts.tv_nsec = tp.tv_usec * 1000;
    ts.tv_nsec += timeout * 1000000;
  }
  pthread_mutex_lock(&mutex_);
  int next = (tail_ + 1) % task_queue_size_;
  if (timeout != -1) {
    while (count_ == task_queue_size_ && !shutdown_) {
      int r = pthread_cond_timedwait(&done_notify_, &mutex_, &ts);
      if (r == ETIMEDOUT) {
        printf("AddTask timeout\n");
        pthread_mutex_unlock(&mutex_);
        return kTimedout;
      }
    }
  } else {
    while (count_ == task_queue_size_ && !shutdown_) {
      pthread_cond_wait(&done_notify_, &mutex_);
    }
  }
  
  if (shutdown_) {
    printf("Pool has been shutdown\n");
    pthread_mutex_unlock(&mutex_);
    return kPoolShutDown;
  }
  tasks_[tail_] = task;
  tail_ = next;
  ++count_;
  pthread_cond_signal(&add_notify_);
  pthread_mutex_unlock(&mutex_);
  return 0;
}

int ThreadPool::AddTask(void (*job)(void *), void *arg, int timeout) {
  return AddTask(Task(job, arg), timeout);
}
