#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATA_COUNT 10000
#define TRIALS 100
#define MAX_KEY 1000000
#define STABLE_COUNT 1000
#define STABLE_KEY_RANGE 50
#define SORT_COUNT 4

typedef struct {
    int key1;
    int key2;
    int order;
} Data;

typedef void (*SortFunc)(Data*, int);

long long comparisons = 0;
long long movements = 0;

int randomKey(void)
{
    return (int)((((long long)rand() * (RAND_MAX + 1LL)) + rand()) % (MAX_KEY + 1));
}

void generateRandomData(Data* a, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        a[i].key1 = randomKey();
        a[i].key2 = randomKey();
        a[i].order = i;
    }
}

void swapData(Data* x, Data* y)
{
    Data temp;
    temp = *x;
    movements++;
    *x = *y;
    movements++;
    *y = temp;
    movements++;
}

void selectionSort(Data* a, int n)
{
    int i, j, min;
    for (i = 0; i < n - 1; i++) {
        min = i;
        for (j = i + 1; j < n; j++) {
            comparisons++;
            if (a[j].key1 < a[min].key1) {
                min = j;
            }
        }
        if (min != i) {
            swapData(&a[i], &a[min]);
        }
    }
}

void bubbleSort(Data* a, int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            comparisons++;
            if (a[j].key1 > a[j + 1].key1) {
                swapData(&a[j], &a[j + 1]);
            }
        }
    }
}

void bubbleSortEarly(Data* a, int n)
{
    int i, j, swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - 1 - i; j++) {
            comparisons++;
            if (a[j].key1 > a[j + 1].key1) {
                swapData(&a[j], &a[j + 1]);
                swapped = 1;
            }
        }
        if (swapped == 0) {
            break;
        }
    }
}

void insertionSortGreater(Data* a, int n)
{
    int i, j;
    Data current;
    for (i = 1; i < n; i++) {
        current = a[i];
        movements++;
        j = i - 1;
        while (j >= 0) {
            comparisons++;
            if (a[j].key1 > current.key1) {
                a[j + 1] = a[j];
                movements++;
                j--;
            }
            else {
                break;
            }
        }
        a[j + 1] = current;
        movements++;
    }
}

void insertionSortGreaterEqual(Data* a, int n)
{
    int i, j;
    Data current;
    for (i = 1; i < n; i++) {
        current = a[i];
        movements++;
        j = i - 1;
        while (j >= 0) {
            comparisons++;
            if (a[j].key1 >= current.key1) {
                a[j + 1] = a[j];
                movements++;
                j--;
            }
            else {
                break;
            }
        }
        a[j + 1] = current;
        movements++;
    }
}

int isSorted(Data* a, int n)
{
    int i;
    for (i = 0; i < n - 1; i++) {
        if (a[i].key1 > a[i + 1].key1) {
            return 0;
        }
    }
    return 1;
}

int isStable(Data* a, int n)
{
    int i;
    for (i = 0; i < n - 1; i++) {
        if (a[i].key1 == a[i + 1].key1 && a[i].order > a[i + 1].order) {
            return 0;
        }
    }
    return 1;
}

int compareForPrepare(const void* x, const void* y)
{
    const Data* p = (const Data*)x;
    const Data* q = (const Data*)y;
    if (p->key1 != q->key1) {
        return (p->key1 > q->key1) - (p->key1 < q->key1);
    }
    return (p->order > q->order) - (p->order < q->order);
}

void makeSortedData(Data* dest, const Data* src, int n)
{
    memcpy(dest, src, sizeof(Data) * n);
    qsort(dest, n, sizeof(Data), compareForPrepare);
}

void makeReverseSortedData(Data* dest, const Data* sorted, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        dest[i] = sorted[n - 1 - i];
    }
}

void makePartiallySortedData(Data* dest, const Data* sorted, int n)
{
    int i, p, q;
    Data temp;
    memcpy(dest, sorted, sizeof(Data) * n);
    for (i = 0; i < n / 20; i++) {
        p = rand() % n;
        q = rand() % n;
        temp = dest[p];
        dest[p] = dest[q];
        dest[q] = temp;
    }
}

int runSort(SortFunc sort, const Data* original, Data* work, int n)
{
    memcpy(work, original, sizeof(Data) * n);
    comparisons = 0;
    movements = 0;
    sort(work, n);
    return isSorted(work, n);
}

