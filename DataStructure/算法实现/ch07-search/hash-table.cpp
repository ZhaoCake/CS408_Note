// ============================================================================
// 第7章 查找 —— 散列表【经典算法】
//   考点：
//     * 散列函数：除留余数法 H(key) = key % p（p 取不大于表长的最大质数）
//     * 冲突处理：开放定址（线性探测、平方探测、双散列）和拉链法（链地址法）
//     * 线性探测会导致"聚集"，平方探测能缓解；拉链法把同义词串成一条链表
//     * 装填因子 alpha = 元素个数 / 表长，它决定性能，与元素个数无关
//     * ASL 成功/失败都要会手算（失败以"每个散列地址到第一个空位的比较次数"平均）
// ============================================================================
#include "../ds_common.h"

#define TSIZE 16          // 表长
#define P 13              // 除留余数法的除数（不大于表长的最大质数）
#define NKEYS 12
#define EMPTY -1

static int keys[NKEYS] = {19, 14, 23, 1, 68, 20, 84, 27, 55, 11, 10, 79};

int Hash(int key) { return key % P; }

// ------------------------- 一、线性探测（开放定址） -------------------------
typedef struct { int data[TSIZE]; int count; } LinearTable;

void LinearInit(LinearTable &H) {
    for (int i = 0; i < TSIZE; ++i) H.data[i] = EMPTY;
    H.count = 0;
}
// 返回插入位置；probes 记录探测次数
int LinearInsert(LinearTable &H, int key, int *probes) {
    int pos = Hash(key);
    int c = 0;
    while (H.data[pos] != EMPTY) {          // 线性探测：一个个往后找
        ++c;
        if (H.data[pos] == key) { if (probes) *probes = c; return pos; }   // 已存在
        pos = (pos + 1) % TSIZE;             // 走到表尾就绕回表头
    }
    ++c;
    H.data[pos] = key;
    ++H.count;
    if (probes) *probes = c;
    return pos;
}
// 返回下标，找不到返回 -1；probes 是探测次数
int LinearSearch(LinearTable &H, int key, int *probes) {
    int pos = Hash(key), c = 0;
    while (H.data[pos] != EMPTY) {
        ++c;
        if (H.data[pos] == key) { if (probes) *probes = c; return pos; }
        pos = (pos + 1) % TSIZE;
        if (c > TSIZE) break;                // 防止死循环（表满）
    }
    ++c;
    if (probes) *probes = c;
    return -1;
}

// ------------------------- 二、平方探测（开放定址） -------------------------
typedef struct { int data[TSIZE]; int count; } QuadTable;

void QuadInit(QuadTable &H) {
    for (int i = 0; i < TSIZE; ++i) H.data[i] = EMPTY;
    H.count = 0;
}
int QuadInsert(QuadTable &H, int key, int *probes) {
    int base = Hash(key), c = 0;
    for (int d = 0; d < TSIZE; ++d) {
        // 探测序列：base, base+1, base-1, base+4, base-4 ...
        int step = (d % 2 == 1) ? ((d + 1) / 2) : -(d / 2);
        int pos = ((base + step) % TSIZE + TSIZE) % TSIZE;
        ++c;
        if (H.data[pos] == EMPTY) {
            H.data[pos] = key;
            ++H.count;
            if (probes) *probes = c;
            return pos;
        }
        if (H.data[pos] == key) { if (probes) *probes = c; return pos; }
    }
    if (probes) *probes = c;
    return -1;                               // 表满
}

// ------------------------- 三、链地址法（拉链） -------------------------
typedef struct HNode { int val; struct HNode *next; } HNode;
typedef struct { HNode *bucket[TSIZE]; int count; } ChainTable;

void ChainInit(ChainTable &H) {
    for (int i = 0; i < TSIZE; ++i) H.bucket[i] = NULL;
    H.count = 0;
}
void ChainInsert(ChainTable &H, int key) {
    int pos = Hash(key);
    HNode *s = (HNode *)malloc(sizeof(HNode));
    s->val = key;
    s->next = H.bucket[pos];                 // 头插
    H.bucket[pos] = s;
    ++H.count;
}
bool ChainSearch(ChainTable &H, int key, int *probes) {
    int c = 0;
    for (HNode *p = H.bucket[Hash(key)]; p; p = p->next) {
        ++c;
        if (p->val == key) { if (probes) *probes = c; return true; }
    }
    if (probes) *probes = c;
    return false;
}
void ChainFree(ChainTable &H) {
    for (int i = 0; i < TSIZE; ++i) {
        HNode *p = H.bucket[i];
        while (p) { HNode *q = p->next; free(p); p = q; }
        H.bucket[i] = NULL;
    }
}

