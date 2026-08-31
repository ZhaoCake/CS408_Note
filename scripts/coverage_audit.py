# -*- coding: utf-8 -*-
"""
408 制卡覆盖率审计 v2（四通道）
  ① 正向覆盖：EX 考点条目 -> 卡片（找漏卡）
  ② 笔记补漏：笔记高亮/加粗/注意/表格 -> 卡片（核对补漏承诺）
  ③ 反向溯源：卡片挖空词 -> 笔记（防无依据卡片）
  ④ 词频盲区：笔记高频词 -> 卡片（盲区探测）

改进（v2）：
  - 扩充停用词表，降低通用词噪声
  - 清洗 latex 残留（\\rightarrow 等）与短英文 token
  - 跨章匹配：EX 条目若在同课程其他章卡片命中，标记"跨章"而非漏卡
  - 状态判定收紧：>=0.6 已覆盖 / 0.3~0.6 部分 / <0.3 未覆盖

用法：python scripts/coverage_audit.py
输出：markji/覆盖率报告/README.md + markji/覆盖率报告/<课程>/<章>.md
依赖：jieba
"""
import os
import re
import logging
from collections import Counter

import jieba

jieba.setLogLevel(logging.ERROR)

ROOT = r"d:/WorkSpace/CS408_Note"

