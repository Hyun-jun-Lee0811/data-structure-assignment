# [자료구조2] Assignment 04 - 2026.09.22

## 과제

괄호 표기법으로 입력된 이진트리를 포인터 연결 방식으로 구성한 후, 이진트리의 전위 순회, 중위 순회, 후위 순회를 구현하는 프로그램을 작성하였다.

세 가지 순회 알고리즘을 **재귀 함수로 구현하지 않고**, 반복문과 스택을 이용하여 구현하였다.

입력된 이진트리는 다음과 같은 괄호 표기법으로 표현한다.
입력된 트리는 내부적으로 포인터 연결 방식으로 구성하며, 이후 각 순회 함수를 통해 순회 결과를 출력한다.

---

## 구현 내용

### 1. 괄호 표기법을 이용한 이진트리 입력

사용자로부터 괄호 표기법으로 이진트리를 입력받도록 구현하였다.

```text
A(B(D(H,I),E(J,K)),C(F,G(L,M)))
```

각 노드의 데이터와 괄호, 쉼표를 분석하여 왼쪽 자식과 오른쪽 자식을 연결한다.

```c
static TreeNode* parseTree(void)
```

`parseTree()` 함수에서 입력 문자열을 순서대로 분석하고 동적으로 노드를 생성하여 이진트리를 구성한다.

입력 과정에서 잘못된 괄호나 쉼표가 사용된 경우 `errorFlag`를 설정하여 잘못된 입력을 처리한다.

---

### 2. 포인터 연결 방식의 이진트리 구성

각 노드는 데이터와 왼쪽, 오른쪽 자식을 가리키는 포인터를 저장하도록 구성하였다.

```c
typedef struct TreeNode {
    char name[MAX_NAME];
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;
```

각 노드는 `malloc()`을 이용하여 동적으로 생성하며, `left`와 `right` 포인터를 이용하여 이진트리의 구조를 표현한다.

따라서 입력된 이진트리는 배열이 아닌 **포인터 연결 방식**으로 저장된다.

---

### 3. 스택 구현

반복적인 트리 순회를 수행하기 위해 노드 포인터를 저장하는 스택을 배열로 구현하였다.

```c
#define MAX_STACK 1024

static TreeNode* stackArray[MAX_STACK];
static int stackTop = 0;
```

스택의 삽입과 삭제를 위해 다음 함수를 사용하였다.

```c
static void pushNode(TreeNode* node)
static TreeNode* popNode(void)
static TreeNode* peekNode(void)
static int stackEmpty(void)
```

스택은 후입선출(LIFO) 방식으로 동작하며, 각 순회 알고리즘에서 재귀 호출 대신 노드의 방문 순서를 관리하는 데 사용하였다.

---

### 4. 전위 순회 (Preorder)

전위 순회는 다음 순서로 노드를 방문한다.

```text
Root → Left → Right
```

전위 순회를 담당하는 함수는 다음과 같이 별도로 구현하였다.

```c
static void preorderIterative(TreeNode* root)
```

현재 노드를 먼저 출력한 후 오른쪽 자식과 왼쪽 자식을 스택에 넣는다.

```c
if (current->right != NULL) {
    pushNode(current->right);
}

if (current->left != NULL) {
    pushNode(current->left);
}
```

스택은 LIFO 구조이므로 오른쪽 자식을 먼저 넣고 왼쪽 자식을 나중에 넣으면 왼쪽 자식이 먼저 처리된다.

따라서 다음과 같은 전위 순회 순서를 유지할 수 있다.

```text
Root → Left → Right
```

---

### 5. 중위 순회 (Inorder)

중위 순회는 다음 순서로 노드를 방문한다.

```text
Left → Root → Right
```

중위 순회를 담당하는 함수는 다음과 같이 구현하였다.

```c
static void inorderIterative(TreeNode* root)
```

현재 노드에서 왼쪽 자식을 계속 스택에 저장하고, 더 이상 왼쪽 자식이 없으면 스택에서 노드를 꺼내 출력한 후 오른쪽 자식으로 이동한다.

```c
while (current != NULL) {
    pushNode(current);
    current = current->left;
}

current = popNode();
printf("%s ", current->name);
current = current->right;
```

이 과정을 반복하여 재귀 호출 없이 중위 순회를 수행한다.

---

### 6. 후위 순회 (Postorder)

후위 순회는 다음 순서로 노드를 방문한다.

```text
Left → Right → Root
```

후위 순회를 담당하는 함수는 다음과 같이 구현하였다.

```c
static void postorderIterative(TreeNode* root)
```

후위 순회에서는 현재 노드의 오른쪽 서브트리를 처리했는지 확인하기 위해 `lastVisited` 변수를 사용하였다.

