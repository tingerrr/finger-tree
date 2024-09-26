#pragma once

#include <sys/types.h>

namespace collections::b_tree {
  constexpr uint ORDER_DEFAULT = 32;

  template<typename K, typename V, uint N>
  class BTree;
}
