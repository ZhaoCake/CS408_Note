// ============================================================================
// 第5章 树与二叉树 —— 二叉树【遍历应用经典算法】
//   交换左右子树 / 删除值为 x 的子树 / 最近公共祖先 / 先序第 k 个结点 / 所有根到叶路径
//   这些题都是"遍历 + 改指针"或"遍历 + 计数"，关键是递归的返回值含义要清楚
// ============================================================================
#include "../ds_common.h"

typedef struct BiTNode {
    int data;
    struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

static BiTNode *N(int v, BiTNode *l = NULL, BiTNode *r = NULL) {
    BiTNode *p = (BiTNode *)malloc(sizeof(BiTNode));
    p->data = v; p->lchild = l; p->rchild = r;
    return p;
}
static void FreeTree(BiTree T) {
    if (!T) return;
    FreeTree(T->lchild);
    FreeTree(T->rchild);
    free(T);
}
static int g_seq[64], g_n;
static void Visit(BiTNode *p) { g_seq[g_n++] = p->data; }
static void ResetSeq() { g_n = 0; }
void PreOrder(BiTree T) { if (!T) return; Visit(T); PreOrder(T->lchild); PreOrder(T->rchild); }

// ---------------------------------------------------------------------------
// 1) 交换所有结点的左右子树（先交换、再递归）
// ---------------------------------------------------------------------------
void SwapChildren(BiTree T) {
    if (!T) return;
    BiTNode *t = T->lchild;            // 交换自己的两个孩子
    T->lchild = T->rchild;
    T->rchild = t;
    SwapChildren(T->lchild);
    SwapChildren(T->rchild);
}

// ---------------------------------------------------------------------------
// 2) 删除值为 x 的结点所对应的整棵子树（T 要传引用，才能把指针改成 NULL）
// ---------------------------------------------------------------------------
void DeleteSubtree(BiTree &T, int x) {
    if (!T) return;
    if (T->data == x) { FreeTree(T); T = NULL; return; }   // 整棵子树一起释放
    DeleteSubtree(T->lchild, x);
    DeleteSubtree(T->rchild, x);
}

// ---------------------------------------------------------------------------
// 3) 最近公共祖先（LCA）：左右子树各找到一个目标，那当前结点就是 LCA
// ---------------------------------------------------------------------------
BiTNode *LCA(BiTree T, int a, int b) {
    if (!T) return NULL;
    if (T->data == a || T->data == b) return T;            // 自己就是目标
    BiTNode *l = LCA(T->lchild, a, b);
    BiTNode *r = LCA(T->rchild, a, b);
    if (l && r) return T;                                  // 一边一个 -> 当前结点是 LCA
    return l ? l : r;
}

// ---------------------------------------------------------------------------
// 4) 先序遍历的第 k 个结点
// ---------------------------------------------------------------------------
static BiTNode *KthPre(BiTree T, int k, int &cnt) {
    if (!T) return NULL;
    if (++cnt == k) return T;
    BiTNode *l = KthPre(T->lchild, k, cnt);
    if (l) return l;
    return KthPre(T->rchild, k, cnt);
}
BiTNode *KthPreOrder(BiTree T, int k) {
    if (k <= 0) return NULL;
    int cnt = 0;
    return KthPre(T, k, cnt);
}

// ---------------------------------------------------------------------------
// 5) 输出所有根到叶子的路径
// ---------------------------------------------------------------------------
static void PathRec(BiTree T, int path[], int depth) {
    if (!T) return;
    path[depth] = T->data;
    if (!T->lchild && !T->rchild) {                        // 叶子 -> 输出一条完整路径
        printf("    ");
        for (int i = 0; i <= depth; ++i) printf("%d%s", path[i], i < depth ? " -> " : "\n");
        count_cmp();
        return;
    }
    PathRec(T->lchild, path, depth + 1);
    PathRec(T->rchild, path, depth + 1);
}
void PrintAllPaths(BiTree T) {
    int path[64];
    PathRec(T, path, 0);
}

// ---------------------------------------------------------------------------
// 6) 求值为 x 的结点的层次（根为第 1 层），找不到返回 0
// ---------------------------------------------------------------------------
int LevelOf(BiTree T, int x, int depth) {
    if (!T) return 0;
    if (T->data == x) return depth;
    int l = LevelOf(T->lchild, x, depth + 1);
    return l ? l : LevelOf(T->rchild, x, depth + 1);
}

int main() {
    banner("Binary tree : traversal applications");
    //         1
    //       /   \ 
    //      2     3
    //     / \   /
    //    4   5 6
    BiTree T = N(1, N(2, N(4), N(5)), N(3, N(6), NULL));

    // ---- LCA ----
    expect_eq("LCA(4,5)", LCA(T, 4, 5)->data, 2);
    expect_eq("LCA(4,6)", LCA(T, 4, 6)->data, 1);
    expect_eq("LCA(1,6)", LCA(T, 1, 6)->data, 1);
    expect_eq("LCA(2,5)", LCA(T, 2, 5)->data, 2);
    expect_true("LCA of a missing value", LCA(T, 4, 99) != NULL);

    // ---- 先序第 k 个 ----
    expect_eq("1st in preorder", KthPreOrder(T, 1)->data, 1);
    expect_eq("3rd in preorder", KthPreOrder(T, 3)->data, 4);
    expect_eq("6th in preorder", KthPreOrder(T, 6)->data, 6);
    expect_true("k out of range -> NULL", KthPreOrder(T, 99) == NULL);

    // ---- 层次 ----
    expect_eq("level of 1", LevelOf(T, 1, 1), 1);
    expect_eq("level of 2", LevelOf(T, 2, 1), 2);
    expect_eq("level of 6", LevelOf(T, 6, 1), 3);
    expect_eq("level of a missing value", LevelOf(T, 99, 1), 0);

    // ---- 所有根到叶路径 ----
    printf("  all root-to-leaf paths:\n");
    PrintAllPaths(T);

    // ---- 交换左右子树 ----
    SwapChildren(T);
    ResetSeq(); PreOrder(T);
    int eSwap[] = {1, 3, 6, 2, 5, 4};
    expect_array("after SwapChildren (preorder)", g_seq, g_n, eSwap, 6);
    SwapChildren(T);                                   // 换回来
    ResetSeq(); PreOrder(T);
    int eBack[] = {1, 2, 4, 5, 3, 6};
    expect_array("swapped twice == original", g_seq, g_n, eBack, 6);

    // ---- 删除子树 ----
    DeleteSubtree(T, 3);                               // 删掉 3 及其孩子 6
    ResetSeq(); PreOrder(T);
    int eDel[] = {1, 2, 4, 5};
    expect_array("after deleting subtree 3", g_seq, g_n, eDel, 4);
    expect_true("value 6 is gone", LCA(T, 6, 6) == NULL);

    DeleteSubtree(T, 2);                               // 删掉整棵左子树，只剩根
    ResetSeq(); PreOrder(T);
    int eDel2[] = {1};
    expect_array("after deleting subtree 2", g_seq, g_n, eDel2, 1);

    DeleteSubtree(T, 99);                              // 值不存在 -> 什么都不变
    ResetSeq(); PreOrder(T);
    expect_array("deleting a missing value changes nothing", g_seq, g_n, eDel2, 1);

    FreeTree(T);
    return finish();
}
