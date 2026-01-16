#ifndef KRUSKAL_ALGORITHM_H
#define KRUSKAL_ALGORITHM_H

#include "kruskal_fetch.h"

typedef struct {
    connection* connection;
    int size;
} mst;
typedef mst* mst_ptr;

void connection_swap(perf_graph_ptr perf_graph, int j, int k);

void pair_sort(perf_graph_ptr perf_graph, int b, int c);

int find(int parent[], int point);

mst_ptr kruskal(perf_graph_ptr perf_graph, int required[]);

void tree_transversal(mst_ptr tree);

void free_mst(mst_ptr tree);

#endif