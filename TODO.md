# 任务看板 | Task Board

**当前里程碑 / Current Milestone**: M2 — 核心机制可运行（第2周）
**最后更新 / Last Updated**: 2026-03-14
**维护者 / Maintainer**: CIO-TD
**数据来源 / Source**: Project log.docx + 2026-03-14 站会

---

## 🔴 本周必须完成（截止 3.20）| Must Complete This Week (Due 3.20)

### 美术线 / Art Track
- [ ] [戴博远/Drawer] 地图美术优化（手绘像素风格适配 64×64 格子）
- [ ] [戴博远/Drawer] 怪物精灵图设计与绘制（至少 1 种敌人的行走动画帧）
- [ ] [戴博远/Drawer] 炮塔精灵图设计与绘制（至少 1 种炮塔的静态素材）
- [ ] [徐世昌/CEO] 协助地图/炮塔美术绘制

### 工程线 / Engineering Track
- [ ] [张嘉毓/Programmer] 地图改进 + 美术素材嵌入代码（替换纯色矩形为贴图）
- [ ] [张嘉毓/Programmer] 实现防御塔/怪物的基本素材显示（用新素材替换圆形/方块占位符）
- [ ] [张嘉毓/Programmer] 将原型代码（`defense.Inc/`）迁移至 `src/` 新结构（对齐命名规范）

### 设计线 / Design Track
- [x] [胡昊泽/Designer] ✅ 机制设计 v1.0 已提交（13 炮塔 + 基地塔 + 3 时代体系 + 升级路径）
- [ ] [胡昊泽/Designer] 补充敌人种类设计（三时代敌人）
- [ ] [胡昊泽/Designer] 定义资源系统（柴火、金属、材料的获取与消耗规则）
- [ ] [胡昊泽/Designer] 确认各炮塔建造费用与核心数值

### CIO / 文档线
- [ ] [符雅翔/CIO] 完成 TDD_enemy.md v1.0
- [ ] [符雅翔/CIO] 完成 TDD_tower.md v1.0
- [ ] [符雅翔/CIO] 完成 TDD_projectile.md v1.0

### 沟通线 / Communication Track
- [ ] [张一奥/Communicator] 更新进度追踪表，确保各角色本周任务清晰

---

## 🟡 进行中 | In Progress

- [ ] [戴博远/Drawer] 地图、动画、怪物美术设计（本周重点）
- [x] [胡昊泽/Designer] ✅ 机制设计 v1.0 已提交（2026-03-14）
- [ ] [符雅翔/CIO] TDD 文档批量编写（enemy → tower → projectile → ui）
- [ ] [张嘉毓/Programmer] 代码迁移与素材集成准备

---

## 🟢 已完成 | Done

### M1 架构阶段（3.06—3.13）

**工程 / Engineering**
- [x] [张嘉毓/Programmer] 原型代码开发与调试，基础游戏可运行（`defense.Inc/`）
- [x] [张嘉毓/Programmer] 解决 C + Raylib Windows 兼容性问题
- [x] [张嘉毓/Programmer] 向美术画师提供 Raylib 技术参数（窗口分辨率、格子大小等）

**美术 / Art**
- [x] [戴博远/Drawer] 初版地图绘制完成
- [x] [戴博远/Drawer] 修订地图尺寸适配 Raylib 窗口参数

**文档 / Documentation**
- [x] [符雅翔/CIO] 项目目录结构初始化（src/ docs/ assets/ lib/）
- [x] [符雅翔/CIO] README.md 完整版发布（含实名团队信息）
- [x] [符雅翔/CIO] CHANGELOG.md 初始化
- [x] [符雅翔/CIO] CODING_STANDARD.md 发布
- [x] [符雅翔/CIO] GIT_WORKFLOW.md 发布（含 GitHub 操作命令）
- [x] [符雅翔/CIO] TECH_STACK.md 发布（Raylib + 视角 + 美术风格已锁定）
- [x] [符雅翔/CIO] TEAM_ROLES.md v2.0 发布（实名 + 实际角色分工）
- [x] [符雅翔/CIO] TDD_map.md v1.0 草稿完成
- [x] [符雅翔/CIO] 所有模块骨架 .h 文件创建
- [x] [符雅翔/CIO] Makefile 创建
- [x] [符雅翔/CIO] .gitignore 更新
- [x] [符雅翔/CIO] GAME_DESIGN.md 模板创建（含原型数值参考）
- [x] [符雅翔/CIO] STORY_AND_UI_TEXT.md 模板创建
- [x] [符雅翔/CIO] Raylib 技术调研完成

**管理 / Management**
- [x] [徐世昌/CEO] 第1次团队会议（3.06 角色分工、主题确认）
- [x] [徐世昌/CEO] 第2次团队会议（3.13 进度评审、下周任务明确）
- [x] [徐世昌/CEO] 里程碑计划制定
- [x] [张一奥/Communicator] 进度追踪表建立、会议出席管理
- [x] [胡昊泽/Designer] 初版玩法提案、主题分析报告提交

**决策 / Decisions**
- [x] 游戏视角确认：半俯视视角（2026-03-14）
- [x] 美术风格确认：手绘像素风格（2026-03-14）
- [x] 美术参考工具确认：Unity（仅用于参考，非引擎）

---

## ⛔ 阻塞中 | Blocked

- [ ] [张嘉毓/Programmer] 素材集成（地图贴图渲染）
  → **阻塞原因**：等待美术画师（戴博远）完成地图贴图素材
  → **预计解除**：3.20 前

- [ ] [符雅翔/CIO] TDD_tower.md 最终版
  → **阻塞原因**：机制设计 v1.0 已提交，但数值尚未锁定，待组长确认分阶段实现策略
  → **预计解除**：组长确认 M2-M3 优先实现的炮塔后

---

## 📬 待组长决策 | Pending CEO Decision

- [ ] 最终关卡数量：目前已完成 2 张地图，后续持续扩展，最终数量待定
- [x] ~~炮塔种类数量~~：✅ 已确认 13 种 + 1 基地塔（2026-03-14，机制设计 v1.0）
- [ ] GitHub 仓库是否公开（Public）？

---

## 📅 里程碑规划（已更新）| Milestone Plan (Updated)

```
M1（第1周 3.06—3.13）：架构完成 ✅ 已达标
  ✅ 原型代码可运行、初版地图完成
  ✅ 文档体系建立、目录结构规范化

M2（第2周 3.14—3.20）：核心机制可运行 ← 当前
  目标：地图美术优化、怪物/炮塔素材完成、素材嵌入代码、
       防御塔/怪物基本显示、机制设计 v1.0 完成

M3（第3周 3.21—3.27）：完整游戏循环
  目标：波次系统完整、UI 完善、GAME OVER / 通关画面、
       UI 文字最终版集成、数值平衡调整

M4（第4周 3.28—4.03）：打磨与收尾
  目标：Bug 修复、视觉细节打磨、README 完善、
       CHANGELOG 最终整理、最终构建验收
```

---

*CIO-TD | Defence.Inc Project*
