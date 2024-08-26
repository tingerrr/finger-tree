#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <sys/types.h>

#define HALF_CEIL(N) ((N) % 2 == 0) ? ((N) / 2) : ((N) / 2 + 1)

namespace btree::node {
  constexpr uint ORDER_DEFAULT = 32;

  template<typename K, typename V, uint N>
  class Node;

  template<typename K, typename V, uint N>
  class Deep;

  template<typename K, typename V, uint N>
  class Leaf;

  template<typename K, typename V, uint N>
  using SharedNode = std::shared_ptr<Node<K, V, N>>;

  template<typename K, typename V, uint N>
  auto make_shared_node(const Deep<K, V, N>& node) -> SharedNode<K, V, N> {
    return std::static_pointer_cast<Node<K, V, N>>(
      std::make_shared<Deep<K, V, N>>(node)
    );
  }

  template<typename K, typename V, uint N>
  auto make_shared_node(const Leaf<K, V, N>& node) -> SharedNode<K, V, N> {
    return std::static_pointer_cast<Node<K, V, N>>(
      std::make_shared<Leaf<K, V, N>>(node)
    );
  }

  template<typename T>
  auto split_vector(std::vector<T>&& vec) -> std::pair<std::vector<T>, std::vector<T>> {
    std::vector<T> other(
      std::make_move_iterator(vec.begin() + vec.size() / 2),
      std::make_move_iterator(vec.end())
    );
    vec.erase(vec.begin() + vec.size() / 2, vec.end());

    return std::make_pair(vec, other);
  }

  template<typename K, typename V, uint N>
  using Split = std::pair<SharedNode<K, V, N>, SharedNode<K, V, N>>;

  template<typename K, typename V, uint N>
  using Inserted = SharedNode<K, V, N>;

  template<typename K, typename V, uint N>
  using InsertResult = std::variant<Split<K, V, N>, Inserted<K, V, N>>;

  template<typename N>
  auto make_result(N&& node) -> InsertResult<
    typename N::KeyType, typename N::ValueType, N::ORDER
  > {
    return std::static_pointer_cast<Node<
      typename N::KeyType, typename N::ValueType, N::ORDER
    >>(std::make_shared<N>(node));
  }

  template<typename N>
  auto make_result(
    N&& left,
    N&& right
  ) -> InsertResult<
    typename N::KeyType, typename N::ValueType, N::ORDER
  > {
    return std::make_pair(make_shared_node(left), make_shared_node(right));
  }

  template<typename K, typename V, uint N>
  auto show(const Node<K, V, N>& node, int indent) -> void {
    auto istr = std::string();

    for (auto i = 0; i < indent; i++) {
      istr += "|   ";
    }

    if (node.is_leaf()) {
      auto leaf = static_cast<const Leaf<K, V, N>&>(node);

      std::cout << istr << "Leaf m: " << leaf.measure() << std::endl;
      std::cout << istr << "k: ";
      std::copy(
        leaf.keys().begin(),
        leaf.keys().end(),
        std::ostream_iterator<int>(std::cout, ", ")
      );
      std::cout << std::endl << istr << "v: ";
      std::copy(
        leaf.vals().begin(),
        leaf.vals().end(),
        std::ostream_iterator<int>(std::cout, ", ")
      );
      std::cout << std::endl;
    } else {
      auto deep = static_cast<const Deep<K, V, N>&>(node);

      std::cout << istr << "The Deep m: " << deep.measure() << std::endl;
      for (auto& child : deep.children()) {
        show(*child, indent + 1);
      }
    }
  }
}
