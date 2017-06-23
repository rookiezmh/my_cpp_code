#include "tcp_conn_pool.h"
#include "util_functions.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>

namespace util {

using namespace std;

const char Remote::kSpliter = ':';

Remote::Remote(const string &remote_str) {
  vector<string> v;
  int l = SplitStr(kSpliter, remote_str, v);
  if (l != 2) {
    throw logic_error("Wrong remote str. Should be like host:port");
  }
  host = v[0];
  port = atoi(v[1].c_str());
}

TcpConnPool::TcpConnPool(const string &remote_str, int conn_num) : 
  remote_(Remote(remote_str)),
  conn_num_(conn_num),
  is_destroyed_(false) { 
  
  pthread_mutex_init(&mutex_, NULL);
  if (!Init()) {
    Destroy();
    throw logic_error("Can not init conns.");
  }
}

void TcpConnPool::FixPool(bool &r) {
  pthread_mutex_lock(&mutex_);
  int broken_num = conn_num_ - sockets_.size();
  if (Conn2Remote(broken_num) != broken_num) {
    r = false;
    pthread_mutex_unlock(&mutex_);
    return;
  }
  r = true;
  pthread_mutex_unlock(&mutex_);
}

void TcpConnPool::Destroy() {
  delete this;
}

TcpConnPool::~TcpConnPool() {
  pthread_mutex_lock(&mutex_);
  while (!sockets_.empty()) {
    int i = sockets_.front();
    sockets_.pop();
    cout << "Close " << i << endl;
    close(i);
  }
  is_destroyed_ = true;
  pthread_cond_broadcast(&cond_);
  pthread_mutex_unlock(&mutex_);
  pthread_mutex_destroy(&mutex_);
}

TcpConnPool *TcpConnPool::Create(const string &remote_str, int conn_num) {
  try {
    return new TcpConnPool(remote_str, conn_num);
  } catch (const logic_error &e) {
    cerr << "Failed to create pool. Why:" << e.what() << endl;
    return NULL; 
  }
}

bool TcpConnPool::Init() {
  if (Conn2Remote(conn_num_) != conn_num_) {
    cerr << " connect failed.\n";
    Destroy();
    return false;
  }
  return true;
}

int TcpConnPool::Conn2Remote(int conn_num) {
  for (int i = 0; i < conn_num; ++i) {
    int sfd = CreateSocket();
    if (sfd == -1) {
      cerr << "Failed to create socket[" << i+1 << "].\n";
      return i;
    }
    pthread_mutex_lock(&mutex_);
    sockets_.push(sfd);
    pthread_mutex_unlock(&mutex_);
  } 
  return conn_num;
}

void TcpConnPool::GetConn(int &fd) {
  if (is_destroyed_) {
    fd = -1;
    return;
  }
  pthread_mutex_lock(&mutex_);
  while (sockets_.empty()) {
    pthread_cond_wait(&cond_, &mutex_);
    if (is_destroyed_) {
      fd = -1;
      pthread_mutex_unlock(&mutex_);
      return;
    }
  }
  fd = sockets_.front();
  printf("Get sfd:%d\n", fd);
  sockets_.pop();
  pthread_mutex_unlock(&mutex_);
}

void TcpConnPool::PutConn(int sfd) {
  pthread_mutex_lock(&mutex_);
  if (sockets_.size() >= conn_num_) {
    pthread_mutex_unlock(&mutex_);
    return;
  }
  printf("Put sfd:%d\n", sfd);
  sockets_.push(sfd);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
}

int TcpConnPool::CreateSocket() {
  const string &host = remote_.host;
  const int port = remote_.port;
  sockaddr_in s_addr;
  memset(&s_addr, 0, sizeof(s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons(port);
  inet_pton(AF_INET, host.c_str(), &s_addr.sin_addr);
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    perror("Socket");
    return -1;
  }
  if (fcntl(sfd, F_GETFL, 0) < 0) {
    cerr << "fcntl error.\n";
    return -1;
  }
  
  int on = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on)) != 0) {
    perror("Set SO_REUSEADDR");
    return -1;
  }
  
  if (setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on)) != 0) {
    perror("Set SO_KEEPALIVE");
    return -1;
  }
  
  if (setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on)) != 0) {
    perror("Set SO_NODELAY");
    return -1;
  }
  struct linger ling = {0, 0};
  if (setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling)) != 0) {
    perror("Set SO_LINGER");
    return -1;
  }
  
  if (connect(sfd, (sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
    perror("Connect");
    return -1;
  }
  return sfd;
}

inline int TcpConnPool::Size() const {
  return conn_num_;
}

inline void TcpConnPool::RemainNum(int &num) const {
  pthread_mutex_lock(&mutex_);
  num = sockets_.size();
  pthread_mutex_unlock(&mutex_);
}


} // namespace util
