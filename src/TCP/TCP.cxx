#include "TCP/TCP.hxx"

#include <iostream>
#include <string>
#include <system_error>
#include <tuple>

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h> // memset
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

namespace AMAYNET
{
  TCP& TCP::operator=(TCP &&other) noexcept {
    std::swap(_file_descriptor, other._file_descriptor);
    std::swap(_port, other._port);

    return *this;
  }

  TCP::TCP(const std::string &port, int file_descriptor) {
    SetPort(port);
    SetFD(file_descriptor);
  }

  TCP& TCP::operator=(const TCP &other)
  {
    TCP copy = other;
    std::swap(*this, copy);
    return *this;
  }
  
  TCP::TCP(const TCP &other)
    : _port(other._port),
      _file_descriptor(other._file_descriptor)
  {}

  TCP::TCP(TCP &&other) noexcept
    : _port(std::move(other._port)), _file_descriptor(other._file_descriptor) {
    other._port.clear();
    other._file_descriptor = 0;
  }

  int TCP::Send(const std::string &msg_buf) {
    int sent_byte;
    if (IsUseSSL()) {
      sent_byte = SSL_write(ssl_obj, msg_buf.c_str(), msg_buf.length());
    } else {
      sent_byte = send(_file_descriptor, msg_buf.c_str(), msg_buf.length(), 0);
    }
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  int TCP::Send(char *msg_buf, size_t size) {
    int sent_byte;
    if (IsUseSSL()) {
      sent_byte = send(_file_descriptor, msg_buf, size, 0);
    } else {
      sent_byte = send(_file_descriptor, msg_buf, size, 0);
    }
    
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  std::vector<char> TCP::
  Recv(size_t buf_size) {
    if (IsUseSSL())
      return RecvSSL(buf_size);
    else
      return RecvTCP(buf_size);
  }

  std::vector<char> TCP::
  RecvSSL(size_t buf_size) {
    char recv_buf[buf_size];
    int bytes_recv;
    int err;
    std::vector<char> recv_obj;
      
    bytes_recv =  SSL_read(ssl_obj, recv_buf, buf_size);
    while (bytes_recv < 1) {
      err = SSL_get_error(ssl_obj, bytes_recv);
      std::cout << err << std::endl;
      sleep(1);	  
      if ((err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)) {
	bytes_recv =  SSL_read(ssl_obj, recv_buf, buf_size);
      }else if (err == SSL_ERROR_ZERO_RETURN) {
	return std::vector<char>();
      } else if (err == SSL_ERROR_SYSCALL) {
	return std::vector<char>();
	throw std::system_error();
	break;
      }
    }

    recv_obj.insert(recv_obj.end(), recv_buf, recv_buf + bytes_recv);
    return recv_obj;
  }

  std::vector<char>
  TCP::RecvTCP(size_t buf_size) {
    char recv_buf[buf_size];
    int bytes_recv = recv(_file_descriptor, recv_buf, buf_size, 0);
    if (bytes_recv < 1) {
      if (bytes_recv == 0) {
	return std::vector<char>();
      }
      if (bytes_recv == -1) {
	throw std::system_error(EFAULT, std::generic_category());
      }
    }

    std::vector<char> recv_obj;
    // copy recv_buf value to recv_obj
    recv_obj.insert(recv_obj.end(), recv_buf, recv_buf + buf_size);
  
    return recv_obj;
  }

  void TCP::SetPort(const std::string &port) {
    _port = port;
    _is_use_ssl = false;
    if (port == "443") {
      _is_use_ssl = true;
    }
  }

  int TCP::Close() const {
    return close(_file_descriptor);
  }

  TCP::~TCP() noexcept {
    if (IsUseSSL()) {
      SSL_shutdown(ssl_obj);
      SSL_free(ssl_obj);
      SSL_CTX_free(ssl_ctx);
    }
    Close();
  }

} // namespace AMAYNET
