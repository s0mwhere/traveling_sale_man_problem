#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "kruskal_fetch.h"

/*
* Input: .Txt file's path.
*
* Output : A structure containing: number of vertices, number of edges and weights.
*
* Description: Fetch all infomation of every edges the two point are connected and the distance between two said points.
*/

perf_graph_ptr load_perf_graph(const char* filename) {

    int edges=0;
    int tmp1, tmp2, tmp3;

    FILE* f = fopen(filename, "r");

    //Count the number of edges
    while (fscanf(f, "%d %d %d",
                &tmp1,
                &tmp2,
                &tmp3) == 3) {
        edges++;
        int dummy;
        while ((dummy = fgetc(f)) != '\n' && dummy != EOF);
    }
    rewind(f);

    //Initialize an empty graph.
    perf_graph_ptr graph = malloc(sizeof(perf_graph));
    graph->num_edges = 0;
    graph->num_vertices = 0;
    graph->connection = malloc(edges * sizeof(connection));

    //Adding infomation of each edges into "graph" structure until theres not line left (fscanf returns < 3).
    int i = 0;
    while ( i < edges &&
            fscanf(f, "%d %d %d",
                &graph->connection[i].point1,
                &graph->connection[i].point2,
                &graph->connection[i].weight) == 3) {
        i++;
        int dummy;
        while ((dummy = fgetc(f)) != '\n' && dummy != EOF);
    }

    /*  
    *   We obtain a perfect graph from applying A* algorithm on every vertices, therefore we can find the number of vertices from the number of edges.
    *  [Number of edges = (number of vertices) * (number of vertices - 1) / 2 ].
    */ 
    graph->num_edges = edges;
    graph->num_vertices = (int)((1 + sqrt(1 + 8.0 * edges)) / 2);

    fclose(f);

    return graph;
}



/*
*   Input: Pointer to a "perf_graph" structure.
* 
*   Output: None.
*
*   Description: Print out the "Perf_graph" infomation, and total weight.
*/
void perf_graph_transversal(perf_graph_ptr graph) {
    printf("Vertices: %d\n", graph->num_vertices);
    printf("Edges:    %d\n", graph->num_edges);
    for (int i = 0; i < graph->num_edges; i++) {
        printf("%d %d %d \n",graph->connection[i].point1,graph->connection[i].point2,graph->connection[i].weight);
    }
}



/*
*   Input: Pointer to a "perf_graph" structure
* 
*   Output: None
*
*   Description: Free memory of the given "perf_graph" structure
*/
void free_graph(perf_graph_ptr graph) {
    free(graph->connection); // free edges in MST
    free(graph);             // free struct itself
}