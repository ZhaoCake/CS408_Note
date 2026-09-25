// ============================================================================
// 第7章 查找 —— B 树【经典算法】
//   以 4 阶 B 树为例（每个结点最多 3 个关键字、4 个孩子；非根结点至少 1 个关键字、2 个孩子）
//   索引约定（1 基，和考研手算一致）：
//     关键字 key[1..n]，孩子 child[0..n]，child[i-1] 和 child[i] 之间夹着 key[i]
//   考点：
//     * m 阶 B 树：关键字个数 ceil(m/2)-1 <= n <= m-1；孩子数 = 关键字数 + 1
//     * 所有叶子都在同一层（B 树"绝对平衡"）
//     * 插入：插到最底层，满了就分裂——中间关键字上移到双亲，左右分成两半
//     * 删除：① 直接删 ② 向兄弟"借" ③ 兄弟也不够就"合并"（合并可能连锁）
// ============================================================================
#include "../ds_common.h"

#define T 2                          // 最小度：2T = 4 阶
#define MAXK (2 * T - 1)             // 最多 3 个关键字
#define MAXC (2 * T)                 // 最多 4 个孩子
#define MINK (T - 1)                 // 非根结点最少 1 个关键字

typedef struct BTNode {
    int key[MAXK + 2];                          // key[1..n]
    struct BTNode *child[MAXC + 2];             // child[0..n]
    int n;
    bool leaf;
} BTNode, *BTree;

BTNode *NewNode(bool leaf) {
    BTNode *p = (BTNode *)malloc(sizeof(BTNode));
    for (int i = 0; i < MAXK + 2; ++i) p->key[i] = 0;
    for (int i = 0; i < MAXC + 2; ++i) p->child[i] = NULL;
    p->n = 0;
    p->leaf = leaf;
    return p;
}
void FreeTree(BTNode *x) {
    if (!x) return;
    if (!x->leaf) for (int i = 0; i <= x->n; ++i) FreeTree(x->child[i]);
    free(x);
}

// ===========================================================================
// 查找
//   关键：走的孩子下标必须落在 [0, n] 内。写成 idx <= n 的循环会在
//   "k 比所有关键字都大"时得到 n+1，而 child[n+1] 是没初始化过的指针
// ===========================================================================
int ChildIndex(BTNode *x, int k) {               // 返回该走哪个孩子，范围 [0, n]
    int idx = 1;
    while (idx < x->n && k > x->key[idx]) ++idx;
    return (k < x->key[idx]) ? idx - 1 : idx;
}

bool BTSearch(BTree x, int k, int &idxOut) {
    if (!x) return false;
    int idx = 1;
    while (idx <= x->n && x->key[idx] < k) ++idx;    // 第一个 >= k 的位置
    if (idx <= x->n && x->key[idx] == k) { idxOut = idx; return true; }
    if (x->leaf) return false;
    return BTSearch(x->child[ChildIndex(x, k)], k, idxOut);
}

// ===========================================================================
// 分裂：把已满的 child[i] 从中间切开，中间关键字上移到 x 的 key[i+1]
// ===========================================================================
void SplitChild(BTNode *x, int i) {
    BTNode *y = x->child[i];
    BTNode *z = NewNode(y->leaf);
    z->n = y->n - T;
    for (int j = 1; j <= z->n; ++j) z->key[j] = y->key[T + j];
    if (!y->leaf) for (int j = 0; j <= z->n; ++j) z->child[j] = y->child[T + j];
    y->n = T - 1;

    for (int j = x->n; j >= i + 1; --j) x->child[j + 1] = x->child[j];
    x->child[i + 1] = z;
    for (int j = x->n; j >= i + 1; --j) x->key[j + 1] = x->key[j];
    x->key[i + 1] = y->key[T];
    x->n++;
    printf("    split child %d: key %d moves up (left n=%d, right n=%d)\n", i, y->key[T], y->n, z->n);
}

