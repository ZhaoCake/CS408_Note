// ============================================================================
// 第2章 线性表 —— 单链表【经典算法题】
//   只留算法本体（408 算法设计题常考），不写"建表/按位插删"这类基本操作
//   约定：L 是带头结点的单链表；位序 i 从 1 开始
// ============================================================================
#include "../ds_common.h"

typedef struct LNode {
    int data;
    struct LNode *next;
} LNode, *LinkList;

// ---------------- 测试脚手架（不算考点） ----------------
LinkList BuildList(const int a[], int n) {
    LinkList L = (LNode *)malloc(sizeof(LNode));
    L->next = NULL;
    LNode *r = L;
    for (int i = 0; i < n; ++i) {
        LNode *s = (LNode *)malloc(sizeof(LNode));
        s->data = a[i];
        s->next = NULL;
        r->next = s;
        r = s;
    }
    return L;
}
void DestroyList(LinkList L) { while (L) { LNode *p = L->next; free(L); L = p; } }
void PrintList(LinkList L, const char *tag) {
    printf("  %-16s [", tag);
    for (LNode *p = L->next; p; p = p->next) { if (p != L->next) printf(" -> "); printf("%d", p->data); }
    printf("]\n");
}
void ToArray(LinkList L, int out[], int &n) {
    n = 0;
    for (LNode *p = L->next; p; p = p->next) out[n++] = p->data;
}

// ============================================================================
// 经典算法
// ============================================================================

// 1) 就地逆置（头插法重建）：O(n)、O(1)
void ReverseInPlace(LinkList &L) {
    LNode *p = L->next;
    L->next = NULL;                       // 摘下整条链
    while (p) {
        LNode *q = p->next;               // 先存后继再动指针
        p->next = L->next;
        L->next = p;
        p = q;
        count_move(2);
    }
}

// 2) 删除所有值为 x 的结点：O(n)、O(1)
//    要点：p 是"待检查结点的前驱"；删掉后 p 不动，因为它现在的后继是新结点
void DeleteX(LinkList &L, int x) {
    LNode *p = L;
    while (p->next) {
        count_cmp();
        if (p->next->data == x) {
            LNode *q = p->next;
            p->next = q->next;
            free(q);
        } else {
            p = p->next;
        }
    }
}

// 3) 找倒数第 k 个结点（双指针，一趟扫描）：O(n)、O(1)
LNode *FindKthFromEnd(LinkList L, int k) {
    if (k <= 0) return NULL;
    LNode *fast = L->next, *slow = L->next;
    for (int i = 0; i < k; ++i) {
        if (!fast) return NULL;           // 表长不足 k
        fast = fast->next;
    }
    while (fast) { fast = fast->next; slow = slow->next; }
    return slow;
}

// 4) 判断是否有环并返回入环点（Floyd 判圈）：O(n)、O(1)
LNode *DetectCycle(LinkList L) {
    LNode *slow = L->next, *fast = L->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {               // 相遇 -> 有环
            slow = L->next;               // 一个指针回到起点
            while (slow != fast) { slow = slow->next; fast = fast->next; }
            return slow;                  // 再次相遇处就是入环点
        }
    }
    return NULL;
}

