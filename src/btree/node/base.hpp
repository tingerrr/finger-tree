#pragma once

#include "src/btree/node/core.hpp"

#include <sys/types.h>

#include <algorithm>
#include <iterator>
#include <span>
#include <utility>
#include <vector>

// TODO: root node can have less than MIN children/less than MIN - 1 key-values
// must either be a separate type or be accounted for in the derived types by
// not checking for MIN

namespace btree {
  namespace node {
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
        Node();

      public:
        auto is_min() const -> bool { return this->keys()->size() == KV_MIN; }
        auto is_max() const -> bool { return this->keys()->size() == KV_MAX; }

        virtual auto is_leaf() const -> bool = 0;
        auto is_deep() const -> bool { return !this->is_leaf(); }

        auto keys() -> std::span<K> {
          return std::span(this->_keys);
        }
        auto keys() const -> std::span<const K> {
          return std::span(this->_keys);
        }

        auto index(const K& key) const -> uint;

        virtual auto insert(const K& key, const V& val) -> InsertResult<K, V, N> = 0;

      protected:
        std::vector<K> _keys;
    };

    template<typename K, typename V, uint N>
    Node<K, V, N>::Node(std::vector<K>&& keys): _keys(std::move(keys)) {
      this->_keys.reserve(Node<K, V, N>::KV_MAX + 1);
    }

    template<typename K, typename V, uint N>
    Node<K, V, N>::Node() : Node({}) {}

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
}
