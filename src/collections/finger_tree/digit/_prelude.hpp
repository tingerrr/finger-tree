#pragma once

#include "src/collections/finger_tree/node/core.hpp"

namespace collections::finger_tree::digit {
  template<typename K, typename V>
  using Node = node::Node<K, V>;

  template<typename K, typename V>
  using NodeBase = node::NodeBase<K, V>;

  template<typename K, typename V>
  using NodeDeep = node::NodeDeep<K, V>;

  template<typename K, typename V>
  using NodeLeaf = node::NodeLeaf<K, V>;
}
