#pragma once

// the only digit variant, this is only used for the separation for persistence
// digits allow 0 and more than 4 elements to avoid excessive copying in
// over/underflow scenarios

#include "src/collections/finger_tree/core.hpp"
#include "src/collections/finger_tree/digit/_prelude.hpp"

#include <iostream>
#include <span>
#include <sys/types.h>
#include <vector>

namespace collections::finger_tree::digit {
  template<typename K, typename V>
  class DigitsBase {
    // constructors
    public:
      DigitsBase() = default;

      // create various digits depending on the number of nodes inside them
      DigitsBase(Node<K, V> const& a);
      DigitsBase(Node<K, V> const& a, Node<K, V> const& b);
      DigitsBase(Node<K, V> const& a, Node<K, V> const& b, Node<K, V> const& c);
      DigitsBase(
        Node<K, V> const& a,
        Node<K, V> const& b,
        Node<K, V> const& c,
        Node<K, V> const& d
      );

    // accessors
    public:
      auto size() const -> uint { return this->_size; }
      auto digit_size() const -> uint { return this->_digits.size(); }
      auto key() const -> K const& { return this->_digits.back().key(); }
      auto digits() const -> std::span<Node<K, V> const> {
        return std::span(this->_digits);
      }
      auto left() -> Node<K, V> const& { return this->_digits.front(); }
      auto right() -> Node<K, V> const& { return this->_digits.back(); }

    // methods
    public:
      // return a pointer to the value this key refers to, or a nullptr if the
      // key didn't exist
      auto get(K const& key) const -> V const*;

      // add a node at the given side
      auto push(Direction dir, Node<K, V> const& node) -> void;

      // pop a node from the given side
      // undefined behavior if called on empty digits
      auto pop(Direction dir) -> void;

      // unpack a deep node and add its children
      // this is used for underflow
      auto unpack(Direction dir, NodeDeep<K, V> const& node) -> void;

      // pack nodes from the given side and return them
      // this is used for overflow
      // undefined behavior if called on less than 3 digits
      auto pack(Direction dir) -> NodeDeep<K, V>;

    // helpers
    public:
      // ensure we're within valid bounds
      auto assert_invariant() const -> void;

      // print a debug representation of the tree with the given indent
      auto show(std::ostream& os, uint indent) const -> std::ostream&;

    private:
      // we cache the size of this directly, the key can be accessed using the
      // last node
      uint _size;
      std::vector<Node<K, V>> _digits;
  };

  template<typename K, typename V>
  DigitsBase<K, V>::DigitsBase(
    Node<K, V> const& a
  ) : _size(0), _digits() {
    this->_size += a.size();
    this->_digits.reserve(5);
    this->_digits.emplace_back(a);
  }

  template<typename K, typename V>
  DigitsBase<K, V>::DigitsBase(
    Node<K, V> const& a,
    Node<K, V> const& b
  ) : _size(0), _digits() {
    this->_size += a.size();
    this->_size += b.size();
    this->_digits.reserve(5);
    this->_digits.emplace_back(a);
    this->_digits.emplace_back(b);
  }

  template<typename K, typename V>
  DigitsBase<K, V>::DigitsBase(
    Node<K, V> const& a,
    Node<K, V> const& b,
    Node<K, V> const& c
  ) : _size(0), _digits() {
    this->_size += a.size();
    this->_size += b.size();
    this->_size += c.size();
    this->_digits.reserve(5);
    this->_digits.emplace_back(a);
    this->_digits.emplace_back(b);
    this->_digits.emplace_back(c);
  }

  template<typename K, typename V>
  DigitsBase<K, V>::DigitsBase(
    Node<K, V> const& a,
    Node<K, V> const& b,
    Node<K, V> const& c,
    Node<K, V> const& d
  ) : _size(0), _digits() {
    this->_size += a.size();
    this->_size += b.size();
    this->_size += c.size();
    this->_size += d.size();
    this->_digits.reserve(5);
    this->_digits.emplace_back(a);
    this->_digits.emplace_back(b);
    this->_digits.emplace_back(c);
    this->_digits.emplace_back(d);
  }

  template<typename K, typename V>
  auto DigitsBase<K, V>::get(K const& key) const -> V const* {
    for (const auto& digit : this->_digits) {
      if (digit.key() >= key) {
        return digit.get(key);
      }
    }

    return nullptr;
  }

  template<typename K, typename V>
  auto DigitsBase<K, V>::push(Direction dir, Node<K, V> const& node) -> void {
    if (dir == Direction::Left) {
      this->_digits.emplace(this->_digits.cbegin(), node);
    } else {
      this->_digits.emplace_back(node);
    }
  }

  template<typename K, typename V>
  auto DigitsBase<K, V>::pop(Direction dir) -> void {
    if (dir == Direction::Left) {
      this->_digits.erase(this->_digits.cbegin());
    } else {
      this->_digits.pop_back();
    }
  }

  template<typename K, typename V>
  auto DigitsBase<K, V>::unpack(
    Direction dir,
    NodeDeep<K, V> const& node
  ) -> void {
    if (dir == Direction::Left) {
      for (auto it = node.children().rbegin(); it != node.children().rend(); it++) {
        this->push(Direction::Left, *it);
      }
    } else {
      for (const auto& child : node.children()) {
        this->push(Direction::Right, child);
      }
    }
  }

  template<typename K, typename V>
  auto DigitsBase<K, V>::pack(Direction dir) -> NodeDeep<K, V> {
    if (dir == Direction::Left) {
      auto a = this->left();
      this->pop(Direction::Left);
      auto b = this->left();
      this->pop(Direction::Left);
      auto c = this->left();
      this->pop(Direction::Left);
      return NodeDeep<K, V>(a, b, c);
    } else {
      auto c = this->right();
      this->pop(Direction::Left);
      auto b = this->right();
      this->pop(Direction::Left);
      auto a = this->right();
      this->pop(Direction::Left);
      return NodeDeep<K, V>(a, b, c);
    }
  }

  template<typename K, typename V>
  auto DigitsBase<K, V>::show(std::ostream& os, uint indent) const -> std::ostream& {
    auto istr = std::string(indent * 2, ' ');
    auto istr2 = std::string((indent + 1) * 2, ' ');

    os << "[" << std::endl;
    for (const auto& digit : this->_digits) {
      os << istr2;
      digit.show(os, indent + 1);
      os << std::endl;
    }
    os << istr << "]";

    return os;
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, DigitsBase<K, V> const& node) {
    return node.show(os, 0);
  }
}
