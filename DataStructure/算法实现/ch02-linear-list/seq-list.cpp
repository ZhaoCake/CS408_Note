// ============================================================================
// 第2章 线性表 —— 顺序表【经典算法题】
//   只留算法本体（408 算法设计题的常考问法），不写"初始化/增删改查"这类基本操作
//   约定：参数直接给 (数组, 长度)，复杂度写在函数上，实现一律原地完成
// ============================================================================
#include "../ds_common.h"

// 逆置闭区间 [l, r]（内部辅助）
void ReverseRange(int A[], int l, int r) {
    while (l < r) { swap_counted(A[l], A[r]); ++l; --r; }
}

// 1) 逆置整个数组：O(n) 时间、O(1) 空间
void Reverse(int A[], int n) { ReverseRange(A, 0, n - 1); }

// 2) 删除所有值为 x 的元素，返回新长度：O(n)、O(1)
//    思路：k 记录"保留下来的元素个数"，一趟扫描原地前移
int DeleteX(int A[], int n, int x) {
    int k = 0;
    for (int i = 0; i < n; ++i) {
        count_cmp();
        if (A[i] != x) A[k++] = A[i];
    }
    return k;
}

// 3) 有序表去重（每个值只留一个），返回新长度：O(n)、O(1)
int DeleteDup(int A[], int n) {
    if (n == 0) return 0;
    int k = 1;                                  // 第一个元素一定保留
    for (int i = 1; i < n; ++i) {
        count_cmp();
        if (A[i] != A[k - 1]) A[k++] = A[i];    // 只和"已保留的最后一个"比较
    }
    return k;
}

// 4) 归并两个有序表到 C，返回 C 的长度：O(n+m)、需要 O(n+m) 新空间
int MergeTwo(const int A[], int n, const int B[], int m, int C[]) {
    int i = 0, j = 0, k = 0;
    while (i < n && j < m) {
        count_cmp();
        if (A[i] <= B[j]) C[k++] = A[i++];
        else              C[k++] = B[j++];
    }
    while (i < n) C[k++] = A[i++];
    while (j < m) C[k++] = B[j++];
    return k;
}

// 5) 循环左移 p 位（1,2,3,4,5 左移 2 -> 3,4,5,1,2）：三次逆置，O(n)、O(1)
void LeftRotate(int A[], int n, int p) {
    if (n <= 0) return;
    p %= n;
    if (p <= 0) return;
    ReverseRange(A, 0, p - 1);       // 前半段逆置
    ReverseRange(A, p, n - 1);       // 后半段逆置
    ReverseRange(A, 0, n - 1);       // 整体逆置
}

// 6) 求两个等长有序序列的中位数（合并后的第 n 个）：O(n)、O(1)
//    要点：只走 n 步，绝不真的合并数组
int MedianOfTwo(const int A[], const int B[], int n) {
    int i = 0, j = 0, mid = 0;
    for (int k = 0; k < n; ++k) {
        count_cmp();
        if (i < n && (j >= n || A[i] <= B[j])) mid = A[i++];
        else                                   mid = B[j++];
    }
    return mid;
}

// 7) 主元素（出现次数 > n/2）：摩尔投票，O(n)、O(1)
//    要点：投票只能找出"候选"，最后必须再数一遍验证
int Majority(const int A[], int n) {
    if (n == 0) return -1;
    int cand = A[0], cnt = 0;
    for (int i = 0; i < n; ++i) {
        count_cmp();
        if (cnt == 0)               { cand = A[i]; cnt = 1; }
        else if (A[i] == cand)      ++cnt;
        else                        --cnt;
    }
    int c = 0;
    for (int i = 0; i < n; ++i) { count_cmp(); if (A[i] == cand) ++c; }
    return (c > n / 2) ? cand : -1;
}

// 8) 未出现的最小正整数：O(n) 时间、O(n) 空间（标记数组）
//    例：{3,4,-1,1} -> 2；{1,2,3} -> 4
int FirstMissingPositive(const int A[], int n) {
    bool *seen = (bool *)calloc(n + 2, sizeof(bool));
    for (int i = 0; i < n; ++i)
        if (A[i] > 0 && A[i] <= n + 1) seen[A[i]] = true;
    int r = 1;
    while (r <= n + 1 && seen[r]) ++r;
    free(seen);
    return r;
}

// ============================================================================
int main() {
    banner("Sequential list : classic algorithm problems");

    int a1[] = {1, 2, 3, 4, 5};
    Reverse(a1, 5);
    int e1[] = {5, 4, 3, 2, 1};
    expect_array("Reverse", a1, 5, e1, 5);

    int a2[] = {3, 1, 3, 5, 3};
    int n2 = DeleteX(a2, 5, 3);
    int e2[] = {1, 5};
    expect_eq("DeleteX returned length", n2, 2);
    expect_array("DeleteX", a2, n2, e2, 2);

    int a3[] = {1, 1, 2, 2, 2, 3};
    int n3 = DeleteDup(a3, 6);
    int e3[] = {1, 2, 3};
    expect_eq("DeleteDup returned length", n3, 3);
    expect_array("DeleteDup", a3, n3, e3, 3);

    int a4[] = {1, 3, 5}, b4[] = {2, 4, 6}, c4[8];
    int n4 = MergeTwo(a4, 3, b4, 3, c4);
    int e4[] = {1, 2, 3, 4, 5, 6};
    expect_eq("MergeTwo returned length", n4, 6);
    expect_array("MergeTwo", c4, n4, e4, 6);

    int a5[] = {1, 2, 3, 4, 5};
    LeftRotate(a5, 5, 2);
    int e5[] = {3, 4, 5, 1, 2};
    expect_array("LeftRotate by 2", a5, 5, e5, 5);
    int a5b[] = {1, 2, 3, 4, 5};
    LeftRotate(a5b, 5, 7);                    // p 可以大于 n
    expect_array("LeftRotate by 7 == by 2", a5b, 5, e5, 5);

    int a6[] = {1, 3, 5}, b6[] = {2, 4, 6};
    expect_eq("MedianOfTwo", MedianOfTwo(a6, b6, 3), 3);
    int a6b[] = {11, 13, 15, 17, 19}, b6b[] = {2, 4, 6, 8, 20};
    expect_eq("MedianOfTwo (second case)", MedianOfTwo(a6b, b6b, 5), 11);

    int a7[] = {1, 2, 3, 1, 1};
    expect_eq("Majority found", Majority(a7, 5), 1);
    int a7b[] = {1, 2, 3, 1};
    expect_eq("Majority not found", Majority(a7b, 4), -1);
    int a7c[] = {5, 5, 5, 5};
    expect_eq("Majority all same", Majority(a7c, 4), 5);

    int a8[] = {3, 4, -1, 1};
    expect_eq("FirstMissingPositive {3,4,-1,1}", FirstMissingPositive(a8, 4), 2);
    int a8b[] = {1, 2, 3};
    expect_eq("FirstMissingPositive {1,2,3}", FirstMissingPositive(a8b, 3), 4);
    int a8c[] = {7, 8, 9};
    expect_eq("FirstMissingPositive {7,8,9}", FirstMissingPositive(a8c, 3), 1);

    return finish();
}
