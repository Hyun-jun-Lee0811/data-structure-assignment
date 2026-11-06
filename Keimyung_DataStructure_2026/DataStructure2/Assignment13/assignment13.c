#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAX_VALUE 1000000
#define SIZE_COUNT 6
#define ALGO_COUNT 4
#define TOTAL_ELEMENTS 4000000

#define ALGO_MERGE 0
#define ALGO_FIRST 1
#define ALGO_RANDOM 2
#define ALGO_MEDIAN 3

#define PIVOT_FIRST 0
#define PIVOT_RANDOM 1
#define PIVOT_MEDIAN 2

long comparisons;
int failCount;

int trackStack;
int curDepth;
uintptr_t stackMin;
uintptr_t stackMax;
uintptr_t firstFrameAddr;
uintptr_t frameBytes;

void recordAddress(void* p) {
    uintptr_t a;

    a = (uintptr_t)p;

    if (a < stackMin) {
        stackMin = a;
    }

    if (a > stackMax) {
        stackMax = a;
    }
}

void enterFrame(void* p) {
    uintptr_t a;

    a = (uintptr_t)p;
    recordAddress(p);

    if (curDepth == 0) {
        firstFrameAddr = a;
    }
    else if (curDepth == 1 && frameBytes == 0) {
        if (a > firstFrameAddr) {
            frameBytes = a - firstFrameAddr;
        }
        else {
            frameBytes = firstFrameAddr - a;
        }
    }

    curDepth++;
}

void swapInt(int* a, int* b) {
    int temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void merge(int arr[], int temp[], int low, int mid, int high) {
    int i;
    int j;
    int k;

    if (trackStack) {
        recordAddress(&i);
    }

    i = low;
    j = mid + 1;
    k = low;

    while (i <= mid && j <= high) {
        comparisons++;
        if (arr[i] <= arr[j]) {
            temp[k] = arr[i];
            i++;
        }
        else {
            temp[k] = arr[j];
            j++;
        }
        k++;
    }

    while (i <= mid) {
        temp[k] = arr[i];
        i++;
        k++;
    }

    while (j <= high) {
        temp[k] = arr[j];
        j++;
        k++;
    }

    for (k = low; k <= high; k++) {
        arr[k] = temp[k];
    }
}

void mergeSortRecursive(int arr[], int temp[], int low, int high) {
    int mid;

    if (trackStack) {
        enterFrame(&mid);
    }

    if (low < high) {
        mid = low + (high - low) / 2;
        mergeSortRecursive(arr, temp, low, mid);
        mergeSortRecursive(arr, temp, mid + 1, high);
        merge(arr, temp, low, mid, high);
    }

    if (trackStack) {
        curDepth--;
    }
}

int choosePivotIndex(int arr[], int low, int high, int method) {
    int mid;
    int a;
    int b;
    int c;

    if (trackStack) {
        recordAddress(&mid);
    }

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

    if (trackStack) {
        recordAddress(&i);
    }

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

    if (trackStack) {
        enterFrame(&p);
    }

    if (low < high) {
        p = partition(arr, low, high, method);
        quickSortRecursive(arr, low, p - 1, method);
        quickSortRecursive(arr, p + 1, high, method);
    }

    if (trackStack) {
        curDepth--;
    }
}

void sortByAlgorithm(int arr[], int n, int algo) {
    int* temp;

    if (algo == ALGO_MERGE) {
        temp = (int*)malloc(sizeof(int) * n);

        if (temp == NULL) {
            printf("메모리 할당에 실패했습니다.\n");
            exit(1);
        }

        mergeSortRecursive(arr, temp, 0, n - 1);
        free(temp);
    }
    else if (algo == ALGO_FIRST) {
        quickSortRecursive(arr, 0, n - 1, PIVOT_FIRST);
    }
    else if (algo == ALGO_RANDOM) {
        quickSortRecursive(arr, 0, n - 1, PIVOT_RANDOM);
    }
    else {
        quickSortRecursive(arr, 0, n - 1, PIVOT_MEDIAN);
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

void measureCountAndMemory(int original[], int work[], int n, int algo, long* comps, long* mem) {
    copyArray(work, original, n);

    comparisons = 0;
    curDepth = 0;
    stackMin = UINTPTR_MAX;
    stackMax = 0;
    firstFrameAddr = 0;
    frameBytes = 0;

    trackStack = 1;
    sortByAlgorithm(work, n, algo);
    trackStack = 0;

    if (!isSorted(work, n)) {
        failCount++;
    }

    *comps = comparisons;
    *mem = (long)(stackMax - stackMin + frameBytes);

    if (algo == ALGO_MERGE) {
        *mem = *mem + (long)(sizeof(int) * n);
    }
}

double measureTime(int original[], int n, int algo) {
    int repeat;
    int r;
    int* copies;
    clock_t start;
    clock_t end;

    repeat = TOTAL_ELEMENTS / n;
    copies = (int*)malloc(sizeof(int) * (size_t)n * (size_t)repeat);

    if (copies == NULL) {
        printf("메모리 할당에 실패했습니다.\n");
        exit(1);
    }

    for (r = 0; r < repeat; r++) {
        copyArray(copies + (size_t)r * n, original, n);
    }

    trackStack = 0;

    start = clock();
    for (r = 0; r < repeat; r++) {
        sortByAlgorithm(copies + (size_t)r * n, n, algo);
    }
    end = clock();

    for (r = 0; r < repeat; r++) {
        if (!isSorted(copies + (size_t)r * n, n)) {
            failCount++;
        }
    }

    free(copies);

    return (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / repeat;
}

void formatComma(long n, char* buf) {
    char tmp[32];
    int len;
    int i;
    int j;

    sprintf(tmp, "%ld", n);
    len = (int)strlen(tmp);

    j = 0;
    for (i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) {
            buf[j] = ',';
            j++;
        }
        buf[j] = tmp[i];
        j++;
    }
    buf[j] = '\0';
}

int main(void) {
    int sizes[SIZE_COUNT] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const char* names[ALGO_COUNT] = { "Merge Sort", "Quick (First)", "Quick (Random)", "Quick (Median3)" };
    int* original;
    int* work;
    int s;
    int a;
    int i;
    int n;
    long comps[ALGO_COUNT];
    long mem[ALGO_COUNT];
    double times[ALGO_COUNT];
    char compStr[32];
    char memStr[32];

    srand((unsigned int)time(NULL));
    failCount = 0;

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

        for (a = 0; a < ALGO_COUNT; a++) {
            measureCountAndMemory(original, work, n, a, &comps[a], &mem[a]);
            times[a] = measureTime(original, n, a);
        }

        printf("Data Size : %d\n\n", n);
        printf("%-15s %-14s %-11s %s\n", "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("--------------------------------------------------------\n");

        for (a = 0; a < ALGO_COUNT; a++) {
            formatComma(comps[a], compStr);
            formatComma(mem[a], memStr);
            printf("%-15s %-14s %-11.4f %s\n", names[a], compStr, times[a], memStr);
        }
        printf("\n");

        free(original);
        free(work);
    }

    if (failCount == 0) {
        printf("모든 정렬 결과가 오름차순으로 정렬\n");
    }
    else {
        printf("정렬 결과가 올바르지 않은 경우가 %d회 발견\n", failCount);
    }

    return 0;
}