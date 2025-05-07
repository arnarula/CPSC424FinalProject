#pragma once

#include "graph_utils.hpp"

typedef std::vector<int> vi;
typedef std::vector<vi> vvi;
typedef std::vector<bool> vb;

// Assumes no negative cycles -- ever
struct Path_Matrix
{
  // Raw adjacency matrix
  matrix adj;
  // Shortest distance matrix
  matrix d;
  // Matrix to compute shortest-paths (linked to d)
  vvi parent;
  // Length (in edges) of lowest-cost paths
  vvi l;
  const int n;

  Path_Matrix(int n);
  Path_Matrix(const matrix &m);
  void init();
  void fw_par(bool time);
  vi get_path(int i, int j, bool par);
  void fill_path(vi &seq, int ind, int i, int j, bool par);
  void update_edge_decrease(int i, int j, long long dec_by);
  void update_edge_increase(int i, int j, long long inc_by);
  void get_fast_tails(vb &table, int target, int j, bool par);
  void get_fast_heads(vb &table, int target, int j, bool par);
};