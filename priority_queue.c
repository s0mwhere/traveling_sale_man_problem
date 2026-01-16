#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "priority_queue.h"

#define MAX_SIZE 100   // maximum number of elements in the queue

/* Swap two nodes (entire payload moves together) */
void swap(Node *a, Node *b) {
    Node tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Create and initialize a priority queue */
PriorityQueue* pq_create(void) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (pq != NULL)
        pq->size = 0;
    return pq;
}

/* Insert a new node with given priority and int array value */
void pq_push(PriorityQueue *pq, int priority, int *value, int value_len) {
    if (pq->size == MAX_SIZE || value_len > MAX_VALUE_LEN)
        return;

    /* Insert at the end of the heap */
    int i = pq->size++;
    pq->data[i].priority = priority;
    pq->data[i].value_len = value_len;

    /* Copy value array */
    for (int j = 0; j < value_len; j++) {
        pq->data[i].value[j] = value[j];
    }

    /* Heapify up: restore min-heap property */
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (pq->data[parent].priority <= pq->data[i].priority)
            break;
        swap(&pq->data[parent], &pq->data[i]);
        i = parent;
    }
}

/* Remove and return the node with minimum priority */
Node pq_pop(PriorityQueue *pq) {
    Node invalid = { -1, {0}, 0 };
    if (pq->size == 0)
        return invalid;

    /* Save the root (minimum priority node) */
    Node min = pq->data[0];

    /* Move last element to root and shrink heap */
    pq->data[0] = pq->data[--pq->size];

    /* Heapify down: restore min-heap property */
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < pq->size &&
            pq->data[left].priority < pq->data[smallest].priority)
            smallest = left;

        if (right < pq->size &&
            pq->data[right].priority < pq->data[smallest].priority)
            smallest = right;

        if (smallest == i)
            break;

        swap(&pq->data[i], &pq->data[smallest]);
        i = smallest;
    }

    return min;
}

int contains(int *a, int n, int x) {    //check if array a of size n have element x
    for (int i = 0; i < n; i++)
        if (a[i] == x)
            return 1;
    return 0;
}

int* duplicate_add_array(const int *src, int len, int new) {     //dupplicate array src of size len into dst and add an element new at the end(remember to free(dst))
    int *dst = malloc((len+1) * sizeof(int));
    if (dst == NULL) return NULL;

    for (int i = 0; i < len; i++) {
        dst[i] = src[i];
    }
    dst[len] = new;
    return dst;
}

void A_Star(AdjList* G, int* req, int n_req, const char* filename){       //graph G, index of require node, number of require node, output file

    FILE *file = fopen(filename, "w");

    //printf(" %d ",n_req);
    for(int i = 0; i < n_req; i++){             
        for(int j = i+1; j < n_req; j++){       //2 nested loop to cycle through all node pair ij and avoid repeating ij-ji
            //printf("next pair\n");
            
            int node_chain[100];                //store the node chain of a route
            PriorityQueue *PQ = pq_create();    //initiate a queue for a single pair search

            int ignore_node[200];               //node that have been check
            int tmp_ignore_node[50];            //new ignore node that going to be add to the main array
            int root_node = req[i];             //current inspecting node (node with current smallest weight)
            int goal_node = req[j];             //the end
            double prior_weight = 0;            //combine weight of the edge chain

            ignore_node[0] = root_node;         //ignore the start node since already check
            int m = 1;                          //number of node in ignore_node, start at 1
            int chain_num = 1;                  //number of node in route chain
            node_chain[0] = req[i];             //the node chain start with the beginning node

            while(1){
                //printf(" loop ");
                int tmp_m = 0;  //the size of the tmp_ignore_node in current iteration

                for (int l = 0; l < G[root_node].size; l++ ){           //put all connected node into queue
                    int leaf_node = G[root_node].edges[l].to;           //node connected to root node
                    if(!contains(ignore_node, m, leaf_node)){      //ignore past node

                        double node_dist = distance2D(G[leaf_node].x, G[leaf_node].y, G[goal_node].x, G[goal_node].y);
                        int *tmp_node_chain = duplicate_add_array(node_chain, chain_num, leaf_node);    //use to pass the update route into queue

                        pq_push(PQ,                                                  //the queue
                            G[root_node].edges[l].weight + node_dist + prior_weight, //combine weight = past weight + node edge weight + node distance to goal
                            tmp_node_chain,                                          //update route
                            chain_num+1);                                            //current number of node in route
                        free(tmp_node_chain);
                    }
                }

                Node n = pq_pop(PQ);
                chain_num = n.value_len;            //load info of the route in next inspection
                root_node = n.value[chain_num-1];
                for(int l = 0; l < chain_num; l++){
                    node_chain[l] = n.value[l];
                }
                prior_weight = n.priority - distance2D(G[root_node].x, G[root_node].y, G[goal_node].x, G[goal_node].y);

                ignore_node[m] = root_node; m++;                //add the pop node to ignore list

                if(root_node == goal_node){                         //if the shortest path found add it info to output file
                    fprintf(file, "%d %d ", req[i], req[j]);        //start node and goal node
                    fprintf(file, "%.f ", prior_weight);            //weight of the shortest route
                    for(int l = 0; l < chain_num; l++){             
                        fprintf(file, "%d ", n.value[l]);           //node chain of the route
                    }
                    fprintf(file, "\n");
                    break;
                }
            }
            free(PQ);
        }
    }
    fclose(file);
}


/* void A_star find the shortest path between all pair of require node and write a text format
start_node1  goal_node1   weight_of_shortest_route    start_node1  node_1  node_2  ... node_n  goal_node1
start_node2  goal_node2   weight_of_shortest_route    start_node2  node_1  node_2  ... node_n  goal_node2
...
*/
