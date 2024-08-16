CONFIG -= qt

OBJECTS_DIR = out/obj
MOC_DIR = out/moc
TARGET = out/main

QMAKE_CXXFLAGS += -std=c++20

HEADERS += src/btree/node.hpp
SOURCES += src/main.cpp
