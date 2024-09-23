#include "src/btree.hpp"
#include "src/finger_tree/finger_tree.hpp"

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

static void finger_tree_get(benchmark::State& state) {
  auto tree = finger_tree::FingerTree<int, int>();
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

static void finger_tree_insert(benchmark::State& state) {
  auto tree = finger_tree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(finger_tree::Direction::Right, i, i);
  }

  for (auto _ : state) {
    tree.insert(std::rand(), 0);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void finger_tree_push(benchmark::State& state) {
  auto tree = finger_tree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(finger_tree::Direction::Right, i, i);
  }

  for (auto _ : state) {
    // NOTE: it's ok to invalidate the ordering invariants because we don't
    // query the tree and the key or value have no influence on the push
    // behavior
    tree.push(finger_tree::Direction::Right, 0, 0);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void finger_tree_concat(benchmark::State& state) {
  auto tree = finger_tree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(finger_tree::Direction::Right, i, i);
  }

  auto copy = tree;

  for (auto _ : state) {
    // NOTE: it's ok to invalidate the ordering invariants because we don't
    // query the tree and the keys or values have no influence on the concat
    // behavior
    auto concat = finger_tree::FingerTree<int, int>::concat(tree, copy);
    benchmark::DoNotOptimize(concat);
  }

  state.SetComplexityN(state.range(0));
}

static void finger_tree_split(benchmark::State& state) {
  auto tree = finger_tree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(finger_tree::Direction::Right, i, i);
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

BENCHMARK(btree_get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(btree_insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(finger_tree_get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(finger_tree_insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(finger_tree_push)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(finger_tree_concat)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(finger_tree_split)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
