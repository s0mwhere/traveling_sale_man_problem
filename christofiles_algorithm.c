#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "kruskal_algorithm.h"
#include "christofiles_algorithm.h"

/*
*   Input: Pointer of a "tree" structure, an array contains must travel to points, a array contains all must visit points and its length.
* 
*   Output: A perfect_graph contains only odd-degree verticies
*
*   Description: This function works by counting all vertices have exists in every connection.
*   If the counting is odd, it is a odd-degree vertices and then add infomation related to it to a new "perf_graph" structure.
*/

perf_graph_ptr isolation (perf_graph_ptr graph ,mst_ptr tree, int required[], int num) {
    int degree[num];
    for (int i = 0; i<num; i++) {
        degree[i] = 0;
    }
    
    for (int i = 0; i<tree->size; i++) {
        for (int j = 0; j< num; j++) {
            if (tree->connection[i].point1 == required[j] || tree->connection[i].point2 == required[j]) {
                degree[j]++;
            }
        }
    }
    

    int num_vertices = 0;

    for (int i = 0; i < num; i++) {
        if (degree[i] % 2 == 1) {
            num_vertices++;
        }
    }
    perf_graph_ptr odd_degree_vertices = malloc(sizeof(perf_graph));
    odd_degree_vertices->num_vertices = num_vertices;
    int num_edges = (num_vertices)*(num_vertices-1)/2;
    odd_degree_vertices->num_edges = num_edges;
    odd_degree_vertices->connection = malloc(num_edges * sizeof(connection));

    int index = 0;
    for (int i = 0; i < num; i++) {
        for (int j = i; j < num; j++) {
            if (degree[i] % 2 == 1 && degree[j] % 2 == 1) {
                for (int k = 0; k<graph->num_edges; k++) {
                        if((graph->connection[k].point1 == required[i] && graph->connection[k].point2 == required[j]) 
                        || (graph->connection[k].point2 == required[i] && graph->connection[k].point1 == required[j])) {
                            odd_degree_vertices->connection[index].point1 = graph->connection[k].point1;
                            odd_degree_vertices->connection[index].point2 = graph->connection[k].point2;
                            odd_degree_vertices->connection[index].weight = graph->connection[k].weight;
                            index++;
                        }
                }
            }
        }
    }

    return odd_degree_vertices;
}

/*
*   Input: Pointer of a "perf_graph" structure contains only odd-degree vertices, Pointer of a "mst" structure;
* 
*   Output: None
*
*   Description: This function aims to find minimum weight matching.
*   A matching means edge connect two odd-vertices, convert it into even-degree vertices
*  
*/

