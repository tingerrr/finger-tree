CONFIG -= qt
CONFIG *= addressSanitizer
#CONFIG *= leakSanitizer

CONFIG *= staticlib

OBJECTS_DIR = out/obj
MOC_DIR = out/moc
TARGET = out/main

QMAKE_CXXFLAGS += -std=c++20

HEADERS += src/btree/node.hpp
SOURCES += src/main.cpp

include(qmake/sanitizer.prf)
