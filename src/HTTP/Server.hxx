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
    using TCPListener::Accept;
    using TCPListener::m_connection;

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
     * @param content
     * @return
     */
    void SendHttpResponse(status_T status,
			  const std::string &content = "");

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
