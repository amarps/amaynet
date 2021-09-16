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
    SetFD(Connect());
  }

  TCPClient::
  ~TCPClient() {
    SSL_shutdown(ssl_obj);
    SSL_free(ssl_obj);
    SSL_CTX_free(ssl_ctx);
  }

  SSL_CTX *TCPClient::
  InitTLS() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    return ctx;
  }

  void TCPClient::
  SetTimeout(size_t sec, size_t micro_sec) {
    _timeval.tv_sec = sec;
    _timeval.tv_usec = micro_sec;
  }

  int TCPClient::
  Send(const std::string &msg_buf) const {
    int sent_byte = SSL_write(ssl_obj, msg_buf.c_str(), msg_buf.length());
    std::cout << msg_buf << std::endl;
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  std::vector<char> TCPClient::
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
	}else if (err == SSL_ERROR_ZERO_RETURN) {
	  return std::vector<char>();
	} else if (err == SSL_ERROR_SYSCALL){
	  return std::vector<char>();
	  throw std::system_error();
	  break;
	}
      }
    }

    recv_obj.insert(recv_obj.end(), recv_buf, recv_buf + bytes_recv);
    return recv_obj;
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
    /* Get SSL context */
    ssl_ctx = InitTLS();

    /* specify required address */
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* get network address */
    addrinfo *res;
    if ((getaddrinfo(_hostname.c_str(), GetPort().c_str(), &hints, &res)) != 0) {
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

    /* Connected */
    /* Now connect to ssl */
    ssl_obj = SSL_new(ssl_ctx);
    if (ssl_obj == nullptr) {
      std::cerr << "SSL_new() failed." << std::endl;
      return -1;
    }

    if (!SSL_set_tlsext_host_name(ssl_obj, _hostname.c_str())) {
      std::cerr << "SSL_set_tlsext_host_name() failed." << std::endl;
      ERR_print_errors_fp(stderr);
      return -1;
    }

    SSL_set_fd(ssl_obj, sockfd);
    if (SSL_connect(ssl_obj) == -1) {
      std::cerr << "SSL_connect() failed." << std::endl;
      return -1;
    }

    X509 *cert = SSL_get_peer_certificate(ssl_obj);
    if (cert == nullptr) {
      std::cerr << "SSL_get_peer_certificate(ssl_obj) failed." << std::endl;
      return -1;
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
    flags = fcntl(sockfd, F_GETFL, 0);

    // set nonblocking socket
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    return sockfd;
  }
} // namespace AMAYNET
