
#ifndef TCP_TCPCLIENT_HXX
#define TCP_TCPCLIENT_HXX

#include "TCP/TCP.hxx"

#include <iostream>
#include <string>

#include <netdb.h>
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

    virtual ~TCPClient() {
      SSL_shutdown(ssl_obj);
      SSL_free(ssl_obj);
      SSL_CTX_free(ssl_ctx);
    }
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
      int bytes_recv;
      int err;
      std::vector<char> recv_obj;
      
      bytes_recv =  SSL_read(ssl_obj, recv_buf, buf_size);
      while (true) {
	if (bytes_recv < 1) {
	  err = SSL_get_error(ssl_obj, bytes_recv);
	  std::cout << err << std::endl;
	  sleep(1);
	  if ((err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)) {
	    bytes_recv =  SSL_read(ssl_obj, recv_buf, buf_size);
	    continue;
	  } else if (err == SSL_ERROR_ZERO_RETURN) {
	    return std::vector<char>();
	  } else if (err == SSL_ERROR_SYSCALL){
	    return std::vector<char>();
	    throw std::system_error();
	  }
	}
	break;
      }

      recv_obj.insert(recv_obj.end(), recv_buf, recv_buf + bytes_recv);
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

      if (FD_ISSET(GetFD(), &_reads) > 0)
	return true;
      else
	return false;
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
