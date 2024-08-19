#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <span>
#include <tuple>
#include <variant>
#include <vector>

#define HALF_CEIL(N) ((N) % 2 == 0) ? ((N) / 2) : ((N) / 2 + 1)

// TODO: root node can have less than MIN children/less than MIN - 1 key-values
// must either be a separate type or be accounted for in the derived types by
// not checking for MIN

namespace btree {
  constexpr uint MAX_DEFAULT = 32;
  constexpr uint MIN_DEFAULT = HALF_CEIL(MAX_DEFAULT);

  template<typename K, typename V, uint MAX, uint MIN>
  class Node;

  template<typename K, typename V, uint MAX, uint MIN>
  class Deep;

  template<typename K, typename V, uint MAX, uint MIN>
  class Leaf;

  template<typename K, typename V, uint MAX, uint MIN>
  using SharedNode = std::shared_ptr<Node<K, V, MAX, MIN>>;

  template<typename K, typename V, uint MAX, uint MIN>
  using Split = std::tuple<
    SharedNode<K, V, MAX, MIN>,
    K,
    SharedNode<K, V, MAX, MIN>
  >;

  template<typename K, typename V, uint MAX, uint MIN>
  using Inserted = SharedNode<K, V, MAX, MIN>;

  template<typename K, typename V, uint MAX, uint MIN>
  using InsertResult = std::variant<
    Split<K, V, MAX, MIN>,
    Inserted<K, V, MAX, MIN>
  >;

  template<typename K, typename V, uint MAX, uint MIN>
  static auto result_inserted(
    Leaf<K, V, MAX, MIN>&& node
  ) -> InsertResult<K, V, MAX, MIN> {
    return std::static_pointer_cast<Node<K, V, MAX, MIN>>(
      std::make_shared<Leaf<K, V, MAX, MIN>>(node)
    );
  }

  template<typename K, typename V, uint MAX, uint MIN>
  static auto result_split(
    Leaf<K, V, MAX, MIN>&& left,
    K key,
    Leaf<K, V, MAX, MIN>&& right
  ) -> InsertResult<K, V, MAX, MIN> {
    return std::tuple(
      std::static_pointer_cast<Node<K, V, MAX, MIN>>(std::make_shared<Leaf<K, V, MAX, MIN>>(left)),
      key,
      std::static_pointer_cast<Node<K, V, MAX, MIN>>(std::make_shared<Leaf<K, V, MAX, MIN>>(right))
    );
  }


  template<
    typename K,
    typename V,
    uint MAX = MAX_DEFAULT,
    uint MIN = HALF_CEIL(MAX)
  >
  class Node {
    public:
      static_assert(1 < MIN, "MIN must be greater than 1");
      static_assert(2 < MAX, "MAX must be greater than 2");
      static_assert(HALF_CEIL(MAX) <= MIN && MIN < MAX, "MIN must be in [ceil(MAX / 2), MAX)");

      static constexpr uint ORDER = MAX;
      static constexpr uint CHILD_MIN = MIN;
      static constexpr uint CHILD_MAX = MAX;

      static constexpr uint KV_MIN = MIN - 1;
      static constexpr uint KV_MAX = MAX - 1;

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

      virtual auto insert(const K& key, const V& val) -> InsertResult<K, V, MAX, MIN> = 0;
      virtual auto show() -> void = 0;

    protected:
      std::vector<K> _keys;
  };

  template<
    typename K,
    typename V,
    uint MAX = MAX_DEFAULT,
    uint MIN = HALF_CEIL(MAX)
  >
  class Deep : public Node<K, V, MAX, MIN> {
    public:
      Deep(
        std::vector<K>&& keys,
        std::vector<SharedNode<K, V, MAX, MIN>>&& children
      );
      Deep();

    public:
      virtual auto is_leaf() const -> bool override {
        return false;
      }

      auto children() -> std::span<SharedNode<K, V, MAX, MIN>> {
        return std::span(this->_children);
      }
      auto children() const -> std::span<const SharedNode<K, V, MAX, MIN>> {
        return std::span(this->_children);
      }

    public:
      virtual auto insert(const K& key, const V& val) -> InsertResult<K, V, MAX, MIN> override;

      virtual auto show() -> void override;

    private:
      std::vector<SharedNode<K, V, MAX, MIN>> _children;
  };

  template<
    typename K,
    typename V,
    uint MAX = MAX_DEFAULT,
    uint MIN = HALF_CEIL(MAX)
  >
  class Leaf : public Node<K, V, MAX, MIN> {
    public:
      Leaf(std::vector<K>&& keys, std::vector<V>&& vals);
      Leaf();

    public:
      virtual auto is_leaf() const -> bool override {
        return true;
      }

