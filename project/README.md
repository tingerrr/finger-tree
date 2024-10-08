# Project structure
The project is structured as follows:
- `qmake`: additional qmake config files for different features
- `out`: compilation output directory
- `src`: source code directory
  - `copy_tracking/main.cpp`: demonstrates the reduced amount of copies of persistent data structures
  - `benchmarks/main.cpp`: collects and runs benchmarks
  - `tests/main.cpp`: a testing file for fiddling with the data strucutres
  - `main.cpp`: phony main file, includes one of the previous main files
  - `collections/b_tree`: a very simple and incomplete persistent B-Tree implementation
  - `collections/finger_tree`: the main 2-3-FingerTree implementation, see below
  - `utils`: helper types and functions

# 2-3-FingerTree
The `collections/finger_tree` directory contains source files which implement a persistent 2-3-FingerTree without any optimizations.
It is almost a defacto copy of the Haskell implementation, it was initially generic over its branching factors, which was later dropped due to lack of proofs of generality.

Each class is defined inside a hpp file without the use of cpp files due to the heavy template usage.
The `finger_tree` namespace has two sub namespaces, `node` and `digit`, which contain the appropriate implementations of nodes and digits.
The directory structure roughly follows that of the modules with the exception of the `core` and `_prelude` files which are used to forward declare various classes and usings.

`src/finger_tree/finger_tree.hpp` should serve as a good **entrypoint** for review, all relevant other files are included there.
The function definitions largely match those of the thesis, but have slight differences which are considered implementation details.

# Tooling
Minumum required tooling for compiling and running are:
- [qmake (Qt)][qmake]
- [make][make]
- [gcc supporting at least C++20][gcc]

# Dependencies
These dependencies were used, other versions may or may not be compatible:
- [libQt 4.8.7 (Qt)][qt]
- [libbenchmark 1.9.0 (google benchmark)][gbench]

# Running
There are three sub projects (for a lack of a better word [^1]), the benchmarks, the copy tracking example and the test main file for debugging.
Uncomment the appropriate line inside `src/main.cpp` and run the following commands in the same directory as this README:

```
mkdir -p out
qmake
make
./out/main
```

[^1]: Yes, I know that these could be actual individual qmake projects, I was short on time.

[qmake]: https://doc.qt.io/qt-6/qmake-manual.html
[make]: https://www.gnu.org/software/make/
[gcc]: https://gcc.gnu.org/
[qt]: https://github.com/qt/qtbase
[gbench]: https://github.com/google/benchmark
