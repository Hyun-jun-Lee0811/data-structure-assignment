#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEAT 5
#define STACK_SIZE 64

typedef struct {
    long long comparisons;
    long long moves;
    size_t peakMemory;
    double timeMs;
    size_t finalCount;
} Metrics;

typedef struct {
    int isInsert;
    int value;
} Operation;

typedef struct {
    int lo;
    int hi;
} Range;

static void* xmalloc(size_t bytes) {
    void* p = malloc(bytes);
    if (p == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    return p;
}

static void updatePeak(Metrics* m, size_t bytes) {
    if (bytes > m->peakMemory) {
        m->peakMemory = bytes;
    }
}

static uint32_t nextRandom(uint64_t* state) {
    uint64_t x = *state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *state = x;
    return (uint32_t)((x * 2685821657736338717ULL) >> 32);
}

typedef struct {
    int* data;
    size_t size;
    size_t capacity;
    Metrics* m;
} SortedArray;

static void arrayInit(SortedArray* a, const int* src, size_t n, Metrics* m) {
    size_t i;
    a->data = (int*)xmalloc(n * sizeof(int));
    a->size = n;
    a->capacity = n;
    a->m = m;
    for (i = 0; i < n; ++i) {
        a->data[i] = src[i];
    }
    updatePeak(m, a->capacity * sizeof(int));
}

static void arrayFree(SortedArray* a) {
    free(a->data);
    a->data = NULL;
}

static int arrayLess(SortedArray* a, int x, int y) {
    ++a->m->comparisons;
    return x < y;
}

static void arraySwap(SortedArray* a, int i, int j) {
    int tmp = a->data[i];
    a->data[i] = a->data[j];
    a->data[j] = tmp;
    a->m->moves += 3;
}

static void arrayPartition(SortedArray* a, int lo, int hi, int* outI, int* outJ) {
    int mid = lo + (hi - lo) / 2;
    int pivot;
    int i;
    int j;

    if (arrayLess(a, a->data[mid], a->data[lo])) {
        arraySwap(a, lo, mid);
    }
    if (arrayLess(a, a->data[hi], a->data[lo])) {
        arraySwap(a, lo, hi);
    }
    if (arrayLess(a, a->data[hi], a->data[mid])) {
        arraySwap(a, mid, hi);
    }
    pivot = a->data[mid];
    ++a->m->moves;

    i = lo;
    j = hi;
    while (i <= j) {
        while (arrayLess(a, a->data[i], pivot)) {
            ++i;
        }
        while (arrayLess(a, pivot, a->data[j])) {
            --j;
        }
        if (i <= j) {
            if (i != j) {
                arraySwap(a, i, j);
            }
            ++i;
            --j;
        }
    }
    *outI = i;
    *outJ = j;
}

static void arrayQuickSort(SortedArray* a) {
    Range stk[STACK_SIZE];
    int top = 0;

    if (a->size < 2) {
        return;
    }
    stk[top].lo = 0;
    stk[top].hi = (int)a->size - 1;
    ++top;

    while (top > 0) {
        int lo;
        int hi;
        --top;
        lo = stk[top].lo;
        hi = stk[top].hi;
        while (lo < hi) {
            int i = 0;
            int j = 0;
            arrayPartition(a, lo, hi, &i, &j);
            if (j - lo < hi - i) {
                if (i < hi) {
                    stk[top].lo = i;
                    stk[top].hi = hi;
                    ++top;
                    updatePeak(a->m, a->capacity * sizeof(int) + (size_t)top * sizeof(Range));
                }
                hi = j;
            }
            else {
                if (lo < j) {
                    stk[top].lo = lo;
                    stk[top].hi = j;
                    ++top;
                    updatePeak(a->m, a->capacity * sizeof(int) + (size_t)top * sizeof(Range));
                }
                lo = i;
            }
        }
    }
}

static void arrayGrow(SortedArray* a) {
    size_t newCap = a->capacity * 2;
    int* nd = (int*)xmalloc(newCap * sizeof(int));
    size_t i;

    updatePeak(a->m, (a->capacity + newCap) * sizeof(int));
    for (i = 0; i < a->size; ++i) {
        nd[i] = a->data[i];
        ++a->m->moves;
    }
    free(a->data);
    a->data = nd;
    a->capacity = newCap;
}

static void arrayInsert(SortedArray* a, int value) {
    size_t pos = 0;
    size_t k;

    while (pos < a->size&& arrayLess(a, a->data[pos], value)) {
        ++pos;
    }
    if (a->size == a->capacity) {
        arrayGrow(a);
    }
    for (k = a->size; k > pos; --k) {
        a->data[k] = a->data[k - 1];
        ++a->m->moves;
    }
    a->data[pos] = value;
    ++a->m->moves;
    ++a->size;
}

static int arrayErase(SortedArray* a, int value) {
    size_t pos = 0;
    size_t k;

    while (pos < a->size && arrayLess(a, a->data[pos], value)) {
        ++pos;
    }
    if (pos >= a->size) {
        return 0;
    }
    ++a->m->comparisons;
    if (a->data[pos] != value) {
        return 0;
    }
    for (k = pos; k + 1 < a->size; ++k) {
        a->data[k] = a->data[k + 1];
        ++a->m->moves;
    }
    --a->size;
    return 1;
}

typedef struct Node Node;

struct Node {
    int key;
    int count;
    int height;
    Node* left;
    Node* right;
};

typedef struct {
    Node* root;
    Metrics* m;
    size_t nodeCount;
    size_t peakNodes;
    size_t maxDepth;
    size_t total;
} AvlTree;

static void avlInit(AvlTree* t, Metrics* m) {
    t->root = NULL;
    t->m = m;
    t->nodeCount = 0;
    t->peakNodes = 0;
    t->maxDepth = 0;
    t->total = 0;
}

static void destroyNodes(Node* node) {
    if (node == NULL) {
        return;
    }
    destroyNodes(node->left);
    destroyNodes(node->right);
    free(node);
}

static void avlFree(AvlTree* t) {
    destroyNodes(t->root);
    t->root = NULL;
}

static void avlUpdatePeak(AvlTree* t) {
    size_t bytes;
    if (t->nodeCount > t->peakNodes) {
        t->peakNodes = t->nodeCount;
    }
    bytes = t->peakNodes * sizeof(Node) + t->maxDepth * sizeof(Node*);
    updatePeak(t->m, bytes);
}

static int heightOf(const Node* n) {
    return n == NULL ? 0 : n->height;
}

static int balanceOf(const Node* n) {
    return n == NULL ? 0 : heightOf(n->left) - heightOf(n->right);
}

static void updateHeight(Node* n) {
    int lh = heightOf(n->left);
    int rh = heightOf(n->right);
    n->height = 1 + (lh > rh ? lh : rh);
}

static Node* rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

static Node* rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    return y;
}

