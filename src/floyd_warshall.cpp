#include <iostream>
#include <climits>
#include <vector>

#include "floyd_warshall.hpp"

matrix floydWarshall(int vertexCount, matrix &adjacencyMatrix)
{
  matrix dist = make_matrix(vertexCount, vertexCount);

  for (int i = 0; i < vertexCount; i++) {
    for (int j = 0; j < vertexCount; j++) {
      dist[i][j] = MISSING_EDGE;
      
      if (i != j && adjacencyMatrix[i][j]) { 
        // TODO: allow edges of weight zero
        dist[i][j] = adjacencyMatrix[i][j];
      }
    }
  }

  for (int k = 0; k < vertexCount; k++) {
    for (int i = 0; i < vertexCount; i++) {
      for (int j = 0; j < vertexCount; j++) {
        if (dist[i][k] == MISSING_EDGE || dist[k][j] == MISSING_EDGE) {
          continue;
        }
        if (dist[i][j] > dist[i][k] + dist[k][j]) {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }
  return dist;
}