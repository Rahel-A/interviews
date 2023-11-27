#include <chrono>
#include <iostream>

using namespace std;
using namespace chrono;

class RateLimiter {
public:
  RateLimiter(int max_ops, int rate_limit_ms)
      : m_max_ops_rate(max_ops), m_rate_limit_ms(milliseconds(rate_limit_ms)) {
    updateRateLimitCycle();
  };

  bool isAllowed() {
    auto now =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    if (now.count() >= m_cycle_ends_at.count()) { // if past rate limit cycle
      updateRateLimitCycle();
    }
    return m_current_ops_rate++ < m_max_ops_rate;
  };

private:
  void updateRateLimitCycle() {
    auto now =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    m_cycle_ends_at = now + m_rate_limit_ms;
    m_current_ops_rate = 0;
  };

  int m_current_ops_rate;
  const int m_max_ops_rate;
  const milliseconds m_rate_limit_ms;
  milliseconds m_cycle_ends_at;
};
