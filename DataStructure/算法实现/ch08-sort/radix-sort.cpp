// ============================================================================
// 第8章 排序 —— 基数排序（LSD 最低位优先）【经典算法】
//   考点：
//     * 按"个位 -> 十位 -> 百位 ..."的顺序，每一趟做"分配 + 收集"
//     * 时间 O(d(n + r))：d 是关键字的位数，r 是基数（十进制就是 10）
//     * 空间 O(r)（桶）或 O(n)；稳定（收集时按顺序放回，相等元素的相对次序不变）
//     * 不能直接排浮点数；适合位数少、范围小的关键字（如学号、手机号）
//     * 手工题：给出每趟分配收集后的序列，下面的输出可以直接对拍
// ============================================================================
#include "../ds_common.h"

// 王道经典例
static int base[10] = {278, 109, 63, 930, 589, 184, 505, 269, 8, 83};
#define N 10

void PrintBuckets(int buckets[][16], int cnt[], int r) {
    for (int d = 0; d < r; ++d) {
        if (cnt[d] == 0) continue;
        printf("    bucket %d:", d);
        for (int i = 0; i < cnt[d]; ++i) printf(" %d", buckets[d][i]);
        printf("\n");
    }
}

void RadixSort(int A[], int n, bool trace) {
    int maxV = A[0];
    for (int i = 1; i < n; ++i) if (A[i] > maxV) maxV = A[i];

    int buckets[10][16];
    int cnt[10];
    for (int exp = 1; maxV / exp > 0; exp *= 10) {      // exp = 1, 10, 100 ... 个位/十位/百位
        for (int d = 0; d < 10; ++d) cnt[d] = 0;
        // 1) 分配：按当前位的数字放进对应的桶（从前往后放 -> 保证稳定）
        for (int i = 0; i < n; ++i) {
            int d = (A[i] / exp) % 10;
            buckets[d][cnt[d]++] = A[i];
        }
        if (trace) { printf("  --- pass for digit %d ---\n", exp); PrintBuckets(buckets, cnt, 10); }
        // 2) 收集：按桶号从小到大放回原数组
        int k = 0;
        for (int d = 0; d < 10; ++d)
            for (int i = 0; i < cnt[d]; ++i) A[k++] = buckets[d][i];
        if (trace) { TRACE("after collecting: "); print_array("", A, n); }
    }
}

// 带标记的版本，用来验证稳定性
struct Item { int v, id;                    // 放在文件作用域，函数和 main 共用同一个类型
};
void RadixSortItems(Item A[], int n) {
    int val[16];                                   // 按 id 保存原始值（id 就是初始下标）
    for (int i = 0; i < n; ++i) val[i] = A[i].v;
    int buckets[10][16], cnt[10];
    int maxV = 0;
    for (int i = 0; i < n; ++i) if (A[i].v > maxV) maxV = A[i].v;
    for (int exp = 1; maxV / exp > 0; exp *= 10) {
        for (int d = 0; d < 10; ++d) cnt[d] = 0;
        for (int i = 0; i < n; ++i) {
            int d = (A[i].v / exp) % 10;
            buckets[d][cnt[d]++] = A[i].id;        // 只存 id，值靠 val[] 查回来
        }
        int k = 0;
        for (int d = 0; d < 10; ++d)
            for (int i = 0; i < cnt[d]; ++i) {
                int id = buckets[d][i];
                A[k].id = id;
                A[k].v = val[id];                  // 千万别把值清零，下一趟还要用
                ++k;
            }
    }
}

static bool IsSorted(const int A[], int n) {
    for (int i = 1; i < n; ++i) if (A[i] < A[i - 1]) return false;
    return true;
}

int main() {
    banner("Radix sort (LSD)");
    int A[N];
    int eSorted[] = {8, 63, 83, 109, 184, 269, 278, 505, 589, 930};

    for (int i = 0; i < N; ++i) A[i] = base[i];
    print_array("input", A, N);
    RadixSort(A, N, true);
    print_array("sorted", A, N);
    expect_array("LSD radix sort result", A, N, eSorted, N);
    expect_true("is sorted", IsSorted(A, N));

    // 位数 = 3（最大数是 930），所以只需要 3 趟
    printf("  passes needed = number of digits of the maximum (930 -> 3)\n");

    // 稳定性：相同关键字的相对次序不变
    Item items[6] = {{24, 0}, {13, 1}, {24, 2}, {13, 3}, {5, 4}, {24, 5}};
    RadixSortItems(items, 6);
    printf("  stability check:");
    for (int i = 0; i < 6; ++i) printf(" %d(id=%d)", items[i].v, items[i].id);
    printf("\n");
    int stable = 1;
    for (int i = 1; i < 6; ++i)
        if (items[i].v == items[i - 1].v && items[i].id < items[i - 1].id) stable = 0;
    expect_true("radix sort is stable", stable != 0);
    int sortedVals = 1;
    for (int i = 1; i < 6; ++i) if (items[i].v < items[i - 1].v) sortedVals = 0;
    expect_true("values are in non-decreasing order", sortedVals != 0);

    // 边界：全相同、单个元素、有 0
    int same[5] = {7, 7, 7, 7, 7};
    RadixSort(same, 5, false);
    expect_true("all equal keys", IsSorted(same, 5));

    int one[1] = {42};
    RadixSort(one, 1, false);
    expect_eq("single element", one[0], 42);

    int withZero[5] = {0, 10, 100, 5, 0};
    RadixSort(withZero, 5, false);
    int eZero[] = {0, 0, 5, 10, 100};
    expect_array("array containing 0", withZero, 5, eZero, 5);

    // 逆序输入也照样正确
    int rev[5] = {930, 589, 278, 109, 8};
    RadixSort(rev, 5, false);
    int eRev[] = {8, 109, 278, 589, 930};
    expect_array("reverse input", rev, 5, eRev, 5);
    return finish();
}
