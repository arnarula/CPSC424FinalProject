#include <iostream>
#include <climits>
#include <vector>

using matrix = std::vector<std::vector<int>>;

void floydWarshall(int vertexCount, matrix& adjacencyMatrix) {

    int dist[vertexCount][vertexCount];
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            dist[i][j] = adjacencyMatrix[i][j];
            if (i != j && dist[i][j] == 0) {
                dist[i][j] = INT_MAX;
            }
        }
    }

    for (int k = 0; k < vertexCount; k++) {
        for (int i = 0; i < vertexCount; i++) {
            for (int j = 0; j < vertexCount; j++) {
                if (dist[i][k] == INT_MAX || dist[k][j] == INT_MAX) {
                    continue;
                }
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    std::cout << "sequential run:\n";
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
    matrix adjacencyMatrix = {
        {0, 3, 0, 6, 0},
        {3, 0, 1, 0, 2},
        {0, 1, 0, 7, 2},
        {6, 0, 7, 0, 3},
        {0, 2, 2, 3, 0}
    };
    floydWarshall(vertexCount, adjacencyMatrix);
    return 0;
}