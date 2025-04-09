CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

SRC = floyd_warshall.cpp
EXE = bin/exe

run: $(EXE)
	@./$(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(SRC)

clean:
	rm -f $(EXE)
