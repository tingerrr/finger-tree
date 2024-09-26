#pragma once

#include "src/collections/finger_tree/node/core.hpp"

#include <ostream>
#include <sys/types.h>

namespace collections::finger_tree::node {
  template<typename K, typename V>
  class NodeLeaf : public NodeBase<K, V> {
    // constructors
    public:
      NodeLeaf() = delete;

      NodeLeaf(K const& key, V const& val);

    // accessors
    public:
      auto key() const -> K const& { return this->_key; };
      auto val() const -> V const& { return this->_val; }

    // helpers
    protected:
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& override;

    private:
      K _key;
      V _val;

      friend class Node<K, V>;
  };

  template<typename K, typename V>
  NodeLeaf<K, V>::NodeLeaf(K const& key, V const& val) : _key(key), _val(val) {}

  template<typename K, typename V>
  auto NodeLeaf<K, V>::show(std::ostream& os, uint) const -> std::ostream& {
    return os << "<" << this->_key << ":" << this->_val << ">";
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, NodeLeaf<K, V> const& node) {
    return node.show(os, 0);
  }
}
