#pragma once

#include <QMap>

#include <benchmark/benchmark.h>

namespace benchmarks::qmap {
  static void get(benchmark::State& state) {
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

  static void insert(benchmark::State& state) {
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
}
