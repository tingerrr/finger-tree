#pragma once

#include "src/collections/finger_tree/finger_tree.hpp"

#include <benchmark/benchmark.h>
#include <cmath>

namespace benchmarks::finger_tree {
  using FT = collections::finger_tree::FingerTree<int, int>;
  using Dir = collections::finger_tree::Direction;

  // for a given depth d, this function returns the number of elements k
  // required to push on one side to have push recursive to this depth d
  //
  // k(0) = 1
  // k(d) = k(d - 1) + 3^(d - 1) + 4 * 3^(d - 1)
  auto depth_to_overflow_count(uint d) -> uint {
    if (d == 0) {
      return 1;
    }

    auto pow = std::pow(3, d - 1);
    return depth_to_overflow_count(d - 1) + pow + 4 * pow;
  }

  // this function takes an expected element count n and returns the nearest
  // value k within the sequence produced by `depth_to_overflow_count`
  auto depth_to_overflow_count_nearest(uint n) -> uint {
    uint d = 1;
    while (depth_to_overflow_count(d) < n) {
      d += 1;
    }

    auto k1 = depth_to_overflow_count(d - 1);
    auto k2 = depth_to_overflow_count(d);

    if (n - k1 < k2 - n) {
      return k1;
    } else {
      return k2;
    }
  }

  // Assuming that a uniform distribution of random values inserted into a
  // finger tree creates a relatively balanced tree, the median value must be in
  // the middle, i.e. the deepest node.
  //
  // Therefore the median gives a reliable worst-case get performance benchmark
  // by always requiring descent into the deepst node.
  auto get(benchmark::State& state) -> void {
    auto tree = FT();
    std::vector<int> vals;

    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      tree.insert(v, v);
      vals.push_back(v);
    }

    std::sort(vals.begin(), vals.end());
    auto split = vals[vals.size() / 2];

    for (auto _ : state) {
      auto v = tree.get(split);
      benchmark::DoNotOptimize(v);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(tree.size());
  }

  // The worst-case performance of push is given by pushing onto a fully unsafe
  // side, i.e. every deep tree has four digits on that side.
  //
  // Pushing only on one side repeatedly creates a tree which is heavy on that
  // side.
  auto push_worst(benchmark::State& state) -> void {
    auto tree = FT();
    auto n = depth_to_overflow_count_nearest(state.range(0));

    // full deep overflow occurs at n pushes, we do one less
    for (uint i = 0; i < n - 1; i++) {
      tree.push(Dir::Right, 0, 0);
    }

    for (auto _ : state) {
      auto copy = tree;
      copy.push(Dir::Right, 0, 0);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(tree.size());
  }

  // Assuming that a uniform distribution of random values inserted into a
  // finger tree creates a relatively balanced tree.
  //
  // Pushing to a relatively balanced tree gives a reliable average-case
  // performance benchmark.
  auto push_avg(benchmark::State& state) -> void {
    auto tree = FT();
    for (uint i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      tree.insert(v, v);
    }

    for (auto _ : state) {
      auto copy = tree;
      copy.push(Dir::Right, 0, 0);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(tree.size());
  }

  // The worst-case performance of pop is given by popping of a fully unsafe
  // side, i.e. every deep tree has only one digit on that side.
  //
  // Pushing only on one side repeatedly creates a tree which is heavy on that
  // side and light on the other.
  auto pop_worst(benchmark::State& state) -> void {
    auto tree = FT();

    // TODO: Ensure this is actually a worst-case tree for the pop opration.
    for (auto i = 0; i < state.range(0); i++) {
      tree.push(Dir::Left, 0, 0);
    }

    for (auto _ : state) {
      auto copy = tree;
      copy.pop(Dir::Right);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(tree.size());
  }

  // Assuming that a uniform distribution of random values inserted into a
  // finger tree creates a relatively balanced tree.
  //
  // Popping from a relatively balanced tree gives a reliable average-case
  // performance benchmark.
  auto pop_avg(benchmark::State& state) -> void {
    auto tree = FT();
    for (uint i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      tree.insert(v, v);
    }

    for (auto _ : state) {
      auto copy = tree;
      copy.pop(Dir::Right);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(tree.size());
  }

  // The worst-case performance of concat is dependent on the packing required
  // on the inside of the new tree.
  //
  // Therefore concatenation of a right-heavy left tree and a left-heavy right
  // tree gives us a reliable worst-case concat performance benchmark.
  auto concat(benchmark::State& state) -> void {
    auto left = FT();
    auto right = FT();

    for (auto i = 0; i < state.range(0); i++) {
      left.push(Dir::Right, 0, 0);
      right.push(Dir::Left, 0, 0);
    }

    for (auto _ : state) {
      auto concat = FT::concat(left, right);
      benchmark::DoNotOptimize(concat);
    }

    benchmark::DoNotOptimize(left);
    benchmark::DoNotOptimize(right);
    state.SetComplexityN(left.size());
  }

  // TODO: order values in a map and push them alternatingly to create a dense tree

  // Assuming that a uniform distribution of random values inserted into a
  // finger tree creates a relatively balanced tree, the median value must be in
  // the middle, i.e. the deepest node.
  //
  // Therefore the median gives a reliable worst-case split performance
  // benchmark by always requiring descent into the deepst node.
  auto split(benchmark::State& state) -> void {
    auto tree = FT();
    std::vector<int> vals;

    for (auto i = 0; i < state.range(0); i++) {
      auto v = std::rand();
      vals.push_back(v);
    }

    std::sort(vals.begin(), vals.end());
    auto split = vals[vals.size() / 2];

    for (int i = vals.size() / 2 - 1; i >= 0; i--) {
      auto v = vals[i];
      tree.push(Dir::Left, v, v);
    }

    for (size_t i = vals.size() / 2; i < vals.size(); i++) {
      auto v = vals[i];
      tree.push(Dir::Right, v, v);
    }

    for (auto _ : state) {
      auto [l, n, r] = tree.split(split);
      benchmark::DoNotOptimize(l);
      benchmark::DoNotOptimize(n);
      benchmark::DoNotOptimize(r);
    }

    benchmark::DoNotOptimize(tree);
    state.SetComplexityN(tree.size());
  }
}
