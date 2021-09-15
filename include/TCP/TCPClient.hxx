
#ifndef TCP_TCPCLIENT_HXX
#define TCP_TCPCLIENT_HXX

#include "TCP/TCP.hxx"

#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/select.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

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

    SSL_CTX
    *InitTLS() {
      SSL_library_init();
      OpenSSL_add_all_algorithms();
      SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
      return ctx;
    }

    void
    SetTimeout(size_t sec, size_t micro_sec) {
      _timeval.tv_sec = sec;
      _timeval.tv_usec = micro_sec;
    }

    int Send(const std::string &msg_buf) {
      int sent_byte = SSL_write(ssl_obj, msg_buf.c_str(), msg_buf.length());
      std::cout << msg_buf << std::endl;
      if (sent_byte < 0) {
	throw std::system_error(EFAULT, std::generic_category());
      }
      return sent_byte;
    }

    std::vector<char>
    Recv(size_t buf_size) const {
      char recv_buf[buf_size];
      int bytes_recv = SSL_read(ssl_obj, recv_buf, buf_size);

      sleep(1);
      std::cout << bytes_recv << std::endl;
      if (bytes_recv == 0) {
	return std::vector<char>();
      }

      std::vector<char> recv_obj;
      // copy recv_buf value to recv_obj
      recv_obj.insert(recv_obj.end(), recv_buf, recv_buf + buf_size);
      
      return recv_obj;
  
    }

    TimeOut_T
    GetTimeout() const
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
      std::cout << "fd isset" << FD_ISSET(GetFD(), &_reads) << std::endl;
      return FD_ISSET(GetFD(), &_reads);
    }

  private:
    std::string _hostname;
    timeval _timeval;
    fd_set _reads;
    SSL_CTX *ssl_ctx;
    SSL *ssl_obj;
  };

} // namespace AMAYNET

#endif // TCP_TCPCLIENT_HXX
