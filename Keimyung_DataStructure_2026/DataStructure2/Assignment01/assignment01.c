#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define TARGET 'C'

char parent[26];
char child[26][26];
int childNum[26];
int used[26];

int totalNode = 0;
int height = 0;
char root;

char stack1[MAX];
int stack2[MAX];
int top = 0;

void error(char* msg)
{
    printf("오류: %s\n", msg);
    exit(1);
}

void printTree(char node, char* prefix, int isLast, int isRoot);

int main(void)
{
    char str[1000];
    int i, len;
    int state;
    char last;

    scanf("%s", str);
    len = strlen(str);
    state = 0;
    last = 0;

    for (i = 0; i < len; i++)
    {
        char c = str[i];

        if (state == 0)
        {
            if (c < 'A' || c > 'Z') {
                error("잘못된 입력입니다.");
            }
            if (used[c - 'A'] == 1) {
                error("잘못된 입력입니다.");
            }

            used[c - 'A'] = 1;
            totalNode++;
            if (top > height) {
                height = top;
            }

            if (top == 0)
            {
                root = c;
                parent[c - 'A'] = 0;
            }
            else
            {
                char p = stack1[top - 1];
                parent[c - 'A'] = p;
                child[p - 'A'][childNum[p - 'A']] = c;
                childNum[p - 'A']++;
                stack2[top - 1]++;
            }

            last = c;
            state = 1;
        }
        else if (state == 1)
        {
            if (c == '(')
            {
                stack1[top] = last;
                stack2[top] = 0;
                top++;
                state = 0;
            }
            else if (c == ',')
            {
                if (top == 0) {
                    error("잘못된 입력입니다.");
                }
                state = 0;
            }
            else if (c == ')')
            {
                if (top == 0) {
                    error("잘못된 입력입니다.");
                }
                top--;
                last = stack1[top];
                state = 2;
            }
            else {
                error("잘못된 입력입니다.");
            }
        }
        else
        {
            if (c == ',')
            {
                if (top == 0) {
                    error("잘못된 입력입니다.");
                }
                state = 0;
            }
            else if (c == ')')
            {
                if (top == 0) {
                    error("잘못된 입력입니다.");
                }
                top--;
                last = stack1[top];
                state = 2;
            }
            else {
                error("잘못된 입력입니다.");
            }
        }
    }

    if (top != 0) {
        error("잘못된 입력입니다.");
    }
    if (state == 0) {
        error("잘못된 입력입니다.");
    }
    if (totalNode == 0) {
        error("잘못된 입력입니다.");
    }

    int leaf = 0, notLeaf = 0, degree = 0;
    for (i = 0; i < 26; i++)
    {
        if (used[i] == 0) {
            continue;
		}
        if (childNum[i] == 0) {
            leaf++;
        } else {
            notLeaf++;
        }
        if (childNum[i] > degree) {
            degree = childNum[i];
        }
    }

    printf("전체 노드의 수 : %d\n", totalNode);
    printf("단말 노드의 수 : %d\n", leaf);
    printf("비단말 노드의 수 : %d\n", notLeaf);
    printf("트리의 높이 : %d\n", height);
    printf("트리의 차수 : %d\n", degree);

    if (used[TARGET - 'A'] == 0)
    {
        printf("%c 노드 없음\n", TARGET);
    }
    else
    {
        if (parent[TARGET - 'A'] == 0) {
            printf("%c의 부모 : 없음\n", TARGET);
        }
        else {
        printf("%c의 부모 : %c\n", TARGET, parent[TARGET - 'A']);
       }
        printf("%c의 자식 : ", TARGET);
        if (childNum[TARGET - 'A'] == 0) {
            printf("없음\n");
        }
        else
        {
            for (i = 0; i < childNum[TARGET - 'A']; i++)
                printf("%c ", child[TARGET - 'A'][i]);
            printf("\n");
        }
    }

    printTree(root, "", 1, 1);

    return 0;
}

void printTree(char node, char* prefix, int isLast, int isRoot)
{
    char newPrefix[1000];
    int i, cnt;

    if (isRoot) {
        printf("%c\n", node);
    }
    else {
        printf("%s+---%c\n", prefix, node);
    }
    if (isRoot) {
        strcpy(newPrefix, "");
    }
    else
    {
        strcpy(newPrefix, prefix);
        strcat(newPrefix, "    ");
    }

    cnt = childNum[node - 'A'];
    for (i = 0; i < cnt; i++)
    {
        char c = child[node - 'A'][i];
        int last = (i == cnt - 1);
        printTree(c, newPrefix, last, 0);
    }
}