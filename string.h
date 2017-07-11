#ifndef GALAXY_STRING_H_
#define GALAXY_STRING_H_

#include <string.h>
#include <algorithm>
#include <stdlib.h>
#include <assert.h>

namespace galaxy {

class String {

public:

    typedef size_t size_type;

    static const size_type npos;

    String() : rep_(&nullrep_) { }

    String(const String &rhs) : rep_(0) {
        Init(rhs.length());
        memcpy(Start(), rhs.data(), length());
    }

    explicit String(const char *copy) : rep_(0) {
        Init(static_cast<size_type>(strlen(copy)));
        memcpy(Start(), copy, length());
    }

    String(const char *str, size_type len) : rep_(0) {
        Init(len);
        memcpy(Start(), str, len);
    }

    ~String() { 
        Quit(); 
    }

    String &operator =(const String &rhs) {
        return assign(rhs.Start(), rhs.length());
    }

    String &operator =(const char *copy) {
        return assign(copy, static_cast<size_type>(strlen(copy)));
    }

    String &operator +=(const char *suffix) {
        return append(suffix, static_cast<size_type>(strlen(suffix)));
    }

    String &operator +=(const String &suffix) {
        return append(suffix.Start(), suffix.length());
    }

    const char *c_str() const { 
        return rep_->str;
    }

    const char *data() const {
        return rep_->str;
    }

    size_type length() const {
        return rep_->size;
    }

    size_type size() const {
        return rep_->size;
    }

    bool empty() const {
        return size() == 0;
    }

    size_type capacity() const {
        return rep_->capacity;
    }

    const char &at(size_type idx) const {
        assert(idx < size());
        return rep_->str[idx];
    }

    char &operator [](size_type idx) const {
        assert(idx < size());
        return rep_->str[idx];
    }

    size_type find(char t) const {
        return find(t, 0);
    }

    size_type find(char t, size_type offset) const {
        if (offset > size()) {
            return npos;
        }
        for (const char *p = c_str() + offset; *p != '\0'; ++p) {
            if (*p == t) {
                return static_cast<size_type>(p - c_str());
            }
        }
        return npos;
    }

    void clear() {
        Quit();
        Init(0, 0);
    }

    void reserve(size_type cap);
    
    String &assign(const char *str, size_type len);

    String &append(const char *str, size_type len);

    void swap(String &o) {
        using std::swap;
        swap(rep_, o.rep_);
    }

private:

    void Init(size_type s) { 
        Init(s, s); 
    }

    void SetSize(size_type s) {
        rep_->str[rep_->size = s] = '\0';
    }

    char *Start() const {
        return rep_->str;
    }

    char *Finish() const {
        return rep_->str + rep_->size;
    }

    struct Rep {
        size_type size, capacity;
        char str[1];
    };

    void Init(size_type sz, size_type cap) {
        if (cap) {
            const size_type bytes_to_allocate = sizeof(Rep) + cap;
            const size_type ints_to_allocate = (bytes_to_allocate + sizeof(int) - 1) / sizeof(int);
            rep_ = reinterpret_cast<Rep *>(new int[ints_to_allocate]);
            rep_->str[rep_->size = sz] = '\0';
            rep_->capacity = cap;
        } else {
            rep_ = &nullrep_;    
        }
    }
    
    void Quit() {
        if (rep_ != &nullrep_) {
            delete [] (reinterpret_cast<int *>(rep_));
        }   
    }

    Rep *rep_;

    static Rep nullrep_;
};

inline bool operator ==(const String &a, const String &b) {
    return (a.size() == b.size()) &&
        (strcmp(a.c_str(), b.c_str()));
}

inline bool operator !=(const String &a, const String &b) {
    return !(a == b);
}

inline bool operator <(const String &a, const String &b) {
    return strcmp(a.c_str(), b.c_str()) < 0;
}

inline bool operator >(const String &a, const String &b) {
    return b < a;
}

inline bool operator <=(const String &a, const String &b) {
    return !(b < a);
}

inline bool operator >=(const String &a, const String &b) {
    return !(a < b);
}

inline bool operator ==(const String &a, const char *b) {
    return strcmp(a.c_str(), b) == 0;
}

inline bool operator ==(const char *a, const String &b) {
    return b == a;
}

inline bool operator !=(const String &a, const char *b) {
    return !(a == b);
}

inline bool operator !=(const char *a, const String &b) {
    return !(b == a);
}

String operator +(const String &a, const String &b);

String operator +(const String &a, const char *b);

String operator +(const char *a, const String &b);

}

#endif // GALAXY_STRING_H_
