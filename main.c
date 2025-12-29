#include <stdio.h>
#include <stdlib.h>
#include "read_txt.h"
#include "graph.h"

int main(){
    int N = 6;
    AdjList* G = create_graph(N);
    load_Graph(G, "input.txt");

    ShowEdges(G, 2);

    return 0;
}