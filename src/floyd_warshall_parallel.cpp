#include <iostream>
#include <climits>
#include <vector>

#include "parlay/primitives.h"
#include "parlay/parallel.h"
#include "parlay/sequence.h"
#include "parlay/utilities.h"

#include "utils.hpp"

using matrix = parlay::sequence<parlay::sequence<int>>;

void floydWarshall(int vertexCount, matrix& adjacencyMatrix) {

    parlay::sequence<parlay::sequence<int>> dist(vertexCount, parlay::sequence<int>(vertexCount));
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
                        if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX) {
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
            if (dist[i][j] == INT_MAX) {
                std::cout << "INF ";
            } else {
                std::cout << dist[i][j] << " ";
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    int vertexCount = 5;
    // matrix adjacencyMatrix = {
    //     {0, 3, 0, 6, 0},
    //     {3, 0, 1, 0, 2},
    //     {0, 1, 0, 7, 2},
    //     {6, 0, 7, 0, 3},
    //     {0, 2, 2, 3, 0}
    // };
    matrix adjacencyMatrix = get_rand_graph(5, 0.1, 83);
    floydWarshall(vertexCount, adjacencyMatrix);
    return 0;
}