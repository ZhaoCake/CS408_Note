// ============================================================================
// 第4章 串 —— KMP：next 数组、nextval 数组、匹配主过程
//   索引约定（和考研手算一致）：字符存 ch[1..length]，next/nextval 也用 1..length
//     * next[1] = 0（约定）
//     * next[j] = 前 j-1 个字符组成的串的"最长相等前后缀"长度 + 1
//     * nextval：如果 T[j] == T[next[j]]，则 nextval[j] = nextval[next[j]]，否则就是 next[j]
//   比较次数实测：同一组数据下 KMP 的比较次数不会超过 O(n+m)，BF 会到 O(n*m)
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

// ------------------------------ next 数组 ------------------------------
// i 是"后缀末尾指针"，j 是"前缀末尾指针"（同时代表当前最长相等前后缀长度）
void GetNext(const SString &T, int next[]) {
    int i = 1, j = 0;
    next[1] = 0;
    while (i < T.length) {
        if (j == 0 || T.ch[i] == T.ch[j]) {
            ++i;
            ++j;
            next[i] = j;
        } else {
            j = next[j];             // 失配：j 退到它自己的 next
        }
    }
}

// ------------------------------ nextval 数组 ------------------------------
void GetNextval(const SString &T, int nextval[]) {
    int i = 1, j = 0;
    nextval[1] = 0;
    while (i < T.length) {
        if (j == 0 || T.ch[i] == T.ch[j]) {
            ++i;
            ++j;
            if (T.ch[i] != T.ch[j]) nextval[i] = j;      // 和 next 一样
            else                    nextval[i] = nextval[j]; // 继续往前跳，少比一次
        } else {
            j = nextval[j];
        }
    }
}

// ------------------------------ 匹配主过程 ------------------------------
// 关键：主串指针 i 从不回退，只动模式串指针 j
int IndexKMP(const SString &S, const SString &T, int pos, const int next[]) {
    int i = pos, j = 1;
    while (i <= S.length && j <= T.length) {
        count_cmp();
        if (j == 0 || S.ch[i] == T.ch[j]) { ++i; ++j; }
        else j = next[j];
    }
    if (j > T.length) return i - T.length;
    return 0;
}

// 用来对拍的 BF（主串指针会回退）
int BFIndex(const SString &S, const SString &T, int pos) {
    int i = pos, j = 1;
    while (i <= S.length && j <= T.length) {
        count_cmp();
        if (S.ch[i] == T.ch[j]) { ++i; ++j; }
        else { i = i - j + 2; j = 1; }
    }
    return (j > T.length) ? i - T.length : 0;
}

void PrintTable(const char *tag, const SString &T, const int a[]) {
    printf("  %-16s ", tag);
    for (int i = 1; i <= T.length; ++i) printf("%c ", T.ch[i]);
    printf("\n  %-16s ", "index");
    for (int i = 1; i <= T.length; ++i) printf("%d ", i);
    printf("\n  %-16s ", tag);
    for (int i = 1; i <= T.length; ++i) printf("%d ", a[i]);
    printf("\n");
}

int main() {
    banner("KMP : next / nextval");
    SString T;
    int next[MAXLEN + 1], nextval[MAXLEN + 1];

    // 王道经典例：T = "abaabc" -> next = 0 1 1 2 2 3, nextval = 0 1 0 2 1 3
    StrAssign(T, "abaabc");
    GetNext(T, next);
    GetNextval(T, nextval);
    PrintTable("next", T, next);
    PrintTable("nextval", T, nextval);
    int expNext1[] = {0, 1, 1, 2, 2, 3};
    int expNv1[]   = {0, 1, 0, 2, 1, 3};
    expect_array("next(\"abaabc\")", next + 1, T.length, expNext1, 6);
    expect_array("nextval(\"abaabc\")", nextval + 1, T.length, expNv1, 6);

    // T = "aaab" -> next = 0 1 2 3, nextval = 0 0 0 3
    StrAssign(T, "aaab");
    GetNext(T, next);
    GetNextval(T, nextval);
    PrintTable("next", T, next);
    PrintTable("nextval", T, nextval);
    int expNext2[] = {0, 1, 2, 3};
    int expNv2[]   = {0, 0, 0, 3};
    expect_array("next(\"aaab\")", next + 1, T.length, expNext2, 4);
    expect_array("nextval(\"aaab\")", nextval + 1, T.length, expNv2, 4);

    // 单字符模式串：next[1] = 0
    StrAssign(T, "a");
    GetNext(T, next);
    expect_eq("next of a single char", next[1], 0);

    banner("KMP : matching (must equal BF on every case)");
    SString S;
    const char *texts[] = {
        "data structure",
        "aaabaaaab",
        "ababcabcacbab",
        "acabaabaabcacaabc",
        "aaaaaaaaab",
    };
    const char *pats[] = {"struct", "aaab", "abcac", "abaabc", "aab"};
    for (int k = 0; k < 5; ++k) {
        StrAssign(S, texts[k]);
        StrAssign(T, pats[k]);
        GetNext(T, next);
        int kp = IndexKMP(S, T, 1, next);
        int bf = BFIndex(S, T, 1);
        printf("  pattern \"%s\" in \"%s\" : KMP = %d, BF = %d\n", pats[k], texts[k], kp, bf);
        char label[96];
        snprintf(label, sizeof(label), "KMP == BF for \"%s\"", pats[k]);
        expect_true(label, kp == bf);
    }

    banner("KMP vs BF : comparison counts on the worst case");
    StrAssign(S, "0000000000000000000001");
    StrAssign(T, "00001");
    GetNext(T, next);
    reset_counters();
    int r1 = IndexKMP(S, T, 1, next);
    long long kmpCmp = g_cmp;
    reset_counters();
    int r2 = BFIndex(S, T, 1);
    long long bfCmp = g_cmp;
    printf("  n = %d, m = %d : KMP cmp = %lld, BF cmp = %lld\n", S.length, T.length, kmpCmp, bfCmp);
    expect_eq("same answer", r1, r2);
    expect_true("KMP compares far fewer times", kmpCmp < bfCmp);

    // 主串指针不回退：比较次数近似 n + m
    expect_true("KMP comparisons stay around n+m", kmpCmp <= 2 * (S.length + T.length));
    return finish();
}
