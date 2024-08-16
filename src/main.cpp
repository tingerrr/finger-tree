#include "btree/node.hpp"

#include <iostream>
#include <iterator>

int main() {
  auto n = btree::Leaf<int, int>();

  n.upsert(1, 1);
  n.upsert(2, 2);
  n.upsert(3, 3);

  n.upsert(2, 42);

  std::cout << n.CHILD_MIN << "..=" << n.CHILD_MAX << " @ " << n.size() << std::endl;

  std::cout << "keys: ";
  std::copy(
    n.keys().begin(),
    n.keys().end(),
    std::ostream_iterator<int>(std::cout, ", ")
  );
  std::cout << std::endl;
  std::cout << "vals: ";
  std::copy(
    n.vals().begin(),
    n.vals().end(),
    std::ostream_iterator<int>(std::cout, ", ")
  );
  std::cout << std::endl;

  return 0;
}
