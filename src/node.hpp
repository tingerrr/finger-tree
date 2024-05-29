#pragma once

#include <algorithm>
#include <memory>
#include <span>
#include <vector>

#define HALF_CEIL(N) ((N) % 2 == 0) ? ((N) / 2) : ((N) / 2 + 1)

template<
  typename K,
  typename V,
  int MAX,
  int MIN = HALF_CEIL(MAX)
>
class Node {
  public:
    static_assert(1 < MIN, "MIN must be greater than 1");
    static_assert(2 < MAX, "MAX must be greater than 2");
    static_assert(HALF_CEIL(MAX) <= MIN && MIN < MAX, "MIN must be in [ceil(MAX / 2), MAX)");

    static constexpr int ORDER = MAX;
    static constexpr int CHILD_MIN = MIN;
    static constexpr int CHILD_MAX = MAX;

    static constexpr int KV_MIN = MIN - 1;
    static constexpr int KV_MAX = MAX - 1;

  public:
    using KeyType = K;
    using ValueType = V;

    using ThisType = Node<K, V, MAX, MIN>;

  public:
    auto size() const -> int { return this->_children.size(); };
    auto is_min() const -> bool { return this->size() == MIN; }
    auto is_max() const -> bool { return this->size() == MAX; }

    auto is_leaf() const -> bool { return !_children[0]; }
    auto is_deep() const -> bool { return _children[0]; }

    auto keys() -> std::span<K> {
      return std::span(this->_keys);
    }
    auto keys() const -> std::span<const K> {
      return std::span(this->_keys);
    }

    auto vals() -> std::span<V> {
      return std::span(this->_vals);
    }
    auto vals() const -> std::span<const V> {
      return std::span(this->_vals);
    }

    auto children() -> std::span<std::shared_ptr<ThisType>> {
      return std::span(this->_children);
    }
    auto children() const -> std::span<const std::shared_ptr<ThisType>> {
      return std::span(this->_children);
    }

  public:
    auto insert(K key, V val);

  protected:
    std::vector<K> _keys;
    std::vector<V> _vals;
    std::vector<std::shared_ptr<ThisType>> _children;
};

#define THIS Node<K, V, MAX, MIN>

template<typename K, typename V, int MAX, int MIN>
auto THIS::insert(K key, V val) {
  auto range = std::equal_range(this->_keys.begin(), this->_keys.end(), key);
  auto idx = std::distance(this->_keys.begin(), range.first);

  this->_keys.insert(range.first, key);
  this->_vals.insert(this->_vals.begin() + idx, val);
}
