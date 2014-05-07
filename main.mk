## Sigproc

TARGET  := sigproc

INCDIRS := include

CXX      := g++-4.8
CXXFLAGS := -std=gnu++11 -g -gdwarf-2

SOURCES := src/main.cpp
SOURCES += src/mimosystem.cpp
SOURCES += src/sigproc.cpp
SOURCES += src/signode.cpp



