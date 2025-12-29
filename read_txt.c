#include <stdio.h>
#include "read_txt.h"
#include "graph.h"

/*---example input.txt---
total_node
node  x  y   (node index and position x and y)
node  x  y
...
node  con_node   (2 connected node, the connection is 2 way so no need to do the reverse)
node  con_node
...
*/

void load_Graph(AdjList* G, const char* filename){
    int node;      
    double x;
    double y;
    int total_node;
    int con_node;

    FILE* f = fopen(filename, "r");
    if (!f) printf("WE DONE GOOF");
    fscanf(f, "%d", &total_node);                   //get the number of node to scan their position data
    for(int i = 1; i < total_node+1; i++){
        fscanf(f, "%d %lf %lf", &node, &x, &y);
        add_position(G, node, x, y);
        //printf("%d  %.2f  %.2f\n", node, x, y);
    }
    while(1){                                       //get the connection data between node and con_node
        if(fscanf(f, "%d %d", &node, &con_node)==2){
            add_edge(G, node, con_node);
            //printf("%d  %d\n", node, con_node);
        }
        else{break;}
    }

    fclose(f);
}