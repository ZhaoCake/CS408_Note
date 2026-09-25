// ============================================================================
// 第7章 查找 —— 平衡二叉树（AVL）【经典算法】
//   考点：
//     * 平衡条件：|左子树高 - 右子树高| <= 1（平衡因子只可能是 -1、0、1）
//     * 四种旋转：LL（右单旋）RR（左单旋）LR（先左后右）RL（先右后左）
//     * 判定旋转类型的口诀：看"新结点插在最小不平衡子树的哪一边"，
//       插入位置和失衡方向同侧是 LL/RR，异侧是 LR/RL
//     * 高度为 h 的 AVL 树最少结点数 N(h) = N(h-1) + N(h-2) + 1（N(1)=1, N(2)=2）
// ============================================================================
#include "../ds_common.h"

typedef struct AVLNode {
    int val, height;
    struct AVLNode *l, *r;
} AVLNode, *AVLTree;

int H(AVLNode *p) { return p ? p->height : 0; }
int BF(AVLNode *p) { return p ? H(p->l) - H(p->r) : 0; }
void Update(AVLNode *p) {
    int l = H(p->l), r = H(p->r);
    p->height = 1 + (l > r ? l : r);
}
AVLNode *NewNode(int v) {
    AVLNode *p = (AVLNode *)malloc(sizeof(AVLNode));
    p->val = v; p->height = 1; p->l = p->r = NULL;
    return p;
}
void FreeTree(AVLTree T) { if (!T) return; FreeTree(T->l); FreeTree(T->r); free(T); }

// ---------------- 四种旋转 ----------------
// LL：在左孩子的左子树插入 -> 右单旋
AVLNode *RotateLL(AVLNode *p) {
    AVLNode *q = p->l;
    p->l = q->r;
    q->r = p;
    Update(p); Update(q);          // 先更新下面的 p，再更新新根 q
    printf("    LL right-rotate at %d (new subtree root %d)\n", p->val, q->val);
    return q;
}
// RR：在右孩子的右子树插入 -> 左单旋
AVLNode *RotateRR(AVLNode *p) {
    AVLNode *q = p->r;
    p->r = q->l;
    q->l = p;
    Update(p); Update(q);
    printf("    RR left-rotate at %d (new subtree root %d)\n", p->val, q->val);
    return q;
}
// LR：在左孩子的右子树插入 -> 先对左孩子左旋，再整体右旋
AVLNode *RotateLR(AVLNode *p) {
    printf("    LR: first left-rotate on the left child, then right-rotate at %d\n", p->val);
    p->l = RotateRR(p->l);
    return RotateLL(p);
}
// RL：在右孩子的左子树插入 -> 先对右孩子右旋，再整体左旋
AVLNode *RotateRL(AVLNode *p) {
    printf("    RL: first right-rotate on the right child, then left-rotate at %d\n", p->val);
    p->r = RotateLL(p->r);
    return RotateRR(p);
}

AVLNode *AVLInsert(AVLNode *p, int v) {
    if (!p) return NewNode(v);
    if (v < p->val)      p->l = AVLInsert(p->l, v);
    else if (v > p->val) p->r = AVLInsert(p->r, v);
    else return p;                                  // 重复值不插入

    Update(p);
    int bf = BF(p);
    if (bf > 1 && v < p->l->val)  return RotateLL(p);   // LL
    if (bf < -1 && v > p->r->val) return RotateRR(p);   // RR
    if (bf > 1 && v > p->l->val)  return RotateLR(p);   // LR
    if (bf < -1 && v < p->r->val) return RotateRL(p);   // RL
    return p;
}

// ---------------- 自检与统计 ----------------
bool IsAVL(AVLNode *p) {
    if (!p) return true;
    if (BF(p) > 1 || BF(p) < -1) return false;
    return IsAVL(p->l) && IsAVL(p->r);
}
int HeightOf(AVLNode *p) { return p ? p->height : 0; }
int CountNodes(AVLNode *p) { return p ? 1 + CountNodes(p->l) + CountNodes(p->r) : 0; }
static int g_seq[64], g_n;
static void InOrder(AVLNode *p) {
    if (!p) return;
    InOrder(p->l); g_seq[g_n++] = p->val; InOrder(p->r);
}
bool IsSortedAsc(const int a[], int n) { for (int i = 1; i < n; ++i) if (a[i] <= a[i - 1]) return false; return true; }

// 高度为 h 的 AVL 树最少结点数：N(h) = N(h-1) + N(h-2) + 1
int MinNodesAVL(int h) {
    if (h == 0) return 0;
    if (h == 1) return 1;
    return MinNodesAVL(h - 1) + MinNodesAVL(h - 2) + 1;
}

