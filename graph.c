#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"graph.h"


double distance2D(double x1, double y1, double x2, double y2) {     //distance between 2 point(node)
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

AdjList* create_graph(int n) {                          // n: number of node in graph   index: 0,1,2,...
    AdjList* G = (AdjList*)malloc(n * sizeof(AdjList));
    for (int i = 0; i < n; i++) {
        G[i].size = 0;
        G[i].capacity = 4;                              //default 1 node support 4 connections
        G[i].edges = (Edge*)malloc(4 * sizeof(Edge));
    }
    return G;
}

void add_position(AdjList* G, int u, int x, int y){     //give node position data
    G[u].x = x;
    G[u].y = y;
}

void add_edge(AdjList* G, int u, int v) {                     //U: current node   V:connecting nodes
    double w = distance2D(G[u].x, G[u].y, G[v].x, G[v].y);              //distance between 2 point (weight)

    if (G[u].size == G[u].capacity) {                                   //if reach max edge, double the memory for connection
        G[u].capacity *= 2;
        G[u].edges = realloc(G[u].edges, G[u].capacity * sizeof(Edge));
    }
    G[u].edges[G[u].size++] = (Edge){v, w};                             // connect U and V with edge of weight W (u->v)


    if (G[v].size == G[v].capacity) {                                   //adding the 2 way connection (V -> U)
        //printf("overload: %d %d", G[v].size, G[v].capacity);
        G[v].capacity *= 2;
        G[v].edges = realloc(G[v].edges, G[v].capacity * sizeof(Edge));
    }
    G[v].edges[G[v].size++] = (Edge){u, w};
}

void ShowEdges(AdjList* G, int u){                                  //print connected edge of node u (testing)
    for (int i = 0; i < G[u].size; i++ ){
        printf("%d: %f , ", G[u].edges[i].to, G[u].edges[i].weight);
    }
}

void ShowNode(AdjList* G, int u){       //print out node u info (testing)
    printf("--------\n");
    printf("Node: %d\n", u);
    printf("posistion: %d %d\n", G[u].x, G[u].y);
    printf("capacity: %d\n", G[u].capacity);
    printf("size: %d\n", G[u].size);
    printf("edge "); ShowEdges(G,u); printf("\n");
}

