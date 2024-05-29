#include "node.hpp"

#include <iostream>
#include <iterator>

int main() {
  Node<int, int, 3> n;
  n.insert(2, 0);
  n.insert(2, 0);
  n.insert(3, 0);
  n.insert(1, 0);
  n.insert(22, 0);

  std::cout << "keys: ";
  std::copy(
    n.keys().begin(),
    n.keys().end(),
    std::ostream_iterator<int>(std::cout, ", ")
  );
  std::cout << std::endl;

  return 0;
}
