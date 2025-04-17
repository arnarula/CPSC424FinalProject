# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iparlaylib/include/ -Iinclude/
LDFLAGS :=

# Directories
SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Source files
# MAIN_SRC := $(SRC_DIR)/main.cpp
# COMMON_SRC := $(filter-out $(MAIN_SRC), $(wildcard $(SRC_DIR)/*.cpp))
LIB_SRC := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)

# Object files
LIB_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(LIB_SRC))
TEST_OBJ := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SRC))
# MAIN_OBJ := $(OBJ_DIR)/src_main.o

# Executables
TEST_EXE := $(BIN_DIR)/test_runner

# Default target
.PHONY: all test clean dirs
all: dirs test

# Ensure directories exist
dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Test executable
test: dirs $(TEST_EXE)
	$(TEST_EXE)

$(TEST_EXE): $(LIB_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)
