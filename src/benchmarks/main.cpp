#include "src/benchmarks/qmap.cpp"
#include "src/benchmarks/b_tree.cpp"
#include "src/benchmarks/finger_tree.cpp"

#include <benchmark/benchmark.h>

BENCHMARK(benchmarks::qmap::get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::qmap::insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::b_tree::get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::b_tree::insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::get)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::insert)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::push)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::concat)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK(benchmarks::finger_tree::split)
  ->Range(2 << 10, 2 << 16)
  ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
