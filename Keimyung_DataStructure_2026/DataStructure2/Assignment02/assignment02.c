#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 32
#define MAX_INPUT 1024
#define ARRAY_SIZE 100000

typedef struct {
    char name[MAX_NAME];
    int used;
} ArrayNode;

typedef struct TreeNode {
    char name[MAX_NAME];
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

static ArrayNode arrayTree[ARRAY_SIZE];
static int arrayMaxIndex = 0;
static TreeNode* linkedRoot = NULL;

static char input[MAX_INPUT];
static int pos = 0;
static int errorFlag = 0;
static int hasTree = 0;

static void skipSpace(void) {
    while (input[pos] == ' ' || input[pos] == '\t') {
        pos++;
    }
}

static int readName(char* out) {
    int len = 0;
    skipSpace();
    while (isalnum((unsigned char)input[pos]) || input[pos] == '_') {
        if (len < MAX_NAME - 1) {
            out[len] = input[pos];
            len++;
        }
        pos++;
    }
    out[len] = '\0';
    if (len == 0) {
        return 0;
    }
    return 1;
}

static void parseArray(int index) {
    char name[MAX_NAME];
    if (errorFlag == 1) {
        return;
    }
    if (index < 1 || index >= ARRAY_SIZE) {
        errorFlag = 1;
        return;
    }
    if (readName(name) == 0) {
        errorFlag = 1;
        return;
    }
    strcpy(arrayTree[index].name, name);
    arrayTree[index].used = 1;
    if (index > arrayMaxIndex) {
        arrayMaxIndex = index;
    }
    skipSpace();
    if (input[pos] != '(') {
        return;
    }
    pos++;
    skipSpace();
    if (input[pos] != ',') {
        parseArray(index * 2);
        if (errorFlag == 1) {
            return;
        }
    }
    skipSpace();
    if (input[pos] != ',') {
        errorFlag = 1;
        return;
    }
    pos++;
    skipSpace();
    if (input[pos] != ')') {
        parseArray(index * 2 + 1);
        if (errorFlag == 1) {
            return;
        }
    }
    skipSpace();
    if (input[pos] != ')') {
        errorFlag = 1;
        return;
    }
    pos++;
}

static TreeNode* parseLinked(void) {
    char name[MAX_NAME];
    TreeNode* node;
    if (errorFlag == 1) {
        return NULL;
    }
    if (readName(name) == 0) {
        errorFlag = 1;
        return NULL;
    }
    node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        printf("메모리 할당에 실패했습니다.\n");
        exit(1);
    }
    strcpy(node->name, name);
    node->left = NULL;
    node->right = NULL;
    skipSpace();
    if (input[pos] != '(') {
        return node;
    }
    pos++;
    skipSpace();
    if (input[pos] != ',') {
        node->left = parseLinked();
        if (errorFlag == 1) {
            return node;
        }
    }
    skipSpace();
    if (input[pos] != ',') {
        errorFlag = 1;
        return node;
    }
    pos++;
    skipSpace();
    if (input[pos] != ')') {
        node->right = parseLinked();
        if (errorFlag == 1) {
            return node;
        }
    }
    skipSpace();
    if (input[pos] != ')') {
        errorFlag = 1;
        return node;
    }
    pos++;
    return node;
}

static void freeLinked(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    freeLinked(node->left);
    freeLinked(node->right);
    free(node);
}

static void clearTrees(void) {
    int i;
    for (i = 1; i <= arrayMaxIndex; i++) {
        arrayTree[i].used = 0;
    }
    arrayMaxIndex = 0;
    freeLinked(linkedRoot);
    linkedRoot = NULL;
    hasTree = 0;
}

static int buildTrees(const char* text) {
    clearTrees();
    strncpy(input, text, MAX_INPUT - 1);
    input[MAX_INPUT - 1] = '\0';

    pos = 0;
    errorFlag = 0;
    parseArray(1);
    if (errorFlag == 0) {
        skipSpace();
        if (input[pos] != '\0') {
            errorFlag = 1;
        }
    }
    if (errorFlag == 1) {
        clearTrees();
        return 0;
    }

    pos = 0;
    errorFlag = 0;
    linkedRoot = parseLinked();
    if (errorFlag == 0) {
        skipSpace();
        if (input[pos] != '\0') {
            errorFlag = 1;
        }
    }
    if (errorFlag == 1) {
        clearTrees();
        return 0;
    }

    hasTree = 1;
    return 1;
}

static int arrayExists(int index) {
    if (index < 1 || index > arrayMaxIndex) {
        return 0;
    }
    return arrayTree[index].used;
}

