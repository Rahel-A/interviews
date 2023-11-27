#include <algorithm>
#include <deque>
#include <functional>
#include <iterator>
#include <optional>
#include <random>

template <typename C> class Deck {
  /* Container requirements */
public:
  typedef std::deque<C>::iterator iterator_t;
  typedef std::deque<C>::const_iterator const_iterator_t;

  /* Container requirements */
public:
  /*! \brief Creates an empty container */
  Deck() : m_cards(){};
  /*! \brief Creates a copy of `deck` */
  Deck(const Deck &deck) : m_cards(deck.m_cards){};
  /*! \brief Moves cards from `deck` */
  Deck(Deck &&deck) : m_cards(std::move(deck.m_cards)){};

  /*! \brief Copy cards from `rhs` */
  Deck &operator=(const Deck &rhs) {
    m_cards = rhs.m_cards;
    return *this;
  }
  /*! \brief Moves cards from `rhs` */
  Deck &operator=(Deck &&rhs) {
    m_cards = std::move(rhs.m_cards);
    return *this;
  }

  /*! \brief Destructor */
  ~Deck() { m_cards.clear(); }

  iterator_t begin() { return m_cards.begin(); }
  iterator_t end() { return m_cards.end(); }
  const_iterator_t cbegin() const { return m_cards.cbegin(); }
  const_iterator_t cend() const { return m_cards.cend(); }

  /*! \brief Equality in decks */
  bool operator==(const Deck &rhs) const {
    return std::equal(rhs.m_cards.begin(), rhs.m_cards.end(), m_cards.begin(),
                      m_cards.end());
  }
  bool operator!=(const Deck &rhs) const { return !(*this == rhs); }

  void swap(Deck &rhs) noexcept { m_cards.swap(rhs.m_cards); }
  static void swap(Deck &lhs, Deck &rhs) noexcept { lhs.swap(rhs); }

  std::size_t size() const noexcept { return m_cards.size(); }
  std::size_t max_size() const noexcept { return m_cards.max_size(); }
  bool empty() const noexcept { return m_cards.empty(); }

  /* Assignment */
public:
  /*! \brief Returns the value of the top card in the deck */
  std::optional<C> top() const noexcept {
    if (m_cards.size() == 0) {
      return std::nullopt;
    } else {
      return std::make_optional<C>(m_cards.front());
    }
  }

  /*! \brief Returns the value of the bottom card in the deck */
  std::optional<C> bottom() const noexcept {
    if (m_cards.size() == 0) {
      return std::nullopt;
    } else {
      return std::make_optional<C>(m_cards.back());
    }
  }

  /*! \brief Removes the top card of the deck, and retrieves it */
  std::optional<C> draw() noexcept {
    if (m_cards.size() == 0) {
      return std::nullopt;
    } else {
      const auto card = std::make_optional<C>(m_cards.front());
      m_cards.pop_front();
      return card;
    }
  }

  /*! \brief Adds a card on the top of the deck */
  void add(C card) noexcept { m_cards.push_front(card); }

  /*! \brief shuffle the order of the cards in the deck
   *  \param gen(optional) seed for the shuffle
   */
  template <typename Gen>
    requires std::uniform_random_bit_generator<std::remove_reference_t<Gen>>
  void shuffle(Gen gen = std::mt19937{std::random_device{}()}) {
    std::ranges::shuffle(m_cards, gen);
  }

private:
  std::deque<C> m_cards;
};
