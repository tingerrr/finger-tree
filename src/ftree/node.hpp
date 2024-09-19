#pragma once

#include "src/ftree/node/core.hpp"
#include "src/ftree/node/deep.hpp"
#include "src/ftree/node/leaf.hpp"

#include <sys/types.h>

#include <iostream>
#include <optional>
#include <span>
#include <memory>
#include <variant>
#include <vector>

namespace ftree::node {
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
          Repr(Deep<K, V>&& repr);
          Repr(Leaf<K, V>&& repr);

        public:
          Variant _repr;
      };

    public:
      Node() = default;

    public:
      Node(Deep<K, V>&& repr);
      Node(Leaf<K, V>&& repr);

      Node(const K& key, const V& val);
      Node(const Node<K, V>& a, const Node<K, V>& b);
      Node(const Node<K, V>& a, const Node<K, V>& b, const Node<K, V>& c);

    public:
      auto size() const -> uint;

      auto key() const -> const K&;

      auto as_leaf() const -> const Leaf<K, V>*;
      auto as_deep() const -> const Deep<K, V>*;

      auto get(const K& key) const -> std::optional<Node<K, V>>;

    public:
      auto show(uint indent) const -> void;

    public:
      static auto pack_nodes(
        std::span<const Node<K, V>> nodes
      ) -> std::vector<Node<K, V>>;

    private:
      std::shared_ptr<Repr> _repr;
  };

  template<typename K, typename V>
  Node<K, V>::Repr::Repr(Deep<K, V>&& repr) : _repr(std::move(repr)) {}

  template<typename K, typename V>
  Node<K, V>::Repr::Repr(Leaf<K, V>&& repr) : _repr(std::move(repr)) {}

  template<typename K, typename V>
  Node<K, V>::Node(Deep<K, V>&& repr)
    : _repr(std::make_shared<Repr>(std::move(repr))) {}

  template<typename K, typename V>
  Node<K, V>::Node(Leaf<K, V>&& repr)
    : _repr(std::make_shared<Repr>(std::move(repr))) {}

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
  auto Node<K, V>::size() const -> uint {
    if (const auto* deep = this->as_deep()) {
      return deep->size();
    } else {
      return this->as_leaf()->size();
    }
  }

  template<typename K, typename V>
  auto Node<K, V>::key() const -> const K& {
    if (const auto* deep = this->as_deep()) {
      return deep->key();
    } else {
      return this->as_leaf()->key();
    }
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
  auto Node<K, V>::get(const K& key) const -> std::optional<Node<K, V>> {
    if (const auto* deep = this->as_deep()) {
    // NOTE: reverse search to find the right most matching node
      for (auto it = deep->children().rbegin(); it != deep->children().rend(); it++) {
        if (it->key() <= key) {
          return it->get(key);
        }
      }

      return std::optional<Node<K, V>>();
    } else {
      const auto* leaf = this->as_leaf();
      if (leaf->key() == key) {
        return std::optional<Node<K, V>>(*this);
      } else {
        return std::optional<Node<K, V>>();
      }
    }
  }

  template<typename K, typename V>
  auto Node<K, V>::show(uint indent) const -> void {
    auto istr = std::string(indent * 2, ' ');

    if (this->_repr == nullptr) {
      std::cout << istr << "null" << std::endl;
      return;
    }

    auto ref_count = this->_repr.use_count();

    if (const Leaf<K, V>* leaf = this->as_deep()) {
      std::cout
        << istr
        << ref_count
        << " <" << leaf->key() << ":" << leaf->val() << ">"
        << std::endl;
      return;
    }

    const Deep<K, V>* deep = this->as_deep();
    std::cout << istr << ref_count << " <"  << std::endl;
    for (const auto& child : deep->children()) {
      child.show(indent + 1);
    }
    std::cout << istr << ">" << std::endl;
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
}
