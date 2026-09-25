// ============================================================================
// 第5章 树与二叉树 —— 中序线索二叉树
//   考点：
//     * 线索化的本质：把空的 lchild 指向前驱、空的 rchild 指向后继，用 ltag/rtag 区分
//     * 中序线索化后，中序遍历不再需要栈，也不需要递归（直接顺着 rtag 走）
//     * 找"中序后继"：若 rtag == 1，rchild 就是后继；否则后继是右子树的最左下结点
//     * 找"中序前驱"：若 ltag == 1，lchild 就是前驱；否则前驱是左子树的最右下结点
// ============================================================================
#include "../ds_common.h"

typedef struct ThreadNode {
    int data;
    struct ThreadNode *lchild, *rchild;
    int ltag, rtag;                 // 0 = 指向孩子，1 = 指向前驱/后继线索
} ThreadNode, *ThreadTree;

static ThreadNode *N(int v, ThreadNode *l = NULL, ThreadNode *r = NULL) {
    ThreadNode *p = (ThreadNode *)malloc(sizeof(ThreadNode));
    p->data = v; p->lchild = l; p->rchild = r;
    p->ltag = p->rtag = 0;
    return p;
}
static void FreeTree(ThreadTree T) {
    if (!T) return;
    if (T->ltag == 0) FreeTree(T->lchild);       // 只递归真正的孩子
    if (T->rtag == 0) FreeTree(T->rchild);
    free(T);
}

// ---------------------------------------------------------------------------
// 中序线索化：pre 始终指向"刚刚访问过的结点"，也就是当前结点的中序前驱
// ---------------------------------------------------------------------------
static void InThread(ThreadTree p, ThreadTree &pre) {
    if (!p) return;
    InThread(p->lchild, pre);                    // 递归左子树

    if (!p->lchild) {                            // 左孩子空 -> 变成指向前驱的线索
        p->lchild = pre;
        p->ltag = 1;
    }
    if (pre && !pre->rchild) {                   // 前驱的右孩子空 -> 指向当前结点
        pre->rchild = p;
        pre->rtag = 1;
    }
    pre = p;                                     // 当前结点变成下一个的前驱

    InThread(p->rchild, pre);                    // 递归右子树
}

void CreateInThread(ThreadTree &T) {
    ThreadTree pre = NULL;
    if (T) {
        InThread(T, pre);
        pre->rchild = NULL;                      // 中序最后一个结点的后继为空
        pre->rtag = 1;
    }
}

// ---------------------------------------------------------------------------
// 找中序第一个结点 / 某个结点的中序后继 / 中序最后一个结点 / 中序前驱
// ---------------------------------------------------------------------------
ThreadNode *FirstNode(ThreadNode *p) {
    while (p->ltag == 0) p = p->lchild;          // 一路向左
    return p;
}
ThreadNode *NextNode(ThreadNode *p) {
    if (p->rtag == 0) return FirstNode(p->rchild);// 有右子树 -> 右子树的最左下
    return p->rchild;                             // 否则 rchild 就是后继线索
}
ThreadNode *LastNode(ThreadNode *p) {
    while (p->rtag == 0) p = p->rchild;          // 一路向右
    return p;
}
ThreadNode *PrevNode(ThreadNode *p) {
    if (p->ltag == 0) return LastNode(p->lchild);// 有左子树 -> 左子树的最右下
    return p->lchild;                             // 否则 lchild 就是前驱线索
}

// 中序线索遍历：不用栈、不用递归
void InOrderThread(ThreadTree T) {
    for (ThreadNode *p = FirstNode(T); p; p = NextNode(p))
        printf(" %d", p->data);
    printf("\n");
}

int main() {
    banner("Threaded binary tree (in-order)");
    //         1
    //       /   \ 
    //      2     3
    //     / \   /
    //    4   5 6
    ThreadTree T = N(1, N(2, N(4), N(5)), N(3, N(6), NULL));
    CreateInThread(T);

    printf("  in-order via threads:");
    InOrderThread(T);
    // 中序序列：4 2 5 1 6 3
    int eIn[] = {4, 2, 5, 1, 6, 3};
    int n = 0, seq[8];
    for (ThreadNode *p = FirstNode(T); p; p = NextNode(p)) seq[n++] = p->data;
    expect_array("thread traversal equals inorder", seq, n, eIn, 6);

    expect_eq("first node", FirstNode(T)->data, 4);
    expect_eq("last node", LastNode(T)->data, 3);

    // 后继链：4->2->5->1->6->3->NULL
    ThreadNode *p = FirstNode(T);
    expect_eq("successor of 4", NextNode(p)->data, 2);
    p = NextNode(p);
    expect_eq("successor of 2", NextNode(p)->data, 5);
    p = NextNode(p);
    expect_eq("successor of 5", NextNode(p)->data, 1);
    p = NextNode(p);
    expect_eq("successor of 1", NextNode(p)->data, 6);
    p = NextNode(p);
    expect_eq("successor of 6", NextNode(p)->data, 3);
    p = NextNode(p);
    expect_true("last node has no successor", NextNode(p) == NULL);

    // 前驱链：3->6->1->5->2->4
    ThreadNode *q = LastNode(T);
    expect_eq("predecessor of 3", PrevNode(q)->data, 6);
    q = PrevNode(q);
    expect_eq("predecessor of 6", PrevNode(q)->data, 1);
    q = PrevNode(q);
    expect_eq("predecessor of 1", PrevNode(q)->data, 5);
    q = PrevNode(q);
    expect_eq("predecessor of 5", PrevNode(q)->data, 2);
    q = PrevNode(q);
    expect_eq("predecessor of 2", PrevNode(q)->data, 4);
    q = PrevNode(q);
    expect_true("first node has no predecessor", PrevNode(q) == NULL);

    // 线索标志：没有左孩子的结点，ltag 一定是 1（存的是前驱线索）
    expect_eq("node 4 has no left child -> ltag = 1", FirstNode(T)->ltag, 1);
    expect_eq("node 5 is a leaf -> ltag = 1", NextNode(NextNode(FirstNode(T)))->ltag, 1);
    expect_eq("node 1 has a left child -> ltag = 0", T->ltag, 0);

    // 自洽性检查：NextNode(PrevNode(x)) == x
    int same = 1;
    for (ThreadNode *t = FirstNode(T); t; t = NextNode(t)) {
        ThreadNode *nx = NextNode(t);
        if (nx && PrevNode(nx) != t) { same = 0; break; }
    }
    expect_true("NextNode(PrevNode(x)) == x", same != 0);

    FreeTree(T);
    return finish();
}
