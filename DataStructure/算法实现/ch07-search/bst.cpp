// ============================================================================
// 第7章 查找 —— 二叉排序树（BST）【经典算法】
//   考点：
//     * 性质：左子树所有值 < 根 < 右子树所有值，所以【中序遍历是递增序列】
//     * 查找 / 插入 / 构造的时间都是 O(树高)；退化成单支树时树高 = n -> O(n)
//     * 删除分三种情况：叶子直接删；只有一个孩子用孩子顶替；两个孩子用
//       直接前驱（左子树最右）或直接后继（右子树最左）顶替，再删那个前驱/后继
//     * 查找失败 ASL 与成功 ASL 都要会算（失败结点是外部结点）
// ============================================================================
#include "../ds_common.h"

typedef struct BSTNode {
    int val;
    struct BSTNode *l, *r;
} BSTNode, *BSTree;

BSTNode *NewNode(int v) {
    BSTNode *p = (BSTNode *)malloc(sizeof(BSTNode));
    p->val = v; p->l = p->r = NULL;
    return p;
}
void FreeTree(BSTree T) { if (!T) return; FreeTree(T->l); FreeTree(T->r); free(T); }

// 查找（迭代）：顺着一路往下，返回结点指针
BSTNode *BSTSearch(BSTree T, int key, int *cmp) {
    int c = 0;
    while (T) {
        ++c;
        if (key == T->val) { if (cmp) *cmp = c; return T; }
        T = (key < T->val) ? T->l : T->r;      // 小了往左，大了往右
    }
    if (cmp) *cmp = c;
    return NULL;
}

// 插入：新结点一定挂在叶子位置上（重复值不插入）
void BSTInsert(BSTree &T, int key) {
    if (!T) { T = NewNode(key); return; }
    if (key < T->val)      BSTInsert(T->l, key);
    else if (key > T->val) BSTInsert(T->r, key);
    // key == T->val 时不插入
}

void BuildBST(BSTree &T, const int a[], int n) {
    T = NULL;
    for (int i = 0; i < n; ++i) BSTInsert(T, a[i]);
}

// 删除：三种情况都要处理
bool BSTDelete(BSTree &T, int key) {
    if (!T) return false;
    if (key < T->val) return BSTDelete(T->l, key);
    if (key > T->val) return BSTDelete(T->r, key);

    if (!T->l && !T->r) {                      // 1) 叶子
        free(T);
        T = NULL;
    } else if (!T->l) {                        // 2) 只有右孩子
        BSTNode *q = T;
        T = T->r;
        free(q);
    } else if (!T->r) {                        // 2) 只有左孩子
        BSTNode *q = T;
        T = T->l;
        free(q);
    } else {                                   // 3) 两个孩子：用直接前驱顶替
        BSTNode *p = T->l;
        while (p->r) p = p->r;                 // 左子树最右结点 = 中序前驱
        T->val = p->val;
        BSTDelete(T->l, p->val);               // 递归删掉那个前驱
    }
    return true;
}

// 中序遍历：BST 的"身份证"，必须是递增的
static int g_seq[64], g_n;
static void InOrder(BSTree T) {
    if (!T) return;
    InOrder(T->l);
    g_seq[g_n++] = T->val;
    InOrder(T->r);
}
bool IsSortedAsc(const int a[], int n) {
    for (int i = 1; i < n; ++i) if (a[i] <= a[i - 1]) return false;
    return true;
}

// 统计 ASL：成功用结点层数，失败用"失败结点（空指针）"的层数
static void CountASL(BSTree T, int depth, int *okSum, int *okCnt, int *failSum, int *failCnt) {
    if (!T) { *failSum += depth; ++(*failCnt); return; }   // 空指针 = 失败结点
    *okSum += depth;
    ++(*okCnt);
    CountASL(T->l, depth + 1, okSum, okCnt, failSum, failCnt);
    CountASL(T->r, depth + 1, okSum, okCnt, failSum, failCnt);
}
void BSTASL(BSTree T, double &aslOk, double &aslFail) {
    int okSum = 0, okCnt = 0, failSum = 0, failCnt = 0;
    CountASL(T, 1, &okSum, &okCnt, &failSum, &failCnt);
    aslOk = okSum / (double)okCnt;
    aslFail = failSum / (double)failCnt;
}

