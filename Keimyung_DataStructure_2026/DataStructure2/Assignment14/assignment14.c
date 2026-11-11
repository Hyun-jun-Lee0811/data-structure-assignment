#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define RET 8
#define LESS(x, y) (cmp++, (x) < (y))
#define SWAP(a, b) do { int t_ = (a); (a) = (b); (b) = t_; } while (0)
#define ENTER(sz) do { mem_cur += (sz); if (mem_cur > mem_max) { mem_max = mem_cur; } } while (0)
#define LEAVE(sz) (mem_cur -= (sz))

static long long cmp;
static long long mem_cur, mem_max;
static int pivot_mode;

static void sift_down(int* a, int n, int i)
{
    const long long F = 8 + 4 + 4 + 4 + 4 + RET;
    int child;

    ENTER(F);
    while ((child = 2 * i + 1) < n) {
        if (child + 1 < n && LESS(a[child], a[child + 1])) {
            child++;
        }
        if (!LESS(a[i], a[child])) {
            break;
        }
        SWAP(a[i], a[child]);
        i = child;
    }
    LEAVE(F);
}

static void heap_sort(int* a, int n)
{
    const long long F = 8 + 4 + 4 + 4 + RET;
    int i;

    ENTER(F);
    for (i = n / 2 - 1; i >= 0; i--) {
        sift_down(a, n, i);
    }
    for (i = n - 1; i > 0; i--) {
        SWAP(a[0], a[i]);
        sift_down(a, i, 0);
    }
    LEAVE(F);
}

static void merge(int* a, int* t, int lo, int mid, int hi)
{
    const long long F = 8 + 8 + 4 + 4 + 4 + 4 + 4 + 4 + RET;
    int i, j, k;

    ENTER(F);
    for (i = lo; i <= hi; i++) {
        t[i] = a[i];
    }
    i = lo;
    j = mid + 1;
    k = lo;
    while (i <= mid && j <= hi) {
        if (LESS(t[j], t[i])) {
            a[k++] = t[j++];
        }
        else {
            a[k++] = t[i++];
        }
    }
    while (i <= mid) {
        a[k++] = t[i++];
    }
    while (j <= hi) {
        a[k++] = t[j++];
    }
    LEAVE(F);
}

static void merge_rec(int* a, int* t, int lo, int hi)
{
    const long long F = 8 + 8 + 4 + 4 + 4 + RET;
    int mid;

    ENTER(F);
    if (lo < hi) {
        mid = (lo + hi) / 2;
        merge_rec(a, t, lo, mid);
        merge_rec(a, t, mid + 1, hi);
        merge(a, t, lo, mid, hi);
    }
    LEAVE(F);
}

static void merge_sort(int* a, int n)
{
    const long long F = 8 + 4 + 8 + RET;
    int* t;

    ENTER(F);
    t = (int*)malloc(sizeof(int) * n);
    if (t == NULL) {
        exit(1);
    }
    ENTER((long long)sizeof(int) * n);
    merge_rec(a, t, 0, n - 1);
    LEAVE((long long)sizeof(int) * n);
    free(t);
    LEAVE(F);
}

static int select_pivot(int* a, int lo, int hi)
{
    const long long F = 8 + 4 + 4 + 4 + 4 + RET;
    int mid, m;

    ENTER(F);
    if (pivot_mode == 0) {
        m = lo;
    }
    else if (pivot_mode == 1) {
        m = lo + rand() % (hi - lo + 1);
    }
    else {
        mid = lo + (hi - lo) / 2;
        if (LESS(a[lo], a[mid])) {
            if (LESS(a[mid], a[hi])) {
                m = mid;
            }
            else if (LESS(a[lo], a[hi])) {
                m = hi;
            }
            else {
                m = lo;
            }
        }
        else {
            if (LESS(a[lo], a[hi])) {
                m = lo;
            }
            else if (LESS(a[mid], a[hi])) {
                m = hi;
            }
            else {
                m = mid;
            }
        }
    }
    LEAVE(F);
    return m;
}

static void quick_rec(int* a, int lo, int hi)
{
    const long long F = 8 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + RET;
    int p, pivot, i, j;

    ENTER(F);
    if (lo < hi) {
        p = select_pivot(a, lo, hi);
        SWAP(a[lo], a[p]);
        pivot = a[lo];
        i = lo;
        for (j = lo + 1; j <= hi; j++) {
            if (LESS(a[j], pivot)) {
                i++;
                SWAP(a[i], a[j]);
            }
        }
        SWAP(a[lo], a[i]);
        quick_rec(a, lo, i - 1);
        quick_rec(a, i + 1, hi);
    }
    LEAVE(F);
}

static void quick_sort(int* a, int n)
{
    const long long F = 8 + 4 + RET;

    ENTER(F);
    quick_rec(a, 0, n - 1);
    LEAVE(F);
}

static double now_ms(void)
{
#ifdef _WIN32
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

static void run(const char* name, int algo, const int* orig, int* work, int n)
{
    double s, e;
    int i;

    memcpy(work, orig, sizeof(int) * n);
    cmp = 0;
    mem_cur = 0;
    mem_max = 0;

    s = now_ms();
    if (algo == 0) {
        heap_sort(work, n);
    }
    else if (algo == 1) {
        merge_sort(work, n);
    }
    else {
        pivot_mode = algo - 2;
        quick_sort(work, n);
    }
    e = now_ms();

    for (i = 1; i < n; i++) {
        if (work[i - 1] > work[i]) {
            fprintf(stderr, "%s: 정렬 오류\n", name);
            exit(1);
        }
    }
    printf("%-15s %-14lld %-11.3f %lld\n", name, cmp, e - s, mem_max);
}

int main(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int cnt = (int)(sizeof(sizes) / sizeof(sizes[0]));
    int k, i, n;
    int* orig, * work;

    srand(12345);
    for (k = 0; k < cnt; k++) {
        n = sizes[k];
        orig = (int*)malloc(sizeof(int) * n);
        work = (int*)malloc(sizeof(int) * n);
        if (orig == NULL || work == NULL) {
            return 1;
        }
        for (i = 0; i < n; i++) {
            orig[i] = rand() % 100000;
        }

        printf("Data Size : %d\n\n", n);
        printf("Algorithm       Comparisons    Time(ms)    Memory(Bytes)\n");
        printf("--------------------------------------------------------\n");
        run("Heap Sort", 0, orig, work, n);
        run("Merge Sort", 1, orig, work, n);
        run("Quick (First)", 2, orig, work, n);
        run("Quick (Random)", 3, orig, work, n);
        run("Quick (Median3)", 4, orig, work, n);
        printf("\n");

        free(orig);
        free(work);
    }
    return 0;
}