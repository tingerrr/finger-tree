#pragma once

#include <sys/types.h>

namespace btree {
  constexpr uint ORDER_DEFAULT = 32;

  template<typename K, typename V, uint N>
  class BTree;
}
