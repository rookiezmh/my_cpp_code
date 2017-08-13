#pragma once

#include "detail.h"

namespace util {

class String {
public:
  String(const char *val = "");
  const char *c_str() const;
  const char &operator [](int index) const;
  char &operator [](int index);
private:
  struct StringValue : public detail::ReferenceCountBase {
    char *data;
    StringValue(const char *val);
    StringValue(const StringValue &rhs);
    void Init(const char *val);
    ~StringValue();
  };
  detail::ReferenceCountPtr<StringValue> value_;
};


} // namespace util
