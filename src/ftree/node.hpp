#pragma once

#include <iostream>
#include <optional>
#include <sys/types.h>

#include <span>
#include <memory>
#include <variant>
#include <vector>

namespace ftree::node {
  template<typename K, typename V>
  class Node;

  template<typename K, typename V>
  class Deep {
    public:
      Deep() = delete;

      Deep(const Node<K, V>& a, const Node<K, V>& b);
      Deep(const Node<K, V>& a, const Node<K, V>& b, const Node<K, V>& c);

    public:
      auto is_two() const -> bool { return this->_is_two; }
      auto is_three() const -> bool { return !this->_is_two; }
      auto size() const -> uint { return this->_is_two ? 2 : 3; }

      auto key() const -> const K& { return this->_key; }

      auto children() const -> std::span<const Node<K, V>> {
        return std::span(this->_children);
      }

    private:
      bool _is_two;
      K _key;
      std::vector<Node<K, V>> _children;
  };

  template<typename K, typename V>
  class Leaf {
    public:
      Leaf() = delete;

      Leaf(const K& key, const V& val);

    public:
      auto key() const -> const K& { return this->_key; }
      auto val() const -> const V& { return this->_val; }

    private:
      K _key;
      V _val;
  };

  template<typename K, typename V>
  class Node {
    private:
      class Repr {
        public:
          using Variant = std::variant<Deep<K, V>, Leaf<K, V>>;

        public:
          Repr() = delete;
          Repr(const Repr& other) = delete;
          Repr(Repr& other) = delete;
          Repr(Repr&& other) = delete;

        public:
          Repr(Deep<K, V> repr);
          Repr(Leaf<K, V> repr);

        public:
          Variant _repr;
      };

    public:
      Node() = delete;

    public:
      Node(Deep<K, V> repr);
      Node(Leaf<K, V> repr);

      Node(const K& key, const V& val);
      Node(const Node<K, V>& a, const Node<K, V>& b);
      Node(const Node<K, V>& a, const Node<K, V>& b, const Node<K, V>& c);

    public:
      auto key() const -> const K&;
      auto as_leaf() const -> const Leaf<K, V>*;
      auto as_deep() const -> const Deep<K, V>*;

      auto get(const K& key) -> std::optional<Node<K, V>>;

    public:
      auto show(uint indent) const -> void;

    public:
      static auto pack_nodes(
        std::span<const Node<K, V>> nodes
      ) -> std::vector<Node<K, V>>;

      static auto digit_get(
        std::span<const Node<K, V>> nodes,
        const K& key
      ) -> std::optional<Node<K, V>>;

    private:
      std::shared_ptr<Repr> _repr;
  };

  template<typename K, typename V>
  Deep<K, V>::Deep(
    const Node<K, V>& a,
    const Node<K, V>& b
  ) :  _is_two(true), _children { a, b } {}

  template<typename K, typename V>
  Deep<K, V>::Deep(
    const Node<K, V>& a,
    const Node<K, V>& b,
    const Node<K, V>& c
  ) :  _is_two(false), _children { a, b, c } {}

  template<typename K, typename V>
  Leaf<K, V>::Leaf(const K& key, const V& val) : _key(key), _val(val) {}

  template<typename K, typename V>
  Node<K, V>::Repr::Repr(Deep<K, V> repr) : _repr(repr) {}

  template<typename K, typename V>
  Node<K, V>::Repr::Repr(Leaf<K, V> repr) : _repr(repr) {}

  template<typename K, typename V>
  Node<K, V>::Node(Deep<K, V> repr)
    : _repr(std::make_shared<Repr>(repr)) {}

  template<typename K, typename V>
  Node<K, V>::Node(Leaf<K, V> repr)
    : _repr(std::make_shared<Repr>(repr)) {}

  template<typename K, typename V>
  Node<K, V>::Node(const K& key, const V& val) : Node(Leaf(key, val)) {}

  template<typename K, typename V>
  Node<K, V>::Node(
    const Node<K, V>& a,
    const Node<K, V>& b
  ) : Node(Deep(a, b)) {}

  template<typename K, typename V>
  Node<K, V>::Node(
    const Node<K, V>& a,
    const Node<K, V>& b,
    const Node<K, V>& c
  ) : Node(Deep(a, b, c)) {}

  template<typename K, typename V>
  auto Node<K, V>::key() const -> const K& {
    return std::visit([](auto& repr) { return repr.key(); }, *this->_repr);
  }

  template<typename K, typename V>
  auto Node<K, V>::as_leaf() const -> const Leaf<K, V>* {
    return std::get_if<Leaf<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto Node<K, V>::as_deep() const -> const Deep<K, V>* {
    return std::get_if<Deep<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto Node<K, V>::get(const K& key) -> std::optional<Node<K, V>> {
    return std::visit([this, key](auto& repr) {
      using T = std::decay_t<decltype(repr)>;

      if constexpr (std::is_same_v<T, Deep<K, V>>) {
        // NOTE: we only have 2 or 3 children, a linear search suffices

        for (const auto& child : repr.children()) {
          if (child.key() <= key) {
            return child.get(key);
          }
        }

        return std::optional<Node<K, V>>();
      } else if constexpr (std::is_same_v<T, Leaf<K, V>>) {
        if (repr.key() == key) {
          return std::optional<Node<K, V>>(*this);
        } else {
          return std::optional<Node<K, V>>();
        }
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, this->_repr->_repr);
  }

  template<typename K, typename V>
  auto Node<K, V>::show(uint indent) const -> void {
    auto istr = std::string(indent * 2, ' ');

    auto ref_count = this->_repr.use_count();

    std::visit([indent, ref_count, istr](auto& repr) {
      using T = std::decay_t<decltype(repr)>;

      if constexpr (std::is_same_v<T, Deep<K, V>>) {
        std::cout << istr << ref_count << " <"  << std::endl;
        for (const auto& child : repr.children()) {
          child.show(indent + 1);
        }
        std::cout << istr << ">" << std::endl;
      } else if constexpr (std::is_same_v<T, Leaf<K, V>>) {
        std::cout
          << istr
          << ref_count
          << " <" << repr.key() << ":" << repr.val() << ">"
          << std::endl;
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, this->_repr->_repr);
  }

  template<typename K, typename V>
  auto Node<K, V>::pack_nodes(
    std::span<const Node<K, V>> nodes
  ) -> std::vector<Node<K, V>> {
    std::vector<Node<K, V>> packed;

    while (nodes.size() != 0) {
      switch (nodes.size()) {
        case 2:
          packed.push_back(Node(Deep(nodes[0], nodes[1])));
          nodes = nodes.subspan(2);
          break;
        case 3:
          packed.push_back(Node(Deep(nodes[0], nodes[1], nodes[2])));
          nodes = nodes.subspan(3);
          break;
        case 4:
          packed.push_back(Node(Deep(nodes[0], nodes[1])));
          packed.push_back(Node(Deep(nodes[2], nodes[3])));
          nodes = nodes.subspan(4);
          break;
        default:
          packed.push_back(Node(Deep(nodes[0], nodes[1], nodes[2])));
          nodes = nodes.subspan(3);
          break;
      }
    }

    return packed;
  }

  template<typename K, typename V>
  auto Node<K, V>::digit_get(
    std::span<const Node<K, V>> nodes,
    const K& key
  ) -> std::optional<Node<K, V>> {
    // NOTE: we only have between 1 and 4 digits, a linear search suffices

    for (const auto& node : nodes) {
      if (node.key() <= key) {
        return node.get(key);
      }
    }

    return std::optional<Node<K, V>>();
  }
}
