CONFIG -= qt
#CONFIG *= addressSanitizer
#CONFIG *= leakSanitizer

CONFIG *= googleBenchmark

OBJECTS_DIR = out/obj
MOC_DIR = out/moc
TARGET = out/main

QMAKE_CXXFLAGS += -std=c++20

HEADERS += src/btree/node/core.hpp
HEADERS += src/btree/node/base.hpp
HEADERS += src/btree/node/deep.hpp
HEADERS += src/btree/node/leaf.hpp
HEADERS += src/btree/node.hpp

HEADERS += src/btree/core.hpp
HEADERS += src/btree.hpp

SOURCES += src/main.cpp

include(qmake/sanitizer.prf)
include(qmake/benchmark.prf)
