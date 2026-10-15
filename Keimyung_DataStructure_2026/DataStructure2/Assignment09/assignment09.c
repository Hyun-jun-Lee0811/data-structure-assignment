#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VERTICES 10
#define EDGES 20
#define MAX_WEIGHT 100
#define INF 1000000

typedef struct {
    int u;
    int v;
    int w;
} Edge;

static void addEdge(Edge* edges, int* count, int weight[][VERTICES], int a, int b) {
    int u = a;
    int v = b;
    int w = rand() % MAX_WEIGHT + 1;

    if (u > v) {
        u = b;
        v = a;
    }
    edges[*count].u = u;
    edges[*count].v = v;
    edges[*count].w = w;
    weight[u][v] = w;
    weight[v][u] = w;
    (*count)++;
}

static void generateGraph(Edge* edges, int weight[][VERTICES]) {
    int count = 0;
    int i;
    int u;
    int v;

    memset(weight, 0, sizeof(int) * VERTICES * VERTICES);

    for (i = 1; i < VERTICES; i++) {
        addEdge(edges, &count, weight, i, rand() % i);
    }

    while (count < EDGES) {
        u = rand() % VERTICES;
        v = rand() % VERTICES;
        if (u == v || weight[u][v] != 0) {
            continue;
        }
        addEdge(edges, &count, weight, u, v);
    }
}

static void floydWarshall(int weight[][VERTICES], int dist[][VERTICES], int next[][VERTICES]) {
    int i;
    int j;
    int k;

    for (i = 0; i < VERTICES; i++) {
        for (j = 0; j < VERTICES; j++) {
            if (i == j) {
                dist[i][j] = 0;
                next[i][j] = j;
            }
            else if (weight[i][j] != 0) {
                dist[i][j] = weight[i][j];
                next[i][j] = j;
            }
            else {
                dist[i][j] = INF;
                next[i][j] = -1;
            }
        }
    }

    for (k = 0; k < VERTICES; k++) {
        for (i = 0; i < VERTICES; i++) {
            for (j = 0; j < VERTICES; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

static void printPath(int next[][VERTICES], int u, int v) {
    int current = u;

    printf("%d", current);
    while (current != v) {
        current = next[current][v];
        printf(" -> %d", current);
    }
}

int main(void) {
    Edge edges[EDGES];
    int weight[VERTICES][VERTICES];
    int dist[VERTICES][VERTICES];
    int next[VERTICES][VERTICES];
    int pairCount = 0;
    int i;
    int j;

    srand((unsigned int)time(NULL));

    generateGraph(edges, weight);
    floydWarshall(weight, dist, next);

    printf("Graph\n");
    printf("Vertices : %d\n", VERTICES);
    printf("Edges    : %d\n", EDGES);
    printf("\nEdges\n");
    for (i = 0; i < EDGES; i++) {
        printf("(%d, %d) Weight: %d\n", edges[i].u, edges[i].v, edges[i].w);
    }

    printf("\nShortest Paths\n\n");
    for (i = 0; i < VERTICES; i++) {
        for (j = i + 1; j < VERTICES; j++) {
            printf("%d -> %d\n", i, j);
            printf("Path     : ");
            printPath(next, i, j);
            printf("\n");
            printf("Distance : %d\n\n", dist[i][j]);
            pairCount++;
        }
    }
    printf("Total vertex pairs : %d\n", pairCount);

    return 0;
}