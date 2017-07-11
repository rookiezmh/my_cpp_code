#include "tcp_conn_pool.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

using namespace std;
using namespace util;

TcpConnPool *p;

int count_num = 0;

void Sig(int t) {
  printf("Signal %d received\n", t);
}

void ReadEchoCb(int fd, short what, void *arg) {
  static char buf[1024] = {0};
}

void StdinReadCb(int fd, short what, void *arg) {
  char buf[1024] = {0};
  ssize_t len;
  if ((len = read(fd, buf, sizeof(buf)))> 0) {
    int sfd = -1;
    p->GetConn(sfd);
    if (sfd == -1) {
      cout << "Error in get conn\n";
      event_base_loopbreak((struct event_base *) arg);
    } else {
      char tmp[1024] = {0};
      sprintf(tmp, "%d:%s", count_num++, buf);
      printf("Write: %s", tmp);
      int l = write(sfd, tmp, strlen(tmp));
      if (l == -1) {
        perror("Write");
        p->PutConn(sfd);
        return;
      }
      char read_buf[1024] = {0};
      if ((l = read(sfd, read_buf, sizeof(read_buf))) > 0) {
        printf("From echo: %s", read_buf);
      } else if (l == 0) {
        printf("EOF received\n");
      } else {
        printf("Error in reading\n");
      }
      p->PutConn(sfd);
    }
  } else {
    cout << "Error in reading from stdin\n";
    event_base_loopbreak((struct event_base *) arg);
  }
}

int main() {
  const string s = "127.0.0.1:5555";
  p = TcpConnPool::Create(s);
  if (p == NULL) {
    cout << "Fuck\n";
    return 1;
  }
  cout << "Create success!\n";
  struct event_base *base = event_base_new();
  if (base == NULL) {
    cout << "Can't create bsae\n";
    return 1;
  }
  struct event *ev = event_new(base, 0, EV_READ | EV_PERSIST, StdinReadCb, base);
  if (ev == NULL) {
    cout << "Can't create event\n";
    return 1;
  }
  event_add(ev, NULL);
  signal(SIGPIPE, Sig);
  event_base_dispatch(base);
  cout << "out" << endl;
}
