// ============================================================================
// 第3章 栈、队列和数组 —— 迷宫求解
//   考点：
//     * DFS + 栈：不一定最短，但一定能找到一条通路（走不通就回溯）
//     * BFS + 队列：按"层"扩展，第一次到达终点时就是最短路径
//     * 两者都要记录"前驱/路径"，最后回溯出完整路线
// ============================================================================
#include "../ds_common.h"

#define R 5
#define C 5

typedef struct { int r, c; } Pos;

// 方向顺序：下、右、上、左（DFS 选不同顺序会得到不同通路）
static const int dr[4] = {1, 0, -1, 0};
static const int dc[4] = {0, 1, 0, -1};

// 0 = 通路，1 = 墙
static int maze[R][C] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0},
    {1, 1, 0, 1, 0},
    {0, 0, 0, 0, 0},
};

bool Inside(int r, int c) { return r >= 0 && r < R && c >= 0 && c < C; }

// ------------------------------ DFS + 栈 ------------------------------
bool DFSMaze(int mz[R][C], Pos path[], int &pathLen) {
    int visited[R][C] = {0};
    Pos stack[R * C];
    int top = -1;

    stack[++top] = Pos{0, 0};
    visited[0][0] = 1;
    while (top >= 0) {
        Pos cur = stack[top];
        if (cur.r == R - 1 && cur.c == C - 1) {
            pathLen = top + 1;
            for (int i = 0; i <= top; ++i) path[i] = stack[i];
            return true;
        }
        int moved = 0;
        for (int d = 0; d < 4; ++d) {            // 试四个方向
            int nr = cur.r + dr[d], nc = cur.c + dc[d];
            if (!Inside(nr, nc) || mz[nr][nc] == 1 || visited[nr][nc]) continue;
            visited[nr][nc] = 1;
            stack[++top] = Pos{nr, nc};
            TRACE("push (%d,%d)\n", nr, nc);
            moved = 1;
            break;
        }
        if (!moved) {                            // 死路 -> 回溯
            TRACE("backtrack (%d,%d)\n", cur.r, cur.c);
            --top;
        }
    }
    return false;
}

// ------------------------------ BFS + 队列 ------------------------------
bool BFSMaze(int mz[R][C], Pos path[], int &pathLen, int &dist) {
    int prev[R][C], d[R][C];
    for (int i = 0; i < R; ++i)
        for (int j = 0; j < C; ++j) { prev[i][j] = -1; d[i][j] = -1; }

    Pos q[R * C];
    int head = 0, tail = 0;
    q[tail++] = Pos{0, 0};
    d[0][0] = 0;
    while (head < tail) {
        Pos cur = q[head++];
        if (cur.r == R - 1 && cur.c == C - 1) {
            dist = d[cur.r][cur.c];
            // 从终点顺着 prev 回溯，再由栈倒过来得到正向路径
            Pos rev[R * C];
            int n = 0;
            int r = cur.r, c = cur.c;
            while (r != -1) {
                rev[n++] = Pos{r, c};
                int p = prev[r][c];
                if (p == -1) break;
                r = p / C;
                c = p % C;
            }
            pathLen = n;
            for (int i = 0; i < n; ++i) path[i] = rev[n - 1 - i];
            return true;
        }
        for (int k = 0; k < 4; ++k) {
            int nr = cur.r + dr[k], nc = cur.c + dc[k];
            if (!Inside(nr, nc) || mz[nr][nc] == 1) continue;
            if (d[nr][nc] != -1) continue;                // 已经访问过（BFS 首次访问即最短）
            d[nr][nc] = d[cur.r][cur.c] + 1;
            prev[nr][nc] = cur.r * C + cur.c;
            q[tail++] = Pos{nr, nc};
        }
    }
    return false;
}

void PrintPath(const Pos path[], int n, const char *tag) {
    printf("  %-14s (%d cells) ", tag, n);
    for (int i = 0; i < n; ++i) printf("(%d,%d)%s", path[i].r, path[i].c, i + 1 < n ? " -> " : "\n");
}

// 校验一条路径是否合法：起点终点对、相邻两格必须紧挨着且不是墙
bool ValidatePath(int mz[R][C], const Pos path[], int n) {
    if (n <= 0) return false;
    if (path[0].r != 0 || path[0].c != 0) return false;
    if (path[n - 1].r != R - 1 || path[n - 1].c != C - 1) return false;
    for (int i = 0; i < n; ++i) if (mz[path[i].r][path[i].c] == 1) return false;
    for (int i = 1; i < n; ++i) {
        int d = (path[i].r - path[i - 1].r) + (path[i].c - path[i - 1].c);
        int d2 = (path[i].r - path[i - 1].r) - (path[i].c - path[i - 1].c);
        if (d * d + d2 * d2 != 2) return false;             // 相邻格的判定：|dr|+|dc| == 1
    }
    return true;
}

int main() {
    banner("Maze : DFS(one path) vs BFS(shortest path)");
    Pos path[R * C];
    int len = 0, dist = 0;

    expect_true("DFS finds a path", DFSMaze(maze, path, len));
    PrintPath(path, len, "DFS path");
    expect_true("DFS path is valid", ValidatePath(maze, path, len));
    expect_eq("DFS path starts at (0,0)", path[0].r * 10 + path[0].c, 0);

    expect_true("BFS finds a path", BFSMaze(maze, path, len, dist));
    PrintPath(path, len, "BFS path");
    expect_true("BFS path is valid", ValidatePath(maze, path, len));
    expect_eq("BFS shortest distance", dist, 8);            // 曼哈顿距离 (5-1)+(5-1) = 8
    expect_eq("BFS path cell count", len, 9);

    // 无解迷宫：把终点仅有的两个入口堵死
    int blocked[R][C];
    for (int i = 0; i < R; ++i) for (int j = 0; j < C; ++j) blocked[i][j] = maze[i][j];
    blocked[R - 1][C - 2] = 1;                              // (4,3) = 墙
    blocked[R - 2][C - 1] = 1;                              // (3,4) = 墙
    expect_true("DFS reports no path", !DFSMaze(blocked, path, len));
    expect_true("BFS reports no path", !BFSMaze(blocked, path, len, dist));
    return finish();
}