// 求某个值的层次（找不到返回 0）
int LevelOf(AVLNode *p, int v, int depth) {
    if (!p) return 0;
    if (p->val == v) return depth;
    int t = LevelOf(p->l, v, depth + 1);
    return t ? t : LevelOf(p->r, v, depth + 1);
}

int main() {
    banner("AVL tree : rotations and insertion");

    // ---- 四种旋转各来一次，最终根都应该是 2 ----
    const char *names[4] = {"LL by inserting 1,2,3", "RR by inserting 3,2,1",
                            "LR by inserting 3,1,2", "RL by inserting 1,3,2"};
    int seqs[4][3] = {{1, 2, 3}, {3, 2, 1}, {3, 1, 2}, {1, 3, 2}};
    for (int k = 0; k < 4; ++k) {
        printf("  --- %s ---\n", names[k]);
        AVLNode *t = NULL;
        for (int i = 0; i < 3; ++i) t = AVLInsert(t, seqs[k][i]);
        expect_eq("root after rebalancing is 2", t->val, 2);
        expect_eq("height is 2", HeightOf(t), 2);
        expect_true("still a valid AVL", IsAVL(t));
        g_n = 0; InOrder(t);
        int e[] = {1, 2, 3};
        expect_array("inorder stays sorted", g_seq, g_n, e, 3);
        FreeTree(t);
    }

    // ---- 连续插入 1..5，最后必须是 2(1, 4(3,5)) ----
    printf("  --- insert 1,2,3,4,5 one by one ---\n");
    AVLNode *t = NULL;
    for (int i = 1; i <= 5; ++i) {
        t = AVLInsert(t, i);
        expect_true("valid AVL after each insertion", IsAVL(t));
    }
    expect_eq("root", t->val, 2);
    expect_eq("height stays 3 (instead of 5 without balancing)", HeightOf(t), 3);
    expect_eq("left child", t->l->val, 1);
    expect_eq("right subtree root", t->r->val, 4);
    FreeTree(t);

    // ---- 王道经典插入序列 ----
    //  1,2,3,4,5,6,7,8 连续插入后树高只有 4（不平衡的话是 8）
    printf("  --- insert 1..8 ---\n");
    t = NULL;
    int a8[] = {1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 8; ++i) t = AVLInsert(t, a8[i]);
    expect_true("valid AVL", IsAVL(t));
    expect_eq("height for n = 8", HeightOf(t), 4);
    expect_eq("node count", CountNodes(t), 8);
    g_n = 0; InOrder(t);
    expect_true("inorder sorted", IsSortedAsc(g_seq, g_n));
    FreeTree(t);

    // ---- 高度与最少结点数：N(h) = N(h-1) + N(h-2) + 1 ----
    int expectMin[] = {0, 1, 2, 4, 7, 12, 20, 33};
    for (int h = 0; h <= 7; ++h) {
        char label[64];
        snprintf(label, sizeof(label), "min nodes for height %d", h);
        expect_eq(label, MinNodesAVL(h), expectMin[h]);
    }
    // 结论：12 个结点最多只能撑到高度 5；想让高度到 6 至少需要 20 个结点
    printf("  min nodes: h=3 -> 4, h=4 -> 7, h=5 -> 12, h=6 -> 20, h=7 -> 33\n");
    expect_true("height is O(log n): 33 nodes can reach height 7", MinNodesAVL(7) == 33);

    // ---- 随机顺序插入，逐个检查平衡性 ----
    int nums[] = {50, 30, 70, 10, 40, 60, 80, 5, 15, 35, 45, 55, 65, 75, 85, 1};
    t = NULL;
    int okAll = 1;
    for (int i = 0; i < 16; ++i) {
        t = AVLInsert(t, nums[i]);
        if (!IsAVL(t)) okAll = 0;
    }
    expect_true("balanced after 16 insertions", okAll != 0);
    // 高度为 5 至少要 12 个结点、高度为 6 至少要 20 个结点，所以 16 个结点的高度只能是 4 或 5
    expect_true("height is 4 or 5 for 16 nodes", HeightOf(t) == 4 || HeightOf(t) == 5);
    expect_eq("node count", CountNodes(t), 16);
    printf("  levels: 1->%d  5->%d  45->%d  85->%d  99->%d\n",
           LevelOf(t, 1, 1), LevelOf(t, 5, 1), LevelOf(t, 45, 1), LevelOf(t, 85, 1), LevelOf(t, 99, 1));
    expect_true("max level <= height", LevelOf(t, 1, 1) <= HeightOf(t));

    // 重复值不插入
    int before = CountNodes(t);
    t = AVLInsert(t, 50);
    expect_eq("duplicate ignored", CountNodes(t), before);
    FreeTree(t);
    return finish();
}
