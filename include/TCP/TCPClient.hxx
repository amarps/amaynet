
#ifndef TCP_TCPCLIENT_HXX
#define TCP_TCPCLIENT_HXX

#include "TCP/TCP.hxx"

#include <iostream>
#include <string>

#include <netdb.h>
#include <sys/select.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

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

    virtual ~TCPClient();

    int Connect();

    SSL_CTX *InitTLS();

    void SetTimeout(size_t sec, size_t micro_sec);

    int Send(const std::string &msg_buf) const;

    std::vector<char> Recv(size_t buf_size) const;

    TimeOut_T GetTimeout() const;

    bool Ready();

  private:
    std::string _hostname;
    timeval _timeval;
    fd_set _reads;
    SSL_CTX *ssl_ctx;
    SSL *ssl_obj;
  };

} // namespace AMAYNET

#endif // TCP_TCPCLIENT_HXX
