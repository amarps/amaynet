#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include "HTTP/Server.hxx"
#include "TCP/TCPClient.hxx"
#include "common.hxx"

TEST(http_test, TestLoopbackConnection)
{
  AMAYNET::HTTPServer server("9000");
  ASSERT_STREQ(server.GetPort().c_str(), "9000");
  ASSERT_EQ(server.GetFD(), 0);
  EXPECT_STREQ(server.GetHostDir().c_str(), ".");

  // server accepting client
  server.Listen();
  AMAYNET::TCPClient client("127.0.0.1", "9000");
  server.Accept();

  // test getrequest
  // invalid request
  client.Send("GET /");
  auto request = server.GetRequest();
  EXPECT_TRUE(!request.IsValid());
  EXPECT_TRUE(request.path.empty());
  EXPECT_EQ(request.request_method, request.INVALID);

  // valid GET request
  client.Send("GET / \r\n\r\n");
  request = server.GetRequest();
  EXPECT_TRUE(request.IsValid());
  EXPECT_EQ(request.request_method, request.GET);
  EXPECT_TRUE(request.path == "/");

  // sending response
  AMAYNET::status_T ok(200, "hello", "hello there");
  server.SendResponse(ok);
  EXPECT_TRUE(!client.Recv().empty());
}
