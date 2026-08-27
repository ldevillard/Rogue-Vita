CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror

TARGET := dvl-tests
TEST_SOURCES := main.cpp $(wildcard math/*.cpp)
DVL_SOURCES := ../src/math/vec.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(TEST_SOURCES) unit_test.h
	$(CXX) $(CXXFLAGS) -I../include $(TEST_SOURCES) $(DVL_SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
