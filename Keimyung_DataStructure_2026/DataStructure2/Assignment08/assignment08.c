#define _CRT_SECURE_NO_WARNINGS
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define VERTICES 50
#define SPARSE_EDGES 100
#define DENSE_EDGES 600
#define MAX_WEIGHT 100
#define INF 1000000000

typedef struct {
    int u;
    int v;
    int w;
} Edge;

typedef struct AdjNode {
    int vertex;
    int weight;
    struct AdjNode* next;
} AdjNode;

static double nowMs(void) {
#ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
#endif
}

static int isConnected(Edge* edges, int edgeCount) {
    int matrix[VERTICES][VERTICES];
    int visited[VERTICES];
    int stack[VERTICES];
    int top = 0;
    int count = 0;
    int i;
    int u;

    memset(matrix, 0, sizeof(matrix));
    memset(visited, 0, sizeof(visited));
    for (i = 0; i < edgeCount; i++) {
        matrix[edges[i].u][edges[i].v] = 1;
        matrix[edges[i].v][edges[i].u] = 1;
    }

    stack[top] = 0;
    top++;
    visited[0] = 1;
    while (top > 0) {
        top--;
        u = stack[top];
        count++;
        for (i = 0; i < VERTICES; i++) {
            if (matrix[u][i] == 1 && visited[i] == 0) {
                visited[i] = 1;
                stack[top] = i;
                top++;
            }
        }
    }

    if (count == VERTICES) {
        return 1;
    }
    return 0;
}

static void addEdge(Edge* edges, int* count, int used[][VERTICES], int u, int v) {
    edges[*count].u = u;
    edges[*count].v = v;
    edges[*count].w = rand() % MAX_WEIGHT + 1;
    used[u][v] = 1;
    used[v][u] = 1;
    (*count)++;
}

static void generateGraph(Edge* edges, int edgeCount) {
    int used[VERTICES][VERTICES];
    int count;
    int i;
    int u;
    int v;

    do {
        memset(used, 0, sizeof(used));
        count = 0;

        for (i = 1; i < VERTICES; i++) {
            addEdge(edges, &count, used, i, rand() % i);
        }

        while (count < edgeCount) {
            u = rand() % VERTICES;
            v = rand() % VERTICES;
            if (u == v || used[u][v] == 1) {
                continue;
            }
            addEdge(edges, &count, used, u, v);
        }
    } while (isConnected(edges, edgeCount) == 0);
}

static AdjNode** buildAdjList(Edge* edges, int edgeCount) {
    AdjNode** adj;
    AdjNode* node;
    int i;

    adj = (AdjNode**)calloc(VERTICES, sizeof(AdjNode*));

    for (i = 0; i < edgeCount; i++) {
        node = (AdjNode*)malloc(sizeof(AdjNode));
        node->vertex = edges[i].v;
        node->weight = edges[i].w;
        node->next = adj[edges[i].u];
        adj[edges[i].u] = node;

        node = (AdjNode*)malloc(sizeof(AdjNode));
        node->vertex = edges[i].u;
        node->weight = edges[i].w;
        node->next = adj[edges[i].v];
        adj[edges[i].v] = node;
    }
    return adj;
}

static void freeAdjList(AdjNode** adj) {
    AdjNode* node;
    AdjNode* next;
    int i;

    for (i = 0; i < VERTICES; i++) {
        node = adj[i];
        while (node != NULL) {
            next = node->next;
            free(node);
            node = next;
        }
    }
    free(adj);
}

static void buildMatrix(Edge* edges, int edgeCount, int matrix[][VERTICES]) {
    int i;

    memset(matrix, 0, sizeof(int) * VERTICES * VERTICES);
    for (i = 0; i < edgeCount; i++) {
        matrix[edges[i].u][edges[i].v] = edges[i].w;
        matrix[edges[i].v][edges[i].u] = edges[i].w;
    }
}

static int compareEdge(const void* a, const void* b) {
    const Edge* x = (const Edge*)a;
    const Edge* y = (const Edge*)b;
    if (x->w < y->w) {
        return -1;
    }
    if (x->w > y->w) {
        return 1;
    }
    return 0;
}

static int findRoot(int* parent, int x) {
    while (parent[x] != x) {
        parent[x] = parent[parent[x]];
        x = parent[x];
    }
    return x;
}

static int kruskal(AdjNode** adj, Edge* edgeArray, int* parent, int* rank, Edge* mst, int* cost) {
    AdjNode* node;
    int edgeCount = 0;
    int mstCount = 0;
    int i;
    int u;
    int rootU;
    int rootV;

    for (u = 0; u < VERTICES; u++) {
        node = adj[u];
        while (node != NULL) {
            if (u < node->vertex) {
                edgeArray[edgeCount].u = u;
                edgeArray[edgeCount].v = node->vertex;
                edgeArray[edgeCount].w = node->weight;
                edgeCount++;
            }
            node = node->next;
        }
    }

    qsort(edgeArray, edgeCount, sizeof(Edge), compareEdge);

    for (i = 0; i < VERTICES; i++) {
        parent[i] = i;
        rank[i] = 0;
    }

    *cost = 0;
    for (i = 0; i < edgeCount && mstCount < VERTICES - 1; i++) {
        rootU = findRoot(parent, edgeArray[i].u);
        rootV = findRoot(parent, edgeArray[i].v);
        if (rootU != rootV) {
            if (rank[rootU] < rank[rootV]) {
                parent[rootU] = rootV;
            }
            else if (rank[rootU] > rank[rootV]) {
                parent[rootV] = rootU;
            }
            else {
                parent[rootV] = rootU;
                rank[rootU]++;
            }
            mst[mstCount] = edgeArray[i];
            mstCount++;
            *cost += edgeArray[i].w;
        }
    }
    return mstCount;
}

