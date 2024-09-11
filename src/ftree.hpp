#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/empty.hpp"
#include "src/ftree/deep.hpp"
#include "src/ftree/node.hpp"
#include "src/ftree/single.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace ftree {
  enum Direction { Left, Right };

  template<typename K, typename V>
  class FingerTree {
    private:
      class Repr {
        public:
          using Variant = std::variant<Empty<K, V>, Single<K, V>, Deep<K, V>>;

        public:
          Repr() = delete;
          Repr(const Repr& other) = delete;
          Repr(Repr& other) = delete;
          Repr(Repr&& other) = delete;

        public:
          Repr(const Empty<K, V>& repr);
          Repr(const Single<K, V>& repr);
          Repr(const Deep<K, V>& repr);

        public:
          Variant _repr;
      };

    public:
      FingerTree();

    private:
      FingerTree(const Empty<K, V>& repr);
      FingerTree(const Single<K, V>& repr);
      FingerTree(const Deep<K, V>& repr);

    public:
      auto key() const -> const K&;

    public:
      auto push(Direction dir, K key, V val) -> void;
      auto pop(Direction dir) -> std::optional<std::pair<K, V>>;

    private:
      auto push_impl(Direction dir, node::Node<K, V> node) -> void;
      auto pop_impl(Direction dir) -> std::optional<node::Node<K, V>>;

      private:
        std::shared_ptr<Repr> _repr;
  };

  template<typename K, typename V>
  FingerTree<K, V>::Repr::Repr(const Empty<K, V>& repr) : _repr(Repr::Variant(repr)) {}

  template<typename K, typename V>
  FingerTree<K, V>::Repr::Repr(const Deep<K, V>& repr) : _repr(Repr::Variant(repr)) {}

  template<typename K, typename V>
  FingerTree<K, V>::Repr::Repr(const Single<K, V>& repr) : _repr(Repr::Variant(repr)) {}
  
  template<typename K, typename V>
  FingerTree<K, V>::FingerTree() : FingerTree(Empty<K, V>()) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(const Empty<K, V>& repr)
    : _repr(std::make_shared<Repr>(repr)) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(const Single<K, V>& repr)
    : _repr(std::make_shared<Repr>(repr)) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(const Deep<K, V>& repr)
    : _repr(std::make_shared<Repr>(repr)) {}

  template<typename K, typename V>
  auto FingerTree<K, V>::push_impl(Direction dir, node::Node<K, V> node) -> void {
    std::visit([this, dir, node](auto& repr) {
      using T = std::decay_t<decltype(repr)>;

      if constexpr (std::is_same_v<T, Empty<K, V>>) {
        this->_repr = std::make_shared<Repr>(Single(node));
      } else if constexpr (std::is_same_v<T, Single<K, V>>) {
        node::Node<K, V> single = repr.node();

        switch (dir) {
          case Left:
            this->_repr = std::make_shared<Repr>(Deep<K, V>({ node }, { single }));
            break;
          case Right:
            this->_repr = std::make_shared<Repr>(Deep<K, V>({ single }, { node }));
            break;
        }
      } else if constexpr (std::is_same_v<T, Deep<K, V>>) {
        std::vector<node::Node<K, V>> left(repr.left().begin(), repr.left().end());
        std::vector<node::Node<K, V>> right(repr.right().begin(), repr.right().end());
        FingerTree<K, V> middle = repr.middle();

        switch (dir) {
          case Left:
            left.insert(left.begin(), node);

            if (left.size() > 4) {
              middle.push_impl(
                dir,
                node::Node<K, V>(left[2], left[3], left[4])
              );
              left = { left[0], left[1] };
            }
            break;
          case Right:
            right.push_back(node);

            if (right.size() > 4) {
              middle.push_impl(
                dir,
                node::Node<K, V>(right[0], right[1], right[2])
              );
              right = { right[3], right[4] };
            }
            break;
        }

        this->_repr = std::make_shared<Repr>(Deep<K, V>(left, middle, right));
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::pop_impl(Direction dir) -> std::optional<node::Node<K, V>> {
    return std::visit([this, dir](auto& repr) {
      using T = std::decay_t<decltype(repr)>;

      if constexpr (std::is_same_v<T, Empty<K, V>>) {
        this->_repr = std::make_shared<Repr>(Empty<K, V>());
        return std::optional<node::Node<K, V>>();
      } else if constexpr (std::is_same_v<T, Single<K, V>>) {
        this->_repr = std::make_shared<Repr>(FingerTree(Empty<K, V>()));
        return std::optional<node::Node<K, V>>(repr.node());
      } else if constexpr (std::is_same_v<T, Deep<K, V>>) {
        switch (dir) {
          case Left:
            break;
          case Right:
            break;
        }
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push(Direction dir, K key, V val) -> void {
    this->push_impl(dir, node::Node<K, V>(key, val));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::pop(Direction dir) -> std::optional<std::pair<K, V>> {
    std::optional<node::Node<K, V>> node = this->pop_impl(dir);
    std::optional<std::pair<K, V>> unpacked;

    if (*node) {
      auto leaf = node.as_leaf();
      unpacked = std::optional(std::make_pair(leaf->key(), leaf->val()));
    }

    return unpacked;
  }
}
