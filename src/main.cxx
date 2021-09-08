#include "AmayHTTPSConfig.h"

#include <string>
#include <iostream>
#include <vector>

#include "HTTP/Server.hxx"
#include "HTTP/status.hxx"

#include <sys/select.h>

using namespace AMAYNET;

int main(int argc, char *argv[]) {
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
  
  // create server
  HTTPServer server("8080");

  // main loop
  for (;;) {
    server.Accept();

    // loop through client list
    for (;server.m_connection.iter() != server.m_connection.end();
	 server.m_connection.Next()) {
      
      std::cout << "current fd "<< server.m_connection.data()->GetFD() << std::endl;

      if (server.m_connection.Ready()) { // check ready to read connection

        auto recv_obj = server.m_connection.data()->Recv();

	// check if request msg contain \r\n\r\n that indicate end of request
	if (recv_obj.msg_recv.find("\r\n\r\n") != std::string::npos) {
	  if (recv_obj.msg_recv.compare(0, 5, "GET /")) { // request not start with GET /
	    server.SendHttpResponse(server.m_connection.data(), GetStatus(Status::BAD_REQUEST));
	  } else { // request start with GET /
	    //get path
	    auto path = recv_obj.msg_recv.substr(4);
	    auto end_path_pos = path.find(' ');
	    if (end_path_pos == std::string::npos) { // space not found
	      server.SendHttpResponse(server.m_connection.data(), GetStatus(Status::BAD_REQUEST));
	    } else {
	      path = path.substr(0, end_path_pos);
	      server.ServeResource(server.m_connection.data(), path);
	    }
	  }
	}
      } // END check ready to read connection
    } // END connection loop
    server.m_connection.MoveToBegin();

  }
  return 0;
}