static Node* rebalance(Node* n) {
    int bf;
    updateHeight(n);
    bf = balanceOf(n);
    if (bf > 1) {
        if (balanceOf(n->left) < 0) {
            n->left = rotateLeft(n->left);
        }
        return rotateRight(n);
    }
    if (bf < -1) {
        if (balanceOf(n->right) > 0) {
            n->right = rotateRight(n->right);
        }
        return rotateLeft(n);
    }
    return n;
}

static Node* insertNode(AvlTree* t, Node* node, int value, size_t depth) {
    if (depth > t->maxDepth) {
        t->maxDepth = depth;
    }
    if (node == NULL) {
        Node* created = (Node*)xmalloc(sizeof(Node));
        created->key = value;
        created->count = 1;
        created->height = 1;
        created->left = NULL;
        created->right = NULL;
        ++t->m->moves;
        ++t->nodeCount;
        return created;
    }
    ++t->m->comparisons;
    if (value < node->key) {
        node->left = insertNode(t, node->left, value, depth + 1);
    }
    else {
        ++t->m->comparisons;
        if (value > node->key) {
            node->right = insertNode(t, node->right, value, depth + 1);
        }
        else {
            ++node->count;
            return node;
        }
    }
    return rebalance(node);
}

static Node* detachMin(Node* node, Node** minNode) {
    if (node->left == NULL) {
        *minNode = node;
        return node->right;
    }
    node->left = detachMin(node->left, minNode);
    return rebalance(node);
}

