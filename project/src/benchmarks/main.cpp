#include "src/benchmarks/qmap.cpp"
#include "src/benchmarks/b_tree.cpp"
#include "src/benchmarks/finger_tree.cpp"

#include <benchmark/benchmark.h>

BENCHMARK(benchmarks::qmap::get)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::qmap::insert_unique)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::qmap::insert_shared)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::b_tree::get)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::b_tree::insert)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::get)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

// NOTE: values required to provoke the worst case
BENCHMARK(benchmarks::finger_tree::push_worst)
  ->Arg(1820)
  ->Arg(5465)
  ->Arg(16400)
  ->Arg(49205)
  ->Arg(147620)
  ->Arg(442865)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::push_avg)
  ->RangeMultiplier(2)
  ->Range(2 << 10, 2 << 18)
  ->Complexity(benchmark::oAuto);

// NOTE: required for easy summation in thesis
BENCHMARK(benchmarks::finger_tree::concat)
  ->Arg(1820)
  ->Arg(5465)
  ->Arg(16400)
  ->Arg(49205)
  ->Arg(147620)
  ->Arg(442865)
  ->Complexity(benchmark::oAuto);

// NOTE: see above
BENCHMARK(benchmarks::finger_tree::split)
  ->Arg(1820)
  ->Arg(5465)
  ->Arg(16400)
  ->Arg(49205)
  ->Arg(147620)
  ->Arg(442865)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