void minimum_matching(perf_graph_ptr graph, mst_ptr tree) {
    printf("DEBUG: graph->num_vertices = %d\n", graph->num_vertices);
    printf("DEBUG: graph->num_edges = %d\n", graph->num_edges);
    
    // Kiểm tra nếu quá nhiều vertices
    if (graph->num_vertices > 20) {
        printf("ERROR: Too many odd vertices (%d). Cannot handle.\n", graph->num_vertices);
        return;
    }
    //An array contains all unique vertices from a graph.
    int required[graph->num_vertices];

    for (int i = 0; i < graph->num_vertices; i++) {
        required[i] = -1;
    }

    int count = 0;
    for (int i = 0; i < graph->num_edges; i++) {
        int u = graph->connection[i].point1;
        int v = graph->connection[i].point2;

    // Check if a vertex is already in the array. If true then skip, If false, said vertex will be added.
        int found = 0;
        for (int j = 0; j < count; j++) {
            if (required[j] == u) {
                found = 1;
                break;
            }
        }
        if (!found) {
            required[count++] = u;
        }

    // Check if a vertex is already in the array. If true then skip, If false, said vertex will be added.
        found = 0;
        for (int j = 0; j < count; j++) {
            if (required[j] == v) {
                found = 1;
                break;
            }
        }
        if (!found) {
            required[count++] = v;
        }
    }
    //Create a adjacent matrix
    int m[graph->num_vertices][graph->num_vertices];

    for (int i = 0; i < graph->num_vertices; i++) {
        for (int j = 0; j < graph->num_vertices; j++) {
            m[i][j] = 0;
        }
    }
    //Fill the matrix with edge's weight, the two indices are two vertices of said edge.
    for (int i = 0; i < graph->num_edges;i++) {
        int u = graph->connection[i].point1;
        int v = graph->connection[i].point2;

        for(int j = 0; j < graph->num_vertices; j++) {
            if (u == required[j]) u = j;
            if (v == required[j]) v = j;
        }
        m[u][v] = graph->connection[i].weight;
        m[v][u] = graph->connection[i].weight;
    }
    

    int size = 1 << graph->num_vertices;                //  The maximum combination of matching is 2^(number of vertices)
    /*  
    *   An array contains all the possible matching 
    *   Each vertices and matching will be hot-one binary encoded, 
    *   for example, 
    *   first vertex in a total of 4 vertices is encoded as 0001(base 2) = 1 (base 10)
    *   Matching the first two vertices will be encoded as 0011 = (base 2) = 3(base 10)
    */
    int dp[size];
    int prev_choice[size];                              //An array contains which maching has been selected
    int pair[size][2];                                  //An 2D array to contains the two points that has been selected
    for (int mask = 0; mask < size; mask++) {
        dp[mask] = INT_MAX;                             //Since we find minimum matching weight, The default value wil be infinity(INT_MAX)
        prev_choice[mask] = -1;
        pair[mask][0] = -1;
        pair[mask][1] = -1;
    }
    dp[0] = 0; //mask = 0 means no matching has been selected, and therefore no weight will be added

    for (int mask = 1; mask < size; mask++) {
        //Count the number of 1 in masks, only even numbers of 1 in masks will be selected
        int count = 0;
        for (int k = 0; k < graph->num_vertices; k++)
            if (mask & (1 << k)) count++;
        if (count % 2 != 0) continue;
        
        int i;
        for (i = 0; i < graph->num_vertices; i++)
            //find the first vertex's index
            if (mask & (1 << i)) break;

        for (int j = i + 1; j < graph->num_vertices; j++) {
            //find the second vertex's index
            if (mask & (1 << j)) {
                int old_mask = mask ^ (1 << i) ^ (1 << j);  // old_mask = the current mask - two new vertices (example 0b110000 = 0b110011 xor 0b0000001 xor 0b000010)
                if (dp[old_mask] + m[i][j] < dp[mask]) {    // if total weight of edges of vertices i and j + the current matching we have is smaller than the best matching we have.
                    dp[mask] = dp[old_mask] + m[i][j];      // -> update the best matching we have.
                    prev_choice[mask] = old_mask;           // Store the value of previous matching.
                    pair[mask][0] = i;                      // Store the vertex index i of previous matching.
                    pair[mask][1] = j;                      // Store the vertex index j of previous matching.
                }
            }
        }
    }
    //Trace back the process of matching to find which edges have been selected in best mask.
    int mask = (1 << graph->num_vertices) - 1;
    while (mask) {
        // Get the vertex index i,j from array
        int i = pair[mask][0]; 
        int j = pair[mask][1];
        // Mapping the index i,j to required list to get vertices.
        i = required[i];
        j = required[j];

        for(int k = 0; k < graph->num_edges; k++) {
            // Find the exact edges that is connected by two found vertices
            if ((graph->connection[k].point1 == i && graph->connection[k].point2 == j) || (graph->connection[k].point1 == j && graph->connection[k].point2 == i)) {
                //Add the edges to MST.
                tree->connection = realloc(tree->connection,((tree->size +1) * sizeof(connection)));
                tree->connection[tree->size] = graph->connection[k];
                tree->size++;
            }
        }
        mask = prev_choice[mask]; // move to the previous mask.
    }
    return;
}
/*
*   Input: Vertex and weight of edges connected by given Vertex and next Vertex.
* 
*   Output: Pointer to a Node of "linked_list" structure
*
*   Description: Make a node of "linked_list" structure
*/
anode make_node(int vertex, int weight) {
    anode p = (anode)malloc(sizeof(struct node));
    p->vertex = vertex;
    p->weight = weight;
    p->next = NULL;
    return p;
}



/*
*   Input: Pointer of head of a linked_list, Vertex and weight of edges connected by given Vertex and next Vertex.
* 
*   Output: Pointer to "linked_list" structure
*
*   Description: Add a node at the start of the linked list
*/
linked_list insert_first(linked_list l, int vertex, int weight) {
    anode p = make_node(vertex, weight);
    p -> next = l;
    l = p;
    return l;
}



/*
*   Input: Pointer to a linked_list, Vertex and weight of edges connected by given Vertex and next Vertex.
* 
*   Output: Pointer to "linked_list" structure
*
*   Description: Add a node at the end of the linked list
*/
linked_list insert_last(linked_list l, int vertex, int weight) {
    anode p = make_node(vertex, weight);
    if (l == NULL) {
        return p;
    }

    anode cur = l;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = p;
    return l;
}



/*
*   Input: Pointer to a linked_list,
* 
*   Output: None
*
*   Description: Print out every value of each nodes of a linked_list.
*/
void print_ll(linked_list l) {
    anode p = l;
    if(p == NULL) {
        printf("\nNULL\n");
    }
    while (p != NULL) {
        printf("{%d %d} ",p -> vertex, p -> weight);
        p = p -> next; 
    }
}



/*
*   Input: Pointer of a node in linked_list
* 
*   Output: None
*
*   Description: Delete given node.
*/
void delete(anode p) {
    if(p->next == NULL) {
        free(p);
        p = NULL;
    }
    else {
    anode q = p->next;
    p -> vertex = q->vertex;
    p -> weight = q->weight;
    p -> next = q->next;
    free(q);
    }
}



