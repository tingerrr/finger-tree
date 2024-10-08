#pragma once

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
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& = 0;
  };

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, FingerTreeBase<K, V> const& tree) {
    return tree.show(os, 0);
  }
}
