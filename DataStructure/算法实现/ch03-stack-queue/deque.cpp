// ============================================================================
// 第3章 栈、队列和数组 —— 双端队列【经典算法】
//   只留算法：判定"给定输出序列能否由双端队列得到"，不写双端队列的基本操作演示
//     * 输入受限：只允许在一端插入，两端都可以删除
//     * 输出受限：两端都可以插入，只允许在一端删除
//   这类题手算很容易漏解，这里用穷举给出确定答案，还能直接列出全部可达序列
// ============================================================================
#include "../ds_common.h"
#include <algorithm>

// 算法内部需要的双端队列（最小接口）
#define DQMAX 16
typedef struct { int data[DQMAX]; int front, rear, count; } Deque;
static void DInit(Deque &D) { D.front = D.rear = 0; D.count = 0; }
static bool DEmpty(const Deque &D) { return D.count == 0; }
static void DPushBack(Deque &D, int x)  { D.data[D.rear] = x; D.rear = (D.rear + 1) % DQMAX; ++D.count; }
static void DPushFront(Deque &D, int x) { D.front = (D.front - 1 + DQMAX) % DQMAX; D.data[D.front] = x; ++D.count; }
static bool DPopFront(Deque &D, int &x) { if (DEmpty(D)) return false; x = D.data[D.front]; D.front = (D.front + 1) % DQMAX; --D.count; return true; }
static bool DPopBack(Deque &D, int &x)  { if (DEmpty(D)) return false; D.rear = (D.rear - 1 + DQMAX) % DQMAX; x = D.data[D.rear]; --D.count; return true; }
static bool DFront(const Deque &D, int &x) { if (DEmpty(D)) return false; x = D.data[D.front]; return true; }
static bool DBack(const Deque &D, int &x)  { if (DEmpty(D)) return false; x = D.data[(D.rear - 1 + DQMAX) % DQMAX]; return true; }

static int g_out[DQMAX];
static int g_n;

// ============================================================================
// 1) 输入受限双端队列：只能从队尾入队，队头/队尾都能出队
//    每步三选一：①下一个数从队尾入队 ②队头出队 ③队尾出队
// ============================================================================
static bool dfsInputRestricted(Deque q, int nextIn, int k) {
    if (k == g_n) return true;
    if (nextIn <= g_n) {                                  // ① 入队
        Deque q2 = q;
        DPushBack(q2, nextIn);
        if (dfsInputRestricted(q2, nextIn + 1, k)) return true;
    }
    if (!DEmpty(q)) {
        int f = 0, b = 0;
        DFront(q, f);
        DBack(q, b);
        if (f == g_out[k]) {                              // ② 队头出
            Deque q2 = q;
            int t = 0;
            DPopFront(q2, t);
            if (dfsInputRestricted(q2, nextIn, k + 1)) return true;
        }
        if (b == g_out[k]) {                              // ③ 队尾出
            Deque q2 = q;
            int t = 0;
            DPopBack(q2, t);
            if (dfsInputRestricted(q2, nextIn, k + 1)) return true;
        }
    }
    return false;
}

bool CheckInputRestricted(const int out[], int n) {
    Deque q;
    DInit(q);
    g_n = n;
    for (int i = 0; i < n; ++i) g_out[i] = out[i];
    return dfsInputRestricted(q, 1, 0);
}

// ============================================================================
// 2) 输出受限双端队列：两端都能入队，只能从队头出队
//    每步三选一：①下一个数从队头入队 ②从队尾入队 ③队头出队
// ============================================================================
static bool dfsOutputRestricted(Deque q, int nextIn, int k) {
    if (k == g_n) return true;
    if (nextIn <= g_n) {
        Deque q1 = q;
        DPushFront(q1, nextIn);
        if (dfsOutputRestricted(q1, nextIn + 1, k)) return true;
        Deque q2 = q;
        DPushBack(q2, nextIn);
        if (dfsOutputRestricted(q2, nextIn + 1, k)) return true;
    }
    if (!DEmpty(q)) {
        int f = 0;
        DFront(q, f);
        if (f == g_out[k]) {                              // 只能从队头出
            Deque q2 = q;
            int t = 0;
            DPopFront(q2, t);
            if (dfsOutputRestricted(q2, nextIn, k + 1)) return true;
        }
    }
    return false;
}

bool CheckOutputRestricted(const int out[], int n) {
    Deque q;
    DInit(q);
    g_n = n;
    for (int i = 0; i < n; ++i) g_out[i] = out[i];
    return dfsOutputRestricted(q, 1, 0);
}

// ============================================================================
// 3) 枚举 1..n 的全部输出序列，标出哪些可达 —— 直接和手算结果对照
// ============================================================================
void EnumAllOutputs(int n, bool inputRestricted) {
    int p[DQMAX];
    for (int i = 0; i < n; ++i) p[i] = i + 1;
    int total = 0, ok = 0;
    printf("\n  [%s restricted deque] n = %d\n",
           inputRestricted ? "input" : "output", n);
    do {
        ++total;
        bool reachable = inputRestricted ? CheckInputRestricted(p, n) : CheckOutputRestricted(p, n);
        if (reachable) {
            ++ok;
            printf("    ");
            for (int i = 0; i < n; ++i) printf("%d", p[i]);
            printf("%s", (ok % 6 == 0) ? "\n" : "   ");
        }
    } while (std::next_permutation(p, p + n));
    if (ok % 6 != 0) printf("\n");
    printf("  -> %d / %d permutations are reachable\n", ok, total);
}

int main() {
    banner("Deque : which output sequences are possible?");

    // 输入受限（只有队尾能入队，两端都能出队），输入 1..4
    // 想先输出 4，只能把 1,2,3,4 全压进去再从队尾弹 4；此后队列固定是 [1,2,3]，
    // 2 既不在队头也不在队尾 -> 任何 {4,2,...} 开头的序列都不可能
    int i1[] = {1, 2, 3, 4};
    int i2[] = {1, 3, 2, 4};
    int i3[] = {4, 3, 2, 1};
    int b1[] = {4, 2, 3, 1};
    int b2[] = {4, 2, 1, 3};
    expect_true("input-restricted: {1,2,3,4} ok", CheckInputRestricted(i1, 4));
    expect_true("input-restricted: {1,3,2,4} ok", CheckInputRestricted(i2, 4));
    expect_true("input-restricted: {4,3,2,1} ok", CheckInputRestricted(i3, 4));
    expect_true("input-restricted: {4,2,3,1} impossible", !CheckInputRestricted(b1, 4));
    expect_true("input-restricted: {4,2,1,3} impossible", !CheckInputRestricted(b2, 4));

    // 输出受限（两端都能入队，只能从队头出队）
    expect_true("output-restricted: {1,2,3,4} ok", CheckOutputRestricted(i1, 4));
    expect_true("output-restricted: {4,3,2,1} ok", CheckOutputRestricted(i3, 4));
    expect_true("output-restricted: {1,3,2,4} ok", CheckOutputRestricted(i2, 4));

    // 两种受限方式的可达序列个数不同 —— 这是选择题常考的对比
    EnumAllOutputs(3, true);
    EnumAllOutputs(3, false);
    EnumAllOutputs(4, true);
    EnumAllOutputs(4, false);
    return finish();
}
