#include "TCP.hxx"

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h> // memset

#include <string>
#include <system_error>
#include <iostream>

int TCP::Send(const std::string &msg_buf) {
  int sent_byte = send(_file_descriptor, msg_buf.c_str(), msg_buf.length(), 0);
  if (sent_byte < 0) {
    throw std::system_error(EFAULT, std::generic_category());
  }
  std::cout << "send success" << std::endl;
  return sent_byte;
}

const std::string TCP::Recv(size_t buf_size) {
  char recv_buf[buf_size];
  if (recv(_file_descriptor, recv_buf, 4096, 0) < 0) {
    throw std::system_error(EFAULT, std::generic_category());
  }
  std::cout << "recv success" << std::endl;
  return std::string(recv_buf);
}

int TCP::Close() {
  return close(_file_descriptor);
}

TCP::~TCP() {
  Close();
}
