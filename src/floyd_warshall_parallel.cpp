#include <iostream>
#include <climits>
#include <vector>

#include "graph_utils.hpp"
#include "floyd_warshall_parallel.hpp"

void floydWarshallParallel(int vertexCount, matrix& adjacencyMatrix) {

    parlay_matrix dist = make_parlay_matrix(vertexCount, vertexCount);
    
    parlay::parallel_for(0, vertexCount,
        [&](int i){
            parlay::parallel_for(0, vertexCount,
                [&](int j){
                    dist[i][j] = adjacencyMatrix[i][j];
                    if (i != j && dist[i][j] == 0) {
                        dist[i][j] = INT_MAX;
                    }
                });
    });

    for (int k = 0; k < vertexCount; k++) {
        parlay::parallel_for(0, vertexCount,
            [&](int i){
                parlay::parallel_for(0, vertexCount,
                    [&](int j){
                        if (dist[i][k] != MISSING_EDGE && dist[k][j] != MISSING_EDGE) {
                            if (dist[i][j] > dist[i][k] + dist[k][j]) {
                                dist[i][j] = dist[i][k] + dist[k][j];
                            }
                        }
                    });
        });
    }

    std::cout << "parallel run:\n";

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (dist[i][j] == MISSING_EDGE) {
                std::cout << "INF ";
            } else {
                std::cout << dist[i][j] << " ";
            }
        }
        std::cout << std::endl;
    }
}
