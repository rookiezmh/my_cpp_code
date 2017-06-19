#ifndef GALAXY_DYNAMIC_WORDS_LIST_H_
#define GALAXY_DYNAMIC_WORDS_LIST_H_

/*
 * This class can be implemented wiht libevent.
 * */

#include <string>
#include <fstream>
#include <set>
#include <assert.h>
#include <pthread.h>
#include "buffer_manager.h"
#include "dynamic_words_list.h"
#include "rwlock_guard.h"
#include <iostream>
#include <string>
#include <unistd.h>

namespace util {

using namespace std;

template<typename T>
class DynamicWordsList {

public:

  typedef string (*GetFileNameCb)(void *arg);

  typedef void (*UpdateCb)(ifstream &file, set<T> &s);

public:
  
  explicit DynamicWordsList(int interval = 60) : 
                                                sbm_(new BufferManager<set<T> *>),
                                                interval_(interval) { 
    Init();
  }

  ~DynamicWordsList() {
    pthread_rwlock_destroy(&prw_lock_); 
  }

  // Register callback to get the file name to load
  void RegisterGetFileNameCb(GetFileNameCb cb, void *arg = NULL) {
    assert(cb);
    file_name_cb_ = cb;
    file_name_cb_arg_ = arg;
  }

  void RegisterUpdateCb(UpdateCb cb) {
    assert(cb);
    update_cb_ = cb;
  }

  pthread_t GetPid() const { return pid_; }
  // Load data to set, and begin the updateing thread.
  int Start();
  
  bool InList(const T &w) const;

private:

  static void *UpdateRoutine(void *arg);

  void Init();

private:

  bool LoadAndSwap();
  
  BufferManager<set<T> *>  *sbm_;

  int interval_;

  GetFileNameCb file_name_cb_;

  void *file_name_cb_arg_;

  UpdateCb update_cb_;

  pthread_t pid_;

  mutable pthread_rwlock_t prw_lock_;

};

template<typename T>
bool DynamicWordsList<T>::LoadAndSwap() {
  const string file_name = file_name_cb_(file_name_cb_arg_);
  ifstream f(file_name);
  if (!f) {
    return false;
  }
  set<T> *s = sbm_->GetUpd();
  s->clear();
  update_cb_(f, *s);
  {
    RWLockGuard l(prw_lock_);
    pthread_rwlock_wrlock(&prw_lock_);
    sbm_->Swap();
  }
  s = sbm_->GetUpd();
  s->clear();
  update_cb_(f, *s);
  return true;
}

template<typename T>
void *DynamicWordsList<T>::UpdateRoutine(void *arg) {
  DynamicWordsList *p = (DynamicWordsList *) arg;
  static int times = 1;
  while (1) {
    p->LoadAndSwap();
    cout << "times:" << times++ << endl;
    sleep(p->interval_);
  }
}

template<typename T>
void DynamicWordsList<T>::Init() {
  file_name_cb_ = NULL;
  file_name_cb_arg_ = NULL;
  update_cb_ = NULL;
  pthread_rwlock_init(&prw_lock_, NULL);
}

template<typename T>
bool DynamicWordsList<T>::InList(const T &w) const {
  RWLockGuard l(prw_lock_);
  pthread_rwlock_rdlock(&prw_lock_);
  const set<T> *list = sbm_->GetUse();
  return list->find(w) != list->end();
}

template<typename T>
int DynamicWordsList<T>::Start() {
  if (!LoadAndSwap()) {
    return -1;
  }
  return pthread_create(&pid_, NULL, &DynamicWordsList::UpdateRoutine, this);
}

} // namespace util

#endif // GALAXY_DYNAMIC_WORDS_LIST_H_
