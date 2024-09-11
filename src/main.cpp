#include "btree.hpp"
#include "ftree.hpp"

#include <benchmark/benchmark.h>

static void btree_insert(benchmark::State& state) {
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

static void ftree_push(benchmark::State& state) {
  auto r = ftree::FingerTree<int, int>();
  // for (auto i = 0; i < state.range(0); i++) {
  // }

  for (auto _ : state) {
    r.push(ftree::Left, 0, 0);
    benchmark::DoNotOptimize(r);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(btree_insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(ftree_push)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
