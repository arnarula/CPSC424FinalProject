#include <random>

#include "graph_utils.hpp"

/*
Goals:
    1. Random Graph Generation
    2. Updates and batching?
*/

// will keep random graph as seq of seq for now but may change it to just one long seq<int>
// shamelessly stolen from here: https://github.com/moorejs/APSP-in-parallel/blob/master/src/floyd_warshall.cpp
parlay_matrix get_rand_graph(const int n, const double p, const unsigned long seed) {
    static std::uniform_real_distribution<double> flip(0,1);
    static std::uniform_int_distribution<int> choose_weight(1, 100);

    std::mt19937_64 rand_engine(seed);

    parlay_matrix graph(n, parlay::sequence<long long>(n));
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            if (i == j) {
                graph[i][j] = 0;
            } else if (flip(rand_engine) < p) {
                int w = choose_weight(rand_engine);
                graph[i][j] = w;
                graph[j][i] = w;
            } else {
                graph[i][j] = MISSING_EDGE;
                graph[j][i] = MISSING_EDGE;
            }
        }
    }

    return graph;
}