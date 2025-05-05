#pragma once

#include <limits>

#include "parlay/primitives.h"
#include "parlay/parallel.h"
#include "parlay/sequence.h"
#include "parlay/utilities.h"

#define MISSING_EDGE (std::numeric_limits<long long>::max())

using vec = std::vector<long long>;
using parlay_vec = parlay::sequence<long long>;

using matrix = std::vector<std::vector<long long>>; 
using parlay_matrix = parlay::sequence<parlay::sequence<long long>>;

using adj_list = std::vector<std::vector<std::pair<int, long long>>>;

inline matrix make_matrix(int r, int c) {
    return std::vector<std::vector<long long>>(r, std::vector<long long>(c));
}

inline matrix copy_to_matrix(const matrix& m) {
    matrix copy(m.size(), std::vector<long long>(m[0].size()));
    for (size_t i = 0; i < m.size(); i++) {
        for (size_t j = 0; j < m[i].size(); j++) {
            copy[i][j] = m[i][j];
        }
    }
    return copy;
}

inline matrix convert_to_matrix(const parlay_matrix& m) {
    matrix copy(m.size(), std::vector<long long>(m[0].size()));
    for (size_t i = 0; i < m.size(); i++) {
        for (size_t j = 0; j < m[i].size(); j++) {
            copy[i][j] = m[i][j];
        }
    }
    return copy;
}

inline matrix copy_to_matrix(const adj_list& m) {
    int n = m.size();
    matrix copy(n, std::vector<long long>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            copy[i][j] = MISSING_EDGE;
        }
    }
    for (int i = 0; i < n; i++) {
        for (const auto& [j, w] : m[i]) {
            copy[i][j] = w;
            copy[j][i] = w;
        }
    }
    return copy;
}

inline parlay_matrix make_parlay_matrix(int r, int c) {
    return parlay::sequence<parlay::sequence<long long>>(r, parlay::sequence<long long>(c));
}

inline parlay_matrix copy_to_parlay_matrix(const matrix& m) {
    parlay_matrix copy(m.size(), parlay::sequence<long long>(m[0].size()));
    for (size_t i = 0; i < m.size(); i++) {
        for (size_t j = 0; j < m[i].size(); j++) {
            copy[i][j] = m[i][j];
        }
    }
    return copy;
}

inline parlay_matrix copy_to_parlay_matrix(const adj_list& m) {
    int n = m.size();
    parlay_matrix copy(n, parlay::sequence<long long>(n));
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            copy[i][j] = MISSING_EDGE;
        }
    }
    for (int i = 0; i < n; i++) {
        for (const auto& [j, w] : m[i]) {
            copy[i][j] = w;
            copy[j][i] = w;
        }
    }
    return copy;
}

inline adj_list matrix_to_adj_list(const matrix& m) {
    int n = m.size();
    adj_list adjacencyList(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (m[i][j] != MISSING_EDGE) {
                adjacencyList[i].emplace_back(j, m[i][j]);
            }
        }
    }
    return adjacencyList;
}

inline matrix adj_list_to_matrix(const adj_list& adjacencyList) {
    int n = adjacencyList.size();
    matrix adjacencyMatrix(n, std::vector<long long>(n, MISSING_EDGE));
    for (int i = 0; i < n; i++) {
        for (const auto& [j, w] : adjacencyList[i]) {
            adjacencyMatrix[i][j] = w;
            adjacencyMatrix[j][i] = w;
        }
    }
    return adjacencyMatrix;
}

parlay_matrix get_rand_graph(const int n, const double p, const unsigned long seed);
matrix get_rand_graph_2(const int n, const double p, const unsigned long seed);

std::vector<std::vector<long long>> get_graph_updates(long long seed, long long n_vtx, long long n_upd, long long minw, long long maxw);