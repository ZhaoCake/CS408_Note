// ============================================================================
// 第6章 图 —— Floyd 多源最短路径【经典算法】
//   考点：
//     * 三重循环，中间点 k 必须在最外层
//     * 时间 O(n^3)、空间 O(n^2)；可以处理负权边，但不能有负权回路
//     * 允许中转：D[i][j] = min(D[i][j], D[i][k] + D[k][j])
//     * 与 Dijkstra 对比：Dijkstra 是"单源"，Floyd 一次算出所有点对
// ============================================================================
#include "../ds_common.h"

#define V 4
#define INF 99999

// 有向图（王道经典例）：A=0 B=1 C=2 D=3
//  A->B:2  A->C:6  A->D:4  B->C:3  C->A:7  C->D:1  D->A:5  D->C:12
static int G[V][V] = {
    {0,   2,   6,   4  },
    {INF, 0,   3,   INF},
    {7,   INF, 0,   1  },
    {5,   INF, 12,  0  },
};

void Floyd(int D[V][V], int path[V][V]) {
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j)
            path[i][j] = (D[i][j] < INF && i != j) ? i : -1;   // 初始：直接边，前驱是起点
    for (int k = 0; k < V; ++k)                               // 中间点必须放最外层
        for (int i = 0; i < V; ++i)
            for (int j = 0; j < V; ++j)
                if (D[i][k] < INF && D[k][j] < INF && D[i][k] + D[k][j] < D[i][j]) {
                    D[i][j] = D[i][k] + D[k][j];
                    path[i][j] = path[k][j];                  // 记录"最后一段"来自谁
                }
}

void PrintMatrix(const int D[V][V], const char *tag) {
    printf("  %s\n", tag);
    for (int i = 0; i < V; ++i) {
        printf("    ");
        for (int j = 0; j < V; ++j) {
            if (D[i][j] >= INF) printf("  INF");
            else printf("%5d", D[i][j]);
        }
        printf("\n");
    }
}

void PrintPath(const int path[V][V], int i, int j) {
    if (path[i][j] == -1) { printf("no path"); return; }
    int st[V * 2], top = -1;
    for (int p = j; p != -1 && p != i; p = path[i][p]) st[++top] = p;
    printf("%d", i);
    for (int t = top; t >= 0; --t) printf(" -> %d", st[t]);
}

int main() {
    banner("Floyd all-pairs shortest paths");
    int D[V][V], path[V][V];
    for (int i = 0; i < V; ++i) for (int j = 0; j < V; ++j) D[i][j] = G[i][j];

    PrintMatrix(D, "initial adjacency matrix:");
    Floyd(D, path);
    PrintMatrix(D, "after Floyd:");

    // 逐项核对（手算结果）
    int e0[] = {0, 2, 5, 4};
    int e1[] = {9, 0, 3, 4};
    int e2[] = {6, 8, 0, 1};
    int e3[] = {5, 7, 10, 0};
    expect_array("row A", D[0], V, e0, V);
    expect_array("row B", D[1], V, e1, V);
    expect_array("row C", D[2], V, e2, V);
    expect_array("row D", D[3], V, e3, V);

    // 对角线必须都是 0
    int diagOk = 1;
    for (int i = 0; i < V; ++i) if (D[i][i] != 0) diagOk = 0;
    expect_true("diagonal is 0", diagOk != 0);

    printf("  paths:\n");
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j) {
            if (i == j) continue;
            printf("    %d -> %d : ", i, j);
            PrintPath(path, i, j);
            printf("   (len=%d)\n", D[i][j]);
        }

    // A->C 走 A->B->C，比直达 6 更短
    expect_eq("A->C uses B as the middle point", D[0][2], 5);
    expect_eq("A->D keeps the direct edge", D[0][3], 4);
    // B->A 必须绕 C、D
    expect_eq("B->A via C,D", D[1][0], 9);
    expect_eq("C->B via D,A", D[2][1], 8);

    // 路径长度自检：把路径上每条边加起来，应该等于 D[i][j]
    int check = 1;
    for (int i = 0; i < V; ++i)
        for (int j = 0; j < V; ++j) {
            if (i == j || D[i][j] >= INF) continue;
            int prev = i, len = 0, guard = 0;
            int st[V * 2], top = -1;
            for (int p = j; p != -1 && p != i; p = path[i][p]) st[++top] = p;
            for (int t = top; t >= 0; --t) { len += G[prev][st[t]]; prev = st[t]; ++guard; if (guard > V) break; }
            if (len != D[i][j]) { check = 0; printf("    mismatch %d->%d: %d vs %d\n", i, j, len, D[i][j]); }
        }
    expect_true("all path lengths match D[][]", check != 0);
    return finish();
}
