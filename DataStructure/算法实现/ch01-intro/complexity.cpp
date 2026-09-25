// ============================================================================
// 第1章 绪论 —— 复杂度实测：用计数器验证 O 推导，而不是背结论
//   编译运行： g++ -std=c++17 -O0 complexity.cpp -o complexity ; ./complexity
// ============================================================================
#include "../ds_common.h"

// 例1：一重循环                          -> O(n)
long long loop1(int n) { long long c = 0; for (int i = 0; i < n; ++i) ++c; return c; }

// 例2：双重嵌套，内外都满                -> O(n^2)
long long loop2(int n) { long long c = 0; for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) ++c; return c; }

// 例3：内层从 i 开始                     -> O(n^2)，系数 1/2
long long loop3(int n) { long long c = 0; for (int i = 0; i < n; ++i) for (int j = i; j < n; ++j) ++c; return c; }

// 例4：i 每次翻倍                        -> O(log n)
long long loop4(int n) { long long c = 0; for (int i = 1; i < n; i *= 2) ++c; return c; }

// 例5：外层翻倍 + 内层线性               -> O(n log n)（不是 O(n)！）
long long loop5(int n) { long long c = 0; for (int i = 1; i < n; i *= 2) for (int j = 0; j < n; ++j) ++c; return c; }

int main() {
    banner("Complexity : count the real operations");

    // 逐个验证：实测次数 == 公式算出来的次数
    expect_eq("loop1(100)  = n",          loop1(100),  100);
    expect_eq("loop2(100)  = n^2",        loop2(100),  10000);
    expect_eq("loop3(100)  = n(n+1)/2",   loop3(100),  5050);
    expect_eq("loop4(1024) = log2(n)",    loop4(1024), 10);
    expect_eq("loop5(1024) = n*log2(n)",  loop5(1024), 10240);

    // 增长率对比表：看 n 翻倍时操作次数怎么变（这才是"阶"的含义）
    printf("\n      n      loop1      loop2        loop3     loop4       loop5\n");
    const int ns[] = {8, 16, 32, 64};
    for (int k = 0; k < 4; ++k) {
        int n = ns[k];
        printf("  %5d  %8lld  %10lld  %11lld  %8lld  %11lld\n",
               n, loop1(n), loop2(n), loop3(n), loop4(n), loop5(n));
    }
    printf("  n 翻倍 -> loop2 x4 (n^2), loop3 x4 (n^2), loop4 +1 (log n), loop5 x2 略多 (n log n)\n");

    return finish();
}
