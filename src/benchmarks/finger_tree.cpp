#pragma once

#include "src/collections/finger_tree/finger_tree.hpp"

#include <benchmark/benchmark.h>

namespace benchmarks::finger_tree {
  using FT = collections::finger_tree::FingerTree<int, int>;
  using Dir = collections::finger_tree::Direction;

  static void get(benchmark::State& state) {
    auto tree = FT();
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

  static void insert(benchmark::State& state) {
    auto tree = FT();

    for (auto i = 0; i < state.range(0); i++) {
      tree.push(Dir::Right, i, i);
    }

    for (auto _ : state) {
      tree.insert(std::rand(), 0);
      benchmark::DoNotOptimize(tree);
    }

    state.SetComplexityN(state.range(0));
  }

  static void push(benchmark::State& state) {
    auto tree = FT();

    for (auto i = 0; i < state.range(0); i++) {
      tree.push(Dir::Right, i, i);
    }

    for (auto _ : state) {
      // NOTE: it's ok to invalidate the ordering invariants because we don't
      // query the tree and the key or value have no influence on the push
      // behavior
      tree.push(Dir::Right, 0, 0);
      benchmark::DoNotOptimize(tree);
    }

    state.SetComplexityN(state.range(0));
  }

  static void concat(benchmark::State& state) {
    auto tree = FT();

    for (auto i = 0; i < state.range(0); i++) {
      tree.push(Dir::Right, i, i);
    }

    auto copy = tree;

    for (auto _ : state) {
      // NOTE: it's ok to invalidate the ordering invariants because we don't
      // query the tree and the keys or values have no influence on the concat
      // behavior
      auto concat = FT::concat(tree, copy);
      benchmark::DoNotOptimize(concat);
    }

    state.SetComplexityN(state.range(0));
  }

  static void split(benchmark::State& state) {
    auto tree = FT();

    for (auto i = 0; i < state.range(0); i++) {
      tree.push(Dir::Right, i, i);
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
}
