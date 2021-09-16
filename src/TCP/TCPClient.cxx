#include "TCP/TCPClient.hxx"

#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>


namespace AMAYNET
{

  TCPClient::TCPClient(const std::string &hostname, const std::string &port)
    : _hostname(hostname)
  {
    SetPort(port);
    SetTimeout(2, 0);
    SetFD(Connect());
  }
  
  int TCPClient::Connect() {
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
    if (!ssl_obj) {
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
    if (!cert) {
      std::cerr << "SSL_get_peer_certificate(ssl_obj) failed." << std::endl;
      return -1;
    }

    char *tmp;
    if ((tmp = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0))) {
      std::cout << "subject: " << tmp << std::endl;      
    }

    if ((tmp = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0))) {
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
