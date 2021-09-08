
#ifndef _AMAYNET_TCPCLIENT_H
#define _AMAYNET_TCPCLIENT_H

#include "TCP.hxx"

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

} // namespace AMAYNET::TCP

#endif // _AMAYNET_TCPCLIENT_H
