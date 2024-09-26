#pragma once

#include "src/utils/uninit_exception.hpp"
#include "src/utils/variant_exception.hpp"

#include "src/collections/finger_tree/node/core.hpp"
#include "src/collections/finger_tree/node/base.hpp"
#include "src/collections/finger_tree/node/deep.hpp"
#include "src/collections/finger_tree/node/leaf.hpp"

#include <ostream>
#include <memory>
#include <span>
#include <sys/types.h>
#include <vector>

namespace collections::finger_tree::node {
  template<typename K, typename V>
  class Node {
    // constructors
    public:
      Node() = delete;

      Node(NodeDeep<K, V> const& deep);
      Node(NodeLeaf<K, V> const& leaf);

      Node(K const& key, V const& val);
      Node(Node<K, V> const& a, Node<K, V> const& b);
      Node(Node<K, V> const& a, Node<K, V> const& b, Node<K, V> const& c);

    // accessors
    public:
      auto kind() const -> Kind { return this->_kind; }
      auto size() const -> uint;
      auto key() const -> K const&;

    // methods
    public:
      auto get(K const& key) const -> V const*;

    // helpers
    public:
      static auto pack_nodes(
        std::span<Node<K, V> const> nodes
      ) -> std::vector<Node<K, V>>;

    public:
      auto is_uninit() const -> bool { return this->_repr == nullptr; }
      auto is_leaf() const -> bool { return this->_kind == Kind::Leaf; }
      auto is_deep() const -> bool { return this->_kind == Kind::Deep; }

      auto as_leaf() const -> NodeLeaf<K, V> const& {
        this->assert_init();
        if (this->is_leaf()) {
          return *static_cast<NodeLeaf<K, V> const*>(this->_repr.get());
        } else {
          throw VariantException("Attmpted to get Leaf reference to Deep");
        }
      }

      auto as_deep() const -> NodeDeep<K, V> const& {
        this->assert_init();
        if (this->is_deep()) {
          return *static_cast<NodeDeep<K, V> const*>(this->_repr.get());
        } else {
          throw VariantException("Attmpted to get Deep reference to Leaf");
        }
      }

      auto assert_init() const -> void;

      auto show(std::ostream& os, uint indent) const -> std::ostream&;

    private:
      Kind _kind;
      std::shared_ptr<NodeBase<K, V>> _repr;
  };

  template<typename K, typename V>
  Node<K, V>::Node(
    NodeDeep<K, V> const& deep
  ) : _kind(Kind::Deep), _repr(std::static_pointer_cast<NodeBase<K, V>>(
    std::make_shared<NodeDeep<K, V>>(deep)
  )) {}

  template<typename K, typename V>
  Node<K, V>::Node(
    NodeLeaf<K, V> const& leaf
  ) : _kind(Kind::Leaf), _repr(std::static_pointer_cast<NodeBase<K, V>>(
    std::make_shared<NodeLeaf<K, V>>(leaf)
  )) {}

  template<typename K, typename V>
  Node<K, V>::Node(
    const K& key,
    const V& val
  ) : Node<K, V>(NodeLeaf<K, V>(key, val)) {}

  template<typename K, typename V>
  Node<K, V>::Node(
    Node<K, V> const& a,
    Node<K, V> const& b
  ) : Node<K, V>(NodeDeep<K, V>(a, b)) {}
  
  template<typename K, typename V>
  Node<K, V>::Node(
    Node<K, V> const& a,
    Node<K, V> const& b,
    Node<K, V> const& c
  ) : Node<K, V>(NodeDeep<K, V>(a, b, c)) {}

  template<typename K, typename V>
  auto Node<K, V>::size() const -> uint {
    this->assert_init();

    if (this->is_leaf()) {
      return 1;
    }

    const auto& deep = this->as_deep();
    return deep.size();
  }

  template<typename K, typename V>
  auto Node<K, V>::key() const -> const K& {
    this->assert_init();

    if (this->is_leaf()) {
      const auto& leaf = this->as_leaf();
      return leaf._key;
    }

    const auto& deep = this->as_deep();
    return deep._key;
  }

  template<typename K, typename V>
  auto Node<K, V>::get(K const& key) const -> V const* {
    this->assert_init();

    if (this->is_leaf()) {
      const auto& leaf = this->as_leaf();
      if (leaf.key() == key) {
        return &leaf._val;
      } else {
        return nullptr;
      }
    }

    const auto& deep = this->as_deep();
    for (const auto& child : deep.children()) {
      if (child.key() >= key) {
        return child.get(key);
      }
    }

    return nullptr;
  }

  template<typename K, typename V>
  auto Node<K, V>::pack_nodes(
    std::span<Node<K, V> const> nodes
  ) -> std::vector<Node<K, V>> {
    std::vector<Node<K, V>> packed;

    while (nodes.size() != 0) {
      switch (nodes.size()) {
        case 2:
          packed.emplace_back(nodes[0], nodes[1]);
          nodes = nodes.subspan(2);
          break;
        case 3:
          packed.emplace_back(nodes[0], nodes[1], nodes[2]);
          nodes = nodes.subspan(3);
          break;
        case 4:
          packed.emplace_back(nodes[0], nodes[1]);
          packed.emplace_back(nodes[2], nodes[3]);
          nodes = nodes.subspan(4);
          break;
        default:
          packed.emplace_back(nodes[0], nodes[1], nodes[2]);
          nodes = nodes.subspan(3);
          break;
      }
    }

    return packed;
  }

  template<typename K, typename V>
  auto Node<K, V>::assert_init() const -> void {
    if (this->is_uninit()) {
      throw UninitException("Node is uninitialized");
    }
  }

  template<typename K, typename V>
  auto Node<K, V>::show(std::ostream& os, uint indent) const -> std::ostream& {
    if (this->_repr == nullptr) {
      return os << "null";
    }

    return this->_repr->show(os, indent);
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, Node<K, V> const& node) {
    return node.show(os, 0);
  }
}
