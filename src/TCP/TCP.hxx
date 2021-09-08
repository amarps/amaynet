#ifndef _AMAYNET_TCP_H
#define _AMAYNET_TCP_H

#include <string>
#include <vector>

struct addrinfo;

namespace AMAYNET
{
  /**
   * 
   * base class for TCP
   */
  class TCP {
  public:
    
  private:
    struct Recv_T {
      size_t bytes_recv;
      std::string msg_recv;

      Recv_T (size_t _bytes_recv, const std::string &msg)
	: msg_recv(msg)
      {
	bytes_recv  = _bytes_recv;
      }
    };
  public:
    //! Copy constructor
    TCP(const TCP &other);

    //! Copy assignment operator
    TCP& operator=(const TCP &other);

    //! Move constructor
    TCP(TCP &&other) noexcept;

    //! Destructor
    virtual ~TCP() noexcept;

    //! Move assignment operator
    TCP& operator=(TCP &&other);
  
    /**
     * @brief send message in string to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    int Send(const std::string &msg_buf);

    /**
     * @brief send message in byte to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    int Send(void *msg, size_t size);

    /**
     * @brief receive message to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent, and recv message
     */
    Recv_T Recv(size_t buf_size=2047);

    /* close file descriptor */
    int Close();

    inline const std::string GetPort() { return _port; }
    inline int GetFD() { return _file_descriptor; };

    explicit TCP(const std::string &port);

    TCP(const std::string &port, int file_descriptor);

  protected:
    std::string _port = 0;
    int _file_descriptor = -1;
  };

} // namespace AMAY::TCP

#endif // _AMAYNET_TCP_H
