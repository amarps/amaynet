#include <iostream>
#include <sstream>
#include "TCP/TCPClient.hxx"
#include "common.hxx"
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: download_file https://www.google.com" << std::endl;
    return 1;
  }
  
  AMAYNET::URL_T url(argv[1]);
  AMAYNET::TCPClient connector(url.host, url.protocol);
 
  std::stringstream request;
  request << "GET " << url.path <<  " HTTP/1.0\r\n"
	  << "Host: " << url.host << ':' << url.protocol << "\r\n"
	  << "Connection: close\r\n"
	  << "\r\n";

  connector.Send(request.str());

  std::vector<char> buffer;
 
  while (true) {
    if (connector.Ready()) {
      auto recv_obj = connector.Recv(2096);
      if(recv_obj.size() > 0) {
	buffer.insert(buffer.end(), recv_obj.begin(), recv_obj.end());
      } else if (recv_obj.empty()) {
	break;
      }
    } else {
      break;
    }
  }

  for (const auto &c : buffer)
    printf("%c", c);

  return 0;
}
