#include "test_framework.hpp"
#include "floyd_warshall.hpp"
#include "floyd_warshall_parallel.hpp"
#include "path_utils.hpp"

#include <iostream>
#include <vector>

// missing edge
#define X std::numeric_limits<long long>::max()

static const matrix basic_adj_mat_5 = {
  {X, 3, X, 6, X},
  {3, X, 1, X, 2},
  {X, 1, X, 7, 2},
  {6, X, 7, X, 3},
  {X, 2, 2, 3, X}
};
static const matrix basic_adj_mat_5_2 = {
  {X, 3, X, 6, X},
  {3, X, 1, X, 2},
  {X, 1, X, 1, 2},
  {6, X, 7, X, 3},
  {X, 2, 2, 3, X}
};
static const matrix basic_adj_mat_5_expected = {
  {0, 3, 4, 6, 5},
  {3, 0, 1, 5, 2},
  {4, 1, 0, 5, 2},
  {6, 5, 5, 0, 3},
  {5, 2, 2, 3, 0}
};
static const matrix basic_adj_mat_5_2_expected = {
  {0, 3, 4, 5, 5},
  {3, 0, 1, 2, 2},
  {4, 1, 0, 1, 2},
  {6, 5, 5, 0, 3},
  {5, 2, 2, 3, 0}
};

bool test_basic_seq_1() {
  matrix adjacencyMatrix{basic_adj_mat_5};
  const matrix &expected = basic_adj_mat_5_expected;

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
  const matrix &expected =basic_adj_mat_5_expected;
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
  matrix adjacencyMatrix{basic_adj_mat_5};
  const matrix &expected = basic_adj_mat_5_expected;

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
   matrix adjacencyMatrix = get_rand_graph_2(1000, 0.1, 1234);
  // matrix adjacencyMatrix = get_rand_graph_2(2000, 0.1, 1234);

  // sequential floyd warshall
  // matrix seq_output = floyd_warshall(adjacencyMatrix, true);
  matrix par_output = floyd_warshall_parallel(adjacencyMatrix, true);
  matrix seq_output = par_output;
  matrix par_output_2 = floyd_warshall_parallel_2(adjacencyMatrix, true);
  matrix par_output_3 = floyd_warshall_parallel_3(adjacencyMatrix, true);
  matrix par_output_4 = floyd_warshall_parallel_4(adjacencyMatrix, true);
  matrix par_output_5 = floyd_warshall_parallel_5(adjacencyMatrix, true);
  matrix par_output_6 = floyd_warshall_parallel_6(adjacencyMatrix, true);

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
          if (seq_output[i][j] != par_output_4[i][j]) {
            std::cout << "At (" << i << ", " << j << "): "
                      << seq_output[i][j] << " != " << par_output_4[i][j] << std::endl;
            return false;
          }
          if (seq_output[i][j] != par_output_5[i][j]) {
            std::cout << "At (" << i << ", " << j << "): "
                      << seq_output[i][j] << " != " << par_output_5[i][j] << std::endl;
            return false;
          }
          if (seq_output[i][j] != par_output_6[i][j]) {
            std::cout << "At (" << i << ", " << j << "): "
                      << seq_output[i][j] << " != " << par_output_6[i][j] << std::endl;
            return false;
          }
      }
  }

  return true;
}

