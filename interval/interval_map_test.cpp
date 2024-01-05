#include "interval_map.hpp"
#include <gtest/gtest.h>

class IntervalMapTest : public ::testing::Test {
protected:
  void SetUp() override {
    EXPECT_EQ(getMapFor("m0").size(), 0);
    m1.assign(2, 3, 'Z'); // size 1
    EXPECT_EQ(getMapFor("m1").size(), 1);
    m2.assign(2, 4, 'Z'); // size 2
    m2.assign(6, 8, 'Z'); // size 2
    EXPECT_EQ(getMapFor("m2").size(), 2);
    m3.assign(2, 3, 'X'); // size 3
    m3.assign(5, 7, 'Y');
    m3.assign(9, 12, 'Z');
    EXPECT_EQ(getMapFor("m3").size(), 3);
  }

  using interval_map_t = interval_map<int, char>;
  void TearDown() override {}

  std::map<int, char> getMapFor(std::string i) {
    if (i == "m0") {
      return m0.m_map;
    }
    else if (i == "m1") {
      return m1.m_map;
    }
    else if (i == "m2") {
      return m2.m_map;
    }
    else { // if (i == "m3") {
      return m3.m_map;
    }
  };
  interval_map_t m0{'A'};
  interval_map_t m1{'A'};
  interval_map_t m2{'A'};
  interval_map_t m3{'A'};
};

TEST_F(IntervalMapTest, IntervalValueBeginMx) {
  EXPECT_EQ(m0[-2], 'A');
  EXPECT_EQ(m0[20], 'A');
  EXPECT_EQ(m1[-2], 'A');
  EXPECT_EQ(m1[20], 'Z');
  EXPECT_EQ(m2[-2], 'A');
  EXPECT_EQ(m2[20], 'Z');
}

TEST_F(IntervalMapTest, AssignUpdateFirstEntry) {
  m2.assign(1, 2, 'I');
  EXPECT_EQ(getMapFor("m2").size(), 3);
  EXPECT_EQ(m2[1], 'I');
}

TEST_F(IntervalMapTest, AssignUpdateLastEntry) {
  m3.assign(12, 15, 'I');
  EXPECT_EQ(getMapFor("m3").size(), 4);
  EXPECT_EQ(m3[15], 'I');
  EXPECT_EQ(m3[26], 'I');
  EXPECT_EQ(m3[11], 'Z');
}

TEST_F(IntervalMapTest, AssignFirstEntrySameValue) {
  m0.assign(0, 1, 'A');
  EXPECT_EQ(getMapFor("m0").size(), 0);
  m1.assign(0, 1, 'A');
  EXPECT_EQ(getMapFor("m1").size(), 1);
}

TEST_F(IntervalMapTest, AssignUpperboundCheck) {
  m3.assign(10, 11, 'Z');
  EXPECT_EQ(getMapFor("m1").size(), 1);
}
