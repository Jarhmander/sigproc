## testapp

TARGET  := sigproc

SRC_INCDIRS := include

SOURCES := src/main.cpp

TGT_LDLIBS  := -lsigproc
TGT_LDFLAGS := -L${TARGET_DIR}
TGT_PREREQS := libsigproc.a

