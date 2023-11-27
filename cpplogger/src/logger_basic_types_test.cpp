#include "logger_test_common.hpp"

/**
* \page custom_types Custom types
* ```
* class BasicType {
* public:
*   BasicType(int data) : m_data(data) {};
*   friend Logger &operator<<(Logger& os, const BasicType& c);
* 
* private:
*   int m_data{42};
* };
* 
* Logger &operator<<(Logger& os, const BasicType &o) {
*   return os << o.m_data;
* }
* ```
*/
class BasicType {
public:
  BasicType(int data) : m_data(data) {};
  friend Logger &operator<<(Logger& os, const BasicType& c);

private:
  int m_data{42};
};

Logger &operator<<(Logger& os, const BasicType &o) {
  return os << o.m_data;
}

TEST_F(StreamTest, OutputBasicType) {
  _logger << BasicType{30};
  CheckOutput("30");
}

