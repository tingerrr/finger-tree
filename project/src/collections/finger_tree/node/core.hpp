#pragma once

// forward delarations

namespace collections::finger_tree::node {
  template<typename K, typename V>
  class NodeDeep;

  template<typename K, typename V>
  class NodeLeaf;

  template<typename K, typename V>
  class NodeBase;

  template<typename K, typename V>
  class Node;

  enum class Kind { Leaf, Deep };
}
