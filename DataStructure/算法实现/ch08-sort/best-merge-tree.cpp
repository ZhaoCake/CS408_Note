// ============================================================================
// 第8章 排序 —— 外部排序：最佳归并树【经典算法】
//   考点：
//     * 最佳归并树 = k 叉哈夫曼树：让长度不同的归并段以"最优"的顺序归并
//     * 树的 WPL（带权路径长度）= 每个归并段长度 × 其归并层数之和
//       = 总记录移动次数；读写次数还要再乘 2（读一次 + 写一次）
//     * k 叉哈夫曼树的构造：每次合并 k 个最小的；为了让每次都能凑满 k 个，
//       需要补"长度为 0 的虚段"：当 (m-1) % (k-1) != 0 时，补 (k-1) - (m-1)%(k-1) 个
//     * 第一趟只合并 (m-1) % (k-1) + 1 个结点，之后每趟都合并 k 个
// ============================================================================
#include "../ds_common.h"

#define MAXN 64

// k 叉哈夫曼：求 WPL（总移动次数）；seg[] 是各归并段的长度
long long KWayHuffmanWPL(long long seg[], int m, int k, bool trace) {
    if (m <= 1) return 0;
    // 1) 补齐虚段：让 (m-1) 能被 (k-1) 整除
    int virtualSeg = 0;
    if ((m - 1) % (k - 1) != 0) virtualSeg = (k - 1) - (m - 1) % (k - 1);
    int total = m + virtualSeg;
    long long a[MAXN];
    for (int i = 0; i < m; ++i) a[i] = seg[i];
    for (int i = 0; i < virtualSeg; ++i) a[m + i] = 0;         // 虚段长度为 0
    for (int i = 0; i < total - 1; ++i)                        // 排成升序，方便每次取最小的
        for (int j = 0; j < total - 1 - i; ++j)
            if (a[j] > a[j + 1]) { long long t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; }

    if (trace) {
        printf("  segments = %d, k = %d, virtual segments needed = %d (total = %d)\n",
               m, k, virtualSeg, total);
    }

    long long wpl = 0;
    int n = total;
    int round = 0;
    while (n > 1) {
        int take = k;
        // 第一趟（n 已经是补过虚段后的规模）如果 n-1 不能整除则仍按规则处理；
        // 由于已经补过虚段，这里 n-1 一定整除，直接每次取 k 个
        if (n - 1 < k) take = n;                               // 兜底（正常不会触发）
        long long sum = 0;
        for (int i = 0; i < take; ++i) sum += a[i];
        wpl += sum;
        if (trace) {
            printf("  round %d: merge %d segments (", ++round, take);
            for (int i = 0; i < take; ++i) printf("%lld%s", a[i], i + 1 < take ? " + " : "");
            printf(") = %lld\n", sum);
        }
        // 把剩下的元素前移，新的合并结果插到有序位置
        int rest = 0;
        for (int i = take; i < n; ++i) a[rest++] = a[i];
        n = rest;
        int pos = n;
        for (int i = 0; i < n; ++i) if (a[i] > sum) { pos = i; break; }
        for (int i = n; i > pos; --i) a[i] = a[i - 1];
        a[pos] = sum;
        ++n;
    }
    return wpl;
}

// 对照：不合并虚段的"错误做法"（每次只取能取到的个数），WPL 会更大
long long GreedyWPL(long long seg[], int m, int k) {
    long long a[MAXN];
    for (int i = 0; i < m; ++i) a[i] = seg[i];
    int n = m;
    long long wpl = 0;
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - 1 - i; ++j)
            if (a[j] > a[j + 1]) { long long t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; }
    while (n > 1) {
        int take = (n < k) ? n : k;
        long long sum = 0;
        for (int i = 0; i < take; ++i) sum += a[i];
        wpl += sum;
        int rest = 0;
        for (int i = take; i < n; ++i) a[rest++] = a[i];
        n = rest;
        a[n++] = sum;
        for (int i = n - 1; i > 0 && a[i - 1] > a[i]; --i) { long long t = a[i - 1]; a[i - 1] = a[i]; a[i] = t; }
    }
    return wpl;
}

