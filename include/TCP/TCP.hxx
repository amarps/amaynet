#ifndef TCP_TCP_HXX
#define TCP_TCP_HXX

#include <iostream>
#include <string>
#include <vector>

// struct addrinfo;
// struct SSL_CTX;
// struct SSL;

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <fcntl.h>

namespace AMAYNET
{
  /* base class for TCP */
  class TCP {
  public: /* construtor */
    TCP() = default;
    TCP(const std::string &port, int file_descriptor);
    virtual ~TCP() noexcept;
    TCP(const TCP &other); 
    TCP(TCP &&other) noexcept;
    TCP& operator=(const TCP &other);
    TCP& operator=(TCP &&other) noexcept;

  public:
    /**
     * @brief send message in string to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    virtual int Send(const std::string &msg_buf);

    /**
     * @brief send message in byte to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    virtual int Send(char *msg, size_t size);

    /**
     * @brief receive message to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent, and recv message
     */
    virtual std::vector<char>
    Recv(size_t buf_size=_default_recv_size);

    /* close file descriptor */
    int Close() const;

    inline std::string GetPort() { return _port; }
    inline int GetFD() const { return _file_descriptor; };
    bool IsUseSSL() { return _is_use_ssl; };
    virtual SSL_CTX *GetSSLCTX() { return ssl_ctx; } 
    virtual SSL *GetSSLObj() { return ssl_obj; }

  protected:
    int SetFD(int fd) { _file_descriptor = fd; return fd; }
    
    /* set port and use ssl if (port == 443) */
    void SetPort(const std::string &port);
    SSL_CTX *InitSSLCTX() {
      SSL_library_init();
      OpenSSL_add_all_algorithms();
      SSL_load_error_strings();
      ssl_ctx = SSL_CTX_new(TLS_client_method());
      if (!ssl_ctx) {
	std::cerr << "SSL_CTX_new() failed." << std::endl;
      }
      
      return ssl_ctx;
    }

    SSL *InitSSL(const std::string &hostname) {
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

    std::vector<char>
    RecvSSL(size_t buf_size=_default_recv_size);

    std::vector<char>
    RecvTCP(size_t buf_size=_default_recv_size);

  private:
    std::string _port;
    int _file_descriptor;
    bool _is_use_ssl;
    SSL_CTX *ssl_ctx;
    SSL *ssl_obj;
    static const int _default_recv_size = 2047;
  };

} // namespace AMAYNET

#endif // TCP_TCP_HXX
