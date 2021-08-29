#ifndef _AMAYNET_TCPCLIENT_H
#define _AMAYNET_TCPCLIENT_H

#include "TCP.hxx"

#include <string>

class TCPClient: public TCP
{
public:
  TCPClient(const std::string &hostname, const std::string &port)
    : TCP(port),
      _hostname(hostname)
  {
    _file_descriptor = Connect();
  }

  virtual ~TCPClient() = default;

protected:
  std::string _hostname;
  int Connect();
};

#endif // _AMAYNET_TCPCLIENT_H