int HeightOf(BSTree T) { if (!T) return 0; int l = HeightOf(T->l), r = HeightOf(T->r); return 1 + (l > r ? l : r); }
int CountNodes(BSTree T) { return T ? 1 + CountNodes(T->l) + CountNodes(T->r) : 0; }
int CountLeaves(BSTree T) {
    if (!T) return 0;
    if (!T->l && !T->r) return 1;
    return CountLeaves(T->l) + CountLeaves(T->r);
}

int main() {
    banner("Binary Search Tree");
    // 王道例：45, 24, 53, 12, 37, 93
    int a[] = {45, 24, 53, 12, 37, 93};
    const int n = 6;
    BSTree T;
    BuildBST(T, a, n);

    g_n = 0; InOrder(T);
    int eIn[] = {12, 24, 37, 45, 53, 93};
    expect_array("inorder of a BST is sorted", g_seq, g_n, eIn, n);
    expect_true("inorder values are increasing", IsSortedAsc(g_seq, g_n));
    expect_eq("node count", CountNodes(T), n);
    expect_eq("height", HeightOf(T), 3);
    expect_eq("leaves", CountLeaves(T), 3);          // 12, 37, 93

    // 查找：比较次数等于结点层数
    int cmp = 0;
    expect_true("find 45", BSTSearch(T, 45, &cmp) != NULL);
    expect_eq("  at depth 1 -> 1 comparison", cmp, 1);
    expect_true("find 37", BSTSearch(T, 37, &cmp) != NULL);
    expect_eq("  at depth 3 -> 3 comparisons", cmp, 3);
    expect_true("not found", BSTSearch(T, 99, &cmp) == NULL);
    expect_eq("  failed after 3 comparisons", cmp, 3);

    double aslOk = 0, aslFail = 0;
    BSTASL(T, aslOk, aslFail);
    printf("  ASL(success) = %.4f, ASL(fail) = %.4f\n", aslOk, aslFail);
    // 层数：45(1) 24(2) 53(2) 12(3) 37(3) 93(3) -> (1+2+2+3+3+3)/6 = 14/6
    expect_true("ASL(success) == 14/6", aslOk == 14.0 / 6.0);
    expect_true("ASL(fail) > ASL(success)", aslFail > aslOk);

    // 重复值不插入
    BSTInsert(T, 45);
    expect_eq("duplicate is ignored", CountNodes(T), n);

    // 插入新值后仍保持 BST 性质
    BSTInsert(T, 50);
    g_n = 0; InOrder(T);
    int eIn2[] = {12, 24, 37, 45, 50, 53, 93};
    expect_array("inorder after inserting 50", g_seq, g_n, eIn2, 7);
    expect_eq("50 sits between 45 and 53", BSTSearch(T, 50, &cmp) != NULL, 1);

    // 删除三种情况
    expect_true("delete a leaf (12)", BSTDelete(T, 12));
    g_n = 0; InOrder(T);
    int eIn3[] = {24, 37, 45, 50, 53, 93};
    expect_array("after deleting leaf 12", g_seq, g_n, eIn3, 6);

    expect_true("delete a node with one child (24)", BSTDelete(T, 24));
    g_n = 0; InOrder(T);
    int eIn4[] = {37, 45, 50, 53, 93};
    expect_array("after deleting 24", g_seq, g_n, eIn4, 5);

    expect_true("delete a node with two children (45)", BSTDelete(T, 45));
    g_n = 0; InOrder(T);
    int eIn5[] = {37, 50, 53, 93};
    expect_array("after deleting 45 (replaced by predecessor 37)", g_seq, g_n, eIn5, 4);
    expect_true("the tree is still a BST", IsSortedAsc(g_seq, g_n));

    expect_true("delete a missing value returns false", !BSTDelete(T, 999));

    // 最坏情况：按递增顺序插入 -> 退化成单支树（链）
    BSTree bad = NULL;
    int inc[] = {1, 2, 3, 4, 5};
    BuildBST(bad, inc, 5);
    expect_eq("increasing insertion -> height n", HeightOf(bad), 5);
    int c2 = 0;
    BSTSearch(bad, 5, &c2);
    expect_eq("  worst case comparisons == n", c2, 5);
    printf("  degraded tree: height = %d, ASL = ?\n", HeightOf(bad));

    FreeTree(T);
    FreeTree(bad);
    return finish();
}
