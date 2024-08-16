#include "btree/node.hpp"

#include <iostream>
#include <memory>

int main() {
  auto n = std::static_pointer_cast<btree::Node<int, int, 3>>(
    std::make_shared<btree::Leaf<int, int, 3>>()
  );
  n->show();

  n = std::get<1>(n->insert(1, 1));
  std::cout << "1 -> 1" << std::endl;
  n->show();

  n = std::get<1>(n->insert(2, 2));
  std::cout << "2 -> 2" << std::endl;
  n->show();

  n = std::get<1>(n->insert(3, 3));
  std::cout << "3 -> 3" << std::endl;
  n->show();

  n = std::get<1>(n->insert(2, 42));
  std::cout << "2 -> 42" << std::endl;
  n->show();

  auto nn = std::get<0>(n->insert(4, 1));
  std::cout << "4 -> 1" << std::endl;
  std::cout << "left" << std::endl;
  std::get<0>(nn)->show();
  std::cout << "k: " << std::get<1>(nn) << std::endl;
  std::cout << "right" << std::endl;
  std::get<2>(nn)->show();

  return 0;
}
