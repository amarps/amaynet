#ifndef COMMON_HXX
#define COMMON_HXX

#include <string>

namespace AMAYNET
{
  struct URL_T {
    std::string protocol;
    std::string host;
    std::string path;

    URL_T() = default;
    URL_T(std::string url);
  };

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

  /* message and detail from https://httpstatuses.com */
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


} // namespace AMAYNET

#endif // COMMON_HXX
