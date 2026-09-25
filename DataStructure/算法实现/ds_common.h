// ============================================================================
// ds_common.h  —  408 数据结构算法实现：公共约定与验收工具
// ----------------------------------------------------------------------------
// 六条约定（每个文件都必须遵守，这样"跑通"才等价于"掌握"）：
//   1. 一个文件一个主题，独立可编译：
//        g++ -std=c++17 -O0 seq-list.cpp -o seq-list
//      批量编译： pwsh scripts/build_ds.ps1 -Run
//   2. 关键操作都走 count_cmp() / count_move()，用实测次数验证复杂度结论
//   3. 过程打印统一用 TRACE(...)，整体关闭：-DDS_TRACE=0
//   4. 练习点用 TODO(你) 标记，配套的 expect_* 已经写在 main 里
//   5. main 返回 finish()，非 0 表示还有没做对的地方 —— 这就是你的进度条
//   6. 代码里的字符串字面量一律 ASCII，避免控制台编码问题；注释用中文
// ============================================================================
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <climits>
#include <ctime>
#include <cmath>       // log2 / sqrt / ceil 等，算法里会用到

// ---------------------------------------------------------------------------
// 过程打印开关：默认开。想只看结果不看过程，编译时加 -DDS_TRACE=0
// ---------------------------------------------------------------------------
#ifndef DS_TRACE
#define DS_TRACE 1
#endif

#define TRACE(...) do { if (DS_TRACE) { printf("  | "); printf(__VA_ARGS__); } } while (0)

// ---------------------------------------------------------------------------
// 操作计数器：用来实测复杂度，而不是"老师说它是 O(n^2)"
//   比较计次口径：每一次关键字比较 count_cmp()
//   移动计次口径：每一次赋值 count_move()，一次交换记 3 次移动
// ---------------------------------------------------------------------------
inline long long g_cmp  = 0;
inline long long g_move = 0;

inline void reset_counters()                 { g_cmp = 0; g_move = 0; }
inline void count_cmp(long long n = 1)       { g_cmp  += n; }
inline void count_move(long long n = 1)      { g_move += n; }
inline void report_counters(const char *tag) {
    printf("  [counters] %-24s cmp=%-8lld move=%lld\n", tag, g_cmp, g_move);
}
inline void swap_counted(int &a, int &b)     { int t = a; a = b; b = t; count_move(3); }

// ---------------------------------------------------------------------------
// 打印工具
// ---------------------------------------------------------------------------
inline void print_array(const char *tag, const int a[], int n) {
    printf("  %-16s [", tag);
    for (int i = 0; i < n; ++i) { if (i) printf(", "); printf("%d", a[i]); }
    printf("]\n");
}
inline void print_range(const char *tag, const int a[], int l, int r) {   // 闭区间 [l, r]
    printf("  %-16s [", tag);
    for (int i = l; i <= r; ++i) { if (i > l) printf(", "); printf("%d", a[i]); }
    printf("]\n");
}

// ---------------------------------------------------------------------------
// 验收工具：每个 expect_* 通过打印 [ok]，失败打印 [FAIL] 并把失败数 +1
// ---------------------------------------------------------------------------
inline int &fail_count() { static int f = 0; return f; }

inline void banner(const char *title) {
    printf("\n================ %s ================\n", title);
}

inline void expect_true(const char *what, bool cond) {
    if (cond) printf("  [ok]   %s\n", what);
    else { ++fail_count(); printf("  [FAIL] %s\n", what); }
}

inline void expect_eq(const char *what, long long got, long long want) {
    if (got == want) printf("  [ok]   %-34s = %lld\n", what, got);
    else { ++fail_count(); printf("  [FAIL] %-34s got %lld, want %lld\n", what, got, want); }
}

inline void expect_str(const char *what, const char *got, const char *want) {
    if (strcmp(got, want) == 0) printf("  [ok]   %-34s = \"%s\"\n", what, got);
    else { ++fail_count(); printf("  [FAIL] %-34s got \"%s\", want \"%s\"\n", what, got, want); }
}

inline void expect_array(const char *what, const int got[], int gotN, const int want[], int wantN) {
    bool ok = (gotN == wantN);
    if (ok) for (int i = 0; i < gotN; ++i) if (got[i] != want[i]) { ok = false; break; }
    if (ok) {
        printf("  [ok]   %-34s = [", what);
        for (int i = 0; i < gotN; ++i) { if (i) printf(", "); printf("%d", got[i]); }
        printf("]\n");
        return;
    }
    ++fail_count();
    printf("  [FAIL] %-34s\n", what);
    printf("         got  [");
    for (int i = 0; i < gotN; ++i) { if (i) printf(", "); printf("%d", got[i]); }
    printf("]\n         want [");
    for (int i = 0; i < wantN; ++i) { if (i) printf(", "); printf("%d", want[i]); }
    printf("]\n");
}

// main 的最后一行：return finish();
inline int finish() {
    if (fail_count() == 0) {
        printf("\n[ALL PASS] this file is done -> now close it and blind-write it\n");
        return 0;
    }
    printf("\n[%d CHECK(S) FAILED] red = TODO not done yet\n", fail_count());
    return 1;
}
