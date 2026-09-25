// ============================================================================
// 第5章 树与二叉树 —— 哈夫曼树与哈夫曼编码
//   考点：
//     * 每次选两个权值最小的根结点合并 -> 得到的树带权路径长度 WPL 最小
//     * n 个叶子的哈夫曼树共有 2n-1 个结点，且没有度为 1 的结点
//     * WPL = 每次合并产生的新结点权值之和 = 所有非叶结点权值之和
//     * 编码：左分支记 0、右分支记 1，叶子结点的路径就是它的编码（前缀码，不会有歧义）
// ============================================================================
#include "../ds_common.h"

#define MAXN 32

typedef struct {
    int weight;
    int parent, lchild, rchild;     // 0 表示空
} HTNode;

typedef HTNode *HuffmanTree;        // 下标 1..2n-1，0 号不用

// 在 1..k 中选出两个 parent == 0 且权值最小的结点下标
static void SelectTwo(const HTNode HT[], int k, int &s1, int &s2) {
    s1 = s2 = 0;
    for (int i = 1; i <= k; ++i) {
        if (HT[i].parent != 0) continue;
        if (s1 == 0 || HT[i].weight < HT[s1].weight) { s2 = s1; s1 = i; }
        else if (s2 == 0 || HT[i].weight < HT[s2].weight) { s2 = i; }
    }
}

void CreateHuffman(HuffmanTree &HT, int n, const int w[]) {
    int m = 2 * n - 1;
    HT = (HTNode *)calloc(m + 1, sizeof(HTNode));
    for (int i = 1; i <= n; ++i) HT[i].weight = w[i - 1];        // 1..n 是叶子
    for (int i = n + 1; i <= m; ++i) {                           // n+1..2n-1 是内部结点
        int s1 = 0, s2 = 0;
        SelectTwo(HT, i - 1, s1, s2);
        HT[i].weight = HT[s1].weight + HT[s2].weight;
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[s1].parent = i;
        HT[s2].parent = i;
        TRACE("merge %d + %d = %d (node %d)\n", HT[s1].weight, HT[s2].weight, HT[i].weight, i);
    }
}

// 从根往下递归数 WPL：叶子到根的深度 × 权值
static int WPLRec(const HTNode HT[], int root, int depth) {
    if (HT[root].lchild == 0 && HT[root].rchild == 0)
        return HT[root].weight * depth;
    return WPLRec(HT, HT[root].lchild, depth + 1) + WPLRec(HT, HT[root].rchild, depth + 1);
}

int WPL(const HuffmanTree HT, int n) {
    return WPLRec(HT, 2 * n - 1, 0);            // 根结点是 2n-1
}

// 另一种算法：每个叶子往上走到根，把深度乘权值累加（用来交叉验证）
int WPLByParent(const HuffmanTree HT, int n) {
    int sum = 0;
    for (int i = 1; i <= n; ++i) {
        int depth = 0;
        for (int p = HT[i].parent; p != 0; p = HT[p].parent) ++depth;
        sum += HT[i].weight * depth;
    }
    return sum;
}

// 生成编码：左 0 右 1，codes[i-1] 存放第 i 个叶子的编码
static void CodeRec(const HTNode HT[], int root, char buf[], int depth, char codes[][MAXN]) {
    if (HT[root].lchild == 0 && HT[root].rchild == 0) {
        buf[depth] = '\0';
        strcpy(codes[root - 1], buf);
        return;
    }
    buf[depth] = '0';
    CodeRec(HT, HT[root].lchild, buf, depth + 1, codes);
    buf[depth] = '1';
    CodeRec(HT, HT[root].rchild, buf, depth + 1, codes);
}

void BuildCodes(const HuffmanTree HT, int n, char codes[][MAXN]) {
    char buf[MAXN];
    for (int i = 0; i < n; ++i) codes[i][0] = '\0';
    CodeRec(HT, 2 * n - 1, buf, 0, codes);
}

// 校验前缀码性质：任何两个编码互不为前缀
bool IsPrefixCode(int n, char codes[][MAXN]) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            if (strncmp(codes[i], codes[j], strlen(codes[i])) == 0) return false;
        }
    return true;
}

int main() {
    banner("Huffman tree and Huffman codes");
    // 王道经典例：权值 {5, 29, 7, 8, 14, 23, 3, 11}，WPL = 271
    int w[] = {5, 29, 7, 8, 14, 23, 3, 11};
    const int n = 8;

    HuffmanTree HT;
    CreateHuffman(HT, n, w);
    printf("  node : weight parent lchild rchild\n");
    for (int i = 1; i <= 2 * n - 1; ++i)
        printf("  %4d : %5d %5d %6d %6d\n", i, HT[i].weight, HT[i].parent, HT[i].lchild, HT[i].rchild);

    expect_eq("total nodes == 2n-1", 2 * n - 1, 15);
    expect_eq("WPL (by depth)", WPL(HT, n), 271);
    expect_eq("WPL (by parent walk)", WPLByParent(HT, n), 271);

    // 哈夫曼树没有度为 1 的结点
    int deg1 = 0;
    for (int i = 1; i <= 2 * n - 1; ++i)
        if ((HT[i].lchild == 0) != (HT[i].rchild == 0)) ++deg1;
    expect_eq("no node has exactly one child", deg1, 0);

    // 根结点的权值 = 所有叶子权值之和
    expect_eq("root weight == sum of leaves", HT[2 * n - 1].weight, 100);

    char codes[MAXN][MAXN];
    BuildCodes(HT, n, codes);
    printf("  codes:");
    for (int i = 0; i < n; ++i) printf(" %d:%s", w[i], codes[i]);
    printf("\n");
    expect_true("codes form a prefix code", IsPrefixCode(n, codes));

    // 编码长度 × 权值 应该等于 WPL
    int total = 0;
    for (int i = 0; i < n; ++i) total += (int)strlen(codes[i]) * w[i];
    expect_eq("sum(len * weight) == WPL", total, 271);

    // 只有两个叶子：编码必须是 0 和 1
    int w2[] = {1, 2};
    HuffmanTree HT2;
    CreateHuffman(HT2, 2, w2);
    char c2[MAXN][MAXN];
    BuildCodes(HT2, 2, c2);
    expect_eq("n=2 WPL", WPL(HT2, 2), 3);
    expect_true("n=2 codes are 0 and 1", (strcmp(c2[0], "0") == 0 && strcmp(c2[1], "1") == 0) ||
                                          (strcmp(c2[0], "1") == 0 && strcmp(c2[1], "0") == 0));

    // 单个叶子：WPL = 0
    int w1[] = {5};
    HuffmanTree HT1;
    CreateHuffman(HT1, 1, w1);
    expect_eq("n=1 WPL", WPL(HT1, 1), 0);

    free(HT); free(HT2); free(HT1);
    return finish();
}
