// ============================================================================
// 第8章 排序 —— 简单选择排序【经典算法】
//   考点：
//     * 每趟在未排序区里找最小元素，和未排序区的第一个元素交换
//     * 比较次数永远是 n(n-1)/2（与初始序列无关）；移动次数最少可以到 0
//     * 不稳定：交换是"远距离"的，可能把相等的元素换到前面去
//     * 一趟结束后，前 i+1 个元素已经在最终位置
// ============================================================================
#include "../ds_common.h"

static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

void SelectSort(int A[], int n, bool trace) {
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            count_cmp();
            if (A[j] < A[minIdx]) minIdx = j;      // 只记下标，不急着换
        }
        if (minIdx != i) swap_counted(A[i], A[minIdx]);   // 每趟最多交换一次
        if (trace) { TRACE("pass %d (min = %d at %d): ", i + 1, A[i], minIdx); print_array("", A, n); }
    }
}

// 不稳定性的演示：{2, 2', 1} 排序后两个 2 的相对次序会变
struct Item { int v, id; };
void SelectSortItems(Item A[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        int m = i;
        for (int j = i + 1; j < n; ++j) if (A[j].v < A[m].v) m = j;
        if (m != i) { Item t = A[i]; A[i] = A[m]; A[m] = t; }
    }
}

int main() {
    banner("Simple selection sort");
    int A[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    reset_counters();
    SelectSort(A, N, true);
    print_array("sorted", A, N);
    expect_array("selection sort result", A, N, eSorted, N);
    report_counters("SelectSort");

    // 比较次数固定是 n(n-1)/2（这点和初始序列无关，是常考点）
    expect_eq("comparisons == n(n-1)/2", g_cmp, (long long)N * (N - 1) / 2);
    printf("  moves for this input: %lld (at most n-1 swaps = %d)\n", g_move, 3 * (N - 1));
    expect_true("at most n-1 swaps", g_move <= 3 * (N - 1));

    // 已经有序时：比较次数不变，但一次都不换
    int sorted[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    reset_counters();
    SelectSort(sorted, N, false);
    printf("  already sorted: cmp = %lld, move = %lld\n", g_cmp, g_move);
    expect_eq("comparisons unchanged", g_cmp, (long long)N * (N - 1) / 2);
    expect_eq("no moves at all", g_move, 0);

    // 每趟结束后前面 i+1 个元素已在最终位置
    for (int i = 0; i < N; ++i) A[i] = base[i];
    for (int i = 0; i < N - 1; ++i) {
        int m = i;
        for (int j = i + 1; j < N; ++j) if (A[j] < A[m]) m = j;
        int t = A[i]; A[i] = A[m]; A[m] = t;
        if (A[i] > A[i + 1] && i + 1 < N) { printf("    invariant broken at pass %d\n", i + 1); break; }
    }
    expect_array("final result (invariant check)", A, N, eSorted, N);

    // 稳定性反例：两个相等的元素可能被换序
    Item items[3] = {{2, 0}, {2, 1}, {1, 2}};      // 两个 2 的原始先后是 0、1
    SelectSortItems(items, 3);
    printf("  after sorting {2(0), 2(1), 1(2)}: ");
    for (int i = 0; i < 3; ++i) printf("%d(%d) ", items[i].v, items[i].id);
    printf("\n");
    expect_true("selection sort is NOT stable (ids of the two 2s are swapped)",
                !(items[1].id < items[2].id));

    // 逆序输入
    int rev[N] = {97, 76, 65, 49, 49, 38, 27, 13};
    reset_counters();
    SelectSort(rev, N, false);
    expect_array("reverse input also sorted", rev, N, eSorted, N);
    expect_eq("comparisons still the same", g_cmp, (long long)N * (N - 1) / 2);
    return finish();
}
