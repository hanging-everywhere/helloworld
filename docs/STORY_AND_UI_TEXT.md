# 故事与 UI 文字文档 | Story & UI Text Document

**维护者 / Maintainer**: 故事设计者 / Narrative Designer
**版本 / Version**: 0.1（模板 / Template）
**状态 / Status**: 🔴 待填写 / Pending — 请故事设计者在 M1 结束前完成初稿
**最后更新 / Last Updated**: 2026-03-10（CIO 创建模板 / Template created by CIO）

> **CIO 说明**：以下部分文字来自原型代码（英文占位文字），供故事设计者参考和替换。
> 涉及动态变量（`%d`、`%.1f` 等）的文字请勿修改格式符，只修改周围文字。
> **CIO Note**: Some text below is from the prototype code (English placeholders) for the narrative designer's reference.
> For text with format specifiers (`%d`, `%.1f`, etc.), do NOT modify the specifier — only the surrounding text.

---

## 1. 世界观简介 | World Overview

> 请故事设计者在此处填写游戏背景故事，100字以内。
> Please fill in the game background story here, within 100 characters.

**中文版本**（待填写）：

> 远古时代，部落在险峻的峡谷中安营扎寨。每当黑夜降临，史前猛兽便从四面八方涌来，觊觎部落的篝火与食物。作为守护者，你必须建造石质防御工事，让猛兽寸步难行，守住这最后的火种。

**English Version**（待填写 / Pending）：

> [Narrative designer: please provide English version]

---

## 2. 游戏标题 | Game Title

| 位置 / Location | 中文文字 / Chinese Text | 英文文字 / English Text | 备注 / Notes |
|---|---|---|---|
| 窗口标题 / Window Title | 千年城影 · 旧石器时代防线 | Millennium Shadow · Paleolithic Defense Line | 已在原型中使用 |
| 主菜单标题 / Main Menu Title | ❓待填写 | ❓Pending | 暂无主菜单界面 |

---

## 3. UI 文字清单 | UI Text List

> **CIO 提示**：带 `%d`、`%.1f` 等格式符的文字，格式符必须保留，只修改周围文字。
> **CIO Note**: Keep format specifiers (`%d`, `%.1f`) unchanged — only modify surrounding text.

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

## 5. 故事设计者操作指南 | Guide for Narrative Designer

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
git commit -m "docs: 更新 STORY_AND_UI_TEXT.md，完成所有 UI 文字初稿"

# ⑤ 推送并发起 PR
git push -u origin docs/update-story-and-ui-text
# 在 GitHub 发起 PR，Reviewer 指定 CIO
```

### 5.2 重要格式规则 | Important Format Rules

```
✅ 保留所有 %d、%.1f、%s 等格式符不变
   Keep all %d, %.1f, %s format specifiers unchanged

✅ 中英文字均需提供（工程师可能用到任一版本）
   Provide both Chinese and English text

✅ 文字长度尽量适中，避免超出 UI 框
   Keep text length reasonable — avoid overflowing UI areas

❌ 不要修改格式符本身（如将 %d 改为 %s）
   Do NOT change the specifier type (e.g., %d to %s)

❌ 不要删除格式符（如将 "第 %d 波" 改为 "第三波"——工程师需要动态数字）
   Do NOT remove specifiers — engineers need dynamic values
```

---

## 6. 待确认事项 | Pending Items

| 事项 / Item | 优先级 / Priority | 截止 / Deadline |
|---|---|---|
| 世界观简介（中英双语）| 🔴 高 | M1 结束 |
| 所有 HUD 文字最终版 | 🔴 高 | M1 结束 |
| GAME OVER / 胜利画面文字 | 🔴 高 | M1 结束 |
| 是否需要"开始游戏"主菜单？| 🟡 中 | M2 前确认 |

---

*维护者：故事设计者（模板由 CIO-TD 创建）*
*Maintained by: Narrative Designer (template created by CIO-TD)*
