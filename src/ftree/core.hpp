#pragma once

#include <sys/types.h>

namespace ftree {
  template<typename K, typename V>
  class Empty;

  template<typename K, typename V>
  class Single;

  template<typename K, typename V>
  class Deep;

  template<typename K, typename V>
  class FingerTree;

  enum Direction { Left, Right };

  auto invert(Direction dir) -> Direction {
    return dir == Left ? Right : Left;
  }
}
