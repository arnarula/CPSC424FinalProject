#pragma once

#include <limits>

#include "parlay/primitives.h"
#include "parlay/parallel.h"
#include "parlay/sequence.h"
#include "parlay/utilities.h"

using vec = std::vector<long long>;
using parlay_vec = parlay::sequence<long long>;

using matrix = std::vector<std::vector<long long>>;
using parlay_matrix = parlay::sequence<parlay::sequence<long long>>;

inline matrix make_matrix(int r, int c) {
    return std::vector<std::vector<long long>>(r, std::vector<long long>(c));
}

inline parlay_matrix make_parlay_matrix(int r, int c) {
    return parlay::sequence<parlay::sequence<long long>>(r, parlay::sequence<long long>(c));
}

#define MISSING_EDGE std::numeric_limits<long long>::max()

parlay_matrix get_rand_graph(const int n, const double p, const unsigned long seed);
    