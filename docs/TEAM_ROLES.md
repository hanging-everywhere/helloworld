# 团队角色与分工说明 | Team Roles & Responsibilities

**维护者 / Maintainer**: CIO-TD
**版本 / Version**: 1.0
**最后更新 / Last Updated**: 2026-03-10

---

## 角色总览 | Role Overview

| 角色 / Role | 主要职责 / Primary Responsibility | 主要产出 / Primary Output |
|---|---|---|
| 组长 / Team Lead | 总体决策、对外沟通、里程碑把控 | 决策记录、资源协调 |
| 主工程师 / Lead Engineer | 核心模块实现（map、enemy、core）| `src/map/`、`src/enemy/`、`src/core/` |
| 副工程师 / Junior Engineer | 辅助模块实现（tower、projectile、ui）| `src/tower/`、`src/projectile/`、`src/ui/` |
| 机制设计者 / Game Designer | 游戏数值、规则、关卡平衡 | `docs/GAME_DESIGN.md` |
| 故事设计者 / Narrative Designer | 世界观、剧情文案、UI 文字 | `docs/STORY_AND_UI_TEXT.md` |
| CIO | 架构、文档、规范、审查、进度 | `docs/`、规范、Review |

---

## 详细职责说明 | Detailed Role Descriptions

### 组长 | Team Lead

**中文职责**

- 负责项目整体进度把控，协调跨角色依赖问题
- 对重大架构变更（如模块新增/删除、技术栈调整）拥有最终决策权
- 负责在 GitHub 完成仓库初始化与权限配置（见 GIT_WORKFLOW.md 第 12 节）
- 每个里程碑节点接收 CIO 的进度摘要，并给出方向性决策

**English Responsibilities**

- Owns overall project timeline; resolves cross-role dependencies
- Final decision authority on major architectural changes (module additions/removals, tech stack changes)
- Responsible for GitHub repository initialization and permission configuration (see GIT_WORKFLOW.md §12)
- Reviews CIO milestone summaries and provides directional decisions

---

### 主工程师 | Lead Engineer

**中文职责**

- 负责核心模块：`src/map/`、`src/enemy/`、`src/core/`
- 确定路径接口（`map_get_waypoints()`）的最终签名，通知 CIO 更新 TDD
- 负责将原型代码（`defense.Inc/`）迁移至新目录结构
- PR 须经 CIO review；有责任对副工程师的 PR 进行技术 review
- 遇到技术难点主动通知 CIO，防止阻塞扩散

**English Responsibilities**

- Owns core modules: `src/map/`, `src/enemy/`, `src/core/`
- Finalizes the path interface (`map_get_waypoints()`) signature and notifies CIO to update TDD
- Responsible for migrating prototype code from `defense.Inc/` to the new structure
- All PRs must be reviewed by CIO; also responsible for technical review of junior engineer's PRs
- Proactively reports blockers to CIO to prevent escalation

---

### 副工程师 | Junior Engineer

**中文职责**

- 负责辅助模块：`src/tower/`、`src/projectile/`、`src/ui/`
- `projectile` 模块依赖 `map` 路径接口，需等待主工程师确认后再实现
- 负责将 `FloatingText`、`VisualProjectile` 从 `Tower.h` 中拆分至 `projectile.h`
- PR 须同时指定 CIO 和主工程师为 Reviewer
- 遇到接口不清晰时，先找 CIO 确认 TDD，再实现，不要自行猜测

**English Responsibilities**

- Owns support modules: `src/tower/`, `src/projectile/`, `src/ui/`
- `projectile` module depends on `map` path interface — wait for lead engineer's confirmation
- Responsible for splitting `FloatingText` and `VisualProjectile` from `Tower.h` to `projectile.h`
- All PRs must assign both CIO and lead engineer as reviewers
- When interface is unclear, consult TDD with CIO before implementing — do not guess

---

### 机制设计者 | Game Designer

**中文职责**

- 负责维护 `docs/GAME_DESIGN.md`，包含所有数值（炮塔属性、敌人属性、波次配置）
- M1 结束前提交 v1.0（数值确认），M3 结束前提交 v2.0（最终锁定）
- 数值变更须通知 CIO；CIO 负责将数值翻译为代码常量（`#define`）
- 无需学习 Git 复杂操作：直接编辑文档，提交 PR 到 `docs/` 目录即可

**English Responsibilities**

- Owns `docs/GAME_DESIGN.md` — all game values (tower stats, enemy stats, wave configs)
- Submit v1.0 by M1 end (confirmed values); submit v2.0 by M3 end (final lock)
- Notify CIO of any value changes; CIO translates values into code constants (`#define`)
- No need for complex Git operations — edit docs and PR to `docs/` branch

---

### 故事设计者 | Narrative Designer

**中文职责**

- 负责维护 `docs/STORY_AND_UI_TEXT.md`，包含世界观简介和所有 UI 文字
- M1 结束前提交初稿，M3 结束前提交最终版，交付工程师集成
- 涉及动态变量的文字（如「第 %d 波」）须与 CIO 确认格式，避免显示错误
- 文案变更无需代码 review，但须通知 CIO 在 CHANGELOG 中登记
- 无需学习 Git 复杂操作：直接编辑文档，提交 PR 到 `docs/` 目录即可

**English Responsibilities**

- Owns `docs/STORY_AND_UI_TEXT.md` — world lore and all UI text strings
- Submit initial draft by M1 end; submit final version by M3 end for engineering integration
- For text with dynamic variables (e.g., "Wave %d"), confirm format with CIO to avoid display errors
- Narrative changes do not require code review, but CIO must log changes in CHANGELOG
- No need for complex Git operations — edit docs and PR to `docs/` branch

---

### CIO | Chief Information Officer

**中文职责**

- 架构设计与文档体系的建立和维护
- 所有合并到 `dev` 的 PR 必须经过 CIO review（强制）
- 在编码开始前完成所有模块的 TDD 文档
- 维护 TODO.md 进度看板，每个里程碑节点输出进度摘要给组长
- 将机制设计者的数值翻译为代码常量，写入骨架头文件
- 确保故事设计者的 UI 文字格式与工程师实现一致
- 为团队成员提供完整的 Git 操作命令，不只给原则

**English Responsibilities**

- Owns architectural design and the full documentation system
- All PRs merging to `dev` require CIO review (mandatory)
- Completes all module TDD documents before coding begins
- Maintains TODO.md task board; delivers milestone summaries to team lead
- Translates game designer values into code constants in skeleton header files
- Ensures narrative designer's UI text format aligns with engineering implementation
- Provides complete Git command sequences for team members — not just principles

---

## CIO 与各角色的协作触发条件 | CIO Collaboration Triggers

| 触发事件 / Trigger | CIO 动作 / CIO Action |
|---|---|
| 机制设计者更新 GAME_DESIGN.md | 检查数值变更是否影响 TDD 常量，必要时同步更新并通知工程师 |
| 故事设计者更新 STORY_AND_UI_TEXT.md | 确认动态变量格式，在 CHANGELOG 登记 |
| 工程师提交 PR | 按 Review 清单（第七节）逐项检查，给出 APPROVED / NEEDS WORK / REJECTED |
| 任何角色要求"直接写代码"但 TDD 未锁定 | 发出 CIO HOLD 通知，先完成 TDD |
| 里程碑节点到达 | 输出进度摘要给组长 |

---

*CIO-TD | Defence.Inc Project*
