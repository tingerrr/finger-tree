#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/empty.hpp"
#include "src/ftree/deep.hpp"
#include "src/ftree/node.hpp"
#include "src/ftree/single.hpp"

#include <iostream>
#include <memory>
#include <optional>
#include <string>
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

      auto get(const K& key) -> std::optional<V>;
      auto push(Direction dir, K key, V val) -> void;
      auto pop(Direction dir) -> std::optional<std::pair<K, V>>;

      auto as_empty() const -> const Empty<K, V>*;
      auto as_single() const -> const Single<K, V>*;
      auto as_deep() const -> const Deep<K, V>*;

    public:
      auto show(uint indent) const -> void;

    private:
      auto get_impl(const K& key) -> std::optional<node::Node<K, V>>;
      auto push_impl(Direction dir, const node::Node<K, V>& node) -> void;
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
  auto FingerTree<K, V>::get_impl(const K& key) -> std::optional<node::Node<K, V>> {
    return std::visit([key](auto& repr) {
      using T = std::decay_t<decltype(repr)>;

      if constexpr (std::is_same_v<T, Empty<K, V>>) {
        return std::optional<node::Node<K, V>>();
      } else if constexpr (std::is_same_v<T, Single<K, V>>) {
        if (repr.key() <= key) {
          return repr.node().get(key);
        } else {
          return std::optional<node::Node<K, V>>();
        }
      } else if constexpr (std::is_same_v<T, Deep<K, V>>) {
        if (repr.left().back().key() <= key) {
          return node::Node<K, V>::digit_get(repr.left());
        } else if (repr.middle().key() <= key) {
          return repr.middle().get_impl(key);
        } else if (repr.right().back().key() <= key) {
          return node::Node<K, V>::digit_get(repr.right());
        } else {
          return std::optional<node::Node<K, V>>();
        }
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push_impl(
    Direction dir,
    const node::Node<K, V>& node
  ) -> void {
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
  auto FingerTree<K, V>::get(const K& key) -> std::optional<V> {
    std::optional<node::Node<K, V>> node = this->get_impl(key);
    std::optional<std::pair<K, V>> unpacked;

    if (*node) {
      auto leaf = node.as_leaf();
      unpacked = std::optional(leaf->val());
    }

    return unpacked;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push(Direction dir, K key, V val) -> void {
    // TODO: validate key invariants
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

  template<typename K, typename V>
  auto FingerTree<K, V>::as_empty() const -> const Empty<K, V>* {
    return std::get_if<Empty<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::as_single() const -> const Single<K, V>* {
    return std::get_if<Single<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::as_deep() const -> const Deep<K, V>* {
    return std::get_if<Deep<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::show(uint indent) const -> void {
    auto istr = std::string(indent * 2, ' ');
    auto istr2 = std::string((indent + 1) * 2, ' ');

    std::visit([indent, &istr, &istr2](auto& repr) {
      using T = std::decay_t<decltype(repr)>;

      if constexpr (std::is_same_v<T, Empty<K, V>>) {
        std::cout << istr << "Empty" << std::endl;
      } else if constexpr (std::is_same_v<T, Single<K, V>>) {
        std::cout << istr << "Single" << std::endl;
        repr.node().show(indent + 1);
      } else if constexpr (std::is_same_v<T, Deep<K, V>>) {
        std::cout << istr << "Deep" << std::endl;
        std::cout << istr2 << "[" << std::endl;
        for (const auto& node : repr.left()) {
          node.show(indent + 2);
        }
        std::cout << istr2 << "]" << std::endl;

        repr.middle().show(indent + 1);

        std::cout << istr2 << "[" << std::endl;
        for (const auto& node : repr.right()) {
          node.show(indent + 2);
        }
        std::cout << istr2 << "]" << std::endl;
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, this->_repr->_repr);
  }
}
