#ifndef GALAXY_DYNAMIC_WORDS_LIST_H_
#define GALAXY_DYNAMIC_WORDS_LIST_H_

#include <string>
#include <fstream>
#include <set>
#include <assert>

namespace util {

class BufferManager<std::string *>;

class DynamicWordsList {

public:

  typedef std::string (*GetFileNameCb)(void *arg);

  typedef void (*UpdateCb)(std::ofstream &file, BufferManager<std::string *> *bm);

public:
  
  DynamicWordsList(BufferManager<std::string *> *bm) : bm_(bm), 
                                                       file_name_cb_(NULL), 
                                                       file_name_cb_arg_(NULL),
                                                       update_cb_(NULL) { }


  // Register callback to get the file name to load
  void RegisterGetFileNameCb(GetFileNameCb cb, void *arg) {
    assert(cb);
    file_name_cb_ = cb;
    file_name_cb_arg_ = arg;
  }

  void RegisterUpdateCb(UpdateCb cb) {
    assert(cb);
    update_cb_ = cb;
  }

  bool Update();

private:
  
  BufferManager<std::string *> sbm;

  GetFileNameCb file_name_cb_;

  void *file_name_cb_arg_;

  UpdateCb update_cb_;

};

} // namespace util

#endif // GALAXY_DYNAMIC_WORDS_LIST_H_
