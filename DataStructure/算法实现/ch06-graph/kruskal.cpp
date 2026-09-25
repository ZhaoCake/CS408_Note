// ============================================================================
// 第6章 图 —— Kruskal 最小生成树【经典算法】
//   考点：
//     * 按边权从小到大试，能加就加；用并查集判断"加这条边会不会成环"
//     * 时间主要花在排序上：O(e log e)，适合稀疏图
//     * 与 Prim 对比：Prim 是"加点"（O(n^2)，稠密图好），Kruskal 是"加边"
//     * 两者选出的 MST 权值必然相同（MST 权值唯一），但边集可能不同
// ============================================================================
#include "../ds_common.h"

#define V 6
#define E 10
#define INFW 99999          // 用来表示"这条边不可用"

typedef struct { int u, v, w; } Edge;

// 和王道 Prim 例题同一张图，便于两个算法交叉验证
static Edge edges[E] = {
    {0, 2, 1}, {3, 5, 2}, {1, 4, 3}, {2, 5, 4}, {0, 3, 5},
    {1, 2, 5}, {2, 3, 5}, {0, 1, 6}, {2, 4, 6}, {4, 5, 6},
};

// 并查集（Kruskal 判环的关键工具）
static int parent[V];
static int Find(int x) {                    // 带路径压缩
    int root = x;
    while (parent[root] != root) root = parent[root];
    while (parent[x] != root) { int nxt = parent[x]; parent[x] = root; x = nxt; }
    return root;
}
static bool Union(int a, int b) {
    int ra = Find(a), rb = Find(b);
    if (ra == rb) return false;             // 已经在同一个集合 -> 这条边会成环
    parent[rb] = ra;
    return true;
}

static void SortEdges() {                   // 冒泡就够（演示用），真实场景用快排
    for (int i = 0; i < E - 1; ++i)
        for (int j = 0; j < E - 1 - i; ++j) {
            count_cmp();
            if (edges[j].w > edges[j + 1].w) {
                Edge t = edges[j]; edges[j] = edges[j + 1]; edges[j + 1] = t;
                count_move(3);
            }
        }
}

// 返回 MST 权值；chosen[] 记录选中的边，返回值为选中的边数
int Kruskal(int chosen[], int &chosenCount, int dump[][3]) {
    for (int i = 0; i < V; ++i) parent[i] = i;
    SortEdges();
    int total = 0;
    chosenCount = 0;
    for (int i = 0; i < E; ++i) {
        if (edges[i].w >= INFW) continue;           // 无效边（用来模拟"删掉这条边"）
        printf("  try edge v%d-v%d (w=%d) : %s\n", edges[i].u, edges[i].v, edges[i].w,
               Find(edges[i].u) == Find(edges[i].v) ? "would form a cycle -> skip" : "take it");
        if (Union(edges[i].u, edges[i].v)) {
            chosen[chosenCount] = i;
            dump[chosenCount][0] = edges[i].u;
            dump[chosenCount][1] = edges[i].v;
            dump[chosenCount][2] = edges[i].w;
            ++chosenCount;
            total += edges[i].w;
            if (chosenCount == V - 1) break;    // n 个点的 MST 只有 n-1 条边
        }
    }
    if (chosenCount < V - 1) return -1;         // 图不连通
    return total;
}

int main() {
    banner("Kruskal minimum spanning tree");
    int chosen[E], cnt = 0;
    int dump[E][3];

    int total = Kruskal(chosen, cnt, dump);
    expect_eq("MST weight", total, 15);
    expect_eq("edge count == n-1", cnt, V - 1);

    // 更简单的实现只返回权值，这里顺便验证：选中边的权值之和 == 返回值
    int sum = 0;
    for (int i = 0; i < cnt; ++i) sum += dump[i][2];
    expect_eq("sum of chosen weights", sum, 15);

    // 按排序顺序，每条边只被考虑一次
    int sorted = 1;
    for (int i = 0; i < E - 1; ++i) if (edges[i].w > edges[i + 1].w) sorted = 0;
    expect_true("edges are sorted by weight", sorted != 0);
    expect_eq("lightest edge is 1", edges[0].w, 1);
    expect_eq("heaviest edge is 6", edges[E - 1].w, 6);

    // 当选够 n-1 条边时就该提前结束，不必看完所有边
    expect_true("stopped early (did not take all 10 edges)", cnt == 5);

    // 不连通：把 v6（下标 5）的所有边都置为无效
    printf("  --- disconnected graph (v6 isolated) ---\n");
    Edge backup[E];                             // 注意：edges 已经被排序过，所以要整份备份
    for (int i = 0; i < E; ++i) backup[i] = edges[i];
    for (int i = 0; i < E; ++i)
        if (edges[i].u == 5 || edges[i].v == 5) edges[i].w = INFW;

    int c2 = 0;
    int dump2[E][3];
    expect_eq("disconnected -> -1", Kruskal(chosen, c2, dump2), -1);

    for (int i = 0; i < E; ++i) edges[i] = backup[i];
    int c3 = 0;
    int dump3[E][3];
    expect_eq("restored graph still gives 15", Kruskal(chosen, c3, dump3), 15);
    return finish();
}