STOPWORDS = set("""的 了 和 与 是 在 及 或 而 也 都 对 为 被 把 让 就 又 并 但 其 该 此 每 各 中 上 下 内 外 等 之 于 从 向 以 已 将 再 且 若 则 即 一 二 三 四 五 六 七 八 九 十 百 千 万 亿 第 会 能 要 可 应 些 种 项 类 级 层 章 节 条 上述 以下 如下 综上 例如 比如 所谓 常见 主要 基本 重要 大致 左右 另外 此外 其中 并且 而且 然而 但是 因此 所以 由于 如果 虽然 即使 无论 不管 只要 除非 更加 十分 非常 较为 相对 绝对 完全 全部 所有 任何 每个 各个 各种 一些 若干 多个 部分 整体 相关 相应 具体 特定 一定 一般 通常 正常 特殊 特别 直接 间接 可能 必然 等等 一个 一种 一类 之间 之后 之前 以上 以下 可以 需要 进行 通过 使用 用于 表示 称为 分为 包括 具有 实现 提供 完成 产生 发生 存在 根据 对于 作为 目标 位置 指向 情况 内容 方面 区别 不同 相同 比较 对比 是否 不会 变为 直到 然后 最后 首先 其次 或者 以及 并且 同时 两者 两者 它们 我们 你们 它们 所谓 所谓 相应 特定 某种 某些 其他 其余 各个 各自 此时 这时 当 如果 假如 假设 决定 判断 选择 得出 结论 方法 方式 过程 结果 作用 特点 类型 结构 概念 功能 问题 条件 原因 目的 对象 单位 时间 数量 数目 个数 各种 每个 每次 一直 始终 不断 逐渐 容易 很难 必须 只能 不能 可能 应该 需要 不需 无需 不用 得到 找到 取出 转入 加上 减去 乘以 除以 算出 求出 给出 设为 改为 变为 称为 视作 看作 认为 知道 说明 描述 指出 显示 表明 强调 注意 提醒 记住 复习 考点 考察 考试 真题 考研 选择 题目 解答 答案 例 比如 例如 设 若 则 即 可见 可知 综上 总之 显然 大概 大约 左右 上下 以内 以外 以上 以下 之前 之后 当中 其中 之三 之一 之二 一部分 另一 另一个 另一个 单独 分别 各自 相应 依次 顺序 逆序 正序 反序 排序 比较 相比 相对 绝对 完全 彻底 基本 几乎 大概 大多 多数 少数 少量 大量 全部 整个 总共 合计 一共 至少 最多 最少 最长 最短 最快 最慢 最高 最低 最大 最小 增加 减少 提高 降低 上升 下降 扩大 缩小 增强 减弱 保持 维持 改变 变化 存在 出现 产生 形成 构成 组成 包含 含有 涉及 包括 相关 无关 有关 关于 对于 针对 围绕 展开 讨论 分析 研究 比较 对比 划分 分类 归纳 总结 概括 提取 归纳
因为 那么 已经 这个 从而 自己 为了 这些 那些 什么 怎么 如何 是否 有些 有时 经常 往往 总之 另外 此外 而且 并且 虽然 但是 不过 然而 否则 到时 再看 再看 也许 或许 大概 基本 几乎 很难 容易 简单 复杂 类似 相似 相近 相关 相同 不同 一样 完全 彻底 充分 比较 相当 较为 更加 更 最 非常 特别 十分 极 尤其 尤为 其次 首先 最后 然后 接着 同时 之后 之前 以后 以前 时候 时 期 初期 后期 中期 早期 最近 现在 当前 目前 曾经 以往 过去 未来 将要 即将 已经 正在 一直 始终 经常 偶尔 有时 往往 仍然 依旧 还是 已经 早已 刚刚 马上 立刻 立即 随后 顿时 忽然 突然 逐渐 渐渐 慢慢 迅速 快速 缓慢 迟迟 尽快 尽量 尽可能 无论如何 不管怎样 总归 总算 终究 到底 究竟 到底 结果 最终 最后 最终 因此 因而 于是 从而 故而 以致 以至于 使得 造成 引起 导致 带来 产生 出现 发生 存在 拥有 具有 持有 包含 含有 涉及 关联 联系 相关 关系 方面 层面 角度 视角 维度 范围 领域 范畴 界 界限 边界 限度 极限 限制 制约 约束 规定 要求 标准 准则 原则 规范 惯例 常规 通常 一般 普通 平常 常规 常见 特殊 特别 特定 具体 抽象 一般性 特殊性 典型 代表性 标志性 主要 次要 核心 关键 重点 焦点 热点 难点 重点 侧重 优先 侧重 倾斜 偏向 倾向 趋向 趋势 走向 方向 导向 目标 目的 宗旨 使命 任务 职责 职能 功能 作用 效用 用途 价值 意义 影响 效果 效率 效益 成果 结果 后果 结局 成效 绩效 业绩 成绩 成就 进步 发展 变化 演变 演化 进化 升级 提高 提升 增强 扩大 缩小 降低 减少 减弱 下降 上升 增加 增多 提升 增强 变多 变少 变大 变小 变快 变慢 更早 更晚 更短 更长 更快 更慢 更多 更少 更大 更小 更高 更低 更强 更弱 更新 更旧 更近 更远 提前 延后 推迟 压缩 延长 缩短 加快 放慢 加速 减速 提高 降低 增高 减小 增大 减少 增加 上升 下降 保持 维持 稳定 持续 延续 继续 中断 暂停 停止 终止 结束 完成 完毕 收尾 收束 收场 了结 了断 处理 解决 应对 对付 敷衍 应付 处置 安置 安排 部署 规划 计划 打算 筹划 筹谋 设计 构思 设想 构架 架构 框架 体系 系统 结构 布局 配置 设定 设置 建立 创建 构造 构成 形成 组成 组合 集合 汇总 归并 归总 归拢 归整 汇总 合计 总计 总量 总数 总额 总合 和 合计 共计 总共 统共 一共 合计""".split())

# latex 残留命令（清洗后整体移除）
LATEX_CMDS = {
    "rightarrow": "", "leftarrow": "", "times": "", "div": "", "frac": "", "cdot": "",
    "leq": "≤", "geq": "≥", "neq": "≠", "times2": "", "Rightarrow": "",
}

