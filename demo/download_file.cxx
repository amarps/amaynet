#include <iostream>
#include "TCP/TCPClient.hxx"

int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cout << "Example Usage: download_file google.com 80" << std::endl;
    return 1;
  }

  AMAYNET::TCPClient connector(argv[1], argv[2]);
  connector.Connect();
  connector.Send("GET /index.html HTTP/1.1\r\n"
		 "\r\n\r\n");
  std::vector<char> buffer;
  while (true) {
    if(connector.Ready()) {
      auto recv_obj = connector.Recv(2048);
      if (recv_obj.bytes_recv == 0) {
	break;
      }
      buffer.insert(buffer.end(), recv_obj.msg_recv.begin(), recv_obj.msg_recv.end());
    } else {
      break;
    }
  }
  for (auto b : buffer) {
    std::cout << b;
  }
  std::cout << std::endl;


  return 0;
}
