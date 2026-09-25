// ============================================================================
// 第4章 串 —— 用代码生成 next / nextval 表，反过来校对笔记里的手算结果
//   用法：手算完一个模式串的 next，来这里看程序算出来的值，
//         不一致就说明手算错了（程序按定义暴力实现，几乎不可能错）
//   手算口诀：
//     * next[j] = "前 j-1 个字符"的最长相等前后缀长度 + 1，next[1] = 0
//     * nextval[j]：若 T[j] == T[next[j]] 则取 nextval[next[j]]，否则就是 next[j]
// ============================================================================
#include "../ds_common.h"

#define MAXLEN 64

typedef struct { char ch[MAXLEN + 1]; int length; } SString;

void StrAssign(SString &S, const char *p) {
    int n = (int)strlen(p);
    S.length = n;
    for (int i = 1; i <= n; ++i) S.ch[i] = p[i - 1];
    S.ch[n + 1] = '\0';
}

void GetNext(const SString &T, int next[]) {
    int i = 1, j = 0;
    next[1] = 0;
    while (i < T.length) {
        if (j == 0 || T.ch[i] == T.ch[j]) { ++i; ++j; next[i] = j; }
        else j = next[j];
    }
}

void GetNextval(const SString &T, int nextval[]) {
    int i = 1, j = 0;
    nextval[1] = 0;
    while (i < T.length) {
        if (j == 0 || T.ch[i] == T.ch[j]) {
            ++i; ++j;
            nextval[i] = (T.ch[i] != T.ch[j]) ? j : nextval[j];
        } else {
            j = nextval[j];
        }
    }
}

// 另一种独立求法：暴力求"最长相等前后缀长度"，用来交叉验证上面的递推写法
int BrutePrefixLen(const SString &T, int j) {          // 前 j-1 个字符的最长相等前后缀长度
    int m = j - 1, best = 0;
    for (int len = 1; len < m; ++len) {
        int ok = 1;
        for (int k = 1; k <= len; ++k)
            if (T.ch[k] != T.ch[m - len + k]) { ok = 0; break; }
        if (ok) best = len;
    }
    return best;
}

void PrintTable(const SString &T, const int next[], const int nextval[]) {
    printf("  %-14s ", "pattern");
    for (int i = 1; i <= T.length; ++i) printf("%c ", T.ch[i]);
    printf("\n  %-14s ", "j");
    for (int i = 1; i <= T.length; ++i) printf("%d ", i);
    printf("\n  %-14s ", "next");
    for (int i = 1; i <= T.length; ++i) printf("%d ", next[i]);
    printf("\n  %-14s ", "nextval");
    for (int i = 1; i <= T.length; ++i) printf("%d ", nextval[i]);
    printf("\n");
}

int main() {
    banner("Verify next / nextval by code (compare with your hand-written table)");

    // 每个模式串配一份"手算应该得到的结果"，跑通说明两者一致
    const char *pats[]  = {"abaabc", "aaab", "ababaa", "abcac", "aaaaa"};
    const char *expNext[5] = {
        "0 1 1 2 2 3",
        "0 1 2 3",
        "0 1 1 2 3 4",
        "0 1 1 1 2",
        "0 1 2 3 4",
    };
    const char *expNv[5] = {
        "0 1 0 2 1 3",
        "0 0 0 3",
        "0 1 0 1 0 4",
        "0 1 1 0 2",
        "0 0 0 0 0",
    };

    for (int k = 0; k < 5; ++k) {
        SString T;
        int next[MAXLEN + 1], nextval[MAXLEN + 1];
        StrAssign(T, pats[k]);
        GetNext(T, next);
        GetNextval(T, nextval);
        PrintTable(T, next, nextval);

        // 手工把期望表拼成字符串，和程序结果逐位比对
        char buf[MAXLEN * 3];
        int p = 0;
        for (int i = 1; i <= T.length; ++i) {
            if (i > 1) buf[p++] = ' ';
            p += snprintf(buf + p, sizeof(buf) - p, "%d", next[i]);
        }
        buf[p] = '\0';
        char label[96];
        snprintf(label, sizeof(label), "next(\"%s\")", pats[k]);
        expect_str(label, buf, expNext[k]);

        p = 0;
        for (int i = 1; i <= T.length; ++i) {
            if (i > 1) buf[p++] = ' ';
            p += snprintf(buf + p, sizeof(buf) - p, "%d", nextval[i]);
        }
        buf[p] = '\0';
        snprintf(label, sizeof(label), "nextval(\"%s\")", pats[k]);
        expect_str(label, buf, expNv[k]);

        // 交叉验证：next[j] 应该等于"前 j-1 个字符的最长相等前后缀长度 + 1"
        int same = 1;
        for (int j = 2; j <= T.length; ++j)
            if (next[j] != BrutePrefixLen(T, j) + 1) { same = 0; break; }
        snprintf(label, sizeof(label), "next(\"%s\") matches the definition", pats[k]);
        expect_true(label, same != 0);
    }
    return finish();
}
