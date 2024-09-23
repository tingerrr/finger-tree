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


HEADERS += src/finger_tree/digit/_prelude.hpp
HEADERS += src/finger_tree/digit/core.hpp
HEADERS += src/finger_tree/digit/base.hpp
HEADERS += src/finger_tree/digit/digit.hpp

HEADERS += src/finger_tree/node/_prelude.hpp
HEADERS += src/finger_tree/node/core.hpp
HEADERS += src/finger_tree/node/base.hpp
HEADERS += src/finger_tree/node/deep.hpp
HEADERS += src/finger_tree/node/leaf.hpp
HEADERS += src/finger_tree/node/node.hpp

HEADERS += src/finger_tree/_prelude.hpp
HEADERS += src/finger_tree/core.hpp
HEADERS += src/finger_tree/base.hpp
HEADERS += src/finger_tree/deep.hpp
HEADERS += src/finger_tree/single.hpp
HEADERS += src/finger_tree/empty.hpp

HEADERS += src/utils/uninit_exception.hpp
HEADERS += src/utils/variant_exception.hpp

SOURCES += src/main.cpp

include(qmake/sanitizer.prf)
include(qmake/benchmark.prf)
