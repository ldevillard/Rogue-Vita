CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -DDVL_PROFILING_ENABLED=0

TARGET := engine-tests
TEST_SOURCES := $(shell find . -type f -name '*.cpp')
ENGINE_SOURCES := $(shell find ../src/engine -type f -name '*.cpp')
DVL_SOURCES := $(shell find ../dvl/src -type f -name '*.cpp' ! -path '*/vita/*' ! -path '*/vitagl/*')
LIBS := -lglfw -lGLEW -lGL -ldl -pthread

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(TEST_SOURCES) $(ENGINE_SOURCES) $(DVL_SOURCES) ../dvl/tests/unit_test.h
	$(CXX) $(CXXFLAGS) -I../include -I../dvl/include -I../dvl/tests $(TEST_SOURCES) $(ENGINE_SOURCES) $(DVL_SOURCES) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
