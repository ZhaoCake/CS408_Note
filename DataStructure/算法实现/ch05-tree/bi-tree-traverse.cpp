// ============================================================================
// 第5章 树与二叉树 —— 二叉树【遍历经典算法】
//   递归三种 + 非递归三种（显式栈）+ 层次遍历（队列）
//   非递归后序是最难的一个：需要用 r 记住"右子树是否已经访问过"
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

// 访问动作统一走 Visit，方便把遍历序列收集起来做断言
static int g_seq[64], g_n;
static void Visit(BiTNode *p) { g_seq[g_n++] = p->data; }
static void ResetSeq() { g_n = 0; }

#define MAXSTK 64

// ------------------------------ 递归版 ------------------------------
void PreOrder(BiTree T)  { if (!T) return; Visit(T); PreOrder(T->lchild); PreOrder(T->rchild); }
void InOrder(BiTree T)   { if (!T) return; InOrder(T->lchild); Visit(T); InOrder(T->rchild); }
void PostOrder(BiTree T) { if (!T) return; PostOrder(T->lchild); PostOrder(T->rchild); Visit(T); }

// ------------------------------ 非递归版 ------------------------------
// 先序：访问后就压右孩子，再压左孩子，弹出来就是"先左后右"
void PreOrderNR(BiTree T) {
    if (!T) return;
    BiTNode *st[MAXSTK];
    int top = -1;
    st[++top] = T;
    while (top >= 0) {
        BiTNode *p = st[top--];
        Visit(p);
        if (p->rchild) st[++top] = p->rchild;
        if (p->lchild) st[++top] = p->lchild;
    }
}

// 中序：一路向左压栈，弹出来访问后转向右子树
void InOrderNR(BiTree T) {
    BiTNode *st[MAXSTK];
    int top = -1;
    BiTNode *p = T;
    while (p || top >= 0) {
        if (p) { st[++top] = p; p = p->lchild; }        // 向左走到底
        else   { p = st[top--]; Visit(p); p = p->rchild; }
    }
}

// 后序：先看右子树是否访问过（用 r 记录上一个被访问的结点）
void PostOrderNR(BiTree T) {
    BiTNode *st[MAXSTK];
    int top = -1;
    BiTNode *p = T, *r = NULL;                          // r = 最近一次访问的结点
    while (p || top >= 0) {
        if (p) {
            st[++top] = p;
            p = p->lchild;
        } else {
            p = st[top];
            if (p->rchild && p->rchild != r) {          // 右子树还在，先去右边
                p = p->rchild;
            } else {                                    // 左右都处理完，可以访问
                Visit(p);
                r = p;
                --top;
                p = NULL;                               // 强制走"弹栈"分支
            }
        }
    }
}

// ------------------------------ 层次遍历 ------------------------------
void LevelOrder(BiTree T) {
    if (!T) return;
    BiTNode *q[MAXSTK];
    int head = 0, tail = 0;
    q[tail++] = T;
    while (head < tail) {
        BiTNode *p = q[head++];
        Visit(p);
        if (p->lchild) q[tail++] = p->lchild;
        if (p->rchild) q[tail++] = p->rchild;
    }
}

int main() {
    banner("Binary tree : traversals");
    //         1
    //       /   \ 
    //      2     3
    //     / \   /
    //    4   5 6
    BiTree T = N(1, N(2, N(4), N(5)), N(3, N(6), NULL));

    ResetSeq(); PreOrder(T);
    int ePre[] = {1, 2, 4, 5, 3, 6};
    expect_array("preorder (recursive)", g_seq, g_n, ePre, 6);
    ResetSeq(); InOrder(T);
    int eIn[] = {4, 2, 5, 1, 6, 3};
    expect_array("inorder (recursive)", g_seq, g_n, eIn, 6);
    ResetSeq(); PostOrder(T);
    int ePost[] = {4, 5, 2, 6, 3, 1};
    expect_array("postorder (recursive)", g_seq, g_n, ePost, 6);

    ResetSeq(); PreOrderNR(T);
    expect_array("preorder (stack)", g_seq, g_n, ePre, 6);
    ResetSeq(); InOrderNR(T);
    expect_array("inorder (stack)", g_seq, g_n, eIn, 6);
    ResetSeq(); PostOrderNR(T);
    expect_array("postorder (stack)", g_seq, g_n, ePost, 6);

    ResetSeq(); LevelOrder(T);
    int eLevel[] = {1, 2, 3, 4, 5, 6};
    expect_array("level order", g_seq, g_n, eLevel, 6);

    // 单结点和空树的边界
    BiTree one = N(7);
    ResetSeq(); PreOrderNR(one);
    expect_eq("single node preorder", g_seq[0], 7);
    expect_eq("single node count", g_n, 1);
    ResetSeq(); InOrder(one);
    expect_eq("single node inorder", g_seq[0], 7);
    ResetSeq(); LevelOrder(NULL);
    expect_eq("empty tree visits nothing", g_n, 0);
    FreeTree(one);

    // 只有右孩子的"斜树"：非递归后序最容易在这里出错
    BiTree skew = N(1, NULL, N(2, NULL, N(3)));
    ResetSeq(); PostOrderNR(skew);
    int eSkew[] = {3, 2, 1};
    expect_array("postorder on a right-skewed tree", g_seq, g_n, eSkew, 3);
    ResetSeq(); PostOrder(skew);
    expect_array("recursive version agrees", g_seq, g_n, eSkew, 3);

    FreeTree(T);
    FreeTree(skew);
    return finish();
}
