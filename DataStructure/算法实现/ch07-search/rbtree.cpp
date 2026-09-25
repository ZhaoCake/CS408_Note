// ============================================================================
// 第7章 查找 —— 红黑树【经典算法】
//   五条性质：
//     1) 每个结点是红色或黑色
//     2) 根是黑色
//     3) 每个叶子（NULL 空结点）是黑色
//     4) 不存在两个相邻的红结点（红结点的父、子都必须是黑）
//     5) 从任一结点到其所有叶子的路径上，黑结点个数相同（黑高相等）
//   红黑树的本质：用颜色约束把树高控制在 O(log n)，最长路径不超过最短路径的两倍
//   插入调整只看叔叔的颜色：①叔叔红 -> 变色 + 上溯 ②叔叔黑 -> 旋转 + 变色（LL/RR/LR/RL）
// ============================================================================
#include "../ds_common.h"

typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int val;
    Color color;
    struct RBNode *l, *r, *p;              // 带父指针，调整时方便找叔叔
} RBNode, *RBTree;

RBNode *NewNode(int v) {
    RBNode *z = (RBNode *)malloc(sizeof(RBNode));
    z->val = v; z->color = RED; z->l = z->r = z->p = NULL;
    return z;
}
void FreeTree(RBNode *x) { if (!x) return; FreeTree(x->l); FreeTree(x->r); free(x); }
Color ColorOf(RBNode *x) { return x ? x->color : BLACK; }        // 空结点算黑色

// ------------------------------- 旋转（要维护父指针） -------------------------------
void RotateLeft(RBTree &root, RBNode *x) {
    RBNode *y = x->r;
    x->r = y->l;
    if (y->l) y->l->p = x;
    y->p = x->p;
    if (!x->p)                 root = y;
    else if (x == x->p->l)     x->p->l = y;
    else                       x->p->r = y;
    y->l = x;
    x->p = y;
}

void RotateRight(RBTree &root, RBNode *x) {
    RBNode *y = x->l;
    x->l = y->r;
    if (y->r) y->r->p = x;
    y->p = x->p;
    if (!x->p)                 root = y;
    else if (x == x->p->r)     x->p->r = y;
    else                       x->p->l = y;
    y->r = x;
    x->p = y;
}

// ------------------------------- 插入调整 -------------------------------
void InsertFixup(RBTree &root, RBNode *z) {
    while (z->p && z->p->color == RED) {              // 只有"父红"才需要调整
        RBNode *parent = z->p, *grand = parent->p;
        if (parent == grand->l) {                     // 父是左孩子
            RBNode *uncle = grand->r;
            if (ColorOf(uncle) == RED) {              // 情况 1：叔叔红 -> 变色 + 上溯
                parent->color = BLACK;
                uncle->color = BLACK;
                grand->color = RED;
                z = grand;
            } else {
                if (z == parent->r) {                 // 情况 2：LR -> 先左旋转成 LL
                    z = parent;
                    RotateLeft(root, z);
                    parent = z->p;
                }
                parent->color = BLACK;                // 情况 3：LL -> 变色 + 右旋
                grand->color = RED;
                RotateRight(root, grand);
            }
        } else {                                      // 父是右孩子（完全对称）
            RBNode *uncle = grand->l;
            if (ColorOf(uncle) == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grand->color = RED;
                z = grand;
            } else {
                if (z == parent->l) {
                    z = parent;
                    RotateRight(root, z);
                    parent = z->p;
                }
                parent->color = BLACK;
                grand->color = RED;
                RotateLeft(root, grand);
            }
        }
    }
    root->color = BLACK;                              // 性质 2：根永远染黑
}

void RBInsert(RBTree &root, int v) {
    RBNode *z = NewNode(v), *y = NULL, *x = root;
    while (x) {                                       // 先按 BST 规则找到插入位置
        y = x;
        if (v == x->val) { free(z); return; }         // 重复值不插入（必须在下降时判断！）
        x = (v < x->val) ? x->l : x->r;
    }
    z->p = y;
    if (!y)              root = z;
    else if (v < y->val) y->l = z;
    else                 y->r = z;
    InsertFixup(root, z);
}

