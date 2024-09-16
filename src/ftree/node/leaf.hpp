#pragma once

#include "src/ftree/node/core.hpp"

#include <sys/types.h>

namespace ftree::node {
  template<typename K, typename V>
  class Leaf {
    public:
      Leaf() = delete;

      Leaf(const K& key, const V& val);

    public:
      auto size() const -> uint { return 1; }

      auto key() const -> const K& { return this->_key; }
      auto val() const -> const V& { return this->_val; }

    private:
      K _key;
      V _val;
  };

  template<typename K, typename V>
  Leaf<K, V>::Leaf(const K& key, const V& val) : _key(key), _val(val) {}
}
