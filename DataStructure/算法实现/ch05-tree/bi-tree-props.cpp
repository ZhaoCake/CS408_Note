// ============================================================================
// 第5章 树与二叉树 —— 二叉树的统计与性质判定
//   统计：结点数、叶子数、高度、宽度、单分支/双分支结点数
//   判定：完全二叉树、平衡二叉树（AVL 的平衡条件）、二叉排序树
//   判定完全二叉树的口诀：层次遍历时一旦遇到空孩子，后面不能再出现非空结点
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

// ------------------------------- 统计 -------------------------------
int CountNodes(BiTree T)  { return T ? 1 + CountNodes(T->lchild) + CountNodes(T->rchild) : 0; }
int CountLeaves(BiTree T) {
    if (!T) return 0;
    if (!T->lchild && !T->rchild) return 1;
    return CountLeaves(T->lchild) + CountLeaves(T->rchild);
}
int Height(BiTree T) {
    if (!T) return 0;
    int l = Height(T->lchild), r = Height(T->rchild);
    return 1 + (l > r ? l : r);
}
// 宽度：某一层最多的结点数（层次遍历时按层计数）
int Width(BiTree T) {
    if (!T) return 0;
    BiTNode *q[64];
    int head = 0, tail = 0, best = 0;
    q[tail++] = T;
    while (head < tail) {
        int cnt = tail - head;                 // 当前层的结点数
        if (cnt > best) best = cnt;
        for (int i = 0; i < cnt; ++i) {
            BiTNode *p = q[head++];
            if (p->lchild) q[tail++] = p->lchild;
            if (p->rchild) q[tail++] = p->rchild;
        }
    }
    return best;
}
// 单分支结点数 = 只有一个孩子的结点个数
int CountDeg1(BiTree T) {
    if (!T) return 0;
    int self = ((T->lchild == NULL) != (T->rchild == NULL)) ? 1 : 0;
    return self + CountDeg1(T->lchild) + CountDeg1(T->rchild);
}
// 双分支结点数（叶子和单分支的互补关系可以互相验证：n0 = n2 + 1）
int CountDeg2(BiTree T) {
    if (!T) return 0;
    int self = (T->lchild && T->rchild) ? 1 : 0;
    return self + CountDeg2(T->lchild) + CountDeg2(T->rchild);
}

// ------------------------------- 判定 -------------------------------
// 完全二叉树：层次遍历，遇到第一个"空孩子"之后不允许再出现非空结点
bool IsComplete(BiTree T) {
    if (!T) return true;
    BiTNode *q[64];
    int head = 0, tail = 0;
    q[tail++] = T;
    int seenEmpty = 0;
    while (head < tail) {
        BiTNode *p = q[head++];
        if (p->lchild) {
            if (seenEmpty) return false;
            q[tail++] = p->lchild;
        } else {
            seenEmpty = 1;
        }
        if (p->rchild) {
            if (seenEmpty) return false;
            q[tail++] = p->rchild;
        } else {
            seenEmpty = 1;
        }
    }
    return true;
}

// 平衡二叉树（每个结点的左右子树高度差 <= 1）；用 -1 表示"已经不平衡"
static int BalanceCheck(BiTree T) {
    if (!T) return 0;
    int l = BalanceCheck(T->lchild);
    if (l == -1) return -1;
    int r = BalanceCheck(T->rchild);
    if (r == -1) return -1;
    if (l - r > 1 || r - l > 1) return -1;
    return 1 + (l > r ? l : r);
}
bool IsBalanced(BiTree T) { return BalanceCheck(T) != -1; }

// 二叉排序树：中序遍历必须严格递增
static BiTNode *g_prev = NULL;
static bool InOrderAsc(BiTree T) {
    if (!T) return true;
    if (!InOrderAsc(T->lchild)) return false;
    if (g_prev && g_prev->data >= T->data) return false;
    g_prev = T;
    return InOrderAsc(T->rchild);
}
bool IsBST(BiTree T) { g_prev = NULL; return InOrderAsc(T); }

int main() {
    banner("Binary tree : statistics and property tests");
    //         1
    //       /   \ 
    //      2     3
    //     / \   /
    //    4   5 6
    BiTree T = N(1, N(2, N(4), N(5)), N(3, N(6), NULL));

    expect_eq("CountNodes", CountNodes(T), 6);
    expect_eq("CountLeaves", CountLeaves(T), 3);        // 4,5,6
    expect_eq("Height", Height(T), 3);
    expect_eq("Width", Width(T), 3);                    // 第 2 层有 2,3；第 3 层有 4,5,6
    expect_eq("CountDeg1", CountDeg1(T), 1);            // 只有 3 是单分支
    expect_eq("CountDeg2", CountDeg2(T), 2);            // 1 和 2
    // 性质互验：n0 = n2 + 1，且 n0 + n1 + n2 = n
    expect_eq("n0 == n2 + 1", CountLeaves(T), CountDeg2(T) + 1);
    expect_eq("n0+n1+n2 == n", CountLeaves(T) + CountDeg1(T) + CountDeg2(T), CountNodes(T));

    expect_true("this tree is complete", IsComplete(T));
    expect_true("this tree is balanced", IsBalanced(T));

    // 不是完全二叉树：中间空一个
    //      1
    //     / \ 
    //    2   3
    //     \ 
    //      4
    BiTree bad = N(1, N(2, NULL, N(4)), N(3));
    expect_true("this one is not complete", !IsComplete(bad));
    FreeTree(bad);

    // 不平衡：左子树很高
    BiTree unb = N(1, N(2, N(3, N(4))), NULL);
    expect_true("not balanced", !IsBalanced(unb));
    expect_eq("its height", Height(unb), 4);
    expect_eq("its width", Width(unb), 1);
    FreeTree(unb);

    // 单结点：既完全又平衡，它是 BST
    BiTree one = N(10);
    expect_true("single node is complete", IsComplete(one));
    expect_true("single node is balanced", IsBalanced(one));
    expect_true("single node is BST", IsBST(one));
    expect_eq("single node leaves", CountLeaves(one), 1);
    expect_eq("empty tree height", Height(NULL), 0);

    // BST 判定
    //        4
    //       / \ 
    //      2   6
    //     / \ 
    //    1   3
    BiTree bst = N(4, N(2, N(1), N(3)), N(6));
    expect_true("bst is a BST", IsBST(bst));
    BiTree notBst = N(4, N(2, N(1), N(5)), N(6));        // 5 在左子树里，违规
    expect_true("5 in the left subtree -> not a BST", !IsBST(notBst));
    expect_eq("bst nodes", CountNodes(bst), 5);
    expect_eq("bst leaves", CountLeaves(bst), 3);
    expect_true("bst is balanced", IsBalanced(bst));
    expect_true("bst is complete", IsComplete(bst));

    FreeTree(one); FreeTree(bst); FreeTree(notBst); FreeTree(T);
    return finish();
}
