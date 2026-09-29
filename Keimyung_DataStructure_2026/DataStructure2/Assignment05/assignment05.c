#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_COUNT 100
#define SEARCH_COUNT 50
#define MAX_VALUE 1000

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(int value) {
    Node* newNode;

    newNode = (Node*)malloc(sizeof(Node));

    if (newNode == NULL) {
        printf("메모리 할당에 실패했습니다.\n");
        exit(1);
    }

    newNode->data = value;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
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

void destroyBST(Node* root) {
    if (root == NULL) {
        return;
    }

    destroyBST(root->left);
    destroyBST(root->right);
    free(root);
}

int isDuplicate(int arr[], int count, int value) {
    int i;

    for (i = 0; i < count; i++) {
        if (arr[i] == value) {
            return 1;
        }
    }

    return 0;
}

void generateUniqueNumbers(int arr[], int count) {
    int i;
    int value;

    i = 0;
    while (i < count) {
        value = rand() % (MAX_VALUE + 1);
        if (!isDuplicate(arr, i, value)) {
            arr[i] = value;
            i++;
        }
    }
}

void generateSearchKeys(int arr[], int count) {
    int i;

    for (i = 0; i < count; i++) {
        arr[i] = rand() % (MAX_VALUE + 1);
    }
}

int sequentialSearch(int arr[], int n, int key, long* comparisons) {
    int i;

    *comparisons = 0;

    for (i = 0; i < n; i++) {
        (*comparisons)++;
        if (arr[i] == key) {
            return 1;
        }
    }

    return 0;
}

int bstSearch(Node* root, int key, long* comparisons) {
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
    int dataArray[DATA_COUNT];
    int searchKeys[SEARCH_COUNT];
    Node* root;
    long buildComparisons;
    long comparisons;
    long seqComparisons[SEARCH_COUNT];
    long bstComparisons[SEARCH_COUNT];
    int found[SEARCH_COUNT];
    long totalSeq;
    long totalBst;
    int i;

    srand((unsigned int)time(NULL));

    generateUniqueNumbers(dataArray, DATA_COUNT);

    printf("1. 생성된 %d개의 정수 (발생 순서)\n", DATA_COUNT);
    for (i = 0; i < DATA_COUNT; i++) {
        printf("%4d ", dataArray[i]);
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    root = NULL;
    buildComparisons = 0;

    for (i = 0; i < DATA_COUNT; i++) {
        comparisons = 0;
        root = insertBST(root, dataArray[i], &comparisons);
        buildComparisons = buildComparisons + comparisons;
    }

    printf("2. 이진 탐색 트리 생성 결과\n");
    printf("BST 생성 과정에서 발생한 총 비교 횟수 : ");
    printComma(buildComparisons);
    printf("\n");

    generateSearchKeys(searchKeys, SEARCH_COUNT);

    printf("3. 탐색 대상 %d개 및 탐색 결과\n", SEARCH_COUNT);
    printf("%-6s %-8s %-8s %-14s %-14s\n", "번호", "탐색값", "결과", "순차비교횟수", "BST비교횟수");

    totalSeq = 0;
    totalBst = 0;

    for (i = 0; i < SEARCH_COUNT; i++) {
        found[i] = sequentialSearch(dataArray, DATA_COUNT, searchKeys[i], &seqComparisons[i]);
        bstSearch(root, searchKeys[i], &bstComparisons[i]);

        totalSeq = totalSeq + seqComparisons[i];
        totalBst = totalBst + bstComparisons[i];

        printf("%-6d %-8d %-8s %-14ld %-14ld\n", i + 1, searchKeys[i],
            found[i] ? "성공" : "실패", seqComparisons[i], bstComparisons[i]);
    }

    printf("4. 탐색 결과 요약\n");
    printf("탐색 횟수 : %d\n\n", SEARCH_COUNT);

    printf("[순차 탐색]\n");
    printf("총 비교 횟수: ");
    printComma(totalSeq);
    printf("\n");
    printf("평균 비교 횟수 : %.2f\n\n", (double)totalSeq / SEARCH_COUNT);

    printf("[BST 탐색]\n");
    printf("총 비교 횟수: ");
    printComma(totalBst);
    printf("\n");
    printf("평균 비교 횟수 : %.2f\n\n", (double)totalBst / SEARCH_COUNT);

    printf("5. BST 생성 비용을 포함한 비교\n");
    printf("BST 생성 비용: ");
    printComma(buildComparisons);
    printf("\n");
    printf("BST 탐색 총 비교 횟수: ");
    printComma(totalBst);
    printf("\n");
    printf("BST 생성 + 탐색 합계: ");
    printComma(buildComparisons + totalBst);
    printf("\n");
    printf("순차 탐색 총 비교 횟수: ");
    printComma(totalSeq);
    printf("\n");

    destroyBST(root);
    root = NULL;

    return 0;
}