bool test_updates_simple() {
  int n = 1000;
  matrix adjacencyMatrix = get_rand_graph_2(n, 0.1, 1234);

  std::vector<std::vector<long long>> upd_list = get_graph_updates(83, n, 10, 1, 100);
  std::cout << "Generated 10 graph updates: \n";
  for (int i = 0; i < 10; i++) {
    std::cout << "\tupdate " << i  << ": (" << upd_list[i][0] << "," << upd_list[i][1] << ") = " << upd_list[i][2] << "\n";
  }

  std::cout << "\n";
  std::cout << "Applying algorithm after each update:\n";
  parlay::internal::timer t;
  t.start();
  for (auto &upd : upd_list) {
    long long u;
    long long v;
    long long w;
    u = upd[0];
    v = upd[1];
    w = upd[2];

    adjacencyMatrix[u][v] = w;
    floyd_warshall_parallel_4(adjacencyMatrix);
  }
  std::cout << "\tTotal time elapsed: " << t.total_time() << " sec\n";
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

bool check_path(int i, int j, Path_Matrix &pm, const vi &path) {
  // std::cout << "Path from " << i << " to " << j << ": ";
  // for (size_t k = 0; k < path.size(); k++) {
  //   std::cout << path[k] << " ";
  // }
  // std::cout << std::endl;
  long long path_cost = 0;
  if(!path.empty()) {
    if(static_cast<int>(path.size()) != pm.l[i][j] + 1) {
      std::cout << "Path length mismatch at (" << i << ", " << j << "): "
                << path.size() << " != " << pm.l[i][j] + 1 << std::endl;
      return false;
    }
    for (size_t k = 0; k < path.size() - 1; k++) {
      path_cost += pm.adj[path[k]][path[k + 1]];
    }
    if (path_cost != pm.d[i][j]) {
      std::cout << "Cost mismatch when checking paths at (" << i << ", " << j << "): "
                << path_cost << " != " << pm.d[i][j] << std::endl;
      return false;
    }
  } else {
    if(pm.l[i][j] != -1) {
      std::cout << "no path found, but l implies path at (" << i << ", " << j << "): "
                << pm.l[i][j] << " != -1" << std::endl;
      return false;
    }
    if(pm.d[i][j] != MISSING_EDGE) {
      std::cout << "no path found, but d implies path at (" << i << ", " << j << "): "
                << pm.d[i][j] << " != " << MISSING_EDGE << std::endl;
      return false;
    }
  }
  return true;
}

bool test_paths_1() {
  Path_Matrix pm(basic_adj_mat_5);
  pm.init();
  pm.fw_par(false);
  const matrix &expected_dist = basic_adj_mat_5_expected;

  for (int i = 0; i < pm.n; i++) {
    for (int j = 0; j < pm.n; j++) {
      if (pm.d[i][j] != expected_dist[i][j]) {
        std::cout << "Cost mismatch when checking dist at (" << i << ", " << j << "): "
                  << pm.d[i][j] << " != " << expected_dist[i][j] << std::endl;
        return false;
      }
    }
  }

  // Check that shortest paths returned indeed have the lowest cost
  std::cout << "Checking get_path_seq...\n";
  for(int i=0; i<pm.n; i++) {
    for(int j=0; j<pm.n; j++) {
      if(!check_path(i, j, pm, pm.get_path_seq(i, j))) {
        return false;
      }
    }
  }
  std::cout << "Checking get_path_par...\n";
  for(int i=0; i<pm.n; i++) {
    for(int j=0; j<pm.n; j++) {
      if(!check_path(i, j, pm, pm.get_path_par(i, j))) {
        return false;
      }
    }
  }
  return true;
}

bool test_paths_2() {
  Path_Matrix pm(basic_adj_mat_5);
  pm.init();
  pm.fw_par(false);
  pm.update_edge_decrease(2, 3, 6); // Decrease 2-3 edge from 7 to 1
  // Check d
  const matrix &expected_dist = basic_adj_mat_5_2_expected;
  for (int i = 0; i < pm.n; i++) {
    for (int j = 0; j < pm.n; j++) {
      if (pm.d[i][j] != expected_dist[i][j]) {
        std::cout << "Cost mismatch when checking dist at (" << i << ", " << j << "): "
                  << pm.d[i][j] << " != " << expected_dist[i][j] << std::endl;
        return false;
      }
    }
  }
  // Check paths
  for(int i=0; i<pm.n; i++) {
    for(int j=0; j<pm.n; j++) {
      if(!check_path(i, j, pm, pm.get_path_par(i, j))) {
        return false;
      }
    }
  }
  return true;
}

bool test_paths_3() {
  matrix adjacencyMatrix = get_rand_graph_2(1000, 0.1, 1234);
  // Find an edge with a positive back-path to decrease:
  // Pick a random amount to decrease that edge by, avoiding negative cycles
  size_t i, j;
  for(i=0; i<adjacencyMatrix.size(); i++) {
    for(j=0; j<adjacencyMatrix[i].size(); j++) {
      if(adjacencyMatrix[i][j] != MISSING_EDGE && adjacencyMatrix[j][i] != MISSING_EDGE && adjacencyMatrix[i][j] + adjacencyMatrix[j][i] > 1) {
        break;
      }
    }
    if(j < adjacencyMatrix[i].size()) {
      break;
    }
  }
  // Decreasing by this amount makes the i-j cycle still positive
  long long dec_by = adjacencyMatrix[j][i] + adjacencyMatrix[i][j] - 1;
  // Apply the decrease manually and time Floyd-Warshall
  std::cout << "Applying a decrease, timing a complete Floyd Warshall\n";
  adjacencyMatrix[i][j] -= dec_by;
  floyd_warshall_parallel_4(adjacencyMatrix, true);
  // Reset completely
  adjacencyMatrix[i][j] += dec_by;
  // Apply the decrease via function and time the update
  Path_Matrix pm(adjacencyMatrix);
  pm.init();
  pm.fw_par(false);
  std::cout << "Timing update_edge_decrease\n";
  parlay::internal::timer t;
  t.start();
  pm.update_edge_decrease(static_cast<int>(i), static_cast<int>(j), dec_by);
  std::cout << "\tTime elapsed: " << t.total_time() << " sec\n";
  return true;
}

// std::vector<testing::TestCase> graph_builder_tests = {
//     {"Graph test: adjacency matrix to adjacency list converter", matrix_adj_list_converter},
// };

// std::vector<testing::TestCase> init_tests = {
//     {"Basic test: sequential floyd w/ 5x5 adjacency matrix.", test_basic_seq_1},
//     {"Basic test: sequential floyd w/ len 5 adjacency list.", test_basic_seq_2},
//     {"Basic test: sequential floyd w/ negative cycle.", test_basic_seq_neg_cycle},
//     {"Basic test: parallel floyd w/ 5x5 adjacency matrix.", test_basic_par_1},
//     {"Medium test: parallel floyd w/ 100x100 adjacency matrix.", test_basic_par_2}
// };

// std::vector<testing::TestCase> runtime_tests = {
//     {"Runtime test: floyd benchmarks.", test_runtime_par}
// };

// std::vector<testing::TestCase> updates_tests = {
//     {"Batching test: small batch.", test_updates_simple}
// };

std::vector<testing::TestCase> paths_tests = {
    {"Shortest-path-building test", test_paths_1},
    {"Decrease a single edge", test_paths_2},
    {"Time decreasing a single edge", test_paths_3}
};