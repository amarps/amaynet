#ifndef _AMAY_HTTPLISTENER_H
#define _AMAY_HTTPLISTENER_H

#include "TCP/TCPListener.hxx"
#include <string>

class HTTPServ : public TCPListener
{
public:
  HTTPServ(const std::string &port)
    : TCPListener(port, 10)
  { }

  
};

#endif // _AMAY_HTTPLISTENER_H
