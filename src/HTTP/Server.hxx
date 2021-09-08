#ifndef _AMAY_HTTPLISTENER_H
#define _AMAY_HTTPLISTENER_H

#include "../TCP/TCPListener.hxx"
#include "status.hxx"
#include <string>
#include <string.h>
#include <sstream>
#include <forward_list>

struct sockaddr_storage;

namespace AMAYNET
{  
  class HTTPServer : TCPListener {
  public:
    enum HTTPRequestMethod {
      INVALID = 0,
      GET,
      POST
    };

    struct HTTPRequest {
      std::string path;
      HTTPRequestMethod request_method;

      HTTPRequest(std::string _path, HTTPRequestMethod _request_method)
	: path(_path),
	  request_method(_request_method)
      { }

      bool IsValid()
      {
	return request_method != HTTPRequestMethod::INVALID;
      }

    };
  public:
    using TCPListener::Accept;
    using TCPListener::NextConnection;
    using TCPListener::IsConnectionEnd;
    using TCPListener::IsConnectionReady;
    using TCPListener::ConnectionBegin;

  public:
    /**
     * @brief
     * @param port
     * @return
     */
    HTTPServer(const std::string &port);
    virtual ~HTTPServer() = default;
    
    /**
     * @brief
     * @param client
     * @param status
     * @param path
     * @return
     */
    void SendResponse(status_T status,
			  const std::string &path = "");

    HTTPRequest GetRequest() {
      auto recv_obj = m_connection.data()->Recv();
      // check if request msg contain \r\n\r\n that indicate end of request
      if(recv_obj.msg_recv.find("\r\n\r\n") != std::string::npos)  {
        if (recv_obj.msg_recv.compare( 0, 5, "GET /")) {
	  // request not start with GET /
	  return HTTPRequest(std::string(), HTTPRequestMethod::INVALID);
        }  else { // request start with GET /
	  auto path = recv_obj.msg_recv.substr(4);
	  auto end_path_pos = path.find(' ');
	  if (end_path_pos == std::string::npos) { // invalid path space not found
	    return HTTPRequest(std::string(), HTTPRequestMethod::INVALID);
	  } else {
	    path = path.substr(0, end_path_pos);
	    return HTTPRequest(path, HTTPRequestMethod::GET);
	  }
	}
      }
      return HTTPRequest(std::string(), HTTPRequestMethod::INVALID);
    };

    void ServeResource(std::string &path);

  private:
    /**
     * @brief
     * @param path
     * @return
     */
    const std::string get_content_type(const std::string &path);
  };

} // namespace AMAY::HTTP
#endif  // _AMAY_HTTPLISTENER_H
