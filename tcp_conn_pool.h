#ifndef GALAXY_TCP_CONN_POOL_
#define GALAXY_TCP_CONN_POOL_

#include <string>
#include <vector>
#include <map>
#include <list>
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

  int GetConn();

  void PutConn(int sfd);

  void DeleteConn(int sfd);

protected:
  // Heap allocated only
  TcpConnPool(const std::string &remote_str, int conn_num);

  ~TcpConnPool();

private:

  bool Init();

  int Conn2Remote(int conn_num);

  int CreateSocket();

  static void *FixPool(void *arg);
  
  Remote remote_;

  std::list<int> sockets_;

  int conn_num_;

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
