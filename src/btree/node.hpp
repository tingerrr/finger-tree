#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#define HALF_CEIL(N) ((N) % 2 == 0) ? ((N) / 2) : ((N) / 2 + 1)

namespace btree {

constexpr uint MAX_DEFAULT = 32;
constexpr uint MIN_DEFAULT = HALF_CEIL(MAX_DEFAULT);

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
    Deep();

  public:
    virtual auto is_leaf() const -> bool override {
      return false;
    }

    auto children() -> std::span<std::shared_ptr<Node<K, V, MAX, MIN>>> {
      return std::span(this->_children);
    }
    auto children() const -> std::span<const std::shared_ptr<Node<K, V, MAX, MIN>>> {
      return std::span(this->_children);
    }

  private:
    std::vector<std::shared_ptr<Node<K, V, MAX, MIN>>> _children;
};

template<
  typename K,
  typename V,
  uint MAX = MAX_DEFAULT,
  uint MIN = HALF_CEIL(MAX)
>
class Leaf : public Node<K, V, MAX, MIN> {
  public:
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
    auto upsert(const K& key, const V& val) -> std::optional<std::pair<K, V>>;

  private:
    std::vector<V> _vals;
};

template<typename K, typename V, uint MAX, uint MIN>
Node<K, V, MAX, MIN>::Node() {
  this->_keys.reserve(Node<K, V, MAX, MIN>::KV_MAX);
}

template<typename K, typename V, uint MAX, uint MIN>
Deep<K, V, MAX, MIN>::Deep() {
  this->_children.reserve(Node<K, V, MAX, MIN>::CHILD_MAX);
}

template<typename K, typename V, uint MAX, uint MIN>
Leaf<K, V, MAX, MIN>::Leaf() {
  this->_vals.reserve(Node<K, V, MAX, MIN>::KV_MAX);
}

template<typename K, typename V, uint MAX, uint MIN>
auto Leaf<K, V, MAX, MIN>::upsert(const K& key, const V& val) -> std::optional<std::pair<K, V>> {
  auto range = std::equal_range(this->_keys.begin(), this->_keys.end(), key);
  auto idx = std::distance(this->_keys.begin(), range.first);

  auto retval = std::optional<std::pair<K, V>>();
  if (*range.first == key) {
    this->_vals[idx] = val;
  } else {
    if (this->is_max()) {
      auto k = *this->_keys.end();
      auto v = *this->_vals.end();
      this->_keys.pop_back();
      this->_vals.pop_back();

      retval = std::pair<K, V>(k, v);
    }

    this->_keys.insert(range.first, key);
    this->_vals.insert(this->_vals.begin() + idx, val);
  }

  return retval;
}

}
