#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUE 1000000
#define SIZE_COUNT 6
#define BONUS_SIZE 10000
#define REPEAT 300
#define STACK_SIZE 40000

#define PIVOT_FIRST 0
#define PIVOT_RANDOM 1
#define PIVOT_MEDIAN 2

long comparisons;
int stackArr[STACK_SIZE];

void swapInt(int* a, int* b) {
    int temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

int choosePivotIndex(int arr[], int low, int high, int method) {
    int mid;
    int a;
    int b;
    int c;

    if (method == PIVOT_FIRST) {
        return low;
    }

    if (method == PIVOT_RANDOM) {
        return low + rand() % (high - low + 1);
    }

    mid = low + (high - low) / 2;
    a = arr[low];
    b = arr[mid];
    c = arr[high];

    comparisons++;
    if (a < b) {
        comparisons++;
        if (b < c) {
            return mid;
        }
        else {
            comparisons++;
            if (a < c) {
                return high;
            }
            else {
                return low;
            }
        }
    }
    else {
        comparisons++;
        if (a < c) {
            return low;
        }
        else {
            comparisons++;
            if (b < c) {
                return high;
            }
            else {
                return mid;
            }
        }
    }
}

int partition(int arr[], int low, int high, int method) {
    int pivotIndex;
    int pivot;
    int i;
    int j;

    pivotIndex = choosePivotIndex(arr, low, high, method);
    swapInt(&arr[pivotIndex], &arr[high]);
    pivot = arr[high];

    i = low - 1;
    for (j = low; j < high; j++) {
        comparisons++;
        if (arr[j] <= pivot) {
            i++;
            swapInt(&arr[i], &arr[j]);
        }
    }

    swapInt(&arr[i + 1], &arr[high]);

    return i + 1;
}

void quickSortRecursive(int arr[], int low, int high, int method) {
    int p;

    if (low >= high) {
        return;
    }

    p = partition(arr, low, high, method);
    quickSortRecursive(arr, low, p - 1, method);
    quickSortRecursive(arr, p + 1, high, method);
}

void quickSortIterative(int arr[], int n, int method) {
    int top;
    int low;
    int high;
    int p;

    if (n < 2) {
        return;
    }

    top = 0;
    stackArr[top] = 0;
    top++;
    stackArr[top] = n - 1;
    top++;

    while (top > 0) {
        top--;
        high = stackArr[top];
        top--;
        low = stackArr[top];

        p = partition(arr, low, high, method);

        if (low < p - 1) {
            stackArr[top] = low;
            top++;
            stackArr[top] = p - 1;
            top++;
        }

        if (p + 1 < high) {
            stackArr[top] = p + 1;
            top++;
            stackArr[top] = high;
            top++;
        }
    }
}

int isSorted(int arr[], int n) {
    int i;

    for (i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return 0;
        }
    }

    return 1;
}

void copyArray(int dest[], int src[], int n) {
    int i;

    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void printComma(long n) {
    if (n >= 1000) {
        printComma(n / 1000);
        printf(",%03ld", n % 1000);
    }
    else {
        printf("%ld", n);
    }
}

long runSort(int original[], int work[], int n, int method) {
    copyArray(work, original, n);
    comparisons = 0;
    quickSortRecursive(work, 0, n - 1, method);

    if (!isSorted(work, n)) {
        printf("정렬 결과가 올바르지 않습니다.\n");
    }

    return comparisons;
}

double measureTime(int original[], int n, int method, int iterative) {
    int* copies;
    int r;
    clock_t start;
    clock_t end;

    copies = (int*)malloc(sizeof(int) * n * REPEAT);

    if (copies == NULL) {
        printf("메모리 할당에 실패했습니다.\n");
        exit(1);
    }

    for (r = 0; r < REPEAT; r++) {
        copyArray(copies + r * n, original, n);
    }

    start = clock();
    for (r = 0; r < REPEAT; r++) {
        if (iterative) {
            quickSortIterative(copies + r * n, n, method);
        }
        else {
            quickSortRecursive(copies + r * n, 0, n - 1, method);
        }
    }
    end = clock();

    if (!isSorted(copies + (REPEAT - 1) * n, n)) {
        printf("정렬 결과가 올바르지 않습니다.\n");
    }

    free(copies);

    return (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / REPEAT;
}

int main(void) {
    int sizes[SIZE_COUNT] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    int* original;
    int* work;
    int s;
    int i;
    int n;
    int bonusSize;
    long firstCount;
    long randomCount;
    long medianCount;
    double recursiveTime;
    double iterativeTime;

    srand((unsigned int)time(NULL));

    for (s = 0; s < SIZE_COUNT; s++) {
        n = sizes[s];
        original = (int*)malloc(sizeof(int) * n);
        work = (int*)malloc(sizeof(int) * n);

        if (original == NULL || work == NULL) {
            printf("메모리 할당에 실패했습니다.\n");
            exit(1);
        }

        for (i = 0; i < n; i++) {
            original[i] = (int)(((long long)rand() * 32768 + rand()) % MAX_VALUE);
        }

        firstCount = runSort(original, work, n, PIVOT_FIRST);
        randomCount = runSort(original, work, n, PIVOT_RANDOM);
        medianCount = runSort(original, work, n, PIVOT_MEDIAN);

        printf("Data Size : %d\n\n", n);
        printf("Quick Sort (First)    : ");
        printComma(firstCount);
        printf(" comparisons\n");
        printf("Quick Sort (Random)   : ");
        printComma(randomCount);
        printf(" comparisons\n");
        printf("Quick Sort (Median-3) : ");
        printComma(medianCount);
        printf(" comparisons\n\n");

        free(original);
        free(work);
    }

    bonusSize = BONUS_SIZE;
    original = (int*)malloc(sizeof(int) * bonusSize);

    if (original == NULL) {
        printf("메모리 할당에 실패했습니다.\n");
        exit(1);
    }

    for (i = 0; i < bonusSize; i++) {
        original[i] = (int)(((long long)rand() * 32768 + rand()) % MAX_VALUE);
    }

    recursiveTime = measureTime(original, bonusSize, PIVOT_MEDIAN, 0);
    iterativeTime = measureTime(original, bonusSize, PIVOT_MEDIAN, 1);

    printf("Data Size : %d\n", bonusSize);
    printf("Pivot     : Median-of-Three\n\n");
    printf("Recursive Quick Sort : %.4f ms\n", recursiveTime);
    printf("Iterative Quick Sort : %.4f ms\n", iterativeTime);

    free(original);

    return 0;
}