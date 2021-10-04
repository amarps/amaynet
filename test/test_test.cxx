#include <gmock/gmock.h>
#include <gtest/gtest.h>

/*
this file is only for checking is gtest and gmock working
*/

TEST(MainTest, CheckMainExecutable)
{
  // expect equality
  ASSERT_EQ(3 * 2, 6); // gtest working
}
