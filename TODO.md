# 任务看板 | Task Board

**当前里程碑 / Current Milestone**: M1 — 架构完成（第1周）
**最后更新 / Last Updated**: 2026-03-10
**维护者 / Maintainer**: CIO-TD

---

## 🔴 本周必须完成 | Must Complete This Week (M1)

- [ ] [CIO] 所有 TDD 文档 v1.0 锁定（map ✅ → enemy → tower → projectile → ui）
- [ ] [CIO] 将原型代码迁移计划整理为迁移说明，发给工程师
- [ ] [主工程师] 将 `defense.Inc/Map.c/h` 迁移至 `src/map/map.c/h`（对齐新命名规范）
- [ ] [主工程师] 将 `defense.Inc/Enemy.c/h` 迁移至 `src/enemy/enemy.c/h`
- [ ] [主工程师] 确认 `src/map/map.h` 中路径接口（`Map_getWaypoints`）最终形态
- [ ] [副工程师] 将 `defense.Inc/Tower.c/h` 迁移至 `src/tower/tower.c/h`
- [ ] [副工程师] 将 `FloatingText` + `VisualProjectile` 拆分至 `src/projectile/projectile.h`
- [ ] [机制设计者] 完成 `docs/GAME_DESIGN.md` v1.0（数值确认、波次配置）- due: M1结束
- [ ] [故事设计者] 完成 `docs/STORY_AND_UI_TEXT.md` 初稿（世界观简介 + 所有 UI 文字）- due: M1结束
- [ ] [组长] 在 GitHub 上完成仓库权限配置与分支保护规则（见 docs/GIT_WORKFLOW.md）

---

## 🟡 进行中 | In Progress

- [ ] [CIO] TDD_enemy.md（依赖主工程师确认敌人类型数量）
- [ ] [CIO] TDD_tower.md（依赖机制设计者确认炮塔种类）
- [ ] [主工程师] `src/core/game.c` 框架重构（将 GameManager 拆分职责）

---

## 🟢 已完成 | Done

- [x] [CIO] 项目目录结构初始化（src/ docs/ assets/ lib/）
- [x] [CIO] README.md 完整版发布
- [x] [CIO] CHANGELOG.md 初始化
- [x] [CIO] CODING_STANDARD.md 发布
- [x] [CIO] GIT_WORKFLOW.md 发布（含 GitHub 操作命令）
- [x] [CIO] docs/TECH_STACK.md（Raylib 决策已锁定）
- [x] [CIO] docs/TEAM_ROLES.md 发布
- [x] [CIO] TDD_map.md v1.0 草稿完成
- [x] [CIO] 所有模块骨架 .h 文件创建
- [x] [CIO] Makefile 创建
- [x] [CIO] .gitignore 更新
- [x] [CIO] GAME_DESIGN.md 模板创建
- [x] [CIO] STORY_AND_UI_TEXT.md 模板创建

---

## ⛔ 阻塞中 | Blocked

- [ ] [副工程师] `src/projectile/` 模块实现
  → **阻塞原因**：等待主工程师确认 `map` 模块路径接口最终形态（`Map_getWaypoints` 签名）
  → **负责解除阻塞**：主工程师，预计 M1 结束前

- [ ] [副工程师] `src/ui/` 模块实现
  → **阻塞原因**：等待故事设计者提交 UI 文字最终内容
  → **负责解除阻塞**：故事设计者，预计 M1 结束前

---

## 📬 待组长决策 | Pending Team Lead Decision

- [ ] 最终关卡数量：1关还是2关？影响 M3 工作量估算，请组长确认
- [ ] 炮塔种类数量：1种（石投石器）还是2种？影响机制设计者工作量与 TDD_tower.md 范围
- [ ] GitHub 仓库是否公开（Public）？影响后续演示与提交

---

## 📅 里程碑规划 | Milestone Plan

```
M1（第1周）：架构完成
  目标：所有 TDD 文档锁定，代码从原型迁移至新结构，Makefile 可编译

M2（第2周）：核心机制可运行
  目标：地图渲染、敌人沿路径移动、炮塔攻击、子弹系统均可运行

M3（第3周）：完整游戏循环
  目标：波次系统、UI、GAME OVER / 通关画面，UI 文字最终版集成

M4（第4周）：打磨与收尾
  目标：Bug 修复、视觉细节、README 完善、最终构建验收
```

---

*CIO-TD | Defence.Inc Project*
