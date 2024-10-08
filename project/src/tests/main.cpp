#include "src/collections/finger_tree/finger_tree.hpp"

#include <iostream>

using FT = collections::finger_tree::FingerTree<int, int>;
using Dir = collections::finger_tree::Direction;

auto main() -> int {
  auto tree = FT();

  for (int i = 0; i < 66; i++) {
    tree.push(Dir::Right, i, i);
  }

  std::cout << tree << std::endl;

  return 0;
}
