#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TCP/TCP.hxx"
#include "TCP/TCPListener.hxx"
#include "TCP/TCPClient.hxx"

namespace net = AMAYNET;
using ::testing::Return;

class TCPMock : public net::TCP {
public:
  MOCK_METHOD(int, Send, (const std::string&), (override));
  MOCK_METHOD(int, Send, (char*, size_t), (override));
  MOCK_METHOD(std::vector<char>, Recv, (size_t), (override));

  void Constructor(const std::string &port, int file_descriptor) /* just assume this is constructor */
  {
    SetPort(port);
    SetFD(file_descriptor);
  }
};

TEST(tcp_test, TestInitnialization) {
  net::TCP tcp1;
  ASSERT_TRUE(tcp1.GetPort().empty());
  ASSERT_EQ(tcp1.GetFD(), 0);
  ASSERT_EQ(tcp1.IsUseSSL(), false);

  net::TCP tcp2("8080", 3);
  ASSERT_EQ(tcp2.GetPort(), "8080");
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
  ASSERT_EQ(tcp2_copy.GetPort(), tcp2.GetPort());
  ASSERT_EQ(tcp2_copy.GetFD(), tcp2.GetFD());
  ASSERT_EQ(tcp2_copy.IsUseSSL(), tcp2.IsUseSSL());

  // test copy assignment
  net::TCP tcp2_copy_assignment = tcp2;
  ASSERT_EQ(tcp2_copy_assignment.GetPort(), tcp2.GetPort());
  ASSERT_EQ(tcp2_copy_assignment.GetFD(), tcp2.GetFD());
  ASSERT_EQ(tcp2_copy_assignment.IsUseSSL(), tcp2.IsUseSSL());

  /* now tcp2_copy and tcp_copy_assignment is equal to tcp */

  // test move
  net::TCP tcp2_move(std::move(tcp2));
  ASSERT_EQ(tcp2_move.GetPort(), tcp2_copy.GetPort());
  ASSERT_EQ(tcp2_move.GetFD(), tcp2_copy.GetFD());
  ASSERT_EQ(tcp2_move.IsUseSSL(), tcp2_copy.IsUseSSL());

  /* now tcp2 is empty*/
  ASSERT_TRUE(tcp2.GetPort().empty());
  ASSERT_EQ(tcp2.GetFD(), 0);
  ASSERT_EQ(tcp2.IsUseSSL(), false);

  // test move assignment
  net::TCP tcp2_move_assignment = std::move(tcp2_copy);
  ASSERT_EQ(tcp2_move_assignment.GetPort(), tcp2_move.GetPort());
  ASSERT_EQ(tcp2_move_assignment.GetFD(), tcp2_move.GetFD());
  ASSERT_EQ(tcp2_move_assignment.IsUseSSL(), tcp2_move.IsUseSSL());

    /* now tcp2_copy is empty*/
  ASSERT_TRUE(tcp2_copy.GetPort().empty());
  ASSERT_EQ(tcp2_copy.GetFD(), 0);
  ASSERT_EQ(tcp2_copy.IsUseSSL(), false);
}
