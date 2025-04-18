#include <iostream>
#include <climits>
#include <vector>

#include "floyd_warshall.hpp"

// input = adjacency matrix
matrix floyd_warshall(matrix& adjacencyMatrix, bool timed)
{
    matrix dist = copy_to_matrix(adjacencyMatrix);
    int n = dist.size(); // vertex count

    parlay::internal::timer t;
    t.start();
    
    // init: min distance from a vertex to itself is 0
    for (int i = 0; i < n; i++) {
        if (dist[i][i] > 0) {
            dist[i][i] = 0; 
        }
    }
 
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] == MISSING_EDGE || dist[k][j] == MISSING_EDGE) {
                    continue;
                }
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
    
    if (timed)
        std::cout << "Sequential floyd time taken: " << t.total_time() << " seconds" << std::endl;
    return dist;
}

// input = vertex count + adjacency list
matrix floyd_warshall(adj_list& adjacencyList, bool timed)
{
    matrix dist = copy_to_matrix(adjacencyList);
    parlay::internal::timer t;
    int n = dist.size(); // vertex count

    // init: min distance from a vertex to itself is 0
    for (int i = 0; i < n; i++) {
        if (dist[i][i] > 0) {
            dist[i][i] = 0; 
        }
    }

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] == MISSING_EDGE || dist[k][j] == MISSING_EDGE) {
                    continue;
                }
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    if (timed)
        std::cout << "Sequential floyd time taken: " << t.total_time() << " seconds" << std::endl;
    return dist;
}

// check floyd warshall's output for negative cycles
bool has_negative_cycle(const matrix& dist) {
    for (size_t i = 0; i < dist.size(); i++) {
        if (dist[i][i] < 0) {
            return true;
        }
    }
    return false;
}
