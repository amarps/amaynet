#include "AmayNetConfig.hxx"

#include <iostream>
#include <string>
#include <vector>

#include "HTTP/Server.hxx"

#include <csignal>
#include <sys/select.h>

using namespace AMAYNET;

sig_atomic_t stopFlag = 0;

static void handler(int /*unused*/)
{
  stopFlag = 1;
}

int main(int argc, char *argv[]) {
  /* check current project version */
  if (argc == 2) {
    if (argv[1] == std::string("--version") || argv[1] == std::string("-V")) {
      std::cout << "Version: " << AMAYNET_VERSION_MAJOR << "."
                << AMAYNET_VERSION_MINOR << "." << AMAYNET_VERSION_PATCH
                << std::endl;
      return 0;
    }
  }

  signal(SIGINT, &handler);

  // create server
  HTTPServer server("8080");

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
