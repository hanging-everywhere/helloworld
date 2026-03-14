# 故事与 UI 文字文档 | Story & UI Text Document

**维护者 / Maintainer**: 张一奥（Zhang Yi'ao）/ Communicator（协助收集整理 / Assists with collection）
**版本 / Version**: 0.3（时代世界观已填入 / Era world lore added）
**状态 / Status**: 🟡 部分填写 / Partially filled — 请在 M3 结束前完成 UI 文字最终版
**最后更新 / Last Updated**: 2026-03-14（CIO 根据机制设计 v1.0 更新 / Updated per mechanism design v1.0）

> **CIO 说明**：以下部分文字来自原型代码（英文占位文字），供参考和替换。
> 涉及动态变量（`%d`、`%.1f` 等）的文字请勿修改格式符，只修改周围文字。
> **CIO Note**: Some text below is from prototype code. For format specifiers (`%d`, `%.1f`), do NOT modify the specifier.

---

## 1. 世界观简介 | World Overview

### 1.1 总体背景 | Overall Background

**中文版本**：

> 千年城影横跨三个时代——从石器时代的蒙昧，到古代文明的理性，再到科幻未来的星辰。每个时代都有独特的防御哲学：石器时代以石、骨、木、火对抗蛮荒猛兽；古代文明用金属、机械、工艺、秩序构筑精密防线；科幻未来则以能量、力场、意识、维度本身作为武器。守护者必须跨越时代，建造防御工事，守住文明的火种。

**English Version**：

> Millennium Shadow spans three eras — from the Ignorance of the Stone Age, to the Rationality of Ancient Civilization, to the Stars of the Sci-Fi Future. Each era has a unique defense philosophy: the Stone Age fights savage beasts with stone, bone, wood, and fire; Ancient Civilization builds precise defenses with metal, machinery, craft, and order; the Sci-Fi Future wields energy, force fields, consciousness, and dimensions as weapons. Guardians must transcend eras to build defenses and protect the flame of civilization.

### 1.2 各时代世界观 | Era-Specific World Lore

**第一时代 · 石器时代 · 蒙昧 / Era 1: Stone Age — Ignorance**

> 石、骨、木、火。
> 原始人的蒙昧认知，造就了最朴素的防御，同时对自然的崇拜，获得了神的青睐。

> Stone, bone, wood, fire.
> Primitive humanity's naive understanding creates the most basic defenses, while nature worship earns divine favor.

**第二时代 · 古代文明 · 理性 / Era 2: Ancient Civilization — Rationality**

> 金属、机械、工艺、秩序。
> 逐渐理解物理规律，用数学和智慧搭建攻防工程。

> Metal, machinery, craft, order.
> Gradually understanding physical laws, using mathematics and wisdom to build offensive and defensive engineering.

**第三时代 · 科幻未来 · 星辰 / Era 3: Sci-Fi Future — Stars**

> 能量、力场、意识、维度。
> 用概念和规律本身作为武器。

> Energy, force fields, consciousness, dimensions.
> Using concepts and laws themselves as weapons.

---

## 2. 游戏标题 | Game Title

| 位置 / Location | 中文文字 / Chinese Text | 英文文字 / English Text | 备注 / Notes |
|---|---|---|---|
| 窗口标题 / Window Title | 千年城影 · 旧石器时代防线 | Millennium Shadow · Paleolithic Defense Line | 已在原型中使用 |
| 主菜单标题 / Main Menu Title | ❓待填写 | ❓Pending | 暂无主菜单界面 |

---

## 3. UI 文字清单 | UI Text List

> **CIO 提示**：带 `%d`、`%.1f` 等格式符的文字，格式符必须保留，只修改周围文字。
> **CIO Note**: Keep format specifiers unchanged — only modify surrounding text.

### 3.1 游戏中 HUD | In-Game HUD

