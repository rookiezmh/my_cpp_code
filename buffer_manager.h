/*
 * Simple class to manage upd and use index
 * Do need extra lock to provide thread safety.
 * @author: zhumaohua
 * */

#ifndef GALAXY_BUFFER_MANAGER_H_
#define GALAXY_BUFFER_MANAGER_H_

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

namespace util {

/*
 * Type T is the type which manager really manages for you. 
 * This new version works good for both built-in type and customer class.
 * */

/*
 * This is mostly for built-in type or class which has non-expensive assignment
 * operaotr. Otherwise, like set or map of very big size, you should go with 
 * BufferManager<set<T> *>, or the Swap will take a lot of time.
 * */
template<typename T>
class BufferManager {
public:
  
  typedef T ValueType;

  BufferManager() : upd_(T()), use_(T()) { }

  template<typename Arg>
  BufferManager(const Arg &use_arg, const Arg &upd_arg) : upd_(T(upd_arg)),
                                                          use_(T(use_arg)) { }

  // This class doesn't store ptr.
  ~BufferManager() { }

  T *GetUpd() {
    return &upd_;
  }

  T *GetUse() {
    return &use_;
  }

  void Swap() {
    using std::swap;
    swap(use_, upd_);
  }
 
  /*
  std::string Info() const {
    std::stringstream ss;
	  ss << "use:" << use_.size() << " upd:" << upd_.size();
    return ss.str();
  }  
  */
private:
  
  T upd_, use_;
  
  // No copying allowed
  BufferManager(const BufferManager &);
  BufferManager &operator=(const BufferManager &);

};

template<typename T>
class BufferManager<T *> {

public: 
  
  typedef T ValueType;

  BufferManager() : upd_(new T()), use_(new T()) { }

  template<typename Arg>
  BufferManager(const Arg &use_arg, const Arg &upd_arg) : upd_(new T(upd_arg)),
                                                          use_(new T(use_arg)) { }

  ~BufferManager() {
    delete upd_;
    delete use_;
  }

  T *GetUpd() {
    return upd_;
  }

  T *GetUse() {
    return use_;
  }

  void Swap() {
    using std::swap;
    swap(use_, upd_);
  }
 
  std::string Info() const {
    std::stringstream ss;
	  ss << "use.size():" << use_->size() << " upd.size():" << upd_->size();
    return ss.str();
  }  

private:
  
  T *upd_; 
  T *use_;
  
  // No copying allowed
  BufferManager(const BufferManager &);
  BufferManager &operator=(const BufferManager &);

};

} // namespace util

#endif // GALAXY_BUFFER_MANAGER_H_
