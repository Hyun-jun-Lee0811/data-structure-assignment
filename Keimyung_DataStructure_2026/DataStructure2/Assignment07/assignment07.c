#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#define VERTEX_COUNT 100
#define SPARSE_EDGE_COUNT 100
#define DENSE_EDGE_COUNT 4000

typedef struct {
    bool exists[VERTEX_COUNT][VERTEX_COUNT];
    int edges[DENSE_EDGE_COUNT][2];
    int edgeCount;
} GraphData;

typedef struct {
    int insertU;
    int insertV;
    int deleteU;
    int deleteV;
    int checkU;
    int checkV;
    int adjacentVertex;
} OperationSet;

typedef struct {
    size_t memory;
    int insertion;
    int deletion;
    int connectivity;
    int adjacent;
    int adjacentVertices[VERTEX_COUNT];
    int adjacentCount;
} Result;

typedef struct {
    int matrix[VERTEX_COUNT][VERTEX_COUNT];
} AdjacencyMatrix;

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct {
    Node* heads[VERTEX_COUNT];
    size_t nodeCount;
} AdjacencyList;

int randomInt(int bound) {
    return rand() % bound;
}

void initMatrix(AdjacencyMatrix* graph) {
    for (int i = 0; i < VERTEX_COUNT; i++) {
        for (int j = 0; j < VERTEX_COUNT; j++) {
            graph->matrix[i][j] = 0;
        }
    }
}

void buildMatrix(AdjacencyMatrix* graph, int u, int v) {
    graph->matrix[u][v] = 1;
    graph->matrix[v][u] = 1;
}

size_t matrixMemory(const AdjacencyMatrix* graph) {
    return sizeof(*graph);
}

void matrixInsert(AdjacencyMatrix* graph, int u, int v, int* comparisons) {
    (*comparisons)++;
    if (graph->matrix[u][v] == 0) {
        graph->matrix[u][v] = 1;
        graph->matrix[v][u] = 1;
    }
}

void matrixRemove(AdjacencyMatrix* graph, int u, int v, int* comparisons) {
    (*comparisons)++;
    if (graph->matrix[u][v] == 1) {
        graph->matrix[u][v] = 0;
        graph->matrix[v][u] = 0;
    }
}

bool matrixHasEdge(const AdjacencyMatrix* graph, int u, int v, int* comparisons) {
    (*comparisons)++;
    return graph->matrix[u][v] == 1;
}

void matrixAdjacent(const AdjacencyMatrix* graph, int u, Result* result) {
    for (int v = 0; v < VERTEX_COUNT; v++) {
        result->adjacent++;
        if (graph->matrix[u][v] == 1) {
            result->adjacentVertices[result->adjacentCount] = v;
            result->adjacentCount++;
        }
    }
}

void initList(AdjacencyList* graph) {
    for (int i = 0; i < VERTEX_COUNT; i++) {
        graph->heads[i] = NULL;
    }
    graph->nodeCount = 0;
}

void freeList(AdjacencyList* graph) {
    for (int i = 0; i < VERTEX_COUNT; i++) {
        Node* cur = graph->heads[i];
        while (cur != NULL) {
            Node* next = cur->next;
            free(cur);
            cur = next;
        }
        graph->heads[i] = NULL;
    }
    graph->nodeCount = 0;
}

void listPushFront(AdjacencyList* graph, int u, int v) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        exit(1);
    }
    node->vertex = v;
    node->next = graph->heads[u];
    graph->heads[u] = node;
    graph->nodeCount++;
}

