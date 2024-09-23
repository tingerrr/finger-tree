#pragma once

namespace finger_tree {
  template<typename K, typename V>
  class FingerTreeEmpty;

  template<typename K, typename V>
  class FingerTreeSingle;

  template<typename K, typename V>
  class FingerTreeDeep;

  template<typename K, typename V>
  class FingerTreeBase;

  template<typename K, typename V>
  class FingerTree;

  enum class Direction { Left, Right };

  enum class Kind { Deep, Single, Empty };

  namespace node {}

  namespace digit {}
}