| 位置 / Location | 当前原型文字 / Prototype Text | 最终文字 / Final Text | 格式说明 / Format Note |
|---|---|---|---|
| 状态栏 / Status Bar | `Base HP: %d    Money: %d    Wave: %d/3` | ❓待填写 | `%d` 分别为血量、金钱、波次编号 |
| 资源不足提示 / Insufficient Gold | `Need 50 Money!` | ❓待填写 | 浮动飘字，持续约1秒 |
| 下波倒计时 / Next Wave Countdown | `Next Wave in: %.1f s` | ❓待填写 | `%.1f` 为倒计时秒数（保留1位小数）|

### 3.2 游戏结果画面 | Game Result Screen

| 位置 / Location | 当前原型文字 / Prototype Text | 最终文字 / Final Text | 备注 / Notes |
|---|---|---|---|
| 游戏失败标题 / Game Over Title | `GAME OVER` | ❓待填写 | 大字体，红色 |
| 游戏失败副标题 / Game Over Subtitle | `The fire went out...` | ❓待填写 | 小字体，浅灰色 |
| 游戏胜利标题 / Victory Title | `LEVEL 1 CLEARED` | ❓待填写 | 大字体，绿色 |
| 游戏胜利副标题 / Victory Subtitle | `Humanity survived the night...` | ❓待填写 | 小字体，浅灰色 |

### 3.3 波次提示（如需增加）| Wave Alert (If Needed)

| 位置 / Location | 文字格式 / Text Format | 最终文字 / Final Text | 格式说明 / Format Note |
|---|---|---|---|
| 波次开始提示 / Wave Start | （暂无 / None yet）| ❓待填写（可选）| 可选功能，如有需要 |

---

## 4. 伤害数字与特效文字 | Damage Numbers & Effect Text

| 类型 / Type | 格式 / Format | 说明 / Notes | CIO 备注 |
|---|---|---|---|
| 伤害飘字 / Damage Float | `-%d` | `%d` 为实际伤害数值，红色，向上飘动 | 格式已锁定，勿修改 |

---

## 5. 提交操作指南 | Submission Guide

### 5.1 如何提交文案更新 | How to Submit Text Updates

```bash
# ① 同步最新代码
git checkout dev
git pull origin dev

# ② 新建文档分支
git checkout -b docs/update-story-and-ui-text

# ③ 编辑此文件（STORY_AND_UI_TEXT.md）
# 直接修改"最终文字"列

# ④ 提交
git add docs/STORY_AND_UI_TEXT.md
git commit -m "docs: 更新 STORY_AND_UI_TEXT.md，完成 UI 文字"

# ⑤ 推送并发起 PR
git push -u origin docs/update-story-and-ui-text
# 在 GitHub 发起 PR，Reviewer 指定 CIO（符雅翔）
```

### 5.2 重要格式规则 | Important Format Rules

```
✅ 保留所有 %d、%.1f、%s 等格式符不变
   Keep all format specifiers unchanged

✅ 中英文字均需提供（工程师可能用到任一版本）
   Provide both Chinese and English text

✅ 文字长度尽量适中，避免超出 UI 框
   Keep text length reasonable

❌ 不要修改格式符本身（如将 %d 改为 %s）
   Do NOT change the specifier type

❌ 不要删除格式符（如将 "第 %d 波" 改为 "第三波"）
   Do NOT remove specifiers — engineers need dynamic values
```

---

## 6. 待确认事项 | Pending Items

| 事项 / Item | 优先级 / Priority | 截止 / Deadline |
|---|---|---|
| 所有 HUD 文字最终版 | 🟡 中 | M3 结束（3.27）|
| GAME OVER / 胜利画面文字 | 🟡 中 | M3 结束（3.27）|
| 世界观英文版本 | 🟡 中 | M3 结束（3.27）|
| 是否需要"开始游戏"主菜单？| 🟡 中 | M2 前确认 |

---

*维护者：张一奥 / Communicator（模板由 CIO-TD 创建并更新）*
*Maintained by: Zhang Yi'ao / Communicator (template created and updated by CIO-TD)*
