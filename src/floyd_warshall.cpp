#include <iostream>
#include <climits>
#include <vector>

using matrix = std::vector<std::vector<int>>;

matrix floydWarshall(int vertexCount, matrix &adjacencyMatrix)
{
  matrix dist;
  dist.resize(vertexCount);
  for (int i = 0; i < vertexCount; i++)
  {
    dist[i].resize(vertexCount);
    for (int j = 0; j < vertexCount; j++)
    {
      dist[i][j] = INT_MAX;
      if (i == j)
      {
        dist[i][j] = 0;
      }
      else if (adjacencyMatrix[i][j])
      { // TODO: allow edges of weight zero
        dist[i][j] = adjacencyMatrix[i][j];
      }
    }
  }

  for (int k = 0; k < vertexCount; k++)
  {
    for (int i = 0; i < vertexCount; i++)
    {
      for (int j = 0; j < vertexCount; j++)
      {
        if (dist[i][k] == INT_MAX || dist[k][j] == INT_MAX)
        {
          continue;
        }
        if (dist[i][j] > dist[i][k] + dist[k][j])
        {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }
  return dist;
}