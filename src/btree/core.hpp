#pragma once

#include "src/btree/node/core.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace btree {
  template<typename K, typename V, uint N>
  class BTree;

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
  auto show(node::Node<K, V, N>& node, int indent) -> void {
    auto istr = std::string();

    for (auto i = 0; i < indent; i++) {
      istr += "|   ";
    }

    if (node.is_leaf()) {
      auto leaf = static_cast<node::Leaf<K, V, N>&>(node);

      std::cout << istr << "Leaf" << std::endl;
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
      auto deep = static_cast<node::Deep<K, V, N>&>(node);

      std::cout << istr << "The Deep" << std::endl;
      std::cout << istr << "k: ";
      std::copy(
        deep.keys().begin(),
        deep.keys().end(),
        std::ostream_iterator<int>(std::cout, ", ")
      );
      std::cout << std::endl;

      for (auto& child : deep.children()) {
        show(*child, indent + 1);
      }
    }
  }
}
