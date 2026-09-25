// ============================================================================
// 第5章 树与二叉树 —— 并查集
//   考点：
//     * 用"双亲表示法"的树/森林表示集合，查某个元素属于哪个集合就是"一路找根"
//     * 两个优化：路径压缩（Find 时把路径上所有点直接挂到根）+ 按秩合并（小树并到大树）
//     * 只做"合并 + 查询"的话，均摊时间近似 O(alpha(n))，比 O(log n) 还快
//     * 典型应用：判无向图的连通性、Kruskal 求最小生成树时判环、等价类划分
// ============================================================================
#include "../ds_common.h"

#define UFMAX 32

typedef struct {
    int parent[UFMAX];       // parent[i] == i 表示 i 是根
    int rank[UFMAX];         // 树高的上界，用于按秩合并
    int n;
} UFSet;

void UFInit(UFSet &S, int n) {
    S.n = n;
    for (int i = 1; i <= n; ++i) { S.parent[i] = i; S.rank[i] = 0; }
}

// 查找 + 路径压缩（迭代写法，避免递归深度问题）
int UFFind(UFSet &S, int x) {
    int root = x;
    while (S.parent[root] != root) root = S.parent[root];   // 1) 先找到根
    while (S.parent[x] != root) {                            // 2) 把路径上所有点直接挂到根
        int next = S.parent[x];
        S.parent[x] = root;
        x = next;
    }
    return root;
}

// 按秩合并：把矮树挂到高树下面，避免树越长越高
void UFUnion(UFSet &S, int a, int b) {
    int ra = UFFind(S, a), rb = UFFind(S, b);
    if (ra == rb) return;
    if (S.rank[ra] < S.rank[rb])      S.parent[ra] = rb;
    else if (S.rank[ra] > S.rank[rb]) S.parent[rb] = ra;
    else { S.parent[rb] = ra; ++S.rank[ra]; }                // 等高时才让树长高一层
}

bool UFSame(UFSet &S, int a, int b) { return UFFind(S, a) == UFFind(S, b); }

// 集合个数 = 根结点的个数
int UFCount(UFSet &S) {
    int c = 0;
    for (int i = 1; i <= S.n; ++i)
        if (UFFind(S, i) == i) ++c;
    return c;
}

// 树的高度（用来观察按秩合并的效果）
int UFHeight(UFSet &S, int x) {
    int h = 0;
    for (int p = x; S.parent[p] != p; p = S.parent[p]) ++h;
    return h;
}

// 应用：用并查集判断无向图的连通性（边用 (u,v) 数组给出）
bool IsConnected(int n, const int eu[], const int ev[], int m) {
    UFSet S;
    UFInit(S, n);
    for (int i = 0; i < m; ++i) UFUnion(S, eu[i], ev[i]);
    return UFCount(S) == 1;
}

int main() {
    banner("Union-Find Set");
    UFSet S;
    UFInit(S, 6);
    expect_eq("initial set count", UFCount(S), 6);
    expect_true("1 and 2 are separate", !UFSame(S, 1, 2));

    UFUnion(S, 1, 2);
    UFUnion(S, 2, 3);
    expect_true("1 and 3 are connected", UFSame(S, 1, 3));
    expect_eq("set count drops to 4", UFCount(S), 4);
    expect_eq("all three share one root", UFFind(S, 1) == UFFind(S, 3), 1);

    // 路径压缩：Find 之后，路径上的点都直接指向根
    UFInit(S, 6);
    S.parent[2] = 1; S.parent[3] = 2; S.parent[4] = 3;     // 人为造一条长链 4->3->2->1
    expect_eq("height before compression", UFHeight(S, 4), 3);
    int root = UFFind(S, 4);
    expect_eq("root", root, 1);
    expect_eq("height after compression", UFHeight(S, 4), 1);
    expect_eq("every node on the path points to the root", S.parent[2] == root && S.parent[3] == root, 1);

    // 按秩合并：不会把高树挂到矮树下面
    UFInit(S, 8);
    UFUnion(S, 1, 2);        // rank 提升
    UFUnion(S, 3, 4);
    UFUnion(S, 1, 3);        // 等高时合并，roots rank +1
    UFUnion(S, 5, 6);
    UFUnion(S, 1, 5);        // 矮的挂到高的下面
    expect_true("tree stays shallow (height <= 2)", UFHeight(S, 5) <= 2);
    expect_eq("set count", UFCount(S), 3);                 // {1..6}, {7}, {8}

    // 合并两个已经在同一个集合里的元素：什么都不做
    int before = UFCount(S);
    UFUnion(S, 1, 6);
    UFUnion(S, 1, 6);
    expect_eq("union within a set is a no-op", UFCount(S), before);

    // 应用：连通性判定
    //  1-2, 2-3, 3-4 -> 连通
    int eu[] = {1, 2, 3};
    int ev[] = {2, 3, 4};
    expect_true("4 nodes 3 edges form one component", IsConnected(4, eu, ev, 3));
    //  再加一个孤立点 5 -> 不连通
    expect_true("adding an isolated node breaks connectivity", !IsConnected(5, eu, ev, 3));
    //  补上 4-5 后又连通
    int eu2[] = {1, 2, 3, 4};
    int ev2[] = {2, 3, 4, 5};
    expect_true("connected again", IsConnected(5, eu2, ev2, 4));

    // 判环应用：一张图有 n 个点、n-1 条边且连通 => 就是树，没有环
    //  再插一条已在同一集合里的边 => 说明有环
    UFInit(S, 5);
    int sameSetEdgeFound = 0;
    int eu3[] = {1, 2, 3, 4, 1};
    int ev3[] = {2, 3, 4, 5, 5};
    for (int i = 0; i < 5; ++i) {
        if (UFSame(S, eu3[i], ev3[i])) { sameSetEdgeFound = 1; break; }
        UFUnion(S, eu3[i], ev3[i]);
    }
    expect_true("edge 1-5 closes a cycle", sameSetEdgeFound != 0);
    return finish();
}