COURSES = [
    {
        "name": "计算机网络",
        "ex_dir": "ComputerNetwork/EX-选择题考点总结",
        "note_dir": "ComputerNetwork/课程笔记",
        "markji": "markji/计算机网络",
        "chapters": [
            (1, "EX-第一章总结.md", "0-summary.md"),
            (2, "EX-第二章总结.md", "1-physical-layer.md"),
            (3, "EX-第三章总结.md", "2-data-link-layer.md"),
            (4, "EX-第四章总结.md", "3-network-layer.md"),
            (5, "EX-第五章总结.md", "4-transport-layer.md"),
            (6, "EX-第六章总结.md", "5-application-layer.md"),
        ],
    },
    {
        "name": "操作系统",
        "ex_dir": "OperateSystem/EX-选择题考点总结",
        "note_dir": "OperateSystem/课程笔记",
        "markji": "markji/操作系统",
        "chapters": [
            (1, "EX-第一章考点总结.md", "0-summary.md"),
            (2, "EX-第二章考点总结 .md", "1-process-management.md"),
            (3, "EX-第三章考点总结.md", "2-memory-management.md"),
            (4, "EX-第四章考点总结.md", "3-file-management.md"),
            (5, "EX-第五章考点总结.md", "4-device-management-ex.md"),
        ],
    },
    {
        "name": "计算机组成原理",
        "ex_dir": "ComputerOrganization/EX-考点总结",
        "note_dir": "ComputerOrganization/课程笔记",
        "markji": "markji/计算机组成原理",
        "chapters": [
            (1, "EX-第一章总结.md", "0-overview.md"),
            (2, "EX-第二章总结.md", "1-data-representation-and-operation.md"),
            (3, "EX-第三章总结.md", "2-storage-system.md"),
            (4, "EX-第四章总结.md", "3-instruction-system.md"),
            (5, "EX-第五章总结.md", "4-central-processing-unit.md"),
            (6, "EX-第六章总结.md", "5-bus.md"),
            (7, "EX-第七章总结.md", "6-input-output-system.md"),
        ],
    },
    {
        "name": "数据结构",
        "ex_dir": "DataStructure/EX-选择题总结",
        "note_dir": "DataStructure/课程笔记",
        "markji": "markji/数据结构",
        "chapters": [
            (1, "__NONE__", "1-summary.md"),          # DS 第一章无 EX 文件，以笔记为主源
            (2, "EX-第二章考点总结.md", "2-linear-list.md"),
            (3, "EX-第三章考点总结.md", "3-stack_queue_and_array.md"),
            (4, "EX-第四章考点总结.md", "4-string.md"),
            (5, "EX-第五章考点总结.md", "5-tree_and_binary_tree.md"),
            (6, "EX-第六章考点总结.md", "6-graph.md"),
            (7, "EX-第七章考点总结.md", "7-search.md"),
            (8, "EX-第八章考点总结.md", "8-sort.md"),
        ],
    },
]


def clean_markup(text):
    """剥离 markdown / markji / latex 语法，返回纯文本（保留标签内容）"""
    text = re.sub(r"!\[[^\]]*\]\([^)]*\)", "", text)          # 图片
    text = re.sub(r"\[P#[^\]]*\]", "", text)                  # [P#H1#类型] 行首标签
    # 挖空/强调前缀 [T#1# [T#B# [T#!hex# [F#1# [E##：删除前缀、保留内容
    text = re.sub(r"\[(?:T|F|E)#(?:B|!\w+|#|\d+#)?", "", text)
    text = text.replace("[", " ").replace("]", " ")
    text = text.replace("$", "").replace("**", "").replace("==", "")
    text = text.replace("`", "").replace("*", "").replace("_", "")
    text = text.replace("###", "").replace("##", "").replace("#", "")
    text = re.sub(r"[|>]", " ", text)
    text = re.sub(r"[①②③④⑤⑥⑦⑧⑨⑩※★☆]", " ", text)
    # latex 命令：先保留常用数学函数名（log 等），其余删除
    text = re.sub(r"\\log\b", "log", text)
    text = re.sub(r"\\[a-zA-Z]+\s*", "", text)
    for k, v in LATEX_CMDS.items():
        text = text.replace(k, v)
    text = re.sub(r"\s+", " ", text)
    return text


def tokens_of(text, stopwords=None):
    if stopwords is None:
        stopwords = STOPWORDS
    out = []
    for w in jieba.lcut(text):
        w = w.strip()
        if len(w) < 2:
            continue
        if w.isdigit():
            continue
        if w.lower() in stopwords:
            continue
        if re.fullmatch(r"[\W_]+", w):
            continue
        # 短英文/数字混合 token（a1、br、ix、af996h）噪声，剔除长度<=2 纯小写或含数字短串
        if re.fullmatch(r"[a-z0-9]{1,2}", w):
            continue
        out.append(w.lower())
    return out


def kw_of(text):
    return set(tokens_of(clean_markup(text)))


