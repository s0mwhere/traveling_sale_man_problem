#include <stdio.h>
#include <stdlib.h>
#include "read_txt.h"
#include "graph.h"
#include "priority_queue.h"

int main(){
    int N = 13;                             //max number of node in the graph
    AdjList* G = create_graph(N);
    int require[] = {0,3,5,7};              //must visit node

    load_Graph(G, "raw_graph.txt");         //load info in txt into graph

    A_Star(G, require, 4, "A_star.txt");    //get the input for Christofide in form of txt

    return 0;
}