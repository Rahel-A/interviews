#include <iostream>
#include <map>

class IntervalMapTest;

template <typename K, typename V> class interval_map {
  friend class IntervalMapTest;
  friend void IntervalMapTest();
  V m_valBegin;
  std::map<K, V> m_map;

public:
  // constructor associates whole range of K with val
  interval_map(V const &val) : m_valBegin(val) {}

  // Assign value val to interval [keyBegin, keyEnd).
  // Overwrite previous values in this interval.
  // Conforming to the C++ Standard Library conventions, the interval
  // includes keyBegin, but excludes keyEnd.
  // If !( keyBegin < keyEnd ), this designates an empty interval,
  // and assign must do nothing.
  void assign(K const &keyBegin, K const &keyEnd, V const &val) {
    // ===== is this going to be the first entry?
    const auto it_lower_bound = m_map.lower_bound(keyBegin);
    if ((val == m_valBegin) && 
        !(m_map.cbegin()->first < it_lower_bound->first)) {
      // first entry cannot be the same as m_valBegin
      return;
    }

    // ===== Get a hint on where the iterator should be. (optmized insert)
    bool skip_prev_value_check{false};
    typename std::map<K, V>::const_iterator it_prev{it_lower_bound};

    if (m_map.size() < 2) {
      it_prev = m_map.cbegin();
      skip_prev_value_check = true;
    } else {
      // hint needs to point 1 element before where we want to insert.
      --it_prev;
    }

    // ==== check next and prev. key - the values range must be canonical
    if ((!skip_prev_value_check && (it_prev->second == val)) ||
        (m_map.upper_bound(keyEnd)->second == val)) {
      return;
    }

    m_map.insert_or_assign(keyBegin, val);
  }

  // look-up of the value associated with key
  V const &operator[](K const &key) const {
    auto it = m_map.upper_bound(key);
    if (it == m_map.begin()) {
      return m_valBegin;
    } else {
      return (--it)->second;
    }
  }
};

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.
