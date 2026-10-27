#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long cnt;

void insertion_sort(int a[], int n)
{
    int i, j, key;

    for (i = 1; i < n; i++) {
        key = a[i];
        j = i - 1;
        while (j >= 0) {
            cnt++;
            if (a[j] > key) {
                a[j + 1] = a[j];
                j--;
            }
            else {
                break;
            }
        }
        a[j + 1] = key;
    }
}

void shell_sort_original(int a[], int n)
{
    int h, i, j, key;

    for (h = n / 2; h >= 1; h /= 2) {
        for (i = h; i < n; i++) {
            key = a[i];
            j = i - h;
            while (j >= 0) {
                cnt++;
                if (a[j] > key) {
                    a[j + h] = a[j];
                    j -= h;
                }
                else {
                    break;
                }
            }
            a[j + h] = key;
        }
    }
}

void shell_sort_knuth(int a[], int n)
{
    int h, i, j, key;

    h = 1;
    while (3 * h + 1 < n) {
        h = 3 * h + 1;
    }

    for (; h >= 1; h = (h - 1) / 3) {
        for (i = h; i < n; i++) {
            key = a[i];
            j = i - h;
            while (j >= 0) {
                cnt++;
                if (a[j] > key) {
                    a[j + h] = a[j];
                    j -= h;
                }
                else {
                    break;
                }
            }
            a[j + h] = key;
        }
    }
}

int is_sorted(int a[], int n)
{
    int i;

    for (i = 0; i < n - 1; i++) {
        if (a[i] > a[i + 1]) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    int sizes[6] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    int s, i, n;
    int* orig, * work;
    long long c_ins, c_shell, c_knuth;

    srand((unsigned int)time(NULL));

    for (s = 0; s < 6; s++) {
        n = sizes[s];
        orig = (int*)malloc(sizeof(int) * n);
        work = (int*)malloc(sizeof(int) * n);

        for (i = 0; i < n; i++) {
            orig[i] = rand();
        }

        for (i = 0; i < n; i++) work[i] = orig[i];
        cnt = 0;
        insertion_sort(work, n);
        c_ins = cnt;
        if (!is_sorted(work, n)) {
            printf("Insertion Sort failed (n = %d)\n", n);
            return 1;
        }

        for (i = 0; i < n; i++) work[i] = orig[i];
        cnt = 0;
        shell_sort_original(work, n);
        c_shell = cnt;
        if (!is_sorted(work, n)) {
            printf("Shell Sort (Original) failed (n = %d)\n", n);
            return 1;
        }

        for (i = 0; i < n; i++) work[i] = orig[i];
        cnt = 0;
        shell_sort_knuth(work, n);
        c_knuth = cnt;
        if (!is_sorted(work, n)) {
            printf("Shell Sort (Knuth) failed (n = %d)\n", n);
            return 1;
        }

        printf("Data Size : %d\n\n", n);
        printf("Insertion Sort       : %lld comparisons\n", c_ins);
        printf("Shell Sort (Original): %lld comparisons\n", c_shell);
        printf("Shell Sort (Knuth)   : %lld comparisons\n\n", c_knuth);

        free(orig);
        free(work);
    }

    return 0;
}