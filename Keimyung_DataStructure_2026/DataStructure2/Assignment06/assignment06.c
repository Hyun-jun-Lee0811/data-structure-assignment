#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RAW_COUNT 100
#define SEARCH_COUNT 50
#define MAX_VALUE 1000
#define MAX_ARRAY 100

typedef struct Node {
    int data;
    int height;
    struct Node* left;
    struct Node* right;
} Node;

int maxInt(int a, int b) {
    if (a > b) {
        return a;
    }
    else {
        return b;
    }
}

Node* createNode(int value) {
    Node* newNode;

    newNode = (Node*)malloc(sizeof(Node));

    if (newNode == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    newNode->data = value;
    newNode->height = 1;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

int getH(Node* node) {
    if (node == NULL) {
        return 0;
    }
    else {
        return node->height;
    }
}

int computeHeight(Node* node) {
    int leftHeight;
    int rightHeight;

    if (node == NULL) {
        return 0;
    }

    leftHeight = computeHeight(node->left);
    rightHeight = computeHeight(node->right);

    return 1 + maxInt(leftHeight, rightHeight);
}


int countNodes(Node* node) {
    if (node == NULL) {
        return 0;
    }

    return 1 + countNodes(node->left) + countNodes(node->right);
}


void destroyTree(Node* node) {
    if (node == NULL) {
        return;
    }

    destroyTree(node->left);
    destroyTree(node->right);

    free(node);
}

int arrayContains(int arr[], int len, int value, long* comparisons) {
    int i;

    *comparisons = 0;

    for (i = 0; i < len; i++) {
        (*comparisons)++;

        if (arr[i] == value) {
            return 1;
        }
    }

    return 0;
}

Node* insertBST(Node* root, int value, long* comparisons) {
    if (root == NULL) {
        return createNode(value);
    }

    (*comparisons)++;

    if (value < root->data) {
        root->left = insertBST(root->left, value, comparisons);
    }
    else if (value > root->data) {
        root->right = insertBST(root->right, value, comparisons);
    }
    else {
        return root;
    }

    return root;
}

Node* rotateRight(Node* y) {
    Node* x;
    Node* t2;

    x = y->left;
    t2 = x->right;

    x->right = y;
    y->left = t2;

    y->height = 1 + maxInt(
        getH(y->left),
        getH(y->right)
    );

    x->height = 1 + maxInt(
        getH(x->left),
        getH(x->right)
    );

    return x;
}


Node* rotateLeft(Node* x) {
    Node* y;
    Node* t2;

    y = x->right;
    t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = 1 + maxInt(
        getH(x->left),
        getH(x->right)
    );

    y->height = 1 + maxInt(
        getH(y->left),
        getH(y->right)
    );

    return y;
}


Node* insertAVL(Node* node, int value, long* comparisons) {
    int balance;

    if (node == NULL) {
        return createNode(value);
    }

    (*comparisons)++;

    if (value < node->data) {
        node->left = insertAVL(
            node->left,
            value,
            comparisons
        );
    }
    else if (value > node->data) {
        node->right = insertAVL(
            node->right,
            value,
            comparisons
        );
    }
    else {
        return node;
    }

    node->height = 1 + maxInt(
        getH(node->left),
        getH(node->right)
    );

    balance = getH(node->left) - getH(node->right);


    if (balance > 1 && value < node->left->data) {
        return rotateRight(node);
    }


    if (balance < -1 && value > node->right->data) {
        return rotateLeft(node);
    }

    if (balance > 1 && value > node->left->data) {
        node->left = rotateLeft(node->left);

        return rotateRight(node);
    }

    if (balance < -1 && value < node->right->data) {
        node->right = rotateRight(node->right);

        return rotateLeft(node);
    }

    return node;
}


int treeSearch(Node* root, int key, long* comparisons) {
    Node* cur;

    *comparisons = 0;

    cur = root;

    while (cur != NULL) {

        (*comparisons)++;

        if (key == cur->data) {
            return 1;
        }
        else if (key < cur->data) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
    }

    return 0;
}

void printComma(long n) {
    if (n < 0) {
        printf("-");
        n = -n;
    }

    if (n >= 1000) {
        printComma(n / 1000);
        printf(",%03ld", n % 1000);
    }
    else {
        printf("%ld", n);
    }
}


int main(void) {

    int rawNumbers[RAW_COUNT];
    int dataArray[MAX_ARRAY];
    int len;
    int searchKeys[SEARCH_COUNT];
    Node* bstRoot;
    Node* avlRoot;
    long comparisons;
    long arrayTotal;
    long bstTotal;
    long avlTotal;
    long seqComparisons[SEARCH_COUNT];
    long bstComparisons[SEARCH_COUNT];
    long avlComparisons[SEARCH_COUNT];
    int seqFound[SEARCH_COUNT];
    int bstFound[SEARCH_COUNT];
    int avlFound[SEARCH_COUNT];
    long totalSeq;
    long totalBst;
    long totalAvl;
    int i;
    int value;


    srand((unsigned int)time(NULL));


    len = 0;

    bstRoot = NULL;
    avlRoot = NULL;

    arrayTotal = 0;
    bstTotal = 0;
    avlTotal = 0;


    for (i = 0; i < RAW_COUNT; i++) {

        value = rand() % (MAX_VALUE + 1);
        rawNumbers[i] = value;
        comparisons = 0;

        if (!arrayContains(
            dataArray,
            len,
            value,
            &comparisons
        )) {

            dataArray[len] = value;
            len++;
        }

        arrayTotal += comparisons;

        comparisons = 0;

        bstRoot = insertBST(
            bstRoot,
            value,
            &comparisons
        );

        bstTotal += comparisons;

        comparisons = 0;

        avlRoot = insertAVL(
            avlRoot,
            value,
            &comparisons
        );

        avlTotal += comparisons;
    }

    printf("1. 생성된 %d개의 정수\n", RAW_COUNT);

    printf("(발생 순서, 중복 포함)\n\n");

    for (i = 0; i < RAW_COUNT; i++) {

        printf("%4d ", rawNumbers[i]);

        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }

    printf("\n");
    printf("2. 삽입 결과\n");
    printf("실제로 저장된 서로 다른 값의 수 : %d\n", len);

    printf("중복으로 인해 삽입되지 않은 값의 수 : %d\n",
        RAW_COUNT - len);

    printf("\n");

    printf("3. 삽입 과정에서 발생한 총 비교 횟수\n");

    printf("배열 비교 횟수 : ");
    printComma(arrayTotal);
    printf("\n");

    printf("BST 비교 횟수  : ");
    printComma(bstTotal);
    printf("\n");

    printf("AVL 비교 횟수  : ");
    printComma(avlTotal);
    printf("\n\n");

    printf("4. 자료구조의 크기 및 높이\n");

    printf("배열의 길이 : %d\n", len);

    printf("BST의 높이  : %d\n",
        computeHeight(bstRoot));

    printf("AVL의 높이  : %d\n",
        computeHeight(avlRoot));

    printf("BST 노드 수 : %d\n",
        countNodes(bstRoot));

    printf("AVL 노드 수 : %d\n",
        countNodes(avlRoot));

    printf("\n");

    for (i = 0; i < SEARCH_COUNT; i++) {
        searchKeys[i] =
            rand() % (MAX_VALUE + 1);
    }
    printf("5. 탐색 대상 %d개 및 탐색 결과\n", SEARCH_COUNT);

    printf(
        "%-5s %-7s %-10s %-10s %-10s %-10s %-10s %-10s\n",
        "번호",
        "탐색값",
        "배열결과",
        "배열비교",
        "BST결과",
        "BST비교",
        "AVL결과",
        "AVL비교"
    );

    totalSeq = 0;
    totalBst = 0;
    totalAvl = 0;

    for (i = 0; i < SEARCH_COUNT; i++) {
        seqFound[i] = arrayContains(
            dataArray,
            len,
            searchKeys[i],
            &seqComparisons[i]
        );

        bstFound[i] = treeSearch(
            bstRoot,
            searchKeys[i],
            &bstComparisons[i]
        );
        avlFound[i] = treeSearch(
            avlRoot,
            searchKeys[i],
            &avlComparisons[i]
        );


        totalSeq += seqComparisons[i];
        totalBst += bstComparisons[i];
        totalAvl += avlComparisons[i];

        printf(
            "%-5d %-7d %-10s %-10ld %-10s %-10ld %-10s %-10ld\n",
            i + 1,
            searchKeys[i],

            seqFound[i] ? "성공" : "실패",
            seqComparisons[i],

            bstFound[i] ? "성공" : "실패",
            bstComparisons[i],

            avlFound[i] ? "성공" : "실패",
            avlComparisons[i]
        );
    }


    printf("\n");
    printf("6. 탐색 결과 요약\n");
    printf("탐색 횟수 : %d\n\n", SEARCH_COUNT);
    printf("[순차 탐색]\n");
    printf("총 비교 횟수   : ");
    printComma(totalSeq);
    printf("\n");
    printf(
        "평균 비교 횟수 : %.2f\n\n",
        (double)totalSeq / SEARCH_COUNT
    );

    printf("[BST 탐색]\n");
    printf("총 비교 횟수   : ");
    printComma(totalBst);
    printf("\n");

    printf(
        "평균 비교 횟수 : %.2f\n\n",
        (double)totalBst / SEARCH_COUNT
    );
    printf("[AVL 탐색]\n");

    printf("총 비교 횟수   : ");
    printComma(totalAvl);
    printf("\n");

    printf(
        "평균 비교 횟수 : %.2f\n\n",
        (double)totalAvl / SEARCH_COUNT
    );

    destroyTree(bstRoot);
    bstRoot = NULL;

    destroyTree(avlRoot);
    avlRoot = NULL;


    return 0;
}