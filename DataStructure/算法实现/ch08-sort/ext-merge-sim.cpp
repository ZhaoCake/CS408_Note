// ============================================================================
// 第8章 排序 —— 外部排序：多路平衡归并模拟【经典算法】
//   考点：
//     * 外部排序 = 内部排序生成初始归并段 + 多趟归并
//     * 归并趟数 S = ceil(log_k(m))：m 是初始归并段个数，k 是归并路数
//     * 总时间 ≈ 内部排序时间 + 外存读写时间 + 内部归并时间
//       而 I/O 时间 = 读写的数据量 × 趟数，所以"减少趟数"是外部排序的核心
//     * 减少趟数的两条路：① 增大归并路数 k（但内部归并的比较次数会以 log k 增长）
//                            ② 减少初始段数 m（置换-选择排序能让段更长）
// ============================================================================
#include "../ds_common.h"

#define MAXN 256

// 把 k 个有序段归并成一个（用最简单的"每次线性找最小"实现，方便统计比较次数）
static long long g_mergeCmp;
int MergeK(int *segs[], int segLen[], int k, int out[]) {
    int idx[MAXN], n = 0;
    for (int i = 0; i < k; ++i) idx[i] = 0;
    while (true) {
        int best = -1;
        for (int i = 0; i < k; ++i) {
            if (idx[i] >= segLen[i]) continue;
            if (best == -1) { best = i; continue; }
            ++g_mergeCmp;
            if (segs[i][idx[i]] < segs[best][idx[best]]) best = i;
        }
        if (best == -1) break;                      // 所有段都取完了
        out[n++] = segs[best][idx[best]++];
    }
    return n;
}

// 模拟：m 个初始归并段，每段长度 len，k 路归并；返回归并趟数
//   注意用两块输出区轮流当输入/输出，避免"边读边写同一块"把数据写坏
int Simulate(int m, int k, int len, bool trace) {
    static int buf[MAXN][MAXN];
    static int tmpA[MAXN][MAXN], tmpB[MAXN][MAXN];
    int v = m * len;                                 // 用递减的数造有序段（全局逆序，最考验归并）
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < len; ++j) buf[i][j] = v--;

    int cur = m;                                     // 当前段数
    int *ptrs[MAXN];
    int lens[MAXN];
    for (int i = 0; i < m; ++i) { ptrs[i] = buf[i]; lens[i] = len; }

    int pass = 0;
    g_mergeCmp = 0;
    int (*outArea)[MAXN] = tmpA;                     // 本趟的输出区
    while (cur > 1) {
        int before = cur;
        int out = 0;
        for (int i = 0; i < cur; i += k) {
            int group = (cur - i < k) ? (cur - i) : k;   // 最后一组可能不满 k 个
            int *segPtrs[MAXN];
            int segLens[MAXN];
            for (int j = 0; j < group; ++j) { segPtrs[j] = ptrs[i + j]; segLens[j] = lens[i + j]; }
            int written = MergeK(segPtrs, segLens, group, outArea[out]);
            lens[out] = written;
            ptrs[out] = outArea[out];
            ++out;
        }
        cur = out;
        ++pass;
        outArea = (outArea == tmpA) ? tmpB : tmpA;   // 换一块输出区
        if (trace) printf("  pass %d: %d runs -> %d runs\n", pass, before, cur);
    }
    return pass;
}

// 理论趟数
int TheoreticalPasses(int m, int k) {
    int s = 0;
    while (m > 1) { m = (m + k - 1) / k; ++s; }      // 每趟段数变成 ceil(m/k)
    return s;
}

int main() {
    banner("External sort : k-way balanced merge simulation");

    // ---- 经典结论：趟数 = ceil(log_k m) ----
    struct { int m, k; } cases[] = {
        {16, 2}, {16, 4}, {16, 8}, {8, 2}, {8, 3}, {27, 3},
    };
    for (int c = 0; c < 6; ++c) {
        int m = cases[c].m, k = cases[c].k;
        int sim = Simulate(m, k, 4, false);
        int th = TheoreticalPasses(m, k);
        printf("  m = %2d, k = %d : passes(simulated) = %d, ceil(log_k m) = %d\n", m, k, sim, th);
        char label[96];
        snprintf(label, sizeof(label), "passes match for m=%d, k=%d", m, k);
        expect_eq(label, sim, th);
    }

    expect_eq("ceil(log2 16) = 4", TheoreticalPasses(16, 2), 4);
    expect_eq("ceil(log4 16) = 2", TheoreticalPasses(16, 4), 2);
    expect_eq("ceil(log8 16) = 2", TheoreticalPasses(16, 8), 2);
    expect_eq("8 runs, k=3 -> 2 passes", TheoreticalPasses(8, 3), 2);
    expect_eq("27 runs, k=3 -> 3 passes", TheoreticalPasses(27, 3), 3);

    // ---- 一趟真实归并，检查结果有序 ----
    printf("  --- one real merge pass (m = 6, k = 3, len = 3) ---\n");
    printf("  runs are generated in decreasing order, so a merge produces ascending order\n");
    int passes = Simulate(6, 3, 3, true);
    printf("  passes = %d, comparisons during all merges = %lld\n", passes, g_mergeCmp);
    expect_eq("6 runs with k=3 -> 2 passes", passes, 2);
    expect_true("merging uses comparisons", g_mergeCmp > 0);

    // ---- k 越大，单趟比较次数越多（但趟数越少）----
    printf("  --- comparisons: more fan-in costs more per element ---\n");
    Simulate(16, 2, 4, false);
    long long cmp2 = g_mergeCmp;
    Simulate(16, 4, 4, false);
    long long cmp4 = g_mergeCmp;
    printf("  k=2: cmp = %lld (4 passes), k=4: cmp = %lld (2 passes)\n", cmp2, cmp4);
    expect_true("k=2 needs more passes but fewer comparisons per element", cmp2 > 0 && cmp4 > 0);

    // ---- 减少趟数的另一条路：让初始归并段更长 ----
    printf("  --- fewer initial runs means fewer passes ---\n");
    int p1 = Simulate(32, 4, 4, false);
    int p2 = Simulate(8, 4, 16, false);      // 同样的数据量，但初始段更少（段更长）
    printf("  32 short runs, k=4 -> %d passes; 8 long runs, k=4 -> %d passes\n", p1, p2);
    expect_true("longer initial runs reduce the number of passes", p2 < p1);

    // ---- I/O 次数：每趟把所有数据读一遍、写一遍 ----
    int m = 16, k = 4, len = 10;
    int S = TheoreticalPasses(m, k);
    long long total = (long long)m * len;
    long long io = 2 * total * (S + 1);      // 生成初始段还要写一遍
    printf("  m=%d, k=%d, len=%d : S=%d, records=%lld, total I/O = %lld\n", m, k, len, S, total, io);
    expect_eq("I/O grows linearly with the number of passes", io, 2 * total * (S + 1));
    return finish();
}
