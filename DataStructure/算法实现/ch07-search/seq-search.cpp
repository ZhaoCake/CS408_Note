// ============================================================================
// 第7章 查找 —— 顺序查找【经典算法】
//   考点：
//     * 普通顺序查找：成功 ASL = (n+1)/2，失败要比较 n 次（或 n+1 次，看有没有哨兵）
//     * 带哨兵（把 key 放在 A[0]）：循环里只要判断 A[i] != key，不用判断越界，
//       代价是失败时比较 n+1 次；这是典型的"用空间/数据换时间"
//     * 有序表的顺序查找失败时可以提前结束，失败 ASL 会降到 n/2 + n/(n+1)
// ============================================================================
#include "../ds_common.h"

// 普通顺序查找：A[0..n-1]，返回下标，找不到返回 -1
int SeqSearch(const int A[], int n, int key, int *cmp) {
    int c = 0;
    for (int i = 0; i < n; ++i) {
        ++c;
        if (A[i] == key) { if (cmp) *cmp = c; return i; }
    }
    if (cmp) *cmp = c;
    return -1;
}

// 带哨兵：数据在 A[1..n]，A[0] 放 key（哨兵）
int SeqSearchSentinel(int A[], int n, int key, int *cmp) {
    A[0] = key;                    // 哨兵
    int i = n, c = 0;
    while (A[i] != key) { ++c; --i; }
    ++c;
    if (cmp) *cmp = c;
    return (i == 0) ? -1 : i;      // i == 0 表示只匹配到哨兵，说明真数据里没有
}

// 有序表的顺序查找：一旦 A[i] > key 就可以停（失败时更省）
int SeqSearchOrdered(const int A[], int n, int key, int *cmp) {
    int c = 0;
    for (int i = 0; i < n; ++i) {
        ++c;
        if (A[i] == key) { if (cmp) *cmp = c; return i; }
        if (A[i] > key) break;     // 后面的只会更大，不用比了
    }
    if (cmp) *cmp = c;
    return -1;
}

// 成功时的平均查找长度：(1 + 2 + ... + n) / n = (n+1)/2
double ASLSuccess(int n) { return (n + 1) / 2.0; }

int main() {
    banner("Sequential search");
    int A[] = {7, 10, 13, 16, 19, 29, 32, 33, 37, 41, 43};
    const int n = 11;
    int cmp = 0;

    expect_eq("find 7 at index 0", SeqSearch(A, n, 7, &cmp), 0);
    expect_eq("  compared once", cmp, 1);
    expect_eq("find 43 at the end (0-based 10)", SeqSearch(A, n, 43, &cmp), 10);
    expect_eq("  compared n times", cmp, n);

    // 平均情况：第 i 个元素需要 i 次比较
    int total = 0;
    for (int i = 0; i < n; ++i) {
        SeqSearch(A, n, A[i], &cmp);
        total += cmp;
    }
    expect_eq("sum of comparisons over all hits", total, n * (n + 1) / 2);
    printf("  ASL(success) = %.4f  vs formula (n+1)/2 = %.4f\n", total / (double)n, ASLSuccess(n));
    expect_true("measured ASL matches the formula", total / (double)n == ASLSuccess(n));

    // 失败：普通版要比较 n 次
    expect_eq("miss -> -1", SeqSearch(A, n, 100, &cmp), -1);
    expect_eq("  failed after n comparisons", cmp, n);

    // 失败：有序版可以提前退出（100 比第一个就大，直接停）
    expect_eq("ordered: miss -> -1", SeqSearchOrdered(A, n, 100, &cmp), -1);
    expect_eq("  100 is larger than all -> compares n times", cmp, n);
    expect_eq("ordered: miss 12 -> -1", SeqSearchOrdered(A, n, 12, &cmp), -1);
    expect_eq("  stops early (after 3)", cmp, 3);      // 7,10,13 -> 13 > 12 停

    // 哨兵版：多一个存储单元，换来循环里少一次边界判断
    int B[16];
    for (int i = 0; i < n; ++i) B[i + 1] = A[i];
    expect_eq("sentinel: find 19", SeqSearchSentinel(B, n, 19, &cmp), 5);
    printf("  sentinel search for 19 used %d comparisons\n", cmp);
    expect_eq("sentinel: miss -> -1", SeqSearchSentinel(B, n, 100, &cmp), -1);
    expect_eq("  sentinel miss costs n+1", cmp, n + 1);

    // 数据量翻倍，ASL 线性增长（这正是 O(n) 的含义）
    printf("  n : ASL(success)\n");
    for (int k = 5; k <= 20; k += 5) printf("  %2d : %.2f\n", k, ASLSuccess(k));
    expect_true("ASL grows linearly", ASLSuccess(20) == 2 * ASLSuccess(10) - 0.5);
    return finish();
}
