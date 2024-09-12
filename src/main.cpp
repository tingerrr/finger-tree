#include "btree.hpp"
#include "ftree.hpp"

#include <benchmark/benchmark.h>

static void btree_get_full(benchmark::State& state) {
  auto r = btree::BTree<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    r = r.insert(i, i);
  }

  for (auto _ : state) {
    auto v = r.get(std::rand() % r.size());
    benchmark::DoNotOptimize(v);
  }

  state.SetComplexityN(state.range(0));
}

static void btree_insert_consecutive(benchmark::State& state) {
  auto r = btree::BTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    r = r.insert(i, i);
  }

  int i = 0;
  for (auto _ : state) {
    r = r.insert(i, i);
    benchmark::DoNotOptimize(r);
    i++;
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_get_full(benchmark::State& state) {
  auto r = btree::BTree<int, int>();
  for (auto i = 0; i < state.range(0); i++) {
    r = r.insert(i, i);
  }

  for (auto _ : state) {
    auto v = r.get(std::rand() % r.size());
    benchmark::DoNotOptimize(v);
  }

  state.SetComplexityN(state.range(0));
}

static void ftree_push_consecutive(benchmark::State& state) {
  auto r = ftree::FingerTree<int, int>();

  for (auto i = 0; i < state.range(0); i++) {
    r.push(ftree::Left, i, i);
  }

  for (auto _ : state) {
    // NOTE: it's ok to violate the unique invariants here because we don't
    // query the tree
    r.push(ftree::Left, 0, 0);
    benchmark::DoNotOptimize(r);
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

BENCHMARK_MAIN();