static void printIndent(int depth) {
    int i;
    for (i = 0; i < depth; i++) {
        printf("    ");
    }
}

static void printArrayTree(int index, int depth) {
    if (arrayExists(index) == 0) {
        return;
    }
    printIndent(depth);
    printf("%s\n", arrayTree[index].name);
    printArrayTree(index * 2, depth + 1);
    printArrayTree(index * 2 + 1, depth + 1);
}

static void printLinkedTree(TreeNode* node, int depth) {
    if (node == NULL) {
        return;
    }
    printIndent(depth);
    printf("%s\n", node->name);
    printLinkedTree(node->left, depth + 1);
    printLinkedTree(node->right, depth + 1);
}

static int arrayChildCount(int index) {
    int count = 0;
    if (arrayExists(index * 2) == 1) {
        count++;
    }
    if (arrayExists(index * 2 + 1) == 1) {
        count++;
    }
    return count;
}

static int arrayNodeCount(void) {
    int i;
    int count = 0;
    for (i = 1; i <= arrayMaxIndex; i++) {
        if (arrayTree[i].used == 1) {
            count++;
        }
    }
    return count;
}

static int arrayLeafCount(void) {
    int i;
    int count = 0;
    for (i = 1; i <= arrayMaxIndex; i++) {
        if (arrayTree[i].used == 1 && arrayChildCount(i) == 0) {
            count++;
        }
    }
    return count;
}

static int arrayHeight(int index) {
    int left;
    int right;
    if (arrayExists(index) == 0) {
        return 0;
    }
    left = arrayHeight(index * 2);
    right = arrayHeight(index * 2 + 1);
    if (left > right) {
        return left + 1;
    }
    return right + 1;
}

static int arrayDegree(void) {
    int i;
    int degree = 0;
    int count;
    for (i = 1; i <= arrayMaxIndex; i++) {
        if (arrayTree[i].used == 1) {
            count = arrayChildCount(i);
            if (count > degree) {
                degree = count;
            }
        }
    }
    return degree;
}

static int linkedNodeCount(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + linkedNodeCount(node->left) + linkedNodeCount(node->right);
}

static int linkedLeafCount(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    if (node->left == NULL && node->right == NULL) {
        return 1;
    }
    return linkedLeafCount(node->left) + linkedLeafCount(node->right);
}

static int linkedHeight(TreeNode* node) {
    int left;
    int right;
    if (node == NULL) {
        return 0;
    }
    left = linkedHeight(node->left);
    right = linkedHeight(node->right);
    if (left > right) {
        return left + 1;
    }
    return right + 1;
}

static int linkedChildCount(TreeNode* node) {
    int count = 0;
    if (node->left != NULL) {
        count++;
    }
    if (node->right != NULL) {
        count++;
    }
    return count;
}

static int linkedDegree(TreeNode* node) {
    int current;
    int left;
    int right;
    if (node == NULL) {
        return 0;
    }
    current = linkedChildCount(node);
    left = linkedDegree(node->left);
    right = linkedDegree(node->right);
    if (left > current) {
        current = left;
    }
    if (right > current) {
        current = right;
    }
    return current;
}

static int arrayIsComplete(void) {
    int i;
    int count = arrayNodeCount();
    for (i = 1; i <= arrayMaxIndex; i++) {
        if (arrayTree[i].used == 1 && i > count) {
            return 0;
        }
    }
    return 1;
}

static int arrayIsPerfect(void) {
    int count = arrayNodeCount();
    int height = arrayHeight(1);
    int full = (1 << height) - 1;
    if (arrayIsComplete() == 1 && count == full) {
        return 1;
    }
    return 0;
}

static int arrayIsSkewed(void) {
    if (arrayNodeCount() == 0) {
        return 0;
    }
    if (arrayDegree() <= 1) {
        return 1;
    }
    return 0;
}

static int linkedIsComplete(TreeNode* node, int index, int count) {
    if (node == NULL) {
        return 1;
    }
    if (index > count) {
        return 0;
    }
    if (linkedIsComplete(node->left, index * 2, count) == 0) {
        return 0;
    }
    if (linkedIsComplete(node->right, index * 2 + 1, count) == 0) {
        return 0;
    }
    return 1;
}

static int linkedIsPerfect(TreeNode* node) {
    int count = linkedNodeCount(node);
    int height = linkedHeight(node);
    int full = (1 << height) - 1;
    if (linkedIsComplete(node, 1, count) == 1 && count == full) {
        return 1;
    }
    return 0;
}

static int linkedIsSkewed(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    if (linkedDegree(node) <= 1) {
        return 1;
    }
    return 0;
}