static Node* removeNode(AvlTree* t, Node* node, int value, int* removed, size_t depth) {
    if (depth > t->maxDepth) {
        t->maxDepth = depth;
    }
    if (node == NULL) {
        return NULL;
    }
    ++t->m->comparisons;
    if (value < node->key) {
        node->left = removeNode(t, node->left, value, removed, depth + 1);
    }
    else {
        ++t->m->comparisons;
        if (value > node->key) {
            node->right = removeNode(t, node->right, value, removed, depth + 1);
        }
        else {
            *removed = 1;
            if (node->count > 1) {
                --node->count;
                return node;
            }
            if (node->left == NULL || node->right == NULL) {
                Node* child = (node->left != NULL) ? node->left : node->right;
                free(node);
                --t->nodeCount;
                return child;
            }
            {
                Node* minNode = NULL;
                node->right = detachMin(node->right, &minNode);
                node->key = minNode->key;
                node->count = minNode->count;
                ++t->m->moves;
                free(minNode);
                --t->nodeCount;
            }
        }
    }
    return rebalance(node);
}

static void avlInsert(AvlTree* t, int value) {
    t->root = insertNode(t, t->root, value, 1);
    ++t->total;
    avlUpdatePeak(t);
}

static int avlErase(AvlTree* t, int value) {
    int removed = 0;
    t->root = removeNode(t, t->root, value, &removed, 1);
    if (removed) {
        --t->total;
    }
    avlUpdatePeak(t);
    return removed;
}

static void inorderFill(const Node* node, int* out, size_t* idx) {
    int c;
    if (node == NULL) {
        return;
    }
    inorderFill(node->left, out, idx);
    for (c = 0; c < node->count; ++c) {
        out[*idx] = node->key;
        ++(*idx);
    }
    inorderFill(node->right, out, idx);
}

static double elapsedMs(const struct timespec* start, const struct timespec* end) {
    return (double)(end->tv_sec - start->tv_sec) * 1000.0 +
        (double)(end->tv_nsec - start->tv_nsec) / 1000000.0;
}

static Metrics runQuick(const int* data, int n, const Operation* ops, int opCount,
    int* finalOut) {
    Metrics m;
    SortedArray arr;
    struct timespec start;
    struct timespec end;
    int i;

    memset(&m, 0, sizeof(m));
    arrayInit(&arr, data, (size_t)n, &m);

    timespec_get(&start, TIME_UTC);
    arrayQuickSort(&arr);
    for (i = 0; i < opCount; ++i) {
        if (ops[i].isInsert) {
            arrayInsert(&arr, ops[i].value);
        }
        else {
            arrayErase(&arr, ops[i].value);
        }
    }
    timespec_get(&end, TIME_UTC);

    m.timeMs = elapsedMs(&start, &end);
    m.finalCount = arr.size;
    if (finalOut != NULL) {
        memcpy(finalOut, arr.data, arr.size * sizeof(int));
    }
    arrayFree(&arr);
    return m;
}

static Metrics runAvl(const int* data, int n, const Operation* ops, int opCount,
    int* finalOut) {
    Metrics m;
    AvlTree tree;
    struct timespec start;
    struct timespec end;
    int i;

    memset(&m, 0, sizeof(m));
    avlInit(&tree, &m);

    timespec_get(&start, TIME_UTC);
    for (i = 0; i < n; ++i) {
        avlInsert(&tree, data[i]);
    }
    for (i = 0; i < opCount; ++i) {
        if (ops[i].isInsert) {
            avlInsert(&tree, ops[i].value);
        }
        else {
            avlErase(&tree, ops[i].value);
        }
    }
    timespec_get(&end, TIME_UTC);

    m.timeMs = elapsedMs(&start, &end);
    m.finalCount = tree.total;
    if (finalOut != NULL) {
        size_t idx = 0;
        inorderFill(tree.root, finalOut, &idx);
    }
    avlFree(&tree);
    return m;
}

