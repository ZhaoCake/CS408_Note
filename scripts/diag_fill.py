# -*- coding: utf-8 -*-
"""诊断卡片挖空使用情况：统计每张卡 [F#数字#] / [T#数字#] / 无挖空"""
import os, re, glob

ROOT = r"d:/WorkSpace/CS408_Note"
CARD_ROOT = os.path.join(ROOT, "markji")

F_PAT = re.compile(r"\[F#\d+#")     # 正确挖空
T_NUM_PAT = re.compile(r"\[T#\d+#")  # 错误挖空（T 标签带数字）
T_VALID = re.compile(r"\[T#(?:B|I|U|!|!!|up|down|link)")  # 合法 T 样式

def stats(course):
    base = os.path.join(CARD_ROOT, course)
    f_ok = t_bad = none_cnt = total = 0
    bad_files = []
    none_files = []
    for fp in sorted(glob.glob(os.path.join(base, "**", "*.txt"), recursive=True)):
        total += 1
        with open(fp, encoding="utf-8") as f:
            c = f.read()
        has_f = F_PAT.search(c)
        has_tnum = T_NUM_PAT.search(c)
        if has_f:
            f_ok += 1
        elif has_tnum:
            t_bad += 1
            bad_files.append(fp)
        else:
            none_cnt += 1
            none_files.append(fp)
    return total, f_ok, t_bad, bad_files, none_cnt, none_files

for course in ["计算机网络", "操作系统", "计算机组成原理", "数据结构"]:
    total, f_ok, t_bad, bad_files, none_cnt, none_files = stats(course)
    print(f"== {course} ==")
    print(f"  总卡 {total} | 正确挖空[F] {f_ok} | 错误挖空[T#数字] {t_bad} | 完全无挖空 {none_cnt}")
    if bad_files:
        print(f"  错误挖空卡示例(前3): {[os.path.basename(x) for x in bad_files[:3]]}")
    if none_files:
        print(f"  无挖空卡示例(前5): {[os.path.basename(x) for x in none_files[:5]]}")
