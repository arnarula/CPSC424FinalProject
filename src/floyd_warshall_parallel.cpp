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

// iteration 4 - same as iteration 2 but more optimized (inner loop operations -> outer loop)
matrix floyd_warshall_parallel_4(matrix& adjacencyMatrix, bool timed) {

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
                auto& dist_i = dist[i];
                auto& dist_ik = dist_i[k];
                if (dist_ik == MISSING_EDGE)
                    return;

                parlay::parallel_for(0, n,
                    [&](int j) {
                        auto& dist_kj = dist_k[j];
                        if (dist_kj == MISSING_EDGE)
                            return;
                        
                        auto total_dist = dist_ik + dist_kj;
                        if (dist_i[j] > total_dist) {
                            dist_i[j] = total_dist;
                        }
                    });
        });
    }

    if (timed)
        std::cout << "Parallel floyd (iter 4) time taken: " << t.total_time() << " seconds" << std::endl;

    // convert parlay_matrix back to matrix
    matrix result = convert_to_matrix(dist);
    return result;
}

// iteration 5 - using blocking to take advantage of cache locality
matrix floyd_warshall_parallel_5(matrix& adjacencyMatrix, bool timed) {
    parlay_matrix dist = copy_to_parlay_matrix(adjacencyMatrix);
    int n = dist.size();

    parlay::internal::timer t;
    t.start();

    parlay::parallel_for(0, n, [&](int i) {
        if (dist[i][i] > 0) dist[i][i] = 0;
    });

    // Blocking parameters
    int block_size = 32;
    int num_blocks = (n + block_size - 1) / block_size;

    for (int k = 0; k < n; ++k) {
        auto& dist_k = dist[k];

        parlay::parallel_for(0, num_blocks, [&](int bi) {
            int i_start = bi * block_size;
            int i_end = std::min(i_start + block_size, n);

            for (int i = i_start; i < i_end; ++i) {
                auto& dist_i = dist[i];
                auto& dist_ik = dist_i[k];
                if (dist_ik == MISSING_EDGE) continue;

                parlay::parallel_for(0, num_blocks, [&](int bj) {
                    int j_start = bj * block_size;
                    int j_end = std::min(j_start + block_size, n);

                    for (int j = j_start; j < j_end; ++j) {
                        auto& dist_kj = dist_k[j];
                        if (dist_kj == MISSING_EDGE) continue;

                        auto new_dist = dist_ik + dist_kj;
                        if (dist_i[j] > new_dist) {
                            dist_i[j] = new_dist;
                        }
                    }
                });
            }
        });
    }

    if (timed)
        std::cout << "Parallel floyd (iter 5) Floyd-Warshall time: " << t.total_time() << " sec\n";

    return convert_to_matrix(dist);
}

matrix floyd_warshall_parallel_6(matrix& adjacencyMatrix, bool timed) {
    
    parlay_matrix dist = copy_to_parlay_matrix(adjacencyMatrix);
    int n = dist.size();
    parlay_vec dist_vec = parlay::flatten(dist);

    parlay::internal::timer t;
    t.start();

    // init: min distance from a vertex to itself is 0
    parlay::parallel_for(0, n,
        [&](int i) {
            if (dist_vec[i * n + i] > 0) {
                dist_vec[i * n + i] = 0; 
            }
        });

    for (int k = 0; k < n; k++) {
        parlay::parallel_for(0, n,
            [&](int i){
                auto& dist_ik = dist_vec[i * n + k];
                if (dist_ik == MISSING_EDGE)
                    return;

                parlay::parallel_for(0, n,
                    [&](int j) {
                        auto& dist_kj = dist_vec[k * n + j];
                        if (dist_kj == MISSING_EDGE)
                            return;
        
                        auto total_dist = dist_ik + dist_kj;
                        if (dist_vec[i * n + j] > total_dist) {
                            dist_vec[i * n + j] = total_dist;
                        }
                    });
        });
    }

    if (timed)
        std::cout << "Parallel floyd (iter 6) time taken: " << t.total_time() << " seconds" << std::endl;

    // convert parlay_matrix back to matrix
    parlay_matrix dist2 = make_parlay_matrix(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dist2[i][j] = dist_vec[i * n + j];
        }
    } 
    matrix result = convert_to_matrix(dist2);
    return result;
}
