#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include"graph.h"
#define MAX_SIZE 100     // maximum number of elements in the priority queue
#define MAX_VALUE_LEN 100 // maximum length of the value array

/* Each node stores:
   - priority: used to order the queue (min-heap)
   - value: an integer array (payload)
   - value_len: number of valid integers in value[] */
typedef struct {
    int priority;
    int value[MAX_VALUE_LEN];
    int value_len;
} Node;

/* Priority queue implemented as a min-heap */
typedef struct {
    Node data[MAX_SIZE];  // heap stored in a 1D array
    int size;             // current number of elements
} PriorityQueue;

void swap(Node *a, Node *b);

PriorityQueue* pq_create(void);

void pq_push(PriorityQueue *pq, int priority, int *value, int value_len);

Node pq_pop(PriorityQueue *pq);

int contains(int *a, int n, int x);

void A_Star(AdjList* G, int* req, int n_req, const char* filename);

#endif