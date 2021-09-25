#include "common.hxx"

namespace AMAYNET
{
  URL_T::URL_T(std::string url)
  {
    unsigned pos = 0;
    std::string match = "https://";
    if (url.find(match, pos) != std::string::npos) {
      pos = match.length();
      protocol = "443";
    } else {
      match = "http://";
      if (url.find(match, pos) != std::string::npos) {
	pos = match.length();
      }
      protocol = "80";
    }

    auto temp_pos = url.find_first_of('/', pos);
    if (temp_pos != std::string::npos) {
      host = url.substr(pos, temp_pos - pos);
      pos = temp_pos;
      path = url.substr(pos);
    } else {
      host = url.substr(pos);
      path = '/';
    }
  }

  HTTPRequest::HTTPRequest(std::string _path, Method _request_method)
    : path(std::move(_path)),
      request_method(_request_method)
  { }

  bool HTTPRequest::IsValid() const
  {
    return request_method != Method::INVALID;
  }

  status_T::status_T(int _code, const std::string &_msg, const std::string &_detail)
    :code(_code),
     msg(_msg),
     detail(_detail) {}
} // namespace AMAYNET
