#pragma once

#include "btree/core.hpp"
#include "btree/node/base.hpp"
#include "btree/node/deep.hpp"
#include "btree/node/leaf.hpp"
#include "btree/node/core.hpp"

namespace btree {
  template<typename K, typename V, uint N = ORDER_DEFAULT>
  class BTree {
    public:
      BTree(node::SharedNode<K, V, N>&& root);
      BTree(node::Deep<K, V, N>&& root);
      BTree(node::Leaf<K, V, N>&& root);
      BTree();

    public:
      auto root() const -> const node::Node<K, V, N>& { return *this->_root; }

    public:
      auto insert(const K& key, const V& val) const -> BTree<K, V, N>;
      auto get(const K& key) const -> const V&;
      auto show() const -> void;

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
    std::make_shared<node::Leaf<K, V, N>>(node::Leaf<K, V, N>::empty_root())
  )) {}

  template<typename K, typename V, uint N>
  auto BTree<K, V, N>::insert(
    const K& key,
    const V& val
  ) const -> BTree<K, V, N> {
    auto res = this->_root->insert(key, val);

    return std::visit([](auto&& res){
      using T = std::decay_t<decltype(res)>;
      if constexpr (std::is_same_v<T, node::Inserted<K, V, N>>) {
        return BTree(std::move(res));
      } else if constexpr (std::is_same_v<T, node::Split<K, V, N>>) {
        auto [left, right] = res;
        return BTree(node::Deep<K, V, N>::from_children({
          std::move(left),
          std::move(right)
        }));
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, res);
  }

  template<typename K, typename V, uint N>
  auto BTree<K, V, N>::get(const K& key) const -> const V& {
    return this->_root->get(key);
  }

  template<typename K, typename V, uint N>
  auto BTree<K, V, N>::show() const -> void {
    node::show(*this->_root, 0);
  }
}
