#CONFIG -= qt
#CONFIG += debug

#CONFIG *= addressSanitizer
#CONFIG *= leakSanitizer

CONFIG *= googleBenchmark

OBJECTS_DIR = out/obj
MOC_DIR = out/moc
TARGET = out/main

QMAKE_CXXFLAGS += -std=c++20

HEADERS += src/collections/b_tree/node/core.hpp
HEADERS += src/collections/b_tree/node/base.hpp
HEADERS += src/collections/b_tree/node/deep.hpp
HEADERS += src/collections/b_tree/node/leaf.hpp
HEADERS += src/collections/b_tree/node.hpp

HEADERS += src/collections/b_tree/core.hpp
HEADERS += src/collections/b_tree/b_tree.hpp

HEADERS += src/collections/finger_tree/digit/_prelude.hpp
HEADERS += src/collections/finger_tree/digit/core.hpp
HEADERS += src/collections/finger_tree/digit/base.hpp
HEADERS += src/collections/finger_tree/digit/digit.hpp

HEADERS += src/collections/finger_tree/node/_prelude.hpp
HEADERS += src/collections/finger_tree/node/core.hpp
HEADERS += src/collections/finger_tree/node/base.hpp
HEADERS += src/collections/finger_tree/node/deep.hpp
HEADERS += src/collections/finger_tree/node/leaf.hpp
HEADERS += src/collections/finger_tree/node/node.hpp

HEADERS += src/collections/finger_tree/_prelude.hpp
HEADERS += src/collections/finger_tree/core.hpp
HEADERS += src/collections/finger_tree/base.hpp
HEADERS += src/collections/finger_tree/deep.hpp
HEADERS += src/collections/finger_tree/single.hpp
HEADERS += src/collections/finger_tree/empty.hpp

HEADERS += src/utils/uninit_exception.hpp
HEADERS += src/utils/variant_exception.hpp

SOURCES += src/main.cpp

include(qmake/sanitizer.prf)
include(qmake/benchmark.prf)
