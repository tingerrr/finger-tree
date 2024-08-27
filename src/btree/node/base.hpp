#pragma once

#include "src/btree/node/core.hpp"

#include <sys/types.h>

#include <algorithm>
#include <iterator>
#include <span>
#include <utility>
#include <vector>

namespace btree::node {
  template<typename K, typename V, uint N = ORDER_DEFAULT>
  class Node {
    public:
      static_assert(2 < N, "N must be greater than 2");

      static constexpr uint ORDER = N;

      static constexpr uint CHILD_MAX = ORDER;
      static constexpr uint CHILD_MIN = HALF_CEIL(ORDER);

      static constexpr uint KV_MAX = CHILD_MAX - 1;
      static constexpr uint KV_MIN = CHILD_MIN - 1;

    public:
      using KeyType = K;
      using ValueType = V;

    protected:
      Node(std::vector<K>&& keys);

    public:
      auto is_min() const -> bool {
        return this->is_leaf()
          ? this->_keys.size() == KV_MIN
          : this->_keys.size() == CHILD_MIN;
      }
      auto is_max() const -> bool {
        return this->is_leaf()
          ? this->_keys.size() == KV_MAX
          : this->_keys.size() == CHILD_MAX;
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
  };

  template<typename K, typename V, uint N>
  Node<K, V, N>::Node(std::vector<K>&& keys) : _keys(std::move(keys)) {}

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
