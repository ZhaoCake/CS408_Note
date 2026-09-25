// ============================================================================
// 第8章 排序 —— 外部排序：置换-选择排序（生成初始归并段）【经典算法】
//   考点：
//     * 目的：让初始归并段尽可能长，从而减少段数 m -> 减少归并趟数 ceil(log_k m)
//     * 做法：内存工作区容量固定为 w，反复"从工作区里挑一个 >= 上次输出值的最小数"输出；
//             挑不出来就结束当前段，开始新段（此时工作区里剩下的都是"比上次输出小"的元素）
//     * 结论：平均情况下段长可达工作区容量的 2 倍（这就是它比"内部排序+顺序分段"强的地方）
//     * 手工题要给出一趟一趟的输出，其实和下面的模拟完全一致
// ============================================================================
#include "../ds_common.h"

#define MAXW 64
#define MAXN 64                       // 段数 × 段长，够用即可（RunSet 会放在静态存储里）
#define NEG -1000000

typedef struct { int runs[MAXN][MAXN]; int len[MAXN]; int cnt; } RunSet;

// input[] 是待排序序列，n 是记录数，w 是内存工作区容量
// 返回产生的初始归并段个数；每段内容放在 R 里
int ReplaceSelect(const int input[], int n, int w, RunSet &R, bool trace) {
    int work[MAXW];
    int inPos = 0, workSize = 0;
    R.cnt = 0;

    // 1) 先把工作区填满
    while (workSize < w && inPos < n) work[workSize++] = input[inPos++];

    int curLen = 0;
    int lastOut = NEG;                                   // 当前段的上一个输出值
    R.len[0] = 0;

    while (workSize > 0) {
        // 2) 在工作区里找"最小且 >= lastOut"的元素
        int best = -1;
        for (int i = 0; i < workSize; ++i) {
            if (work[i] < lastOut) continue;
            if (best == -1 || work[i] < work[best]) best = i;
        }

        if (best == -1) {
            // 3) 找不到了 -> 本段结束，开始新段
            if (trace) printf("  run %d finished with %d records\n", R.cnt, curLen);
            ++R.cnt;
            R.len[R.cnt] = 0;
            curLen = 0;
            lastOut = NEG;                               // 新段可以从头开始挑
            continue;
        }

        // 4) 输出它，并用输入里的下一条记录补上这个空位
        int val = work[best];
        if (trace) printf("    output %d (work area slot %d)\n", val, best);
        R.runs[R.cnt][curLen++] = val;
        R.len[R.cnt] = curLen;
        lastOut = val;

        if (inPos < n) work[best] = input[inPos++];       // 补新记录
        else {                                            // 输入空了 -> 移除该位置
            work[best] = work[workSize - 1];
            workSize--;
        }
    }
    if (curLen > 0) {
        if (trace) printf("  run %d finished with %d records\n", R.cnt, curLen);
        ++R.cnt;
    }
    return R.cnt;
}

