#include "test_framework.hpp"
#include "floyd_warshall.cpp"
#include <iostream>

bool test_basic_seq() {
  int vertexCount = 5;
  matrix adjacencyMatrix = {
      {0, 3, 0, 6, 0},
      {3, 0, 1, 0, 2},
      {0, 1, 0, 7, 2},
      {6, 0, 7, 0, 3},
      {0, 2, 2, 3, 0}
  };
  matrix dist = floydWarshall(vertexCount, adjacencyMatrix);
  // TODO: change from printing to "asserting" the result
  std::cout << "sequential run:\n";
  for (int i = 0; i < vertexCount; i++) {
      for (int j = 0; j < vertexCount; j++) {
          if (dist[i][j] == INT_MAX) {
              std::cout << "INF ";
          } else {
              std::cout << dist[i][j] << " ";
          }
      }
      std::cout << std::endl;
  }
  return true;
}

std::vector<testing::TestCase> init_tests = {{"Basic test of sequential Floyd Warshall implementation.", test_basic_seq}};