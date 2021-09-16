#include "TCP/TCP.hxx"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string.h> // memset

#include <iostream>
#include <string>
#include <system_error>
#include <tuple>

namespace AMAYNET
{
  TCP& TCP::operator=(TCP &&other) 
 noexcept   {
    std::swap(_file_descriptor, other._file_descriptor);
    std::swap(_port, other._port);

    return *this;
  }

  TCP::TCP(const std::string &port, int file_descriptor)
    : _port(port), _file_descriptor(file_descriptor) {}

  TCP& TCP::operator=(const TCP &other)
  {
    TCP copy = other;
    std::swap(*this, copy);
    return *this;
  }
  
  TCP::TCP(const TCP &other)
    : _port(other._port),
      _file_descriptor(other._file_descriptor)
  {}

  TCP::TCP(TCP &&other) noexcept
    : _port(std::move(other._port)), _file_descriptor(other._file_descriptor) {
    other._port.clear();
    other._file_descriptor = 0;
  }

  int TCP::Send(const std::string &msg_buf) const {
    int sent_byte = send(_file_descriptor, msg_buf.c_str(), msg_buf.length(), 0);
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  int TCP::Send(char *msg_buf, size_t size) const {
    int sent_byte = send(_file_descriptor, msg_buf, size, 0);
    if (sent_byte < 0) {
      throw std::system_error(EFAULT, std::generic_category());
    }
    return sent_byte;
  }

  std::vector<char>
  TCP::Recv(size_t buf_size) const {
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

  int TCP::Close() const {
    return close(_file_descriptor);
  }

  TCP::~TCP() noexcept { Close(); }

} // namespace AMAYNET
