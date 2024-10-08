#pragma once

// the empty variant of a finger tree, included for completeness and easier
// comparisong to the haskell definition

#include "src/collections/finger_tree/core.hpp"

#include <ostream>
#include <sys/types.h>

namespace collections::finger_tree {
  template<typename K, typename V>
  class FingerTreeEmpty : public FingerTreeBase<K, V> {
    // constructors
    public:
      FingerTreeEmpty() = default;

    // helpers
    protected:
      // print a debug representation of the tree with the given indent
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& override;

    private:
      // for completeness lol
      friend class FingerTree<K, V>;
  };

  template<typename K, typename V>
  auto FingerTreeEmpty<K, V>::show(std::ostream& os, uint) const -> std::ostream& {
    return os << "Empty";
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, FingerTreeEmpty<K, V> const& tree) {
    return tree.show(os, 0);
  }
}
