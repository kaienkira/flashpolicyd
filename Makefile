TARGET = bin/flashpolicyd
SRCS = src/flashpolicyd.cc
LINK_TYPE = exec
BUILD_DIR = build
LIB = -lbrickredcore -lbrickredunix

include mak/main.mak
