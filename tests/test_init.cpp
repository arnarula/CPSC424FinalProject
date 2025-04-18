#include "test_framework.hpp"
#include "floyd_warshall.hpp"
#include "floyd_warshall_parallel.hpp"

#include <iostream>
#include <vector>

// missing edge
#define X std::numeric_limits<long long>::max()

bool test_basic_seq_1() {
  matrix adjacencyMatrix = {
      {X, 3, X, 6, X},
      {3, X, 1, X, 2},
      {X, 1, X, 7, 2},
      {6, X, 7, X, 3},
      {X, 2, 2, 3, X}
  };

  matrix expected = {
    {0, 3, 4, 6, 5},
    {3, 0, 1, 5, 2},
    {4, 1, 0, 5, 2},
    {6, 5, 5, 0, 3},
    {5, 2, 2, 3, 0}
  };

  matrix output = floyd_warshall(adjacencyMatrix);

  for (size_t i = 0; i < output.size(); i++) {
    for (size_t j = 0; j < output[i].size(); j++) {
      if (output[i][j] != expected[i][j]) {
        std::cout << "At (" << i << ", " << j << "): "
                  << output[i][j] << " != " << expected[i][j] << std::endl;
        return false;
      }
    }
  }

  return true;
}

bool test_basic_seq_2() {

  adj_list adjacencyList = 
  {
    {{1, 3}, {3, 6}},
    {{0, 3}, {2, 1}, {4, 2}},
    {{1, 1}, {3, 7}, {4, 2}},
    {{0, 6}, {4, 3}},
    {{1, 2}, {2, 2}, {3, 3}}
  };

  matrix expected = {
    {0, 3, 4, 6, 5},
    {3, 0, 1, 5, 2},
    {4, 1, 0, 5, 2},
    {6, 5, 5, 0, 3},
    {5, 2, 2, 3, 0}
  };

  matrix output = floyd_warshall(adjacencyList);

  for (size_t i = 0; i < output.size(); i++) {
    for (size_t j = 0; j < output[i].size(); j++) {
      if (output[i][j] != expected[i][j]) {
        std::cout << "At (" << i << ", " << j << "): "
                  << output[i][j] << " != " << expected[i][j] << std::endl;
        return false;
      }
    }
  }

  return true;
}

bool test_basic_seq_neg_cycle() {
  // has negative cycle
  matrix adjacencyMatrix = {
      {0, 1, X},
      {X, 0,-1},
      {-1, X, 0}
  };

  if (!has_negative_cycle(floyd_warshall(adjacencyMatrix))) {
      return false;
  }

  // doesn't have negative cycle
  matrix adjacencyMatrix2 = {
      {0, 1, X},
      {X, 0, 1},
      {-1, X, 0}
  };

  if (has_negative_cycle(floyd_warshall(adjacencyMatrix2))) {
      return false;
  }

  return true;
}

bool test_basic_par_1() {
  matrix adjacencyMatrix = {
      {X, 3, X, 6, X},
      {3, X, 1, X, 2},
      {X, 1, X, 7, 2},
      {6, X, 7, X, 3},
      {X, 2, 2, 3, X}
  };

  matrix expected = {
    {0, 3, 4, 6, 5},
    {3, 0, 1, 5, 2},
    {4, 1, 0, 5, 2},
    {6, 5, 5, 0, 3},
    {5, 2, 2, 3, 0}
  };

  matrix output = floyd_warshall_parallel(adjacencyMatrix);

  for (size_t i = 0; i < output.size(); i++) {
    for (size_t j = 0; j < output[i].size(); j++) {
      if (output[i][j] != expected[i][j]) {
        std::cout << "At (" << i << ", " << j << "): "
                  << output[i][j] << " != " << expected[i][j] << std::endl;
        return false;
      }
    }
  }

  return true;
}

bool test_basic_par_2() {
    matrix adjacencyMatrix = get_rand_graph_2(100, 0.1, 1234);

    // sequential floyd warshall
    matrix seq_output = floyd_warshall(adjacencyMatrix);

    // parallel floyd warshall
    matrix par_output = floyd_warshall_parallel(adjacencyMatrix);

    for (size_t i = 0; i < seq_output.size(); i++) {
        for (size_t j = 0; j < seq_output[i].size(); j++) {
            if (seq_output[i][j] != par_output[i][j]) {
                std::cout << "At (" << i << ", " << j << "): "
                          << seq_output[i][j] << " != " << par_output[i][j] << std::endl;
                return false;
            }
        }
    }

    return true;
}

bool test_runtime_par() {
  matrix adjacencyMatrix = get_rand_graph_2(500, 0.1, 1234);

  // sequential floyd warshall
  matrix seq_output = floyd_warshall(adjacencyMatrix, true);
  matrix par_output = floyd_warshall_parallel(adjacencyMatrix, true);
  matrix par_output_2 = floyd_warshall_parallel_2(adjacencyMatrix, true);
  matrix par_output_3 = floyd_warshall_parallel_3(adjacencyMatrix, true);

  for (size_t i = 0; i < seq_output.size(); i++) {
      for (size_t j = 0; j < seq_output[i].size(); j++) {
          if (seq_output[i][j] != par_output[i][j]) {
              std::cout << "At (" << i << ", " << j << "): "
                        << seq_output[i][j] << " != " << par_output[i][j] << std::endl;
              return false;
          }
          if (seq_output[i][j] != par_output_2[i][j]) {
              std::cout << "At (" << i << ", " << j << "): "
                        << seq_output[i][j] << " != " << par_output_2[i][j] << std::endl;
              return false;
          }
          if (seq_output[i][j] != par_output_3[i][j]) {
              std::cout << "At (" << i << ", " << j << "): "
                        << seq_output[i][j] << " != " << par_output_3[i][j] << std::endl;
              return false;
          }
      }
  }

  return true;
}

bool matrix_adj_list_converter() {
  matrix adjacencyMatrix = {
      {X, 3, X, 6, X},
      {3, X, 1, X, 2},
      {X, 1, X, 7, 2},
      {6, X, 7, X, 3},
      {X, 2, 2, 3, X}
  };

  adj_list adjacencyList = matrix_to_adj_list(adjacencyMatrix);
  matrix restoredAdjacencyMatrix = adj_list_to_matrix(adjacencyList);

  for (size_t i = 0; i < restoredAdjacencyMatrix.size(); i++) {
    for (size_t j = 0; j < restoredAdjacencyMatrix[i].size(); j++) {
      if (restoredAdjacencyMatrix[i][j] != adjacencyMatrix[i][j]) {
        std::cout << "At (" << i << ", " << j << "): "
                  << restoredAdjacencyMatrix[i][j] << " != " << adjacencyMatrix[i][j] << std::endl;
        return false;
      }
    }
  }

  return true;
}

std::vector<testing::TestCase> graph_builder_tests = {
    {"Graph test: adjacency matrix to adjacency list converter", matrix_adj_list_converter},
};

std::vector<testing::TestCase> init_tests = {
    {"Basic test: sequential floyd w/ 5x5 adjacency matrix.", test_basic_seq_1},
    {"Basic test: sequential floyd w/ len 5 adjacency list.", test_basic_seq_2},
    {"Basic test: sequential floyd w/ negative cycle.", test_basic_seq_neg_cycle},
    {"Basic test: parallel floyd w/ 5x5 adjacency matrix.", test_basic_par_1},
    {"Medium test: parallel floyd w/ 100x100 adjacency matrix.", test_basic_par_2}
};

std::vector<testing::TestCase> runtime_tests = {
    {"Runtime test: floyd benchmarks.", test_runtime_par}
};
