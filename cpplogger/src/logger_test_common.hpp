#include "logger.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

class StreamTest : public ::testing::Test {
protected:
  void SetUp() override { testing::internal::CaptureStdout(); }

  void TearDown() override {}

  void CheckOutput(std::string expected) {
    EXPECT_EQ(expected, testing::internal::GetCapturedStdout());
  }
  Logger _logger{std::cout};
};

class FileStreamTest : public ::testing::Test {
protected:
  void SetUp() override {
    using std::ios;
    stream = std::ofstream{std::string{"/tmp/test_logger_file"}, ios::trunc | ios::out};
  }
  void TearDown() override {
    stream.close();
  }

  void CheckOutput(std::string expected) { EXPECT_EQ(expected, output_buffer.str()); }

  template <typename T>
  void AppendLineToStream(const T &l) {
    output_buffer << l;
    _logger << l;
  }

  std::ofstream stream; 
  Logger _logger{stream};
  std::stringstream output_buffer;
};
