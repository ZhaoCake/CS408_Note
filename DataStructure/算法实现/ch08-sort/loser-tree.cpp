// ============================================================================
// 第8章 排序 —— 外部排序：败者树【经典算法】
//   考点：
//     * k 路归并时，每次要从 k 个段的当前元素里选最小的：
//         线性扫描要比较 k-1 次，总共 O((k-1) * n)；
//         败者树只要沿"叶子到根"的路径重赛，每次 O(log2 k) 次比较
//     * 败者树的内部结点存【败者】，冠军单独存在 ls[0]；这样每次只需沿着
//       一条路径往上重赛即可（胜者树存胜者，也必须重赛一遍路径，但败者树的
//       实现更省：新元素一路上只需和"败者"比）
//     * 和"堆"的关系：败者树是 k 路归并的专用结构，功能和堆类似但更省比较
// ============================================================================
#include "../ds_common.h"

#define K 8                  // 归并路数
#define MAXV 999999
#define INF 1000000000
#define MAXN_SAFE 256

static int cur[K];           // 每个归并段的当前元素
static int segPos[K];        // 每个段读到第几个了
static int segLen[K];        // 每个段的长度
static int *segData[K];      // 每个段的数组

// 虚拟段（哨兵）：值取 -INF，保证它在早期比赛里总是"赢"，
// 这样逐段插入时，先插入的段会被正确地下放到败者位置
#define SENT K
static int Val(int s) { return (s == SENT) ? (-INF) : cur[s]; }

static int loser[K];         // loser[1..K-1] 存败者段号
static int winner;           // 最终胜者（段号）
static long long g_loserCmp; // 注意：不要叫 g_cmp，ds_common.h 里已经有同名的全局计数器

// 返回两个段里的胜者（值小的）
static int Play(int a, int b) {
    ++g_loserCmp;
    return (Val(a) <= Val(b)) ? a : b;
}
// 段 s 换了一个新元素，沿着叶子到根的路径重赛
static void Adjust(int s) {
    for (int node = (K + s) / 2; node >= 1; node /= 2) {
        int w = Play(s, loser[node]);
        loser[node] = (w == s) ? loser[node] : s;      // 败者留在结点上
        s = w;
    }
    winner = s;
}

// 建树：所有内部结点先指向虚拟段，再逐个把真实段插进去
static void BuildLoserTree() {
    for (int i = 0; i < K; ++i) loser[i] = SENT;
    for (int s = K - 1; s >= 0; --s) Adjust(s);
}

// 用败者树做 k 路归并
static int LoserTreeMerge(int out[]) {
    g_loserCmp = 0;
    int remain = 0;
    for (int i = 0; i < K; ++i) {
        segPos[i] = 0;
        cur[i] = segData[i][0];
        remain += segLen[i];
    }
    BuildLoserTree();                                  // 注意：先填 cur[] 再建树
    int n = 0;
    while (remain > 0) {                               // 用剩余记录数管理终止条件
        int s = winner;
        out[n++] = cur[s];
        --remain;
        if (++segPos[s] < segLen[s]) cur[s] = segData[s][segPos[s]];
        else                         cur[s] = INF;      // 该段读完 -> 值设成 +INF，之后必败
        Adjust(s);                                      // 只沿一条路径重赛
    }
    return n;
}

// 对照：线性扫描选最小（每次比较 k-1 次）
static int LinearMerge(int out[]) {
    int idx[K];
    for (int i = 0; i < K; ++i) idx[i] = 0;
    int n = 0;
    long long cmp = 0;
    while (true) {
        int best = -1;
        for (int i = 0; i < K; ++i) {
            if (idx[i] >= segLen[i]) continue;
            if (best == -1) { best = i; continue; }
            ++cmp;
            if (segData[i][idx[i]] < segData[best][idx[best]]) best = i;
        }
        if (best == -1) break;
        out[n++] = segData[best][idx[best]++];
    }
    return n;
}

