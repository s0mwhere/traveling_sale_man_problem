#ifndef GRAPH_H
#define GRAPH_H

typedef struct {    //connection between node
    int to;         //the connected node
    double weight;  //weight of connection (duh)
} Edge;

typedef struct {    //the graph
    Edge* edges;    //all the edge(connection) of a node
    int x;          //position of the node on the map (x,y)
    int y;
    int size;       //current numbers of connection of a node
    int capacity;   //max connection of a node before having to expand memmory space(malloc)
} AdjList;

double distance2D(double x1, double y1, double x2, double y2);
AdjList* create_graph(int n);
void add_position(AdjList* G, int u, int x, int y);
void add_edge(AdjList* G, int u, int v);
void ShowEdges(AdjList* G, int u);
void ShowNode(AdjList* G, int u);

#endif