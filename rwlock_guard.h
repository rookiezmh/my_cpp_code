#ifndef _GALAXY_RW_LOCK_H_
#define _GALAXY_RW_LOCK_H_

#include <pthread.h>

namespace util {

class RWLockGuard {

public:

  RWLockGuard(pthread_rwlock_t *rw_lock) : rw_lock_(rw_lock) { }

  RWLockGuard(pthread_rwlock_t &rw_lock) : rw_lock_(&rw_lock) { }

  ~RWLockGuard() { pthread_rwlock_unlock(rw_lock_); }

private:

  RWLockGuard(const RWLockGuard &);
  RWLockGuard &operator=(const RWLockGuard &);

  pthread_rwlock_t *rw_lock_;

};

}

#endif // pthread_rwlock_rdlock
