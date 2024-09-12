#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/node.hpp"

namespace ftree {
  template<typename K, typename V>
  class Single {
    public:
      Single() = delete;

      Single(const node::Node<K, V>& node);

    public:
      auto node() const -> const node::Node<K, V>& { return this->_node; }

    private:
      node::Node<K, V> _node;
  };

  template<typename K, typename V>
  Single<K, V>::Single(const node::Node<K, V>& node) : _node(node) {}
}
