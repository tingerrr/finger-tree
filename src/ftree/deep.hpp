#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/node.hpp"

#include <span>
#include <vector>

namespace ftree {
  template<typename K, typename V>
  class Deep {
    public:
      Deep() = delete;

      Deep(
        const std::vector<node::Node<K, V>>& left,
        const std::vector<node::Node<K, V>>& right
      );
      Deep(
        const std::vector<node::Node<K, V>>& left,
        const FingerTree<K, V>& middle,
        const std::vector<node::Node<K, V>>& right
      );

    public:
      auto left() const -> const std::span<const node::Node<K, V>> {
        return std::span(this->_left);
      }
      auto middle() const -> const FingerTree<K, V>& {
        return this->_middle;
      }
      auto right() const -> const std::span<const node::Node<K, V>> {
        return std::span(this->_right);
      }

    private:
      std::vector<node::Node<K, V>> _left;
      FingerTree<K, V> _middle;
      std::vector<node::Node<K, V>> _right;
  };

  template<typename K, typename V>
  Deep<K, V>::Deep(
    const std::vector<node::Node<K, V>>& left,
    const std::vector<node::Node<K, V>>& right
  ) : Deep(left, FingerTree<K, V>(), right) {
  }

  template<typename K, typename V>
  Deep<K, V>::Deep(
    const std::vector<node::Node<K, V>>& left,
    const FingerTree<K, V>& middle,
    const std::vector<node::Node<K, V>>& right
  ) : _left(left), _middle(middle), _right(right) {}
}
