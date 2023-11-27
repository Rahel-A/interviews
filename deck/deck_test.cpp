#include "deck.hpp"
#include <gtest/gtest.h>

class DeckTestInt : public ::testing::Test {
protected:
  void SetUp() override {
    d1_.add(4);
    d2_.add(2);
    d2_.add(0);
  }

  void TearDown() override {}

  typedef Deck<int> deck_t;
  // 0 elements deck
  deck_t d0_;
  // 1 element deck
  deck_t d1_;
  // 2 elements deck
  deck_t d2_;
  std::mt19937 gen_{/* seed */ 5544};
};

TEST_F(DeckTestInt, ContainerReqConstructorEmptyParam) {
  deck_t d{};
  EXPECT_TRUE(d.empty());
}

TEST_F(DeckTestInt, ContainerReqConstructorCopyParamEq) {
  EXPECT_EQ(d2_, deck_t{d2_});
}

TEST_F(DeckTestInt, ContainerReqConstructorMoveParamEq) {
  auto d2_copy{d2_}; // FIXED AFTER SUBMISSION
  EXPECT_EQ(d2_copy, deck_t{std::move(d2_)});
  EXPECT_TRUE(d2_.empty());
}

TEST_F(DeckTestInt, ContainerReqAssignCopyParamEq) {
  deck_t d2_copy;
  EXPECT_EQ(d2_, d2_copy = d2_);
}

TEST_F(DeckTestInt, ContainerReqMoveParamEq) {
  deck_t d2_new;
  EXPECT_NE(d2_, d2_new = std::move(d2_));
  EXPECT_EQ(d2_.size(), 0);
}

TEST_F(DeckTestInt, ContainerReqIterators) {
  auto deck{d2_};
  EXPECT_EQ(deck.cend() - deck.cbegin(), deck.size());
  EXPECT_EQ(*deck.cbegin(), deck.top());
  EXPECT_EQ(*(deck.cend() - 1), deck.bottom());
}

TEST_F(DeckTestInt, ContainerReqSwapStatic) {
  auto d1_copy{d1_};
  auto d2_copy{d2_};

  d1_.swap(d2_);

  EXPECT_EQ(d1_.size(), d2_copy.size());
  EXPECT_EQ(d2_.size(), d1_copy.size());
  EXPECT_EQ(d1_, d2_copy);
  EXPECT_EQ(d2_, d1_copy);
}

TEST_F(DeckTestInt, ContainerReqSwapThis) {
  auto d1_copy{d1_};
  auto d2_copy{d2_};

  deck_t::swap(d1_, d2_);

  EXPECT_EQ(d1_.size(), d2_copy.size());
  EXPECT_EQ(d2_.size(), d1_copy.size());
  EXPECT_EQ(d1_, d2_copy);
  EXPECT_EQ(d2_, d1_copy);
}

TEST_F(DeckTestInt, ContainerReqSize) {
  EXPECT_EQ(0, d0_.size());
  EXPECT_EQ(2, d2_.size());
}

TEST_F(DeckTestInt, ContainerReqMaxSize) {
  // expect a large number, in runtime depends on memory available
  EXPECT_GT(d0_.max_size(), 100'000'000);
}

TEST_F(DeckTestInt, ContainerReqEmpty) { EXPECT_TRUE(d0_.empty()); }

TEST_F(DeckTestInt, ContainerReqNonEmpty) { EXPECT_FALSE(d1_.empty()); }

TEST_F(DeckTestInt, PropertyTopOfEmptyDeck) {
  auto &deck{d0_};

  EXPECT_FALSE(deck.top().has_value());
}

TEST_F(DeckTestInt, PropertyTopOfDeckWorks) {
  const auto card{0}; // was inserted last, therefore shall be top
  auto &deck{d2_};

  EXPECT_EQ(card, deck.top().value());
}

TEST_F(DeckTestInt, PropertyBottomOfEmptyDeck) {
  auto &deck{d0_};

  EXPECT_FALSE(deck.top().has_value());
}

TEST_F(DeckTestInt, PropertyBottomOfDeckWorks) {
  const auto card{2}; // was inserted first, therefore shall be bottom
  auto &deck{d2_};

  EXPECT_EQ(card, deck.bottom().value());
}

TEST_F(DeckTestInt, PropertyDrawEmpty) {
  auto &deck{d0_};

  EXPECT_FALSE(deck.draw().has_value());
}

TEST_F(DeckTestInt, PropertyDrawOfSingleDeckEmpties) {
  auto &deck{d1_};

  EXPECT_EQ(1, deck.size());
  deck.draw();
  EXPECT_EQ(0, deck.size());
}

TEST_F(DeckTestInt, PropertyDrawFromLargeDeckWorks) {
  const auto card{0};
  auto &deck{d2_};

  EXPECT_EQ(card, deck.top().value());
  deck.draw();
  EXPECT_NE(card, deck.top().value());
}

TEST_F(DeckTestInt, PropertyAddEmptyDeck) {
  const auto card{5};
  auto &deck{d0_};

  deck.add(card);
  EXPECT_EQ(card, deck.top().value());
}

TEST_F(DeckTestInt, PropertyAddPrepends) {
  const auto card{5};
  auto &deck{d2_};

  EXPECT_NE(card, deck.top().value());
  deck.add(card);
  EXPECT_EQ(card, deck.top().value());
}

TEST_F(DeckTestInt, PropertyShuffleEmpty) {
  auto &deck{d0_};

  EXPECT_NO_THROW(deck.shuffle(gen_));
}

TEST_F(DeckTestInt, PropertyShuffleRemovesOrder) {
  auto &deck{d0_};

  for (int i = 52; i > 0; i--) {
    deck.add(i);
  }
  const auto prev_deck_order{d0_}; // a copy
  EXPECT_EQ(prev_deck_order, deck);
  deck.shuffle(gen_);
  EXPECT_NE(prev_deck_order, deck);
}