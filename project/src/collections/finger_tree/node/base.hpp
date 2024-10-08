#pragma once

// the base type of the node variants

#include "src/collections/finger_tree/node/core.hpp"

#include <iostream>
#include <sys/types.h>

namespace collections::finger_tree::node {
  template<typename K, typename V>
  class NodeBase {
    // constructors
    protected:
      NodeBase() = default;

    // helpers
    public:
      // print a debug representation of the node with the given indent
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& = 0;
  };

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, NodeBase<K, V> const& node) {
    return node.show(os, 0);
  }
}
