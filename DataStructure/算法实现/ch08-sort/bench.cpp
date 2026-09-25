// ============================================================================
// 第8章 排序 —— 统一评估器：同一组数据下对比所有排序算法的比较/移动次数
//   用法：把这份输出当作"复杂度结论"的实测证据，而不是背下来的文字
//   结论速查（408 必背）：
//     直接插入  O(n^2) / O(n^2) / O(1) 空间 / 稳定
//     折半插入  O(n^2)（比较 O(n log n)）/ O(1) / 稳定
//     希尔      约 O(n^1.3) / O(1) / 不稳定
//     冒泡      O(n) 最好、O(n^2) 最坏 / O(1) / 稳定
//     快速排序  O(n log n) 平均、O(n^2) 最坏 / O(log n) / 不稳定
//     简单选择  O(n^2)（比较恒为 n(n-1)/2）/ O(1) / 不稳定
//     堆排序    O(n log n) / O(1) / 不稳定
//     归并排序  O(n log n)（与初始序列无关）/ O(n) / 稳定
//     基数排序  O(d(n+r)) / O(r) / 稳定
// ============================================================================
#include "../ds_common.h"
#include <algorithm>

#define MAXN 64

// ------------------------------ 九种排序（简版） ------------------------------
void InsertSort(int A[], int n) {
    for (int i = 1; i < n; ++i) {
        int tmp = A[i], j = i - 1;
        while (j >= 0) { count_cmp(); if (A[j] <= tmp) break; A[j + 1] = A[j]; count_move(); --j; }
        A[j + 1] = tmp;
    }
}
void BinaryInsertSort(int A[], int n) {
    for (int i = 1; i < n; ++i) {
        int tmp = A[i], lo = 0, hi = i - 1;
        while (lo <= hi) { int mid = (lo + hi) / 2; count_cmp(); if (A[mid] <= tmp) lo = mid + 1; else hi = mid - 1; }
        for (int j = i - 1; j >= lo; --j) { A[j + 1] = A[j]; count_move(); }
        A[lo] = tmp;
    }
}
void ShellSort(int A[], int n) {
    for (int gap = n / 2; gap >= 1; gap /= 2)
        for (int i = gap; i < n; ++i) {
            int tmp = A[i], j = i - gap;
            while (j >= 0) { count_cmp(); if (A[j] <= tmp) break; A[j + gap] = A[j]; count_move(); j -= gap; }
            A[j + gap] = tmp;
        }
}
void BubbleSort(int A[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        bool sw = false;
        for (int j = 0; j < n - 1 - i; ++j) { count_cmp(); if (A[j] > A[j + 1]) { swap_counted(A[j], A[j + 1]); sw = true; } }
        if (!sw) break;
    }
}
void SelectSort(int A[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        int m = i;
        for (int j = i + 1; j < n; ++j) { count_cmp(); if (A[j] < A[m]) m = j; }
        if (m != i) swap_counted(A[i], A[m]);
    }
}
void SiftDown(int A[], int i, int n) {
    int tmp = A[i], child = 2 * i + 1;
    while (child < n) {
        count_cmp();
        if (child + 1 < n && A[child + 1] > A[child]) ++child;
        count_cmp();
        if (tmp >= A[child]) break;
        A[i] = A[child]; count_move();
        i = child; child = 2 * i + 1;
    }
    A[i] = tmp;
}
void HeapSort(int A[], int n) {
    for (int i = n / 2 - 1; i >= 0; --i) SiftDown(A, i, n);
    for (int i = n - 1; i > 0; --i) { swap_counted(A[0], A[i]); SiftDown(A, 0, i); }
}
int Partition(int A[], int lo, int hi) {
    int key = A[lo];
    while (lo < hi) {
        count_cmp();
        while (lo < hi && A[hi] >= key) { count_cmp(); --hi; }
        A[lo] = A[hi];
        while (lo < hi && A[lo] <= key) { count_cmp(); ++lo; }
        A[hi] = A[lo];
    }
    A[lo] = key;
    return lo;
}
void QuickSortRec(int A[], int lo, int hi) {
    if (lo >= hi) return;
    int p = Partition(A, lo, hi);
    QuickSortRec(A, lo, p - 1);
    QuickSortRec(A, p + 1, hi);
}
void QuickSort(int A[], int n) { QuickSortRec(A, 0, n - 1); }
void Merge(int A[], int tmp[], int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) { count_cmp(); tmp[k++] = (A[i] <= A[j]) ? A[i++] : A[j++]; count_move(); }
    while (i <= mid) { tmp[k++] = A[i++]; count_move(); }
    while (j <= hi)  { tmp[k++] = A[j++]; count_move(); }
    for (int t = lo; t <= hi; ++t) A[t] = tmp[t];
}
void MergeSortRec(int A[], int tmp[], int lo, int hi) {
    if (lo >= hi) return;
    int mid = (lo + hi) / 2;
    MergeSortRec(A, tmp, lo, mid);
    MergeSortRec(A, tmp, mid + 1, hi);
    Merge(A, tmp, lo, mid, hi);
}
void MergeSort(int A[], int n) {
    int *tmp = (int *)malloc(sizeof(int) * n);
    MergeSortRec(A, tmp, 0, n - 1);
    free(tmp);
}
void RadixSort(int A[], int n) {
    int maxV = A[0];
    for (int i = 1; i < n; ++i) if (A[i] > maxV) maxV = A[i];
    int bucket[MAXN][MAXN], cnt[10];
    for (int exp = 1; maxV / exp > 0; exp *= 10) {
        for (int d = 0; d < 10; ++d) cnt[d] = 0;
        for (int i = 0; i < n; ++i) { count_cmp(); int d = (A[i] / exp) % 10; bucket[d][cnt[d]++] = A[i]; }
        int k = 0;
        for (int d = 0; d < 10; ++d) for (int i = 0; i < cnt[d]; ++i) { A[k++] = bucket[d][i]; count_move(); }
    }
}

