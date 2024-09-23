#include "src/finger_tree/finger_tree.hpp"

#include <iostream>

using FT = finger_tree::FingerTree<int, int>;
using Dir = finger_tree::Direction;

auto main() -> int {
  auto tree = FT();

  for (int i = 0; i < 50; i++) {
    tree.insert(i, i);
  }

  std::cout << "TREE" << std::endl;
  std::cout << tree << std::endl;

  return 0;
}
