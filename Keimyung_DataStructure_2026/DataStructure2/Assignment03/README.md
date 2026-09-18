# [자료구조2] Assignment 03 - 2026.09.19

## 과제

빈 이진트리를 생성한 후 사용자가 명령어를 입력하여 노드를 추가, 삭제, 수정하거나 특정 노드의 자식 정보를 확인하고 전체 트리를 출력할 수 있는 프로그램을 구현하였다.

주어진 이진트리 ADT를 기반으로 프로그램을 구성하였으며, 각 노드의 위치는 루트부터 해당 노드까지의 데이터를 `/`로 구분한 경로(path)로 표현하였다.

예:

```text
/A/B/C
```

서로 다른 위치에 있는 노드는 동일한 데이터를 가질 수 있도록 하되, 동일한 부모의 왼쪽 자식과 오른쪽 자식은 같은 데이터를 가질 수 없도록 구현하였다.

---

## 구현 내용
<img width="506" height="369" alt="image" src="https://github.com/user-attachments/assets/603ddcfa-b3be-4600-bc94-26af4a1388da" />

### 1. 이진트리 ADT 구현

이진트리 자체와 명령어 처리 부분을 분리하여 구현하였다.

이진트리는 루트 포인터와 최대 노드 수, 현재 노드 수를 저장하도록 구성하였다.

```c
typedef struct {
    TreeNode* root;
    int capacity;
    int count;
} BinaryTree;
```

각 노드는 영문 대문자 한 글자의 데이터와 왼쪽, 오른쪽 자식 포인터를 가진다.

```c
typedef struct TreeNode {
    char data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;
```

---

### 2. 빈 이진트리 생성 및 제거

프로그램이 시작되면 `create_btree()`를 이용하여 루트가 없는 빈 이진트리를 생성한다.

```c
static BinaryTree* create_btree(int size)
{
    BinaryTree* tree = (BinaryTree*)malloc(sizeof(BinaryTree));

    tree->root = NULL;
    tree->capacity = size;
    tree->count = 0;

    return tree;
}
```

프로그램 종료 시에는 `destroy_btree()`를 호출하여 트리에 존재하는 모든 노드를 제거하고 이진트리 자체의 메모리도 해제하도록 구현하였다.

```c
static void destroy_btree(BinaryTree* tree)
{
    if (tree == NULL)
        return;

    freeSubtree(tree->root);
    tree->root = NULL;
    free(tree);
}
```

---

### 3. 루트 노드 생성

빈 트리에서만 `Insert / A` 형식으로 루트 노드를 생성할 수 있도록 구현하였다.

```text
Insert / A
```

루트가 이미 존재하는 경우에는 새로운 루트를 생성하지 않고 오류 메시지를 출력한다.

```c
static int insert_root(BinaryTree* tree, char value)
{
    if (tree->root != NULL)
        return 0;

    tree->root = create_node(value);
    tree->count++;

    return 1;
}
```

---

### 4. 경로를 이용한 노드 탐색

각 노드의 위치를 `/A/B/C`와 같은 경로로 표현하였다.

`parsePath()` 함수를 이용하여 경로를 `/` 단위의 영문 대문자로 분리하고, `findNode()`에서 루트부터 왼쪽과 오른쪽 자식을 따라가며 해당 경로의 노드를 탐색하였다.

```c
static int parsePath(
    const char* path,
    char* letters,
    int maxLen)
```

예를 들어 다음 경로가 입력되면

```text
/A/B/C
```

다음과 같이 루트 A에서 시작하여 B, C 순서로 이동한다.

```text
A → B → C
```

경로에 해당하는 노드가 존재하지 않는 경우에는 오류를 출력하고 트리의 상태를 변경하지 않는다.

---

### 5. 자식 노드 삽입

`Insert` 명령을 이용하여 지정된 부모 노드의 왼쪽 또는 오른쪽에 새로운 자식 노드를 추가하도록 구현하였다.

```text
Insert /A L B
Insert /A R C
```

`L`은 왼쪽 자식, `R`은 오른쪽 자식을 의미한다.

조건에 따라 **새로운 자식은 단말 노드 아래에만 추가할 수 있도록** 구현하였다.

```c
if (parent->left != NULL || parent->right != NULL) {
    return 0;
}
```

또한 새로 입력되는 데이터는 영문 대문자 한 글자인지 검사하였다.

---

### 6. 데이터 중복 처리

서로 다른 위치의 노드는 동일한 데이터를 가질 수 있도록 구현하였다.

예를 들어 다음과 같은 트리는 허용된다.

```text
      A
     / \
    B   C
   /     \
  D       D
```

각 D 노드는 서로 다른 경로로 구분할 수 있다.

```text
/A/B/D
/A/C/D
```

반면 동일한 부모의 왼쪽 자식과 오른쪽 자식이 같은 데이터를 가지는 경우에는 삽입 또는 수정하지 않는다.

---

### 7. 단말 노드 삭제

`Delete` 명령을 이용하여 지정한 경로의 노드를 삭제하였다.

```text
Delete /A/B/X/D
```

