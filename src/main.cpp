#include "btree.hpp"
#include "ftree.hpp"

#include <iostream>

#include <QMap>

#include <benchmark/benchmark.h>

static void qmap_get(benchmark::State& state) {
  auto map = QMap<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    map.insert(i, i);
  }

  for (auto _ : state) {
    // NOTE: most random values fall above the tree max value and would cause
    // only best-case early exit for querying
    auto v = map.find(std::rand() % map.size());
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(map);
  }

  state.SetComplexityN(state.range(0));
}

static void qmap_insert(benchmark::State& state) {
  auto map = QMap<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    map.insert(i, i);
  }

  for (auto _ : state) {
    auto v = map.insert(std::rand(), 0);
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(map);
  }

  state.SetComplexityN(state.range(0));
}

static void btree_get(benchmark::State& state) {
  auto tree = btree::BTree<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    tree = tree.insert(i, i);
  }

  for (auto _ : state) {
    // NOTE: most random values fall above the tree max value and would cause
    // only best-case early exit for querying
    auto v = tree.get(std::rand() % tree.size());
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(tree);
  }
  state.SetComplexityN(state.range(0));
}

static void btree_insert(benchmark::State& state) {
  auto tree = btree::BTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree = tree.insert(i, i);
  }

  for (auto _ : state) {
    tree = tree.insert(std::rand(), 0);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_get(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    tree.insert(i, i);
  }

  for (auto _ : state) {
    // NOTE: most random values fall above the tree max value and would cause
    // only best-case early exit for querying
    auto v = tree.get(std::rand() % tree.size());
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_insert(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(ftree::Right, i, i);
  }

  for (auto _ : state) {
    tree.insert(std::rand(), 0);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_push(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(ftree::Right, i, i);
  }

  for (auto _ : state) {
    // NOTE: it's ok to invalidate the ordering invariants because we don't
    // query the tree and the key or value have no influence on the push
    // behavior
    tree.push(ftree::Right, 0, 0);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_concat(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(ftree::Right, i, i);
  }

  auto copy = tree;

  for (auto _ : state) {
    // NOTE: it's ok to invalidate the ordering invariants because we don't
    // query the tree and the keys or values have no influence on the concat
    // behavior
    auto concat = ftree::FingerTree<int, int>::concat(tree, copy);
    benchmark::DoNotOptimize(concat);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_split(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(ftree::Right, i, i);
  }

  for (auto _ : state) {
    // NOTE: most random values fall above the tree max value and would cause
    // only best-case splits
    auto [l, n, r] = tree.split(std::rand() % tree.size());
    benchmark::DoNotOptimize(l);
    benchmark::DoNotOptimize(n);
    benchmark::DoNotOptimize(r);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(qmap_get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(qmap_insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

// BENCHMARK(btree_get)
//   ->Range(2 << 10, 2 << 16)
//   ->Complexity(benchmark::oAuto);

// BENCHMARK(btree_insert)
//   ->Range(2 << 10, 2 << 16)
//   ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_push)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_concat)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_split)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();

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

static constexpr uint ITERATIONS = 100'000;

void b_tree() {
  auto last = btree::BTree<int, TrackCopy, 32>();

  for (uint i = 0; i < ITERATIONS; i++) {
    auto copy = last;
    copy.insert(i, TrackCopy(i));
    last = copy;
  }
}

void finger_tree() {
  auto last = ftree::FingerTree<int, TrackCopy>();

  for (uint i = 0; i < ITERATIONS; i++) {
    auto copy = last;
    copy.insert(i, TrackCopy(i));
    last = copy;
  }
}

void q_map() {
  auto last = QMap<int, TrackCopy>();

  for (uint i = 0; i < ITERATIONS; i++) {
    auto copy = last;
    copy.insert(i, TrackCopy(i));
    last = copy;
  }
}

// int main() {
  // std::cout << "iterations: " << ITERATIONS << "\n" << std::endl;

  // b_tree();
  // std::cout << "persistent B-Tree: " << COPIES << std::endl;
  // TrackCopy::reset();

  // finger_tree();
  // std::cout << "persistent FingerTree: " << COPIES << std::endl;
  // TrackCopy::reset();

  // q_map();
  // std::cout << "QMap: " << COPIES << std::endl;
  // TrackCopy::reset();

//   return 0;
// }
