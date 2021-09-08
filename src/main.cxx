#include "AmayNetConfig.h"

#include <string>
#include <iostream>
#include <vector>

#include "HTTP/Server.hxx"
#include "HTTP/status.hxx"

#include <sys/select.h>
#include <csignal>

using namespace AMAYNET;

sig_atomic_t stopFlag = 0;

static void handler(int)
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

  // main loop
  for (;!stopFlag;) {
    server.Accept();

    // loop through client list
    for (;!server.IsConnectionEnd(); server.NextConnection()) {
      
      if (server.IsConnectionReady()) { // check ready to read connection
        auto request = server.GetRequest();

	if (request.IsValid())
	  server.ServeResource(request.path);

      } // END check ready to read connection
    } // END connection iteration
    server.ConnectionBegin();
  }
  return 0;
}
