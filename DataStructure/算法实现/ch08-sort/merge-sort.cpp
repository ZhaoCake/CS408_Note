// ============================================================================
// 第8章 排序 —— 二路归并排序【经典算法】
//   考点：
//     * 分治：先递归排好左右两半，再把两个有序段归并成一个；稳定，空间 O(n)
//     * 时间永远是 O(n log n)（比较次数与初始序列无关，最好最坏一样）
//     * 归并趟数 = ceil(log2 n)；每趟都要 O(n) 的辅助空间，这是它比快排"费空间"的地方
//     * 迭代版（自底向上）更适合手算"每趟归并结果"，也是外部排序的基础
//     * 应用：用归并排序求逆序对（归并时统计"右边元素先出"的次数）
// ============================================================================
#include "../ds_common.h"

static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

// 把有序的 A[lo..mid] 和 A[mid+1..hi] 归并成一个有序段
void Merge(int A[], int tmp[], int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        count_cmp();
        if (A[i] <= A[j]) tmp[k++] = A[i++];      // 相等时先取左边 -> 稳定
        else              tmp[k++] = A[j++];
        count_move();
    }
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

// 迭代版（自底向上）：一趟一趟地把长度为 len 的相邻段归并
void MergeSortIter(int A[], int n, bool trace) {
    int *tmp = (int *)malloc(sizeof(int) * n);
    int pass = 0;
    for (int len = 1; len < n; len *= 2) {
        for (int lo = 0; lo < n; lo += 2 * len) {
            int mid = lo + len - 1;
            int hi = lo + 2 * len - 1;
            if (mid >= n) break;                       // 没有右段可归并
            if (hi >= n) hi = n - 1;                   // 右段不完整
            Merge(A, tmp, lo, mid, hi);
        }
        ++pass;
        if (trace) { TRACE("pass %d (len = %d): ", pass, len); print_array("", A, n); }
    }
    printf("  iterative merge sort used %d passes = ceil(log2(%d)) = %d\n",
           pass, n, (int)std::ceil(std::log2((double)n)));
    free(tmp);
}

static bool IsSorted(const int A[], int n) {
    for (int i = 1; i < n; ++i) if (A[i] < A[i - 1]) return false;
    return true;
}

// 求逆序对数：在归并过程中，右边的元素"提前"出来几次就有几个逆序对
long long g_inv;
void MergeCount(int A[], int tmp[], int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        if (A[i] <= A[j]) tmp[k++] = A[i++];
        else { g_inv += (mid - i + 1); tmp[k++] = A[j++]; }   // 左边剩下的都比 A[j] 大
    }
    while (i <= mid) tmp[k++] = A[i++];
    while (j <= hi)  tmp[k++] = A[j++];
    for (int t = lo; t <= hi; ++t) A[t] = tmp[t];
}
void CountInversion(int A[], int tmp[], int lo, int hi) {
    if (lo >= hi) return;
    int mid = (lo + hi) / 2;
    CountInversion(A, tmp, lo, mid);
    CountInversion(A, tmp, mid + 1, hi);
    MergeCount(A, tmp, lo, mid, hi);
}
long long CountInversions(int A[], int n) {
    int *tmp = (int *)malloc(sizeof(int) * n);
    g_inv = 0;
    CountInversion(A, tmp, 0, n - 1);
    free(tmp);
    return g_inv;
}

int main() {
    banner("Merge sort");
    int A[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    printf("  --- recursive ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    reset_counters();
    MergeSort(A, N);
    print_array("sorted", A, N);
    expect_array("recursive merge sort result", A, N, eSorted, N);
    report_counters("MergeSort");

    printf("  --- iterative (bottom-up), this is what you hand-calculate ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    MergeSortIter(A, N, true);
    expect_array("iterative merge sort result", A, N, eSorted, N);

    // 稳定性：两个 49 的先后次序不变
    struct Item { int v, id; };
    Item items[N];
    for (int i = 0; i < N; ++i) { items[i].v = base[i]; items[i].id = i; }
    Item *tmp = (Item *)malloc(sizeof(Item) * N);
    for (int len = 1; len < N; len *= 2)
        for (int lo = 0; lo < N; lo += 2 * len) {
            int mid = lo + len - 1, hi = lo + 2 * len - 1;
            if (mid >= N) break;
            if (hi >= N) hi = N - 1;
            int i = lo, j = mid + 1, k = lo;
            while (i <= mid && j <= hi) tmp[k++] = (items[i].v <= items[j].v) ? items[i++] : items[j++];
            while (i <= mid) tmp[k++] = items[i++];
            while (j <= hi)  tmp[k++] = items[j++];
            for (int t = lo; t <= hi; ++t) items[t] = tmp[t];
        }
    int stable = 1;
    for (int i = 1; i < N; ++i)
        if (items[i].v == items[i - 1].v && items[i].id < items[i - 1].id) stable = 0;
    expect_true("merge sort IS stable", stable != 0);
    free(tmp);

    // ---- 复杂度：比较次数与初始序列无关（这是归并排序的特点） ----
    int asc[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    int rev[N] = {97, 76, 65, 49, 49, 38, 27, 13};
    reset_counters(); MergeSort(asc, N);
    long long cmpAsc = g_cmp;
    reset_counters(); MergeSort(rev, N);
    long long cmpRev = g_cmp;
    printf("  comparisons: already-sorted = %lld, reverse = %lld\n", cmpAsc, cmpRev);
    expect_true("both are O(n log n)", cmpAsc <= 3LL * N * 3 && cmpRev <= 3LL * N * 3);
    expect_true("comparison counts are close (independent of the input order)",
                (cmpAsc > cmpRev ? cmpAsc - cmpRev : cmpRev - cmpAsc) <= 8);

    // ---- 应用：求逆序对 ----
    int inv[N] = {49, 38, 65, 97, 76, 13, 27, 49};
    long long cnt = CountInversions(inv, N);
    printf("  inversions of the test array = %lld\n", cnt);
    int invSorted[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    expect_eq("sorted array has 0 inversions", CountInversions(invSorted, N), 0);
    int invRev[N] = {97, 76, 65, 49, 49, 38, 27, 13};
    // 注意：这里有两个相等的 49，它们不构成逆序对，所以是 n(n-1)/2 - 1 = 27
    expect_eq("reverse array with one duplicate pair -> 27 inversions",
              CountInversions(invRev, N), (long long)N * (N - 1) / 2 - 1);
    expect_true("the test array has some inversions", cnt > 0);
    expect_true("after counting, the array is sorted", IsSorted(inv, N));
    return finish();
}
