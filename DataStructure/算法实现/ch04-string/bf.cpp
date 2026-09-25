// ============================================================================
// 第4章 串 —— BF（朴素）模式匹配 + 比较次数实测
//   考点：
//     * BF 最坏时间 O(n*m)：每次失配主串指针都要回退到"起点 + 1"，前面比过的白比了
//     * 例子：主串 "0000000001"、模式 "00001" —— 这是 BF 的经典最坏情况
//     * KMP 正是为了消除"主串指针回退"才需要 next 数组
// ============================================================================
#include "../ds_common.h"

#define MAXLEN 64

typedef struct {
    char ch[MAXLEN + 1];
    int  length;
} SString;

void StrAssign(SString &S, const char *p) {
    int n = (int)strlen(p);
    S.length = n;
    for (int i = 1; i <= n; ++i) S.ch[i] = p[i - 1];
    S.ch[n + 1] = '\0';
}

// 返回匹配起点（位序，从 1 开始），失败返回 0；比较次数记录在 g_cmp 里
int BFIndex(const SString &S, const SString &T, int pos) {
    int i = pos, j = 1;
    while (i <= S.length && j <= T.length) {
        count_cmp();
        if (S.ch[i] == T.ch[j]) { ++i; ++j; }
        else { i = i - j + 2; j = 1; }
    }
    return (j > T.length) ? i - T.length : 0;
}

int main() {
    banner("BF (naive) pattern matching");
    SString S, T;
    int r = 0;

    StrAssign(S, "data structure");
    StrAssign(T, "struct");
    reset_counters();
    r = BFIndex(S, T, 1);
    expect_eq("found at pos 6", r, 6);
    report_counters("BF(\"data structure\",\"struct\")");

    StrAssign(T, "abc");
    reset_counters();
    expect_eq("not found", BFIndex(S, T, 1), 0);
    report_counters("BF(not found)");

    // ---------------- 最坏情况：主串全 0、模式结尾才是 1 ----------------
    // 每比到最后一个字符才失配 -> 比较次数接近 n*m
    StrAssign(S, "0000000000000000000001");
    StrAssign(T, "00001");                      // m = 5
    reset_counters();
    r = BFIndex(S, T, 1);
    printf("  -> n = %d, m = %d, comparisons = %lld\n", S.length, T.length, g_cmp);
    expect_eq("worst case: match starts at n-4", r, S.length - 4);
    expect_true("comparisons far exceed n (that is the O(n*m) problem)", g_cmp > S.length);

    // 主串越长，比较次数按 n*m 增长
    StrAssign(S, "00000000000000000000000000000000000000001");
    reset_counters();
    r = BFIndex(S, T, 1);
    printf("  -> longer text: n = %d, comparisons = %lld\n", S.length, g_cmp);
    expect_eq("match starts at n-4", r, S.length - 4);
    expect_true("comparisons scale with n*m", g_cmp > 150);

    // 边界
    StrAssign(S, "aaa");
    StrAssign(T, "aaa");
    expect_eq("whole string matches", BFIndex(S, T, 1), 1);
    StrAssign(T, "aaaa");
    expect_eq("pattern longer than text", BFIndex(S, T, 1), 0);
    StrAssign(T, "a");
    expect_eq("single char", BFIndex(S, T, 2), 2);
    return finish();
}
