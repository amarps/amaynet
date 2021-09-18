#include "TCP/TCPClient.hxx"

#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>

namespace AMAYNET
{

  TCPClient::
  TCPClient(const std::string &hostname, const std::string &port)
    : _hostname(hostname)
  {
    SetPort(port);
    SetTimeout(2, 0);
    if (IsUseSSL()) {
      TCP::InitSSLCTX();
    }

    SetFD(Connect());

    /* Connected */
    if (IsUseSSL()) {
      TCP::InitSSL(_hostname);
    }
  }

  void TCPClient::
  SetTimeout(size_t sec, size_t micro_sec) {
    _timeval.tv_sec = sec;
    _timeval.tv_usec = micro_sec;
  }

  TCPClient::TimeOut_T TCPClient::
  GetTimeout() const
  {
    TimeOut_T ret;
    ret.second = _timeval.tv_sec;
    ret.micro_second = _timeval.tv_usec;
    return ret;
  };

  bool TCPClient::
  Ready() {
    FD_ZERO(&_reads);
    FD_SET(GetFD(), &_reads);
    if (select(GetFD() + 1, &_reads, 0, 0, &_timeval) < 0) {
      perror("Error ");
      return false;
    }

    return (FD_ISSET(GetFD(), &_reads) > 0);
  }

  
  int TCPClient::
  Connect() {
    /* specify required address */
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* get network address */
    addrinfo *res;
    if ((getaddrinfo(_hostname.c_str(), GetPort().c_str(), &hints, &res)) != 0) {
      /* if you get this error and hostname is not in your local
	 network check your internet connection */
      std::cerr << "getaddrinfo() failed." << std::endl;
      return -1;
    }

    /* constructing socket */
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
      std::cerr << "socket() failed." << std::endl;
      return -1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
      std::cerr << "connect() failed." << std::endl;
      return -1;
    }

    freeaddrinfo(res);
    
    return sockfd;
  }
} // namespace AMAYNET
