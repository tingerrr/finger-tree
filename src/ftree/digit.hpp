#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/node/core.hpp"
#include "src/ftree/node/deep.hpp"

#include <sys/types.h>

#include <algorithm>
#include <array>
#include <optional>
#include <span>

namespace ftree::digit {
  template<typename K, typename V>
  class Digits {
    public:
      Digits() = default;

    public:
      Digits(const node::Node<K, V>& a);
      Digits(const node::Node<K, V>& a, const node::Node<K, V>& b);
      Digits(
        const node::Node<K, V>& a,
        const node::Node<K, V>& b,
        const node::Node<K, V>& c
      );
      Digits(
        const node::Node<K, V>& a,
        const node::Node<K, V>& b,
        const node::Node<K, V>& c,
        const node::Node<K, V>& d
      );

    // public constructors helpers
    public:
      static auto from(std::span<const node::Node<K, V>> nodes) -> Digits<K, V>;

    // accessors
    public:
      auto is_max_digit_size() const -> bool { return this->_digit_size == 4; }
      auto is_min_digit_size() const -> bool { return this->_digit_size == 1; }

      auto digits() const -> std::span<const node::Node<K, V>> {
        return std::span(this->_digits.data(), this->_digit_size);
      }
      auto key() const -> const K& { return this->digits().back().key(); }
      auto size() const -> uint { return this->_size; }
      auto digit_size() const -> uint { return this->_digit_size; }

    // public methods
    public:
      auto get(const K& key) const -> std::optional<node::Node<K, V>>;

      auto shift(Direction dir, uint n);

      auto push(
        Direction dir,
        const node::Node<K, V>& node
      );

      auto take_packed(Direction dir) -> node::Node<K, V>;

      auto pop(Direction dir) -> node::Node<K, V>;

      auto put_packed(
        Direction dir,
        const node::Deep<K, V>& node
      );

      auto split(const K& key) const -> std::tuple<
        digit::Digits<K, V>,
        std::optional<node::Node<K, V>>,
        digit::Digits<K, V>
      >;

    private:
      std::array<node::Node<K, V>, 4> _digits;
      uint _size;
      char _digit_size;
  };

  template<typename K, typename V>
  Digits<K, V>::Digits(
    const node::Node<K, V>& a
  ) : _digits { a }
    , _size(a.size())
    , _digit_size(1)
  {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    const node::Node<K, V>& a,
    const node::Node<K, V>& b
  ) : _digits { a, b }
    , _size(a.size() + b.size())
    , _digit_size(2)
  {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    const node::Node<K, V>& a,
    const node::Node<K, V>& b,
    const node::Node<K, V>& c
  ) : _digits { a, b, c }
    , _size(a.size() + b.size() + c.size())
    , _digit_size(3)
  {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    const node::Node<K, V>& a,
    const node::Node<K, V>& b,
    const node::Node<K, V>& c,
    const node::Node<K, V>& d
  ) : _digits { a, b, c, d }
    , _size(a.size() + b.size() + c.size() + d.size())
    , _digit_size(4)
  {}

  template<typename K, typename V>
  auto Digits<K, V>::from(std::span<const node::Node<K, V>> nodes) -> Digits<K, V> {
    switch (nodes.size()) {
      case 1:
        return Digits(nodes[0]);
      case 2:
        return Digits(nodes[0], nodes[1]);
      case 3:
        return Digits(nodes[0], nodes[1], nodes[2]);
      case 4:
        return Digits(nodes[0], nodes[1], nodes[2], nodes[3]);
      default:
        return Digits();
    }
  }

  template<typename K, typename V>
  auto Digits<K, V>::get(const K& key) const -> std::optional<node::Node<K, V>> {
    // NOTE: reverse search to find the right most matching node
    for (auto it = this->digits().rbegin(); it != this->digits().rend(); it++) {
      if (it->key() <= key) {
        return it->get(key);
      }
    }

    return std::optional<node::Node<K, V>>();
  }

  template<typename K, typename V>
  auto Digits<K, V>::shift(Direction dir, uint n) {
    n = n % 4;

    if (n == 0) {
      return;
    }

    if (dir == ftree::Left) {
      std::move(
        this->_digits.begin() + n,
        this->_digits.end(),
        this->_digits.begin()
      );
      std::fill(this->_digits.end() - n, this->_digits.end(), node::Node<K, V>());
    } else {
      std::move_backward(
        this->_digits.begin(),
        this->_digits.end() - n,
        this->_digits.end()
      );
      std::fill(this->_digits.begin(), this->_digits.begin() + n, node::Node<K, V>());
    }
  }

  // UB if called on four digits
  template<typename K, typename V>
  auto Digits<K, V>::push(
    Direction dir,
    const node::Node<K, V>& node
  ) {
    switch (dir) {
      case ftree::Left:
        this->shift(ftree::Right, 1);
        this->_digits[0] = node;
        this->_digit_size += 1;
        break;
      case ftree::Right:
        this->_digits[this->_digit_size] = node;
        this->_digit_size += 1;
        break;
    }
  }

  // UB if called on less than three digits
  template<typename K, typename V>
  auto Digits<K, V>::take_packed(Direction dir) -> node::Node<K, V> {
    std::span<const node::Node<K, V>> end = dir == ftree::Left
      ? this->digits().first(3)
      : this->digits().last(3);

    node::Node<K, V> node = node::Node(end[0], end[1], end[2]);
    if (dir == ftree::Left) {
      this->shift(ftree::Left, 3);
    }
    this->_digit_size -= 3;

    return node;
  }

  // UB if called on zero digits
  template<typename K, typename V>
  auto Digits<K, V>::pop(Direction dir) -> node::Node<K, V> {
    if (dir == ftree::Left) {
      node::Node<K, V> node = this->digits().front();
      this->shift(ftree::Left, 1);
      this->_digit_size -= 1;
      return std::move(node);
    } else {
      node::Node<K, V> node = this->digits().back();
      this->_digit_size -= 1;
      return std::move(node);
    }
  }

  // UB if called on oversized digits
  template<typename K, typename V>
  auto Digits<K, V>::put_packed(
    Direction dir,
    const node::Deep<K, V>& node
  ) {
    switch (dir) {
      case ftree::Left:
        this->shift(ftree::Right, node.children().size());
        this->_digits[0] = node.children()[0];
        this->_digits[1] = node.children()[1];
        if (node.is_three()) {
          this->_digits[2] = node.children()[2];
        }
        break;
      case ftree::Right:
        this->_digits[this->_digit_size] = node.children()[0];
        this->_digits[this->_digit_size + 1] = node.children()[1];
        if (node.is_three()) {
          this->_digits[this->_digit_size + 2] = node.children()[2];
        }
        break;
    }

    this->_digit_size += node.children().size();
  }

  template<typename K, typename V>
  auto Digits<K, V>::split(const K& key) const -> std::tuple<
    digit::Digits<K, V>,
    std::optional<node::Node<K, V>>,
    digit::Digits<K, V>
  > {
    std::span<const node::Node<K, V>> nodes = this->digits();

    for (uint i = 0; i < nodes.size(); i++) {
      if (this->digits()[i].key() >= key) {
        return std::make_tuple(
          Digits<K, V>::from(nodes.subspan(0, i)),
          std::optional(nodes[i]),
          Digits<K, V>::from(nodes.subspan(i + 1))
        );
      }
    }

    return std::make_tuple(*this, std::optional<node::Node<K, V>>(), Digits<K, V>());
  }
}
