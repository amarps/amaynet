#ifndef TCP_TCP_HXX
#define TCP_TCP_HXX

#include <string>
#include <vector>

typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_st SSL;

namespace AMAYNET
{
  class TCP {
  public:
    TCP();
    TCP(const std::string &port, int file_descriptor);
    virtual ~TCP() noexcept;
    TCP(const TCP &other); 
    TCP(TCP &&other) noexcept;
    TCP& operator=(const TCP &other);
    TCP& operator=(TCP &&other) noexcept;

    /**
     * @brief send message in string to this socket
     * @param msg_buf message to send
     * @return number of bytes sent
     */
    virtual int Send(const std::string &msg_buf);

    /**
     * @brief send message in byte to this socket
     * @param msg_buf message to send
     * @return number of bytes sent
     */
    virtual int Send(char *msg, size_t size);

    /**
     * @brief receive message from this socket
     * @param buf_size length of message to receive
     * @return message
     */
    virtual std::vector<char>
    Recv(size_t buf_size=_default_recv_size);

    /* close file descriptor */
    int Close() const;

    inline std::string GetPort() { return _port; }
    inline int GetFD() const { return _file_descriptor; }
    bool IsUseSSL() const { return _is_use_ssl; }
    virtual SSL_CTX *GetSSLCTX() { return ssl_ctx; } 
    virtual SSL *GetSSLObj() { return ssl_obj; }

  protected:
    int SetFD(int fd) { _file_descriptor = fd; return fd; }
    
    /* set port and use ssl if (port == 443) */
    void SetPort(const std::string &port);
    SSL_CTX *InitSSLCTX();
    SSL *InitSSL(const std::string &hostname);

    std::vector<char>
    RecvSSL(size_t buf_size=_default_recv_size);
    std::vector<char>
    RecvTCP (size_t buf_size=_default_recv_size) const;

  private:
    std::string _port;
    int _file_descriptor;
    bool _is_use_ssl;
    SSL_CTX *ssl_ctx;
    SSL *ssl_obj;
    static const int _default_recv_size = 2047;
  };

} // namespace AMAYNET

#endif // TCP_TCP_HXX
