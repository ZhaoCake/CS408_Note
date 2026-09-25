// ============================================================================
// 第8章 排序 —— 插入类排序（直接插入 + 折半插入）【经典算法】
//   考点：
//     * 直接插入：把 A[i] 插到前面有序区的正确位置；空间 O(1)，稳定
//     * 最好情况（已基本有序）比较 n-1 次：O(n)；最坏/平均 O(n^2)
//     * 折半插入：用折半查找找插入位置，比较次数降到 O(n log n)，但移动次数还是 O(n^2)
//     * "稳定"的原因：遇到相等就停止后移（A[j] <= tmp 就 break）
// ============================================================================
#include "../ds_common.h"

// 数据统一用这组（两个 49 用来验稳定性）
static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

void CopyBase(int A[]) { for (int i = 0; i < N; ++i) A[i] = base[i]; }

// ------------------------------ 直接插入排序 ------------------------------
void InsertSort(int A[], int n, bool trace) {
    for (int i = 1; i < n; ++i) {
        int tmp = A[i], j = i - 1;
        while (j >= 0) {
            count_cmp();
            if (A[j] <= tmp) break;         // 相等就停 -> 保证稳定
            A[j + 1] = A[j];
            count_move();
            --j;
        }
        A[j + 1] = tmp;
        count_move();
        if (trace) { TRACE("pass %d (insert %d): ", i, tmp); print_array("", A, n); }
    }
}

// ------------------------------ 折半插入排序 ------------------------------
void BinaryInsertSort(int A[], int n, bool trace) {
    for (int i = 1; i < n; ++i) {
        int tmp = A[i];
        int lo = 0, hi = i - 1;
        while (lo <= hi) {                  // 折半找插入位置
            int mid = (lo + hi) / 2;
            count_cmp();
            if (A[mid] <= tmp) lo = mid + 1;    // <= 保证稳定（插到相等元素的右边）
            else               hi = mid - 1;
        }
        for (int j = i - 1; j >= lo; --j) {     // 统一后移
            A[j + 1] = A[j];
            count_move();
        }
        A[lo] = tmp;
        count_move();
        if (trace) { TRACE("pass %d (insert %d at %d): ", i, tmp, lo); print_array("", A, n); }
    }
}

int main() {
    banner("Insertion sorts (direct + binary)");
    int A[N], B[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    printf("  --- direct insertion sort ---\n");
    CopyBase(A);
    print_array("input", A, N);
    reset_counters();
    InsertSort(A, N, true);
    print_array("sorted", A, N);
    expect_array("direct insertion result", A, N, eSorted, N);
    report_counters("InsertSort");

    printf("  --- binary insertion sort ---\n");
    CopyBase(B);
    reset_counters();
    BinaryInsertSort(B, N, true);
    print_array("sorted", B, N);
    expect_array("binary insertion result", B, N, eSorted, N);
    report_counters("BinaryInsertSort");

    // 两个 49 的相对次序必须不变 -> 稳定
    int first49 = -1, second49 = -1;
    for (int i = 0; i < N; ++i) {
        if (A[i] == 49) { if (first49 == -1) first49 = i; else second49 = i; }
    }
    expect_eq("two 49s are adjacent after sorting", second49 - first49, 1);
    // 原来的第一个 49 是下标 0，第二个是下标 7：稳定排序后它们仍是这个相对顺序
    // 用一个带标记的版本验证
    struct Item { int v, id; } C[N];
    for (int i = 0; i < N; ++i) { C[i].v = base[i]; C[i].id = i; }
    for (int i = 1; i < N; ++i) {                       // 直接插入（带 id 版）
        Item tmp = C[i];
        int j = i - 1;
        while (j >= 0 && C[j].v > tmp.v) { C[j + 1] = C[j]; --j; }
        C[j + 1] = tmp;
    }
    int stable = 1;
    for (int i = 1; i < N; ++i)
        if (C[i].v == C[i - 1].v && C[i].id < C[i - 1].id) stable = 0;
    expect_true("direct insertion is stable (ids keep their order)", stable != 0);

    // 最好情况：已经有序 -> 比较 n-1 次
    int sorted[N] = {13, 27, 38, 49, 49, 65, 76, 97};
    reset_counters();
    InsertSort(sorted, N, false);
    printf("  best case (already sorted): cmp = %lld, move = %lld\n", g_cmp, g_move);
    expect_eq("best case comparisons", g_cmp, N - 1);

    // 最坏情况：逆序 -> 比较和移动都是 O(n^2)
    int rev[N] = {97, 76, 65, 49, 49, 38, 27, 13};
    reset_counters();
    InsertSort(rev, N, false);
    printf("  worst case (reverse order): cmp = %lld, move = %lld\n", g_cmp, g_move);
    expect_eq("worst case comparisons", g_cmp, N * (N - 1) / 2);

    // 折半插入的比较次数明显少于直接插入（数据同序时移动次数一样）
    CopyBase(A); CopyBase(B);
    reset_counters(); InsertSort(A, N, false);
    long long directCmp = g_cmp, directMove = g_move;
    reset_counters(); BinaryInsertSort(B, N, false);
    long long binCmp = g_cmp, binMove = g_move;
    printf("  comparisons: direct = %lld, binary = %lld\n", directCmp, binCmp);
    printf("  moves      : direct = %lld, binary = %lld (almost the same)\n", directMove, binMove);
    expect_true("binary insertion compares fewer times", binCmp < directCmp);
    expect_true("but moves are still O(n^2)", binMove > 20);
    return finish();
}
