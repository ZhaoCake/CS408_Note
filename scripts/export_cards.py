# -*- coding: utf-8 -*-
"""
墨墨记忆卡批量导入文本导出脚本

将 markji/<课程>/ 下各章的卡片 txt 合并为墨墨记忆卡支持的批量导入格式：
  - "### 章节名称" 指定章节
  - "===" 分割卡片

用法：python scripts/export_cards.py
输出：markji/导出/<课程>.txt  （四个科目各一份）
"""
import os
import re

ROOT = r"d:/WorkSpace/CS408_Note"
CARD_ROOT = os.path.join(ROOT, "markji")
OUT_DIR = os.path.join(ROOT, "markji", "导出")

COURSES = ["计算机网络", "操作系统", "计算机组成原理", "数据结构"]


def collect_chapters(course):
    """收集课程下所有章节：[(章号, 章目录名, [卡片文件绝对路径, ...])]，按章号排序"""
    chapters = []
    base = os.path.join(CARD_ROOT, course)
    if not os.path.isdir(base):
        return chapters
    for d in sorted(os.listdir(base)):
        m = re.match(r"第(\d+)章", d)
        if not m:
            continue
        p = os.path.join(base, d)
        if not os.path.isdir(p):
            continue
        cards = [os.path.join(p, f) for f in sorted(os.listdir(p))
                 if f.endswith(".txt")]
        if cards:
            chapters.append((int(m.group(1)), d, cards))
    chapters.sort(key=lambda x: x[0])
    return chapters


def export_course(course):
    """导出单科批量导入文本"""
    chapters = collect_chapters(course)
    if not chapters:
        print(f"[SKIP] {course}：无卡片目录")
        return None

    out = []
    n_chapters = len(chapters)
    for ci, (ch_no, title, files) in enumerate(chapters):
        out.append(f"### {title}")
        for fi, fp in enumerate(files):
            with open(fp, encoding="utf-8") as f:
                content = f.read().strip()
            if not content:
                continue
            out.append(content)
            # 卡片之间用 === 分割；全文件最后一张卡后不加
            is_last = (ci == n_chapters - 1) and (fi == len(files) - 1)
            if not is_last:
                out.append("===")

    text = "\n".join(out) + "\n"
    os.makedirs(OUT_DIR, exist_ok=True)
    out_path = os.path.join(OUT_DIR, f"{course}.txt")
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(text)
    n_cards = sum(len(files) for _, _, files in chapters)
    print(f"[OK] {course}  章节{len(chapters)} 卡片{n_cards}  -> {out_path}")
    return out_path


def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    for course in COURSES:
        export_course(course)
    print(f"\n导出完成，输出目录：{OUT_DIR}")


if __name__ == "__main__":
    main()
