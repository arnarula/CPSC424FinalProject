#pragma once

#include "parlay/primitives.h"
#include "parlay/parallel.h"
#include "parlay/sequence.h"
#include "parlay/utilities.h"

#include "graph_utils.hpp"

void floydWarshallParallel(int vertexCount, matrix& adjacencyMatrix);
