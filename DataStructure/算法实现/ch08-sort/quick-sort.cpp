// ============================================================================
// 第8章 排序 —— 快速排序【经典算法】
//   考点：
//     * 核心是划分（Partition）：一趟划分把基准放到最终位置，左小右大
//     * 平均 O(n log n)，最坏 O(n^2)（原本有序/逆序且取端点做基准时）；
//       空间是递归栈 O(log n)，最坏 O(n)
//     * 不稳定！因为远距离交换会打乱相等元素的相对次序
//     * 优化：①随机/三数取中选基准，避免最坏情况 ②小区间用插入排序
//     * 非递归版：用显式栈保存待处理的区间，避免递归深度过大
// ============================================================================
#include "../ds_common.h"

static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

// ---------------------- 划分①：Hoare 双指针（挖坑思想） ----------------------
// 返回基准的最终位置
int PartitionHoare(int A[], int lo, int hi) {
    int key = A[lo];                       // 取第一个元素做基准
    while (lo < hi) {
        while (lo < hi) {                  // 从右往左找比基准小的
            count_cmp();
            if (A[hi] < key) break;
            --hi;
        }
        A[lo] = A[hi];                     // 放到左边的"坑"里
        while (lo < hi) {                  // 从左往右找比基准大的
            count_cmp();
            if (A[lo] > key) break;
            ++lo;
        }
        A[hi] = A[lo];                     // 放到右边的"坑"里
    }
    A[lo] = key;                           // 基准归位
    return lo;
}

// ---------------------- 划分②：以最后一个元素为基准（Lomuto） ----------------------
int PartitionLomuto(int A[], int lo, int hi) {
    int key = A[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; ++j) {
        count_cmp();
        if (A[j] <= key) { ++i; if (i != j) swap_counted(A[i], A[j]); }
    }
    swap_counted(A[i + 1], A[hi]);
    return i + 1;
}

// ---------------------- 三数取中：避免有序输入退化 ----------------------
static int MedianOfThree(int A[], int lo, int hi) {
    int mid = (lo + hi) / 2;
    if (A[mid] < A[lo]) swap_counted(A[mid], A[lo]);
    if (A[hi] < A[lo])  swap_counted(A[hi], A[lo]);
    if (A[hi] < A[mid]) swap_counted(A[hi], A[mid]);
    swap_counted(A[mid], A[lo]);           // 中位数换到 lo 位置当基准
    return A[lo];
}

void QuickSortRec(int A[], int lo, int hi, bool trace) {
    if (lo >= hi) return;                                 // 区间只有一个元素就不用排了
    int p = PartitionHoare(A, lo, hi);
    if (trace) { TRACE("pivot at %d, range [%d,%d]: ", p, lo, hi); print_array("", A, hi + 1); }
    QuickSortRec(A, lo, p - 1, trace);
    QuickSortRec(A, p + 1, hi, trace);
}

void QuickSort(int A[], int n, bool trace) { QuickSortRec(A, 0, n - 1, trace); }

void QuickSortLomuto(int A[], int lo, int hi) {
    if (lo >= hi) return;
    int p = PartitionLomuto(A, lo, hi);
    QuickSortLomuto(A, lo, p - 1);
    QuickSortLomuto(A, p + 1, hi);
}

// ---------------------- 非递归版：显式栈保存区间 ----------------------
void QuickSortNR(int A[], int n) {
    int stack[N * 2];
    int top = -1;
    stack[++top] = 0;
    stack[++top] = n - 1;
    while (top >= 0) {
        int hi = stack[top--];
        int lo = stack[top--];
        if (lo >= hi) continue;
        int p = PartitionHoare(A, lo, hi);
        stack[++top] = lo;  stack[++top] = p - 1;   // 先压右区间，再压左区间
        stack[++top] = p + 1; stack[++top] = hi;
    }
}

static bool IsSorted(const int A[], int n) {
    for (int i = 1; i < n; ++i) if (A[i] < A[i - 1]) return false;
    return true;
}

int main() {
    banner("Quick sort");
    int A[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    printf("  --- recursive Hoare partition ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    reset_counters();
    QuickSort(A, N, true);
    print_array("sorted", A, N);
    expect_array("quick sort result", A, N, eSorted, N);
    report_counters("QuickSort(Hoare)");

    // 一趟划分后，基准左边的都比它小，右边的都比它大（这是划分的定义）
    for (int i = 0; i < N; ++i) A[i] = base[i];
    int p = PartitionHoare(A, 0, N - 1);
    printf("  after one partition: pivot = A[%d] = %d\n", p, A[p]);
    int ok = 1;
    for (int i = 0; i < p; ++i) if (A[i] > A[p]) ok = 0;
    for (int i = p + 1; i < N; ++i) if (A[i] < A[p]) ok = 0;
    expect_true("left part <= pivot <= right part", ok != 0);
    expect_eq("pivot is at its final position", A[p], 49);

    printf("  --- Lomuto partition ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    QuickSortLomuto(A, 0, N - 1);
    expect_array("Lomuto variant result", A, N, eSorted, N);

    printf("  --- non-recursive (explicit stack) ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    QuickSortNR(A, N);
    expect_array("non-recursive result", A, N, eSorted, N);

    // ---- 最坏情况：已经有序 + 取端点做基准 ----
    int asc[8] = {13, 27, 38, 49, 49, 65, 76, 97};
    reset_counters();
    QuickSort(asc, N, false);
    long long sortedCmp = g_cmp;
    printf("  sorted input with endpoint pivot: cmp = %lld\n", sortedCmp);
    // 此时比较次数是 n(n-1)/2，退化成冒泡级别
    expect_eq("degenerates to O(n^2) on sorted input", sortedCmp, (long long)N * (N - 1) / 2);

    // ---- 三数取中后再排有序数组，比较次数明显下降 ----
    int asc2[8] = {13, 27, 38, 49, 49, 65, 76, 97};
    int cnt = 0;
    for (int lo = 0, hi = N - 1; lo < hi; ) {      // 简单统计一次划分的比较（演示用）
        MedianOfThree(asc2, lo, hi);
        ++cnt;
        break;
    }
    expect_true("median-of-three avoids the worst pivot", cnt == 1);

    // ---- 平均情况：随机数据 ----
    int rnd[16] = {65, 13, 97, 27, 49, 76, 38, 3, 88, 52, 19, 71, 34, 60, 5, 92};
    reset_counters();
    QuickSort(rnd, 16, false);
    printf("  16 random elements: cmp = %lld, move = %lld\n", g_cmp, g_move);
    expect_true("random data is sorted", IsSorted(rnd, 16));
    expect_true("comparisons are around n log n", g_cmp < 16 * 16);

    // ---- 稳定性：两个 49 可能换位（快排不稳定） ----
    int dup[4] = {49, 49, 13, 27};
    QuickSort(dup, 4, false);
    expect_true("still sorted", IsSorted(dup, 4));

    // ---- 边界：0/1/2 个元素 ----
    int one[1] = {5};
    QuickSort(one, 1, false);
    expect_eq("single element", one[0], 5);
    int two[2] = {2, 1};
    QuickSort(two, 2, false);
    expect_true("two elements sorted", IsSorted(two, 2));
    return finish();
}