void listRemoveFrom(AdjacencyList* graph, int u, int v, int* comparisons) {
    Node* prev = NULL;
    Node* cur = graph->heads[u];
    while (cur != NULL) {
        (*comparisons)++;
        if (cur->vertex == v) {
            if (prev == NULL) {
                graph->heads[u] = cur->next;
            }
            else {
                prev->next = cur->next;
            }
            free(cur);
            graph->nodeCount--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void buildList(AdjacencyList* graph, int u, int v) {
    listPushFront(graph, u, v);
    listPushFront(graph, v, u);
}

size_t listMemory(const AdjacencyList* graph) {
    return sizeof(*graph) + graph->nodeCount * sizeof(Node);
}

void listInsert(AdjacencyList* graph, int u, int v, int* comparisons) {
    Node* cur = graph->heads[u];
    while (cur != NULL) {
        (*comparisons)++;
        if (cur->vertex == v) {
            return;
        }
        cur = cur->next;
    }
    listPushFront(graph, u, v);
    listPushFront(graph, v, u);
}

void listRemove(AdjacencyList* graph, int u, int v, int* comparisons) {
    listRemoveFrom(graph, u, v, comparisons);
    listRemoveFrom(graph, v, u, comparisons);
}

bool listHasEdge(const AdjacencyList* graph, int u, int v, int* comparisons) {
    const Node* cur = graph->heads[u];
    while (cur != NULL) {
        (*comparisons)++;
        if (cur->vertex == v) {
            return true;
        }
        cur = cur->next;
    }
    return false;
}

void listAdjacent(const AdjacencyList* graph, int u, Result* result) {
    const Node* cur = graph->heads[u];
    while (cur != NULL) {
        result->adjacent++;
        result->adjacentVertices[result->adjacentCount] = cur->vertex;
        result->adjacentCount++;
        cur = cur->next;
    }
}

void generateGraph(GraphData* data, int edgeCount) {
    for (int i = 0; i < VERTEX_COUNT; i++) {
        for (int j = 0; j < VERTEX_COUNT; j++) {
            data->exists[i][j] = false;
        }
    }
    data->edgeCount = 0;

    while (data->edgeCount < edgeCount) {
        int u = randomInt(VERTEX_COUNT);
        int v = randomInt(VERTEX_COUNT);
        if (u == v || data->exists[u][v]) {
            continue;
        }
        data->exists[u][v] = true;
        data->exists[v][u] = true;
        data->edges[data->edgeCount][0] = u;
        data->edges[data->edgeCount][1] = v;
        data->edgeCount++;
    }
}

OperationSet selectOperations(const GraphData* data) {
    OperationSet ops;

    while (true) {
        int u = randomInt(VERTEX_COUNT);
        int v = randomInt(VERTEX_COUNT);
        if (u != v && !data->exists[u][v]) {
            ops.insertU = u;
            ops.insertV = v;
            break;
        }
    }

    int index = randomInt(data->edgeCount);
    ops.deleteU = data->edges[index][0];
    ops.deleteV = data->edges[index][1];

    while (true) {
        int u = randomInt(VERTEX_COUNT);
        int v = randomInt(VERTEX_COUNT);
        if (u != v) {
            ops.checkU = u;
            ops.checkV = v;
            break;
        }
    }

    ops.adjacentVertex = randomInt(VERTEX_COUNT);
    return ops;
}

void resetResult(Result* result) {
    result->memory = 0;
    result->insertion = 0;
    result->deletion = 0;
    result->connectivity = 0;
    result->adjacent = 0;
    result->adjacentCount = 0;
}

void measureMatrix(const GraphData* data, const OperationSet* ops, Result* result) {
    AdjacencyMatrix graph;
    initMatrix(&graph);
    resetResult(result);

    for (int i = 0; i < data->edgeCount; i++) {
        buildMatrix(&graph, data->edges[i][0], data->edges[i][1]);
    }

    result->memory = matrixMemory(&graph);
    matrixInsert(&graph, ops->insertU, ops->insertV, &result->insertion);
    matrixRemove(&graph, ops->deleteU, ops->deleteV, &result->deletion);
    matrixHasEdge(&graph, ops->checkU, ops->checkV, &result->connectivity);
    matrixAdjacent(&graph, ops->adjacentVertex, result);
}

void measureList(const GraphData* data, const OperationSet* ops, Result* result) {
    AdjacencyList graph;
    initList(&graph);
    resetResult(result);

    for (int i = 0; i < data->edgeCount; i++) {
        buildList(&graph, data->edges[i][0], data->edges[i][1]);
    }

    result->memory = listMemory(&graph);
    listInsert(&graph, ops->insertU, ops->insertV, &result->insertion);
    listRemove(&graph, ops->deleteU, ops->deleteV, &result->deletion);
    listHasEdge(&graph, ops->checkU, ops->checkV, &result->connectivity);
    listAdjacent(&graph, ops->adjacentVertex, result);

    freeList(&graph);
}

void printResult(int caseNumber, const char* title, const Result* result, int adjacentVertex) {
    printf("Case %d : %s\n", caseNumber, title);
    printf("Memory                         : %zu Bytes\n", result->memory);
    printf("Edge insertion comparisons     : %d\n", result->insertion);
    printf("Edge deletion comparisons      : %d\n", result->deletion);
    printf("Connectivity check comparisons : %d\n", result->connectivity);
    printf("Adjacent vertices comparisons  : %d\n", result->adjacent);
    printf("Adjacent vertices of %d :", adjacentVertex);
    for (int i = 0; i < result->adjacentCount; i++) {
        printf(" %d", result->adjacentVertices[i]);
    }
    printf("\n\n");
}

int main(void) {
    srand((unsigned int)time(NULL));

    static GraphData sparse;
    static GraphData dense;
    static Result r1;
    static Result r2;
    static Result r3;
    static Result r4;

    generateGraph(&sparse, SPARSE_EDGE_COUNT);
    generateGraph(&dense, DENSE_EDGE_COUNT);

    OperationSet sparseOps = selectOperations(&sparse);
    OperationSet denseOps = selectOperations(&dense);

    measureMatrix(&sparse, &sparseOps, &r1);
    measureList(&sparse, &sparseOps, &r2);
    measureMatrix(&dense, &denseOps, &r3);
    measureList(&dense, &denseOps, &r4);

    printResult(1, "Sparse Graph - Adjacency Matrix", &r1, sparseOps.adjacentVertex);
    printResult(2, "Sparse Graph - Adjacency List", &r2, sparseOps.adjacentVertex);
    printResult(3, "Dense Graph - Adjacency Matrix", &r3, denseOps.adjacentVertex);
    printResult(4, "Dense Graph - Adjacency List", &r4, denseOps.adjacentVertex);

    return 0;
}