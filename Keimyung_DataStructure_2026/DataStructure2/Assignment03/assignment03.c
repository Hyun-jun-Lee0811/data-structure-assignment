#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_TOKENS 5
#define MAX_PATH_LEN 64
#define MAX_TOKEN_UPPER 16

#define DIR_LEFT 0
#define DIR_RIGHT 1

typedef struct TreeNode {
    char data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct {
    TreeNode* root;
    int capacity;
    int count;
} BinaryTree;

typedef enum {
    CMD_INSERT,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_READ,
    CMD_PRINT,
    CMD_UNKNOWN
} CommandType;

static BinaryTree* create_btree(int size);
static void destroy_btree(BinaryTree* tree);
static int insert_root(BinaryTree* tree, char value);
static int insert_child(BinaryTree* tree, TreeNode* parent, int dir, char value);
static int delete_node(BinaryTree* tree, TreeNode* leaf, TreeNode* parent, int dir);
static int update_value(TreeNode* node, TreeNode* parent, char value);
static void read_child(TreeNode* parent);
static void print_btree(TreeNode* node, int depth);

static TreeNode* create_node(char value);
static void freeSubtree(TreeNode* node);
static int isValidLetter(char c);
static int isValidDataToken(const char* tok);
static int parsePath(const char* path, char* letters, int maxLen);
static TreeNode* findNode(BinaryTree* tree, const char* path, TreeNode** parentOut, int* dirOut);
static void toUpperCopy(const char* src, char* dst, int maxLen);
static CommandType matchCommand(const char* tok);
static int matchDirection(const char* tok);
static void handleInsert(BinaryTree* tree, char** tokens, int tokenCount);
static void handleDelete(BinaryTree* tree, char** tokens, int tokenCount);
static void handleUpdate(BinaryTree* tree, char** tokens, int tokenCount);
static void handleRead(BinaryTree* tree, char** tokens, int tokenCount);
static void handlePrint(BinaryTree* tree, char** tokens, int tokenCount);
static void processCommand(BinaryTree* tree, char** tokens, int tokenCount);

static BinaryTree* create_btree(int size) {
    BinaryTree* tree = (BinaryTree*)malloc(sizeof(BinaryTree));
    if (tree == NULL) {
        printf("메모리 할당에 실패했습니다.\n");
        exit(1);
    }
    tree->root = NULL;
    tree->capacity = size;
    tree->count = 0;
    return tree;
}

static void destroy_btree(BinaryTree* tree) {
    if (tree == NULL) {
        return;
    }
    freeSubtree(tree->root);
    tree->root = NULL;
    free(tree);
}

static int insert_root(BinaryTree* tree, char value) {
    if (tree->root != NULL) {
        return 0;
    }
    tree->root = create_node(value);
    tree->count++;
    return 1;
}

static int insert_child(BinaryTree* tree, TreeNode* parent, int dir, char value) {
    TreeNode* node;
    if (parent == NULL) {
        return 0;
    }
    if (parent->left != NULL || parent->right != NULL) {
        return 0;
    }
    if (dir == DIR_LEFT) {
        if (parent->right != NULL && parent->right->data == value) {
            return 0;
        }
        node = create_node(value);
        parent->left = node;
    }
    else if (dir == DIR_RIGHT) {
        if (parent->left != NULL && parent->left->data == value) {
            return 0;
        }
        node = create_node(value);
        parent->right = node;
    }
    else {
        return 0;
    }
    tree->count++;
    return 1;
}

static int delete_node(BinaryTree* tree, TreeNode* leaf, TreeNode* parent, int dir) {
    if (leaf == NULL) {
        return 0;
    }
    if (leaf->left != NULL || leaf->right != NULL) {
        return 0;
    }
    if (parent == NULL) {
        tree->root = NULL;
    }
    else if (dir == DIR_LEFT) {
        parent->left = NULL;
    }
    else if (dir == DIR_RIGHT) {
        parent->right = NULL;
    }
    else {
        return 0;
    }
    free(leaf);
    tree->count--;
    return 1;
}

static int update_value(TreeNode* node, TreeNode* parent, char value) {
    if (node == NULL) {
        return 0;
    }
    if (parent != NULL) {
        if (parent->left != NULL && parent->left != node && parent->left->data == value) {
            return 0;
        }
        if (parent->right != NULL && parent->right != node && parent->right->data == value) {
            return 0;
        }
    }
    node->data = value;
    return 1;
}

static void read_child(TreeNode* parent) {
    int printedLeft;
    if (parent->left == NULL && parent->right == NULL) {
        printf("자식이 없습니다.\n");
        return;
    }
    printedLeft = 0;
    if (parent->left != NULL) {
        printf("%c(L)", parent->left->data);
        printedLeft = 1;
    }
    if (parent->right != NULL) {
        if (printedLeft == 1) {
            printf(", ");
        }
        printf("%c(R)", parent->right->data);
    }
    printf("\n");
}

static void print_btree(TreeNode* node, int depth) {
    int i;
    if (node == NULL) {
        return;
    }
    if (depth == 0) {
        printf("%c\n", node->data);
    }
    else {
        for (i = 0; i < depth - 1; i++) {
            printf("    ");
        }
        printf("+---%c\n", node->data);
    }
    print_btree(node->left, depth + 1);
    print_btree(node->right, depth + 1);
}

static TreeNode* create_node(char value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        exit(1);
    }
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static void freeSubtree(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    freeSubtree(node->left);
    freeSubtree(node->right);
    free(node);
}

static int isValidLetter(char c) {
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}

static int isValidDataToken(const char* tok) {
    if (strlen(tok) != 1) {
        return 0;
    }
    if (isValidLetter(tok[0]) == 0) {
        return 0;
    }
    return 1;
}

static int parsePath(const char* path, char* letters, int maxLen) {
    int idx;
    int count;

    if (path == NULL) {
        return -1;
    }
    if (path[0] != '/') {
        return -1;
    }
    idx = 1;
    count = 0;
    if (path[idx] == '\0') {
        return 0;
    }
    while (path[idx] != '\0') {
        if (isValidLetter(path[idx]) == 0) {
            return -1;
        }
        if (count >= maxLen) {
            return -1;
        }
        letters[count] = path[idx];
        count++;
        idx++;
        if (path[idx] == '/') {
            idx++;
            if (path[idx] == '\0') {
                return -1;
            }
        }
        else if (path[idx] != '\0') {
            return -1;
        }
    }
    return count;
}

static TreeNode* findNode(BinaryTree* tree, const char* path, TreeNode** parentOut, int* dirOut) {
    char letters[MAX_PATH_LEN];
    int count;
    TreeNode* current;
    TreeNode* parent;
    int dir;
    int i;

    if (parentOut != NULL) {
        *parentOut = NULL;
    }
    if (dirOut != NULL) {
        *dirOut = -1;
    }

    count = parsePath(path, letters, MAX_PATH_LEN);
    if (count <= 0) {
        return NULL;
    }
    if (tree->root == NULL) {
        return NULL;
    }
    if (tree->root->data != letters[0]) {
        return NULL;
    }

    current = tree->root;
    parent = NULL;
    dir = -1;
    for (i = 1; i < count; i++) {
        if (current->left != NULL && current->left->data == letters[i]) {
            parent = current;
            dir = DIR_LEFT;
            current = current->left;
        }
        else if (current->right != NULL && current->right->data == letters[i]) {
            parent = current;
            dir = DIR_RIGHT;
            current = current->right;
        }
        else {
            return NULL;
        }
    }

    if (parentOut != NULL) {
        *parentOut = parent;
    }
    if (dirOut != NULL) {
        *dirOut = dir;
    }
    return current;
}

static void toUpperCopy(const char* src, char* dst, int maxLen) {
    int i;
    for (i = 0; src[i] != '\0' && i < maxLen - 1; i++) {
        dst[i] = (char)toupper((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static CommandType matchCommand(const char* tok) {
    char upper[MAX_TOKEN_UPPER];
    toUpperCopy(tok, upper, MAX_TOKEN_UPPER);
    if (strlen(upper) == 1) {
        if (upper[0] == 'I') {
            return CMD_INSERT;
        }
        if (upper[0] == 'D') {
            return CMD_DELETE;
        }
        if (upper[0] == 'U') {
            return CMD_UPDATE;
        }
        if (upper[0] == 'R') {
            return CMD_READ;
        }
        if (upper[0] == 'P') {
            return CMD_PRINT;
        }
        return CMD_UNKNOWN;
    }
    if (strcmp(upper, "INSERT") == 0) {
        return CMD_INSERT;
    }
    if (strcmp(upper, "DELETE") == 0) {
        return CMD_DELETE;
    }
    if (strcmp(upper, "UPDATE") == 0) {
        return CMD_UPDATE;
    }
    if (strcmp(upper, "READ") == 0) {
        return CMD_READ;
    }
    if (strcmp(upper, "PRINT") == 0) {
        return CMD_PRINT;
    }
    return CMD_UNKNOWN;
}

static int matchDirection(const char* tok) {
    char upper[MAX_TOKEN_UPPER];
    toUpperCopy(tok, upper, MAX_TOKEN_UPPER);
    if (strlen(upper) == 1) {
        if (upper[0] == 'L') {
            return DIR_LEFT;
        }
        if (upper[0] == 'R') {
            return DIR_RIGHT;
        }
        return -1;
    }
    if (strcmp(upper, "LEFT") == 0) {
        return DIR_LEFT;
    }
    if (strcmp(upper, "RIGHT") == 0) {
        return DIR_RIGHT;
    }
    return -1;
}

static void handleInsert(BinaryTree* tree, char** tokens, int tokenCount) {
    TreeNode* parent;
    int dir;
    int ok;

    if (tokenCount == 3) {
        if (strcmp(tokens[1], "/") != 0) {
            printf("경로 형식이 올바르지 않습니다.\n");
            return;
        }
        if (isValidDataToken(tokens[2]) == 0) {
            printf("데이터는 영문 대문자 한 글자여야 합니다.\n");
            return;
        }
        if (tree->root != NULL) {
            printf("이미 루트 노드가 존재합니다.\n");
            return;
        }
        ok = insert_root(tree, tokens[2][0]);
        if (ok == 0) {
            printf("루트 노드를 생성할 수 없습니다.\n");
        }
        return;
    }

    if (tokenCount == 4) {
        parent = findNode(tree, tokens[1], NULL, NULL);
        if (parent == NULL) {
            printf("부모 노드가 존재하지 않습니다.\n");
            return;
        }
        if (parent->left != NULL || parent->right != NULL) {
            printf("부모 노드가 단말 노드가 아닙니다.\n");
            return;
        }
        dir = matchDirection(tokens[2]);
        if (dir == -1) {
            printf("자식 위치는 L 또는 R이어야 합니다.\n");
            return;
        }
        if (isValidDataToken(tokens[3]) == 0) {
            printf("데이터는 영문 대문자 한 글자여야 합니다.\n");
            return;
        }
        ok = insert_child(tree, parent, dir, tokens[3][0]);
        if (ok == 0) {
            printf("동일한 부모 아래에 같은 데이터를 가진 노드가 이미 존재합니다.\n");
        }
        return;
    }

    printf("인자 개수가 올바르지 않습니다.\n");
}

static void handleDelete(BinaryTree* tree, char** tokens, int tokenCount) {
    TreeNode* node;
    TreeNode* parent;
    int dir;
    int ok;

    if (tokenCount != 2) {
        printf("인자 개수가 올바르지 않습니다.\n");
        return;
    }

    node = findNode(tree, tokens[1], &parent, &dir);
    if (node == NULL) {
        printf("노드가 존재하지 않습니다.\n");
        return;
    }
    if (node->left != NULL || node->right != NULL) {
        printf("단말 노드가 아닙니다.\n");
        return;
    }
    ok = delete_node(tree, node, parent, dir);
    if (ok == 0) {
        printf("노드를 삭제할 수 없습니다.\n");
    }
}

static void handleUpdate(BinaryTree* tree, char** tokens, int tokenCount) {
    TreeNode* node;
    TreeNode* parent;
    int dir;
    int ok;

    if (tokenCount != 3) {
        printf("인자 개수가 올바르지 않습니다.\n");
        return;
    }
    if (isValidDataToken(tokens[2]) == 0) {
        printf("데이터는 영문 대문자 한 글자여야 합니다.\n");
        return;
    }

    node = findNode(tree, tokens[1], &parent, &dir);
    if (node == NULL) {
        printf("노드가 존재하지 않습니다.\n");
        return;
    }

    ok = update_value(node, parent, tokens[2][0]);
    if (ok == 0) {
        printf("동일한 부모 아래에 같은 데이터를 가진 노드가 이미 존재합니다.\n");
    }
}

static void handleRead(BinaryTree* tree, char** tokens, int tokenCount) {
    TreeNode* node;

    if (tokenCount != 2) {
        printf("인자 개수가 올바르지 않습니다.\n");
        return;
    }

    node = findNode(tree, tokens[1], NULL, NULL);
    if (node == NULL) {
        printf("노드가 존재하지 않습니다.\n");
        return;
    }

    read_child(node);
}

static void handlePrint(BinaryTree* tree, char** tokens, int tokenCount) {
    if (tokenCount != 1) {
        printf("인자 개수가 올바르지 않습니다.\n");
        return;
    }
    if (tree->root == NULL) {
        printf("트리가 비어 있습니다.\n");
        return;
    }
    print_btree(tree->root, 0);
}

static void processCommand(BinaryTree* tree, char** tokens, int tokenCount) {
    CommandType cmd = matchCommand(tokens[0]);

    if (cmd == CMD_INSERT) {
        handleInsert(tree, tokens, tokenCount);
    }
    else if (cmd == CMD_DELETE) {
        handleDelete(tree, tokens, tokenCount);
    }
    else if (cmd == CMD_UPDATE) {
        handleUpdate(tree, tokens, tokenCount);
    }
    else if (cmd == CMD_READ) {
        handleRead(tree, tokens, tokenCount);
    }
    else if (cmd == CMD_PRINT) {
        handlePrint(tree, tokens, tokenCount);
    }
    else {
        printf("알 수 없는 명령어입니다.\n");
    }
}

int main(void) {
    BinaryTree* tree = create_btree(100000);
    char line[MAX_LINE];
    char* tokens[MAX_TOKENS];
    int tokenCount;
    char* tok;
    int len;

    while (fgets(line, MAX_LINE, stdin) != NULL) {
        len = (int)strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }

        tokenCount = 0;
        tok = strtok(line, " \t");
        while (tok != NULL && tokenCount < MAX_TOKENS) {
            tokens[tokenCount] = tok;
            tokenCount++;
            tok = strtok(NULL, " \t");
        }

        if (tokenCount == 0) {
            continue;
        }

        processCommand(tree, tokens, tokenCount);
    }

    destroy_btree(tree);
    return 0;
}