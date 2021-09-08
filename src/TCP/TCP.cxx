#include "TCP.hxx"

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h> // memset

#include <string>
#include <system_error>
#include <iostream>
#include <tuple>

namespace AMAYNET
{
  TCP& TCP::operator=(TCP &&other) 
  {
    std::swap(_file_descriptor, other._file_descriptor);
    std::swap(_port, other._port);

    return *this;
  }

  TCP::TCP(const std::string &port)
    :_port(port)
  {}

  TCP::TCP(const std::string &port, int file_descriptor)
    : _port(port), _file_descriptor(file_descriptor) {}

  TCP& TCP::operator=(const TCP &other)
  {
    TCP copy = other;
    std::swap(*this, copy);
    return *this;
  }
  
  TCP::TCP(const TCP &other)
    : _file_descriptor(other._file_descriptor),
      _port(other._port)
  {}

  TCP::TCP(TCP &&other) noexcept
    : _port(other._port), _file_descriptor(other._file_descriptor) {
    other._file_descriptor = 0;
    other._port.clear();
  }

  int TCP::Send(const std::string &msg_buf) {
    int sent_byte = send(_file_descriptor, msg_buf.c_str(), msg_buf.length(), 0);
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  int TCP::Send(void *msg_buf, size_t size) {
    int sent_byte = send(_file_descriptor, msg_buf, size, 0);
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  TCP::Recv_T TCP::Recv(size_t buf_size) {
    char recv_buf[buf_size];
    size_t bytes_recv = recv(_file_descriptor, recv_buf, buf_size, 0);
    if (bytes_recv < 1) {
      throw std::system_error(EFAULT, std::generic_category());
    }

    Recv_T recv_obj{bytes_recv, recv_buf};
  
    return recv_obj;
  }

  int TCP::Close() {
    return close(_file_descriptor);
  }

  TCP::~TCP() noexcept { Close(); }

} // namespace AMAY::TCP
