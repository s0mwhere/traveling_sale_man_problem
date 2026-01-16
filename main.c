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
    int require[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};       //must visit node

    load_Graph(G, "raw_graph.txt");         //load info in txt into graph

    A_Star(G, require, 13, "A_star.txt");    //get the input for Christofide in form of txt

    perf_graph_ptr graph = load_perf_graph("A_star.txt");

    mst_ptr tree = kruskal(graph, require);

    linked_list ll = christofides(graph, tree, require, 13);

    print_ll(ll);

    free_ll(ll);

    return 0;
}