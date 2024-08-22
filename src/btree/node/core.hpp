#pragma once

#include <memory>
#include <utility>
#include <variant>

#define HALF_CEIL(N) ((N) % 2 == 0) ? ((N) / 2) : ((N) / 2 + 1)

namespace btree {
  namespace node {
    constexpr uint ORDER_DEFAULT = 32;

    template<typename K, typename V, uint N>
    class Node;

    template<typename K, typename V, uint N>
    class Deep;

    template<typename K, typename V, uint N>
    class Leaf;

    template<typename K, typename V, uint N>
    using SharedNode = std::shared_ptr<Node<K, V, N>>;

    template<typename K, typename V, uint N>
    using Split = std::pair<SharedNode<K, V, N>, SharedNode<K, V, N>>;

    template<typename K, typename V, uint N>
    using Inserted = SharedNode<K, V, N>;

    template<typename K, typename V, uint N>
    using InsertResult = std::variant<Split<K, V, N>, Inserted<K, V, N>>;

    template<typename N>
    auto make_result(N&& node) -> InsertResult<
      typename N::KeyType, typename N::ValueType, N::ORDER
    > {
      return std::static_pointer_cast<Node<
        typename N::KeyType, typename N::ValueType, N::ORDER
      >>(std::make_shared<N>(node));
    }

    template<typename N>
    auto make_result(
      N&& left,
      N&& right
    ) -> InsertResult<
      typename N::KeyType, typename N::ValueType, N::ORDER
    > {
      return std::make_pair(
        std::static_pointer_cast<Node<
          typename N::KeyType, typename N::ValueType, N::ORDER
        >>(std::make_shared<N>(left)),
        std::static_pointer_cast<Node<
          typename N::KeyType, typename N::ValueType, N::ORDER
        >>(std::make_shared<N>(right))
      );
    }
  }
}
