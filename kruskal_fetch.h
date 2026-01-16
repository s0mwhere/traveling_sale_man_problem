#ifndef KRUSKAL_FETCH_H
#define KRUSKAL_FETCH_H

typedef struct {
    int point1;
    int point2;
    int weight;
} connection;

typedef struct {
    connection* connection; 
    int num_vertices;
    int num_edges; 
} perf_graph;

typedef perf_graph* perf_graph_ptr;

perf_graph_ptr load_perf_graph(const char* filename);

void perf_graph_transversal(perf_graph_ptr graph);

void free_graph(perf_graph_ptr graph);

#endif