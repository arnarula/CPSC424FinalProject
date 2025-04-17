#pragma once

#include "parlay/primitives.h"
#include "parlay/parallel.h"
#include "parlay/sequence.h"
#include "parlay/utilities.h"

#include "graph_utils.hpp"

matrix floyd_warshall_parallel(matrix& adjacencyMatrix, bool timed = false);
// matrix floyd_warshall_parallel(adj_list& adjacencyList);
