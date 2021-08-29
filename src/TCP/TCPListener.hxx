#ifndef _AMAY_TCPLISTENER_H
#define _AMAY_TCPLISTENER_H

#include "TCP.hxx"
#include <string>
#include <map>

struct sockaddr;

class TCPListener : public TCP
{
public:
  /**
   * @brief create listening socket at port 8080 and 10 listen size
   */
  TCPListener()
    : TCP("8080"),
      _listen_size(10)
  { Listen(); }

  /**
   * @brief create listening socket at `port` with size of `listen_size`
   */
  TCPListener(const std::string &port, int listen_size)
    : TCP(port),
      _listen_size(listen_size)
  { Listen(); }

  /**
   * @brief accept connection from `_sockfd` return the connected socket descriptor.
   *        store the client addr buff in `ret_client_buffer`.
   * @param `ret_client_addr` printable connected client addr.
   * @return connected socket descriptor.
   * @return -1 if fail.
   */
  TCP Accept();

protected:
  int _listen_size;     // how much listen queue size
  std::map<int, TCP> listener;

  /**
   * @brief create listening socket at port
   * @return listening socket file descriptor
   */
  int Listen();

private:
  void *get_in_addr(sockaddr *sa);

}; 

#endif // _AMAY_TCPLISTENER_H
