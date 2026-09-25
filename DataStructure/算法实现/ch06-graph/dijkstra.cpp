// ============================================================================
// 第6章 图 —— Dijkstra 单源最短路径【经典算法】
//   考点：
//     * 每轮"在还没确定的点里选 dist 最小的" -> 把它定下来 -> 用它松弛邻居
//     * 时间复杂度 O(n^2)（邻接矩阵 + 线性找最小）；用堆可以做到 O(e log n)
//     * 不能处理负权边（因为算法默认"已经确定的点不会再变短"）
//     * path[] 记录前驱，最后顺着 path 回溯就能得到完整路径
// ============================================================================
#include "../ds_common.h"

#define V 6
#define INF 99999

// 与 Prim 用同一张无向图，方便交叉验证
static int G[V][V] = {
    {0,   6,   1,   5,   INF, INF},
    {6,   0,   5,   INF, 3,   INF},
    {1,   5,   0,   5,   6,   4  },
    {5,   INF, 5,   0,   INF, 2  },
    {INF, 3,   6,   INF, 0,   6  },
    {INF, INF, 4,   2,   6,   0  },
};

// 从 s 出发到所有点的最短距离；path[j] 是 j 的前驱，-1 表示不可达
void Dijkstra(int s, int dist[], int path[]) {
    bool done[V];
    for (int i = 0; i < V; ++i) {
        dist[i] = G[s][i];
        path[i] = (dist[i] < INF && i != s) ? s : -1;
        done[i] = false;
    }
    dist[s] = 0;
    done[s] = true;

    for (int round = 1; round < V; ++round) {
        // 1) 选一个 dist 最小且还没确定的点
        int u = -1;
        for (int i = 0; i < V; ++i)
            if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1 || dist[u] >= INF) break;          // 剩下的点都不可达
        done[u] = true;
        printf("  round %d: settle v%d (dist=%d)\n", round, u, dist[u]);

        // 2) 用 u 松弛
        for (int w = 0; w < V; ++w)
            if (!done[w] && G[u][w] < INF && dist[u] + G[u][w] < dist[w]) {
                dist[w] = dist[u] + G[u][w];
                path[w] = u;
            }
    }
}

// 顺着 path 回溯打印路径（从 s 到 v）
void PrintPath(const int path[], int v) {
    int st[V], top = -1;
    for (int p = v; p != -1; p = path[p]) st[++top] = p;
    for (int i = top; i >= 0; --i) printf("%d%s", st[i], i ? " -> " : "\n");
}

int main() {
    banner("Dijkstra shortest path (single source)");
    int dist[V], path[V];

    Dijkstra(0, dist, path);
    int eDist[] = {0, 6, 1, 5, 7, 5};
    expect_array("dist from v0", dist, V, eDist, V);

    printf("  paths from v0:\n");
    for (int i = 0; i < V; ++i) { printf("    to v%d : ", i); PrintPath(path, i); }

    // 路径的总长度必须等于 dist
    int check = 1;
    for (int i = 0; i < V; ++i) {
        if (i == 0) continue;
        int len = 0;
        for (int p = i; p != -1 && path[p] != -1; p = path[p]) len += G[p][path[p]];
        if (len != dist[i]) { check = 0; printf("    mismatch at v%d: %d vs %d\n", i, len, dist[i]); }
    }
    expect_true("reconstructed lengths match dist[]", check != 0);

    // 换源点：从 v2 出发
    Dijkstra(2, dist, path);
    int eDist2[] = {1, 5, 0, 5, 6, 4};
    expect_array("dist from v2", dist, V, eDist2, V);

    // 直达的距离不可能比中转更差：这是 Dijkstra 的不变量
    expect_true("distance is the best of all routes", dist[0] <= dist[2] + G[2][0]);

    // 不可达：把 v4 的所有边都去掉，它就与源点断开了
    int s1 = G[1][4], s2 = G[2][4], s3 = G[5][4];
    G[1][4] = G[4][1] = INF;
    G[2][4] = G[4][2] = INF;
    G[5][4] = G[4][5] = INF;
    Dijkstra(0, dist, path);
    expect_eq("unreachable vertex keeps INF", dist[4] >= INF, 1);
    expect_eq("its path is -1", path[4], -1);
    G[1][4] = G[4][1] = s1;
    G[2][4] = G[4][2] = s2;
    G[5][4] = G[4][5] = s3;
    return finish();
}
