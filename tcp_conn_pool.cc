#include "tcp_conn_pool.h"
#include "util_functions.h"
#include "rwlock_guard.h"
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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
  
  int r = pthread_create(&fix_thread_, NULL, FixPool, this);
  if (r != 0) {
    Destroy();
    throw logic_error("Can not create fix routine.");
  }
}

void TcpConnPool::Destroy() {
  delete this;
}

TcpConnPool::~TcpConnPool() {
  pthread_mutex_lock(&mutex_);
  for (int i : sockets_)
    close(i);
  is_destroyed_ = true;
  pthread_cond_broadcast(&cond_);
  pthread_mutex_unlock(&mutex_);
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
    sockets_.push_back(sfd);
    pthread_mutex_unlock(&mutex_);
  } 
  return conn_num;
}

int TcpConnPool::GetConn() {
  if (is_destroyed_) return -1;
  pthread_mutex_lock(&mutex_);
  while (sockets_.empty()) {
    pthread_cond_wait(&cond_, &mutex_);
    if (is_destroyed_) {
      pthread_mutex_unlock(&mutex_);
      return -1;
    }
  }
  int r = sockets_.front();
  sockets_.pop_front();
  pthread_mutex_unlock(&mutex_);
  return r;
}

void TcpConnPool::PutConn(int sfd) {
  pthread_mutex_lock(&mutex_);
  if (sockets_.size() >= conn_num_)
    return;
  sockets_.push_back(sfd);
  pthread_cond_signal(&cond_);
  pthread_mutex_unlock(&mutex_);
}

} // namespace util
