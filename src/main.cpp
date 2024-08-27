#include "btree.hpp"

int main() {
  auto r = btree::BTree<int, int>();

  for (auto i = 0; i < 100; i++) {
    r = r.insert(i, i);
  }

  r.show();

  return 0;
}
