#include "btree/node.hpp"

#include <iostream>
#include <string>

template<typename K, typename V, uint N>
auto show(btree::Node<K, V, N>& node, int indent) -> void {
  auto istr = std::string();

  for (auto i = 0; i < indent; i++) {
    istr += "|   ";
  }

  if (node.is_leaf()) {
    auto leaf = static_cast<btree::Leaf<K, V, N>&>(node);

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
    auto deep = static_cast<btree::Deep<K, V, N>&>(node);

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

int main() {
  auto r = btree::BTree<int, int, 3>();

  for (auto i = 0; i < 4; i++) {
    r = r.insert(i, i);
  }

  show(r.root(), 0);

  return 0;
}
