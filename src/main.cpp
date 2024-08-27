#include "btree.hpp"

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

BENCHMARK(btree_insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
