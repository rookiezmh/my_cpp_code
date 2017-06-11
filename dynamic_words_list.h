#ifndef GALAXY_DYNAMIC_WORDS_LIST_H_
#define GALAXY_DYNAMIC_WORDS_LIST_H_

#include <string>
#include <fstream>
#include <set>
#include <assert.h>
#include <pthread.h>
#include "buffer_manager.h"

namespace util {

class DynamicWordsList {

public:

  typedef std::string (*GetFileNameCb)(void *arg);

  typedef void (*UpdateCb)(std::ofstream &file, BufferManager<std::string *> *bm);

public:
  
  DynamicWordsList() : sbm_(new BufferManager<std::set<std::string> *>) { 
    Init();
  }

  DynamicWordsList(BufferManager<std::set<std::string> *> *bm) : sbm_(bm) { 
    Init();
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

  // Load data to set, and begin the updateing thread.
  int Start();

private:

  static void *UpdateRoutine(void *arg);

  void Init();

private:
  
  BufferManager<std::set<std::string> *>  *sbm_;

  GetFileNameCb file_name_cb_;

  void *file_name_cb_arg_;

  UpdateCb update_cb_;

  pthread_t pid_;
};

} // namespace util

#endif // GALAXY_DYNAMIC_WORDS_LIST_H_
