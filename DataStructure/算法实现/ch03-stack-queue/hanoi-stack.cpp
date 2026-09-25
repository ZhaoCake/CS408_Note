// ============================================================================
// 第3章 栈、队列和数组 —— 递归转非递归（用显式栈模拟系统栈）
//   考点：
//     * 递归的本质就是系统栈，n 层递归 = n 层栈帧
//     * 把递归改成非递归 = 自己造栈存"回到哪一步（stage）"
//     * 汉诺塔移动次数 = 2^n - 1
// ============================================================================
#include "../ds_common.h"

// ------------------------------- 递归版 -------------------------------
void HanoiRec(int n, char a, char b, char c, char moves[], int &cnt) {
    if (n == 0) return;
    HanoiRec(n - 1, a, c, b, moves, cnt);
    moves[cnt++] = a;            // 把 a 上的盘子搬到 c，这里只记录"从哪个柱搬走"
    TRACE("move disk %d: %c -> %c\n", n, a, c);
    HanoiRec(n - 1, b, a, c, moves, cnt);
}

// --------------------------- 非递归版（显式栈） ---------------------------
typedef struct {
    int  n;
    char a, b, c;
    int  stage;                  // 0 = 还没开始，1 = 左子树已处理，2 = 全部完成
} Frame;

#define STACK_MAX 128

typedef struct {
    Frame data[STACK_MAX];
    int   top;
} FrameStack;

void InitFS(FrameStack &S) { S.top = -1; }
void PushFS(FrameStack &S, Frame f) { S.data[++S.top] = f; }
Frame PopFS(FrameStack &S) { return S.data[S.top--]; }
Frame &TopFS(FrameStack &S) { return S.data[S.top]; }
bool FSEmpty(const FrameStack &S) { return S.top == -1; }

void HanoiStack(int n, char a, char b, char c, char moves[], int &cnt) {
    FrameStack S;
    InitFS(S);
    Frame f0 = {n, a, b, c, 0};
    PushFS(S, f0);
    while (!FSEmpty(S)) {
        Frame &f = TopFS(S);
        if (f.n == 0) { PopFS(S); continue; }        // 空问题直接返回
        if (f.stage == 0) {
            // 第一件事：去处理 n-1, a -> b
            f.stage = 1;
            Frame sub = {f.n - 1, f.a, f.c, f.b, 0};
            PushFS(S, sub);
        } else if (f.stage == 1) {
            // 第二件事：搬动最大的那个盘子
            moves[cnt++] = f.a;
            TRACE("move disk %d: %c -> %c\n", f.n, f.a, f.c);
            f.stage = 2;
            Frame sub = {f.n - 1, f.b, f.a, f.c, 0};
            PushFS(S, sub);
        } else {
            PopFS(S);                                 // 两件事都做完，出栈
        }
    }
}

int main() {
    banner("Hanoi : recursion vs explicit stack");

    const int N = 4;
    char mvRec[64], mvStk[64];
    int cr = 0, cs = 0;

    HanoiRec(N, 'A', 'B', 'C', mvRec, cr);
    HanoiStack(N, 'A', 'B', 'C', mvStk, cs);

    expect_eq("recursive move count", cr, (1 << N) - 1);        // 2^n - 1
    expect_eq("stack version move count", cs, (1 << N) - 1);
    expect_eq("N = 4 -> 15 moves", cr, 15);

    int same = (cr == cs);
    for (int i = 0; same && i < cr; ++i) if (mvRec[i] != mvStk[i]) same = 0;
    expect_true("both versions produce the identical move sequence", same != 0);

    // n = 1 的边界：只搬一次
    char m1[8];
    int c1 = 0;
    HanoiStack(1, 'A', 'B', 'C', m1, c1);
    expect_eq("n = 1 -> 1 move", c1, 1);

    return finish();
}
