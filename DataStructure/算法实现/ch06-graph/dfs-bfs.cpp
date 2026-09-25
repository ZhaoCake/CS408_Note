// ============================================================================
// 第6章 图 —— DFS / BFS【经典算法】
//   考点：
//     * DFS 用栈（递归本身就是系统栈），BFS 用队列
//     * 邻接矩阵的 DFS/BFS 时间复杂度都是 O(n^2)（要找每个点的所有邻接点）
//       邻接表的 DFS/BFS 是 O(n+e)
//     * 遍历序列与"邻接点的扫描顺序"有关，题目一般默认按编号从小到大
//     * 连通分量个数 = 每次从没访问过的点重新出发的次数
// ============================================================================
#include "../ds_common.h"

#define V 7
#define INF 99999

// 无向图（最后加了一个孤立点 6，用来演示连通分量计数）
//  0-1:6  0-2:1  0-3:5  1-2:5  1-4:3  2-3:5  2-4:6  2-5:4  3-5:2  4-5:6
static int M[V][V] = {
    {0,   6,   1,   5,   INF, INF, INF},
    {6,   0,   5,   INF, 3,   INF, INF},
    {1,   5,   0,   5,   6,   4,   INF},
    {5,   INF, 5,   0,   INF, 2,   INF},
    {INF, 3,   6,   INF, 0,   6,   INF},
    {INF, INF, 4,   2,   6,   0,   INF},
    {INF, INF, INF, INF, INF, INF, 0},
};

static bool g_visited[V];
static int  g_seq[V * 2], g_n;

static void ResetVisit() {
    for (int i = 0; i < V; ++i) g_visited[i] = 0;
    g_n = 0;
}

// ------------------------------- DFS（递归） -------------------------------
void DFS(int u) {
    g_visited[u] = 1;
    g_seq[g_n++] = u;
    for (int v = 0; v < V; ++v)                    // 按编号从小到大扫描邻接点
        if (M[u][v] != 0 && M[u][v] != INF && !g_visited[v]) DFS(v);
}

// ------------------------------- DFS（非递归，显式栈） -------------------------------
void DFSNR(int u) {
    int st[V * 2];
    int top = -1;
    g_visited[u] = 1;
    g_seq[g_n++] = u;
    st[++top] = u;
    while (top >= 0) {
        int x = st[top];
        int found = -1;
        for (int v = 0; v < V; ++v) {              // 找一个还没访问的邻接点
            if (M[x][v] != 0 && M[x][v] != INF && !g_visited[v]) { found = v; break; }
        }
        if (found == -1) {
            --top;                                 // 该点已经走不下去，回溯
        } else {
            g_visited[found] = 1;
            g_seq[g_n++] = found;
            st[++top] = found;
        }
    }
}

// ------------------------------- BFS（队列） -------------------------------
void BFS(int u) {
    int q[V * 2];
    int head = 0, tail = 0;
    g_visited[u] = 1;
    q[tail++] = u;
    while (head < tail) {
        int x = q[head++];
        g_seq[g_n++] = x;
        for (int v = 0; v < V; ++v)
            if (M[x][v] != 0 && M[x][v] != INF && !g_visited[v]) {
                g_visited[v] = 1;                  // BFS 一入队就标记，避免重复入队
                q[tail++] = v;
            }
    }
}

// 连通分量个数（对非连通图，每次都要挑一个没访问过的点重新开始）
int CountComponents() {
    ResetVisit();
    int comp = 0;
    for (int i = 0; i < V; ++i)
        if (!g_visited[i]) { ++comp; DFS(i); }
    return comp;
}

int main() {
    banner("Graph : DFS / BFS");
    // 结构：0-1,0-2,0-3,1-2,1-4,2-3,2-4,2-5,3-5,4-5，外加孤立点 6

    ResetVisit(); DFS(0);
    int eDFS[] = {0, 1, 2, 3, 5, 4};
    expect_array("DFS from 0", g_seq, g_n, eDFS, 6);

    ResetVisit(); DFSNR(0);
    expect_array("DFS from 0 (stack version)", g_seq, g_n, eDFS, 6);

    ResetVisit(); BFS(0);
    int eBFS[] = {0, 1, 2, 3, 4, 5};
    expect_array("BFS from 0", g_seq, g_n, eBFS, 6);

    // 连通分量：孤立点 6 自成一个分量
    expect_eq("connected components", CountComponents(), 2);

    // 从孤立点出发只能访问到它自己
    ResetVisit(); BFS(6);
    expect_eq("BFS from an isolated vertex", g_n, 1);

    printf("  degrees: ");
    for (int i = 0; i < V; ++i) {
        int deg = 0;
        for (int j = 0; j < V; ++j) if (M[i][j] != 0 && M[i][j] != INF) ++deg;
        printf("v%d=%d ", i, deg);
    }
    printf("\n");
    // 度数与边数的关系：无向图度数之和 = 2 * 边数（此时 6 号点还是孤立点）
    int degSum = 0, edges = 0;
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            if (M[i][j] != 0 && M[i][j] != INF) { ++degSum; if (i < j) ++edges; }
    expect_eq("sum of degrees == 2 * edges", degSum, 2 * edges);
    expect_eq("edge count", edges, 10);
    expect_eq("isolated vertex contributes 0 degree", degSum, 20);

    // 把孤立点接上去，整张图就只剩一个连通分量
    M[0][6] = M[6][0] = 7;
    expect_eq("components after connecting the isolated vertex", CountComponents(), 1);
    return finish();
}
