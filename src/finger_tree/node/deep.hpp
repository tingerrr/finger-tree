#pragma once

#include "src/finger_tree/node/core.hpp"

#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <sys/types.h>
#include <vector>

namespace finger_tree::node {
  template<typename K, typename V>
  class NodeDeep : public NodeBase<K, V> {
    // constructors
    public:
      NodeDeep() = delete;

      NodeDeep(Node<K, V> const& a, Node<K, V> const& b);
      NodeDeep(Node<K, V> const& a, Node<K, V> const& b, Node<K, V> const& c);

    // accessors
    public:
      auto size() const -> bool { return this->_size; }
      auto key() const -> bool { return this->_key; }

      auto is_two() const -> bool { return this->children().size() == 2; }
      auto is_three() const -> bool { return this->children().size() == 3; }

      auto children() const -> std::span<Node<K, V> const> {
        return std::span(this->_children);
      }

    // methods
    public:
      auto split(K const& key) const -> std::tuple<
        std::span<Node<K, V> const>,
        std::optional<Node<K, V>>,
        std::span<Node<K, V> const>
      >;

    // helpers
    protected:
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& override;

    private:
      uint _size;
      K _key;
      std::vector<Node<K, V>> _children;

      friend class Node<K, V>;
  };

  template<typename K, typename V>
  NodeDeep<K, V>::NodeDeep(
    Node<K, V> const& a,
    Node<K, V> const& b
  ) : _size(a.size() + b.size()), _key(b.key()), _children() {
    this->_children.reserve(2);
    this->_children.emplace_back(a);
    this->_children.emplace_back(b);
  }

  template<typename K, typename V>
  NodeDeep<K, V>::NodeDeep(
    Node<K, V> const& a,
    Node<K, V> const& b,
    Node<K, V> const& c
  ) : _size(a.size() + b.size() + c.size()), _key(c.key()), _children() {
    this->_children.reserve(3);
    this->_children.emplace_back(a);
    this->_children.emplace_back(b);
    this->_children.emplace_back(c);
  }

  template<typename K, typename V>
  auto NodeDeep<K, V>::split(K const& key) const -> std::tuple<
    std::span<Node<K, V> const>,
    std::optional<Node<K, V>>,
    std::span<Node<K, V> const>
  > {
    std::span<Node<K, V> const> nodes = this->children();

    for (uint i = 0; i < nodes.size(); i++) {
      if (nodes[i].key() >= key) {
        return std::tuple(
          nodes.subspan(0, i),
          std::optional(nodes[i]),
          nodes.subspan(i + 1)
        );
      }
    }

    return std::tuple(
      nodes,
      std::optional<Node<K, V>>(),
      std::span<Node<K, V> const>()
    );
  }
 

  template<typename K, typename V>
  auto NodeDeep<K, V>::show(std::ostream& os, uint indent) const -> std::ostream& {
    auto istr = std::string(indent * 2, ' ');
    auto istr2 = std::string((indent + 1) * 2, ' ');

    os << "<" << std::endl;
    for (const auto& child : this->_children) {
      os << istr2;
      child.show(os, indent + 1);
      os << std::endl;
    }
    os << istr << ">";
    return os;
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, NodeDeep<K, V> const& node) {
    return node.show(os, 0);
  }
}
