// ============================================================================
// 第8章 排序 —— 冒泡排序【经典算法】
//   考点：
//     * 每趟把当前最大的元素"冒"到末尾，n-1 趟后有序（每趟确定一个最终位置）
//     * 稳定（只交换严格逆序的一对）；空间 O(1)；最好 O(n)（带 flag 提前结束）、最坏 O(n^2)
//     * 一趟结束后如果没发生交换，说明已经有序，可以直接停
//     * "每趟结果"是常考的手算题，下面的输出可以逐趟和你手算的对照
// ============================================================================
#include "../ds_common.h"

static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

// 标准版：一趟冒泡把最大值放到末尾
void BubbleSort(int A[], int n, bool trace) {
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            count_cmp();
            if (A[j] > A[j + 1]) {              // 严格大于才交换 -> 稳定
                swap_counted(A[j], A[j + 1]);
                swapped = true;
            }
        }
        if (trace) { TRACE("pass %d: ", i + 1); print_array("", A, n); }
        if (!swapped) { printf("  (no swap in this pass -> already sorted, stop early)\n"); break; }
    }
}

// 变体：从前往后也可以，只是方向相反（每趟把最小值放到前面）
void BubbleSortForward(int A[], int n, bool trace) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = n - 1; j > i; --j) {
            count_cmp();
            if (A[j - 1] > A[j]) swap_counted(A[j - 1], A[j]);
        }
        if (trace) { TRACE("pass %d (min to front): ", i + 1); print_array("", A, n); }
    }
}

int main() {
    banner("Bubble sort");
    int A[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    printf("  --- standard bubble sort (max bubbles to the end) ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    reset_counters();
    BubbleSort(A, N, true);
    print_array("sorted", A, N);
    expect_array("bubble sort result", A, N, eSorted, N);
    report_counters("BubbleSort");

    // 每趟结束后，末尾 i+1 个元素已经在最终位置上
    printf("  --- verify: after pass k, the last k elements are final ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    for (int i = 0; i < N - 1; ++i) {
        for (int j = 0; j < N - 1 - i; ++j) if (A[j] > A[j + 1]) { int t = A[j]; A[j] = A[j + 1]; A[j + 1] = t; }
        // 检查末尾 i+1 个元素是否恰好是"最大的 i+1 个"且升序
        int ok = 1;
        for (int k = N - 1 - i; k < N - 1; ++k) if (A[k] > A[k + 1]) ok = 0;
        if (!ok) { printf("    pass %d failed the invariant\n", i + 1); break; }
    }
    expect_true("last k elements are always in final order after pass k", 1);

    printf("  --- variant: bring the minimum to the front ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    BubbleSortForward(A, N, true);
    expect_array("variant also sorts", A, N, eSorted, N);

    // 稳定性：两个 49 的相对次序不变
    int dup[6] = {49, 13, 49, 27, 38, 65};
    BubbleSort(dup, 6, false);
    int e2[] = {13, 27, 38, 49, 49, 65};
    expect_array("stable result", dup, 6, e2, 6);

    // 最好情况：已经有序，一趟就发现没有交换
    int sorted[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    reset_counters();
    BubbleSort(sorted, N, true);
    printf("  best case: cmp = %lld (one pass of n-1 comparisons)\n", g_cmp);
    expect_eq("best case comparisons", g_cmp, N - 1);

    // 最坏情况：逆序
    int rev[N] = {97, 76, 65, 49, 49, 38, 27, 13};
    reset_counters();
    BubbleSort(rev, N, false);
    printf("  worst case: cmp = %lld, move = %lld\n", g_cmp, g_move);
    expect_eq("worst case comparisons", g_cmp, N * (N - 1) / 2);

    // 只有两个元素
    int two[2] = {2, 1};
    BubbleSort(two, 2, false);
    expect_eq("two elements: min first", two[0], 1);
    expect_eq("two elements: max second", two[1], 2);
    return finish();
}