/*
*   Input: Pointer of a linked_list
* 
*   Output: None
*
*   Description: Delete given linked_list
*/
void free_ll(linked_list ll) {
    linked_list current_ll = ll;
    while (ll != NULL) {
        ll=ll->next;
        free(current_ll);
        current_ll = ll;
    }
}



/*
*   Input: A pointer to a "mst" structure, A pointer to a array containing whether an edge has been visited (0 = haven't been visited, 1 = have been visited)
*          A starting vertex, an linked_list to store transversed path. 
*
*   Output: None
*
*   Description: This recursive function works by checking whether an edge is visited and next point that starting_vertex connected to
*                Then calls itself with the starting_vertex is the newly found vertex.
*/
void visit_edge(mst_ptr tree, int *transversal, int starting_vertex, linked_list* ll) {
    int i;
    for(i = 0; i < tree->size; i++) {
        if(!transversal[i] && (tree->connection[i].point1 == starting_vertex || tree->connection[i].point2 == starting_vertex)) {
            transversal[i] = 1;
            int v = 0;
            if (tree->connection[i].point1 == starting_vertex) {
                v = tree->connection[i].point2;
            }
            else {
                v = tree->connection[i].point1;
            }

            visit_edge(tree, transversal, v, ll);

            *ll = insert_first(*ll, starting_vertex, tree->connection[i].weight);
        }
    }
}


/*
*   Input: A pointer to "mst" structure, a array of must visit points.
* 
*   Output: A pointer to "linked_list" structure that contains transversal path.
*
*   Description: 
*/
linked_list eulerian_tour(mst_ptr tree, int require[]) {
    int transversal[tree->size];
    for (int i = 0; i < tree->size; i++) {
        transversal[i] = 0;
    }

    linked_list ll = NULL;
    int starting_vertex = tree->connection[0].point1;

    visit_edge(tree, transversal, starting_vertex, &ll);

    ll = insert_last(ll, starting_vertex, 0); //Manually add the starting vertex at the end, because in eulerian tour, starting point matches ending point

    return ll;
}


/*
*   Input: Pointer of a "perf_graph" structure, pointer to a "linked_list" structure
* 
*   Output: None
*
*   Description: This function will transverse the entire "linked_list" structure, delete any point that has existed previously and change the weight accourdingly 
*/
void tsp_tour(perf_graph_ptr graph, linked_list ll) {
    linked_list current_ll = ll;
    while (current_ll->next != NULL) {
        linked_list pulse = ll;
        int endpoint = -1;
        while(pulse != current_ll) {
            if((pulse->vertex == current_ll->vertex)) {
                endpoint = current_ll->next->vertex;
                while(pulse->next != current_ll) {
                    pulse = pulse -> next;
                }
                break;
            }
            else{
                pulse = pulse->next;
            }
        }
        if(endpoint != -1) {
            delete(current_ll);
            for(int i = 0; i < graph->num_edges; i++) {
                if((endpoint == graph->connection[i].point1 && pulse->vertex == graph->connection[i].point2)
                || (endpoint == graph->connection[i].point2 && pulse->vertex == graph->connection[i].point1)) {
                    pulse->weight = graph->connection[i].weight;
                }
            }
            current_ll = pulse;
        }
        current_ll = current_ll->next;
    }
}

/*
*   Input: Pointer to a "perf_graph" structure, Pointer of a "mst" structure, a array contains all must visit points and its length.
* 
*   Output: an optimised "mst" structure 
*
*   Description: The Christofides algorithm as follow
*   1/ Find MST T of Graph (execute via Kruskal Algorithm)
*   2/ Isolate Set of Odd-Degree Vertices S
*   3/ Find Minimum Weight Perfect Matching M of S (execute via Subset DP)
*   4/ Combine T and M into Multigraph G
*   5/ Generate Eulerian Tour of G (execute via Hierholzer algorithm)
*   6/ Generate TSP Tour from Eulerian Tour
*/
linked_list christofides(perf_graph_ptr graph, mst_ptr tree, int require[], int num) {
    //Step 2: Isolate Set of Odd-Degree Vertices S
    perf_graph_ptr odd_vertices = isolation(graph,tree,require,13);

    //Step 3 and 4: Find Minimum Weight Perfect Matching M of S (execute via Subset DP)
    minimum_matching(odd_vertices, tree);

    //Step 5: Generate Eulerian Tour of G (execute via Hierholzer algorithm)
    linked_list ll = eulerian_tour(tree, require);

    //Step 6: Generate TSP Tour from Eulerian Tour
    tsp_tour(graph, ll);

    //Free memory
    free_mst(tree);
    free_graph(graph);
    free_graph(odd_vertices);
    return ll;
}