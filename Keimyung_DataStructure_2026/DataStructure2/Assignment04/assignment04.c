#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 32
#define MAX_INPUT 2048
#define MAX_STACK 1024

typedef struct TreeNode {
    char name[MAX_NAME];
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

static char input[MAX_INPUT];
static int pos = 0;
static int errorFlag = 0;

static TreeNode* stackArray[MAX_STACK];
static int stackTop = 0;

static void pushNode(TreeNode* node) {
    if (stackTop >= MAX_STACK) {
        printf("스택 용량을 초과했습니다.\n");
        exit(1);
    }
    stackArray[stackTop] = node;
    stackTop++;
}

static TreeNode* popNode(void) {
    stackTop--;
    return stackArray[stackTop];
}

static TreeNode* peekNode(void) {
    return stackArray[stackTop - 1];
}

static int stackEmpty(void) {
    if (stackTop == 0) {
        return 1;
    }
    return 0;
}

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

static TreeNode* parseTree(void) {
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
        node->left = parseTree();
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
        node->right = parseTree();
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

static void freeTree(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

static void printTree(TreeNode* node, int depth) {
    int i;
    if (node == NULL) {
        return;
    }
    for (i = 0; i < depth; i++) {
        printf("    ");
    }
    printf("%s\n", node->name);
    printTree(node->left, depth + 1);
    printTree(node->right, depth + 1);
}

static void preorderIterative(TreeNode* root) {
    TreeNode* current;
    stackTop = 0;
    if (root == NULL) {
        return;
    }
    pushNode(root);
    while (stackEmpty() == 0) {
        current = popNode();
        printf("%s ", current->name);
        if (current->right != NULL) {
            pushNode(current->right);
        }
        if (current->left != NULL) {
            pushNode(current->left);
        }
    }
}

static void inorderIterative(TreeNode* root) {
    TreeNode* current;
    stackTop = 0;
    current = root;
    while (current != NULL || stackEmpty() == 0) {
        while (current != NULL) {
            pushNode(current);
            current = current->left;
        }
        current = popNode();
        printf("%s ", current->name);
        current = current->right;
    }
}

static void postorderIterative(TreeNode* root) {
    TreeNode* current;
    TreeNode* lastVisited;
    TreeNode* peeked;
    stackTop = 0;
    current = root;
    lastVisited = NULL;
    while (current != NULL || stackEmpty() == 0) {
        while (current != NULL) {
            pushNode(current);
            current = current->left;
        }
        peeked = peekNode();
        if (peeked->right != NULL && peeked->right != lastVisited) {
            current = peeked->right;
        }
        else {
            printf("%s ", peeked->name);
            lastVisited = popNode();
            current = NULL;
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
    TreeNode* root;

    printf("괄호 표기법 입력 : ");
    readLine(line, MAX_INPUT);
    strncpy(input, line, MAX_INPUT - 1);
    input[MAX_INPUT - 1] = '\0';
    pos = 0;
    errorFlag = 0;

    root = parseTree();
    if (errorFlag == 0) {
        skipSpace();
        if (input[pos] != '\0') {
            errorFlag = 1;
        }
    }

    if (errorFlag == 1) {
        printf("입력된 괄호 표현이 올바르지 않습니다.\n");
        freeTree(root);
        return 1;
    }

    printf("\n입력된 이진트리 구조\n");
    printTree(root, 0);

    printf("\nPreorder  : ");
    preorderIterative(root);
    printf("\n");

    printf("Inorder   : ");
    inorderIterative(root);
    printf("\n");

    printf("Postorder : ");
    postorderIterative(root);
    printf("\n");

    freeTree(root);
    return 0;
}