      auto vals() -> std::span<V> {
        return std::span(this->_vals);
      }
      auto vals() const -> std::span<const V> {
        return std::span(this->_vals);
      }

    public:
      virtual auto insert(
        const K& key,
        const V& val
      ) -> InsertResult<K, V, MAX, MIN> override;

      virtual auto show() -> void override;

    private:
      std::vector<V> _vals;
  };

  template<typename K, typename V, uint MAX, uint MIN>
  Node<K, V, MAX, MIN>::Node(std::vector<K>&& keys): _keys(std::move(keys)) {
    this->_keys.reserve(Node<K, V, MAX, MIN>::KV_MAX + 1);
  }

  template<typename K, typename V, uint MAX, uint MIN>
  Node<K, V, MAX, MIN>::Node() : Node({}) {}

  template<typename K, typename V, uint MAX, uint MIN>
  Deep<K, V, MAX, MIN>::Deep(
    std::vector<K>&& keys,
    std::vector<SharedNode<K, V, MAX, MIN>>&& children
  ) : Node<K, V, MAX, MIN>(std::move(keys)), _children(std::move(children)) {
    this->_children.reserve(Node<K, V, MAX, MIN>::CHILD_MAX + 1);
  }

  template<typename K, typename V, uint MAX, uint MIN>
  Deep<K, V, MAX, MIN>::Deep() : Deep({}, {}) {}

  template<typename K, typename V, uint MAX, uint MIN>
  Leaf<K, V, MAX, MIN>::Leaf(
    std::vector<K>&& keys,
    std::vector<V>&& vals
  ) : Node<K, V, MAX, MIN>(std::move(keys)), _vals(std::move(vals)) {
    this->_vals.reserve(Node<K, V, MAX, MIN>::KV_MAX + 1);
  }

  template<typename K, typename V, uint MAX, uint MIN>
  Leaf<K, V, MAX, MIN>::Leaf(): Leaf({}, {}) {}

  template<typename K, typename V, uint MAX, uint MIN>
  auto Deep<K, V, MAX, MIN>::insert(
    const K& key,
    const V& val
  ) -> InsertResult<K, V, MAX, MIN> {
    auto range = std::equal_range(this->_keys.begin(), this->_keys.end(), key);
    auto idx = std::distance(this->_keys.begin(), range.first);
    auto child = this->_children[idx];

    // TODO: rebalance
    return child.insert(key, val);
  }

  template<typename K, typename V, uint MAX, uint MIN>
  auto Leaf<K, V, MAX, MIN>::insert(
    const K& key,
    const V& val
  ) -> InsertResult<K, V, MAX, MIN> {
    std::vector<K> k(this->_keys.begin(), this->_keys.end());
    std::vector<V> v(this->_vals.begin(), this->_vals.end());

    auto range = std::equal_range(k.begin(), k.end(), key);
    auto idx = std::distance(k.begin(), range.first);

    if (range.first != k.end() && *range.first == key) {
      k[idx] = key;
      v[idx] = val;
      auto self = Leaf(std::move(k), std::move(v));
      return result_inserted<K, V, MAX, MIN>(std::move(self));
    } else {
      k.insert(range.first, key);
      v.insert(v.begin() + idx, val);

      if (this->size() == Node<K, V, MAX, MIN>::KV_MAX + 1) {
        std::vector<K> k1(k.begin(), k.begin() + this->_keys.size() / 2);
        std::vector<V> v1(v.begin(), v.begin() + this->_vals.size() / 2);

        std::vector<K> k2(k.begin() + this->_keys.size() / 2, k.end());
        std::vector<V> v2(v.begin() + this->_vals.size() / 2, v.end());

        auto k = k1.back();

        auto left = Leaf(std::move(k1), std::move(v1));
        auto right = Leaf(std::move(k2), std::move(v2));

        return result_split(std::move(left), k, std::move(right));
      } else {
        auto self = Leaf(std::move(k), std::move(v));
        return result_inserted<K, V, MAX, MIN>(std::move(self));
      }
    }
  }

  template<typename K, typename V, uint MAX, uint MIN>
  auto Deep<K, V, MAX, MIN>::show() -> void {
    std::cout << "The Deep" << std::endl;
  }

  template<typename K, typename V, uint MAX, uint MIN>
  auto Leaf<K, V, MAX, MIN>::show() -> void {
    std::cout << "keys: ";
    std::copy(
      this->keys().begin(),
      this->keys().end(),
      std::ostream_iterator<int>(std::cout, ", ")
    );
    std::cout << std::endl;
    std::cout << "vals: ";
    std::copy(
      this->vals().begin(),
      this->vals().end(),
      std::ostream_iterator<int>(std::cout, ", ")
    );
    std::cout << std::endl;
  }
}
