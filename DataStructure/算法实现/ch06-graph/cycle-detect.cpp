// ============================================================================
// 第6章 图 —— 判环【经典算法】
//   三种判法：
//     1) 有向图：DFS 三色标记（白=没访问，灰=在递归栈里，黑=已完成）
//                遇到"灰点"说明回到了当前路径上的祖先 -> 有环
//     2) 有向图：Kahn 拓扑排序，输出顶点数 < n 就有环
//     3) 无向图：DFS 时记住"父亲"，遇到已访问且不是父亲的点 -> 有环
//                （无向图也可以用并查集：加边时两端已在同一集合 -> 有环）
// ============================================================================
#include "../ds_common.h"

#define V 6

// ------------------------------- 1) 有向图：DFS 三色 -------------------------------
static int g_dir[V][V];

static bool dfsColor(int u, int color[]) {
    color[u] = 1;                                  // 灰色：正在处理的路径上
    for (int v = 0; v < V; ++v) {
        if (!g_dir[u][v]) continue;
        if (color[v] == 1) return true;            // 回到灰点 -> 有环
        if (color[v] == 0 && dfsColor(v, color)) return true;
    }
    color[u] = 2;                                  // 黑色：处理完了
    return false;
}

bool HasCycleDirected() {
    int color[V] = {0};
    for (int i = 0; i < V; ++i)
        if (color[i] == 0 && dfsColor(i, color)) return true;
    return false;
}

// ------------------------------- 2) 有向图：Kahn -------------------------------
bool HasCycleByKahn() {
    int indeg[V] = {0};
    int edges = 0;
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            if (g_dir[i][j]) { ++indeg[j]; ++edges; }
    int q[V], head = 0, tail = 0;
    for (int i = 0; i < V; ++i) if (indeg[i] == 0) q[tail++] = i;
    int cnt = 0;
    while (head < tail) {
        int u = q[head++];
        ++cnt;
        for (int v = 0; v < V; ++v)
            if (g_dir[u][v] && --indeg[v] == 0) q[tail++] = v;
    }
    return cnt < V;                                // 排不完所有的点 -> 有环
}

// ------------------------------- 3) 无向图：DFS + 父亲 -------------------------------
static int g_undir[V][V];

static bool dfsUndir(int u, int parent, bool vis[]) {
    vis[u] = true;
    for (int v = 0; v < V; ++v) {
        if (!g_undir[u][v]) continue;
        if (!vis[v]) {
            if (dfsUndir(v, u, vis)) return true;
        } else if (v != parent) {
            return true;                           // 访问到"不是父亲"的已访问点 -> 有环
        }
    }
    return false;
}

bool HasCycleUndirected() {
    bool vis[V] = {false};
    for (int i = 0; i < V; ++i)
        if (!vis[i] && dfsUndir(i, -1, vis)) return true;
    return false;
}

// 无向图判环（并查集版）：加边时两端已经连通 -> 有环
static int uf[V];
static int Find(int x) { while (uf[x] != x) x = uf[x]; return x; }
bool HasCycleUndirectedUF() {
    for (int i = 0; i < V; ++i) uf[i] = i;
    for (int i = 0; i < V; ++i)
        for (int j = i + 1; j < V; ++j) {
            if (!g_undir[i][j]) continue;
            int a = Find(i), b = Find(j);
            if (a == b) return true;
            uf[a] = b;
        }
    return false;
}

static void ClearGraphs() {
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j) { g_dir[i][j] = 0; g_undir[i][j] = 0; }
}

int main() {
    banner("Cycle detection");

    // ---- 有向无环图：0->1, 0->2, 1->3, 2->3, 3->4, 3->5 ----
    ClearGraphs();
    int dagU[] = {0, 0, 1, 2, 3, 3};
    int dagV[] = {1, 2, 3, 3, 4, 5};
    for (int i = 0; i < 6; ++i) g_dir[dagU[i]][dagV[i]] = 1;
    expect_true("DAG has no cycle (DFS colors)", !HasCycleDirected());
    expect_true("DAG has no cycle (Kahn)", !HasCycleByKahn());

    // ---- 加一条 4->0，制造环 0->1->3->4->0 ----
    g_dir[4][0] = 1;
    expect_true("adding 4->0 creates a cycle (DFS)", HasCycleDirected());
    expect_true("adding 4->0 creates a cycle (Kahn)", HasCycleByKahn());
    g_dir[4][0] = 0;

    // ---- 自环也算环 ----
    g_dir[2][2] = 1;
    expect_true("self loop is a cycle", HasCycleDirected());
    g_dir[2][2] = 0;

    // ---- 只有两点互指的环 ----
    ClearGraphs();
    g_dir[0][1] = g_dir[1][0] = 1;
    expect_true("2-cycle detected", HasCycleDirected());
    expect_true("2-cycle detected (Kahn)", HasCycleByKahn());

    // ---- 无向图：树（无环）----
    ClearGraphs();
    int tU[] = {0, 0, 1, 1, 2};
    int tV[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) g_undir[tU[i]][tV[i]] = g_undir[tV[i]][tU[i]] = 1;
    expect_true("tree has no cycle (DFS parent)", !HasCycleUndirected());
    expect_true("tree has no cycle (union-find)", !HasCycleUndirectedUF());

    // ---- 无向图加一条 3-5，出现环 1-3-...-5-2-1 ----
    g_undir[3][5] = g_undir[5][3] = 1;
    expect_true("adding 3-5 creates a cycle (DFS parent)", HasCycleUndirected());
    expect_true("adding 3-5 creates a cycle (union-find)", HasCycleUndirectedUF());
    g_undir[3][5] = g_undir[5][3] = 0;

    // ---- 再加一条 4-5，环是 1-4-5-2-0-1 ----
    g_undir[4][5] = g_undir[5][4] = 1;
    expect_true("adding 4-5 creates a cycle (DFS parent)", HasCycleUndirected());
    expect_true("adding 4-5 creates a cycle (union-find)", HasCycleUndirectedUF());
    g_undir[4][5] = g_undir[5][4] = 0;

    // ---- 无环图边数上限：n 个点的森林最多 n-1 条边 ----
    ClearGraphs();
    int edges = 0;
    for (int i = 0; i < 5; ++i) { g_undir[i][i + 1] = g_undir[i + 1][i] = 1; ++edges; }
    expect_eq("chain of 6 vertices has 5 edges", edges, V - 1);
    expect_true("chain is acyclic", !HasCycleUndirected());
    g_undir[5][0] = g_undir[0][5] = 1;              // 首尾相接 -> 成环
    expect_true("closing the chain creates a cycle", HasCycleUndirected());
    return finish();
}