def shared_filter(kw_sets, ratio=0.3):
    c = Counter()
    for s in kw_sets:
        for w in set(s):
            c[w] += 1
    n = len(kw_sets)
    common = {w for w, k in c.items() if n > 0 and k / n >= ratio}
    return [{w for w in s if w not in common} for s in kw_sets]


def hit_ratio(kws, corpus_lower):
    if not kws:
        return None
    hits = [w for w in kws if w in corpus_lower]
    return len(hits) / len(kws)


def extract_ex_items(path):
    items = []
    if not os.path.exists(path):
        return items
    with open(path, encoding="utf-8") as f:
        for line in f:
            m = re.match(r"^\s*(\d+(?:\.\d+)*)\.\s+(.+)", line)
            if not m:
                continue
            num, text = m.group(1), m.group(2).strip()
            if len(text) < 8:
                continue
            items.append((num, text))
    return items


def extract_note_points(path):
    points = []
    if not os.path.exists(path):
        return points
    with open(path, encoding="utf-8") as f:
        lines = f.readlines()
    for line in lines:
        s = line.strip()
        if s.startswith("```"):          # 跳过代码块
            continue
        if re.search(r"&nbsp;|&lt;|&gt;|&amp;", s):
            continue
        for frag in re.findall(r"==([^=\n]{2,})==", s):
            points.append(frag)
        for frag in re.findall(r"\*\*([^*\n]{2,})\*\*", s):
            if frag not in ("注意", "重点", "提示"):
                points.append(frag)
        if re.match(r">\s*", s) and ("注意" in s or "提醒" in s or "⚠" in s):
            points.append(re.sub(r"^>\s*", "", s))
        if s.startswith("|") and not re.match(r"^\|[\s:-]+\|", s):
            cells = [c.strip() for c in s.split("|")[1:-1] if len(c.strip()) >= 2]
            points.extend(cells)
    seen, out = set(), []
    for p in points:
        p = p.replace("&emsp;", " ").replace("&nbsp;", " ").strip()
        if re.fullmatch(r"[\$\\{}a-zA-Z0-9\s_^~|:.+=()【】]+", p):   # 纯公式/符号/标签碎片
            continue
        if re.match(r"^【(例题|解析|答案|注意|考点|提示)】", p):
            continue
        if len(p) < 4 or p in seen:
            continue
        seen.add(p)
        out.append(p)
    return out


def read_cards(card_dir):
    cards = []
    if not os.path.isdir(card_dir):
        return cards
    for fn in sorted(os.listdir(card_dir)):
        if not fn.endswith(".txt"):
            continue
        with open(os.path.join(card_dir, fn), encoding="utf-8") as f:
            raw = f.read()
        cards.append((fn, raw, clean_markup(raw)))
    return cards


def read_course_corpus(cfg, skip_ch):
    """全课程卡片语料（跨章匹配用，跳过本章）"""
    base = os.path.join(ROOT, cfg["markji"])
    parts = []
    if os.path.isdir(base):
        for d in os.listdir(base):
            m = re.match(r"第(\d+)章", d)
            if m and int(m.group(1)) == skip_ch:
                continue
            p = os.path.join(base, d)
            if os.path.isdir(p):
                for fn in os.listdir(p):
                    if fn.endswith(".txt"):
                        with open(os.path.join(p, fn), encoding="utf-8") as f:
                            parts.append(clean_markup(f.read()))
    return " ".join(parts).lower()


def find_card_dir(course_cfg, ch_no):
    base = os.path.join(ROOT, course_cfg["markji"])
    if not os.path.isdir(base):
        return None
    candidates = []
    for d in os.listdir(base):
        m = re.match(r"第(\d+)章", d)
        if m and int(m.group(1)) == ch_no:
            p = os.path.join(base, d)
            n = len([f for f in os.listdir(p) if f.endswith(".txt")]) if os.path.isdir(p) else 0
            candidates.append((p, n))
    if not candidates:
        return None
    candidates.sort(key=lambda x: x[1], reverse=True)
    return candidates[0][0]


