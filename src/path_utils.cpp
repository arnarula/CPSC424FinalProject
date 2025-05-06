#include "path_utils.hpp"
#include "parlay/internal/get_time.h"

Path_Matrix::Path_Matrix(int n) : n(d.size())
{
  adj = make_matrix(n, n);
  parent = std::vector<std::vector<int>>(n, std::vector<int>(n, -1));
  l = std::vector<std::vector<int>>(n, std::vector<int>(n, -1));
}
Path_Matrix::Path_Matrix(const matrix &m) : n(m.size())
{
  adj = d = m;                                                            // Copies
  parent = l = std::vector<std::vector<int>>(n, std::vector<int>(n, -1)); // Copies
}
// Sets diagonal of adjacency matrix to 0
// Copies adjacency matrix to d
// Fills parent matrix with -1
// Fills short-path-length matrix
void Path_Matrix::init()
{
  parlay::parallel_for(0, n,
                       [&](int i)
                       {
                         d[i][i] = adj[i][i] = 0;
                       });
  parlay::parallel_for(0, n,
                       [&](int i)
                       {
                         //  parlay::parallel_for(0, n, [&](int j)
                         //                       { d[i][j] = adj[i][j]; });
                         //  parlay::parallel_for(0, n, [&](int j)
                         //                       { parent[i][j] = -1; });
                         parlay::parallel_for(0, n, [&](int j)
                                              {
                           if (i == j)
                           {
                             l[i][j] = 0;
                           }
                           else if (adj[i][j] < MISSING_EDGE)
                           {
                             l[i][j] = 1;
                           }
                           else
                           {
                             l[i][j] = -1;
                           } });
                       });
}
// Run parallel Floyd-Warshall on d
// Keep parent and l matrices up to date
// - Using iteration 4
void Path_Matrix::fw_par(bool time)
{
  parlay_matrix dist = copy_to_parlay_matrix(d);
  int n = dist.size(); // vertex count

  parlay::internal::timer t;
  if (time)
    t.start();

  for (int k = 0; k < n; k++)
  {
    auto &dist_k = dist[k];
    parlay::parallel_for(0, n,
                         [&](int i)
                         {
                           auto &dist_i = dist[i];
                           auto &dist_ik = dist_i[k];
                           if (dist_ik == MISSING_EDGE)
                             return;

                           parlay::parallel_for(0, n,
                                                [&](int j)
                                                {
                                                  auto &dist_kj = dist_k[j];
                                                  if (dist_kj == MISSING_EDGE)
                                                    return;

                                                  auto total_dist = dist_ik + dist_kj;
                                                  if (dist_i[j] > total_dist)
                                                  {
                                                    if (i == k || j == k)
                                                    {
                                                      std::cerr << "Error: i == k or j == k" << std::endl;
                                                      std::cerr << "i: " << i << ", j: " << j << ", k: " << k << std::endl;
                                                    }
                                                    parent[i][j] = k;
                                                    l[i][j] = l[i][k] + l[k][j];
                                                    dist_i[j] = total_dist;
                                                  }
                                                });
                         });
  }
  if (time)
    std::cout << "Parallel floyd (iter 4) time taken: " << t.total_time() << " seconds" << std::endl;

  // convert parlay_matrix back to matrix
  d = convert_to_matrix(dist);
}
// Sequential version of get_fast_path
// Returns as vector of int vertices a lowest-cost path from i to j
vi Path_Matrix::get_path_seq(int i, int j)
{
  if (d[i][j] == MISSING_EDGE)
  {
    // std:: cout << "No path exists from " << i << " to " << j << std::endl;
    return {}; // No path exists
  }
  if (i == j)
  {
    return {i};
  }
  if (parent[i][j] < 0)
  {
    return {i, j};
  }
  vi p1 = get_path_seq(i, parent[i][j]);
  vi p2 = get_path_seq(parent[i][j], j);
  p1.pop_back();
  p1.insert(p1.end(), p2.begin(), p2.end());
  return p1;
}
// Parallel version of get_fast_path
// Returns as vector of int vertices a lowest-cost path from i to j
vi Path_Matrix::get_path_par(int i, int j)
{
  if (d[i][j] == MISSING_EDGE)
  {
    // std:: cout << "No path exists from " << i << " to " << j << std::endl;
    return {}; // No path exists
  }
  if (i == j)
  {
    return {i};
  }
  if (parent[i][j] < 0)
  {
    return {i, j};
  }
  vi p1, p2;
  parlay::par_do(
      [&]()
      {
        p1 = get_path_par(i, parent[i][j]);
      },
      [&]()
      {
        p2 = get_path_par(parent[i][j], j);
      });
  p1.pop_back();
  p1.insert(p1.end(), p2.begin(), p2.end());
  return p1;
}
// Important: assumes negative cycles are never formed!
// Also assumes the edge being updated exists
void Path_Matrix::update_edge_decrease(int i, int j, long long dec_by)
{
  adj[i][j] -= dec_by;
  parlay::parallel_for(0, n, [&](int i2) { 
    parlay::parallel_for(0, n, [&](int j2) {
      // i2..i->j..j2 isn't a path
      if(d[i2][i] != MISSING_EDGE && d[j][j2] != MISSING_EDGE) {
        // No negative cycles means no dependencies
        long long new_dist = d[i2][i] + adj[i][j] + d[j][j2];
        if(new_dist < d[i2][j2]) {
          d[i2][j2] = new_dist;
          l[i2][j2] = l[i2][i] + 1 + l[j][j2]; // correct for (i2,j2) == (i,j)
          if(i2 != i) {
            parent[i2][j2] = i;
          } else if(j2 != j) {
            parent[i2][j2] = j;
          } else {
            parent[i2][j2] = -1;
          }
        }
      } }); });
}
void Path_Matrix::update_edge_increase(int i, int j, long long inc_by)
{
  adj[i][j] += inc_by;
  if (parent[i][j] < 0)
  {
    // i->j used to be fastest -- now it *might* not be.
    // I don't see a way around just running Floyd-Warshall again.
    // Even if i->j is still fastest, the fastest path i' to j' that
    // used to pass through i->j just might not anymore.
    fw_par(false);
  }
  else
  {
    // i->j was never fastest -- no shortest paths change
  }
}
// At one level: marks entries in table corresponding to vertices for which parent-determined
// fastest paths to j pass through target.
// Initial call should be get_fast_tails_par(table, i, j).
// At that level: markes entries in table corresponding to v s.t. shortest path from v to j
// ends with the edge i->j and v is the "earliest" possible.
void Path_Matrix::get_fast_tails(vb &table, int target, int j, bool par)
{
  // TODO: check for race conditions
  if (par)
  {
    vi found;
    for (int v = 0; v < n; v++)
    {
      if (parent[v][j] == target)
      {
        found.push_back(v);
      }
    }
    if (found.empty())
    {
      table[j] = true;
    }
    else
    {
      parlay::parallel_for(0, found.size(),
                           [&](int i)
                           {
                             get_fast_tails(table, found[i], j, true);
                           });
    }
  }
  else
  {
    bool found_any = false;
    for (int v = 0; v < n; v++)
    {
      if (parent[v][j] == target)
      {
        get_fast_tails(table, v, j, false);
        found_any = true;
      }
    }
    if (!found_any)
    {
      table[j] = true;
    }
  }
}
// Marks entries in table corresponding to vertices for which parent-determined
// fastest paths from i pass through target.
// Initial call should be get_fast_heads_seq(table, i, j)
void Path_Matrix::get_fast_heads(vb &table, int i, int target, bool par)
{
  // TODO: check for race conditions
  if (par)
  {
    vi found;
    for (int v = 0; v < n; v++)
    {
      if (parent[i][v] == target)
      {
        found.push_back(v);
      }
    }
    if (found.empty())
    {
      table[i] = true;
    }
    else
    {
      parlay::parallel_for(0, found.size(),
                           [&](int j_)
                           {
                             get_fast_heads(table, i, found[j_], true);
                           });
    }
  }
  else
  {
    bool found_any = false;
    for (int v = 0; v < n; v++)
    {
      if (parent[i][v] == target)
      {
        get_fast_heads(table, i, v, false);
        found_any = true;
      }
    }
    if (!found_any)
    {
      table[i] = true;
    }
  }
}