// ============================================================================
// 第3章 栈、队列和数组 —— 栈的【经典算法】
//   只留算法本体（栈自身的初始化/入栈/出栈是"基本数据结构"，这里不写）
//   函数内部按需用一个最小栈辅助，保证每个算法都能独立看懂
// ============================================================================
#include "../ds_common.h"

// ---------------- 算法内部需要的栈（最小接口） ----------------
#define STACK_MAX 64
typedef struct { int data[STACK_MAX]; int top; } Stack;
static void SInit(Stack &S)          { S.top = -1; }
static bool SEmpty(const Stack &S)   { return S.top == -1; }
static void SPush(Stack &S, int x)   { S.data[++S.top] = x; }
static bool SPop(Stack &S, int &x)   { if (SEmpty(S)) return false; x = S.data[S.top--]; return true; }
static bool STop(const Stack &S, int &x) { if (SEmpty(S)) return false; x = S.data[S.top]; return true; }

// ============================================================================
// 1) 括号匹配：左括号入栈，右括号与栈顶配对，最后栈必须为空
// ============================================================================
bool BracketsMatch(const char *s) {
    Stack S;
    SInit(S);
    for (int i = 0; s[i] != '\0'; ++i) {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{') {
            SPush(S, c);
        } else if (c == ')' || c == ']' || c == '}') {
            int t = 0;
            if (!SPop(S, t)) return false;                    // 右括号多了
            char want = (c == ')') ? '(' : (c == ']') ? '[' : '{';
            if ((char)t != want) return false;                // 类型不匹配
        }
    }
    return SEmpty(S);                                        // 左括号多了
}

// ============================================================================
// 2) 中缀转后缀：遇操作数直接输出，遇运算符先弹掉"优先级 >= 自己"的
//    例："(a+b)*c-d" -> "ab+c*d-"
// ============================================================================
int OpPrec(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;                                                // '(' 当作最低
}

void InfixToPostfix(const char *infix, char *postfix) {
    Stack S;
    SInit(S);
    int k = 0;
    for (int i = 0; infix[i] != '\0'; ++i) {
        char c = infix[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            postfix[k++] = c;
        } else if (c == '(') {
            SPush(S, c);
        } else if (c == ')') {
            int t = 0;
            while (STop(S, t) && t != '(') { SPop(S, t); postfix[k++] = (char)t; }
            SPop(S, t);                                      // 弹掉 '('，不输出
        } else {
            int t = 0;
            while (STop(S, t) && t != '(' && OpPrec((char)t) >= OpPrec(c)) {
                SPop(S, t);
                postfix[k++] = (char)t;
            }
            SPush(S, c);
        }
    }
    int t = 0;
    while (SPop(S, t)) postfix[k++] = (char)t;               // 栈里剩下的全部输出
    postfix[k] = '\0';
}

// ============================================================================
// 3) 后缀表达式求值：遇数字入栈，遇运算符弹出两个数（先弹出的是右操作数）
//    例："34+5*" -> (3+4)*5 = 35
// ============================================================================
int EvalPostfix(const char *postfix) {
    Stack S;
    SInit(S);
    for (int i = 0; postfix[i] != '\0'; ++i) {
        char c = postfix[i];
        if (c >= '0' && c <= '9') {
            SPush(S, c - '0');
        } else {
            int r = 0, l = 0;
            SPop(S, r);
            SPop(S, l);
            switch (c) {
                case '+': SPush(S, l + r); break;
                case '-': SPush(S, l - r); break;             // 顺序不能反
                case '*': SPush(S, l * r); break;
                case '/': SPush(S, l / r); break;
            }
        }
    }
    int v = 0;
    SPop(S, v);
    return v;
}