void PrintLinear(const LinearTable &H) {
    printf("  index :");
    for (int i = 0; i < TSIZE; ++i) printf("%4d", i);
    printf("\n  value :");
    for (int i = 0; i < TSIZE; ++i) {
        if (H.data[i] == EMPTY) printf("   -");
        else printf("%4d", H.data[i]);
    }
    printf("\n");
}

int main() {
    banner("Hash table : linear probing / quadratic probing / chaining");

    // ---- 线性探测：王道经典数据集 ----
    LinearTable L;
    LinearInit(L);
    for (int i = 0; i < NKEYS; ++i) LinearInsert(L, keys[i], NULL);
    PrintLinear(L);
    expect_eq("all keys inserted", L.count, NKEYS);

    int ok = 1, sumOk = 0;
    for (int i = 0; i < NKEYS; ++i) {
        int c = 0;
        int pos = LinearSearch(L, keys[i], &c);
        if (pos < 0) ok = 0;
        sumOk += c;
    }
    expect_true("every key can be found", ok != 0);
    double aslOk = sumOk / (double)NKEYS;
    printf("  ASL(success) = %d / %d = %.4f\n", sumOk, NKEYS, aslOk);
    expect_true("ASL(success) == 2.5 (classic result)", aslOk == 2.5);

    // 失败 ASL：对每个散列地址统计"找到第一个空位"的比较次数
    int sumFail = 0;
    for (int h = 0; h < P; ++h) {
        int pos = h, c = 0;
        while (L.data[pos] != EMPTY) { ++c; pos = (pos + 1) % TSIZE; }
        ++c;
        sumFail += c;
    }
    printf("  ASL(fail) = %d / %d = %.4f\n", sumFail, P, sumFail / (double)P);
    expect_true("ASL(fail) == 7.0 (classic result)", sumFail == 91);

    expect_eq("miss -> -1", LinearSearch(L, 100, NULL), -1);
    expect_true("load factor", L.count / (double)TSIZE < 1.0);

    // ---- 平方探测：探测次数不应超过线性探测 ----
    QuadTable Q;
    QuadInit(Q);
    int sumQ = 0;
    for (int i = 0; i < NKEYS; ++i) {
        int c = 0;
        QuadInsert(Q, keys[i], &c);
        sumQ += c;
    }
    int sumL = 0;
    for (int i = 0; i < NKEYS; ++i) {
        int c = 0;
        LinearInsert(L, keys[i], &c);        // 已存在 -> 只返回探测次数
        sumL += c;
    }
    printf("  total probes: linear = %d, quadratic = %d\n", sumL, sumQ);
    expect_eq("quadratic probing also stores all keys", Q.count, NKEYS);
    expect_true("quadratic probing reduces clustering", sumQ <= sumL);
    for (int i = 0; i < NKEYS; ++i) {
        int found = 0;
        for (int j = 0; j < TSIZE; ++j) if (Q.data[j] == keys[i]) found = 1;
        if (!found) { ok = 0; }
    }
    expect_true("every key is stored in the quadratic table", ok != 0);

    // ---- 链地址法 ----
    ChainTable C;
    ChainInit(C);
    for (int i = 0; i < NKEYS; ++i) ChainInsert(C, keys[i]);
    expect_eq("chaining stores all keys", C.count, NKEYS);
    int ok2 = 1, sumC = 0;
    for (int i = 0; i < NKEYS; ++i) {
        int c = 0;
        if (!ChainSearch(C, keys[i], &c)) ok2 = 0;
        sumC += c;
    }
    expect_true("every key found in the chain table", ok2 != 0);
    printf("  chaining ASL(success) = %d / %d = %.4f\n", sumC, NKEYS, sumC / (double)NKEYS);
    expect_true("chaining beats linear probing here", sumC / (double)NKEYS < aslOk);
    expect_true("miss in chaining is cheap", !ChainSearch(C, 100, NULL));

    // 同义词都落在同一条链上（说明散列函数起作用）
    int sameBucket = 0;
    for (int i = 0; i < NKEYS; ++i) if (Hash(keys[i]) == Hash(19)) ++sameBucket;
    printf("  keys sharing bucket H(19): %d\n", sameBucket);
    expect_true("19 and 84 are synonyms (both %% 13 == 6)", Hash(19) == Hash(84));

    ChainFree(C);
    return finish();
}
