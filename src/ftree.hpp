#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/deep.hpp"
#include "src/ftree/digit.hpp"
#include "src/ftree/node.hpp"
#include "src/ftree/single.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace ftree {
  template<typename K, typename V>
  class FingerTree {
    private:
      class Repr {
        public:
          using Variant = std::variant<Single<K, V>, Deep<K, V>>;

        public:
          Repr() = delete;
          Repr(const Repr& other) = delete;
          Repr(Repr& other) = delete;
          Repr(Repr&& other) = delete;

        public:
          Repr(Single<K, V>&& repr);
          Repr(Deep<K, V>&& repr);

        public:
          Variant _repr;
      };

    public:
      FingerTree();

      static auto from(std::span<const std::pair<K, V>> nodes) -> FingerTree<K, V>;

    private:
      FingerTree(Single<K, V>&& repr);
      FingerTree(Deep<K, V>&& repr);

      static auto from_impl(
        std::span<const node::Node<K, V>> nodes
      ) -> FingerTree<K, V>;

      static auto deep_smart(
        digit::Digits<K, V>&& left,
        FingerTree<K, V>&& middle,
        digit::Digits<K, V>&& right
      ) -> FingerTree<K, V>;

    public:
      auto size() const -> uint;

      auto as_single() const -> const Single<K, V>*;
      auto as_deep() const -> const Deep<K, V>*;

      auto is_empty() const -> bool { return this->_repr == nullptr; };
      auto is_single() const -> bool { return this->as_single() != nullptr; };
      auto is_deep() const -> bool { return this->as_deep() != nullptr; };

    public:
      auto get(const K& key) -> std::optional<V>;

      auto push(Direction dir, const K& key, const V& val) -> void;
      auto append(Direction dir, std::span<const std::pair<K, V>> pairs) -> void;
      auto insert(const K& key, const V& val) -> std::optional<V>;

      auto pop(Direction dir) -> std::optional<std::pair<K, V>>;
      auto take(Direction dir, uint count) -> std::vector<std::pair<K, V>>;
      auto remove(const K& key) -> std::optional<V>;

      auto split(
        const K& key
      ) -> std::tuple<FingerTree<K, V>, std::optional<V>, FingerTree<K, V>>;

      static auto concat(
        const FingerTree<K, V>& left,
        const FingerTree<K, V>& right
      ) -> FingerTree<K, V>;

    public:
      auto show(uint indent) const -> void;

    private:
      auto get_impl(const K& key) -> std::optional<node::Node<K, V>>;

      auto push_impl(Direction dir, const node::Node<K, V>& node) -> void;
      auto append_impl(Direction dir, std::span<const node::Node<K, V>> nodes) -> void;
      auto insert_impl(
        const node::Node<K, V>& node
      ) -> std::optional<node::Node<K, V>>;

      auto pop_impl(Direction dir) -> std::optional<node::Node<K, V>>;
      auto take_impl(Direction dir, uint count) -> std::vector<node::Node<K, V>>;
      auto remove_impl(const K& key) -> std::optional<node::Node<K, V>>;

      auto split_impl(const K& key) -> std::tuple<
        FingerTree<K, V>,
        std::optional<node::Node<K, V>>,
        FingerTree<K, V>
      >;

      static auto concat_impl(
        const FingerTree<K, V>& left,
        std::span<const node::Node<K, V>> middle,
        const FingerTree<K, V>& right
      ) -> FingerTree<K, V>;

      private:
        std::shared_ptr<Repr> _repr;
  };

  template<typename K, typename V>
  FingerTree<K, V>::Repr::Repr(Deep<K, V>&& repr)
    : _repr(Repr::Variant(std::move(repr))) {}

  template<typename K, typename V>
  FingerTree<K, V>::Repr::Repr(Single<K, V>&& repr)
    : _repr(Repr::Variant(std::move(repr))) {}

  //
  // public constructors and consturctor functions
  //
  template<typename K, typename V>
  FingerTree<K, V>::FingerTree() : _repr(nullptr) {}

  template<typename K, typename V>
  auto from(std::span<const std::pair<K, V>> pairs) -> FingerTree<K, V> {
    // TODO: ensure invariants
    std::vector<node::Node<K, V>> nodes;
    nodes.reserve(pairs.size());

    for (const auto& [k, v] : pairs) {
      nodes.push_back(nodes);
    }
    return FingerTree<K, V>::from_impl(std::span(nodes));
  }

  //
  // private constructors and consturctor functions
  //
  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(Single<K, V>&& repr)
    : _repr(std::make_shared<Repr>(std::move(repr))) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(Deep<K, V>&& repr)
    : _repr(std::make_shared<Repr>(std::move(repr))) {}

  template<typename K, typename V>
  auto FingerTree<K, V>::from_impl(
    std::span<const node::Node<K, V>> nodes
  ) -> FingerTree<K, V> {
    auto tree = FingerTree<K, V>();
    tree.append_impl(Right, nodes);
    return tree;
  }

  // deep_L and deep_R special case
  // - may contain undersized sides which are filled by underflow
  // - retains the depth of the passed in nodes
  template<typename K, typename V>
  auto FingerTree<K, V>::deep_smart(
    digit::Digits<K, V>&& left,
    FingerTree<K, V>&& middle,
    digit::Digits<K, V>&& right
  ) -> FingerTree<K, V> {
    digit::Digits<K, V> left_copy = left;
    digit::Digits<K, V> right_copy = right;

    if (left_copy.digits().size() == 0) {
      if (middle.is_empty()) {
        return FingerTree<K, V>::from_impl(right_copy.digits());
      }

      // NOTE: middle cannot contain leaves and is not empty
      auto underflow = *middle.pop_impl(Left);
      left_copy.put_packed(Right, *underflow.as_deep());
    } else if (right_copy.digits().size() == 0) {
      if (middle.is_empty()) {
        return FingerTree<K, V>::from_impl(left_copy.digits());
      }

      // NOTE: middle cannot contain leaves and is not empty
      auto underflow = *middle.pop_impl(Right);
      right_copy.put_packed(Left, *underflow.as_deep());
    }

    return FingerTree(Deep<K, V>(
      std::move(left_copy),
      std::move(middle),
      std::move(right_copy)
    ));
  }

  //
  // getters and helper functions
  //
  template<typename K, typename V>
  auto FingerTree<K, V>::size() const -> uint {
    if (this->is_empty()) {
      return 0;
    }

    if (const auto* single = this->as_single()) {
      return single->node().size();
    }

    return this->as_deep()->size();
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::as_single() const -> const Single<K, V>* {
    return this->is_empty()
      ? nullptr
      : std::get_if<Single<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::as_deep() const -> const Deep<K, V>* {
    return this->is_empty()
      ? nullptr
      : std::get_if<Deep<K, V>>(&this->_repr->_repr);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::get_impl(const K& key) -> std::optional<node::Node<K, V>> {
    if (this->is_empty()) {
      return std::optional<node::Node<K, V>>();
    }

    if (const auto* single = this->as_single()) {
      if (single->key() <= key) {
        return single->node().get(key);
      } else {
        return std::optional<node::Node<K, V>>();
      }
    }

    const auto* deep = this->as_deep();
    if (deep->left().back().key() <= key) {
      return node::Node<K, V>::digit_get(deep->left());
    } else if (deep->middle().key() <= key) {
      return deep->middle().get_impl(key);
    } else if (deep->right().back().key() <= key) {
      return node::Node<K, V>::digit_get(deep->right());
    } else {
      return std::optional<node::Node<K, V>>();
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push_impl(
    Direction dir,
    const node::Node<K, V>& node
  ) -> void {
    if (this->is_empty()) {
      this->_repr = std::make_shared<Repr>(Single(node));
      return;
    }

    if (const auto* single = this->as_single()) {
      node::Node<K, V> other = single->node();

      this->_repr = std::make_shared<Repr>(Deep<K, V>(
        digit::Digits<K, V>(dir == Left ? node : other),
        digit::Digits<K, V>(dir == Left ? other : node)
      ));
      return;
    }

    const auto* deep = this->as_deep();

    digit::Digits<K, V> left = deep->left();
    digit::Digits<K, V> right = deep->right();
    FingerTree<K, V> middle = deep->middle();

    std::optional<node::Node<K, V>> overflow;
    switch (dir) {
      case Left:
        if (left.is_max_digit_size()) {
          overflow = left.take_packed(Right);
        }
        left.push(Left, node);
        break;
      case Right:
        if (right.is_max_digit_size()) {
          overflow = right.take_packed(Left);
        }
        right.push(Right, node);
        break;
    }

    if (overflow) {
      middle.push_impl(dir, *overflow);
    }

    this->_repr = std::make_shared<Repr>(Deep<K, V>(
      std::move(left),
      std::move(middle),
      std::move(right)
    ));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::append_impl(
    Direction dir,
    std::span<const node::Node<K, V>> nodes
  ) -> void {
    switch (dir) {
      case Left:
        for (auto it = nodes.rbegin(); it != nodes.rend(); it++) {
          this->push_impl(dir, *it);
        }
        break;
      case Right:
        for (auto it = nodes.begin(); it != nodes.end(); it++) {
          this->push_impl(dir, *it);
        }
        break;
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::insert_impl(
    const node::Node<K, V>& node
  ) -> std::optional<node::Node<K, V>> {
    auto [left, found, right] = this->split_impl(node.key());
    left.push_impl(Right, node);

    *this = FingerTree<K, V>::concat(left, right);
    return found;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::pop_impl(Direction dir) -> std::optional<node::Node<K, V>> {
    if (this->is_empty()) {
      return std::optional<node::Node<K, V>>();
    }

    if (const auto* single = this->as_single()) {
      this->_repr = nullptr;
      return std::optional<node::Node<K, V>>(single->node());
    }

    const auto* deep = this->as_deep();

    digit::Digits<K, V> left = deep->left();
    digit::Digits<K, V> right = deep->right();
    FingerTree<K, V> middle = deep->middle();

    if (middle.is_empty()) {
      if (left.is_min_digit_size() && right.is_min_digit_size()) {
        this->_repr = std::make_shared<Repr>(Single(
          dir == Left
            ? right.digits().back()
            : left.digits().front()
        ));
        return std::optional<node::Node<K, V>>(
          dir == Left
            ? left.digits().front()
            : right.digits().back()
        );
      }

      if (dir == Left && left.is_min_digit_size()) {
        left.push(Right, right.pop(Left));
        return left.pop(Left);
      }

      if (dir == Right && right.is_min_digit_size()) {
        right.push(Left, left.pop(Right));
        return right.pop(Right);
      }
    }

    if (dir == Left && !left.is_min_digit_size()) {
      return left.pop(Left);
    }

    if (dir == Right && !right.is_min_digit_size()) {
      return right.pop(Right);
    }

    // NOTE: middle tree is not empty
    std::optional<node::Node<K, V>> node;
    node::Node<K, V> underflow = *middle.pop_impl(dir);

    switch (dir) {
      case Left:
        // NOTE: a middle tree cannot contain leaves
        left.put_packed(Right, *underflow.as_deep());
        node = left.pop(Left);
        break;
      case Right:
        // NOTE: a middle tree cannot contain leaves
        right.put_packed(Left, *underflow.as_deep());
        node = right.pop(Right);
        break;
    }

    this->_repr = std::make_shared<Repr>(Deep<K, V>(
      std::move(left),
      std::move(middle),
      std::move(right)
    ));

    return node;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::take_impl(
    Direction dir,
    uint count
  ) -> std::vector<node::Node<K, V>> {
    uint min = std::min(count, this->size());

    std::vector<node::Node<K, V>> nodes;
    nodes.reserve(min );

    for (uint i = 0; i < min; i++) {
      nodes.push_back(this->pop_impl(dir));
    }

    switch (dir) {
      case Left:
        break;
      case Right:
        std::reverse(nodes.begin(), nodes.end());
        break;
    }

    return nodes;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::remove_impl(const K& key) -> std::optional<node::Node<K, V>> {
    auto [left, node, right] = this->split_impl(key);
    *this = FingerTree<K, V>::concat(left, right);
    return node;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::split_impl(const K& key) -> std::tuple<
    FingerTree<K, V>,
    std::optional<node::Node<K, V>>,
    FingerTree<K, V>
  > {
    if (this->is_empty()) {
      return std::tuple(
        FingerTree(),
        std::optional<node::Node<K, V>>(),
        FingerTree()
      );
    }

    if (const auto* single = this->as_single()) {
      if (single->node().key() >= key) {
        return std::tuple(
          FingerTree(),
          std::optional(single->node()),
          FingerTree()
        );
      }

      return std::tuple(
        *this,
        std::optional<node::Node<K, V>>(),
        FingerTree()
      );
    }

    const auto* deep = this->as_deep();
    FingerTree<K, V> middle = deep->middle();

    if (deep->left().key() >= key) {
      auto [left, node, right] = deep->left().split(key);
      digit::Digits<K, V> outer_right = deep->right();

      return std::make_tuple(
        FingerTree<K, V>::from_impl(left.digits()),
        node,
        FingerTree<K, V>::deep_smart(
          std::move(right),
          std::move(middle),
          std::move(outer_right)
        )
      );
    }

    auto middle_single = middle.as_single();
    auto middle_deep = middle.as_deep();

    bool is_middle = middle_single
      ? middle_single->key() >= key
      : middle_deep
        ? middle_deep->key() >= key
        : false;

    if (is_middle) {
      auto [middle_left, packed_node, middle_right] = middle.split_impl(key);
      // NOTE: middle cannot contain leaves and is not empty
      auto [left, node, right] = digit::Digits<K, V>::from(
        packed_node->as_deep()->children()
      ).split(key);

      digit::Digits<K, V> outer_left = deep->left();
      digit::Digits<K, V> outer_right = deep->right();

      return std::make_tuple(
        FingerTree<K, V>::deep_smart(
          std::move(outer_left),
          std::move(middle_left),
          std::move(left)
        ),
        node,
        FingerTree<K, V>::deep_smart(
          std::move(right),
          std::move(middle_right),
          std::move(outer_right)
        )
      );
    }

    auto [left, node, right] = deep->right().split(key);
    digit::Digits<K, V> outer_left = deep->left();
    return std::make_tuple(
      FingerTree<K, V>::deep_smart(
        std::move(outer_left),
        std::move(middle),
        std::move(left)
      ),
      node,
      FingerTree<K, V>::from_impl(right.digits())
    );
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::concat_impl(
    const FingerTree<K, V>& left,
    std::span<const node::Node<K, V>> middle,
    const FingerTree<K, V>& right
  ) -> FingerTree<K, V> {

    if (left.is_empty()) {
      FingerTree<K, V> copy = right;
      copy.append_impl(Left, middle);
      return copy;
    } else if (right.is_empty()) {
      FingerTree<K, V> copy = left;
      copy.append_impl(Right, middle);
      return copy;
    } else if (const auto* left_single = left.as_single()) {
      FingerTree<K, V> copy = right;
      copy.append_impl(Left, middle);
      copy.push_impl(Left, left_single->node());
      return copy;
    } else if (const auto* right_single = right.as_single()) {
      FingerTree<K, V> copy = left;
      copy.append_impl(Right, middle);
      copy.push_impl(Right, right_single->node());
      return copy;
    } else {
      const Deep<K, V>* left_deep = left.as_deep();
      const Deep<K, V>* right_deep = right.as_deep();

      // TODO: this vector can be used as in- and output by packing nodes in place
      std::vector<node::Node<K, V>> concat;
      concat.reserve(
        left_deep->right().digit_size()
        + middle.size()
        + right_deep->left().digit_size()
      );

      for (const auto& node : left_deep->right().digits()) {
        concat.push_back(node);
      }

      for (const auto& node : middle) {
        concat.push_back(node);
      }

      for (const auto& node : right_deep->left().digits()) {
        concat.push_back(node);
      }

      std::vector<node::Node<K, V>> packed = node::Node<K, V>::pack_nodes(
        std::span(concat)
      );

      digit::Digits<K, V> outer_left = left_deep->left();
      digit::Digits<K, V> outer_right = right_deep->right();

      return FingerTree(Deep<K, V>(
        std::move(outer_left),
        FingerTree<K, V>::concat_impl(
          left_deep->middle(),
          std::span(packed),
          right_deep->middle()
        ),
        std::move(outer_right)
      ));
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::get(const K& key) -> std::optional<V> {
    std::optional<node::Node<K, V>> node = this->get_impl(key);
    std::optional<std::pair<K, V>> unpacked;

    if (node) {
      auto leaf = node->as_leaf();
      unpacked = std::optional(leaf->val());
    }

    return unpacked;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::push(Direction dir, const K& key, const V& val) -> void {
    // TODO: validate key invariants
    this->push_impl(dir, node::Node<K, V>(key, val));
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::append(
    Direction dir,
    std::span<const std::pair<K, V>> pairs
  ) -> void {
    switch (dir) {
      case Left:
        for (auto it = pairs.rbegin(); it != pairs.rend(); it++) {
          this->push(dir, it->first, it->second);
        }
        break;
      case Right:
        for (auto it = pairs.begin(); it != pairs.end(); it++) {
          this->push(dir, it->first, it->second);
        }
        break;
    }
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::insert(const K& key, const V& val) -> std::optional<V> {
    auto [left, found, right] = this->split(key);
    left.push(Right, key, val);

    *this = FingerTree<K, V>::concat(left, right);
    return found;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::pop(Direction dir) -> std::optional<std::pair<K, V>> {
    std::optional<node::Node<K, V>> node = this->pop_impl(dir);
    std::optional<std::pair<K, V>> unpacked;

    if (node) {
      // NOTE: we know that this top level impl is not a recursive call to
      // pop_impl and therefore returns a leaf node
      auto leaf = node->as_leaf();
      unpacked = std::optional(std::make_pair(leaf->key(), leaf->val()));
    }

    return unpacked;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::take(
    Direction dir,
    uint count
  ) -> std::vector<std::pair<K, V>> {
    uint min = std::min(count, this->size());

    std::vector<std::pair<K, V>> pairs;
    pairs.reserve(min);

    for (uint i = 0; i < min; i++) {
      pairs.push_back(this->pop(dir));
    }

    switch (dir) {
      case Left:
        break;
      case Right:
        std::reverse(pairs.begin(), pairs.end());
        break;
    }

    return pairs;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::remove(const K& key) -> std::optional<V> {
    auto [left, val, right] = this->split(key);
    *this = FingerTree<K, V>::concat(left, right);
    return val;
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::split(
    const K& key
  ) -> std::tuple<FingerTree<K, V>, std::optional<V>, FingerTree<K, V>> {
    auto [left, node, right] = this->split_impl(key);
    std::optional<V> unpacked;

    if (node) {
      // NOTE: we know that this top level impl is not a recursive call to
      // split_impl and therefore returns a leaf node
      auto leaf = node->as_leaf();
      // NOTE: the inner impl of split returns the first node which is larger
      // than or equal to our key as it needs this for the recursive definition
      // we don't need that, we need to only return the actual valid key, but
      // the fully split left and right trees, if it's ont the exact node we
      // want to push it back
      if (leaf->key() == key) {
        unpacked = std::optional(leaf->val());
      } else {
        right.push_impl(Left, *node);
      }
    }

    return std::make_tuple(left, unpacked, right);
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::concat(
    const FingerTree<K, V>& left,
    const FingerTree<K, V>& right
  ) -> FingerTree<K, V> {
    return FingerTree<K, V>::concat_impl(
      left,
      std::span<const node::Node<K, V>>(),
      right
    );
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::show(uint indent) const -> void {
    auto istr = std::string(indent * 2, ' ');
    auto istr2 = std::string((indent + 1) * 2, ' ');

    auto ref_count = this->_repr.use_count();

    if (this->is_empty()) {
      std::cout << istr << ref_count << " Empty" << std::endl;
      return;
    }

    if (const auto* single = this->as_single()) {
      std::cout << istr << ref_count << " Single" << std::endl;
      single->node().show(indent + 1);
      return;
    }

    const auto* deep = this->as_deep();

    std::cout << istr << ref_count << " Deep" << std::endl;
    std::cout << istr2 << "Left [" << std::endl;
    for (const auto& node : deep->left().digits()) {
      node.show(indent + 2);
    }
    std::cout << istr2 << "]" << std::endl;

    deep->middle().show(indent + 1);

    std::cout << istr2 << "Right [" << std::endl;
    for (const auto& node : deep->right().digits()) {
      node.show(indent + 2);
    }
    std::cout << istr2 << "]" << std::endl;
  }
}
