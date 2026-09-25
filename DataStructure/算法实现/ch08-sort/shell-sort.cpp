// ============================================================================
// 第8章 排序 —— 希尔排序【经典算法】
//   考点：
//     * 本质是"分组 + 直接插入"，增量逐渐缩小到 1；也叫缩小增量排序
//     * 时间依赖增量序列，约为 O(n^1.3)，最坏 O(n^2)；空间 O(1)
//     * 不稳定！因为相同元素可能被分到不同组，相对次序会变
//     * 手工题常给固定增量（如 5,3,1 或 n/2, n/4, ..., 1），要会写每趟结果
// ============================================================================
#include "../ds_common.h"

static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

// 按指定增量序列排序（这样和手算的趟次完全一致）
void ShellSort(int A[], int n, const int gaps[], int gn, bool trace) {
    for (int k = 0; k < gn; ++k) {
        int gap = gaps[k];
        for (int i = gap; i < n; ++i) {         // 对每个组做直接插入
            int tmp = A[i], j = i - gap;
            while (j >= 0) {
                count_cmp();
                if (A[j] <= tmp) break;
                A[j + gap] = A[j];
                count_move();
                j -= gap;
            }
            A[j + gap] = tmp;
            count_move();
        }
        if (trace) { TRACE("gap = %d: ", gap); print_array("", A, n); }
    }
}

// 常用的折半增量：n/2, n/4, ..., 1
void ShellSortHalf(int A[], int n, bool trace) {
    for (int gap = n / 2; gap >= 1; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            int tmp = A[i], j = i - gap;
            while (j >= 0 && A[j] > tmp) { A[j + gap] = A[j]; j -= gap; }
            A[j + gap] = tmp;
        }
        if (trace) { TRACE("gap = %d: ", gap); print_array("", A, n); }
    }
}

int main() {
    banner("Shell sort");
    int A[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    // ---- 固定增量 5,3,1：和手算题一致 ----
    printf("  --- gaps = 5, 3, 1 (the classic hand-calculation setting) ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    int gaps[] = {5, 3, 1};
    ShellSort(A, N, gaps, 3, true);
    print_array("sorted", A, N);
    expect_array("result with gaps 5,3,1", A, N, eSorted, N);

    // ---- 折半增量 ----
    printf("  --- gaps = n/2, n/4, ... (4, 2, 1) ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    ShellSortHalf(A, N, true);
    print_array("sorted", A, N);
    expect_array("result with halving gaps", A, N, eSorted, N);

    // ---- 不同增量序列的结果都必须有序（但中间过程可能不同） ----
    int g2[] = {1};                                  // 增量只有 1 就退化成直接插入
    for (int i = 0; i < N; ++i) A[i] = base[i];
    ShellSort(A, N, g2, 1, false);
    expect_array("gap = 1 degenerates to insertion sort", A, N, eSorted, N);

    int g3[] = {3, 1};
    for (int i = 0; i < N; ++i) A[i] = base[i];
    ShellSort(A, N, g3, 2, true);
    expect_array("gaps 3,1 also sorts correctly", A, N, eSorted, N);

    // ---- 比较次数：希尔明显少于直接插入 ----
    int sorted[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    reset_counters();
    ShellSortHalf(sorted, N, false);                 // 输入已逆序时希尔优势更明显
    printf("  already-sorted input: cmp = %lld, move = %lld\n", g_cmp, g_move);

    int rev[N] = {97, 76, 65, 49, 49, 38, 27, 13};
    reset_counters();
    ShellSortHalf(rev, N, false);
    printf("  reverse input: shell cmp = %lld, move = %lld\n", g_cmp, g_move);
    printf("  (direct insertion sort would take n(n-1)/2 = %d comparisons here)\n", N * (N - 1) / 2);
    expect_array("reverse input sorted by shell sort", rev, N, eSorted, N);

    // 注意：希尔排序对"已经有序"的输入并不比直接插入快 ——
    // 每个元素仍要在多轮不同增量下参加比较，这是它的特点
    int asc[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    reset_counters();
    ShellSortHalf(asc, N, false);
    printf("  sorted input: shell cmp = %lld (direct insertion would be %d)\n", g_cmp, N - 1);
    expect_array("sorted input stays sorted", asc, N, eSorted, N);

    // 不稳定：演示两个相等的 49 可能换位
    printf("  note: shell sort is NOT stable (elements with the same key can be regrouped)\n");
    int dup[6] = {49, 13, 49, 27, 38, 65};
    ShellSortHalf(dup, 6, false);
    int e2[] = {13, 27, 38, 49, 49, 65};
    expect_array("still correctly sorted", dup, 6, e2, 6);
    return finish();
}
