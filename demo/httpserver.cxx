#include "HTTP/Server.hxx"

#include <iostream>
#include <csignal>

using namespace AMAYNET;

sig_atomic_t stopFlag = 0;

static void handler(int /*unused*/)
{
  stopFlag = 1;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: httpserver 8080";
  }
  
  signal(SIGINT, &handler); // signal for terminate program

  // create server
  HTTPServer server(argv[1]);

  server.Listen();

  // main loop
  for (;stopFlag == 0;) {
    server.Accept();

    // loop through client list
    for (;!server.IsConnectionEnd(); server.NextConnection()) {
      
      if (server.IsConnectionReady()) { // check ready to read connection
        auto request = server.GetRequest();
	if (request.IsValid()) {
	  server.ServeResource(request.path);
	}

      } // END check ready to read connection
    } // END connection iteration
    server.ConnectionBegin();
  }
  return 0;
}
