#CONFIG -= qt
#CONFIG += debug

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


HEADERS += src/ftree/core.hpp
HEADERS += src/ftree/deep.hpp
HEADERS += src/ftree/digit/core.hpp
HEADERS += src/ftree/digit.hpp
HEADERS += src/ftree/node/core.hpp
HEADERS += src/ftree/node/deep.hpp
HEADERS += src/ftree/node/leaf.hpp
HEADERS += src/ftree/node.hpp
HEADERS += src/ftree/single.hpp
HEADERS += src/ftree.hpp

SOURCES += src/main.cpp

include(qmake/sanitizer.prf)
include(qmake/benchmark.prf)
