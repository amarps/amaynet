#include "AmayHTTPSConfig.h"
#include "HTTP/HTTPServ.hxx"
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
  /* check current project version */
  if (argc == 2) {
    if (argv[1] == std::string("--version") || argv[1] == std::string("-V")) {
      std::cout << "Version: "
		<< AMAYHTTPS_VERSION_MAJOR << "."
		<< AMAYHTTPS_VERSION_MINOR << "."
		<< AMAYHTTPS_VERSION_PATCH << std::endl;
      return 0;
    }
  }

  TCPListener server("8080", 10);

  server.Accept();
  
  return 0;
}
