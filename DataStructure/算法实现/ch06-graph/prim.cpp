// ============================================================================
// 第6章 图 —— Prim 最小生成树【经典算法】
//   考点：
//     * 贪心：每次从"已经在树里的点"到"还没进树的点"的所有边中挑权值最小的
//     * 邻接矩阵实现 O(n^2)，适合稠密图；k 轮选点 + 每轮更新 lowcost
//     * lowcost[j] 的含义：树中所有点到 j 的最小边权；closest[j] 记录这条边来自谁
//     * 与 Kruskal 的区别：Prim 看"点"扩张，Kruskal 看"边"排序 + 并查集判环
// ============================================================================
#include "../ds_common.h"

#define V 6
#define INF 99999

// 王道经典例：v1..v6 = 0..5
static int G[V][V] = {
    {0,   6,   1,   5,   INF, INF},
    {6,   0,   5,   INF, 3,   INF},
    {1,   5,   0,   5,   6,   4  },
    {5,   INF, 5,   0,   INF, 2  },
    {INF, 3,   6,   INF, 0,   6  },
    {INF, INF, 4,   2,   6,   0  },
};

// 返回最小生成树的总权值；closest[j] 表示 j 是通过哪个点接入的
int Prim(int start, int closest[], int order[]) {
    int lowcost[V];
    bool inTree[V];
    for (int i = 0; i < V; ++i) {
        lowcost[i] = G[start][i];            // 初始化：只有起点在树里
        closest[i] = (lowcost[i] < INF) ? start : -1;
        inTree[i] = false;
    }
    inTree[start] = true;
    lowcost[start] = 0;

    int total = 0, cnt = 0;
    order[cnt++] = start;
    for (int round = 1; round < V; ++round) {
        // 1) 在还没进树的点里找 lowcost 最小的
        int best = -1;
        for (int j = 0; j < V; ++j)
            if (!inTree[j] && (best == -1 || lowcost[j] < lowcost[best])) best = j;
        if (best == -1 || lowcost[best] >= INF) return -1;   // 图不连通

        // 2) 把 best 加入树
        inTree[best] = true;
        total += lowcost[best];
        order[cnt++] = best;
        printf("  round %d: add v%d  edge v%d-v%d (w=%d)  total=%d\n",
               round, best, closest[best], best, lowcost[best], total);

        // 3) 用 best 更新其他点的 lowcost
        for (int j = 0; j < V; ++j)
            if (!inTree[j] && G[best][j] < lowcost[j]) {
                lowcost[j] = G[best][j];
                closest[j] = best;
            }
    }
    return total;
}

int main() {
    banner("Prim minimum spanning tree");
    int closest[V], order[V];
    int total = Prim(0, closest, order);

    expect_eq("MST weight", total, 15);                 // 1+4+2+5+3 = 15
    // 每个点都要接入，且起点不需要被接入
    int eOrder[] = {0, 2, 5, 3, 1, 4};
    expect_array("vertices added in order", order, V, eOrder, V);

    // 检查这 5 条边确实构成一棵树（无环 + 连通）：用并查集验证
    int parent[V];
    for (int i = 0; i < V; ++i) parent[i] = i;
    auto find = [&](int x) { while (parent[x] != x) x = parent[x]; return x; };
    int ok = 1;
    int weightCheck = 0;
    for (int j = 0; j < V; ++j) {
        if (j == order[0]) continue;                    // 起点没有入边
        int a = closest[j], b = j;
        if (find(a) == find(b)) { ok = 0; break; }      // 成环 -> 不是树
        parent[find(a)] = find(b);
        weightCheck += G[a][b];
    }
    expect_true("the chosen edges form a tree (no cycle)", ok != 0);
    expect_eq("sum of chosen edge weights == MST weight", weightCheck, 15);

    // 换一个起点，总权值必须一样（MST 权值唯一，但形状可能不同）
    int c2[V], o2[V];
    printf("  --- start from v3 (index 2) ---\n");
    int total2 = Prim(2, c2, o2);
    expect_eq("MST weight is the same from any start", total2, 15);

    // 不连通图：把 v6 的所有边都去掉，它就成了孤立点，Prim 必须报告失败
    int s1 = G[5][2], s2 = G[5][3], s3 = G[5][4];
    G[5][2] = G[2][5] = INF;
    G[5][3] = G[3][5] = INF;
    G[5][4] = G[4][5] = INF;
    printf("  --- disconnected graph (v6 isolated) ---\n");
    expect_eq("disconnected -> return -1", Prim(0, c2, o2), -1);
    G[5][2] = G[2][5] = s1;
    G[5][3] = G[3][5] = s2;
    G[5][4] = G[4][5] = s3;
    expect_eq("graph restored", Prim(0, c2, o2), 15);
    return finish();
}
