#include "rc_string.h"
#include <string.h>

namespace util {

void String::StringValue::Init(const char *val) {
  data = new char[strlen(val) + 1];
  strcpy(data, val);
}

String::StringValue::StringValue(const char *val) {
  Init(val);
}

String::StringValue::StringValue(const StringValue &rhs) {
  Init(rhs.data);
}

String::StringValue::~StringValue() {
  delete [] data;
}

String::String(const char *val) : value_(new StringValue(val)) { }

const char &String::operator [](int index) const {
  return value_->data[index];
}

const char *String::c_str() const {
  return value_->data;
}

char &String::operator [](int index) {
  if (value_->IsShared()) {
    value_ = new StringValue(value_->data);
  }
  value_->MarkUnshareable();
  return value_->data[index];
}

} // namespace util
