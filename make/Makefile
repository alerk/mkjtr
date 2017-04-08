MAKE_DIR := $(shell pwd)
PROJECT_DIR := $(shell dirname $(MAKE_DIR))
DRIVER_DIR := $(PROJECT_DIR)/driver
LOGGER_DIR := $(PROJECT_DIR)/logger
UNAME := $(shell uname)
LBITS := $(shell getconf LONG_BIT)

ifeq ($(UNAME), Linux)
	OS = LINUX
	CC := gcc
	CXX := g++
else ifeq ($(UNAME), Darwin)
	OS = MACOSX
	CC := clang -arch x86_64
	CXX := clang -arch x86_64 -lc++
else
	OS = OTHER
endif

LD = $(CXX)
LDFLAGS        =  -Wall -Wno-unused -O3

TARGET_NAME = mkjtr_driver

SRC =
OBJ = $(SRC:%.cpp = %.o)
include $(DRIVER_DIR)/make/Makefile.obj
include $(LOGGER_DIR)/make/Makefile.obj

LINK = -std=c++0x
LIBS = -lpthread
LIBS_PATH = -I$(DRIVER_DIR)/lib
INCLUDE_PATH = -I$(DRIVER_DIR)/include -I$(LOGGER_DIR)/include

CXXFLAGS = $(LIBS) $(LIBS_PATH) $(INCLUDE_PATH)

%.o: %.cpp $(DEPS)
	$(CXX) -v -c $< -o $@ $(CXXFLAGS)

all: $(TARGET_NAME)

$(TARGET_NAME): $(OBJ)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(CXXFLAGS)


.PHONY: clean all

clean:
	rm $(OBJS) $(TARGET_NAME)