static int isSortedAscending(const int* a, size_t n) {
    size_t i;
    for (i = 1; i < n; ++i) {
        if (a[i - 1] > a[i]) {
            return 0;
        }
    }
    return 1;
}

static void printRow(const char* label, const char* q, const char* a) {
    printf("%-18s%-16s%s\n", label, q, a);
}

static void printResult(int n, int opCount, const Metrics* q, const Metrics* a) {
    char qs[64];
    char as[64];

    printf("Data Size : %d\n", n);
    printf("Operations: %d\n\n", opCount);
    printf("                  Quick Sort      AVL Tree\n");
    printf("------------------------------------------------\n");

    snprintf(qs, sizeof(qs), "%.3f ms", q->timeMs);
    snprintf(as, sizeof(as), "%.3f ms", a->timeMs);
    printRow("Total Time", qs, as);

    snprintf(qs, sizeof(qs), "%lld", q->comparisons);
    snprintf(as, sizeof(as), "%lld", a->comparisons);
    printRow("Comparisons", qs, as);

    snprintf(qs, sizeof(qs), "%lld", q->moves);
    snprintf(as, sizeof(as), "%lld", a->moves);
    printRow("Data Movements", qs, as);

    snprintf(qs, sizeof(qs), "%zu Bytes", q->peakMemory);
    snprintf(as, sizeof(as), "%zu Bytes", a->peakMemory);
    printRow("Memory Usage", qs, as);

    printf("\n");
    snprintf(qs, sizeof(qs), "%zu", q->finalCount);
    snprintf(as, sizeof(as), "%zu", a->finalCount);
    printRow("Final Data Count", qs, as);
    printf("\n");
}

int main(void) {
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int sizeCount = (int)(sizeof(sizes) / sizeof(sizes[0]));
    int s;

    for (s = 0; s < sizeCount; ++s) {
        int n = sizes[s];
        int opCount = n / 5;
        uint64_t state = 20240615ULL + (uint64_t)n;
        int* data = (int*)xmalloc((size_t)n * sizeof(int));
        Operation* ops = (Operation*)xmalloc((size_t)opCount * sizeof(Operation));
        int* quickFinal = (int*)xmalloc((size_t)(n + opCount) * sizeof(int));
        int* avlFinal = (int*)xmalloc((size_t)(n + opCount) * sizeof(int));
        Metrics quick;
        Metrics avl;
        double quickTime;
        double avlTime;
        int i;
        int r;

        for (i = 0; i < n; ++i) {
            data[i] = 1 + (int)(nextRandom(&state) % (uint32_t)n);
        }
        for (i = 0; i < opCount; ++i) {
            ops[i].isInsert = (nextRandom(&state) % 1000000u) % 2u == 0u;
            ops[i].value = 1 + (int)(nextRandom(&state) % (uint32_t)n);
        }

        quick = runQuick(data, n, ops, opCount, quickFinal);
        avl = runAvl(data, n, ops, opCount, avlFinal);

        if (quick.finalCount != avl.finalCount ||
            memcmp(quickFinal, avlFinal, quick.finalCount * sizeof(int)) != 0 ||
            !isSortedAscending(quickFinal, quick.finalCount)) {
            fprintf(stderr, "검증 실패: n=%d\n", n);
            return 1;
        }

        quickTime = quick.timeMs;
        avlTime = avl.timeMs;
        for (r = 1; r < REPEAT; ++r) {
            quickTime += runQuick(data, n, ops, opCount, NULL).timeMs;
            avlTime += runAvl(data, n, ops, opCount, NULL).timeMs;
        }
        quick.timeMs = quickTime / REPEAT;
        avl.timeMs = avlTime / REPEAT;

        printResult(n, opCount, &quick, &avl);

        free(data);
        free(ops);
        free(quickFinal);
        free(avlFinal);
    }
    return 0;
}