```c
TreeNode* lastVisited;
```

스택의 최상위 노드를 확인하여 오른쪽 자식이 아직 방문되지 않은 경우 오른쪽 서브트리로 이동하고, 오른쪽 자식까지 처리된 경우 현재 노드를 출력한다.

```c
peeked = peekNode();

if (peeked->right != NULL && peeked->right != lastVisited) {
    current = peeked->right;
}
else {
    printf("%s ", peeked->name);
    lastVisited = popNode();
    current = NULL;
}
```

이를 통해 재귀 함수 없이 후위 순회를 구현하였다.

---

### 7. 입력된 이진트리 구조 출력

순회 결과를 확인하기 전에 입력된 이진트리의 구조를 계층적으로 출력하도록 구현하였다.

```c
static void printTree(TreeNode* node, int depth)
```

노드의 깊이에 따라 들여쓰기를 추가하여 부모와 자식 관계를 확인할 수 있도록 하였다.

예:

```text
A
    B
        D
            H
            I
        E
            J
            K
    C
        F
        G
            L
            M
```

이를 통해 입력된 트리의 구조와 순회 결과를 비교할 수 있다.

---

### 8. 잘못된 입력 처리

괄호 표기법의 형식이 잘못된 경우 `errorFlag`를 이용하여 오류를 처리하였다.

```c
static int errorFlag = 0;
```

예를 들어 괄호가 올바르게 닫히지 않았거나 필요한 쉼표가 없는 경우 오류 상태를 설정한다.

입력이 올바르지 않은 경우 다음과 같은 메시지를 출력한다.

```text
입력된 괄호 표현이 올바르지 않습니다.
```

잘못된 입력으로 인해 일부 노드가 생성된 경우에도 `freeTree()`를 호출하여 할당된 메모리를 정리하도록 구현하였다.

---

### 9. 동적 메모리 관리

각 이진트리 노드는 `malloc()`을 이용하여 동적으로 생성하였다.

```c
node = (TreeNode*)malloc(sizeof(TreeNode));
```

프로그램 종료 시 생성된 모든 노드를 해제하기 위해 다음 함수를 사용하였다.

```c
static void freeTree(TreeNode* node)
```

왼쪽과 오른쪽 서브트리의 노드를 해제한 후 현재 노드의 메모리를 해제하여 동적 메모리 누수를 방지하였다.

```c
freeTree(node->left);
freeTree(node->right);
free(node);
```

---

## 테스트

과제에서 요구하는 10개 이상의 노드를 포함하도록 총 **13개의 노드**로 구성된 이진트리를 테스트하였다.

### 입력

```text
A(B(D(H,I),E(J,K)),C(F,G(L,M)))
```

### 입력된 이진트리 구조

```text
A
    B
        D
            H
            I
        E
            J
            K
    C
        F
        G
            L
            M
```

왼쪽과 오른쪽 서브트리가 모두 존재하며, 각 노드가 서로 다른 위치에 배치되어 있어 세 가지 순회 결과를 비교하기에 적합한 형태이다.

---

### Preorder 결과

```text
A B D H I E J K C F G L M
```

방문 순서:

```text
Root → Left → Right
```

---

### Inorder 결과

```text
H D I B J E K A F C L G M
```

방문 순서:

```text
Left → Root → Right
```

---

### Postorder 결과

```text
H I D J K E B F L M G C A
```

방문 순서:

```text
Left → Right → Root
```

---

## 함수 구성

| 함수                     | 역할                   |
| ---------------------- | -------------------- |
| `parseTree()`          | 괄호 표기법을 분석하여 이진트리 생성 |
| `preorderIterative()`  | 반복문과 스택을 이용한 전위 순회   |
| `inorderIterative()`   | 반복문과 스택을 이용한 중위 순회   |
| `postorderIterative()` | 반복문과 스택을 이용한 후위 순회   |
| `printTree()`          | 입력된 이진트리 구조 출력       |
| `freeTree()`           | 생성된 이진트리의 메모리 해제     |
| `pushNode()`           | 스택에 노드 삽입            |
| `popNode()`            | 스택에서 노드 제거           |
| `peekNode()`           | 스택의 최상위 노드 확인        |
| `stackEmpty()`         | 스택이 비어 있는지 확인        |
| `readName()`           | 입력 문자열에서 노드 이름 읽기    |
| `readLine()`           | 사용자 입력 한 줄 처리        |

---

## 프로그램 실행 결과

<img width="482" height="469" alt="image" src="https://github.com/user-attachments/assets/58ec274a-41de-415b-841a-1c781c950d9a" />
