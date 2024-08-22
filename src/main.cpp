#include "btree.hpp"

int main() {
  auto r = btree::BTree<int, int, 3>();

  for (auto i = 0; i < 4; i++) {
    r = r.insert(i, i);
  }

  btree::show(r.root(), 0);

  return 0;
}
