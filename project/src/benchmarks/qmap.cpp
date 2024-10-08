#pragma once

#include <QMap>

#include <benchmark/benchmark.h>

namespace benchmarks::qmap {
  auto get(benchmark::State& state) -> void {
    auto map = QMap<int, int>();
    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      map.insert(v, v);
    }

    for (auto _ : state) {
      auto v = map.find(std::rand());
      benchmark::DoNotOptimize(v);
    }

    benchmark::DoNotOptimize(map);
    state.SetComplexityN(state.range(0));
  }

  auto insert_unique(benchmark::State& state) -> void {
    auto map = QMap<int, int>();
    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      map.insert(v, v);
    }

    for (auto _ : state) {
      auto v = map.insert(std::rand(), 0);
      benchmark::DoNotOptimize(v);
    }

    benchmark::DoNotOptimize(map);
    state.SetComplexityN(state.range(0));
  }

  auto insert_shared(benchmark::State& state) -> void {
    auto map = QMap<int, int>();
    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      map.insert(v, v);
    }

    for (auto _ : state) {
      auto copy = map;
      auto v = map.insert(std::rand(), 0);
      benchmark::DoNotOptimize(v);
      benchmark::DoNotOptimize(copy);
    }

    benchmark::DoNotOptimize(map);
    state.SetComplexityN(state.range(0));
  }
}