void runRandomExperiment(SortFunc* sorts, const char** names)
{
    static Data original[DATA_COUNT];
    static Data work[DATA_COUNT];
    long long totalComparisons[SORT_COUNT] = { 0 };
    long long totalMovements[SORT_COUNT] = { 0 };
    int allCorrect[SORT_COUNT] = { 1, 1, 1, 1 };
    int t, s;

    for (t = 0; t < TRIALS; t++) {
        generateRandomData(original, DATA_COUNT);
        for (s = 0; s < SORT_COUNT; s++) {
            if (!runSort(sorts[s], original, work, DATA_COUNT)) {
                allCorrect[s] = 0;
            }
            totalComparisons[s] += comparisons;
            totalMovements[s] += movements;
        }
    }

    printf("Random Data\n");
    printf("Number of Trials : %d\n\n", TRIALS);
    for (s = 0; s < SORT_COUNT; s++) {
        printf("%s\n", names[s]);
        printf("Total Comparisons   : %lld\n", totalComparisons[s]);
        printf("Average Comparisons : %.2f\n", (double)totalComparisons[s] / TRIALS);
        printf("Average Movements   : %.2f\n", (double)totalMovements[s] / TRIALS);
        printf("Sorted Check        : %s\n\n", allCorrect[s] ? "Correct" : "Incorrect");
    }
}

void runSingleExperiment(const char* title, const Data* original, SortFunc* sorts, const char** names)
{
    static Data work[DATA_COUNT];
    int s, correct;

    printf("%s\n\n", title);
    for (s = 0; s < SORT_COUNT; s++) {
        correct = runSort(sorts[s], original, work, DATA_COUNT);
        printf("%s\n", names[s]);
        printf("Comparisons : %lld\n", comparisons);
        printf("Movements   : %lld\n", movements);
        printf("Sorted Check : %s\n\n", correct ? "Correct" : "Incorrect");
    }
}

void runStabilityTest(void)
{
    static Data original[STABLE_COUNT];
    static Data work[STABLE_COUNT];
    SortFunc sorts[4] = { selectionSort, bubbleSort, insertionSortGreater, insertionSortGreaterEqual };
    const char* names[4] = {
        "Selection Sort      ",
        "Bubble Sort         ",
        "Insertion Sort (>)  ",
        "Insertion Sort (>=) "
    };
    int i, s, correct;

    for (i = 0; i < STABLE_COUNT; i++) {
        original[i].key1 = rand() % STABLE_KEY_RANGE;
        original[i].key2 = randomKey();
        original[i].order = i;
    }

    printf("Stability Verification\n\n");
    for (s = 0; s < 4; s++) {
        correct = runSort(sorts[s], original, work, STABLE_COUNT);
        if (!correct) {
            printf("%s: Sort Error\n", names[s]);
        }
        else if (isStable(work, STABLE_COUNT)) {
            printf("%s: Stable\n", names[s]);
        }
        else {
            printf("%s: Unstable\n", names[s]);
        }
    }
}

int main(void)
{
    static Data randomData[DATA_COUNT];
    static Data sortedData[DATA_COUNT];
    static Data reverseData[DATA_COUNT];
    static Data partialData[DATA_COUNT];
    SortFunc sorts[SORT_COUNT] = { selectionSort, bubbleSort, bubbleSortEarly, insertionSortGreater };
    const char* names[SORT_COUNT] = {
        "Selection Sort",
        "Bubble Sort",
        "Bubble Sort (Early Termination)",
        "Insertion Sort"
    };

    srand((unsigned int)time(NULL));

    printf("Data Count : %d\n\n", DATA_COUNT);

    runRandomExperiment(sorts, names);

    generateRandomData(randomData, DATA_COUNT);
    makeSortedData(sortedData, randomData, DATA_COUNT);
    makeReverseSortedData(reverseData, sortedData, DATA_COUNT);
    makePartiallySortedData(partialData, sortedData, DATA_COUNT);

    runSingleExperiment("Sorted Data", sortedData, sorts, names);
    runSingleExperiment("Reverse-Sorted Data", reverseData, sorts, names);
    runSingleExperiment("Partially Sorted Data", partialData, sorts, names);

    runStabilityTest();

    return 0;
}