// ============================================================================
// 4) 判定出栈序列是否合法：入栈序列固定为 1..n
//    例：n=5 时 {4,5,3,2,1} 合法，{4,3,5,1,2} 非法
// ============================================================================
bool CheckPopSeq(const int popOrder[], int n) {
    Stack S;
    SInit(S);
    int next = 1;                                            // 下一个待入栈的元素
    for (int i = 0; i < n; ++i) {
        // 一直入栈，直到栈顶正好是目标元素
        while (next <= n && (SEmpty(S) || S.data[S.top] != popOrder[i])) {
            if (S.top == STACK_MAX - 1) return false;
            SPush(S, next++);
        }
        if (SEmpty(S) || S.data[S.top] != popOrder[i]) return false;
        int t = 0;
        SPop(S, t);
    }
    return true;
}

// ============================================================================
// 5) 进制转换（2 <= base <= 16，字母大写）：取余入栈，再出栈
// ============================================================================
void DecimalToBase(int n, int base, char *out) {
    const char *digits = "0123456789ABCDEF";
    if (n == 0) { out[0] = '0'; out[1] = '\0'; return; }
    Stack S;
    SInit(S);
    while (n > 0) { SPush(S, n % base); n /= base; }
    int k = 0, t = 0;
    while (SPop(S, t)) out[k++] = digits[t];
    out[k] = '\0';
}

// ============================================================================
// 6) 判断回文串（用栈把前半段压栈，再和后半段比）
// ============================================================================
bool IsPalindrome(const char *s) {
    int n = (int)strlen(s);
    Stack S;
    SInit(S);
    for (int i = 0; i < n / 2; ++i) SPush(S, s[i]);
    for (int i = (n + 1) / 2; i < n; ++i) {
        int t = 0;
        SPop(S, t);
        if ((char)t != s[i]) return false;
    }
    return true;
}

// ============================================================================
int main() {
    banner("Stack : classic algorithms");

    expect_true("()[]{} ok", BracketsMatch("()[]{}"));
    expect_true("[]{}() ok", BracketsMatch("[]{}()"));
    expect_true("([)] bad", !BracketsMatch("([)]"));
    expect_true("(() bad", !BracketsMatch("(()"));
    expect_true("()) bad", !BracketsMatch("())"));
    expect_true("empty string ok", BracketsMatch(""));

    char buf[64];
    InfixToPostfix("(a+b)*c-d", buf);
    expect_str("infix (a+b)*c-d", buf, "ab+c*d-");
    InfixToPostfix("A+B*(C-D)-E/F", buf);
    expect_str("infix A+B*(C-D)-E/F", buf, "ABCD-*+EF/-");

    expect_eq("eval 34+5*", EvalPostfix("34+5*"), 35);
    expect_eq("eval 23+45-*", EvalPostfix("23+45-*"), -5);
    expect_eq("eval 123*+", EvalPostfix("123*+"), 7);

    int ok5[]  = {4, 5, 3, 2, 1};
    int bad5[] = {4, 3, 5, 1, 2};
    int bad5b[] = {3, 2, 1, 5, 4};               // 合法
    expect_true("{4,5,3,2,1} valid", CheckPopSeq(ok5, 5));
    expect_true("{4,3,5,1,2} invalid", !CheckPopSeq(bad5, 5));
    expect_true("{3,2,1,5,4} valid", CheckPopSeq(bad5b, 5));
    int asc[] = {1, 2, 3, 4, 5};
    expect_true("{1,2,3,4,5} valid", CheckPopSeq(asc, 5));

    DecimalToBase(10, 2, buf);
    expect_str("10 -> base 2", buf, "1010");
    DecimalToBase(255, 16, buf);
    expect_str("255 -> base 16", buf, "FF");
    DecimalToBase(0, 2, buf);
    expect_str("0 -> base 2", buf, "0");

    expect_true("\"abba\" is palindrome", IsPalindrome("abba"));
    expect_true("\"abcba\" is palindrome", IsPalindrome("abcba"));
    expect_true("\"abca\" is not palindrome", !IsPalindrome("abca"));

    return finish();
}
