// ============================================================================
// 第7章 查找 —— 折半（二分）查找【经典算法】
//   考点：
//     * 只适用于【顺序存储 + 有序】的表（链表不能用，因为要随机访问 A[mid]）
//     * 判定树是一棵平衡二叉树，树高 = floor(log2(n)) + 1 = 最坏比较次数
//     * 成功 ASL = (每个结点的层数之和) / n，常见近似公式 log2(n+1) - 1
//     * 失败 ASL 要用"失败结点（外部结点）"的层数来算
// ============================================================================
#include "../ds_common.h"

#define MAXN 64

// 迭代版：注意 mid 的取法（下取整）和边界的开闭
int BinarySearchIter(const int A[], int n, int key, int *cmp) {
    int lo = 0, hi = n - 1, c = 0;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        ++c;
        if (A[mid] == key) { if (cmp) *cmp = c; return mid; }
        if (A[mid] < key) lo = mid + 1;
        else              hi = mid - 1;
    }
    if (cmp) *cmp = c;
    return -1;
}

int BinarySearchRec(const int A[], int lo, int hi, int key, int *cmp) {
    if (lo > hi) return -1;
    int mid = (lo + hi) / 2;
    ++(*cmp);
    if (A[mid] == key) return mid;
    if (A[mid] < key) return BinarySearchRec(A, mid + 1, hi, key, cmp);
    return BinarySearchRec(A, lo, mid - 1, key, cmp);
}

// 建判定树：每个结点存 A[mid]，用来数层数和树高
typedef struct BSNode { int val; struct BSNode *l, *r; } BSNode;
static BSNode *BuildDecisionTree(const int A[], int lo, int hi) {
    if (lo > hi) return NULL;
    int mid = (lo + hi) / 2;
    BSNode *p = (BSNode *)malloc(sizeof(BSNode));
    p->val = A[mid];
    p->l = BuildDecisionTree(A, lo, mid - 1);
    p->r = BuildDecisionTree(A, mid + 1, hi);
    return p;
}
static void FreeTree(BSNode *p) { if (!p) return; FreeTree(p->l); FreeTree(p->r); free(p); }
static void DepthSum(BSNode *p, int depth, int *sum, int *nodes, int *maxDepth) {
    if (!p) return;
    *sum += depth;
    ++(*nodes);
    if (depth > *maxDepth) *maxDepth = depth;
    DepthSum(p->l, depth + 1, sum, nodes, maxDepth);
    DepthSum(p->r, depth + 1, sum, nodes, maxDepth);
}

int main() {
    banner("Binary search");
    // 王道标准例：11 个元素
    int A[] = {7, 10, 13, 16, 19, 29, 32, 33, 37, 41, 43};
    const int n = 11;
    int cmp = 0;

    expect_eq("find 29 -> mid at first try", BinarySearchIter(A, n, 29, &cmp), 5);
    expect_eq("  compared once", cmp, 1);
    expect_eq("find 7", BinarySearchIter(A, n, 7, &cmp), 0);
    expect_eq("  compared 3 times", cmp, 3);
    expect_eq("find 43 (the last one)", BinarySearchIter(A, n, 43, &cmp), 10);
    expect_eq("  compared 4 times", cmp, 4);
    expect_eq("miss 5 -> -1", BinarySearchIter(A, n, 5, &cmp), -1);
    expect_eq("  compared 3 times", cmp, 3);

    // 递归版必须和迭代版结果一致
    int same = 1;
    for (int k = 0; k <= 50; ++k) {
        int c1 = 0, c2 = 0;
        int r1 = BinarySearchIter(A, n, k, &c1);
        int r2 = BinarySearchRec(A, 0, n - 1, k, &c2);
        if (r1 != r2 || c1 != c2) { same = 0; printf("  mismatch on %d\n", k); }
    }
    expect_true("iterative == recursive (result and comparisons)", same != 0);

    // 判定树：层数之和 / n 就是成功 ASL；树高就是最坏比较次数
    BSNode *root = BuildDecisionTree(A, 0, n - 1);
    int sum = 0, nodes = 0, maxDepth = 0;
    DepthSum(root, 1, &sum, &nodes, &maxDepth);
    printf("  decision tree: nodes = %d, sum of depths = %d, height = %d\n", nodes, sum, maxDepth);
    expect_eq("decision tree has n nodes", nodes, n);
    expect_eq("ASL numerator", sum, 33);              // 1*1 + 2*2 + 3*4 + 4*4
    printf("  ASL(success) = %.4f  (approx log2(n+1)-1 = %.4f)\n",
           sum / (double)n, std::log2(n + 1) - 1);
    expect_true("ASL == 3.0", sum / (double)n == 3.0);
    expect_eq("worst case comparisons == tree height", maxDepth, 4);
    expect_eq("height == floor(log2(n)) + 1", maxDepth, 4);
    FreeTree(root);

    // 找不到的情况，最坏比较次数也是树高
    int worst = 0;
    for (int k = -5; k <= 60; ++k) {
        BinarySearchIter(A, n, k, &cmp);
        if (cmp > worst) worst = cmp;
    }
    expect_eq("worst comparisons in any search", worst, 4);

    // n = 2^k - 1 时判定树是满二叉树（这是常考性质）
    int full[] = {1, 2, 3, 4, 5, 6, 7};
    BSNode *t2 = BuildDecisionTree(full, 0, 6);
    sum = nodes = maxDepth = 0;
    DepthSum(t2, 1, &sum, &nodes, &maxDepth);
    expect_eq("n = 7 -> complete tree of height 3", maxDepth, 3);
    expect_eq("n = 7 -> 7 nodes", nodes, 7);
    FreeTree(t2);

    // 单元素表
    int one[] = {5};
    expect_eq("single element found", BinarySearchIter(one, 1, 5, &cmp), 0);
    expect_eq("single element miss", BinarySearchIter(one, 1, 9, &cmp), -1);
    return finish();
}
