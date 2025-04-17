#pragma once

#include "graph_utils.hpp"

matrix floyd_warshall(matrix& adjacencyMatrix, bool timed = false);
matrix floyd_warshall(adj_list& adjacencyList, bool timed = false);

bool has_negative_cycle(const matrix& dist);