static void showArrayInfo(void) {
    int count = arrayNodeCount();
    int leaf = arrayLeafCount();
    printf("\n[배열 구현] 트리 정보\n");
    printf("전체 노드 수   : %d\n", count);
    printf("단말 노드 수   : %d\n", leaf);
    printf("비단말 노드 수 : %d\n", count - leaf);
    printf("트리의 높이    : %d\n", arrayHeight(1));
    printf("트리의 차수    : %d\n", arrayDegree());
}

static void showLinkedInfo(void) {
    int count = linkedNodeCount(linkedRoot);
    int leaf = linkedLeafCount(linkedRoot);
    printf("\n[연결 자료구조 구현] 트리 정보\n");
    printf("전체 노드 수   : %d\n", count);
    printf("단말 노드 수   : %d\n", leaf);
    printf("비단말 노드 수 : %d\n", count - leaf);
    printf("트리의 높이    : %d\n", linkedHeight(linkedRoot));
    printf("트리의 차수    : %d\n", linkedDegree(linkedRoot));
}

static void showArrayShape(void) {
    printf("\n[배열 구현] 트리 형태 판별\n");
    printf("완전 이진트리  : ");
    if (arrayIsComplete() == 1) {
        printf("예\n");
    }
    else {
        printf("아니오\n");
    }
    printf("포화 이진트리  : ");
    if (arrayIsPerfect() == 1) {
        printf("예\n");
    }
    else {
        printf("아니오\n");
    }
    printf("편향 이진트리  : ");
    if (arrayIsSkewed() == 1) {
        printf("예\n");
    }
    else {
        printf("아니오\n");
    }
}

static void showLinkedShape(void) {
    int count = linkedNodeCount(linkedRoot);
    printf("\n[연결 자료구조 구현] 트리 형태 판별\n");
    printf("완전 이진트리  : ");
    if (linkedIsComplete(linkedRoot, 1, count) == 1) {
        printf("예\n");
    }
    else {
        printf("아니오\n");
    }
    printf("포화 이진트리  : ");
    if (linkedIsPerfect(linkedRoot) == 1) {
        printf("예\n");
    }
    else {
        printf("아니오\n");
    }
    printf("편향 이진트리  : ");
    if (linkedIsSkewed(linkedRoot) == 1) {
        printf("예\n");
    }
    else {
        printf("아니오\n");
    }
}

static void showMemoryUsage(void) {
    int count = arrayNodeCount();
    int slots = arrayMaxIndex + 1;
    long arrayBytes = (long)slots * (long)sizeof(ArrayNode);
    long linkedBytes = (long)count * (long)sizeof(TreeNode);

    printf("\n메모리 사용량 비교\n");
    printf("노드 수 : %d, 높이 : %d\n", count, arrayHeight(1));
    printf("배열 구현 : 노드 1개 %lu바이트 x 배열 칸 %d개 = %ld바이트\n",
        (unsigned long)sizeof(ArrayNode), slots, arrayBytes);
    printf("연결 구현 : 노드 1개 %lu바이트 x 노드 %d개 = %ld바이트\n",
        (unsigned long)sizeof(TreeNode), count, linkedBytes);

    if (arrayBytes < linkedBytes) {
        printf("결과 : 배열 구현이 %ld바이트 적게 사용\n", linkedBytes - arrayBytes);
    }
    else if (arrayBytes > linkedBytes) {
        printf("결과 : 연결 구현이 %ld바이트 적게 사용\n", arrayBytes - linkedBytes);
    }
    else {
        printf("결과 : 두 구현의 사용량이 같음\n");
    }
}

static int arrayFindIndex(const char* name) {
    int i;
    for (i = 1; i <= arrayMaxIndex; i++) {
        if (arrayTree[i].used == 1 && strcmp(arrayTree[i].name, name) == 0) {
            return i;
        }
    }
    return 0;
}

static TreeNode* linkedFind(TreeNode* node, const char* name) {
    TreeNode* found;
    if (node == NULL) {
        return NULL;
    }
    if (strcmp(node->name, name) == 0) {
        return node;
    }
    found = linkedFind(node->left, name);
    if (found != NULL) {
        return found;
    }
    return linkedFind(node->right, name);
}

static TreeNode* linkedFindParent(TreeNode* node, TreeNode* target) {
    TreeNode* found;
    if (node == NULL) {
        return NULL;
    }
    if (node->left == target || node->right == target) {
        return node;
    }
    found = linkedFindParent(node->left, target);
    if (found != NULL) {
        return found;
    }
    return linkedFindParent(node->right, target);
}

