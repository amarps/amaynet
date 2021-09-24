#ifndef HTTP_SERVER_HXX
#define HTTP_SERVER_HXX

#include "../TCP/TCPListener.hxx"
#include "common.hxx"
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
    HTTPServer(const std::string &port);
    virtual ~HTTPServer() = default;

    HTTPRequest GetRequest();
    void SendResponse(const status_T& status, const std::string &content = "");
    void ServeResource(std::string &path, int chunk_size=default_chunk_size);

    std::string GetHostDir() { return _hostdir; }
    void SetHostDir(const std::string &dir) { _hostdir = dir; }

  protected:
    static std::string GetContentType(const std::string &path);
    static status_T GetStatus(Status _status);
    
  private:
    static constexpr int default_chunk_size = 10;
    std::string _hostdir = ".";
  };

} // namespace AMAYNET
#endif // HTTP_SERVER_HXX
