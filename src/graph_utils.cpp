#include <random>

#include "graph_utils.hpp"

// will keep random graph as seq of seq for now but may change it to just one long seq<int>
// shamelessly stolen from here: https://github.com/moorejs/APSP-in-parallel/blob/master/src/floyd_warshall.cpp
parlay_matrix get_rand_graph(const int n, const double p, const unsigned long seed) {
    static std::uniform_real_distribution<double> flip(0,1);
    static std::uniform_int_distribution<int> choose_weight(1, 100);

    std::mt19937_64 rand_engine(seed);

    parlay_matrix graph = make_parlay_matrix(n, n);
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

matrix get_rand_graph_2(const int n, const double p, const unsigned long seed) {
    static std::uniform_real_distribution<double> flip(0,1);
    static std::uniform_int_distribution<int> choose_weight(1, 100);

    std::mt19937_64 rand_engine(seed);

    matrix graph = make_matrix(n, n);
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

std::vector<std::vector<long long>> get_graph_updates(
    long long seed, 
    long long n_vtx, 
    long long n_upd, 
    long long minw, 
    long long maxw
) {
    std::mt19937_64 rand_engine(seed);
    static std::uniform_int_distribution<int> get_vertex(0, n_vtx);
    static std::uniform_int_distribution<int> get_weight(minw, maxw+1);


    std::vector<std::vector<long long>> out(n_upd, std::vector<long long>(3));

    for (auto &e : out) {
       e[0] = get_vertex(rand_engine); 
       e[1] = get_vertex(rand_engine); 
       e[2] = get_weight(rand_engine); 
       if (e[2] == maxw) {
           e[2] = MISSING_EDGE; 
       }
    }

    return out;
}