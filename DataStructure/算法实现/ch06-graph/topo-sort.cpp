// ============================================================================
// 第6章 图 —— 拓扑排序（AOV 网）【经典算法】
//   考点：
//     * 拓扑排序序列不唯一；只要每条边 u->v 都满足 u 排在 v 前面就是合法序列
//     * Kahn（入度法）：反复取入度为 0 的点，删掉它和它的出边
//     * DFS 法：按"完成时间"逆序输出（本质是后序遍历的逆序）
//     * 判有向环：Kahn 法输出的顶点数 < n 就说明有环
//     * 只有 DAG（有向无环图）才有拓扑排序
// ============================================================================
#include "../ds_common.h"

#define V 6
#define E 6

// AOV 网：0->1, 0->2, 1->3, 2->3, 3->4, 3->5
static int g_adj[V][V];
static const int edgeU[E] = {0, 0, 1, 2, 3, 3};
static const int edgeV[E] = {1, 2, 3, 3, 4, 5};

static void BuildGraph() {
    for (int i = 0; i < V; ++i) for (int j = 0; j < V; ++j) g_adj[i][j] = 0;
    for (int i = 0; i < E; ++i) g_adj[edgeU[i]][edgeV[i]] = 1;
}

// ------------------------------- Kahn（入度法） -------------------------------
// 返回实际输出的顶点个数；小于 n 说明有环
int TopoSortKahn(int order[]) {
    int indeg[V] = {0};
    // 入度必须从当前邻接矩阵算，不能从固定的边表算（否则改图后就不一致了）
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            if (g_adj[i][j]) ++indeg[j];
    int q[V], head = 0, tail = 0;
    for (int i = 0; i < V; ++i) if (indeg[i] == 0) q[tail++] = i;   // 入度为 0 的先入队

    int cnt = 0;
    while (head < tail) {
        int u = q[head++];
        order[cnt++] = u;
        for (int v = 0; v < V; ++v)
            if (g_adj[u][v] && --indeg[v] == 0) q[tail++] = v;
    }
    return cnt;
}

// ------------------------------- DFS 法 -------------------------------
static bool g_vis[V];
static int  g_finish[V], g_finN;
static void DFSFinish(int u) {
    g_vis[u] = 1;
    for (int v = 0; v < V; ++v)
        if (g_adj[u][v] && !g_vis[v]) DFSFinish(v);
    g_finish[g_finN++] = u;                 // 所有后代都处理完了才记录自己
}
int TopoSortDFS(int order[]) {
    for (int i = 0; i < V; ++i) g_vis[i] = 0;
    g_finN = 0;
    for (int i = 0; i < V; ++i)
        if (!g_vis[i]) DFSFinish(i);
    for (int i = 0; i < g_finN; ++i) order[i] = g_finish[g_finN - 1 - i];   // 逆序
    return g_finN;
}

// 校验：每条边 u->v 都必须满足 pos[u] < pos[v]
bool IsValidTopo(const int order[], int n) {
    int pos[V];
    for (int i = 0; i < V; ++i) pos[i] = -1;
    for (int i = 0; i < n; ++i) pos[order[i]] = i;
    for (int i = 0; i < E; ++i) {
        count_cmp();
        if (pos[edgeU[i]] >= pos[edgeV[i]]) return false;
    }
    return true;
}

int main() {
    banner("Topological sort (AOV network)");
    BuildGraph();

    int order[V];
    int n1 = TopoSortKahn(order);
    printf("  Kahn order:");
    for (int i = 0; i < n1; ++i) printf(" %d", order[i]);
    printf("\n");
    expect_eq("Kahn outputs all vertices (no cycle)", n1, V);
    expect_true("Kahn order is valid", IsValidTopo(order, n1));

    int order2[V];
    int n2 = TopoSortDFS(order2);
    printf("  DFS order :");
    for (int i = 0; i < n2; ++i) printf(" %d", order2[i]);
    printf("\n");
    expect_eq("DFS outputs all vertices", n2, V);
    expect_true("DFS order is valid", IsValidTopo(order2, n2));

    // 两种方法给出的顺序一般不同 —— 拓扑排序序列本来就不唯一
    int same = (n1 == n2);
    for (int i = 0; same && i < n1; ++i) if (order[i] != order2[i]) same = 0;
    expect_true("topological order is NOT unique", same == 0);
    expect_true("but both orders are valid", IsValidTopo(order, n1) && IsValidTopo(order2, n2));

    // 关键的顺序约束
    int pos[V];
    for (int i = 0; i < V; ++i) pos[order[i]] = i;
    expect_true("0 before 1 and 2", pos[0] < pos[1] && pos[0] < pos[2]);
    expect_true("1 and 2 before 3", pos[1] < pos[3] && pos[2] < pos[3]);
    expect_true("3 before 4 and 5", pos[3] < pos[4] && pos[3] < pos[5]);

    // 加一条 4 -> 0 的边，制造环
    printf("  --- add edge 4->0 (creates a cycle) ---\n");
    g_adj[4][0] = 1;
    int n3 = TopoSortKahn(order);
    expect_true("Kahn now outputs fewer than n vertices", n3 < V);
    printf("  Kahn stopped after %d vertices -> there is a cycle\n", n3);
    g_adj[4][0] = 0;

    // 完全独立的点也能排进来
    printf("  --- make v5 an isolated vertex ---\n");
    g_adj[3][5] = 0;
    int n4 = TopoSortKahn(order);
    expect_eq("still all vertices", n4, V);
    g_adj[3][5] = 1;
    return finish();
}
