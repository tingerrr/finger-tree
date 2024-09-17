#pragma once

#include "src/ftree/core.hpp"
#include "src/ftree/deep.hpp"
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
          Repr(const Single<K, V>& repr);
          Repr(const Deep<K, V>& repr);

        public:
          Variant _repr;
      };

    public:
      FingerTree();

      static auto from(std::span<const std::pair<K, V>> nodes) -> FingerTree<K, V>;

    private:
      FingerTree(const Single<K, V>& repr);
      FingerTree(const Deep<K, V>& repr);

      static auto from_impl(
        std::span<const node::Node<K, V>> nodes
      ) -> FingerTree<K, V>;

      static auto deep_smart(
        std::span<const node::Node<K, V>> left,
        const FingerTree<K, V>& middle,
        std::span<const node::Node<K, V>> right
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
  FingerTree<K, V>::Repr::Repr(const Deep<K, V>& repr) : _repr(Repr::Variant(repr)) {}

  template<typename K, typename V>
  FingerTree<K, V>::Repr::Repr(const Single<K, V>& repr) : _repr(Repr::Variant(repr)) {}

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
  FingerTree<K, V>::FingerTree(const Single<K, V>& repr)
    : _repr(std::make_shared<Repr>(repr)) {}

  template<typename K, typename V>
  FingerTree<K, V>::FingerTree(const Deep<K, V>& repr)
    : _repr(std::make_shared<Repr>(repr)) {}

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
    std::span<const node::Node<K, V>> left,
    const FingerTree<K, V>& middle,
    std::span<const node::Node<K, V>> right
  ) -> FingerTree<K, V> {
    std::vector<node::Node<K, V>> left_copy(left.begin(), left.end());
    std::vector<node::Node<K, V>> right_copy(right.begin(), right.end());
    FingerTree<K, V> middle_copy = middle;

    if (left_copy.size() == 0) {
      if (middle_copy.is_empty()) {
        return FingerTree<K, V>::from_impl(right_copy);
      }

      // NOTE: middle cannot contain leaves and is not empty
      auto underflow = *middle_copy.pop_impl(Left);
      for (const auto& unpacked : underflow.as_deep()->children()) {
        left_copy.push_back(unpacked);
      }
    } else if (right_copy.size() == 0) {
      if (middle_copy.is_empty()) {
        return FingerTree<K, V>::from_impl(left_copy);
      }

      // NOTE: middle cannot contain leaves and is not empty
      auto underflow = *middle_copy.pop_impl(Right);
      for (const auto& unpacked : underflow.as_deep()->children()) {
        right_copy.push_back(unpacked);
      }
    }

    return FingerTree(Deep(std::move(left_copy), middle_copy, std::move(right_copy)));
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
      std::vector<node::Node<K, V>> left;
      std::vector<node::Node<K, V>> right;

      left.reserve(5);
      right.reserve(5);

      switch (dir) {
        case Left:
          left.push_back(node);
          right.push_back(other);
          break;
        case Right:
          left.push_back(other);
          right.push_back(node);
          break;
      }

      this->_repr = std::make_shared<Repr>(Deep<K, V>(
        std::move(left),
        std::move(right)
      ));
      return;
    }

    const auto* deep = this->as_deep();

    std::vector<node::Node<K, V>> left(deep->left().begin(), deep->left().end());
    std::vector<node::Node<K, V>> right(deep->right().begin(), deep->right().end());
    left.reserve(5);
    right.reserve(5);

    FingerTree<K, V> middle = deep->middle();

    switch (dir) {
      case Left:
        left.insert(left.begin(), node);

        if (left.size() > 4) {
          middle.push_impl(
            dir,
            node::Node<K, V>(left[2], left[3], left[4])
          );
          left = { left[0], left[1] };
        }
        break;
      case Right:
        right.push_back(node);

        if (right.size() > 4) {
          middle.push_impl(
            dir,
            node::Node<K, V>(right[0], right[1], right[2])
          );
          right = { right[3], right[4] };
        }
        break;
    }

    this->_repr = std::make_shared<Repr>(Deep<K, V>(
      std::move(left),
      middle,
      std::move(right))
    );
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

    std::vector<node::Node<K, V>> left(deep->left().begin(), deep->left().end());
    std::vector<node::Node<K, V>> right(deep->right().begin(), deep->right().end());
    FingerTree<K, V> middle = deep->middle();

    std::optional<node::Node<K, V>> node;

    switch (dir) {
      case Left:
        node = std::optional(left.front());
        left.erase(left.begin());
        if (left.size() > 0) {
          this->_repr = std::make_shared<Repr>(Deep(
            std::move(left),
            middle,
            std::move(right))
          );
          return node;
        }
        break;
      case Right:
        node = std::optional(right.back());
        right.pop_back();
        if (right.size() > 0) {
          this->_repr = std::make_shared<Repr>(Deep(
            std::move(left),
            middle,
            std::move(right))
          );
          return node;
        }
        break;
    }

    // NOTE: left/right is empty after taking out a node, we need to either
    // take from the other side or the middle
    if (middle.is_empty()) {
      switch (dir) {
        case Left:
          if (right.size() == 1) {
            this->_repr = std::make_shared<Repr>(Single(right.front()));
          } else {
            for (
              auto it = right.begin(); it != right.begin() + right.size() / 2; it++
            ) {
              left.push_back(*it);
            }
            right.erase(right.begin(), right.begin() + right.size() / 2);
            this->_repr = std::make_shared<Repr>(Deep(
              std::move(left),
              middle,
              std::move(right))
            );
          }
          break;
        case Right:
          if (left.size() == 1) {
            this->_repr = std::make_shared<Repr>(Single(left.back()));
          } else {
            for (
              auto it = --left.end(); it != left.begin() + left.size() / 2 - 1; it--
            ) {
              right.insert(right.begin(), *it);
            }
            left.erase(left.begin() + left.size() / 2, left.end());
            this->_repr = std::make_shared<Repr>(Deep(
              std::move(left),
              middle,
              std::move(right))
            );
          }
          break;
      }
    } else {
      // NOTE: we know this is deep because a middle tree cannot have leaf
      // nodes
      node::Node<K, V> underflow = *middle.pop_impl(dir);

      switch (dir) {
        case Left:
          for (const auto& child : underflow.as_deep()->children()) {
            left.push_back(child);
          }
          break;
        case Right:
          for (
            auto it = underflow.as_deep()->children().rbegin();
            it != underflow.as_deep()->children().rend();
            it++
          ) {
            right.insert(right.begin(), *it);
          }
          break;
      }

      this->_repr = std::make_shared<Repr>(Deep(
        std::move(left),
        middle,
        std::move(right))
      );
    }
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

    if (deep->left().back().key() >= key) {
      auto [left, node, right] = node::Node<K, V>::digit_split(deep->left(), key);
      return std::make_tuple(
        FingerTree<K, V>::from_impl(left),
        node,
        FingerTree<K, V>::deep_smart(right, deep->middle(), deep->right())
      );
    }

    auto middle_single = deep->middle().as_single();
    auto middle_deep = deep->middle().as_deep();

    bool is_middle = middle_single
      ? middle_single->node().key() >= key
      : middle_deep
        ? middle_deep->right().back().key() >= key
        : false;

    if (is_middle) {
      auto middle = deep->middle();
      auto [middle_left, packed_node, middle_right] = middle.split_impl(key);
      // NOTE: middle cannot contain leaves and is not empty
      auto [left, node, right] = node::Node<K, V>::digit_split(
        packed_node->as_deep()->children(),
        key
      );

      return std::make_tuple(
        FingerTree<K, V>::deep_smart(deep->left(), middle_left, left),
        node,
        FingerTree<K, V>::deep_smart(right, middle_right, deep->right())
      );
    }

    auto [left, node, right] = node::Node<K, V>::digit_split(deep->right(), key);
    return std::make_tuple(
      FingerTree<K, V>::deep_smart(deep->left(), deep->middle(), left),
      node,
      FingerTree<K, V>::from_impl(right)
    );
  }

  template<typename K, typename V>
  auto FingerTree<K, V>::concat_impl(
    const FingerTree<K, V>& left,
    std::span<const node::Node<K, V>> middle,
    const FingerTree<K, V>& right
  ) -> FingerTree<K, V> {
    FingerTree<K, V> left_copy = left;
    FingerTree<K, V> right_copy = right;

    if (left_copy.is_empty()) {
      right_copy.append_impl(Left, middle);
      return right_copy;
    } else if (right_copy.is_empty()) {
      left_copy.append_impl(Right, middle);
      return left_copy;
    } else if (const auto* left_single = left_copy.as_single()) {
      right_copy.append_impl(Left, middle);
      right_copy.push_impl(Left, left_single->node());
      return right_copy;
    } else if (const auto* right_single = right_copy.as_single()) {
      left_copy.append_impl(Right, middle);
      left_copy.push_impl(Right, right_single->node());
      return left_copy;
    } else {
      const Deep<K, V>* left_deep = left_copy.as_deep();
      const Deep<K, V>* right_deep = right_copy.as_deep();

      // TODO: this vector can be used as in- and output by packing nodes in place
      std::vector<node::Node<K, V>> concat;
      concat.reserve(
        left_deep->right().size() + middle.size() + right_deep->left().size()
      );

      for (const auto& node : left_deep->right()) {
        concat.push_back(node);
      }

      for (const auto& node : middle) {
        concat.push_back(node);
      }

      for (const auto& node : right_deep->left()) {
        concat.push_back(node);
      }

      std::vector<node::Node<K, V>> packed = node::Node<K, V>::pack_nodes(
        std::span(concat)
      );

      return FingerTree(Deep<K, V>(
        std::move(std::vector(left_deep->left().begin(), left_deep->left().end())),
        FingerTree<K, V>::concat_impl(
          left_deep->middle(),
          std::span(packed),
          right_deep->middle()
        ),
        std::move(std::vector(right_deep->right().begin(), right_deep->right().end()))
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
    }

    const auto* deep = this->as_deep();

    std::cout << istr << ref_count << " Deep" << std::endl;
    std::cout << istr2 << "Left [" << std::endl;
    for (const auto& node : deep->left()) {
      node.show(indent + 2);
    }
    std::cout << istr2 << "]" << std::endl;

    deep->middle().show(indent + 1);

    std::cout << istr2 << "Right [" << std::endl;
    for (const auto& node : deep->right()) {
      node.show(indent + 2);
    }
    std::cout << istr2 << "]" << std::endl;
  }
}
