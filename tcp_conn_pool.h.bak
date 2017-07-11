#ifndef GALAXY_TCP_CONN_POOL_
#define GALAXY_TCP_CONN_POOL_

/*
 * A simple thread safe tcp conn pool. Maybe buggy
 * */

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <pthread.h>

namespace util {

// TODO: Should do something about validating host and port.
struct Remote {
  static const char kSpliter;
  explicit Remote(const std::string &remote_str);
  std::string host;
  int port;
};
  
class TcpConnPool {

public:

  static TcpConnPool *Create(const std::string &remote_str, int conn_num = 5);

  void Destroy();

  void GetConn(int &fd);

  void PutConn(int sfd);

  void DeleteConn(int sfd);

  int Size() const;

  void FixPool(bool &r);
  
  void RemainNum(int &num) const;

protected:
  // Heap allocated only
  TcpConnPool(const std::string &remote_str, int conn_num);

  ~TcpConnPool();

private:

  bool Init();

  int Conn2Remote(int conn_num);

  int CreateSocket();

  Remote remote_;

  std::queue<int> sockets_;

  size_t conn_num_;

  int broken_conn_num_;

  pthread_t fix_thread_;
  
  bool is_destroyed_;

  mutable pthread_mutex_t mutex_;
  pthread_cond_t cond_;

private:
  // No copying allowed
  TcpConnPool(const TcpConnPool &);
  TcpConnPool &operator=(const TcpConnPool &);

};

} // namespace util

#endif // GALAXY_TCP_CONN_POOL_
