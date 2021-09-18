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
} // namespace AMAYNET