// ===========================================================================
// 插入
// ===========================================================================
void InsertNonFull(BTNode *x, int k) {
    int i = x->n;
    if (x->leaf) {
        while (i >= 1 && k < x->key[i]) { x->key[i + 1] = x->key[i]; --i; }
        x->key[i + 1] = k;
        x->n++;
        return;
    }
    // i 退到"最后一个小于 k 的关键字"，此时该走 child[i]
    while (i >= 1 && k < x->key[i]) --i;
    if (x->child[i]->n == MAXK) {                 // 往下走之前先把满孩子裂开
        SplitChild(x, i);
        if (k > x->key[i + 1]) ++i;               // 中间关键字上移到了 key[i+1]
    }
    InsertNonFull(x->child[i], k);
}

void Insert(BTree &root, int k) {
    if (!root) {
        root = NewNode(true);
        root->key[1] = k;
        root->n = 1;
        return;
    }
    int idx = 0;
    if (BTSearch(root, k, idx)) return;           // 重复值不插
    if (root->n == MAXK) {                        // 根满了 -> 整棵树长高一层
        BTNode *s = NewNode(false);
        s->child[0] = root;
        root = s;
        SplitChild(s, 0);
        InsertNonFull(s, k);
    } else {
        InsertNonFull(root, k);
    }
}

// ===========================================================================
// 删除
// ===========================================================================
int GetPred(BTNode *x, int i) {                   // child[i] 子树里的最大值
    BTNode *p = x->child[i];
    while (!p->leaf) p = p->child[p->n];
    return p->key[p->n];
}
int GetSucc(BTNode *x, int i) {                   // child[i] 子树里的最小值
    BTNode *p = x->child[i];
    while (!p->leaf) p = p->child[0];
    return p->key[1];
}

void RemoveFromLeaf(BTNode *x, int idx) {
    for (int i = idx + 1; i <= x->n; ++i) x->key[i - 1] = x->key[i];
    x->n--;
    printf("    removed from a leaf\n");
}

// 向前一个兄弟借：把 key[idx] 拉下来，兄弟的最大关键字顶上去
void BorrowFromPrev(BTNode *x, int idx) {
    BTNode *c = x->child[idx], *s = x->child[idx - 1];
    for (int i = c->n; i >= 1; --i) c->key[i + 1] = c->key[i];
    if (!c->leaf) for (int i = c->n; i >= 0; --i) c->child[i + 1] = c->child[i];
    c->key[1] = x->key[idx];
    if (!c->leaf) c->child[0] = s->child[s->n];
    x->key[idx] = s->key[s->n];
    c->n++;
    s->n--;
    printf("    borrow from the previous sibling\n");
}

// 向后一个兄弟借：分隔关键字是 key[idx+1]
void BorrowFromNext(BTNode *x, int idx) {
    BTNode *c = x->child[idx], *s = x->child[idx + 1];
    c->key[c->n + 1] = x->key[idx + 1];
    if (!c->leaf) c->child[c->n + 1] = s->child[0];
    x->key[idx + 1] = s->key[1];
    for (int i = 1; i < s->n; ++i) s->key[i] = s->key[i + 1];
    if (!s->leaf) for (int i = 0; i < s->n; ++i) s->child[i] = s->child[i + 1];
    c->n++;
    s->n--;
    printf("    borrow from the next sibling\n");
}

// 合并 child[sepIdx-1] + key[sepIdx] + child[sepIdx]，结果放在 child[sepIdx-1]
void Merge(BTNode *x, int sepIdx) {
    BTNode *c = x->child[sepIdx - 1], *s = x->child[sepIdx];
    c->key[T] = x->key[sepIdx];
    for (int i = 1; i <= s->n; ++i) c->key[T + i] = s->key[i];
    if (!c->leaf) for (int i = 0; i <= s->n; ++i) c->child[T + i] = s->child[i];
    c->n += s->n + 1;
    for (int i = sepIdx; i < x->n; ++i) x->key[i] = x->key[i + 1];
    for (int i = sepIdx + 1; i <= x->n; ++i) x->child[i - 1] = x->child[i];
    x->n--;
    free(s);
    printf("    merged two children (key %d pulled down), left n=%d\n", c->key[T], c->n);
}

// 保证 child[idx] 至少有 T 个关键字：借不到就合并
void Fill(BTNode *x, int idx) {
    if (idx > 0 && x->child[idx - 1]->n >= T)          BorrowFromPrev(x, idx);
    else if (idx < x->n && x->child[idx + 1]->n >= T)  BorrowFromNext(x, idx);
    else if (idx < x->n)                               Merge(x, idx + 1);
    else                                               Merge(x, idx);
}

