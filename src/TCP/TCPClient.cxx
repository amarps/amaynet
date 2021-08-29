#include "TCPClient.hxx"

#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

#include <system_error>


int TCPClient::Connect() {

    /* specify required address */
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  /* get network address */
  addrinfo *res;
  if ((getaddrinfo(_hostname.c_str(), _port.c_str(), &hints, &res)) != 0) {
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }

  /* constructing socket */
  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }

  if(connect(sockfd,
	     res->ai_addr,
	     res->ai_addrlen)) {
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }

  freeaddrinfo(res);

  return sockfd;
}