static int prim(int matrix[][VERTICES], int* key, int* parent, int* visited, Edge* mst, int* cost) {
    int mstCount = 0;
    int i;
    int j;
    int u;
    int minKey;

    for (i = 0; i < VERTICES; i++) {
        key[i] = INF;
        parent[i] = -1;
        visited[i] = 0;
    }
    key[0] = 0;
    *cost = 0;

    for (i = 0; i < VERTICES; i++) {
        u = -1;
        minKey = INF;
        for (j = 0; j < VERTICES; j++) {
            if (visited[j] == 0 && key[j] < minKey) {
                minKey = key[j];
                u = j;
            }
        }
        visited[u] = 1;
        if (parent[u] != -1) {
            mst[mstCount].u = parent[u];
            mst[mstCount].v = u;
            mst[mstCount].w = key[u];
            mstCount++;
            *cost += key[u];
        }
        for (j = 0; j < VERTICES; j++) {
            if (matrix[u][j] != 0 && visited[j] == 0 && matrix[u][j] < key[j]) {
                key[j] = matrix[u][j];
                parent[j] = u;
            }
        }
    }
    return mstCount;
}

static void printResult(int caseNo, const char* graphName, const char* structName, const char* algoName,
    int edgeCount, int mstCount, int cost, size_t memory, double timeMs, Edge* mst) {
    int i;

    printf("Case %d : %s - %s - %s\n", caseNo, graphName, structName, algoName);
    printf("Vertices      : %d\n", VERTICES);
    printf("Edges         : %d\n", edgeCount);
    printf("MST Edges     : %d\n", mstCount);
    printf("MST Cost      : %d\n", cost);
    printf("Memory        : %lu Bytes\n", (unsigned long)memory);
    printf("Execution Time: %.6f ms\n", timeMs);
    printf("MST Edge List (u - v : weight)\n");
    for (i = 0; i < mstCount; i++) {
        printf("(%2d - %2d : %3d)", mst[i].u, mst[i].v, mst[i].w);
        if (i % 5 == 4 || i == mstCount - 1) {
            printf("\n");
        }
        else {
            printf("  ");
        }
    }
    printf("\n");
}

static void runKruskal(int caseNo, const char* graphName, Edge* edges, int edgeCount) {
    AdjNode** adj;
    Edge* edgeArray;
    Edge mst[VERTICES - 1];
    int parent[VERTICES];
    int rank[VERTICES];
    int mstCount;
    int cost;
    size_t memory;
    double start;
    double end;

    adj = buildAdjList(edges, edgeCount);
    edgeArray = (Edge*)malloc(sizeof(Edge) * edgeCount);

    start = nowMs();
    mstCount = kruskal(adj, edgeArray, parent, rank, mst, &cost);
    end = nowMs();

    memory = sizeof(AdjNode*) * VERTICES
        + sizeof(AdjNode) * 2 * edgeCount
        + sizeof(Edge) * edgeCount
        + sizeof(int) * VERTICES * 2
        + sizeof(Edge) * (VERTICES - 1);

    printResult(caseNo, graphName, "Adjacency List", "Kruskal", edgeCount, mstCount, cost, memory,
        end - start, mst);

    free(edgeArray);
    freeAdjList(adj);
}

static void runPrim(int caseNo, const char* graphName, Edge* edges, int edgeCount) {
    int matrix[VERTICES][VERTICES];
    int key[VERTICES];
    int parent[VERTICES];
    int visited[VERTICES];
    Edge mst[VERTICES - 1];
    int mstCount;
    int cost;
    size_t memory;
    double start;
    double end;

    buildMatrix(edges, edgeCount, matrix);

    start = nowMs();
    mstCount = prim(matrix, key, parent, visited, mst, &cost);
    end = nowMs();

    memory = sizeof(int) * VERTICES * VERTICES
        + sizeof(int) * VERTICES * 3
        + sizeof(Edge) * (VERTICES - 1);

    printResult(caseNo, graphName, "Adjacency Matrix", "Prim", edgeCount, mstCount, cost, memory,
        end - start, mst);
}

int main(void) {
    Edge sparseEdges[SPARSE_EDGES];
    Edge denseEdges[DENSE_EDGES];

    srand((unsigned int)time(NULL));

    generateGraph(sparseEdges, SPARSE_EDGES);
    generateGraph(denseEdges, DENSE_EDGES);

    runKruskal(1, "Sparse Graph", sparseEdges, SPARSE_EDGES);
    runKruskal(2, "Dense Graph", denseEdges, DENSE_EDGES);
    runPrim(3, "Sparse Graph", sparseEdges, SPARSE_EDGES);
    runPrim(4, "Dense Graph", denseEdges, DENSE_EDGES);

    return 0;
}