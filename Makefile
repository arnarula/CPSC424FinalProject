CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iparlaylib/include/
LDFLAGS :=

BIN_DIR := bin
SRC_DIR := src

par: LDFLAGS += -fopenmp


all: $(BIN_DIR)/seq $(BIN_DIR)/par

$(BIN_DIR)/seq: $(SRC_DIR)/floyd_warshall.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/par: $(SRC_DIR)/floyd_warshall_parallel.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

test: all
	./$(BIN_DIR)/seq
	./$(BIN_DIR)/par

.PHONY: clean
clean:
	$(RM) $(BIN_DIR)/seq
	$(RM) $(BIN_DIR)/par