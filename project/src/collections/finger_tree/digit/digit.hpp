#pragma once

#include "src/utils/uninit_exception.hpp"

#include "src/collections/finger_tree/digit/base.hpp"
#include "src/collections/finger_tree/digit/core.hpp"
#include "src/collections/finger_tree/digit/_prelude.hpp"

#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <sys/types.h>

namespace collections::finger_tree::digit {
  template<typename K, typename V>
  class Digits {
    // constructors
    public:
      Digits();

      // create various digits depending on the number of nodes inside them
      Digits(Node<K, V> const& a);
      Digits(Node<K, V> const& a, Node<K, V> const& b);
      Digits(Node<K, V> const& a, Node<K, V> const& b, Node<K, V> const& c);
      Digits(
        Node<K, V> const& a,
        Node<K, V> const& b,
        Node<K, V> const& c,
        Node<K, V> const& d
      );

    public:
      // creates digits from the given nodes, throwing an exception if there
      // aren't enough to too many
      static auto from_nodes(std::span<Node<K, V> const> nodes) -> Digits<K, V>;

    // accessors
    public:
      auto size() const -> uint;
      auto digit_size() const -> uint;
      auto key() const -> K const&;
      auto digits() const -> std::span<Node<K, V> const>;
      auto left() const -> Node<K, V> const&;
      auto right() const -> Node<K, V> const&;

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

      // split the digit similar to a finger tree, but only do a shallow split
      //
      // because this may return empty spans and is used to create new trees,
      // the deep_smart construtor helper is needed for finger trees
      auto split(K const& key) const -> std::tuple<
        std::span<Node<K, V> const>,
        std::optional<Node<K, V>>,
        std::span<Node<K, V> const>
      >;

    // helpers
    public:
      auto is_uninit() const -> bool { return this->_repr == nullptr; }

      // ensure we're initalized (not nullptr)
      auto assert_init() const -> void;

      // ensure we're operating on a _repr instance which has no other
      // referents
      //
      // the invisible persistnce section explains why this is tricky, hence why
      // this currently always defensively copies the inner instance (not the
      // shared ptr, but the inner variant object itself)
      auto ensure_unique() -> void;

      // print a debug representation of the digits with the given indent
      auto show(std::ostream& os, uint indent) const -> std::ostream&;

    private:
      std::shared_ptr<DigitsBase<K, V>> _repr;
  };

  template<typename K, typename V>
  Digits<K, V>::Digits() : _repr(std::make_shared<DigitsBase<K, V>>()) {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    Node<K, V> const& a
  ) : _repr(std::make_shared<DigitsBase<K, V>>(a)) {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    Node<K, V> const& a,
    Node<K, V> const& b
  ) : _repr(std::make_shared<DigitsBase<K, V>>(a, b)) {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    Node<K, V> const& a,
    Node<K, V> const& b,
    Node<K, V> const& c
  ) : _repr(std::make_shared<DigitsBase<K, V>>(a, b, c)) {}

  template<typename K, typename V>
  Digits<K, V>::Digits(
    Node<K, V> const& a,
    Node<K, V> const& b,
    Node<K, V> const& c,
    Node<K, V> const& d
  ) : _repr(std::make_shared<DigitsBase<K, V>>(a, b, c, d)) {}

  template<typename K, typename V>
  auto Digits<K, V>::from_nodes(std::span<Node<K, V> const> nodes) -> Digits<K, V> {
    switch (nodes.size()) {
      case 0:
        // NOTE: this is technically not allowed but only serves as an
        // intermediate state
        return Digits();
      case 1:
        return Digits(nodes[0]);
      case 2:
        return Digits(nodes[0], nodes[1]);
      case 3:
        return Digits(nodes[0], nodes[1], nodes[2]);
      case 4:
        return Digits(nodes[0], nodes[1], nodes[2], nodes[3]);
      default:
        throw std::out_of_range("no more than 4 nodes are permitted for digits");
    }
  }

  template<typename K, typename V>
  auto Digits<K, V>::size() const -> uint {
    this->assert_init();
    return this->_repr->size();
  }

  template<typename K, typename V>
  auto Digits<K, V>::digit_size() const -> uint {
    this->assert_init();
    return this->_repr->digit_size();
  }

  template<typename K, typename V>
  auto Digits<K, V>::key() const -> K const& {
    this->assert_init();
    return this->_repr->key();
  }

  template<typename K, typename V>
  auto Digits<K, V>::get(K const& key) const -> V const* {
    this->assert_init();
    return this->_repr->get(key);
  }

  template<typename K, typename V>
  auto Digits<K, V>::digits() const -> std::span<Node<K, V> const> {
    this->assert_init();
    return this->_repr->digits();
  }

  template<typename K, typename V>
  auto Digits<K, V>::left() const -> Node<K, V> const& {
    this->assert_init();
    return this->_repr->left();
  }

  template<typename K, typename V>
  auto Digits<K, V>::right() const -> Node<K, V> const& {
    this->assert_init();
    return this->_repr->right();
  }

  template<typename K, typename V>
  auto Digits<K, V>::push(Direction dir, Node<K, V> const& node) -> void {
    this->ensure_unique();
    return this->_repr->push(dir, node);
  }

  template<typename K, typename V>
  auto Digits<K, V>::pop(Direction dir) -> void {
    this->ensure_unique();
    this->_repr->pop(dir);
  }

  template<typename K, typename V>
  auto Digits<K, V>::unpack(Direction dir, NodeDeep<K, V> const& node) -> void {
    this->ensure_unique();
    return this->_repr->unpack(dir, node);
  }

  template<typename K, typename V>
  auto Digits<K, V>::pack(Direction dir) -> NodeDeep<K, V> {
    this->ensure_unique();
    return this->_repr->pack(dir);
  }

  template<typename K, typename V>
  auto Digits<K, V>::split(K const& key) const -> std::tuple<
    std::span<Node<K, V> const>,
    std::optional<Node<K, V>>,
    std::span<Node<K, V> const>
  > {
    std::span<Node<K, V> const> nodes = this->digits();

    for (uint i = 0; i < nodes.size(); i++) {
      if (nodes[i].key() >= key) {
        return std::tuple(
          nodes.subspan(0, i),
          std::optional(nodes[i]),
          nodes.subspan(i + 1)
        );
      }
    }

    return std::tuple(
      nodes,
      std::optional<Node<K, V>>(),
      std::span<Node<K, V> const>()
    );
  }
 
  template<typename K, typename V>
  auto Digits<K, V>::assert_init() const -> void {
    if (this->is_uninit()) {
      throw UninitException("Digits are uninitialized");
    }
  }

  template<typename K, typename V>
  auto Digits<K, V>::ensure_unique() -> void {
    this->assert_init();
    // NOTE: no pointers or references to a Digits my be sent to another thread,
    // only values of Digits, therefor no copy may be done between this check
    // and subsequent writes
    // if (this->_repr.use_count() == 1) {
    //   return;
    // }

    this->_repr = std::make_shared<DigitsBase<K, V>>(*this->_repr);
  }

  template<typename K, typename V>
  auto Digits<K, V>::show(std::ostream& os, uint indent) const -> std::ostream& {
    if (this->_repr == nullptr) {
      return os << "null";
    }

    return this->_repr->show(os, indent);
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, Node<K, V> const& node) {
    return node.show(os, 0);
  }
}
