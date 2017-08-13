#include "buffer.h"

#include <errno.h>
#include <sys/uio.h>

namespace util {

const size_t Buffer::kCheapPrepend = 8;

const size_t Buffer::kInitialSize = 1024 * 1;

const char Buffer::kCRLF[] = "\r\n";

const char Buffer::kEOL = '\n';

static const int kReadBufferSize = 1024 * 64;

ssize_t Buffer::ReadFromFd(int fd, int &err) {
  char buf[kReadBufferSize] = {0};
  struct iovec vec[2];
  const size_t avail = WritableBytes();
  vec[0].iov_base = BeginWrite();
  vec[0].iov_len = avail;
  vec[1].iov_base = buf;
  vec[1].iov_len = sizeof(buf);
  const int count = (avail < sizeof(buf)) ? 2 : 1;
  const ssize_t n = readv(fd, vec, count);
  if (n < 0) {
    err = errno;
  } else if ((size_t) n < avail) {
    Skip(n);
  } else {
    write_idx_ = data_.size();
    Append(buf, n - avail);
  }
  return n;
}

} // namespace util
