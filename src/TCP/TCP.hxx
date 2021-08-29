#ifndef _AMAYNET_TCP_H
#define _AMAYNET_TCP_H

#include <string>

struct addrinfo;

/**
 * 
 * ba class for TCP
 */
class TCP {
public:
  /**
   * @brief send message to this socket
   * @param msg_buf content of the message to send
   * @return number of bytes sent
   */
  int Send(const std::string &msg_buf);

  /**
   * @brief receive message to this socket
   * @param msg_buf content of the message to send
   * @return number of bytes sent
   */
  const std::string Recv(size_t buf_size);

  /* close file descriptor */
  int Close();

  inline const std::string GetPort() { return _port; }
  inline int GetFileDescriptor() { return _file_descriptor; };

  virtual ~TCP();

  TCP(const std::string &port, int file_descriptor)
    :_port(port),
     _file_descriptor(file_descriptor)
  {}

protected:
  TCP() = default;
  TCP(const TCP& other) = default;
  TCP(TCP &&other) = default;
  
  explicit TCP(const std::string &port)
    :_port(port)
  {}
  
  std::string _port = 0;
  int _file_descriptor = -1;

};

#endif // _AMAYNET_TCP_H
