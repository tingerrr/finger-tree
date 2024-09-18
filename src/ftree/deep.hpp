#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/digit.hpp"

namespace ftree {
  template<typename K, typename V>
  class Deep {
    public:
      Deep() = delete;

      Deep(
        digit::Digits<K, V>&& left,
        digit::Digits<K, V>&& right
      );
      Deep(
        digit::Digits<K, V>&& left,
        FingerTree<K, V>&& middle,
        digit::Digits<K, V>&& right
      );

    public:
      auto size() const -> uint {
        return this->_left.size() + this->_middle.size() + this->_right.size();
      }

      auto key() const -> const K& { return this->_right.key(); }

      auto left() const -> const digit::Digits<K, V>& { return this->_left; }
      auto middle() const -> const FingerTree<K, V>& { return this->_middle; }
      auto right() const -> const digit::Digits<K, V>& { return this->_right; }

    private:
      digit::Digits<K, V> _left;
      FingerTree<K, V> _middle;
      digit::Digits<K, V> _right;
  };

  template<typename K, typename V>
  Deep<K, V>::Deep(
    digit::Digits<K, V>&& left,
    digit::Digits<K, V>&& right
  ) : Deep(std::move(left), FingerTree<K, V>(), std::move(right)) {}

  template<typename K, typename V>
  Deep<K, V>::Deep(
    digit::Digits<K, V>&& left,
    FingerTree<K, V>&& middle,
    digit::Digits<K, V>&& right
  ) : _left(std::move(left)), _middle(middle), _right(std::move(right)) {}
}