void RemoveFromNode(BTNode *x, int k);

// 要删的关键字在内部结点里：用前驱/后继替换，再递归删掉那个前驱/后继
void RemoveFromInternal(BTNode *x, int idx) {
    int k = x->key[idx];
    if (x->child[idx - 1]->n >= T) {                 // 左孩子够胖 -> 用前驱顶替
        int pred = GetPred(x, idx - 1);
        printf("    replace %d by its predecessor %d\n", k, pred);
        x->key[idx] = pred;
        RemoveFromNode(x->child[idx - 1], pred);
    } else if (x->child[idx]->n >= T) {              // 右孩子够胖 -> 用后继顶替
        int succ = GetSucc(x, idx);
        printf("    replace %d by its successor %d\n", k, succ);
        x->key[idx] = succ;
        RemoveFromNode(x->child[idx], succ);
    } else {                                         // 都不够 -> 合并后递归删
        Merge(x, idx);
        RemoveFromNode(x->child[idx - 1], k);
    }
}

void RemoveFromNode(BTNode *x, int k) {
    int idx = 1;
    while (idx <= x->n && x->key[idx] < k) ++idx;    // 第一个 >= k 的位置
    if (idx <= x->n && x->key[idx] == k) {           // 在当前结点里找到
        if (x->leaf) RemoveFromLeaf(x, idx);
        else         RemoveFromInternal(x, idx);
        return;
    }
    if (x->leaf) { printf("    key %d not found\n", k); return; }

    int childIdx = ChildIndex(x, k);                 // 该走哪个孩子
    int oldN = x->n;
    if (x->child[childIdx]->n < T) Fill(x, childIdx);
    // Fill 有可能把 child[childIdx] 合并到前一个孩子里去，此时要换个孩子下探
    if (childIdx == oldN && x->n < oldN) childIdx = oldN - 1;
    RemoveFromNode(x->child[childIdx], k);
}

void RemoveKey(BTree &root, int k) {
    if (!root) return;
    RemoveFromNode(root, k);
    if (root->n == 0) {                              // 根空了 -> 树矮一层
        BTNode *old = root;
        root = root->leaf ? NULL : root->child[0];
        free(old);
    }
}

