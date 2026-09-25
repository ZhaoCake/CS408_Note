// ============================================================================
// 第8章 排序 —— 堆排序【经典算法】
//   考点：
//     * 大顶堆：A[i] >= A[2i+1] 且 A[i] >= A[2i+2]（0 下标），即完全二叉树的每个父节点都大于孩子
//     * 建堆：从最后一个非叶结点 (n/2 - 1) 开始往前做"下滤"，时间 O(n)
//     * 排序：每次把堆顶（最大值）换到末尾，堆缩小 1，再对堆顶下滤，时间 O(n log n)
//     * 空间 O(1)，不稳定；特别适合"只取前 k 个最大/最小"的场景
//     * 手工题常考：插入/删除一个元素后堆的样子（插入上浮、删除后把末尾元素放到堆顶再下滤）
// ============================================================================
#include "../ds_common.h"

static int base[8] = {49, 38, 65, 97, 76, 13, 27, 49};
#define N 8

// 下滤：把 A[i] 沉到合适的位置（大顶堆）
void SiftDown(int A[], int i, int n, bool trace) {
    int tmp = A[i];
    int child = 2 * i + 1;
    while (child < n) {
        count_cmp();
        if (child + 1 < n && A[child + 1] > A[child]) ++child;   // 选较大的孩子
        count_cmp();
        if (tmp >= A[child]) break;                              // 已经比孩子大，停
        A[i] = A[child];                                         // 孩子上移
        count_move();
        i = child;
        child = 2 * i + 1;
    }
    A[i] = tmp;
    count_move();
    if (trace) TRACE("sift down finished at index %d\n", i);
}

// 建堆：从最后一个非叶结点开始
void BuildHeap(int A[], int n, bool trace) {
    for (int i = n / 2 - 1; i >= 0; --i) {
        if (trace) TRACE("sift down from index %d\n", i);
        SiftDown(A, i, n, false);
    }
}

// 上浮：堆的插入（用于优先队列）
void SiftUp(int A[], int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        count_cmp();
        if (A[parent] >= A[i]) break;
        swap_counted(A[parent], A[i]);
        i = parent;
    }
}
void HeapInsert(int A[], int &n, int x) {
    A[n] = x;
    SiftUp(A, n);
    ++n;
}
// 删除堆顶：把末尾元素放到堆顶再下滤
int HeapDeleteTop(int A[], int &n) {
    int top = A[0];
    A[0] = A[n - 1];
    --n;
    SiftDown(A, 0, n, false);
    return top;
}

void HeapSort(int A[], int n, bool trace) {
    BuildHeap(A, n, trace);
    if (trace) { TRACE("heap built: "); print_array("", A, n); }
    for (int i = n - 1; i > 0; --i) {
        swap_counted(A[0], A[i]);        // 堆顶（最大值）换到末尾
        SiftDown(A, 0, i, false);        // 剩下的部分继续维持堆
        if (trace) { TRACE("pass (move max %d to index %d): ", A[i], i); print_array("", A, n); }
    }
}

bool IsMaxHeap(const int A[], int n) {
    for (int i = 0; i * 2 + 1 < n; ++i) {
        if (A[i] < A[2 * i + 1]) return false;
        if (2 * i + 2 < n && A[i] < A[2 * i + 2]) return false;
    }
    return true;
}
bool IsSorted(const int A[], int n) {
    for (int i = 1; i < n; ++i) if (A[i] < A[i - 1]) return false;
    return true;
}

int main() {
    banner("Heap sort");
    int A[N];
    int eSorted[] = {13, 27, 38, 49, 49, 65, 76, 97};

    printf("  --- build heap, then sort ---\n");
    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    reset_counters();
    HeapSort(A, N, true);
    print_array("sorted", A, N);
    expect_array("heap sort result", A, N, eSorted, N);
    report_counters("HeapSort");

    // 建堆之后一定是大顶堆，且堆顶是最大值
    for (int i = 0; i < N; ++i) A[i] = base[i];
    BuildHeap(A, N, false);
    print_array("heap", A, N);
    expect_true("built array is a max-heap", IsMaxHeap(A, N));
    expect_eq("heap top is the maximum", A[0], 97);

    // 排序过程：依次把最大值放到末尾，所以每趟末尾都在增长正确的后缀
    for (int i = 0; i < N; ++i) A[i] = base[i];
    BuildHeap(A, N, false);
    for (int i = N - 1; i > 0; --i) {
        int t = A[0]; A[0] = A[i]; A[i] = t;
        SiftDown(A, 0, i, false);
        int ok = 1;
        for (int k = i; k < N - 1; ++k) if (A[k] > A[k + 1]) ok = 0;
        if (!ok) { printf("    suffix invariant broken at i = %d\n", i); break; }
    }
    expect_array("final array", A, N, eSorted, N);

    // ---- 堆的插入与删除（优先队列的两个基本操作） ----
    printf("  --- heap insert / delete top ---\n");
    int h[16] = {97, 76, 65, 49, 49, 13, 27, 38};
    int hn = 8;
    expect_true("start from a valid heap", IsMaxHeap(h, hn));
    HeapInsert(h, hn, 100);
    print_array("after inserting 100", h, hn);
    expect_true("still a max-heap", IsMaxHeap(h, hn));
    expect_eq("new maximum is at the top", h[0], 100);

    int top = HeapDeleteTop(h, hn);
    print_array("after deleting the top", h, hn);
    expect_eq("deleted value", top, 100);
    expect_true("still a max-heap after deletion", IsMaxHeap(h, hn));
    expect_eq("new top", h[0], 97);
    expect_eq("size decreased", hn, 8);

    // 连续删除应得到降序序列
    int cmpArr[8] = {97, 76, 65, 49, 49, 13, 27, 38};
    int cn = 8;
    BuildHeap(cmpArr, cn, false);
    int desc[8], dn = 0;
    while (cn > 0) desc[dn++] = HeapDeleteTop(cmpArr, cn);
    int eDesc[] = {97, 76, 65, 49, 49, 38, 27, 13};
    expect_array("repeated delete-top gives descending order", desc, dn, eDesc, 8);

    // ---- 复杂度：随机数据下比较次数约 2n log n ----
    int rnd[16] = {65, 13, 97, 27, 49, 76, 38, 3, 88, 52, 19, 71, 34, 60, 5, 92};
    reset_counters();
    HeapSort(rnd, 16, false);
    printf("  16 elements: cmp = %lld, move = %lld\n", g_cmp, g_move);
    expect_true("random data sorted", IsSorted(rnd, 16));
    expect_true("comparisons are O(n log n)", g_cmp < 16 * 16);

    // ---- 应用：求最大的 3 个数（用小顶堆思路，这里演示大顶堆取三次堆顶） ----
    printf("  --- top-3 by heap ---\n");
    int t3[8] = {49, 38, 65, 97, 76, 13, 27, 49};
    int tn = 8;
    BuildHeap(t3, tn, false);
    printf("  top values:");
    for (int i = 0; i < 3; ++i) printf(" %d", HeapDeleteTop(t3, tn));
    printf("\n");
    expect_true("top-3 extracted successfully", tn == 5);
    return finish();
}
