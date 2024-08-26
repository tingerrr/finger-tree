#pragma once

#include "src/btree/node/core.hpp"
#include "src/btree/node/base.hpp"

#include <algorithm>
#include <span>
#include <utility>
#include <vector>

namespace btree::node {
  template<typename K, typename V, uint N = ORDER_DEFAULT>
  class Leaf : public Node<K, V, N> {
    public:
      Leaf(std::vector<K>&& keys, std::vector<V>&& vals);
      Leaf();

    public:
      virtual auto is_leaf() const -> bool override { return true; }

      auto vals() -> std::span<V> {
        return std::span(this->_vals);
      }
      auto vals() const -> std::span<const V> {
        return std::span(this->_vals);
      }

      virtual auto insert(const K& key, const V& val) -> InsertResult<K, V, N> override;

    protected:
      std::vector<V> _vals;
  };

  template<typename K, typename V, uint N>
  Leaf<K, V, N>::Leaf(
    std::vector<K>&& keys,
    std::vector<V>&& vals
  ) : Node<K, V, N>(std::move(keys)), _vals(std::move(vals)) {
    this->_vals.reserve(Node<K, V, N>::KV_MAX + 1);
  }

  template<typename K, typename V, uint N>
  Leaf<K, V, N>::Leaf() : Leaf({}, {}) {}

  template<typename K, typename V, uint N>
  auto Leaf<K, V, N>::insert(
    const K& key,
    const V& val
  ) -> InsertResult<K, V, N> {
    std::vector<K> k(this->keys().begin(), this->keys().end());
    std::vector<V> v(this->vals().begin(), this->vals().end());

    auto idx = this->index(key);

    if (idx < k.size() && k[idx] == key) {
      v[idx] = val;
    } else {
      k.insert(k.begin() + idx, key);
      v.insert(v.begin() + idx, val);

      if (k.size() == Node<K, V, N>::KV_MAX + 1) {
        auto [kl, kr] = split_vector(std::move(k));
        auto [vl, vr] = split_vector(std::move(v));

        return make_result(
          Leaf(std::move(kl), std::move(vl)),
          Leaf(std::move(kr), std::move(vr))
        );
      }
    }

    return make_result(Leaf(std::move(k), std::move(v)));
  }
}
