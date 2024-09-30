#pragma once

#include "src/collections/b_tree/b_tree.hpp"

#include <benchmark/benchmark.h>

namespace benchmarks::b_tree {
  using BT = collections::b_tree::BTree<int, int, 32>;

  auto get(benchmark::State& state) -> void {
    auto tree = BT();
    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      tree = tree.insert(v, v);
    }

    for (auto _ : state) {
      auto v = tree.get(std::rand());
      benchmark::DoNotOptimize(v);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(state.range(0));
  }

  auto insert(benchmark::State& state) -> void {
    auto tree = BT();
    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      tree = tree.insert(v, v);
    }

    for (auto _ : state) {
      tree.insert(std::rand(), 0);
      benchmark::DoNotOptimize(tree);
      benchmark::ClobberMemory();
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(state.range(0));
  }
}
