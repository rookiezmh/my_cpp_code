#include "buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <string>

using namespace util;
using namespace std;

struct T {
  char buf[8];
  T() {
    memcpy(buf, "12345678", 8);
  }
};

int main(int argc, char *argv[]) {
  int f = open(argv[1], O_RDONLY);
  if (f == -1) {
    perror("open");
    exit(1);
  }
  Buffer buf;
  int err;
  ssize_t len = buf.ReadFromFd(f, err);
  if (len < 0) {
    perror("ReadFromFd");
    exit(1);
  }
  char t[] = "hehehehehe";
  buf.Append(t, sizeof(t)-1);
  T *tt = new T;
  buf.Append(tt);
  char p[] = "pre ";
  buf.Prepend(p, sizeof(p)-1);
  string s;
  buf.RetrieveAllString(s);
  printf("%s, %ld\n", s.c_str(), s.size());
}
