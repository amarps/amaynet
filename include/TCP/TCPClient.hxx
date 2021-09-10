
#ifndef TCP_TCPCLIENT_HXX
#define TCP_TCPCLIENT_HXX

#include "TCP/TCP.hxx"

#include <string>

namespace AMAYNET
{
  class TCPClient: public TCP
  {
  public:
    TCPClient(const std::string &hostname, const std::string &port);

    virtual ~TCPClient() = default;

  protected:
    std::string _hostname;
    int Connect();
  };

} // namespace AMAYNET

#endif // TCP_TCPCLIENT_HXX
