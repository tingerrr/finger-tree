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
        std::vector<node::Node<K, V>>&& left,
        std::vector<node::Node<K, V>>&& right
      );
      Deep(
        std::vector<node::Node<K, V>>&& left,
        const FingerTree<K, V>& middle,
        std::vector<node::Node<K, V>>&& right
      );

    public:
      auto size() const -> uint { return this->_size; }

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
      uint _size;
      std::vector<node::Node<K, V>> _left;
      FingerTree<K, V> _middle;
      std::vector<node::Node<K, V>> _right;
  };

  template<typename K, typename V>
  Deep<K, V>::Deep(
    std::vector<node::Node<K, V>>&& left,
    std::vector<node::Node<K, V>>&& right
  ) : Deep(std::move(left), FingerTree<K, V>(), std::move(right)) {}

  template<typename K, typename V>
  Deep<K, V>::Deep(
    std::vector<node::Node<K, V>>&& left,
    const FingerTree<K, V>& middle,
    std::vector<node::Node<K, V>>&& right
  ) : _left(std::move(left)), _middle(middle), _right(std::move(right)) {
    // NOTE: we alloc one more for the overflow to avoid reallocs there
    this->_left.reserve(5);
    this->_right.reserve(5);

    for (const node::Node<K, V>& node : this->_left) {
      this->_size += node.size();
    }
    this->_size += this->_middle.size();
    for (const node::Node<K, V>& node : this->_right) {
      this->_size += node.size();
    }
  }
}
