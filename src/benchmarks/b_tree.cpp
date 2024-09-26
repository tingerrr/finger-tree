#pragma once

#include "src/collections/b_tree/b_tree.hpp"

#include <benchmark/benchmark.h>

namespace benchmarks::b_tree {
  using BT = collections::b_tree::BTree<int, int, 32>;

  static void get(benchmark::State& state) {
    auto tree = BT();
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

  static void insert(benchmark::State& state) {
    auto tree = BT();
    for (auto i = 0; i < state.range(0); i++) {
      tree = tree.insert(i, i);
    }

    for (auto _ : state) {
      tree = tree.insert(std::rand(), 0);
      benchmark::DoNotOptimize(tree);
    }

    state.SetComplexityN(state.range(0));
  }
}