int main() {
    banner("External sort : best merge tree (k-way Huffman)");

    // 王道经典例：9 个归并段，长度如下，k = 3
    long long seg[] = {9, 30, 12, 18, 3, 17, 2, 6, 24};
    const int m = 9;
    const int k = 3;

    printf("  --- k = 3, 9 segments ---\n");
    long long wpl = KWayHuffmanWPL(seg, m, k, true);
    printf("  WPL (total records moved) = %lld\n", wpl);
    printf("  total I/O (read + write) = 2 * WPL = %lld\n", 2 * wpl);

    // (9-1) % (3-1) == 0 -> 不需要补虚段
    expect_true("no virtual segment is needed here", ((m - 1) % (k - 1)) == 0);
    expect_eq("WPL == 223", wpl, 223);
    expect_eq("total read+write == 446", 2 * wpl, 446);
    // 每个归并段都要读一次：非叶结点权值之和 = WPL = 所有段的读取总量
    long long sumSeg = 0;
    for (int i = 0; i < m; ++i) sumSeg += seg[i];
    long long totalLen = 0;
    for (int i = 0; i < m; ++i) totalLen += seg[i];
    expect_eq("root weight == sum of all segments", 121, sumSeg);
    expect_true("WPL >= sum of segment lengths", wpl >= sumSeg);

    // ---- 对比：k = 2（二路归并）的 WPL ----
    printf("  --- k = 2 (binary merge) ---\n");
    long long wpl2 = KWayHuffmanWPL(seg, m, 2, false);
    printf("  WPL with k = 2 : %lld, total I/O = %lld\n", wpl2, 2 * wpl2);
    expect_true("more merge passes -> larger WPL", wpl2 > wpl);
    // 二路哈夫曼的趟数：9 段 -> 8 次合并

    // ---- 需要补虚段的例子：m = 5, k = 3 ----
    long long seg2[] = {5, 10, 4, 8, 3};
    printf("  --- k = 3, 5 segments (needs virtual segments) ---\n");
    long long wpl3 = KWayHuffmanWPL(seg2, 5, 3, true);
    printf("  WPL = %lld\n", wpl3);
    // (5-1) % 2 = 0 -> 其实也不需要补；换成 4 段再看
    long long seg3[] = {5, 10, 4, 8};
    printf("  --- k = 3, 4 segments ---\n");
    long long wpl4 = KWayHuffmanWPL(seg3, 4, 3, true);
    printf("  WPL = %lld\n", wpl4);
    expect_true("(m-1) % (k-1) != 0 -> one virtual segment is added", (4 - 1) % (3 - 1) != 0);
    expect_eq("segment lengths sum", totalLen, 121);
    // 手工验证：补 1 个长度 0 的虚段 -> {0,4,5,8,10}，第一趟合并 3 个最小的 {0,4,5} = 9
    //           然后 {8,9,10} 合并 = 27 -> WPL = 9 + 27 = 36
    expect_eq("WPL for 4 segments with k=3 (one virtual)", wpl4, 36);

    // 等价性验证：WPL 一定等于"所有非叶结点权值之和"
    // 用同一组段做 k=2 的验证：手算 9 段二路哈夫曼的 WPL
    long long a[9];
    for (int i = 0; i < 9; ++i) a[i] = seg[i];
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8 - i; ++j)
            if (a[j] > a[j + 1]) { long long t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; }
    long long manual = 0;
    int n = 9;
    while (n > 1) {
        long long s = a[0] + a[1];
        manual += s;
        int rest = 0;
        for (int i = 2; i < n; ++i) a[rest++] = a[i];
        n = rest;
        int pos = n;
        for (int i = 0; i < n; ++i) if (a[i] > s) { pos = i; break; }
        for (int i = n; i > pos; --i) a[i] = a[i - 1];
        a[pos] = s;
        ++n;
    }
    expect_eq("k=2 WPL matches the manual Huffman computation", wpl2, manual);

    // 最优性：补虚段的做法不会比"贪心不补"更差
    long long greedy = GreedyWPL(seg3, 4, 3);
    printf("  with/without virtual segment: %lld vs %lld\n", wpl4, greedy);
    expect_true("proper virtual padding is never worse", wpl4 <= greedy);

    // 三个段的特殊情况：k=3 时一趟就搞定
    long long seg4[] = {3, 5, 7};
    expect_eq("3 segments, k=3 -> single merge", KWayHuffmanWPL(seg4, 3, 3, false), 15);
    return finish();
}
