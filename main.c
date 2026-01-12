#include <stdio.h>
#include <stdlib.h>

#include "read_txt.h"
#include "graph.h"
#include "priority_queue.h"
#include "kruskal_fetch.h"
#include "kruskal_algorithm.h"
#include "christofiles_algorithm.h"

int main(){
    int N = 13;                             //max number of node in the graph
    AdjList* G = create_graph(N);
    int require[] = {0,1,2,3,5,6,7,8,9,10};       //must visit node

    load_Graph(G, "raw_graph.txt");         //load info in txt into graph

    A_Star(G, require, 10, "A_star.txt");    //get the input for Christofide in form of txt

    perf_graph_ptr graph = load_perf_graph("A_star.txt");

    mst_ptr tree = kruskal(graph, require);
    
    perf_graph_ptr odd_vertices = isolation(graph,tree,require,10);

    minimum_matching(odd_vertices, tree);

    linked_list ll = eulerian_tour(tree, require);

    print_ll(ll);
    printf("\n");

    tsp_tour(graph, ll);

    print_ll(ll);

    free_mst(tree);
    free_graph(graph);
    free_graph(odd_vertices);
    free_ll(ll);

    return 0;
}