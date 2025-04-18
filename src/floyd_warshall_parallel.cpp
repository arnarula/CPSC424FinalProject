#include <iostream>
#include <climits>
#include <vector>

#include "parlay/internal/get_time.h"

#include "graph_utils.hpp"
#include "floyd_warshall_parallel.hpp"

// iteration 1
matrix floyd_warshall_parallel(matrix& adjacencyMatrix, bool timed) {
    
    parlay_matrix dist = copy_to_parlay_matrix(adjacencyMatrix);
    int n = dist.size(); // vertex count

    parlay::internal::timer t;
    t.start();

    // init: min distance from a vertex to itself is 0
    parlay::parallel_for(0, n,
        [&](int i) {
            if (dist[i][i] > 0) {
                dist[i][i] = 0; 
            }
        });

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
        std::cout << "Parallel floyd (iter 1) time taken: " << t.total_time() << " seconds" << std::endl;

    // convert parlay_matrix back to matrix
    matrix result = convert_to_matrix(dist);
    return result;
}

// iteration 2 - caching repeated accesses
matrix floyd_warshall_parallel_2(matrix& adjacencyMatrix, bool timed) {

    parlay_matrix dist = copy_to_parlay_matrix(adjacencyMatrix);
    int n = dist.size(); // vertex count

    parlay::internal::timer t;
    t.start();

    // init: min distance from a vertex to itself is 0
    parlay::parallel_for(0, n,
        [&](int i) {
            if (dist[i][i] > 0) {
                dist[i][i] = 0; 
            }
        });

    for (int k = 0; k < n; k++) {
        auto& dist_k = dist[k];
        parlay::parallel_for(0, n,
            [&](int i){
                parlay::parallel_for(0, n,
                    [&](int j) {
                        auto& dist_i = dist[i];
                        auto& dist_ik = dist_i[k];
                        auto& dist_kj = dist_k[j];

                        if (dist_ik == MISSING_EDGE || dist_kj == MISSING_EDGE)
                            return;
                        
                        if (dist_i[j] > dist_ik + dist_kj) {
                            dist_i[j] = dist_ik + dist_kj;
                        }
                    });
        });
    }

    if (timed)
        std::cout << "Parallel floyd (iter 2) time taken: " << t.total_time() << " seconds" << std::endl;

    // convert parlay_matrix back to matrix
    matrix result = convert_to_matrix(dist);
    return result;
}

// iteration 3 - caching + inner loop unrolling
matrix floyd_warshall_parallel_3(matrix& adjacencyMatrix, bool timed) {
    
    parlay_matrix dist = copy_to_parlay_matrix(adjacencyMatrix);
    int n = dist.size(); // vertex count

    parlay::internal::timer t;
    t.start();

    // init: min distance from a vertex to itself is 0
    parlay::parallel_for(0, n,
        [&](int i) {
            if (dist[i][i] > 0) {
                dist[i][i] = 0; 
            }
        });

    for (int k = 0; k < n; k++) {
        auto& dist_k = dist[k];
        parlay::parallel_for(0, n * n, [&](int idx) {
            int i = idx / n;
            int j = idx % n;

            auto& dist_i = dist[i];
            auto& dist_ik = dist_i[k];
            auto& dist_kj = dist_k[j];

            if (dist_ik == MISSING_EDGE || dist_kj == MISSING_EDGE)
                return;

            if (dist_i[j] > dist_ik + dist_kj) {
                dist_i[j] = dist_ik + dist_kj;
            }
        });
    }

    if (timed)
        std::cout << "Parallel floyd (iter 3) time taken: " << t.total_time() << " seconds" << std::endl;

    // convert parlay_matrix back to matrix
    matrix result = convert_to_matrix(dist);
    return result;
}
