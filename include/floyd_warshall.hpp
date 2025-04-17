#pragma once

#include "graph_utils.hpp"

matrix floyd_warshall(matrix& adjacencyMatrix);
matrix floyd_warshall(adj_list& adjacencyList);