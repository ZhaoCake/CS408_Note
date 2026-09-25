// ============================================================================
// 第6章 图 —— 关键路径（AOE 网）【经典算法】
//   考点（计算步骤必须背下来）：
//     1) 拓扑排序求 ve[]（事件最早发生时间）：ve[v] = max(ve[u] + w(u,v))
//     2) 逆拓扑序求 vl[]（事件最迟发生时间）：vl[u] = min(vl[v] - w(u,v))
//     3) 活动最早开始 e(i) = ve[起点]，活动最迟开始 l(i) = vl[终点] - w
//     4) e(i) == l(i) 的活动就是关键活动，它们连起来就是关键路径
//     5) 关键路径长度 = ve[汇点]；缩短关键活动可能缩短工期，缩短非关键活动没用
// ============================================================================
#include "../ds_common.h"

#define V 6
#define E 7
#define INF 99999

typedef struct { int u, v, w; const char *name; } Act;

// AOE 网：a1:0->1=3  a2:0->2=2  a3:1->3=2  a4:2->3=4
//         a5:1->4=3  a6:3->5=3  a7:4->5=2
static Act A[E] = {
    {0, 1, 3, "a1"}, {0, 2, 2, "a2"}, {1, 3, 2, "a3"}, {2, 3, 4, "a4"},
    {1, 4, 3, "a5"}, {3, 5, 3, "a6"}, {4, 5, 2, "a7"},
};

static void TopoOrder(int order[], int &n) {
    int indeg[V] = {0};
    for (int i = 0; i < E; ++i) ++indeg[A[i].v];
    int q[V], head = 0, tail = 0;
    for (int i = 0; i < V; ++i) if (indeg[i] == 0) q[tail++] = i;
    n = 0;
    while (head < tail) {
        int u = q[head++];
        order[n++] = u;
        for (int i = 0; i < E; ++i)
            if (A[i].u == u && --indeg[A[i].v] == 0) q[tail++] = A[i].v;
    }
}

// 返回关键路径长度；ve/vl 为事件时间，e/l 为活动时间
int CriticalPath(int ve[], int vl[], int e[], int l[]) {
    int order[V], n = 0;
    TopoOrder(order, n);

    // 1) 正推 ve
    for (int i = 0; i < V; ++i) ve[i] = 0;
    for (int k = 0; k < n; ++k) {
        int u = order[k];
        for (int i = 0; i < E; ++i)
            if (A[i].u == u && ve[u] + A[i].w > ve[A[i].v])
                ve[A[i].v] = ve[u] + A[i].w;
    }

    // 2) 逆推 vl
    for (int i = 0; i < V; ++i) vl[i] = ve[order[n - 1]];      // 汇点的 vl = ve
    for (int k = n - 1; k >= 0; --k) {
        int u = order[k];
        for (int i = 0; i < E; ++i)
            if (A[i].u == u && vl[A[i].v] - A[i].w < vl[u])
                vl[u] = vl[A[i].v] - A[i].w;
    }

    // 3) 活动时间
    for (int i = 0; i < E; ++i) {
        e[i] = ve[A[i].u];
        l[i] = vl[A[i].v] - A[i].w;
    }
    return ve[order[n - 1]];
}

int main() {
    banner("Critical path (AOE network)");
    int ve[V], vl[V], e[E], l[E];
    int len = CriticalPath(ve, vl, e, l);

    printf("  vertex : ve  vl\n");
    for (int i = 0; i < V; ++i) printf("    v%d   : %2d  %2d\n", i + 1, ve[i], vl[i]);
    printf("  activity : e   l   slack  critical?\n");
    for (int i = 0; i < E; ++i)
        printf("    %s (%d->%d, w=%d) : %2d  %2d   %2d   %s\n",
               A[i].name, A[i].u + 1, A[i].v + 1, A[i].w, e[i], l[i], l[i] - e[i],
               (e[i] == l[i]) ? "YES" : "no");

    int eVe[] = {0, 3, 2, 6, 6, 9};
    expect_array("ve[]", ve, V, eVe, V);
    int eVl[] = {0, 4, 2, 6, 7, 9};
    expect_array("vl[]", vl, V, eVl, V);
    expect_eq("critical path length", len, 9);

    // 关键活动必须是 a2(0->2), a4(2->3), a6(3->5)
    int key[E], kn = 0;
    for (int i = 0; i < E; ++i) if (e[i] == l[i]) key[kn++] = i;
    expect_eq("number of critical activities", kn, 3);
    expect_eq("a2 is critical", e[1] == l[1], 1);
    expect_eq("a4 is critical", e[3] == l[3], 1);
    expect_eq("a6 is critical", e[5] == l[5], 1);
    expect_true("a1 is not critical", e[0] != l[0]);
    expect_true("a5 is not critical", e[4] != l[4]);
    expect_true("a7 is not critical", e[6] != l[6]);

    // 关键活动连成的路径长度必须正好等于关键路径长度
    int sum = 0;
    for (int i = 0; i < kn; ++i) sum += A[key[i]].w;
    expect_eq("sum of critical activity weights", sum, 9);

    // 基本不变量：ve <= vl（最早不能晚于最迟），且源点 ve == vl == 0
    int ok = 1;
    for (int i = 0; i < V; ++i) if (ve[i] > vl[i]) ok = 0;
    expect_true("ve[i] <= vl[i] for every event", ok != 0);
    expect_eq("source ve", ve[0], 0);
    expect_eq("source vl", vl[0], 0);
    expect_eq("sink ve == sink vl", ve[5], vl[5]);

    // 关键路径上的事件，ve == vl（没有任何机动时间）
    expect_eq("v3 is on the critical path", ve[2] == vl[2], 1);
    expect_eq("v4 is on the critical path", ve[3] == vl[3], 1);
    expect_true("v2 is NOT on the critical path", ve[1] != vl[1]);
    expect_true("v5 is NOT on the critical path", ve[4] != vl[4]);
    return finish();
}
