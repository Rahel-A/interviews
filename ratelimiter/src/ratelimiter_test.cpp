#include "ratelimiter.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

class RateLimiterTest : public ::testing::Test {
protected:
  void SetUp() override {
    rl = std::make_unique<RateLimiter>(RateLimiter{ops_limit, rate_limit_ms});
  }
  void TearDown() override {}
  static const int rate_limit_ms{100};
  static const int ops_limit{5};
  std::unique_ptr<RateLimiter> rl;
};

TEST_F(RateLimiterTest, testWithinRate) {
  for (int i = 0; i < ops_limit; i++) {
    ASSERT_TRUE(rl->isAllowed());
  }
}

TEST_F(RateLimiterTest, testOutsideRate) {
  for (int i = 0; i < ops_limit; i++) {
    ASSERT_TRUE(rl->isAllowed()) << "Within ops limit failed: " << i;
  }
  for (int i = 0; i < ops_limit; i++) {
    ASSERT_FALSE(rl->isAllowed()) << "Outside ops limit failed " << i;
  }
}

TEST_F(RateLimiterTest, testRateCycleReset) {
  for (int i = 0; i < ops_limit; i++) {
    ASSERT_TRUE(rl->isAllowed()) << "Within ops limit failed: " << i;
  }

  auto const time = rate_limit_ms;
  std::this_thread::sleep_for(std::chrono::milliseconds(time));

  for (int i = 0; i < ops_limit; i++) {
    ASSERT_TRUE(rl->isAllowed()) << "Within ops limit failed " << i;
  }
}
