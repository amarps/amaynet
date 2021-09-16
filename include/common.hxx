#ifndef AMAYNETCOMMON_HXX
#define AMAYNETCOMMON_HXX

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

#endif // AMAYNETCONFIG_HXX
