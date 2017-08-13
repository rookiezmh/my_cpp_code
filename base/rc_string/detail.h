#pragma once

namespace detail {

class ReferenceCountBase {
public:
  void AddReference();
  void RemoveReference();
  void MarkUnshareable();
  bool IsShareable() const; 
  bool IsShared() const; 
protected:
  ReferenceCountBase();
  ReferenceCountBase(const ReferenceCountBase &);
  ReferenceCountBase &operator =(const ReferenceCountBase &);
  virtual ~ReferenceCountBase() = 0;
private:
  int reference_count_;
  bool shareable_;
};

template<typename Countable>
class ReferenceCountPtr {
public:
  ReferenceCountPtr(Countable *p = 0) : pointee_(p) {
    Init();
  }
  ReferenceCountPtr(const ReferenceCountPtr &rhs) : pointee_(rhs.pointee_) {
    Init();
  }
  ReferenceCountPtr &operator =(const ReferenceCountPtr &rhs) {
    if (pointee_ != rhs.pointee_) {
      if (pointee_) {
        pointee_->RemoveReference();
      }
      pointee_ = rhs.pointee_;
      Init();
    }  
    return *this;
  }
  ~ReferenceCountPtr() {
    if (pointee_) {
      pointee_->RemoveReference();
    }
  }
  Countable &operator *() const {
    return *pointee_;
  }
  Countable *operator ->() const {
    return pointee_;
  }
private:
  Countable *pointee_;
  void Init() {
    if (!pointee_) {
      return;
    }
    if (!pointee_->IsShareable()) {
      pointee_ = new Countable(*pointee_);
    }
    pointee_->AddReference();
  }
};

}
