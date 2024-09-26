#pragma once

#include "src/utils/uninit_exception.hpp"

#include "src/collections/finger_tree/_prelude.hpp"
#include "src/collections/finger_tree/core.hpp"

#include "src/collections/finger_tree/base.hpp"
#include "src/collections/finger_tree/empty.hpp"
#include "src/collections/finger_tree/single.hpp"
#include "src/collections/finger_tree/deep.hpp"

#include "src/collections/finger_tree/digit/base.hpp"
#include "src/collections/finger_tree/digit/digit.hpp"

#include "src/collections/finger_tree/node/node.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace collections::finger_tree {
  // NOTE: references or pointers to this cannot leave the thread they originate
  // from, only cloned values can, i.e. always pass by value across threads
  template<typename K, typename V>
  class FingerTree {
    // constructors
    public:
      FingerTree();

      FingerTree(FingerTreeEmpty<K, V> const& empty);
      FingerTree(FingerTreeSingle<K, V> const& single);
      FingerTree(FingerTreeDeep<K, V> const& deep);

    private:
      static auto from_nodes(std::span<Node<K, V> const> nodes) -> FingerTree<K, V>;
      static auto deep_smart(
        std::span<Node<K, V> const> left,
        FingerTree<K, V> const& middle,
        std::span<Node<K, V> const> right
      ) -> FingerTree<K, V>;

    // accessors
    public:
      auto size() const -> uint;

    // methods
    public:
      auto get(K const& key) const -> V const*;

      auto push(Direction dir, K const& key, V const& val) -> void;
      auto pop(Direction dir) -> std::optional<std::pair<K, V>>;

      auto insert(K const& key, V const& val) -> std::optional<V>;
      auto remove(K const& key) -> std::optional<V>;

      auto split(
        K const& key
      ) const -> std::tuple<FingerTree<K, V>, std::optional<V>, FingerTree<K, V>>;

    private:
      auto push_node(Direction dir, Node<K, V> const& node) -> void;
      auto pop_node(Direction dir) -> std::optional<Node<K, V>>;

      auto append_nodes(Direction dir, std::span<Node<K, V> const> nodes) -> void;
      auto take_nodes(Direction dir, uint count) -> std::vector<Node<K, V>>;

      auto split_node(K const& key) const -> std::tuple<
        FingerTree<K, V>,
        std::optional<Node<K, V>>,
        FingerTree<K, V>
      >;

    // functions
    public:
      static auto concat(
        FingerTree<K, V> const& left,
        FingerTree<K, V> const& right
      ) -> FingerTree<K, V>;

    private:
      static auto concat_inner(
        FingerTree<K, V> const& left,
        std::vector<Node<K, V>> const& middle,
        FingerTree<K, V> const& right
      ) -> FingerTree<K, V>;

    // helpers
    public:
      auto is_uninit() const -> bool { return this->_repr == nullptr; }
      auto is_empty() const -> bool { return this->_kind == Kind::Empty; }
      auto is_single() const -> bool { return this->_kind == Kind::Single; }
      auto is_deep() const -> bool { return this->_kind == Kind::Deep; }

      auto as_empty() const -> FingerTreeEmpty<K, V> const& {
        if (this->is_empty()) {
          return *static_cast<FingerTreeEmpty<K, V> const*>(this->_repr.get());
        } else {
          if (this->is_single()) {
            throw VariantException("Attmpted to get Empty reference to Single");
          } else {
            throw VariantException("Attmpted to get Empty reference to Deep");
          }
        }
      }

      auto as_single() const -> FingerTreeSingle<K, V> const& {
        if (this->is_single()) {
          return *static_cast<FingerTreeSingle<K, V> const*>(this->_repr.get());
        } else {
          if (this->is_empty()) {
            throw VariantException("Attmpted to get Single reference to Empty");
          } else {
            throw VariantException("Attmpted to get Single reference to Deep");
          }
        }
      }

      auto as_deep() const -> FingerTreeDeep<K, V> const& {
        if (this->is_deep()) {
          return *static_cast<FingerTreeDeep<K, V> const*>(this->_repr.get());
        } else {
          if (this->is_empty()) {
            throw VariantException("Attmpted to get Deep reference to Empty");
          } else {
            throw VariantException("Attmpted to get Deep reference to Single");
          }
        }
      }

      auto assert_init() const -> void;
      auto ensure_unique() -> void;
      auto set(FingerTreeEmpty<K, V> empty) -> void;
      auto set(FingerTreeSingle<K, V> single) -> void;
      auto set(FingerTreeDeep<K, V> deep) -> void;

      auto show(std::ostream& os, uint indent) const -> std::ostream&;

    private:
      Kind _kind;
      std::shared_ptr<FingerTreeBase<K, V>> _repr;
  };

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree() : FingerTree<K, V>(FingerTreeEmpty<K, V>()) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(
    FingerTreeEmpty<K, V> const& empty
  ) : _kind(Kind::Empty), _repr(std::static_pointer_cast<FingerTreeBase<K, V>>(
    std::make_shared<FingerTreeEmpty<K, V>>(empty)
  )) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(
    FingerTreeSingle<K, V> const& single
  ) : _kind(Kind::Single), _repr(std::static_pointer_cast<FingerTreeBase<K, V>>(
    std::make_shared<FingerTreeSingle<K, V>>(single)
  )) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(
    FingerTreeDeep<K, V> const& deep
  ) : _kind(Kind::Deep), _repr(std::static_pointer_cast<FingerTreeBase<K, V>>(
    std::make_shared<FingerTreeDeep<K, V>>(deep)
  )) {}

  template<typename K, typename V>
  auto FingerTree<K, V>::from_nodes(
    std::span<Node<K, V> const> nodes
  ) -> FingerTree<K, V> {
    auto tree = FingerTree<K, V>();
    tree.append_nodes(Direction::Right, nodes);
    return tree;
  }

  // deep_L and deep_R special case
  // - may contain undersized sides which are filled by underflow
  // - retains the depth of the passed in nodes
  template<typename K, typename V>
  auto FingerTree<K, V>::deep_smart(
    std::span<Node<K, V> const> left,
    FingerTree<K, V> const& middle,
    std::span<Node<K, V> const> right
  ) -> FingerTree<K, V> {
    Digits<K, V> left_copy = Digits<K, V>::from_nodes(left);
    FingerTree<K, V> middle_copy = middle;
    Digits<K, V> right_copy = Digits<K, V>::from_nodes(right);

    if (left_copy.digit_size() == 0) {
      if (middle.is_empty()) {
        return FingerTree<K, V>::from_nodes(right_copy.digits());
      }

      // NOTE: middle cannot contain leaves and is not empty
      Node<K, V> underflow = *middle_copy.pop_node(Direction::Left);
      left_copy.unpack(Direction::Right, underflow.as_deep());
    }

    if (right_copy.digit_size() == 0) {
      if (middle.is_empty()) {
        return FingerTree<K, V>::from_nodes(left_copy.digits());
      }

      // NOTE: middle cannot contain leaves and is not empty
      Node<K, V> underflow = *middle_copy.pop_node(Direction::Right);
      right_copy.unpack(Direction::Left, underflow.as_deep());
    }

    return FingerTree(FingerTreeDeep<K, V>(left_copy, middle_copy, right_copy));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::size() const -> uint {
    this->assert_init();

    if (this->is_empty()) {
      return 0;
    }

    if (this->is_single()) {
      return this->as_single().node().size();
    }

    return this->as_deep()._size;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::get(K const& key) const -> V const* {
    this->assert_init();

    if (this->is_empty()) {
      return nullptr;
    }

    if (this->is_single()) {
      const auto& single = this->as_single();

      if (single.key() >= key) {
        return single.node().get(key);
      } else {
        return nullptr;
      }
    }

    const auto& deep = this->as_deep();
    if (deep.left().key() >= key) {
      return deep.left().get(key);
    }

    const auto& middle = deep.middle();
    bool is_middle = false;
    if (middle.is_single()) {
      is_middle = middle.as_single().key() >= key;
    }

    if (middle.is_deep()) {
      is_middle = middle.as_deep().key() >= key;
    }

    if (is_middle) {
      return middle.get(key);
    }

    if (deep.right().key() >= key) {
      return deep.right().get(key);
    }

    return nullptr;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push(Direction dir, K const& key, V const& val) -> void {
    this->push_node(dir, Node<K, V>(key, val));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::pop(Direction dir) -> std::optional<std::pair<K, V>> {
    auto node = this->pop_node(dir);

    std::optional<std::pair<K, V>> unpacked;
    if (node) {
      unpacked = std::optional(std::pair(
        node->as_leaf().key(),
        node->as_leaf().val()
      ));
    }

    return unpacked;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::insert(
    K const& key,
    V const& val
  ) -> std::optional<V> {
    auto [left, found, right] = this->split(key);
    left.push_node(Direction::Right, Node<K, V>(key, val));
    *this = FingerTree<K, V>::concat(left, right);
    return found;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::remove(K const& key) -> std::optional<V> {
    auto [left, found, right] = this->split(key);
    *this = FingerTree<K, V>::concat(left, right);
    return found;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::split(
    const K& key
  ) const -> std::tuple<FingerTree<K, V>, std::optional<V>, FingerTree<K, V>> {
    auto [left, node, right] = this->split_node(key);
    std::optional<V> unpacked;

    if (node) {
      const auto& leaf = node->as_leaf();
      if (leaf.key() == key) {
        unpacked = std::optional(leaf.val());
      } else {
        right.push_node(Direction::Left, *node);
      }
    }

    return std::tuple(left, unpacked, right);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push_node(
    Direction dir,
    Node<K, V> const& node
  ) -> void {
    if (this->is_empty()) {
      this->set(FingerTreeSingle<K, V>(node));
      return;
    }

    if (this->is_single()) {
      Node<K, V> other = this->as_single().node();
      this->set(FingerTreeDeep<K, V>(
          Digits<K, V>(dir == Direction::Left ? node : other),
          FingerTree<K, V>(),
          Digits<K, V>(dir == Direction::Left ? other : node)
      ));
      return;
    }

    // TODO: do not unconditionally copy, use ensure_unique
    const auto& deep = this->as_deep();

    Digits<K, V> left = deep.left();
    Digits<K, V> right = deep.right();
    FingerTree<K, V> middle = deep.middle();

    std::optional<Node<K, V>> overflow;
    switch (dir) {
      case Direction::Left:
        if (left.digit_size() == 4) {
          overflow = std::optional(Node<K, V>(left.pack(Direction::Right)));
        }
        left.push(Direction::Left, node);
        break;
      case Direction::Right:
        if (right.digit_size() == 4) {
          overflow = std::optional(Node<K, V>(right.pack(Direction::Left)));
        }
        right.push(Direction::Right, node);
        break;
    }

    if (overflow) {
      middle.push_node(dir, *overflow);
    }

    this->set(FingerTreeDeep<K, V>(left, middle, right));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::pop_node(Direction dir) -> std::optional<Node<K, V>> {
    if (this->is_empty()) {
      return std::optional<Node<K, V>>();
    }

    if (this->is_single()) {
      Node<K, V> node = this->as_single().node();
      this->set(FingerTreeEmpty<K, V>());
      return node;
    }

    // TODO: do not unconditionally copy, use ensure_unique
    const auto& deep = this->as_deep();

    Digits<K, V> left = deep.left();
    Digits<K, V> right = deep.right();
    FingerTree<K, V> middle = deep.middle();

    if (middle.is_empty()) {
      if (left.digit_size() == 1 && right.digit_size() == 1) {
        this->set(FingerTreeSingle<K, V>(
          dir == Direction::Left ? right.digits().back() : left.digits().front()
        ));
        return Node<K, V>(
          dir == Direction::Left ? left.digits().front() : right.digits().back()
        );
      }

      if (dir == Direction::Left && left.digit_size() == 1) {
        Node<K, V> other = right.left();
        right.pop(Direction::Left);
        left.push(Direction::Right, other);
        Node<K, V> node = left.left();
        left.pop(Direction::Left);
        this->set(FingerTreeDeep<K, V>(left, middle, right));
        return node;
      }

      if (dir == Direction::Right && right.digit_size() == 1) {
        Node<K, V> other = left.right();
        left.pop(Direction::Right);
        right.push(Direction::Left, other);
        Node<K, V> node = right.right();
        right.pop(Direction::Right);
        this->set(FingerTreeDeep<K, V>(left, middle, right));
        return node;
      }
    }

    if (dir == Direction::Left && left.digit_size() > 1) {
      Node<K, V> node = left.left();
      left.pop(Direction::Left);
      this->set(FingerTreeDeep<K, V>(left, middle, right));
      return node;
    }

    if (dir == Direction::Right && right.digit_size() > 1) {
      Node<K, V> node = right.right();
      right.pop(Direction::Right);
      this->set(FingerTreeDeep<K, V>(left, middle, right));
      return node;
    }

    // NOTE: we know middle is not empty
    Node<K, V> underflow = *middle.pop_node(dir);
    std::optional<Node<K, V>> node;

    switch (dir) {
      case Direction::Left:
        // NOTE: a middle tree cannot contain leaves
        left.unpack(Direction::Right, underflow.as_deep());
        node = std::optional(left.left());
        left.pop(Direction::Left);
        break;
      case Direction::Right:
        // NOTE: a middle tree cannot contain leaves
        right.unpack(Direction::Left, underflow.as_deep());
        node = std::optional(right.right());
        right.pop(Direction::Right);
        break;
    }

    this->set(FingerTreeDeep<K, V>(left, middle, right));
    return node;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::append_nodes(
    Direction dir,
    std::span<Node<K, V> const> nodes
  ) -> void {
    if (dir == Direction::Left) {
      for (auto it = nodes.rbegin(); it != nodes.rend(); it++) {
        this->push_node(Direction::Left, *it);
      }
    } else {
      for (const auto& node : nodes) {
        this->push_node(Direction::Right, node);
      }
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::take_nodes(
    Direction dir,
    uint count
  ) -> std::vector<Node<K, V>> {
    count = std::min(count, this->size());

    std::vector<Node<K, V>> nodes;
    nodes.reserve(count);

    for (uint i = 0; i <= count; i++) {
      // NOTE: we've asserted that at least this many elements exist
      nodes.emplace_back(*this->pop_node(dir));
    }

    if (dir == Direction::Right) {
      std::reverse(nodes.begin(), nodes.end());
    }

    return nodes;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::split_node(const K& key) const -> std::tuple<
    FingerTree<K, V>,
    std::optional<Node<K, V>>,
    FingerTree<K, V>
  > {
    if (this->is_empty()) {
      return std::tuple(FingerTree(), std::optional<Node<K, V>>(), FingerTree());
    }

    if (this->is_single()) {
      const auto& single = this->as_single();
      if (single.key() >= key) {
        return std::tuple(FingerTree(), std::optional(single.node()), FingerTree());
      }

      return std::tuple(*this, std::optional<Node<K, V>>(), FingerTree());
    }

    const auto& deep = this->as_deep();
    const auto& middle = deep.middle();

    if (deep.left().key() >= key) {
      auto [left, node, right] = deep.left().split(key);
        return std::tuple(
        FingerTree<K, V>::from_nodes(left),
        node,
        FingerTree<K, V>::deep_smart(right, middle, deep.right().digits())
      );
    }

    bool is_middle = false;
    if (middle.is_single()) {
      is_middle = middle.as_single().key() >= key;
    }

    if (middle.is_deep()) {
      is_middle = middle.as_deep().key() >= key;
    }

    if (is_middle) {
      auto [left, packed_node, right] = middle.split_node(key);

      // NOTE: middle cannot contain leaves and is not empty
      auto [inner_left, node, inner_right] = packed_node->as_deep().split(key);
      return std::tuple(
        FingerTree<K, V>::deep_smart(deep.left().digits(), left, inner_left),
        node,
        FingerTree<K, V>::deep_smart(inner_right, right, deep.right().digits())
      );
    }

    auto [left, node, right] = deep.right().split(key);
    return std::tuple(
      FingerTree<K, V>::deep_smart(deep.left().digits(), middle, left),
      node,
      FingerTree<K, V>::from_nodes(right)
    );
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::concat(
    FingerTree<K, V> const& left,
    FingerTree<K, V> const& right
  ) -> FingerTree<K, V> {
    return FingerTree<K, V>::concat_inner(left, std::vector<Node<K, V>>(), right);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::concat_inner(
    FingerTree<K, V> const& left,
    std::vector<Node<K, V>> const& middle,
    FingerTree<K, V> const& right
  ) -> FingerTree<K, V> {
    if (left.is_empty()) {
      FingerTree<K, V> copy = right;
      copy.append_nodes(Direction::Left, middle);
      return copy;
    }

    if (right.is_empty()) {
      FingerTree<K, V> copy = left;
      copy.append_nodes(Direction::Right, middle);
      return copy;
    }

    if (left.is_single()) {
      FingerTree<K, V> copy = right;
      copy.append_nodes(Direction::Left, middle);
      copy.push_node(Direction::Left, left.as_single().node());
      return copy;
    }

    if (right.is_single()) {
      FingerTree<K, V> copy = left;
      copy.append_nodes(Direction::Right, middle);
      copy.push_node(Direction::Right, right.as_single().node());
      return copy;
    }

    const auto& left_deep = left.as_deep();
    const auto& right_deep = right.as_deep();

    // TODO: this vector can be used as in- and output by packing nodes in place
    std::vector<Node<K, V>> concat;
    concat.reserve(
      left_deep.right().digit_size()
      + middle.size()
      + right_deep.left().digit_size()
    );

    for (const auto& node : left_deep.right().digits()) {
      concat.emplace_back(node);
    }

    for (const auto& node : middle) {
      concat.emplace_back(node);
    }

    for (const auto& node : right_deep.left().digits()) {
      concat.emplace_back(node);
    }

    std::vector<Node<K, V>> packed = Node<K, V>::pack_nodes(
      std::span(concat)
    );

    return FingerTree(FingerTreeDeep<K, V>(
      left_deep.left(),
      FingerTree<K, V>::concat_inner(
        left_deep.middle(),
        packed,
        right_deep.middle()
      ),
      right_deep.right()
    ));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::assert_init() const -> void {
    if (this->is_uninit()) {
      throw UninitException("FingerTree is uninitialized");
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::ensure_unique() -> void {
    this->assert_init();

    // NOTE: no pointers or references to a FingerTree my be sent to another
    // thread, only values of FingerTree, therefor no copy may be done between
    // this check and subsequent writes
    if (this->_repr.use_count() == 1) {
      return;
    }

    if (this->is_empty()) {
      const auto& empty = this->as_empty();
      this->set(empty);
    }

    if (this->is_single()) {
      const auto& single = this->as_single();
      this->set(single);
    }

    if (this->is_deep()) {
      const auto& deep = this->as_deep();
      this->set(deep);
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::set(FingerTreeEmpty<K, V> empty) -> void {
    this->_repr = std::static_pointer_cast<FingerTreeBase<K, V>>(
      std::make_shared<FingerTreeEmpty<K, V>>(empty)
    );
    this->_kind = Kind::Empty;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::set(FingerTreeSingle<K, V> single) -> void {
    this->_repr = std::static_pointer_cast<FingerTreeBase<K, V>>(
      std::make_shared<FingerTreeSingle<K, V>>(single)
    );
    this->_kind = Kind::Single;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::set(FingerTreeDeep<K, V> deep) -> void {
    this->_repr = std::static_pointer_cast<FingerTreeBase<K, V>>(
      std::make_shared<FingerTreeDeep<K, V>>(deep)
    );
    this->_kind = Kind::Deep;
  }


  template<typename K, typename V>
  auto FingerTree<K, V>::show(
    std::ostream& os,
    uint indent
  ) const -> std::ostream& {
    if (this->_repr == nullptr) {
      return os << "null";
    }

    return this->_repr->show(os, indent);
  }


  template<typename K, typename V>
  std::ostream& operator<<(std::ostream& os, FingerTree<K, V> const& tree) {
    return tree.show(os, 0);
  }
}
