#pragma once

#include "src/ftree/node/core.hpp"

#include <span>
#include <sys/types.h>
#include <vector>

namespace ftree::node {
  template<typename K, typename V>
  class Deep {
    public:
      Deep() = delete;

      Deep(const Node<K, V>& a, const Node<K, V>& b);
      Deep(const Node<K, V>& a, const Node<K, V>& b, const Node<K, V>& c);

    public:
      auto size() const -> uint { return this->_size; }

      auto key() const -> const K& { return this->_key; }

      auto is_two() const -> bool { return this->size() == 2; }
      auto is_three() const -> bool { return this->size() == 3; }

      auto children() const -> std::span<const Node<K, V>> {
        return std::span(this->_children);
      }

    private:
      uint _size;
      K _key;
      std::vector<Node<K, V>> _children;
  };

  template<typename K, typename V>
  Deep<K, V>::Deep(
    const Node<K, V>& a,
    const Node<K, V>& b
  ) : _size(a.size() + b.size()), _key(b.key()), _children { a, b } {}

  template<typename K, typename V>
  Deep<K, V>::Deep(
    const Node<K, V>& a,
    const Node<K, V>& b,
    const Node<K, V>& c
  ) : _size(a.size() + b.size() + c.size()), _key(c.key()), _children { a, b, c } {}
}