static void searchByArray(const char* name) {
    int index = arrayFindIndex(name);
    int parent;
    int sibling;

    printf("\n배열 구현- 노드 %s 탐색\n", name);
    if (index == 0) {
        printf("해당 노드가 존재하지 않습니다.\n");
        return;
    }

    printf("왼쪽 자식 : ");
    if (arrayExists(index * 2) == 1) {
        printf("%s\n", arrayTree[index * 2].name);
    }
    else {
        printf("없음\n");
    }

    printf("오른쪽 자식 : ");
    if (arrayExists(index * 2 + 1) == 1) {
        printf("%s\n", arrayTree[index * 2 + 1].name);
    }
    else {
        printf("없음\n");
    }

    parent = index / 2;
    printf("부모 : ");
    if (arrayExists(parent) == 1) {
        printf("%s\n", arrayTree[parent].name);
    }
    else {
        printf("없음\n");
    }

    if (index % 2 == 0) {
        sibling = index + 1;
    }
    else {
        sibling = index - 1;
    }
    printf("형제 : ");
    if (index > 1 && arrayExists(sibling) == 1) {
        printf("%s\n", arrayTree[sibling].name);
    }
    else {
        printf("없음\n");
    }
}

static void searchByLinked(const char* name) {
    TreeNode* node = linkedFind(linkedRoot, name);
    TreeNode* parent;
    TreeNode* sibling;

    printf("\n연결 자료구조 구현- 노드 %s 탐색\n", name);
    if (node == NULL) {
        printf("해당 노드가 존재하지 않습니다.\n");
        return;
    }

    printf("왼쪽 자식 : ");
    if (node->left != NULL) {
        printf("%s\n", node->left->name);
    }
    else {
        printf("없음\n");
    }

    printf("오른쪽 자식 : ");
    if (node->right != NULL) {
        printf("%s\n", node->right->name);
    }
    else {
        printf("없음\n");
    }

    parent = linkedFindParent(linkedRoot, node);
    printf("부모 : ");
    if (parent != NULL) {
        printf("%s\n", parent->name);
    }
    else {
        printf("없음\n");
    }

    printf("형제 : ");
    if (parent == NULL) {
        printf("없음\n");
    }
    else {
        if (parent->left == node) {
            sibling = parent->right;
        }
        else {
            sibling = parent->left;
        }
        if (sibling != NULL) {
            printf("%s\n", sibling->name);
        }
        else {
            printf("없음\n");
        }
    }
}


static void readLine(char* buffer, int size) {
    int length;
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    length = (int)strlen(buffer);
    while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r')) {
        buffer[length - 1] = '\0';
        length--;
    }
}

int main(void) {
    char line[MAX_INPUT];
    char name[MAX_NAME];
    int menu;

    while (1) {
        printf("1. 이진트리 입력 (괄호 표기법)\n");
        printf("2. 배열 구현 : 출력 / 정보 / 형태 판별\n");
        printf("3. 연결 자료구조 구현 : 출력 / 정보 / 형태 판별\n");
        printf("4. 두 구현의 메모리 사용량 비교\n");
        printf("5. 특정 노드의 자식 / 부모 / 형제 출력\n");
        printf("0. 종료\n");
        printf("선택 : ");
        readLine(line, MAX_INPUT);
        menu = atoi(line);

        if (menu == 0) {
            break;
        }

        if (menu == 1) {
            printf("괄호 표기법 입력 : ");
            readLine(line, MAX_INPUT);
            if (buildTrees(line) == 0) {
                printf("입력 형식이 올바르지 않습니다.\n");
            }
            else {
                printf("트리 생성이 완료되었습니다.\n");
            }
            continue;
        }

        if (hasTree == 0) {
            printf("먼저 1번 메뉴에서 트리를 입력하세요.\n");
            continue;
        }

        if (menu == 2) {
            printf("\n배열 구현- 이진트리 출력\n");
            printArrayTree(1, 0);
            showArrayInfo();
            showArrayShape();
        }
        else if (menu == 3) {
            printf("\n연결 자료구조 구현- 이진트리 출력\n");
            printLinkedTree(linkedRoot, 0);
            showLinkedInfo();
            showLinkedShape();
        }
        else if (menu == 4) {
            showMemoryUsage();
        }
        else if (menu == 5) {
            printf("노드 이름 입력 : ");
            readLine(line, MAX_INPUT);
            strncpy(name, line, MAX_NAME - 1);
            name[MAX_NAME - 1] = '\0';
            searchByArray(name);
            searchByLinked(name);
        }
        else {
            printf("잘못된 선택입니다.\n");
        }
    }

    clearTrees();
    return 0;
}