def audit_chapter(cfg, ch_no, ex_fn, note_fn, card_dir):
    ex_path = os.path.join(ROOT, cfg["ex_dir"], ex_fn)
    note_path = os.path.join(ROOT, cfg["note_dir"], note_fn)
    cards = read_cards(card_dir)
    card_corpus = " ".join(c[2] for c in cards).lower()
    course_corpus = read_course_corpus(cfg, ch_no)

    # ---- ① 正向覆盖 ----
    ex_items = extract_ex_items(ex_path)
    kw_sets = shared_filter([kw_of(t) for _, t in ex_items])
    ex_rows = []
    for (num, text), kws in zip(ex_items, kw_sets):
        if not kws:
            continue
        r = hit_ratio(kws, card_corpus)
        if r is None:
            continue
        if r >= 0.6:
            status = "已覆盖"
        elif r >= 0.3:
            status = "部分"
        else:
            # 未覆盖：检查跨章
            r2 = hit_ratio(kws, course_corpus)
            if r2 is not None and r2 >= 0.6:
                status = "跨章"
            else:
                status = "未覆盖"
        miss_kw = [w for w in kws if w not in card_corpus and w not in course_corpus]
        ex_rows.append({"num": num, "text": clean_markup(text)[:60], "status": status,
                        "miss": "、".join(miss_kw[:10]) if miss_kw else ""})
    c1_cover = sum(1 for x in ex_rows if x["status"] == "已覆盖")
    c1_cross = sum(1 for x in ex_rows if x["status"] == "跨章")
    c1_part = sum(1 for x in ex_rows if x["status"] == "部分")
    c1_miss = sum(1 for x in ex_rows if x["status"] == "未覆盖")

    # ---- ② 笔记补漏 ----
    note_points = extract_note_points(note_path)
    np_kws = shared_filter([kw_of(p) for p in note_points], ratio=0.5)
    note_rows = []
    for p, kws in zip(note_points, np_kws):
        if not kws:
            continue
        r = hit_ratio(kws, card_corpus)
        if r is None:
            continue
        if r < 0.3:
            miss_kw = [w for w in kws if w not in card_corpus]
            note_rows.append({"point": p[:50], "miss": "、".join(miss_kw[:10])})

    # ---- ③ 反向溯源 ----
    note_text = ""
    if os.path.exists(note_path):
        with open(note_path, encoding="utf-8") as f:
            note_text = clean_markup(f.read()).lower()
    source_rows = []
    for fn, raw, _ in cards:
        blanks = re.findall(r"\[T#\d+#([^\]]+)\]", raw)
        if not blanks:
            continue
        kw = kw_of(" ".join(blanks))
        miss_kw = [w for w in kw if w not in note_text]
        if miss_kw:
            source_rows.append({"card": fn, "miss": "、".join(miss_kw[:10])})

    # ---- ④ 词频盲区 ----
    note_plain = clean_markup(note_text)
    cnt = Counter(tokens_of(note_plain))
    blind_rows = []
    for w, c in cnt.most_common(200):
        if w in STOPWORDS or w.isdigit():
            continue
        if w in course_corpus:          # 跨章已覆盖（如异常在第7章）
            continue
        if w not in card_corpus:
            blind_rows.append((w, c))
        if len(blind_rows) >= 25:
            break

    return {
        "ch": ch_no, "card_dir": card_dir, "cards": len(cards),
        "ex_total": len(ex_rows), "c1_cover": c1_cover, "c1_cross": c1_cross,
        "c1_part": c1_part, "c1_miss": c1_miss,
        "ex_rows": [x for x in ex_rows if x["status"] in ("未覆盖", "跨章")],
        "note_rows": note_rows, "source_rows": source_rows, "blind_rows": blind_rows,
    }