// ------------------------------- 性质自检 -------------------------------
// 返回黑高；发现违规返回 -1
static int CheckRBRec(RBNode *x, RBNode *parent) {
    if (!x) return 1;                                 // 空结点是黑的，黑高贡献 1
    if (x->color == RED && ColorOf(parent) == RED) {
        printf("    violation: two adjacent red nodes (%d)\n", x->val);
        return -1;
    }
    int lb = CheckRBRec(x->l, x);
    if (lb == -1) return -1;
    int rb = CheckRBRec(x->r, x);
    if (rb == -1) return -1;
    if (lb != rb) {                                   // 黑高必须相等
        printf("    violation: black height mismatch at %d (%d vs %d)\n", x->val, lb, rb);
        return -1;
    }
    return lb + (x->color == BLACK ? 1 : 0);
}

bool CheckRB(RBTree root) {
    if (!root) return true;
    if (root->color != BLACK) { printf("    violation: root is not black\n"); return false; }
    if (root->p != NULL)      { printf("    violation: root has a parent\n"); return false; }
    return CheckRBRec(root, NULL) != -1;
}

// 中序检查 BST 性质，并统计
static int g_seq[64], g_n;
static void InOrder(RBNode *x) {
    if (!x) return;
    InOrder(x->l); g_seq[g_n++] = x->val; InOrder(x->r);
}
int BlackHeight(RBNode *root) {
    int h = 0;
    for (RBNode *x = root; x; x = x->l) if (x->color == BLACK) ++h;
    return h;
}
int Height(RBNode *x) { if (!x) return 0; int a = Height(x->l), b = Height(x->r); return 1 + (a > b ? a : b); }
int CountRed(RBNode *x) { return x ? (x->color == RED) + CountRed(x->l) + CountRed(x->r) : 0; }
int CountNodes(RBNode *x) { return x ? 1 + CountNodes(x->l) + CountNodes(x->r) : 0; }
void PrintInOrderColors(RBNode *x) {
    if (!x) return;
    PrintInOrderColors(x->l);
    printf(" %d%c", x->val, x->color == RED ? 'R' : 'B');
    PrintInOrderColors(x->r);
}

int main() {
    banner("Red-black tree : insertion and property check");

    // ---- 逐个插入 1..10，每一步都检查五条性质 ----
    RBTree T = NULL;
    int okAll = 1;
    for (int i = 1; i <= 10; ++i) {
        RBInsert(T, i);
        if (!CheckRB(T)) { okAll = 0; printf("  broken after inserting %d\n", i); }
    }
    expect_true("all properties hold after every insertion of 1..10", okAll != 0);
    expect_eq("root is black", T->color == BLACK, 1);
    printf("  inorder with colors:");
    PrintInOrderColors(T);
    printf("\n");

    g_n = 0; InOrder(T);
    int eIn[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    expect_array("inorder == sorted sequence (BST property)", g_seq, g_n, eIn, 10);

    // ---- 关键性质：最长路径 <= 2 * 最短路径 ----
    int h = Height(T), bh = BlackHeight(T);
    printf("  height = %d, black height = %d, red nodes = %d\n", h, bh, CountRed(T));
    // 核心结论：最长路径 <= 2 * 最短路径，也就是 height <= 2 * 黑高
    expect_true("longest path <= 2 * shortest path", h <= 2 * bh);
    expect_true("height is O(log n)", h <= 2 * (bh + 1));
    expect_eq("root is black", T->color, BLACK);

    // ---- 换个插入顺序，性质依然成立 ----
    RBTree T2 = NULL;
    int a[] = {50, 30, 70, 10, 40, 60, 80, 5, 15, 35, 45, 55, 65, 75, 85};
    int ok2 = 1;
    for (int i = 0; i < 15; ++i) {
        RBInsert(T2, a[i]);
        if (!CheckRB(T2)) ok2 = 0;
    }
    expect_true("random-ish order also keeps the properties", ok2 != 0);
    g_n = 0; InOrder(T2);
    int eIn2[] = {5, 10, 15, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85};
    expect_array("inorder of T2", g_seq, g_n, eIn2, 15);
    printf("  T2: height = %d, black height = %d\n", Height(T2), BlackHeight(T2));
    expect_true("T2 is balanced-ish (height <= 2 * bh)", Height(T2) <= 2 * BlackHeight(T2));

    // ---- 重复值不插入 ----
    int before = CountNodes(T2);
    RBInsert(T2, 50);
    expect_eq("duplicate ignored", CountNodes(T2), before);

    // ---- 空树：根是黑的（性质 2 的边界） ----
    RBTree empty = NULL;
    expect_true("empty tree satisfies the properties", CheckRB(empty));
    RBInsert(empty, 42);
    expect_eq("single node is black", empty->color == BLACK, 1);
    expect_true("single node is valid", CheckRB(empty));

    FreeTree(T); FreeTree(T2); FreeTree(empty);
    return finish();
}
