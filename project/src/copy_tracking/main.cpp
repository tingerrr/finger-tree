#pragma once

#include "src/btree.hpp"
#include "src/finger_tree/finger_tree.hpp"

#include <QMap>

#include <iostream>
#include <ostream>

static int COPIES = 0;

struct TrackCopy {
  TrackCopy(int value) : _value(value) {}
  TrackCopy(TrackCopy& other) : _value(other._value) { COPIES += 1; }
  TrackCopy(const TrackCopy& other) : _value(other._value) { COPIES += 1; }

  TrackCopy& operator=(TrackCopy& other) {
    this->_value = other._value;
    COPIES += 1;
    return *this;
  }
  TrackCopy& operator=(const TrackCopy& other) {
    this->_value = other._value;
    COPIES += 1;
    return *this;
  }

  static void reset() {
    COPIES = 0;
  }

  int _value;
};

std::ostream& operator<<(std::ostream &os, const TrackCopy &m) {
  return os << m._value;
}

template<typename T>
auto persistent_insert(uint iterations) -> void {
  auto last = T();

  for (uint i = 0; i < iterations; i++) {
    auto copy = last;
    copy.insert(i, TrackCopy(i));
    last = copy;
  }
}

auto copy_tracking(uint iterations) -> void {
  std::cout << "iter: " << iterations << std::endl;

  persistent_insert<btree::BTree<int, TrackCopy, 32>>(iterations);
  std::cout << "      B-Tree: " << COPIES << std::endl;
  TrackCopy::reset();

  persistent_insert<finger_tree::FingerTree<int, TrackCopy>>(iterations);
  std::cout << "  FingerTree: " << COPIES << std::endl;
  TrackCopy::reset();

  persistent_insert<QMap<int, TrackCopy>>(iterations);
  std::cout << "        QMap: " << COPIES << std::endl;
  TrackCopy::reset();
}

auto main() -> int {
  for (int i = 0; i < 16; i++) {
    copy_tracking(1 << i);
  }

  return 0;
}