def write_chapter_report(cfg, ch_no, card_dir, r):
    d = os.path.join(ROOT, "markji", "覆盖率报告", cfg["name"])
    os.makedirs(d, exist_ok=True)
    title = os.path.basename(card_dir)
    total = r["ex_total"]
    pure_rate = f"{r['c1_cover'] / total * 100:.1f}%" if total else "N/A"
    with_rate = f"{(r['c1_cover'] + r['c1_part'] + r['c1_cross']) / total * 100:.1f}%" if total else "N/A"
    out = [f"# {cfg['name']} {title} 覆盖率报告", ""]
    out += [f"- 卡片数：{r['cards']}", f"- EX 考点条目：{total}",
            f"- 已覆盖 {r['c1_cover']} / 部分 {r['c1_part']} / 跨章 {r['c1_cross']} / 未覆盖 {r['c1_miss']}",
            f"- **完全覆盖：{pure_rate}；含部分+跨章：{with_rate}**", ""]

    out.append("## ① EX 考点：未覆盖 / 跨章")
    if r["ex_rows"]:
        out.append("| 条目 | 状态 | 未命中词 |")
        out.append("|---|---|---|")
        for x in r["ex_rows"]:
            out.append(f"| {x['num']} {x['text']} | {x['status']} | {x['miss']} |")
    else:
        out.append("无未覆盖 / 跨章条目。")
    out.append("")

    out.append("## ② 笔记补漏未命中（<30% 关键词命中）")
    if r["note_rows"]:
        out.append("| 笔记考点 | 未命中词 |")
        out.append("|---|---|")
        for x in r["note_rows"]:
            out.append(f"| {x['point']} | {x['miss']} |")
    else:
        out.append("笔记高亮/加粗/注意/表格考点均已被卡片覆盖。")
    out.append("")

    out.append("## ③ 反向溯源：挖空词在笔记中找不到依据")
    if r["source_rows"]:
        out.append("| 卡片 | 未在笔记出现的挖空词 |")
        out.append("|---|---|")
        for x in r["source_rows"]:
            out.append(f"| {x['card']} | {x['miss']} |")
    else:
        out.append("全部卡片挖空词均能在笔记中找到依据。")
    out.append("")

    out.append("## ④ 词频盲区：笔记高频但卡片未出现")
    if r["blind_rows"]:
        out.append("| 高频词 | 笔记次数 |")
        out.append("|---|---|")
        for w, c in r["blind_rows"][:20]:
            out.append(f"| {w} | {c} |")
    else:
        out.append("笔记高频词均已在卡片中出现。")
    out.append("")

    p = os.path.join(d, f"{title}.md")
    with open(p, "w", encoding="utf-8") as f:
        f.write("\n".join(out))
    return p


def main():
    os.makedirs(os.path.join(ROOT, "markji", "覆盖率报告"), exist_ok=True)
    all_rows = []
    for cfg in COURSES:
        for ch_no, ex_fn, note_fn in cfg["chapters"]:
            card_dir = find_card_dir(cfg, ch_no)
            if not card_dir:
                print(f"[SKIP] {cfg['name']} 第{ch_no}章：未找到 markji 目录")
                continue
            r = audit_chapter(cfg, ch_no, ex_fn, note_fn, card_dir)
            p = write_chapter_report(cfg, ch_no, card_dir, r)
            rate = r["c1_cover"] / r["ex_total"] * 100 if r["ex_total"] else None
            all_rows.append((cfg["name"], ch_no, os.path.basename(card_dir),
                             r["cards"], r["ex_total"], r["c1_miss"], r["c1_cross"], rate))
            rate_s = f"{rate:.1f}%" if rate is not None else "N/A"
            print(f"[OK] {cfg['name']} 第{ch_no}章  卡{r['cards']} EX{r['ex_total']} "
                  f"未覆盖{r['c1_miss']} 跨章{r['c1_cross']}  完全覆盖率{rate_s}")

    lines = ["# 408 制卡覆盖率总览", "",
             "> 由 `scripts/coverage_audit.py` 自动生成（四通道）。", ""]
    lines.append("| 课程 | 章 | 卡片数 | EX条目 | 未覆盖 | 跨章 | 完全覆盖率 |")
    lines.append("|---|---|---|---|---|---|---|")
    for name, ch, title, cards, exn, miss, cross, rate in all_rows:
        rate_s = f"{rate:.1f}%" if rate is not None else "N/A"
        lines.append(f"| {name} | {title} | {cards} | {exn} | {miss} | {cross} | {rate_s} |")
    with open(os.path.join(ROOT, "markji", "覆盖率报告", "README.md"), "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("\n汇总 -> markji/覆盖率报告/README.md")


if __name__ == "__main__":
    main()
