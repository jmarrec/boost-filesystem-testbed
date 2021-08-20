#ifndef TEST_FIXTURE_HPP
#define TEST_FIXTURE_HPP

#include <gtest/gtest.h>

class TestFixture : public ::testing::Test
{
 protected:
  // initialize for each test
  virtual void SetUp() override;

  // tear down after each test
  virtual void TearDown() override;

  // initialize static members
  static void SetUpTestSuite();

  // tear down static members
  static void TearDownTestSuite();

};

#endif  // TEST_FIXTURE_HPP
