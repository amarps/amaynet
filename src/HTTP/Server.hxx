#ifndef _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_HTTP_SERVER_HXX
#define _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_HTTP_SERVER_HXX

#include "../TCP/TCPListener.hxx"
#include <forward_list>
#include <sstream>
#include <string>
#include <string.h>
#include <utility>

struct sockaddr_storage;

namespace AMAYNET
{  
  class HTTPServer : public TCPListener {
  public:
    struct HTTPRequest {
      std::string path;
      enum Method {
	INVALID = 0,
	GET,
	POST
      } request_method;

      HTTPRequest(std::string _path, Method _request_method)
	: path(std::move(_path)),
	  request_method(_request_method)
      { }

      bool IsValid() const
      {
	return request_method != Method::INVALID;
      }

    };

    /**
     * 
     * message and detail from https://httpstatuses.com
     */
    struct status_T {
      int code;
      std::string msg;
      std::string detail;
  
      status_T(int _code, const std::string &_msg, const std::string &_detail)
	:code(_code),
	 msg(_msg),
	 detail(_detail) {}
    };

    enum Status {
      // Success status
      OK,
      CREATED,
      ACCEPTED,

      // Client Error
      BAD_REQUEST,
      UNAUTHORIZED,
      FORBIDDEN,
      NOT_FOUND,
      REQUEST_TIMEOUT,

      // Server Error
      NOT_IMPLEMENTED,
      HTTP_VERSION_NOT_SUPPORTED,
    };
  
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
    void SendResponse(const status_T& status, const std::string &content = "");

    HTTPRequest GetRequest() {
      auto recv_obj = CurrentConnection()->Recv();
      // check if request msg contain \r\n\r\n that indicate end of request
      if (recv_obj.msg_recv.find("\r\n\r\n") == std::string::npos) {
        return HTTPRequest(std::string(), HTTPRequest::INVALID);
      }
      
      auto requestMethod = HTTPRequest::INVALID;
      auto path = std::string();

      const int num_method_element = 2;
      std::string str_req_methods[num_method_element] {
	"GET /",
	"POST /"
      };

      for (int mthd_i=0; mthd_i < num_method_element; mthd_i++) {
	if (recv_obj.msg_recv
	    .compare(0, str_req_methods[mthd_i].length(),
		     str_req_methods[mthd_i])) {
	  path = recv_obj.msg_recv.substr(str_req_methods[mthd_i].length());
	  requestMethod = (HTTPRequest::Method)(mthd_i + 1);
	  break;
	}
      }

      auto end_path_pos = path.find(' ');
      if (end_path_pos == std::string::npos) { // invalid path. space not found
	return HTTPRequest(std::string(), HTTPRequest::INVALID);
      }

      path = path.substr(0, end_path_pos);
      return HTTPRequest(path, requestMethod);
    };

    void ServeResource(std::string &path);

  protected:
    /**
     * @brief
     * @param path
     * @return
     */
    static std::string GetContentType(const std::string &path);

    static status_T GetStatus(Status _status);
    
  };

} // namespace AMAYNET
#endif // _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_HTTP_SERVER_HXX
