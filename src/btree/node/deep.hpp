#pragma once

#include "src/btree/core.hpp"
#include "src/btree/node/core.hpp"
#include "src/btree/node/base.hpp"

#include <span>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace btree {
  namespace node {
    template<typename K, typename V, uint N = ORDER_DEFAULT>
    class Deep : public Node<K, V, N> {
      public:
        Deep(
          std::vector<K>&& keys,
          std::vector<SharedNode<K, V, N>>&& children
        );
        Deep();

      public:
        virtual auto is_leaf() const -> bool override { return false; }

        auto children() -> std::span<SharedNode<K, V, N>> {
          return std::span(this->_children);
        }
        auto children() const -> std::span<const SharedNode<K, V, N>> {
          return std::span(this->_children);
        }

        virtual auto insert(const K& key, const V& val) -> InsertResult<K, V, N> override;

      protected:
        std::vector<SharedNode<K, V, N>> _children;
    };

    template<typename K, typename V, uint N>
    Deep<K, V, N>::Deep(
      std::vector<K>&& keys,
      std::vector<SharedNode<K, V, N>>&& children
    ) : Node<K, V, N>(std::move(keys)), _children(std::move(children)) {
      this->_children.reserve(Node<K, V, N>::CHILD_MAX + 1);
    }

    template<typename K, typename V, uint N>
    Deep<K, V, N>::Deep() : Deep({}, {}) {}

    template<typename K, typename V, uint N>
    auto Deep<K, V, N>::insert(
      const K& key,
      const V& val
    ) -> InsertResult<K, V, N> {
      std::vector<K> k(this->_keys.begin(), this->_keys.end());
      std::vector<SharedNode<K, V, N>> c(
        this->_children.begin(),
        this->_children.end()
      );

      auto idx  = this->index(key);
      auto child = c[idx];

      return std::visit([&c, &k, idx, this](auto&& res){
        using T = std::decay_t<decltype(res)>;
        if constexpr (std::is_same_v<T, Inserted<K, V, N>>) {
          k[idx] = res->keys().back();
          c[idx] = std::move(res);
          return make_result(Deep(std::move(k), std::move(c)));
        } else if constexpr (std::is_same_v<T, Split<K, V, N>>) {
          auto& left = res.first;
          auto& right = res.second;

          // BUG: we need to handle cases where this is the last in the buffer
          // and/or node
          k.insert(k.begin() + idx, left->keys().back());
          c.insert(c.begin() + idx, std::move(right));
          c.insert(c.begin() + idx, std::move(left));

          if (k.size() == Node<K, V, N>::KV_MAX + 1) {
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
  }
}
