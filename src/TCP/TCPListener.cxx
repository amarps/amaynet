#include "TCP/TCPListener.hxx"

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

#include <iostream>
#include <string>
#include <system_error>

namespace AMAYNET {

  TCPListener::TCPListener()
    : _listen_size(default_listen_size),
      m_connection(this)
  { }

  TCPListener::TCPListener(const std::string &port, int listen_size)
    : TCP(port, 0),
      _listen_size(listen_size),
      m_connection(this)
   { }  

  void *TCPListener::get_in_addr(sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      sockaddr_in *sa_in;
      memcpy(&sa_in, &sa, sizeof(sa));
      return &sa_in->sin_addr;
    }
    
    sockaddr_in6 *sa_in6;
    memcpy(&sa_in6, &sa, sizeof(sa));
    return &sa_in6->sin6_addr;  
  }


  int TCPListener::Listen(bool relisten_fail) {
    /* specify required address */
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* get network address */
    addrinfo *res;
    if ((getaddrinfo(NULL, GetPort().c_str(), &hints, &res)) != 0) {
      throw std::system_error(EFAULT, std::generic_category());
      return -1;
    }

    // socket file descriptor
    auto fd = SetFD(socket(res->ai_family, res->ai_socktype, res->ai_protocol));
    if (fd == -1) {
      perror("Error ");
      throw std::system_error(EFAULT, std::generic_category());
      return -1;
    }

    int bind_status = bind(fd, res->ai_addr, res->ai_addrlen);
    if(relisten_fail)
      {
	while(bind_status == -1) {
	  perror("Error ");
	  std::cout << "re binding" << std::endl;
	  bind_status = bind(fd, res->ai_addr, res->ai_addrlen);
	  sleep(1.0);
	}
	std::cout << "address ready" << std::endl;
      } else {
      if (bind_status == -1) {
	close(fd);
	perror("Error ");
	throw std::system_error(EFAULT, std::generic_category());
	return -1;
      }  
    }
  
    freeaddrinfo(res);

    if (listen(fd, _listen_size) == -1) {
      perror("Error ");
      close(fd);
      throw std::system_error(EFAULT, std::generic_category());
      return -1;
    }
  
    return fd;
  }

  TCP *TCPListener::Accept() {
    reads = m_connection.Wait();
    if (!FD_ISSET(GetFD(), &reads)) {  // server fd ready to read
      return nullptr;
}
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int newfd = accept(GetFD(), (struct sockaddr*)&client_addr, &client_addr_len);

    if (newfd == -1) { /* accept failed */
      perror("Error ");
      throw std::system_error(EFAULT, std::generic_category());
    }

    /* convert network client address to printable client address */
    // TODO create connected store
    char addrbuf[INET6_ADDRSTRLEN];
    inet_ntop(client_addr.ss_family,
              get_in_addr((struct sockaddr *)&client_addr), addrbuf,
              INET6_ADDRSTRLEN);
    
    auto *new_connection = new TCP(GetPort(), newfd);
    m_connection.push_back(new_connection);
    
    return new_connection;
  }

} // namespace AMAYNET
