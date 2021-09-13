#ifndef TCP_TCPLISTENER_HXX
#define TCP_TCPLISTENER_HXX

#include "TCP/TCP.hxx"
#include <forward_list>
#include <stdexcept>
#include <string>

struct sockaddr;

namespace AMAYNET
{
  class TCPListener : public TCP {    
  private:
    class Connection
    {
    public:
      Connection(TCP *_server);

      ~Connection();

      std::forward_list<TCP*>::iterator iter() const;

      std::forward_list<TCP*>::iterator end();

      void push_back(TCP *val);
  
      void Next();

      void Drop();

      bool Ready();

      fd_set Wait();
      
      void MoveToBegin();

      TCP *data();

    private:
      TCP* server;
      bool connection_droped;
      fd_set _reads;
      std::forward_list<TCP*> _conn_list;
      std::forward_list<TCP*>::iterator _iter;
      std::forward_list<TCP*>::iterator _iter_prev;
      std::forward_list<TCP*>::iterator _last_element;
    };

  public:
    /**
     * @brief create listening socket at port 8080 and 10 listen size
     */
    TCPListener();

    /**
     * @brief create listening socket at `port` with size of `listen_size`
     */
    TCPListener(const std::string &port, int listen_size=default_listen_size);

    /**
     * @brief accept connection from `_sockfd` return the connected socket
     * descriptor. store the client addr buff in `ret_client_buffer`.
     * @param `ret_client_addr` printable connected client addr.
     * @return connected socket descriptor.
     * @return -1 if fail.
     */
    TCP *Accept();

    bool IsConnectionEnd() {
      return m_connection.iter() == m_connection.end();
    }

    TCP* CurrentConnection() {
      return m_connection.data();
    }

    bool IsConnectionReady() {
      return m_connection.Ready();
    }

    void DropConnection() {
      m_connection.Drop();
    }

    void NextConnection() {
      m_connection.Next();
    }

    void ConnectionBegin() {
      m_connection.MoveToBegin();
    }

    /**
     * @brief create listening socket at port
     * @return listening socket file descriptor
     */
    int Listen(bool relisten_fail = true);

  private:
    static void *get_in_addr(sockaddr *sa);

    int _listen_size; // how much listen queue size
    Connection m_connection; // store connected socket info
    fd_set reads;
    static const int default_listen_size = 10;
  };

} // namespace AMAYNET

#endif
