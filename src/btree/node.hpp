#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <span>
#include <variant>
#include <vector>

#define HALF_CEIL(N) ((N) % 2 == 0) ? ((N) / 2) : ((N) / 2 + 1)

// TODO: root node can have less than MIN children/less than MIN - 1 key-values
// must either be a separate type or be accounted for in the derived types by
// not checking for MIN

namespace btree {
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
  auto result_inserted(N&& node) -> InsertResult<
    typename N::KeyType, typename N::ValueType, N::ORDER
  > {
    return std::static_pointer_cast<Node<
      typename N::KeyType, typename N::ValueType, N::ORDER
    >>(std::make_shared<N>(node));
  }

  template<typename N>
  auto result_split(
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

  template<typename T>
  auto split_vector(std::vector<T>&& vec) -> std::pair<std::vector<T>, std::vector<T>> {
    std::vector<T> other(
      std::make_move_iterator(vec.begin() + vec.size() / 2),
      std::make_move_iterator(vec.end())
    );
    vec.erase(vec.begin() + vec.size() / 2, vec.end());

    return std::make_pair(vec, other);
  }

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
      auto size() const -> int { return this->_keys.size(); }
      auto is_min() const -> bool { return this->size() == KV_MIN; }
      auto is_max() const -> bool { return this->size() == KV_MAX; }

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

  template<typename K, typename V, uint N = ORDER_DEFAULT>
  class BTree {
    public:
      BTree(SharedNode<K, V, N>&& root);
      BTree(Deep<K, V, N>&& root);
      BTree(Leaf<K, V, N>&& root);
      BTree();

    public:
      auto root() -> Node<K, V, N>& { return *this->_root; }
      auto root() const -> const Node<K, V, N>& { return *this->_root; }

    public:
      auto insert(const K& key, const V& val) -> BTree<K, V, N>;

    private:
      SharedNode<K, V, N> _root;
  };

  template<typename K, typename V, uint N>
  Node<K, V, N>::Node(std::vector<K>&& keys): _keys(std::move(keys)) {
    this->_keys.reserve(Node<K, V, N>::KV_MAX + 1);
  }

  template<typename K, typename V, uint N>
  Node<K, V, N>::Node() : Node({}) {}

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
  Leaf<K, V, N>::Leaf(
    std::vector<K>&& keys,
    std::vector<V>&& vals
  ) : Node<K, V, N>(std::move(keys)), _vals(std::move(vals)) {
    this->_vals.reserve(Node<K, V, N>::KV_MAX + 1);
  }

  template<typename K, typename V, uint N>
  Leaf<K, V, N>::Leaf() : Leaf({}, {}) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree(SharedNode<K, V, N>&& root) : _root(root) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree(
    Leaf<K, V, N>&& root
  ) : BTree(std::static_pointer_cast<Node<K, V, N>>(
    std::make_shared<Leaf<K, V, N>>(std::move(root))
  )) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree(
    Deep<K, V, N>&& root
  ) : BTree(std::static_pointer_cast<Node<K, V, N>>(
    std::make_shared<Deep<K, V, N>>(std::move(root))
  )) {}

  template<typename K, typename V, uint N>
  BTree<K, V, N>::BTree() : BTree(std::static_pointer_cast<Node<K, V, N>>(
    std::make_shared<Leaf<K, V, N>>()
  )) {}

  template<typename K, typename V, uint N>
  auto Node<K, V, N>::index(const K& key) const -> uint {
    // TODO: linear scan for small factors

    auto begin = this->_keys.begin();
    auto end = this->_keys.end();

    auto range = std::equal_range(begin, end, key);
    auto idx = std::distance(begin, range.first);

    return idx;
  }

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
        c[idx] = res;
        return result_inserted(Deep(std::move(k), std::move(c)));
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

          return result_split(
            Deep(std::move(kl), std::move(cl)),
            Deep(std::move(kr), std::move(cr))
          );
        } else {
          return result_inserted(Deep(std::move(k), std::move(c)));
        }

      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, child->insert(key, val));
  }

  template<typename K, typename V, uint N>
  auto Leaf<K, V, N>::insert(
    const K& key,
    const V& val
  ) -> InsertResult<K, V, N> {
    std::vector<K> k(this->_keys.begin(), this->_keys.end());
    std::vector<V> v(this->_vals.begin(), this->_vals.end());

    auto idx = this->index(key);

    if (idx < k.size() && k[idx] == key) {
      k[idx] = key;
      v[idx] = val;
    } else {
      k.insert(k.begin() + idx, key);
      v.insert(v.begin() + idx, val);

      if (k.size() == Node<K, V, N>::KV_MAX + 1) {
        auto [kl, kr] = split_vector(std::move(k));
        auto [vl, vr] = split_vector(std::move(v));

        return result_split(
          Leaf(std::move(kl), std::move(vl)),
          Leaf(std::move(kr), std::move(vr))
        );
      }
    }

    return result_inserted(Leaf(std::move(k), std::move(v)));
  }

  template<typename K, typename V, uint N>
  auto BTree<K, V, N>::insert(
    const K& key,
    const V& val
  ) -> BTree<K, V, N> {
    auto res = this->_root->insert(key, val);

    return std::visit([](auto&& res){
      using T = std::decay_t<decltype(res)>;
      if constexpr (std::is_same_v<T, Inserted<K, V, N>>) {
        return BTree(std::move(res));
      } else if constexpr (std::is_same_v<T, Split<K, V, N>>) {
        auto& left = res.first;
        auto& right = res.second;
        auto key = left->keys().back();
        return BTree(Deep<K, V, N>({key}, {std::move(left), std::move(right)}));
      } else {
        static_assert(false, "non-exhaustive visitor");
      }
    }, res);
  }
}
