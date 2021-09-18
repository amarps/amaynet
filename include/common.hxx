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


} // namespace AMAYNET

#endif // COMMON_HXX
