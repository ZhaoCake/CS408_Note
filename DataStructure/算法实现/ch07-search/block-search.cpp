// ============================================================================
// 第7章 查找 —— 分块查找（索引顺序查找）【经典算法】
//   考点：
//     * 前提：块间有序（第 i 块所有元素 < 第 i+1 块所有元素），块内可以无序
//     * 顺序查索引 + 块内顺序查：ASL = (b+1)/2 + (s+1)/2（b 块数、s 每块长度）
//     * 折半查索引 + 块内顺序查：ASL ≈ log2(b+1) + (s+1)/2，块数取 sqrt(n) 时最优
//     * 与折半查找对比：折半要求整体有序，分块只要求块间有序（插入删除更方便）
// ============================================================================
#include "../ds_common.h"

#define BLOCKS 3
#define BLOCKLEN 6
#define N (BLOCKS * BLOCKLEN)

// 块内无序、块间有序
static int data[N] = {
    22, 12, 28,  9, 18, 25,      // 第 0 块：都 < 30
    44, 32, 56, 39, 47, 51,      // 第 1 块：30 ~ 59
    62, 88, 71, 65, 93, 60,      // 第 2 块：>= 60
};
typedef struct { int max; int start; int len; } IndexItem;
static IndexItem indexTab[BLOCKS];

void BuildIndex() {
    for (int b = 0; b < BLOCKS; ++b) {
        indexTab[b].start = b * BLOCKLEN;
        indexTab[b].len = BLOCKLEN;
        int mx = data[indexTab[b].start];
        for (int i = 1; i < BLOCKLEN; ++i)
            if (data[indexTab[b].start + i] > mx) mx = data[indexTab[b].start + i];
        indexTab[b].max = mx;
    }
}

// 顺序查索引表 + 块内顺序查找；返回数组下标，失败返回 -1
int BlockSearch(int key, int *cmp) {
    int c = 0;
    int b = -1;
    for (int i = 0; i < BLOCKS; ++i) {        // 1) 顺查索引，找第一个 max >= key 的块
        ++c;
        if (indexTab[i].max >= key) { b = i; break; }
    }
    if (b == -1) { if (cmp) *cmp = c; return -1; }   // 比所有块的最大值都大
    int s = indexTab[b].start;
    for (int i = 0; i < indexTab[b].len; ++i) {      // 2) 块内顺序查找
        ++c;
        if (data[s + i] == key) { if (cmp) *cmp = c; return s + i; }
    }
    if (cmp) *cmp = c;
    return -1;
}

// 折半查索引表 + 块内顺序查找
int BlockSearchBinaryIndex(int key, int *cmp) {
    int c = 0, lo = 0, hi = BLOCKS - 1, b = -1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        ++c;
        if (indexTab[mid].max >= key) { b = mid; hi = mid - 1; }   // 往左找"第一个"满足的块
        else lo = mid + 1;
    }
    if (b == -1) { if (cmp) *cmp = c; return -1; }
    int s = indexTab[b].start;
    for (int i = 0; i < indexTab[b].len; ++i) {
        ++c;
        if (data[s + i] == key) { if (cmp) *cmp = c; return s + i; }
    }
    if (cmp) *cmp = c;
    return -1;
}

int main() {
    banner("Block search (indexed sequential search)");
    BuildIndex();
    printf("  index table:");
    for (int i = 0; i < BLOCKS; ++i)
        printf("  block%d[max=%d]", i, indexTab[i].max);
    printf("\n");

    int cmp = 0;
    expect_eq("find 32 (block 1)", BlockSearch(32, &cmp), 7);
    expect_eq("find 9 (block 0, first position)", BlockSearch(9, &cmp), 3);
    expect_eq("find 93 (block 2)", BlockSearch(93, &cmp), 16);
    expect_eq("miss 100 -> -1", BlockSearch(100, &cmp), -1);
    expect_eq("  miss is cheap: only compares the index", cmp, BLOCKS);
    expect_eq("miss 35 (falls in block 1 but absent)", BlockSearch(35, &cmp), -1);

    // 两种索引查法结果必须一致
    int same = 1, sumSeq = 0, sumBin = 0;
    for (int k = 0; k <= 100; ++k) {
        int c1 = 0, c2 = 0;
        int r1 = BlockSearch(k, &c1);
        int r2 = BlockSearchBinaryIndex(k, &c2);
        if (r1 != r2) same = 0;
        sumSeq += c1;
        sumBin += c2;
    }
    expect_true("sequential index == binary index (same results)", same != 0);
    printf("  total comparisons: sequential index = %d, binary index = %d\n", sumSeq, sumBin);
    expect_true("binary index saves comparisons", sumBin < sumSeq);

    // 块间有序是前提：检查每一块的最大值随块号递增
    int ordered = 1;
    for (int b = 1; b < BLOCKS; ++b)
        if (indexTab[b].max <= indexTab[b - 1].max) ordered = 0;
    expect_true("block max values are increasing", ordered != 0);

    // 块内无序也没关系（第 0 块是 22,12,28,9,18,25）
    expect_true("elements inside a block are NOT sorted", data[0] > data[1]);

    // ASL 公式：(b+1)/2 + (s+1)/2，b = 块数，s = 块长
    double asl = (BLOCKS + 1) / 2.0 + (BLOCKLEN + 1) / 2.0;
    printf("  ASL formula = %.2f = (b+1)/2 + (s+1)/2, optimal block size = sqrt(n) = %.2f\n",
           asl, std::sqrt((double)N));
    expect_true("ASL == 2 + 3.5 = 5.5", asl == 5.5);

    // 所有元素都能找到，且位置正确
    int allOk = 1;
    for (int i = 0; i < N; ++i)
        if (BlockSearch(data[i], &cmp) < 0) allOk = 0;
    expect_true("every stored element is found", allOk != 0);
    return finish();
}
