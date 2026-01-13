#ifndef CHRISTOFILES_ALGORITHM_H
#define CHRISTOFILES_ALGORITHM_H

#include "kruskal_algorithm.h"

typedef struct node {
    int vertex;
    int weight;
    struct node* next;
} node;

typedef struct node* linked_list;
typedef struct node* anode;

perf_graph_ptr isolation (perf_graph_ptr graph ,mst_ptr tree, int required[], int num);

void minimum_matching(perf_graph_ptr graph, mst_ptr tree);

anode make_node(int vertex, int weight);

linked_list insert_last(linked_list l, int vertex, int weight);

void print_ll(linked_list l);

void free_ll(linked_list ll);

void visit_edge(mst_ptr tree, int *transversal, int starting_vertex, linked_list* ll);

linked_list eulerian_tour(mst_ptr tree, int require[]);

void tsp_tour(perf_graph_ptr graph, linked_list ll);

linked_list christofides(perf_graph_ptr graph, mst_ptr tree, int require[], int num);

#endif