int main() {
    banner("External sort : loser tree (k-way merge)");
    // 造 K 个有序段：第 i 段的元素满足 值 ≡ i (mod K)，这样各个段交错
    static int data[K][32];
    int total = 0;
    const int len = 8;
    for (int i = 0; i < K; ++i) {
        segData[i] = data[i];
        segLen[i] = len;
        for (int j = 0; j < len; ++j) data[i][j] = i + 1 + j * K;   // 每段内部递增
        segPos[i] = 0;
        cur[i] = data[i][0];
        total += len;
    }
    printf("  %d runs of length %d, total %d records\n", K, len, total);

    // ---- 败者树归并 ----
    static int out1[MAXN_SAFE];
    int n1 = LoserTreeMerge(out1);
    long long cmpLoser = g_loserCmp;
    expect_eq("loser tree outputs all records", n1, total);
    int sorted = 1;
    for (int i = 1; i < n1; ++i) if (out1[i] < out1[i - 1]) sorted = 0;
    expect_true("loser tree merge produces a sorted stream", sorted != 0);
    printf("  loser tree comparisons = %lld (%.2f per record)\n", cmpLoser, cmpLoser / (double)total);
    // k = 8 -> 路径长 3，还有建树的 O(k) 一次性开销
    expect_true("comparisons per record are O(log k) ~ 3", cmpLoser / (double)total <= 5);

    // ---- 线性扫描归并（对照）----
    static int out2[MAXN_SAFE];
    int n2 = LinearMerge(out2);
    expect_eq("linear merge outputs all records", n2, total);
    // 线性方法的比较次数 = (k-1) 量级 * 记录数
    long long cmpLinear = 0;
    {   // 重新算一遍并统计
        int idx[K] = {0};
        int n = 0;
        while (true) {
            int best = -1;
            for (int i = 0; i < K; ++i) {
                if (idx[i] >= segLen[i]) continue;
                if (best == -1) { best = i; continue; }
                ++cmpLinear;
                if (data[i][idx[i]] < data[best][idx[best]]) best = i;
            }
            if (best == -1) break;
            ++idx[best];
            ++n;
        }
    }
    printf("  linear scan comparisons = %lld (%.2f per record)\n", cmpLinear, cmpLinear / (double)total);
    expect_true("loser tree beats the linear scan", cmpLoser < cmpLinear);

    // 结果必须完全一致
    int same = (n1 == n2);
    for (int i = 0; same && i < n1; ++i) if (out1[i] != out2[i]) same = 0;
    expect_true("both methods produce the same stream", same != 0);

    // ---- 阶段一：只从最小的段流出，比较次数不应该爆掉 ----
    printf("  --- k way fan-in trend ---\n");
    const int ks[] = {2, 4, 8};
    for (int t = 0; t < 3; ++t) {
        int kk = ks[t];
        // 用长度为 1 的 k 个段做一次归并，展示"每输出一个元素的比较次数"
        long long worst = 0;
        for (int start = 0; start < kk; ++start) {
            // 路径长度 = ceil(log2 k)
            int nodes = 0;
            for (int node = (kk + start) / 2; node >= 1; node /= 2) ++nodes;
            if (nodes > worst) worst = nodes;
        }
        printf("  k = %d : path length (comparisons per output) = %lld, linear would be %d\n",
               kk, worst, kk - 1);
        if (kk == 8) expect_eq("k=8 -> 3 comparisons per output", worst, 3);
        if (kk == 2) expect_eq("k=2 -> 1 comparison per output", worst, 1);
    }

    // ---- 动态调整：一段读完就把它设成 +INF，之后自动让位 ----
    printf("  --- after an exhausted run, its value becomes +INF ---\n");
    for (int i = 0; i < K; ++i) { segPos[i] = 0; cur[i] = data[i][0]; }
    BuildLoserTree();                                  // cur[] 已经填好，可以建树
    int firstFew[8];
    for (int i = 0; i < 8; ++i) {
        firstFew[i] = cur[winner];
        int s = winner;
        if (++segPos[s] < segLen[s]) cur[s] = segData[s][segPos[s]];
        else                         cur[s] = INF;
        Adjust(s);
    }
    printf("  first 8 outputs:");
    for (int i = 0; i < 8; ++i) printf(" %d", firstFew[i]);
    printf("\n");
    expect_eq("the smallest element comes first", firstFew[0], 1);
    expect_true("outputs are non-decreasing", firstFew[1] >= firstFew[0]);
    return finish();
}
