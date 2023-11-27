#include "logger_test_common.hpp"
#include <iostream>
#include <ostream>
#include <fstream>

TEST(NullTest, OutputConsole) {
  std::ofstream stream{std::string{"/tmp/qowpej pqowje pqjwe /test_logger_file"}, std::ios::trunc | std::ios::out};
  Logger l{stream};
  l << "test"; // bad file no output logged...
}

TEST_F(StreamTest, OutputInt) {
  _logger << 5;
  CheckOutput("5");
}

TEST_F(StreamTest, OutputMultiInt) {
  _logger << 5 << 42;
  CheckOutput("542");
}

TEST_F(StreamTest, OutputString) {
  _logger << "5";
  CheckOutput("5");
}

TEST_F(StreamTest, OutputStringNewline) {
  _logger << "hi\nbye";
  CheckOutput("hi\nbye");
}

TEST_F(FileStreamTest, OutputInt) {
  AppendLineToStream(5);
  CheckOutput("5");
}

TEST_F(FileStreamTest, OutputMultiInt) {
  AppendLineToStream(5);
  AppendLineToStream(42);
  CheckOutput("542");
}

TEST_F(FileStreamTest, OutputString) {
  AppendLineToStream("5");
  CheckOutput("5");
}

TEST_F(FileStreamTest, OutputStringNewline) {
  AppendLineToStream("hi\n");
  AppendLineToStream("bye");
  CheckOutput("hi\nbye");
}

