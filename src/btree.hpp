#pragma once

#include "btree/core.hpp"
#include "btree/node/base.hpp"
#include "btree/node/deep.hpp"
#include "btree/node/leaf.hpp"

namespace btree {
  template<typename K, typename V, uint N = node::ORDER_DEFAULT>
  class BTree {
    public:
      BTree(node::SharedNode<K, V, N>&& root);
      BTree(node::Deep<K, V, N>&& root);
      BTree(node::Leaf<K, V, N>&& root);
      BTree();

    public:
      auto root() -> node::Node<K, V, N>& { return *this->_root; }
      auto root() const -> const node::Node<K, V, N>& { return *this->_root; }

    public:
      auto insert(const K& key, const V& val) -> BTree<K, V, N>;

    private:
      node::SharedNode<K, V, N> _root;
  };

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree(node::SharedNode<K, V, N>&& root) : _root(root) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree(
    node::Leaf<K, V, N>&& root
  ) : BTree(std::static_pointer_cast<node::Node<K, V, N>>(
    std::make_shared<node::Leaf<K, V, N>>(std::move(root))
  )) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree(
    node::Deep<K, V, N>&& root
  ) : BTree(std::static_pointer_cast<node::Node<K, V, N>>(
    std::make_shared<node::Deep<K, V, N>>(std::move(root))
  )) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree() : BTree(std::static_pointer_cast<node::Node<K, V, N>>(
    std::make_shared<node::Leaf<K, V, N>>()
  )) {}

  template<typename K, typename V, uint N>
  auto BTree<K, V, N>::insert(
    const K& key,
    const V& val
  ) -> BTree<K, V, N> {
    auto res = this->_root->insert(key, val);

    return std::visit([](auto&& res){
      using T = std::decay_t<decltype(res)>;
      if constexpr (std::is_same_v<T, node::Inserted<K, V, N>>) {
        return BTree(std::move(res));
      } else if constexpr (std::is_same_v<T, node::Split<K, V, N>>) {
        auto& left = res.first;
        auto& right = res.second;
        auto key = left->keys().back();
        return BTree(node::Deep<K, V, N>({key}, {std::move(left), std::move(right)}));
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, res);
  }
}
