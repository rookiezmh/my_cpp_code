#include "detail.h"

namespace detail {
  ReferenceCountBase::ReferenceCountBase() 
    : reference_count_(0), shareable_(false) { }
  
  ReferenceCountBase::ReferenceCountBase(const ReferenceCountBase &)
    : reference_count_(0), shareable_(false) { }
  
  ReferenceCountBase &ReferenceCountBase::operator =(const ReferenceCountBase &) {
    return *this;
  }

  ReferenceCountBase::~ReferenceCountBase() { }

  void ReferenceCountBase::AddReference() {
    ++reference_count_;
  }

  void ReferenceCountBase::RemoveReference() {
    if (--reference_count_ == 0) {
      delete this;
    }
  }

  void ReferenceCountBase::MarkUnshareable() {
    shareable_ = false;
  }

  bool ReferenceCountBase::IsShareable() const {
    return shareable_;
  }

  bool ReferenceCountBase::IsShared() const {
    return reference_count_ > 1;
  }

}
