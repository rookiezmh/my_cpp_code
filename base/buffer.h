#pragma once

#include <vector>
#include <algorithm>
#include <string>

#include <string.h>
#include <assert.h>

namespace util {

class Buffer {
public:
  static const size_t kCheapPrepend; 
  static const size_t kInitialSize;

  // At least we allocate 1024 bytes at first
  explicit Buffer(size_t initial_size = kInitialSize)
    : data_(std::max(kInitialSize, initial_size) + kCheapPrepend),
      read_idx_(kCheapPrepend),
      write_idx_(kCheapPrepend) { }

  // Default dtor, cpoy-ctor is enough (cause we use vector<char> instread of char[])
  
  void swap(Buffer &other) {
    using std::swap;
    swap(data_, other.data_);
    swap(read_idx_, other.read_idx_);
    swap(write_idx_, other.write_idx_);
  }

  // How many bytes not read
  size_t ReadableBytes() const { return write_idx_ - read_idx_; }

  //size_t WritableBytes() const { return data_.size() - write_idx_; }
  size_t WritableBytes() const { return data_.capacity() - write_idx_; }

  size_t PrependableBytes() const { return read_idx_; }

  const char *Peek() const { return Begin() + read_idx_; }
  
  char *Peek() { return Begin() + read_idx_; }

  const char *BeginWrite() const { return Begin() + write_idx_; }
  
  char *BeginWrite() { return Begin() + write_idx_; }

  const char *FindCRLF() const {
    const char *crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
    return (crlf == BeginWrite() ? NULL : crlf);
  }

  const char *FindCRLF(const char *start) const {
    assert(start >= Peek() && start <= BeginWrite());
    const char *crlf = std::search(start, BeginWrite(), kCRLF, kCRLF + 2);
    return (crlf == BeginWrite() ? NULL : crlf);
  }

  const char *FindEOL() const {
    void *p = memchr(Peek(), kEOL, ReadableBytes());  
    return static_cast<const char *>(p);
  }

  const char *FindEOL(const char *start) const {
    assert(start >= Peek() && start <= BeginWrite());
    void *p = memchr(start, kEOL, BeginWrite() - start);  
    return static_cast<const char *>(p);
  }

  void Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    if (len < ReadableBytes()) {
      read_idx_ += len;
    } else {
      RetrieveAll();
    }
  }

  void RetrieveAll() {
    read_idx_ = write_idx_ = kCheapPrepend;
  }

  void RetrieveUntil(const char *end) {
    assert(end >= Peek() && end <= BeginWrite());
    return Retrieve(end - Peek());
  }

  std::string RetrieveAsString(size_t len) {
    assert(len <= ReadableBytes());
    std::string s(Peek(), len);
    Retrieve(len);
    return s;
  }

  void RetrieveAsString(size_t len, std::string &out) {
    assert(len <= ReadableBytes());
    out = std::string(Peek(), len);
    Retrieve(len);
  }

  std::string RetrieveAllString() {
    return RetrieveAsString(ReadableBytes());
  }

  void RetrieveAllString(std::string &out) {
    return RetrieveAsString(ReadableBytes(), out);
  }
  
  void EnsureWritableBytes(size_t len) {
    if (WritableBytes() < len) {
      MakeSpace(len);
    }
    assert(WritableBytes() >= len);
  }

  template<typename T>
  void Append(const T &t) {
    return Append(reinterpret_cast<const void *>(&t), sizeof(T));
  }
  
  template<typename T>
  void Append(const T *p) {
    return Append(*p);
  }

  template<typename T>
  void Append(T *p) {
    return Append(*p);
  }

  void Append(const void *data, size_t len) {
    EnsureWritableBytes(len);
    memmove(BeginWrite(), data, len);
    Skip(len);
  }

  void Append(const char *data, size_t len) {
    return Append(reinterpret_cast<const void *>(data), len);
  }

  void UnWrite(size_t n) {
    assert(n <= ReadableBytes());
    write_idx_ -= n;
  }

  void Prepend(const void *data, size_t len) {
    assert(len <= PrependableBytes());
    read_idx_ -= len;
    memmove(Peek(), data, len);
  }

  void Prepend(const char *data, size_t len) {
    return Prepend(reinterpret_cast<const void *>(data), len);
  }
  
  template<typename T>
  void Prepend(const T &t) {
    return Prepend(reinterpret_cast<const void *>(&t), sizeof(T));
  }

  template<typename T>
  void Prepend(const T *p) {
    return Prepend(reinterpret_cast<const void *>(p), sizeof(T));
  }

  template<typename T>
  void Prepend(T *p) {
    return Prepend(reinterpret_cast<const void *>(p), sizeof(T));
  }

  ssize_t ReadFromFd(int fd, int &err);

private:
  void MoveData() {
    // Reuse memory have been read
    assert(kCheapPrepend <= read_idx_);
    size_t b = ReadableBytes();
    memmove(Begin() + kCheapPrepend, Peek(), b);
    read_idx_ = kCheapPrepend;
    write_idx_ = read_idx_ + b;
    assert(b = ReadableBytes());
  }

  void MakeSpace(size_t len) {
    if (WritableBytes() + PrependableBytes() < len + kCheapPrepend) {
      data_.resize((write_idx_<<1) + len);
    } 
    MoveData();
  }

  char *Begin() {
    return &*data_.begin();
  }

  const char *Begin() const {
    return &*data_.begin();
  }
  
  void Skip(size_t n) {
    assert(n <= WritableBytes());  
    write_idx_ += n;
  }

private:
  std::vector<char> data_;
  size_t read_idx_; // First byte for user to read
  size_t write_idx_; // Next position to write into buffer
  static const char kCRLF[];
  static const char kEOL;
};

} // namespace util
