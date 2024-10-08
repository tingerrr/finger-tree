#pragma once

// the base type of the finger tree variants

#include "src/collections/finger_tree/core.hpp"

#include <ostream>
#include <sys/types.h>

namespace collections::finger_tree {
  template<typename K, typename V>
  class FingerTreeBase {
    // constructors
    protected:
      FingerTreeBase() = default;

    // helpers
    public:
      // print a debug representation of the tree with the given indent
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& = 0;
  };

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, FingerTreeBase<K, V> const& tree) {
    return tree.show(os, 0);
  }
}
