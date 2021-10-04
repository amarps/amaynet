#include <gmock/gmock.h>
#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include "TCP/TCP.hxx"
#include "TCP/TCPClient.hxx"
#include "TCP/TCPListener.hxx"

namespace net = AMAYNET;
using ::testing::Return;

class TCPMock : public net::TCP
{
public:
  TCPMock() = default;
  TCPMock(const std::string &port, int file_descriptor)
  {
    SetPort(port);
    SetFD(file_descriptor);
  }

  int Send(char *msg_buf, size_t size) override
  {
    if (GetFD() < 1 || GetPort().empty()) {
      ADD_FAILURE() << "Expected valid port and file descriptor";
      return -1;
    }
    if (!msg_buf) {
      return 0;
    }
    if (size > strlen(msg_buf)) {
      ADD_FAILURE() << "Error size out of range";
      return -1;
    }
    return size;
  }
};

TEST(tcp_test, TestInitnialization)
{
  net::TCP tcp1;
  ASSERT_TRUE(tcp1.GetPort().empty()) << "Check empty initialization";
  ASSERT_EQ(tcp1.GetFD(), 0);
  ASSERT_EQ(tcp1.IsUseSSL(), false);

  net::TCP tcp2("8080", 3);
  ASSERT_EQ(tcp2.GetPort(), "8080") << "Check initialization";
  ASSERT_EQ(tcp2.GetFD(), 3);
  ASSERT_EQ(tcp2.IsUseSSL(), false);

  /** @BUG: call to ssl on test cause segfault */
  // net::TCP tcp2_ssl("443", 3);
  // ASSERT_EQ(tcp2_ssl.GetPort(), "443");
  // ASSERT_EQ(tcp2_ssl.GetFD(), 3);
  // // IsUseSSL() will be true if running at port 443
  // ASSERT_EQ(tcp2_ssl.IsUseSSL(), true);

  // test copy
  net::TCP tcp2_copy(tcp2);
  ASSERT_EQ(tcp2_copy.GetPort(), tcp2.GetPort()) << "Test copy constructructor";
  ASSERT_EQ(tcp2_copy.GetFD(), tcp2.GetFD());
  ASSERT_EQ(tcp2_copy.IsUseSSL(), tcp2.IsUseSSL());

  // test copy assignment
  net::TCP tcp2_copy_assignment = tcp2;
  ASSERT_EQ(tcp2_copy_assignment.GetPort(), tcp2.GetPort())
      << "Test copy assignment";
  ASSERT_EQ(tcp2_copy_assignment.GetFD(), tcp2.GetFD());
  ASSERT_EQ(tcp2_copy_assignment.IsUseSSL(), tcp2.IsUseSSL());

  /* now tcp2_copy and tcp_copy_assignment is equal to tcp */

  // test move
  net::TCP tcp2_move(std::move(tcp2));
  ASSERT_EQ(tcp2_move.GetPort(), tcp2_copy.GetPort())
      << "Test move conststructor";
  ASSERT_EQ(tcp2_move.GetFD(), tcp2_copy.GetFD());
  ASSERT_EQ(tcp2_move.IsUseSSL(), tcp2_copy.IsUseSSL());

  /* now tcp2 is empty*/
  ASSERT_TRUE(tcp2.GetPort().empty());
  ASSERT_EQ(tcp2.GetFD(), 0);
  ASSERT_EQ(tcp2.IsUseSSL(), false);

  // test move assignment
  net::TCP tcp2_move_assignment = std::move(tcp2_copy);
  ASSERT_EQ(tcp2_move_assignment.GetPort(), tcp2_move.GetPort())
      << "Test move assignment";
  ASSERT_EQ(tcp2_move_assignment.GetFD(), tcp2_move.GetFD());
  ASSERT_EQ(tcp2_move_assignment.IsUseSSL(), tcp2_move.IsUseSSL());

  /* now tcp2_copy is empty*/
  ASSERT_TRUE(tcp2_copy.GetPort().empty());
  ASSERT_EQ(tcp2_copy.GetFD(), 0);
  ASSERT_EQ(tcp2_copy.IsUseSSL(), false);
}

TEST(tcp_test, TestSend)
{
  TCPMock tcp1;

  // send empty string to unspecified file descriptor
  EXPECT_NONFATAL_FAILURE(tcp1.AMAYNET::TCP::Send(""),
                          "Expected valid port and file descriptor");
  EXPECT_NONFATAL_FAILURE(tcp1.Send(0, 0),
                          "Expected valid port and file descriptor");

  // send empty string to valid file descriptor
  TCPMock tcp2("8080", 3);
  EXPECT_EQ(tcp2.AMAYNET::TCP::Send(""), 0) << "Send empty string";
  EXPECT_EQ(tcp2.Send(0, 0), 0) << "Send empty buffer";

  // send ok
  EXPECT_EQ(tcp2.AMAYNET::TCP::Send("assalamualaikum beijing"), 23)
      << "Send empty string";
  char msg[] = "assalamualaikum beijing";
  EXPECT_EQ(tcp2.Send(msg, 23), 23) << "Send empty buffer";

  // send Out of range (size > msg.size)
  EXPECT_NONFATAL_FAILURE(tcp2.Send(msg, 25), "Error size out of range");
}

TEST(tcp_test, TestLoopbackConnection)
{
  net::TCPListener server("9000");
  server.Listen(true);

  // no connection on server
  EXPECT_TRUE(server.IsConnectionEnd());

  net::TCPClient client("127.0.0.1", "9000");
  server.Accept();

  // now there is connection on server
  EXPECT_TRUE(!server.IsConnectionEnd());
  EXPECT_TRUE(server.CurrentConnection());

  // now client and server can comunicate now
  // client send and server recv
  EXPECT_EQ(client.Send("Hello"), 5) << "client send message";
  auto recv_buf = server.CurrentConnection()->Recv(5);
  EXPECT_STREQ(recv_buf.data(), "Hello") << "server recv message";

  // server send and client recv
  EXPECT_TRUE(!client.Ready()) << "client not ready to recv";
  EXPECT_EQ(server.CurrentConnection()->Send("Seramat pagi"), 12)
      << "server send message";
  EXPECT_TRUE(client.Ready()) << "client ready to recv";
  recv_buf = client.Recv(12);
  EXPECT_STREQ(recv_buf.data(), "Seramat pagi") << "client recv message";

  // server drop client
  server.DropConnection();
  EXPECT_TRUE(server.IsConnectionEnd());
}
