#ifndef TCP_TCPCLIENT_HXX
#define TCP_TCPCLIENT_HXX

#include "TCP.hxx"

#include <iostream>
#include <string>

namespace AMAYNET {
class TCPClient : public TCP
{
public:
  struct TimeOut_T
  {
    int second;
    int micro_second;
  };

  TCPClient(const std::string &hostname, const std::string &port);
  virtual ~TCPClient() {}

  void SetTimeout(size_t sec, size_t micro_sec);
  TimeOut_T GetTimeout() const;
  bool Ready();

protected:
  int Connect();

private:
  std::string _hostname;
  timeval _timeval;
  fd_set _reads;
};

} // namespace AMAYNET

#endif // TCP_TCPCLIENT_HXX