// 5) 找中间结点（快慢指针），偶数个结点时返回前一个：O(n)、O(1)
LNode *MiddleNode(LinkList L) {
    LNode *slow = L->next, *fast = L->next;
    while (fast && fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// 6) 两个递增有序链表就地合并为一个递增有序链表（结果放 A，B 释放）：O(n+m)、O(1)
void MergeTwoSorted(LinkList &A, LinkList &B) {
    LNode *pa = A->next, *pb = B->next, *r = A;   // 复用 A 的头结点
    while (pa && pb) {
        count_cmp();
        if (pa->data <= pb->data) { r->next = pa; pa = pa->next; }
        else                      { r->next = pb; pb = pb->next; }
        r = r->next;
    }
    r->next = pa ? pa : pb;               // 剩下的一整条直接接上
    free(B);
    B = NULL;
}

// 7) 判断是否中心对称（如 1,2,3,2,1）：O(n)、O(1)
//    三步：找中间结点 -> 逆置后半段 -> 两端向中间比较（结束后还原，不留副作用）
bool IsSymmetric(LinkList L) {
    if (!L || !L->next) return true;

    LNode *slow = L->next, *fast = L->next;
    while (fast->next && fast->next->next) { slow = slow->next; fast = fast->next->next; }

    LNode *p = slow->next;                 // 后半段起点
    slow->next = NULL;
    LNode *rev = NULL;
    while (p) { LNode *q = p->next; p->next = rev; rev = p; p = q; }

    bool ok = true;
    LNode *a = L->next, *b = rev;
    while (b) {
        count_cmp();
        if (a->data != b->data) { ok = false; break; }
        a = a->next;
        b = b->next;
    }

    p = rev;                               // 还原后半段
    LNode *restored = NULL;
    while (p) { LNode *q = p->next; p->next = restored; restored = p; p = q; }
    slow->next = restored;
    return ok;
}

// 8) 删除最小值结点（唯一）：O(n)
//    要点：用 minpre 记住最小值结点的前驱，一次扫描就能删
bool DeleteMin(LinkList &L, int &minVal) {
    if (!L->next) return false;
    LNode *pre = L, *p = L->next;
    LNode *minpre = pre, *minp = p;
    while (p) {
        count_cmp();
        if (p->data < minp->data) { minp = p; minpre = pre; }
        pre = p;
        p = p->next;
    }
    minVal = minp->data;
    minpre->next = minp->next;
    free(minp);
    return true;
}

// 9) 拆分：把链表按奇偶位序拆成两条链表（A 保留奇数位，B 放偶数位）：O(n)
void SplitOddEven(LinkList &A, LinkList &B) {
    B = (LNode *)malloc(sizeof(LNode));
    B->next = NULL;
    LNode *ra = A, *rb = B, *p = A->next;
    int idx = 1;
    while (p) {
        LNode *q = p->next;
        if (idx % 2 == 1) { ra->next = p; ra = p; }
        else              { rb->next = p; rb = p; }
        ra->next = rb->next = NULL;
        p = q;
        ++idx;
    }
}

// ============================================================================
int main() {
    banner("Singly linked list : classic algorithm problems");
    int buf[32], n = 0;

    int a[] = {1, 2, 3, 4, 5};
    LinkList L = BuildList(a, 5);
    ReverseInPlace(L);
    int e1[] = {5, 4, 3, 2, 1};
    ToArray(L, buf, n);
    expect_array("ReverseInPlace", buf, n, e1, 5);

    ReverseInPlace(L);                       // 再逆置一次回到原序
    int b[] = {3, 1, 3, 5, 3};
    DestroyList(L);
    L = BuildList(b, 5);
    DeleteX(L, 3);
    int e2[] = {1, 5};
    ToArray(L, buf, n);
    expect_array("DeleteX", buf, n, e2, 2);
    DestroyList(L);

    L = BuildList(a, 5);
    LNode *k2 = FindKthFromEnd(L, 2);
    expect_true("FindKthFromEnd(L,2) != NULL", k2 != NULL);
    expect_eq("  value (want 4)", k2 ? k2->data : -1, 4);
    expect_true("k out of range -> NULL", FindKthFromEnd(L, 9) == NULL);

    LNode *p5 = L->next;
    while (p5->next) p5 = p5->next;          // 尾结点
    p5->next = L->next->next->next;          // 造成环，入环点值为 3
    LNode *entry = DetectCycle(L);
    expect_true("DetectCycle != NULL", entry != NULL);
    expect_eq("  entry value (want 3)", entry ? entry->data : -1, 3);
    p5->next = NULL;
    expect_true("no cycle -> NULL", DetectCycle(L) == NULL);
    DestroyList(L);

    L = BuildList(a, 5);
    expect_eq("MiddleNode 1..5 (want 3)", MiddleNode(L)->data, 3);
    DestroyList(L);
    int a4[] = {1, 2, 3, 4};
    L = BuildList(a4, 4);
    expect_eq("MiddleNode 1..4 (want 2)", MiddleNode(L)->data, 2);
    DestroyList(L);

    int mA[] = {1, 3, 5}, mB[] = {2, 4, 6};
    LinkList A = BuildList(mA, 3);
    LinkList B = BuildList(mB, 3);
    MergeTwoSorted(A, B);
    int e3[] = {1, 2, 3, 4, 5, 6};
    ToArray(A, buf, n);
    expect_array("MergeTwoSorted", buf, n, e3, 6);
    DestroyList(A);

    int s1[] = {1, 2, 3, 2, 1}, s2[] = {1, 2, 2, 1}, s3[] = {1, 2, 3, 4};
    L = BuildList(s1, 5);
    expect_true("IsSymmetric {1,2,3,2,1}", IsSymmetric(L));
    DestroyList(L);
    L = BuildList(s2, 4);
    expect_true("IsSymmetric {1,2,2,1}", IsSymmetric(L));
    DestroyList(L);
    L = BuildList(s3, 4);
    expect_true("!IsSymmetric {1,2,3,4}", !IsSymmetric(L));
    DestroyList(L);

    int d[] = {4, 2, 9, 1, 7};
    L = BuildList(d, 5);
    int minv = 0;
    expect_true("DeleteMin", DeleteMin(L, minv));
    expect_eq("  min value", minv, 1);
    int e4[] = {4, 2, 9, 7};
    ToArray(L, buf, n);
    expect_array("after DeleteMin", buf, n, e4, 4);

    SplitOddEven(L, B);
    PrintList(L, "odd positions");
    PrintList(B, "even positions");
    int e5[] = {4, 9}, e6[] = {2, 7};
    ToArray(L, buf, n);
    expect_array("odd list", buf, n, e5, 2);
    ToArray(B, buf, n);
    expect_array("even list", buf, n, e6, 2);
    DestroyList(L);
    DestroyList(B);
    return finish();
}
