#ifndef TCP_TCP_HXX
#define TCP_TCP_HXX

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
    // empty contstructor
    TCP() = default;

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
    virtual int Send(const std::string &msg_buf) const;

    /**
     * @brief send message in byte to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent
     */
    virtual int Send(char *msg, size_t size) const;

    /**
     * @brief receive message to this socket
     * @param msg_buf content of the message to send
     * @return number of bytes sent, and recv message
     */
    virtual std::vector<char>
    Recv(size_t buf_size=_default_recv_size) const;

    /* close file descriptor */
    int Close() const;

    inline std::string GetPort() { return _port; }
    inline int GetFD() const { return _file_descriptor; };

  protected:
    int SetFD(int fd) { _file_descriptor = fd; return fd; }
    void SetPort(const std::string &port) { _port = port; }

  private:
    std::string _port;
    int _file_descriptor = 0;
    static const int _default_recv_size = 2047;
  };

} // namespace AMAYNET

#endif // TCP_TCP_HXX
