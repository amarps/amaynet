#include "TCPListener.hxx"

extern "C" {
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
}

#include <string>
#include <system_error>

using std::string;

void *TCPListener::get_in_addr(sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);  
}


int TCPListener::Listen() {
  /* specify required address */
  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  /* get network address */
  addrinfo *res;
  if ((getaddrinfo(NULL, _port.c_str(), &hints, &res)) != 0) {
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }

  /* constructing socket */
  _file_descriptor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (_file_descriptor == -1) {
    perror("Error ");
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }

  if (bind(_file_descriptor, res->ai_addr, res->ai_addrlen) == -1) {
    close(_file_descriptor);
    freeaddrinfo(res);
    perror("Error ");
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }
  freeaddrinfo(res);

  if (listen(_file_descriptor, _listen_size) == -1) {
    perror("Error ");
    close(_file_descriptor);
    throw std::system_error(EFAULT, std::generic_category());
    return -1;
  }
  
  return _file_descriptor;
}

TCP TCPListener::Accept() {
  struct sockaddr_storage client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int newfd = accept(_file_descriptor, (struct sockaddr*)&client_addr, &client_addr_len);

  if (newfd == -1) { /* accept failed */
    perror("Error ");
    throw std::system_error(EFAULT, std::generic_category());
  }

  /* convert network client address to printable client address */
  // TODO create connected store
  char addrbuf[INET6_ADDRSTRLEN];
  inet_ntop(client_addr.ss_family,
	    get_in_addr((struct sockaddr *)&client_addr),
	    addrbuf, INET6_ADDRSTRLEN);

  return TCP(_port, newfd);
}