// ------------------------------ 评估框架 ------------------------------
typedef void (*SortFn)(int[], int);

struct Stat { const char *name; long long cmp, move; double ms; };

int baseData[MAXN], N;

Stat RunOne(const char *name, SortFn fn) {
    int A[MAXN];
    for (int i = 0; i < N; ++i) A[i] = baseData[i];
    reset_counters();
    fn(A, N);
    Stat s{name, g_cmp, g_move, 0};
    // 顺便验证排序结果正确
    for (int i = 1; i < N; ++i) if (A[i] < A[i - 1]) { printf("  !! %s failed\n", name); break; }
    return s;
}

int main() {
    banner("Bench : compare all sorts on the same data");

    // 固定数据（两个 49 用来验稳定性）
    int fixed[] = {49, 38, 65, 97, 76, 13, 27, 49};
    N = 8;
    for (int i = 0; i < N; ++i) baseData[i] = fixed[i];
    printf("  data: ");
    for (int i = 0; i < N; ++i) printf("%d ", baseData[i]);
    printf("\n\n");

    struct { const char *name; SortFn fn; } algs[] = {
        {"insert",        InsertSort},
        {"binary-insert", BinaryInsertSort},
        {"shell",         ShellSort},
        {"bubble",        BubbleSort},
        {"select",        SelectSort},
        {"heap",          HeapSort},
        {"quick",         QuickSort},
        {"merge",         MergeSort},
        {"radix",         RadixSort},
    };

    printf("  %-14s %10s %10s\n", "algorithm", "comparisons", "moves");
    for (int i = 0; i < 9; ++i) {
        Stat s = RunOne(algs[i].name, algs[i].fn);
        printf("  %-14s %10lld %10lld\n", s.name, s.cmp, s.move);
    }

    // ---- 结论验证 ----
    printf("\n  --- what the numbers tell us ---\n");

    // 1) 选择排序的比较次数恒为 n(n-1)/2，与初始序列无关
    Stat sel = RunOne("select", SelectSort);
    expect_eq("selection sort comparisons == n(n-1)/2", sel.cmp, (long long)N * (N - 1) / 2);

    // 2) 折半插入的比较次数少于直接插入
    Stat ins = RunOne("insert", InsertSort);
    Stat bins = RunOne("binary-insert", BinaryInsertSort);
    printf("  insert cmp = %lld, binary-insert cmp = %lld\n", ins.cmp, bins.cmp);
    expect_true("binary insertion compares fewer", bins.cmp < ins.cmp);

    // 3) 归并排序的比较次数与初始序列无关（有序/逆序几乎一样）
    //    注意：排序会原地改动数组，所以复制一份来测，别把 baseData 弄脏
    int rev[MAXN], copyOfBase[MAXN];
    for (int i = 0; i < N; ++i) { rev[i] = baseData[N - 1 - i]; copyOfBase[i] = baseData[i]; }
    reset_counters(); MergeSort(copyOfBase, N);
    long long m1 = g_cmp;
    reset_counters(); MergeSort(rev, N);
    long long m2 = g_cmp;
    printf("  merge: sorted-input cmp = %lld, reverse-input cmp = %lld\n", m1, m2);
    expect_true("merge sort is insensitive to the input order", (m1 > m2 ? m1 - m2 : m2 - m1) <= 4);

    // 4) 快速排序在有序/逆序输入上退化（端点做基准的最坏情况）
    int revSorted[MAXN] = {97, 76, 65, 49, 49, 38, 27, 13};
    int revForMerge[MAXN] = {97, 76, 65, 49, 49, 38, 27, 13};
    reset_counters(); QuickSort(revSorted, N);
    long long q1 = g_cmp;
    reset_counters(); MergeSort(revForMerge, N);
    long long mMerge = g_cmp;
    printf("  reverse-sorted input: quick = %lld comparisons, merge = %lld comparisons\n", q1, mMerge);
    expect_true("quick sort degrades on reverse input (worse than merge sort)", q1 > mMerge);

    // 5) 数据量变大时的整体趋势
    printf("\n  n = 64 random data:\n  %-14s %10s\n", "algorithm", "comparisons");
    N = 64;
    unsigned seed = 12345;
    for (int i = 0; i < N; ++i) { seed = seed * 1103515245u + 12345u; baseData[i] = (int)((seed >> 16) % 100) + 1; }
    for (int i = 0; i < 9; ++i) {
        Stat s = RunOne(algs[i].name, algs[i].fn);
        printf("  %-14s %10lld\n", s.name, s.cmp);
    }
    Stat s1 = RunOne("insert", InsertSort);
    Stat s2 = RunOne("merge", MergeSort);
    Stat s3 = RunOne("quick", QuickSort);
    printf("  comparisons on 64 elements: insert = %lld, merge = %lld, quick = %lld\n",
           s1.cmp, s2.cmp, s3.cmp);
    expect_true("O(n^2) insert blows up", s1.cmp > s2.cmp);
    expect_true("O(n log n) merge stays small", s2.cmp < 64 * 10);
    expect_true("quick sort is fast on random data", s3.cmp < s1.cmp);
    return finish();
}
