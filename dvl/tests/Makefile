CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror

TARGET := dvl-tests
TEST_SOURCES := main.cpp $(wildcard math/*.cpp) $(wildcard anim/*.cpp)
DVL_SOURCES := ../src/math/mat.cpp ../src/math/math.cpp ../src/math/quat.cpp ../src/math/vec.cpp ../src/anim/anim.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(TEST_SOURCES) $(DVL_SOURCES) unit_test.h
	$(CXX) $(CXXFLAGS) -I../include $(TEST_SOURCES) $(DVL_SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
