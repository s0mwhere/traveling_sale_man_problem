#include <stdio.h>
#include <stdlib.h>

#include "kruskal_fetch.h"
#include "kruskal_algorithm.h"

/*
*   Input: Pointer of "perf_graph" strucure, index of first connection, index of second connection.
* 
*   Output: None.
*
*   Description: connection_swap manually two connection's information inside the "perf_graph" structure.
*/
void connection_swap(perf_graph_ptr perf_graph, int j, int k) {
    int tmp = perf_graph->connection[j].point1;
    perf_graph->connection[j].point1 = perf_graph->connection[k].point1;
    perf_graph->connection[k].point1 = tmp;

    tmp = perf_graph->connection[j].point2;
    perf_graph->connection[j].point2 = perf_graph->connection[k].point2;
    perf_graph->connection[k].point2 = tmp;

    tmp = perf_graph->connection[j].weight;
    perf_graph->connection[j].weight = perf_graph->connection[k].weight;
    perf_graph->connection[k].weight = tmp;
}



/*
*   Input: A pointer of "perf_graph" structure, the first index and the last index of dynamic "connection" structure of "perfect_graph".
* 
*   Output: None
*
*   Description: Arrange the connection's indices in ascending order according to weight's value by using recursive Quick Sort algorithm.
*/
void pair_sort(perf_graph_ptr perf_graph, int b, int c) {
    if (b >= c) return;

    int j = b + 1;
    int k = c;
    int pivot = perf_graph->connection[b].weight;

    while (j <= k) {
        while (j <= c && perf_graph->connection[j].weight < pivot) j++;
        while (k > b && perf_graph->connection[k].weight > pivot) k--;

        if (j <= k) {
            connection_swap(perf_graph, j, k);
            j++;
            k--;
        }
    }

    connection_swap(perf_graph, b, k);

    pair_sort(perf_graph, b, k - 1);
    pair_sort(perf_graph, k + 1, c);
}



/*
*   Input: an array that stores every vertex of a graph, and a vertex included in said array.
* 
*   Output: connected representative of given vertex
*
*   Description: 
*   Connected representative means a vertex that represents all vertices that it connected to.      
*   This recursive function aims to find the representative of any given vertex.
*/
int find(int parent[], int point) {
    if (parent[point] != point)
        parent[point] = find(parent, parent[point]);
    return parent[point];
}



/*
*   Input: Pointer of a "perf_graph" structure, an array contains must travel to points.
* 
*   Output: a "mst" structure.
*
*   Description: The Kruskal algorithm as follow
*   1/ Initialize a cluster G, each of its component contains one vertex. 
*   2/ Create a queue Q containing all edges of G, arrange in ascending order of weight.
*   3/ Create an empty Tree T
*   4/ Sequentially putting each edge into T
*   5/ Return T
*/
mst_ptr kruskal(perf_graph_ptr perf_graph, int required[]) {

    //Step 1: Initialize a cluster G, each of its component contains one vertex. 
    int parent[perf_graph->num_vertices];
    for (int i = 0; i < perf_graph->num_vertices; i++) {
        parent[i] = i;
    }

    //Step 2: Create a queue Q containing all edges of G, arrange in ascending order of weight.
    pair_sort(perf_graph, 0, perf_graph->num_edges - 1);

    //Step 3: Create an empty Tree T
    mst_ptr tree = malloc(sizeof(mst));
    tree->size = 0;
    tree->connection = malloc((perf_graph->num_vertices - 1) * sizeof(connection));

    //Step 4: Sequentially putting each edge into T
    for (int i = 0; i < perf_graph->num_edges && tree->size < perf_graph->num_vertices - 1; i++) {
        int u = perf_graph->connection[i].point1;
        int v = perf_graph->connection[i].point2;

        //Mapping points into indices
        for(int j = 0; j< perf_graph->num_vertices;j++) {
            if (u == required[j]) u = j;
            if (v == required[j]) v = j;
        }

        if (find(parent,u) != find(parent,v)) {                                     //If the connected representative are the same, these two vertices are connected. If not, add two vertices into Tree.
            tree->connection[tree->size] = perf_graph->connection[i];               //Add two vertices into Tree.
            parent[find(parent,u)] = find(parent,v);                                //Replace the connected representative by one of two vertices.
            tree->size++;
        }
    }

    //Step 5: Return T
    return tree;

}



/*
*   Input: Pointer to a "mst" structure
* 
*   Output: None
*
*   Description: Print out the tree. And its total weight.
*/
void tree_transversal(mst_ptr tree) {
    printf("Size: %d\n", tree->size);
    int accummulate = 0;
    for(int i = 0; i < tree->size;i++){
        printf("%d %d %d\n", tree->connection[i].point1, tree->connection[i].point2, tree->connection[i].weight);
        accummulate += tree->connection[i].weight;
    }
    printf("Total Weight = %d\n", accummulate);
    return;
}



/*
*   Input: Pointer to a "mst" structure
* 
*   Output: None
*
*   Description: Free memory of the given "mst" structure
*/
void free_mst(mst_ptr tree) {
    free(tree->connection); // free edges in MST
    free(tree);             // free struct itself
}