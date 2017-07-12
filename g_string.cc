#include "./g_string.h"

namespace galaxy {

const String::size_type String::npos = static_cast<String::size_type>(-1);

String::Rep String::nullrep_ = {0, 0, ""};

void String::reserve(size_type cap) {
    if (cap > capacity()) {
        String tmp;
        tmp.Init(length(), cap);
        memcpy(tmp.Start(), c_str(), size());
        swap(tmp);
    } 
}

String &String::assign(const char *str, size_type len) {
    size_type cap = capacity();
    if (len > cap || cap > 3*(len + 8)) {
        String tmp;
        tmp.Init(len);
        memcpy(tmp.Start(), str, len);
        swap(tmp);
    } else {
        memmove(Start(), str, len);
        SetSize(len);
    }
    return *this;
}

String &String::append(const char *str, size_type len) {
    size_type new_size = len + size();
    if (new_size > capacity()) {
        reserve(2*new_size);
    }
    memmove(Finish(), str, len);
    SetSize(new_size);
    return *this;
}

String operator +(const String &a, const String &b) {
    String tmp;
    tmp.reserve(a.size() + b.size());
    tmp += a;
    tmp += b;
    return tmp;
}

String operator +(const String &a, const char *b) {
    String tmp;
    tmp.reserve(a.size() + static_cast<String::size_type>(strlen(b)));
    tmp += a;
    tmp += b;
    return tmp;
}

// 不能直接return b + a; 这样会多一次String的拷贝构造和析构
String operator +(const char *a, const String &b) {
    String tmp;
    tmp.reserve(b.size() + static_cast<String::size_type>(strlen(a)));
    tmp += a;
    tmp += b;
    return tmp;
}

}
