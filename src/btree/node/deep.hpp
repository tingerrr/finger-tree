#pragma once

#include "src/btree/node/core.hpp"
#include "src/btree/node/base.hpp"

#include <span>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace btree::node {
  template<typename K, typename V, uint N = ORDER_DEFAULT>
  class Deep : public Node<K, V, N> {
    public:
      using BaseType = Node<K, V, N>;

    public:
      Deep() = delete;

    private:
      Deep(
        std::vector<K>&& keys,
        std::vector<SharedNode<K, V, N>>&& children
      );

    public:
      static auto from_children(
        std::vector<SharedNode<K, V, N>>&& children
      ) -> Deep<K, V, N>;
      static auto from_children(
        const std::vector<SharedNode<K, V, N>>& children
      ) -> Deep<K, V, N>;

    public:
      virtual auto is_leaf() const -> bool override { return false; }

      auto children() const -> std::span<const SharedNode<K, V, N>> {
        return std::span(this->_children);
      }

    public:
      virtual auto insert(
        const K& key,
        const V& val
      ) const -> InsertResult<K, V, N> override;

       virtual auto get(const K& key) const -> const V& override;

    protected:
      std::vector<SharedNode<K, V, N>> _children;
  };

  template<typename K, typename V, uint N>
  Deep<K, V, N>::Deep(
    std::vector<K>&& keys,
    std::vector<SharedNode<K, V, N>>&& children
  ) : Node<K, V, N>(std::move(keys)), _children(std::move(children)) {
    this->_keys.reserve(Node<K, V, N>::CHILD_MAX + 1);
    this->_children.reserve(Node<K, V, N>::CHILD_MAX + 1);
  }

  template<typename K, typename V, uint N>
  auto Deep<K, V, N>::from_children(
    std::vector<SharedNode<K, V, N>>&& children
  ) -> Deep<K, V, N> {
    std::vector<K> keys;

    for (auto& child : children) {
      keys.push_back(child->measure());
    }

    return Deep(std::move(keys), std::move(children));
  }

  template<typename K, typename V, uint N>
  auto Deep<K, V, N>::from_children(
    const std::vector<SharedNode<K, V, N>>& children
  ) -> Deep<K, V, N> {
    std::vector<K> keys;
    std::vector<SharedNode<K, V, N>> children_copy;

    for (auto& child : children) {
      keys.push_back(child->measure());
      children_copy.push_back(child);
    }

    return Deep(
      std::move(keys),
      std::move(children_copy)
    );
  }

  template<typename K, typename V, uint N>
  auto Deep<K, V, N>::insert(
    const K& key,
    const V& val
  ) const -> InsertResult<K, V, N> {
    std::vector<K> k(this->_keys.begin(), this->_keys.end());
    std::vector<SharedNode<K, V, N>> c(
      this->children().begin(),
      this->children().end()
    );

    auto idx  = this->index(key);
    auto& child = c[idx];

    return std::visit([&c, &k, idx, this](auto&& res){
      using T = std::decay_t<decltype(res)>;
      if constexpr (std::is_same_v<T, Inserted<K, V, N>>) {
        k[idx] = res->measure();
        c[idx] = std::move(res);
        return make_result(Deep(std::move(k), std::move(c)));
      } else if constexpr (std::is_same_v<T, Split<K, V, N>>) {
        auto [left, right] = res;

        k[idx] = right->measure();
        k.insert(k.begin() + idx, left->measure());
        c[idx] = std::move(right);
        c.insert(c.begin() + idx, std::move(left));

        if (c.size() == Node<K, V, N>::CHILD_MAX + 1) {
          auto [kl, kr] = split_vector(std::move(k));
          auto [cl, cr] = split_vector(std::move(c));

          return make_result(
            Deep(std::move(kl), std::move(cl)),
            Deep(std::move(kr), std::move(cr))
          );
        } else {
          return make_result(Deep(std::move(k), std::move(c)));
        }

      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, child->insert(key, val));
  }

  template<typename K, typename V, uint N>
  auto Deep<K, V, N>::get(const K& key) const -> const V& {
    auto idx  = this->index(key);
    return this->_children[idx]->get(key);
  }
}
