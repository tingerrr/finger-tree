#pragma once

#include "src/collections/b_tree/node/core.hpp"

#include <sys/types.h>

#include <algorithm>
#include <iterator>
#include <span>
#include <utility>
#include <vector>

namespace collections::b_tree::node {
  template<typename K, typename V, uint N>
  class Node {
    public:
      static_assert(2 < N, "N must be greater than 2");

      static constexpr uint ORDER = N;

      static constexpr uint CHILD_MAX = ORDER;
      static constexpr uint CHILD_MIN = ORDER % 2 == 0
        ? ORDER / 2
        : ORDER / 2 + 1;

      static constexpr uint DEEP_KV_MAX = CHILD_MAX;
      static constexpr uint DEEP_KV_MIN = CHILD_MIN;

      static constexpr uint LEAF_KV_MAX = CHILD_MAX - 1;
      static constexpr uint LEAF_KV_MIN = CHILD_MIN - 1;

    public:
      using KeyType = K;
      using ValueType = V;

    protected:
      Node(std::vector<K>&& keys, uint _size);

    public:
      auto size() const -> uint { return this->_size; }
      auto is_node_min() const -> bool {
        return this->is_leaf()
          ? this->keys().size() == DEEP_KV_MIN
          : this->keys().size() == LEAF_KV_MIN;
      }
      auto is_node_max() const -> bool {
        return this->is_leaf()
          ? this->keys().size() == DEEP_KV_MAX
          : this->keys().size() == LEAF_KV_MAX;
      }

      virtual auto is_leaf() const -> bool = 0;
      auto is_deep() const -> bool { return !this->is_leaf(); }

      auto keys() const -> std::span<const K> {
        return std::span(
            this->_keys.data(),
            this->is_leaf() ? this->_keys.size() : this->_keys.size() - 1
        );
      }

      auto measure() const -> const K& {
        return this->_keys.back();
      }

    public:
      virtual auto insert(
        const K& key,
        const V& val
      ) const -> InsertResult<K, V, N> = 0;

      virtual auto get(const K& key) const -> const V& = 0;

    protected:
      auto index(const K& key) const -> uint;

    protected:
      std::vector<K> _keys;
      uint _size;
  };

  template<typename K, typename V, uint N>
  Node<K, V, N>::Node(
    std::vector<K>&& keys,
    uint size
  ) : _keys(std::move(keys)), _size(size) {}

  template<typename K, typename V, uint N>
  auto Node<K, V, N>::index(const K& key) const -> uint {
    // TODO: linear scan for small factors

    auto begin = this->keys().begin();
    auto end = this->keys().end();

    auto range = std::equal_range(begin, end, key);
    auto idx = std::distance(begin, range.first);

    return idx;
  }
}
