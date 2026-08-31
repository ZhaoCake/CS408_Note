# -*- coding: utf-8 -*-
"""修复卡片挖空语法：错误 [T#数字#] -> 正确 [F#数字#]，并检测嵌套/编号问题"""
import os, re, glob

ROOT = r"d:/WorkSpace/CS408_Note"
CARD_ROOT = os.path.join(ROOT, "markji")

T_NUM = re.compile(r"\[T#(\d+)#")
F_TAG = re.compile(r"\[F#(\d+)#([^\]]*)\]")
NEST_IN_F = re.compile(r"\[F#\d+#[^\]]*\[(?:T|F|Choice|Pic|Audio|Card|E)#")  # F内容内嵌套

fixed = 0
nest_issues = []
num_issues = []

for fp in sorted(glob.glob(os.path.join(CARD_ROOT, "**", "*.txt"), recursive=True)):
    with open(fp, encoding="utf-8") as f:
        c = f.read()
    new = T_NUM.sub(lambda m: f"[F#{m.group(1)}#", c)
    if new != c:
        fixed += 1
    # 检测嵌套：F 内容内出现其他语法标签
    for m in NEST_IN_F.finditer(new):
        nest_issues.append((fp, m.group(0)[:40]))
    # 检测编号规范性：F 编号应是从 1 开始的正整数
    nums = [int(m.group(1)) for m in F_TAG.finditer(new)]
    if nums:
        seen = set(nums)
        # 编号应连续（1..max），且从1开始
        if min(nums) != 1 or seen != set(range(1, max(nums) + 1)):
            num_issues.append((fp, sorted(seen)))
    if new != c:
        with open(fp, "w", encoding="utf-8") as f:
            f.write(new)

print(f"已修复 [T#数字#] -> [F#数字#] 的卡片: {fixed}")
print(f"F 内容嵌套其他语法的卡片: {len(nest_issues)}")
for fp, frag in nest_issues[:20]:
    print(f"  NEST {os.path.basename(fp)}: {frag}")
print(f"F 编号不连续/不从1开始的卡片: {len(num_issues)}")
for fp, nums in num_issues[:20]:
    print(f"  NUM  {os.path.basename(fp)}: {nums}")
