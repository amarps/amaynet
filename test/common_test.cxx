#include "common.hxx"
#include <gtest/gtest.h>

TEST(CommonTest, URL_Ttest)
{
  // Call to default constructor will return empty object
  AMAYNET::URL_T url0;
  ASSERT_TRUE(url0.protocol.empty());
  ASSERT_TRUE(url0.host.empty());
  ASSERT_TRUE(url0.path.empty());

  // test url without specifying protocol and path
  AMAYNET::URL_T url1("example.com");
  ASSERT_EQ(url1.protocol, "80");
  ASSERT_EQ(url1.host, "example.com");
  ASSERT_EQ(url1.path, "/");

  // test http url
  AMAYNET::URL_T url2("http://example.com");
  ASSERT_EQ(url2.protocol, "80");
  ASSERT_EQ(url2.host, "example.com");
  ASSERT_EQ(url2.path, "/");

  // test http url with path
  AMAYNET::URL_T url3("http://example.com/page2.html");
  ASSERT_EQ(url3.protocol, "80");
  ASSERT_EQ(url3.host, "example.com");
  ASSERT_EQ(url3.path, "/page2.html");

  // test https url with path
  AMAYNET::URL_T url4("https://example.com/page2.html");
  ASSERT_EQ(url4.protocol, "443");
  ASSERT_EQ(url4.host, "example.com");
  ASSERT_EQ(url4.path, "/page2.html");

  /** @Note invalid url is accepted at URL_t. i will fix it later.
   *  for now invalid url is handled by getaddrinfo()
   */
  // test url without specifying protocol and path
  AMAYNET::URL_T invalid_url("cqwrryc8oyacayy3");
  ASSERT_EQ(invalid_url.protocol, "80");
  ASSERT_EQ(invalid_url.host, "cqwrryc8oyacayy3");
  ASSERT_EQ(invalid_url.path, "/");
}
