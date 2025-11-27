CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS := -pthread

SRC := $(wildcard src/*.cpp)
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))
TARGET := xbroker

.PHONY: all debug release clean run

all: release

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(TARGET)

release: CXXFLAGS += -O2
release: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)

run: $(TARGET)
	./$(TARGET)
