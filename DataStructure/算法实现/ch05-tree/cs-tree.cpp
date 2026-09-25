// ============================================================================
// 第5章 树与二叉树 —— 树/森林与二叉树的转换（孩子兄弟表示法）
//   关键结论（必须背下来）：
//     * 孩子兄弟表示法里：firstChild 当左孩子，nextSibling 当右孩子
//     * 树的【先根遍历】序列 == 对应二叉树的【先序遍历】序列
//     * 树的【后根遍历】序列 == 对应二叉树的【中序遍历】序列
//     * 森林的遍历同理：森林先序 = 二叉树先序；森林中序（后根）= 二叉树中序
// ============================================================================
#include "../ds_common.h"

typedef struct CSNode {
    int data;
    struct CSNode *firstChild, *nextSibling;
} CSNode, *CSTree;

static CSNode *N(int v, CSNode *fc = NULL, CSNode *ns = NULL) {
    CSNode *p = (CSNode *)malloc(sizeof(CSNode));
    p->data = v; p->firstChild = fc; p->nextSibling = ns;
    return p;
}
static void FreeTree(CSTree T) {
    if (!T) return;
    FreeTree(T->firstChild);
    FreeTree(T->nextSibling);
    free(T);
}
static int g_seq[64], g_n;
static void Visit(CSNode *p) { g_seq[g_n++] = p->data; }
static void ResetSeq() { g_n = 0; }

// ------------------------- 树的遍历（孩子兄弟结构上） -------------------------
// 先根遍历：先访问根，再依次遍历每一棵子树
void PreRoot(CSTree T) {
    if (!T) return;
    Visit(T);
    for (CSNode *p = T->firstChild; p; p = p->nextSibling) PreRoot(p);
}
// 后根遍历：先依次遍历每一棵子树，最后访问根
void PostRoot(CSTree T) {
    if (!T) return;
    for (CSNode *p = T->firstChild; p; p = p->nextSibling) PostRoot(p);
    Visit(T);
}

// --------------------- 同一结构按"二叉树"解释时的遍历 ---------------------
// 先序：根 -> 左(第一个孩子) -> 右(下一个兄弟)
void BiPreOrder(CSTree T) {
    if (!T) return;
    Visit(T);
    BiPreOrder(T->firstChild);
    BiPreOrder(T->nextSibling);
}
// 中序：左(第一个孩子) -> 根 -> 右(下一个兄弟)
void BiInOrder(CSTree T) {
    if (!T) return;
    BiInOrder(T->firstChild);
    Visit(T);
    BiInOrder(T->nextSibling);
}

// ------------------------------- 树的统计 -------------------------------
int TreeNodes(CSTree T) {
    if (!T) return 0;
    int n = 1;
    for (CSNode *p = T->firstChild; p; p = p->nextSibling) n += TreeNodes(p);
    return n;
}
// 叶子 = 没有孩子的结点（注意不是没有兄弟）
int TreeLeaves(CSTree T) {
    if (!T) return 0;
    int n = T->firstChild ? 0 : 1;
    for (CSNode *p = T->firstChild; p; p = p->nextSibling) n += TreeLeaves(p);
    return n;
}
// 树的深度 = 子树深度最大值 + 1
int TreeDepth(CSTree T) {
    if (!T) return 0;
    int best = 0;
    for (CSNode *p = T->firstChild; p; p = p->nextSibling) {
        int d = TreeDepth(p);
        if (d > best) best = d;
    }
    return best + 1;
}
// 求某个结点的度（孩子个数）
int DegreeOf(CSTree T, int x) {
    if (!T) return -1;
    if (T->data == x) {
        int d = 0;
        for (CSNode *p = T->firstChild; p; p = p->nextSibling) ++d;
        return d;
    }
    int r = DegreeOf(T->firstChild, x);
    if (r != -1) return r;
    return DegreeOf(T->nextSibling, x);
}

int main() {
    banner("Tree <-> Binary tree (child-sibling representation)");
    // 树：      1
    //        /   \ 
    //       2     3
    //      / \     \ 
    //     4   5     6
    CSNode *n4 = N(4), *n5 = N(5), *n6 = N(6);
    CSNode *n3 = N(3, n6);           // 3 的孩子是 6
    CSNode *n2 = N(2, n4);           // 2 的孩子是 4
    n4->nextSibling = n5;            // 5 是 4 的兄弟 -> 5 也是 2 的孩子
    n2->nextSibling = n3;            // 3 是 2 的兄弟 -> 3 也是 1 的孩子
    CSTree T = N(1, n2);             // 1 的孩子是 2

    ResetSeq(); PreRoot(T);
    int ePreRoot[] = {1, 2, 4, 5, 3, 6};
    expect_array("pre-root order", g_seq, g_n, ePreRoot, 6);

    ResetSeq(); PostRoot(T);
    int ePostRoot[] = {4, 5, 2, 6, 3, 1};
    expect_array("post-root order", g_seq, g_n, ePostRoot, 6);

    // 核心结论：树的先根 == 对应二叉树的先序；树的后根 == 对应二叉树的中序
    ResetSeq(); BiPreOrder(T);
    expect_array("binary preorder == pre-root order", g_seq, g_n, ePreRoot, 6);

    ResetSeq(); BiInOrder(T);
    expect_array("binary inorder == post-root order", g_seq, g_n, ePostRoot, 6);

    // 统计
    expect_eq("node count", TreeNodes(T), 6);
    expect_eq("leaf count", TreeLeaves(T), 3);          // 4,5,6
    expect_eq("depth", TreeDepth(T), 3);
    expect_eq("degree of 1", DegreeOf(T, 1), 2);
    expect_eq("degree of 2", DegreeOf(T, 2), 2);
    expect_eq("degree of 3", DegreeOf(T, 3), 1);
    expect_eq("degree of 4", DegreeOf(T, 4), 0);
    expect_eq("degree of a missing node", DegreeOf(T, 99), -1);

    // 森林：两棵独立的树，森林的"先序"就是依次对每棵树做先根遍历
    CSTree forestA = N(1, n2);       // n2->nextSibling 是 n3，所以这棵树里有两棵子树
    CSTree T2 = N(7, N(8));
    ResetSeq(); PreRoot(forestA); PreRoot(T2);
    int eForest[] = {1, 2, 4, 5, 3, 6, 7, 8};
    expect_array("forest preorder", g_seq, g_n, eForest, 8);
    expect_eq("forest node count", TreeNodes(forestA) + TreeNodes(T2), 8);

    FreeTree(T2);
    FreeTree(forestA);
    return finish();
}