int main() {
    banner("External sort : replacement selection (generate initial runs)");

    // 经典的 24 个记录、工作区容量 4 的例子
    int input[] = {24, 15, 30, 36, 25, 20, 8, 12, 5, 18, 26, 22, 6, 28, 16, 35,
                   3, 10, 21, 40, 33, 14, 9, 27};
    const int n = 24;
    const int w = 4;

    printf("  input (%d records): ", n);
    for (int i = 0; i < n; ++i) printf("%d ", input[i]);
    printf("\n  work area size w = %d\n", w);

    static RunSet R;                       // 结构比较大，放静态区
    int runs = ReplaceSelect(input, n, w, R, true);
    printf("  generated %d initial runs:\n", runs);
    for (int i = 0; i < runs; ++i) {
        printf("    run %d (len %2d): ", i + 1, R.len[i]);
        for (int j = 0; j < R.len[i]; ++j) printf("%d ", R.runs[i][j]);
        printf("\n");
    }

    // ---- 校验 1：每个段内部必须有序 ----
    int sortedOk = 1;
    for (int i = 0; i < runs; ++i)
        for (int j = 1; j < R.len[i]; ++j)
            if (R.runs[i][j] < R.runs[i][j - 1]) sortedOk = 0;
    expect_true("every generated run is internally sorted", sortedOk != 0);

    // ---- 校验 2：所有段合起来恰好是原来的记录（不丢不多） ----
    int total = 0;
    for (int i = 0; i < runs; ++i) total += R.len[i];
    expect_eq("all records are kept", total, n);

    int sumIn = 0, sumOut = 0;
    for (int i = 0; i < n; ++i) sumIn += input[i];
    for (int i = 0; i < runs; ++i) for (int j = 0; j < R.len[i]; ++j) sumOut += R.runs[i][j];
    expect_eq("the multiset of values is unchanged", sumOut, sumIn);

    // ---- 校验 3：第一个输出应该是"工作区里最小的那个"
    //      （注意不是全局最小值：全局最小值可能还没读进内存）----
    int minInWork = input[0];
    for (int i = 1; i < w; ++i) if (input[i] < minInWork) minInWork = input[i];
    printf("  first output = %d, minimum of the first w records = %d\n", R.runs[0][0], minInWork);
    expect_eq("the first output is the minimum inside the work area", R.runs[0][0], minInWork);

    // 全程最小值迟早会被输出（它在所有记录里最小）
    int globalMin = input[0], foundMin = 0;
    for (int i = 1; i < n; ++i) if (input[i] < globalMin) globalMin = input[i];
    for (int i = 0; i < runs; ++i)
        for (int j = 0; j < R.len[i]; ++j) if (R.runs[i][j] == globalMin) foundMin = 1;
    expect_true("the global minimum does appear somewhere", foundMin != 0);

    // ---- 校验 4：段数比"直接按工作区切段"更少，这才是置换-选择的价值 ----
    double avg = total / (double)runs;
    printf("  average run length = %.2f (work area = %d)\n", avg, w);
    printf("  naive split would give ceil(24/4) = 6 runs of length 4\n");
    expect_true("fewer runs than the naive split", runs < 6);
    expect_true("average run length is longer than w", avg > w);

    // ---- 对比：工作区越大，段越少 ----
    printf("  --- effect of the work area size ---\n");
    for (int ww = 2; ww <= 8; ww += 2) {
        static RunSet R2;
        int r2 = ReplaceSelect(input, n, ww, R2, false);
        printf("  w = %d -> %d runs, average length = %.2f\n", ww, r2, n / (double)r2);
    }
    static RunSet R3, R4;
    int r3 = ReplaceSelect(input, n, 2, R3, false);
    int r4 = ReplaceSelect(input, n, 8, R4, false);
    expect_true("a bigger work area produces fewer runs", r4 < r3);

    // ---- 已排序的输入：置换-选择排序只需一段（最好情况）----
    int asc[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    static RunSet R5;
    int r5 = ReplaceSelect(asc, 12, 4, R5, false);
    printf("  already sorted input -> %d run(s) of length %d\n", r5, R5.len[0]);
    expect_eq("sorted input gives a single run", r5, 1);
    expect_eq("that run contains everything", R5.len[0], 12);

    // ---- 逆序输入：最坏情况，每段长度只有工作区那么大 ----
    int desc[] = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    static RunSet R6;
    int r6 = ReplaceSelect(desc, 12, 4, R6, false);
    printf("  reverse-sorted input -> %d runs\n", r6);
    expect_eq("worst case: the first run has length w", R6.len[0], 4);
    expect_true("worst case still needs more than one run", r6 > 1);
    expect_true("worst case has more runs than the best case", r6 > r5);

    // ---- 单元素与空输入的边界 ----
    int one[] = {7};
    static RunSet R7;
    expect_eq("single record -> 1 run", ReplaceSelect(one, 1, 4, R7, false), 1);
    expect_eq("that run has 1 record", R7.len[0], 1);
    return finish();
}
