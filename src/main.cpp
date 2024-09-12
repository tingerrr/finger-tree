#include "btree.hpp"
#include "ftree.hpp"

#include <benchmark/benchmark.h>

static void btree_get_full(benchmark::State& state) {
  auto tree = btree::BTree<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    tree = tree.insert(i, i);
  }

  for (auto _ : state) {
    auto v = tree.get(std::rand() % tree.size());
    benchmark::DoNotOptimize(v);
  }

  state.SetComplexityN(state.range(0));
}

static void btree_insert_consecutive(benchmark::State& state) {
  auto tree = btree::BTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree = tree.insert(i, i);
  }

  int i = 0;
  for (auto _ : state) {
    tree = tree.insert(i, i);
    benchmark::DoNotOptimize(tree);
    i++;
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_get_full(benchmark::State& state) {
  auto tree = btree::BTree<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    tree = tree.insert(i, i);
  }

  for (auto _ : state) {
    auto v = tree.get(std::rand() % tree.size());
    benchmark::DoNotOptimize(v);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_push_consecutive(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(ftree::Left, i, i);
  }

  for (auto _ : state) {
    // NOTE: it's ok to violate the unique invariants here because we don't
    // query the tree
    tree.push(ftree::Left, 0, 0);
    benchmark::DoNotOptimize(tree);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_concat(benchmark::State& state) {
  auto tree = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    tree.push(ftree::Left, i, i);
  }

  auto copy = tree;

  for (auto _ : state) {
    // NOTE: it's ok to violate the ordering invariants here because we don't
    // query the tree
    auto concat = ftree::FingerTree<int, int>::concat(tree, copy);
    benchmark::DoNotOptimize(concat);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(btree_get_full)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(btree_insert_consecutive)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_get_full)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_push_consecutive)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_concat)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
