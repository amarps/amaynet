#include "TCP/TCP.hxx"

#include <iostream>
#include <string>
#include <system_error>
#include <tuple>

#include <fcntl.h>
#include <netdb.h>
#include <string.h> // memset
#include <sys/socket.h>
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
    :_port(other._port),
     _file_descriptor(other._file_descriptor),
     _is_use_ssl(other._is_use_ssl),
     ssl_ctx(other.ssl_ctx),
     ssl_obj(other.ssl_obj)
  { }

  TCP::TCP(TCP &&other) noexcept
    : _port(other._port),
      _file_descriptor(other._file_descriptor),
      _is_use_ssl(other._is_use_ssl),
      ssl_ctx(other.ssl_ctx),
      ssl_obj(other.ssl_obj) {
    other._port.clear();
    other._file_descriptor = 0;
    other.ssl_ctx = nullptr;
    other.ssl_obj = nullptr;
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
    if (IsUseSSL()) {
      return RecvSSL(buf_size);
    }
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
  TCP::RecvTCP(size_t buf_size) const {
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

  SSL *TCP::
  InitSSL(const std::string &hostname) {
    ssl_obj = SSL_new(ssl_ctx);
    if (ssl_obj == nullptr) {
      std::cerr << "SSL_new() failed." << std::endl;
      return nullptr;
    }

    if (!SSL_set_tlsext_host_name(ssl_obj, hostname.c_str())) {
      std::cerr << "SSL_set_tlsext_host_name() failed." << std::endl;
      ERR_print_errors_fp(stderr);
      return nullptr;
    }

    SSL_set_fd(ssl_obj, GetFD());
    if (SSL_connect(ssl_obj) == -1) {
      std::cerr << "SSL_connect() failed." << std::endl;
      return nullptr;
    }

    X509 *cert = SSL_get_peer_certificate(ssl_obj);
    if (cert == nullptr) {
      std::cerr << "SSL_get_peer_certificate(ssl_obj) failed." << std::endl;
      return nullptr;
    }

    char *tmp;
    if ((tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)) != nullptr) {
      std::cout << "subject: " << tmp << std::endl;      
    }

    if ((tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)) != nullptr) {
      std::cout << "issuer: " << tmp << std::endl;
    }

    X509_free(cert);

    // get current socket flags
    int flags;
    flags = fcntl(GetFD(), F_GETFL, 0);

    // set nonblocking socket
    fcntl(GetFD(), F_SETFL, flags | O_NONBLOCK);
    return ssl_obj;
  }

  SSL_CTX *TCP::
  InitSSLCTX() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (ssl_ctx==nullptr) {
      std::cerr << "SSL_CTX_new() failed." << std::endl;
    }
      
    return ssl_ctx;
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
    if (IsUseSSL() && (ssl_obj || ssl_ctx)) {
      SSL_shutdown(ssl_obj);
      SSL_free(ssl_obj);
      SSL_CTX_free(ssl_ctx);
    }
    Close();
  }

} // namespace AMAYNET