// ===========================================================================
// 自检与打印
// ===========================================================================
bool CheckBTree(BTNode *x, bool isRoot, int depth, int &leafDepth, int lo, int hi) {
    if (!x) return false;
    if (x->n > MAXK) return false;
    if (isRoot) { if (x->n < 1) return false; }
    else        { if (x->n < MINK) return false; }
    for (int i = 1; i < x->n; ++i) if (x->key[i] >= x->key[i + 1]) return false;
    if (x->key[1] <= lo || x->key[x->n] >= hi) return false;
    if (x->leaf) {
        if (leafDepth == -1) leafDepth = depth;
        else if (leafDepth != depth) return false;   // 所有叶子必须同层
        return true;
    }
    for (int i = 0; i <= x->n; ++i) {
        int l = (i == 0) ? lo : x->key[i];
        int h = (i == x->n) ? hi : x->key[i + 1];
        if (!CheckBTree(x->child[i], false, depth + 1, leafDepth, l, h)) return false;
    }
    return true;
}
bool Validate(BTree root) {
    if (!root) return true;
    int leafDepth = -1;
    return CheckBTree(root, true, 0, leafDepth, -100000, 100000);
}
void PrintBTree(BTNode *x, int depth) {
    if (!x) return;
    printf("  %*s[", depth * 4, "");
    for (int i = 1; i <= x->n; ++i) printf("%d%s", x->key[i], i < x->n ? " " : "");
    printf("]\n");
    if (!x->leaf) for (int i = 0; i <= x->n; ++i) PrintBTree(x->child[i], depth + 1);
}
int TreeHeight(BTNode *x) {
    if (!x) return 0;
    if (x->leaf) return 1;
    return 1 + TreeHeight(x->child[0]);
}
int CountKeys(BTNode *x) {
    if (!x) return 0;
    int n = x->n;
    if (!x->leaf) for (int i = 0; i <= x->n; ++i) n += CountKeys(x->child[i]);
    return n;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);        // 关掉缓冲，出问题时也能看到打印到哪一步
    banner("B-tree (order 4) : search / insert+split / delete+borrow+merge");

    // ---- 逐个插入 1..10，每步都检查 B 树性质 ----
    printf("  --- insert 1..10 ---\n");
    BTree root = NULL;
    int okAll = 1;
    for (int i = 1; i <= 10; ++i) {
        printf("  insert %d:\n", i);
        Insert(root, i);
        if (!Validate(root)) { okAll = 0; printf("    ! invalid after inserting %d\n", i); }
    }
    expect_true("valid after every insertion", okAll != 0);
    PrintBTree(root, 0);
    expect_eq("total keys", CountKeys(root), 10);

    int idx = 0;
    int foundAll = 1;
    for (int i = 1; i <= 10; ++i) if (!BTSearch(root, i, idx)) foundAll = 0;
    expect_true("all inserted keys are found", foundAll != 0);
    expect_true("a missing key is not found", !BTSearch(root, 99, idx));
    expect_true("inserting a duplicate changes nothing", (Insert(root, 5), CountKeys(root) == 10));
    printf("  height = %d\n", TreeHeight(root));
    expect_true("height stays small", TreeHeight(root) <= 3);

    // ---- 删除：叶子 / 内部结点 / 借位 / 合并都会遇到 ----
    int toDelete[] = {5, 1, 7, 2, 3, 4, 6, 8, 9, 10};
    int okDel = 1;
    for (int i = 0; i < 10; ++i) {
        printf("  --- delete %d ---\n", toDelete[i]);
        RemoveKey(root, toDelete[i]);
        if (!Validate(root)) { okDel = 0; printf("    ! invalid after deleting %d\n", toDelete[i]); }
    }
    expect_true("valid after deleting everything", okDel != 0);
    expect_true("tree becomes empty", root == NULL);
    expect_eq("no keys left", CountKeys(root), 0);

    // ---- 反向删除：考验另一个方向的借位 ----
    printf("  --- insert 1..12, then delete from large to small ---\n");
    for (int i = 1; i <= 12; ++i) Insert(root, i);
    expect_true("valid after inserting 1..12", Validate(root));
    expect_eq("keys after insert", CountKeys(root), 12);
    int okDel2 = 1;
    for (int i = 12; i >= 1; --i) {
        RemoveKey(root, i);
        if (!Validate(root)) { okDel2 = 0; printf("    ! invalid after deleting %d\n", i); }
    }
    expect_true("valid after deleting all 12", okDel2 != 0);
    expect_true("empty again", root == NULL);

    // ---- 大量随机顺序插入 + 删除，反复检查性质 ----
    printf("  --- 30 keys inserted in a scrambled order, then all deleted ---\n");
    int order[] = {15, 3, 22, 8, 19, 27, 1, 11, 24, 6, 17, 30, 13, 4, 21, 9, 26, 2,
                   14, 7, 20, 29, 5, 12, 25, 10, 18, 28, 16, 23};
    int ok3 = 1;
    for (int i = 0; i < 30; ++i) {
        Insert(root, order[i]);
        if (!Validate(root)) { ok3 = 0; printf("    ! invalid after inserting %d\n", order[i]); }
    }
    expect_true("valid after 30 scrambled insertions", ok3 != 0);
    expect_eq("all 30 keys are in the tree", CountKeys(root), 30);
    int allFound = 1;
    for (int i = 1; i <= 30; ++i) if (!BTSearch(root, i, idx)) allFound = 0;
    expect_true("every key is searchable", allFound != 0);
    printf("  height with 30 keys = %d\n", TreeHeight(root));

    int ok4 = 1;
    for (int i = 0; i < 30; ++i) {
        RemoveKey(root, order[i]);
        if (!Validate(root)) { ok4 = 0; printf("    ! invalid after deleting %d\n", order[i]); }
    }
    expect_true("valid after deleting all 30", ok4 != 0);
    expect_true("empty at the end", root == NULL);

    // ---- 删除不存在的关键字不应该破坏树 ----
    for (int i = 1; i <= 5; ++i) Insert(root, i);
    RemoveKey(root, 99);
    expect_true("deleting a missing key keeps the tree valid", Validate(root));
    expect_eq("key count unchanged", CountKeys(root), 5);

    FreeTree(root);
    return finish();
}
