
#ifndef _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_TCP_TCPCLIENT_HXX
#define _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_TCP_TCPCLIENT_HXX

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

} // namespace AMAYNET

#endif // _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_TCP_TCPCLIENT_HXX
