#ifndef _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_TCP_TCP_HXX
#define _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_TCP_TCP_HXX

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
    // empty contstructor
    TCP()
      :_port(std::string())
    { }

    TCP(const std::string &port, int file_descriptor);
    
    //! Copy constructor
    TCP(const TCP &other);

    //! Copy assignment operator
    TCP& operator=(const TCP &other);

    //! Move constructor
    TCP(TCP &&other) noexcept;

    //! Destructor
    virtual ~TCP() noexcept;

    //! Move assignment operator
    TCP& operator=(TCP &&other) noexcept ;
  
    /**
     * @brief send message in string to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    int Send(const std::string &msg_buf) const;

    /**
     * @brief send message in byte to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    int Send(void *msg, size_t size) const;

    /**
     * @brief receive message to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent, and recv message
     */
    Recv_T Recv(size_t buf_size=_default_recv_size) const;

    /* close file descriptor */
    int Close() const;

    inline std::string GetPort() { return _port; }
    inline int GetFD() const { return _file_descriptor; };

  protected:
    int SetFD(int fd) { _file_descriptor = fd; return fd; }
    void SetPort(const std::string &port);

  private:
    std::string _port;
    int _file_descriptor = -1;
    static const int _default_recv_size = 2047;
  };

} // namespace AMAYNET

#endif // _HOME_AMAY_CODE_CPP_NETWORKING_AMAYHTTPS_SRC_TCP_TCP_HXX
