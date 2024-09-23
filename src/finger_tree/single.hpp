#pragma once

#include "src/finger_tree/core.hpp"
#include "src/finger_tree/_prelude.hpp"

#include <ostream>
#include <sys/types.h>

namespace finger_tree {
  template<typename K, typename V>
  class FingerTreeSingle : public FingerTreeBase<K, V> {
    // constructors
    public:
      FingerTreeSingle(const Node<K, V>& node);

    // accessors
    public:
      auto key() const -> const K& { return this->_node.key(); }
      auto node() const -> const Node<K, V>& { return this->_node; }

    // helpers
    protected:
      virtual auto show(std::ostream& os, uint indent) const -> std::ostream& override;

    private:
      Node<K, V> _node;

      friend class FingerTree<K, V>;
  };

  template<typename K, typename V>
  FingerTreeSingle<K, V>::FingerTreeSingle(
    const Node<K, V>& node
  ) : _node(node) {}

  template<typename K, typename V>
  auto FingerTreeSingle<K, V>::show(std::ostream& os, uint indent) const -> std::ostream& {
    os << "Single" << std::endl;
    os << std::string((indent + 1) * 2, ' ');
    this->_node.show(os, indent + 1);
    return os;
  }

  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, FingerTreeSingle<K, V> const& tree) {
    return tree.show(os, 0);
  }
}
