#ifndef __TCP_POOL__
#define __TCP_POOL__

#include <string>
#include <vector>
#include "util_functions.h"

namespace util {

enum ConnStatus {
  kNormal = 0,
  kDisconnected,
}

struct Conn {
  std::string ip;
  int port;
  int fd;
  ConnStatus status;
};

}

#endif // __TCP_POOL__
