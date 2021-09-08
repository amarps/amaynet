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
  class HTTPServer : public Listener {
  public:
    /**
     * @brief
     * @param port
     * @return
     */
    HTTPServer(const std::string &port);
    
    /**
     * @brief
     * @param client
     * @param status
     * @param content
     * @return
     */
    void SendHttpResponse(Base *client, status_T status,
			  const std::string &content = "");

    void ServeResource(Base *client, std::string &path);

    virtual ~HTTPServer() = default;

  private:
    /**
     * @brief
     * @param path
     * @return
     */
    const std::string get_content_type(const std::string &path);
    std::forward_list<Base *> m_connection_list; // store connected socket info
  };

} // namespace AMAY::HTTP
#endif  // _AMAY_HTTPLISTENER_H
