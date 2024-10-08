#pragma once

// the deep variant of the finger tree
// as shown in the thesis, this is not a non-regular type as these cannot be
// defined in C++

#include "src/collections/finger_tree/_prelude.hpp"
#include "src/collections/finger_tree/core.hpp"

#include <ostream>
#include <sys/types.h>

namespace collections::finger_tree {
  template<typename K, typename V>
  class FingerTreeDeep : public FingerTreeBase<K, V> {
    // constructors
    public:
      FingerTreeDeep(
        Digits<K, V> const& left,
        FingerTree<K, V> const& middle,
        Digits<K, V> const& right
      );

    // accessors
    public:
      auto key() const -> const K& { return this->_right.key(); }

      // we include mutable accessors for the ensure_unique optimization, they
      // are however not used atm
      auto left() -> Digits<K, V>& { return this->_left; }
      auto left() const -> Digits<K, V> const& { return this->_left; }

      auto middle() -> FingerTree<K, V>& { return this->_middle; }
      auto middle() const -> FingerTree<K, V> const& { return this->_middle; }

      auto right() -> Digits<K, V>& { return this->_right; }
      auto right() const -> Digits<K, V> const& { return this->_right; }

    // helpers
    protected:
      // print a debug representation of the tree with the given indent
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& override;

    private:
      // we cache the size of this directly, the key can be accessed using the
      // _right digits field
      uint _size;

      Digits<K, V> _left;
      FingerTree<K, V> _middle;
      Digits<K, V> _right;

      // give the wrapper type access to this variant's internals
      friend class FingerTree<K, V>;
  };

  template<typename K, typename V>
  FingerTreeDeep<K, V>::FingerTreeDeep(
    Digits<K, V> const& left,
    FingerTree<K, V> const& middle,
    Digits<K, V> const& right
  ) : _left(left), _middle(middle), _right(right) {
    // the size of this tree must be the sum of its parts sizes
    this->_size += this->_left.size();
    this->_size += this->_middle.size();
    this->_size += this->_right.size();
  }

  template<typename K, typename V>
  auto FingerTreeDeep<K, V>::show(std::ostream& os, uint indent) const -> std::ostream& {
    auto istr2 = std::string((indent + 1) * 2, ' ');

    os << "Deep" << std::endl;
    os << istr2 << "Left ";
    this->_left.show(os, indent + 1);
    os << std::endl;

    os << istr2;
    this->_middle.show(os, indent + 1);
    os << std::endl;

    os << istr2 << "Right ";
    this->_right.show(os, indent + 1);
    return os;
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, FingerTreeDeep<K, V> const& tree) {
    return tree.show(os, 0);
  }
}
