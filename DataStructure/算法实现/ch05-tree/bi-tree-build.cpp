// ============================================================================
// 第5章 树与二叉树 —— 由遍历序列构造二叉树
//   结论（必须记住）：
//     * 先序 + 中序 -> 唯一确定一棵二叉树
//     * 后序 + 中序 -> 唯一确定
//     * 层次 + 中序 -> 唯一确定
//     * 先序 + 后序 -> 不唯一（无法区分只有一个孩子的情况）
//   做法：先序的第一个 / 后序的最后一个 是根，用它在中序里把左右子树切开，递归
// ============================================================================
#include "../ds_common.h"

typedef struct BiTNode {
    int data;
    struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

static void FreeTree(BiTree T) {
    if (!T) return;
    FreeTree(T->lchild);
    FreeTree(T->rchild);
    free(T);
}
static int g_seq[64], g_n;
static void Visit(BiTNode *p) { g_seq[g_n++] = p->data; }
static void ResetSeq() { g_n = 0; }
void PreOrder(BiTree T)  { if (!T) return; Visit(T); PreOrder(T->lchild); PreOrder(T->rchild); }
void InOrder(BiTree T)   { if (!T) return; InOrder(T->lchild); Visit(T); InOrder(T->rchild); }
void PostOrder(BiTree T) { if (!T) return; PostOrder(T->lchild); PostOrder(T->rchild); Visit(T); }

// 在中序序列 in[l..r] 里找值 v 的位置，找不到返回 -1
static int FindIn(const int in[], int l, int r, int v) {
    for (int i = l; i <= r; ++i) if (in[i] == v) return i;
    return -1;
}

// ---------------------- 1) 先序 + 中序 ----------------------
BiTree BuildByPreIn(const int pre[], const int in[], int l, int r, int &preIdx) {
    if (l > r) return NULL;
    int root = pre[preIdx++];                  // 先序序列的当前元素就是根
    BiTree T = (BiTNode *)malloc(sizeof(BiTNode));
    T->data = root;
    int k = FindIn(in, l, r, root);            // 在中序里定位，左边是左子树，右边是右子树
    T->lchild = BuildByPreIn(pre, in, l, k - 1, preIdx);
    T->rchild = BuildByPreIn(pre, in, k + 1, r, preIdx);
    return T;
}

// ---------------------- 2) 后序 + 中序 ----------------------
//    后序序列要从最后一个元素往左用，所以 preIdx 递减
BiTree BuildByPostIn(const int post[], const int in[], int l, int r, int &postIdx) {
    if (l > r) return NULL;
    int root = post[postIdx--];                // 后序的最后一个就是根
    BiTree T = (BiTNode *)malloc(sizeof(BiTNode));
    T->data = root;
    int k = FindIn(in, l, r, root);
    T->rchild = BuildByPostIn(post, in, k + 1, r, postIdx);   // 先建右子树！
    T->lchild = BuildByPostIn(post, in, l, k - 1, postIdx);
    return T;
}

// ---------------------- 3) 层次 + 中序 ----------------------
BiTree BuildByLevelIn(const int level[], int lvN, const int in[], int l, int r) {
    if (l > r) return NULL;
    int idx = -1;
    for (int i = 0; i < lvN; ++i) {            // 层次序列里第一个落在 [l,r] 内的就是根
        int k = FindIn(in, l, r, level[i]);
        if (k != -1) { idx = i; break; }
    }
    if (idx == -1) return NULL;
    BiTree T = (BiTNode *)malloc(sizeof(BiTNode));
    T->data = level[idx];
    int k = FindIn(in, l, r, level[idx]);
    T->lchild = BuildByLevelIn(level, lvN, in, l, k - 1);
    T->rchild = BuildByLevelIn(level, lvN, in, k + 1, r);
    return T;
}

int main() {
    banner("Build a binary tree from traversal sequences");

    // 目标树：      1
    //             /   \ 
    //            2     3
    //           / \   /
    //          4   5 6
    int pre[]   = {1, 2, 4, 5, 3, 6};
    int in[]    = {4, 2, 5, 1, 6, 3};
    int post[]  = {4, 5, 2, 6, 3, 1};
    int level[] = {1, 2, 3, 4, 5, 6};
    int ePre[] = {1, 2, 4, 5, 3, 6};
    int eIn[]  = {4, 2, 5, 1, 6, 3};
    int ePost[] = {4, 5, 2, 6, 3, 1};

    int idx = 0;
    BiTree T1 = BuildByPreIn(pre, in, 0, 5, idx);
    expect_eq("pre+in: all preorder consumed", idx, 6);
    ResetSeq(); PreOrder(T1);
    expect_array("rebuilt preorder", g_seq, g_n, ePre, 6);
    ResetSeq(); InOrder(T1);
    expect_array("rebuilt inorder", g_seq, g_n, eIn, 6);
    FreeTree(T1);

    idx = 5;
    BiTree T2 = BuildByPostIn(post, in, 0, 5, idx);
    expect_eq("post+in: all postorder consumed", idx, -1);
    ResetSeq(); PostOrder(T2);
    expect_array("rebuilt postorder", g_seq, g_n, ePost, 6);
    ResetSeq(); InOrder(T2);
    expect_array("rebuilt inorder", g_seq, g_n, eIn, 6);
    FreeTree(T2);

    BiTree T3 = BuildByLevelIn(level, 6, in, 0, 5);
    ResetSeq(); InOrder(T3);
    expect_array("rebuild from level+in: inorder", g_seq, g_n, eIn, 6);
    ResetSeq(); PreOrder(T3);
    expect_array("rebuild from level+in: preorder", g_seq, g_n, ePre, 6);
    FreeTree(T3);

    // 只有一个结点
    int p1[] = {9}, i1[] = {9};
    int k = 0;
    BiTree T4 = BuildByPreIn(p1, i1, 0, 0, k);
    ResetSeq(); PreOrder(T4);
    expect_eq("single-node tree", g_seq[0], 9);
    expect_eq("single-node count", g_n, 1);
    FreeTree(T4);

    // 空树
    int kk = 0;
    expect_true("empty ranges -> NULL", BuildByPreIn(pre, in, 1, 0, kk) == NULL);
    return finish();
}
