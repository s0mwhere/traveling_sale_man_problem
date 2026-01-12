#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "kruskal_algorithm.h"
#include "christofiles_algorithm.h"

/*
*   Input: Pointer of a "tree" structure, an array contains must travel to points.
* 
*   Output: A perfect_graph contains only odd-degree verticies
*
*   Description: This function works by counting all vertices have exists in every connection.
*   If the counting is odd then it is a odd-degree vertices and add infomation related to it to a new "perf_graph" structure.
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

void minimum_matching(perf_graph_ptr graph, mst_ptr tree) {
    int required[graph->num_vertices];

    for (int i = 0; i < graph->num_vertices; i++) {
        required[i] = -1;
    }

    int count = 0;
    for (int i = 0; i < graph->num_edges; i++) {
        int u = graph->connection[i].point1;
        int v = graph->connection[i].point2;

    // check if u is already in the array
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

    // check if v is already in the array
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
    
    int m[graph->num_vertices][graph->num_vertices];

    for (int i = 0; i < graph->num_vertices; i++) {
        for (int j = 0; j < graph->num_vertices; j++) {
            m[i][j] = 0;
        }
    }
    for (int i = 0; i < graph->num_vertices;i++) {
        int u = graph->connection[i].point1;
        int v = graph->connection[i].point2;

        for(int j = 0; j < graph->num_vertices; j++) {
            if (u == required[j]) u = j;
            if (v == required[j]) v = j;
        }
        m[u][v] = graph->connection[i].weight;
        m[v][u] = graph->connection[i].weight;
    }

    int size = 1 << graph->num_vertices;  
    int dp[size];
    int prev_choice[size];
    int pair[size][2];
    for (int mask = 0; mask < size; mask++) {
        dp[mask] = INT_MAX;
        prev_choice[mask] = -1;
        pair[mask][0] = -1;
        pair[mask][1] = -1;
    }
    dp[0] = 0;

    for (int mask = 1; mask < size; mask++) {
        //Count the number of 1 in mask, even number of mask -> continue
        int count = 0;
        for (int k = 0; k < graph->num_vertices; k++)
            if (mask & (1 << k)) count++;
        if (count % 2 != 0) continue;
        
        int i;
        for (i = 0; i < graph->num_vertices; i++)
            //find the first vertex
            if (mask & (1 << i)) break;

        for (int j = i + 1; j < graph->num_vertices; j++) {
            //find the second vertex
            if (mask & (1 << j)) {
                int new_mask = mask ^ (1 << i) ^ (1 << j);
                if (dp[new_mask] + m[i][j] < dp[mask]) {
                    dp[mask] = dp[new_mask] + m[i][j];
                    prev_choice[mask] = new_mask;
                    pair[mask][0] = i;
                    pair[mask][1] = j;
                }
            }
        }
    }

    int mask = (1 << graph->num_vertices) - 1;
    while (mask) {
        int i = pair[mask][0];
        int j = pair[mask][1];

        i = required[i];
        j = required[j];

        for(int k = 0; k < graph->num_edges; k++) {
            if ((graph->connection[k].point1 == i && graph->connection[k].point2 == j) || (graph->connection[k].point1 == j && graph->connection[k].point2 == i)) {
                tree->connection = realloc(tree->connection,((tree->size +1) * sizeof(connection)));
                tree->connection[tree->size] = graph->connection[k];
                tree->size++;
            }
        }
        mask = prev_choice[mask]; // move to the previous mask
    }
    return;
}

anode make_node(int vertex, int weight) {
    anode p = (anode)malloc(sizeof(struct node));
    p->vertex = vertex;
    p->weight = weight;
    p->next = NULL;
    return p;
}

linked_list insert_first(linked_list l, int vertex, int weight) {
    anode p = make_node(vertex, weight);
    p -> next = l;
    l = p;
    return l;
}

linked_list insert_last(linked_list l, int vertex, int weight) {
    anode p = make_node(vertex, weight); // create new node
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

void free_ll(linked_list ll) {
    linked_list current_ll = ll;
    while (ll != NULL) {
        ll=ll->next;
        free(current_ll);
        current_ll = ll;
    }
}

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

linked_list eulerian_tour(mst_ptr tree, int require[]) {
    int transversal[tree->size];
    for (int i = 0; i < tree->size; i++) {
        transversal[i] = 0;
    }

    linked_list ll = NULL;
    int starting_vertex = tree->connection[0].point1;

    visit_edge(tree, transversal, starting_vertex, &ll);

    ll = insert_last(ll, starting_vertex, 0);

    return ll;
}

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
*   Input: Pointer of a "tree" structure, an array contains must travel to points.
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



void christofides() {

}