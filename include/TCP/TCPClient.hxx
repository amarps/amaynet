
#ifndef TCP_TCPCLIENT_HXX
#define TCP_TCPCLIENT_HXX

#include "TCP/TCP.hxx"

#include <iostream>
#include <string>

#include <sys/select.h>

namespace AMAYNET
{
  class TCPClient : public TCP
  {
  public:
    struct TimeOut_T {
      int second;
      int micro_second;
    };
    
    TCPClient(const std::string &hostname, const std::string &port);

    virtual ~TCPClient() = default;
    int Connect();

    void SetTimeout(size_t sec, size_t micro_sec)
    {
      _timeval.tv_sec = sec;
      _timeval.tv_usec = micro_sec;
    }

    TimeOut_T GetTimeout() const
    {
      TimeOut_T ret;
      ret.second = _timeval.tv_sec;
      ret.micro_second = _timeval.tv_usec;
      return ret;
    };

    bool Ready() {
      FD_ZERO(&_reads);
      FD_SET(GetFD(), &_reads);
      if (select(GetFD() + 1, &_reads, 0, 0, &_timeval) < 0) {
	perror("Error ");
	return false;
      }
      return FD_ISSET(GetFD(), &_reads);
    }

  private:
    std::string _hostname;
    timeval _timeval;
    fd_set _reads;
  };

} // namespace AMAYNET

#endif // TCP_TCPCLIENT_HXX
