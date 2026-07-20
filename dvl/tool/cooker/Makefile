CXX ?= c++

TARGET := build/dvl-cooker
SOURCES := $(wildcard src/*.cpp) ../../src/log/desktop/desktop_log.cpp

CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS := -Iinclude -I../../include -Ithird_party
LDLIBS := -lassimp

ifeq ($(shell uname -s),Darwin)
ASSIMP_PREFIX := $(shell brew --prefix assimp)
CPPFLAGS += -I$(ASSIMP_PREFIX)/include
LDFLAGS += -L$(ASSIMP_PREFIX)/lib
endif

.PHONY: all clean

all:
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

clean:
	rm -rf build