삭제 대상은 반드시 단말 노드여야 하므로 왼쪽 또는 오른쪽 자식이 존재하는 비단말 노드를 삭제하려는 경우에는 오류를 출력한다.

```c
if (leaf->left != NULL || leaf->right != NULL) {
    return 0;
}
```

루트 노드가 유일한 노드인 경우에는 루트를 삭제하여 다시 빈 트리 상태로 만들 수 있도록 구현하였다.

---

### 8. 노드 데이터 수정

`Update` 명령을 이용하여 특정 경로의 노드 데이터를 변경하였다.

```text
Update /A/B/C X
```

위 명령을 수행하면 C 노드의 데이터가 X로 변경되고, 해당 노드의 새로운 경로는 다음과 같이 변경된다.

```text
/A/B/X
```

수정 시에도 동일한 부모 아래의 다른 자식과 같은 데이터가 되는 경우에는 수정을 수행하지 않는다.

```c
static int update_value(
    TreeNode* node,
    TreeNode* parent,
    char value)
```

---

### 9. 자식 정보 확인

`Read` 명령을 이용하여 특정 노드의 왼쪽 및 오른쪽 자식 정보를 확인할 수 있도록 구현하였다.

```text
Read /A/B/C
```

예를 들어 C의 오른쪽 자식이 D인 경우:

```text
D(R)
```

와 같이 출력한다.

왼쪽과 오른쪽 자식이 모두 존재하는 경우에는 다음과 같이 출력한다.

```text
Y(L), X(R)
```

단말 노드인 경우에는 자식이 없다는 메시지를 출력한다.

---

### 10. 전체 이진트리 출력

`Print` 명령을 이용하여 현재 이진트리 전체를 이전 과제와 동일한 방식으로 왼쪽으로 눕힌 형태로 출력하였다.

```text
A
+---B
    +---C
        +---D
```

재귀 함수에 현재 노드의 깊이를 전달하여 깊이에 따라 들여쓰기를 추가하였다.

```c
static void print_btree(TreeNode* node, int depth)
{
    if (node == NULL)
        return;

    if (depth == 0) {
        printf("%c\n", node->data);
    } else {
        for (int i = 0; i < depth - 1; i++)
            printf("    ");

        printf("+---%c\n", node->data);
    }

    print_btree(node->left, depth + 1);
    print_btree(node->right, depth + 1);
}
```

빈 트리에서 `Print` 명령을 수행한 경우에는 트리가 비어 있다는 메시지를 출력하도록 구현하였다.

---

### 11. 명령어 처리

사용자가 입력한 명령어를 공백 기준으로 분리한 후 명령의 종류에 따라 각각의 처리 함수를 호출하였다.

```text
Insert
Delete
Update
Read
Print
```

또한 과제에서 요구한 것처럼 명령어의 첫 글자도 사용할 수 있도록 구현하였다.

```text
I = Insert
D = Delete
U = Update
R = Read
P = Print
```

대소문자 입력을 처리하기 위해 입력된 명령어와 방향을 대문자로 변환하여 비교하였다.

---

### 12. 잘못된 명령 처리

프로그램 실행 중 오류가 발생하더라도 프로그램이 종료되지 않고 다음 명령을 계속 입력받도록 구현하였다.

다음과 같은 경우 오류 메시지를 출력한다.

* 존재하지 않는 노드 경로
* 잘못된 명령어
* 잘못된 인자 개수
* 잘못된 경로 형식
* 영문 대문자 한 글자가 아닌 데이터
* 빈 트리가 아닌 상태에서 루트 생성
* 단말 노드가 아닌 노드에 자식 삽입
* 단말 노드가 아닌 노드 삭제
* 동일한 부모 아래의 자식 데이터 중복
* 잘못된 자식 위치(L/R)

오류가 발생한 경우 해당 명령에 의한 트리의 변경은 수행하지 않고 다음 명령을 처리하도록 구성하였다.

---

## 프로그램 동작 예

다음 명령을 순서대로 입력하여 기본적인 이진트리 조작을 확인하였다.

```text
Insert / A
Insert /A L B
Insert /A/B L C
Insert /A/B/C R D
Print
Read /A/B/C
Update /A/B/C X
Delete /A/B/X/D
Print
```

초기 `Print` 결과:

```text
A
+---B
    +---C
        +---D
```

`Read /A/B/C` 실행 결과:

```text
D(R)
```

`Update /A/B/C X`를 실행하여 C 노드를 X로 변경한 후 `/A/B/X/D` 경로를 이용하여 D를 삭제하였다.

최종 `Print` 결과:

```text
A
+---B
    +---X
```

이를 통해 노드 삽입, 자식 정보 확인, 데이터 수정, 단말 노드 삭제 및 전체 트리 출력 기능이 정상적으로 동작하는 것을 확인하였다.

---

## 프로그램 명령어

```text
Insert / A
Insert /A L B
Delete /A/B
Update /A/B X
Read /A/B
Print
```

각 명령어는 전체 이름 또는 첫 글자로 입력할 수 있다.

```text
I /A L B
D /A/B
U /A/B X
R /A/B
P
```

프로그램 종료 시 생성된 트리의 모든 노드를 `destroy_btree()`를 통해 해제하도록 구현하였다.
