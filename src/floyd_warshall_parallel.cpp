#include <iostream>
#include <climits>
#include <vector>

#include "parlay/internal/get_time.h"

#include "graph_utils.hpp"
#include "floyd_warshall_parallel.hpp"

matrix floyd_warshall_parallel(matrix& adjacencyMatrix, bool timed) {
    
    parlay_matrix dist = copy_to_parlay_matrix(adjacencyMatrix);
    int n = dist.size(); // vertex count

    parlay::internal::timer t;
    // init: min distance from a vertex to itself is 0
    for (int i = 0; i < n; i++) {
        if (dist[i][i] > 0) {
            dist[i][i] = 0; 
        }
    }

    for (int k = 0; k < n; k++) {
        parlay::parallel_for(0, n,
            [&](int i){
                parlay::parallel_for(0, n,
                    [&](int j) {
                        if (dist[i][k] == MISSING_EDGE || dist[k][j] == MISSING_EDGE)
                            return;

                        if (dist[i][j] > dist[i][k] + dist[k][j]) {
                            dist[i][j] = dist[i][k] + dist[k][j];
                        }
                    });
        });
    }

    if (timed)
        std::cout << "Parallel floyd time taken: " << t.total_time() << " seconds" << std::endl;

    // convert parlay_matrix back to matrix
    matrix result = convert_to_matrix(dist);
    return result;
}
