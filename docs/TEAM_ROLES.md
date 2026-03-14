# 团队角色与分工说明 | Team Roles & Responsibilities

**维护者 / Maintainer**: CIO-TD
**版本 / Version**: 2.0
**最后更新 / Last Updated**: 2026-03-14
**数据来源 / Source**: Project log.docx（2026-03-06 至 2026-03-13 实际分工）

---

## 角色总览 | Role Overview

| 成员 / Member | 角色 / Role | 主要职责 / Primary Responsibility | 主要产出 / Primary Output |
|---|---|---|---|
| 徐世昌（Xu Shichang）| CEO / 组长 | 总体规划、协调、里程碑把控；兼美术（地图/炮塔绘制）| 决策记录、里程碑计划、部分美术素材 |
| 张嘉毓（Zhang Jiayu）| Programmer / 主工程师 | **全部**代码模块开发与调试 | `src/` 目录下所有 `.c/.h` 文件 |
| 戴博远（Dai Boyuan）| Drawer / 美术画师 | 地图绘制、怪物/炮塔/动画素材设计 | `assets/` 目录下所有图片资源 |
| 符雅翔（Fu Yaxiang）| CIO | 架构、文档、技术调研、规范、审查 | `docs/` 文档体系、TDD、代码 Review |
| 张一奥（Zhang Yi'ao）| Communicator / 沟通者 | 进度追踪、内部沟通、会议管理 | 进度追踪表、沟通记录、会议纪要 |
| 胡昊泽（Hu Haoze）| Designer / 机制设计者 | 游戏数值、规则设计、关卡平衡 | `docs/GAME_DESIGN.md` |

---

## 详细职责说明 | Detailed Role Descriptions

### 徐世昌 — CEO / 组长 | Xu Shichang — CEO / Team Lead

**中文职责**

- 负责项目整体进度把控，协调跨角色依赖问题
- 对重大架构变更（如模块新增/删除、技术栈调整）拥有最终决策权
- 负责在 GitHub 完成仓库初始化与权限配置（见 GIT_WORKFLOW.md）
- 每个里程碑节点接收 CIO 的进度摘要，并给出方向性决策
- **兼任部分美术工作**：地图绘制、炮塔素材绘制（与美术画师协作）
- 主持团队会议，把控会议决策执行

**English Responsibilities**

- Owns overall project timeline; resolves cross-role dependencies
- Final decision authority on major architectural changes
- Responsible for GitHub repository initialization and permission configuration
- Reviews CIO milestone summaries and provides directional decisions
- **Also contributes to art**: map drawing, tower asset creation (collaborating with drawer)
- Chairs team meetings; ensures decisions are executed

---

### 张嘉毓 — Programmer / 主工程师 | Zhang Jiayu — Programmer / Lead Engineer

**中文职责**

- 负责**全部**代码模块：`src/core/`、`src/map/`、`src/enemy/`、`src/tower/`、`src/projectile/`、`src/ui/`、`src/utils/`
- 确定所有模块接口的最终签名，通知 CIO 更新 TDD
- 负责将原型代码（`defense.Inc/`）迁移至新目录结构
- 解决 C + Raylib 在 Windows 平台的兼容性问题
- 向美术画师提供技术参数（分辨率、格子尺寸、素材规格）
- PR 须经 CIO review
- 已完成：原型代码开发与调试，基础游戏可运行

**English Responsibilities**

- Owns **ALL** code modules: `src/core/`, `src/map/`, `src/enemy/`, `src/tower/`, `src/projectile/`, `src/ui/`, `src/utils/`
- Finalizes all module interface signatures and notifies CIO to update TDD
- Responsible for migrating prototype code from `defense.Inc/` to the new structure
- Resolves C + Raylib Windows platform compatibility issues
- Provides technical parameters to drawer (resolution, cell size, asset specs)
- All PRs require CIO review
- Completed: prototype code development and debugging, basic game runnability achieved

---

### 戴博远 — Drawer / 美术画师 | Dai Boyuan — Drawer / Artist

**中文职责**

- 负责 `assets/` 目录下所有游戏美术资源：地图贴图、怪物精灵图、炮塔精灵图、动画帧、特效素材
- 美术风格：**手绘像素风格**（已确认）
- 游戏视角：**半俯视视角**（已确认）
- 素材须适配 Raylib 窗口参数（1024×768，格子 64×64 像素）
- 参考工具：Unity（用于美术参考与素材预览，非游戏引擎）
- 素材格式：PNG（透明背景），命名遵循英文小写 + 下划线规范
- 美术 PR 提交到 `assets/` 分支，Reviewer 指定 CIO 与组长
- 已完成：初版地图绘制，修订地图尺寸适配 Raylib 窗口

**English Responsibilities**

- Owns all game art assets under `assets/`: map tiles, monster sprites, tower sprites, animation frames, VFX assets
- Art style: **hand-drawn pixel art** (confirmed)
- Game perspective: **semi-overhead / isometric** (confirmed)
- Assets must match Raylib window parameters (1024×768, cell size 64×64 pixels)
- Reference tool: Unity (for art reference and asset preview, NOT the game engine)
- Asset format: PNG (transparent background), naming follows lowercase_underscore convention
- Art PRs submitted to `assets/` branch, reviewer: CIO and team lead
- Completed: initial map drawing, revised map size to match Raylib window

---

### 符雅翔 — CIO | Fu Yaxiang — CIO

**中文职责**

- 架构设计与文档体系的建立和维护
- Raylib 技术调研，为工程师提供技术文档支持
- 所有合并到 `dev` 的 PR 必须经过 CIO review（强制）
- 在编码开始前完成所有模块的 TDD 文档
- 维护 TODO.md 进度看板，每个里程碑节点输出进度摘要给组长
- 将机制设计者的数值翻译为代码常量（`#define`），写入骨架头文件
- 协助代码测试
- 为团队成员提供完整的 Git 操作命令，不只给原则

**English Responsibilities**

- Owns architectural design and the full documentation system
- Raylib technical research; provides technical documentation support for engineer
- All PRs merging to `dev` require CIO review (mandatory)
- Completes all module TDD documents before coding begins
- Maintains TODO.md task board; delivers milestone summaries to team lead
- Translates game designer values into code constants (`#define`)
- Assists with code testing
- Provides complete Git command sequences for team members

---

### 张一奥 — Communicator / 沟通者 | Zhang Yi'ao — Communicator

**中文职责**

- 负责进度追踪表的维护与更新
- 内部沟通协调：确保各角色间信息流通
- 会议管理：出席提醒、会议记录、决策事项追踪
- 协助维护 `docs/STORY_AND_UI_TEXT.md`（UI 文字收集与整理）
- 沟通 PR 提交到 `docs/` 分支，Reviewer 指定 CIO

**English Responsibilities**

- Maintains and updates progress tracking sheets
- Internal communication coordination: ensures information flows between roles
- Meeting management: attendance reminders, meeting notes, decision tracking
- Assists maintaining `docs/STORY_AND_UI_TEXT.md` (UI text collection and organization)
- Communication PRs submitted to `docs/` branch, reviewer: CIO

---

### 胡昊泽 — Designer / 机制设计者 | Hu Haoze — Designer / Game Designer

**中文职责**

- 负责维护 `docs/GAME_DESIGN.md`，包含所有数值（炮塔属性、敌人属性、波次配置）
- 提出游戏玩法方案，撰写主题分析报告
- M1 结束前提交 v1.0（数值确认），M3 结束前提交 v2.0（最终锁定）
- 数值变更须通知 CIO；CIO 负责将数值翻译为代码常量（`#define`）
- 提供设计反馈与优化建议
- 已完成：初版玩法提案、主题分析报告

**English Responsibilities**

- Owns `docs/GAME_DESIGN.md` — all game values (tower stats, enemy stats, wave configs)
- Proposes gameplay ideas; writes theme analysis reports
- Submit v1.0 by M1 end (confirmed values); submit v2.0 by M3 end (final lock)
- Notify CIO of any value changes; CIO translates values into code constants (`#define`)
- Provides design feedback and optimization suggestions
- Completed: initial gameplay proposal, theme analysis report

---

## CIO 与各角色的协作触发条件 | CIO Collaboration Triggers

| 触发事件 / Trigger | CIO 动作 / CIO Action |
|---|---|
| 机制设计者（胡昊泽）更新 GAME_DESIGN.md | 检查数值变更是否影响 TDD 常量，必要时同步更新并通知工程师 |
| 美术画师（戴博远）提交新素材 | 确认素材规格（尺寸、格式、命名）符合技术规范，通知工程师集成 |
| 沟通者（张一奥）更新 STORY_AND_UI_TEXT.md | 确认动态变量格式（%d 等），在 CHANGELOG 登记 |
| 主工程师（张嘉毓）提交 PR | 按 Review 清单逐项检查，给出 APPROVED / NEEDS WORK / REJECTED |
| 任何角色要求"直接写代码"但 TDD 未锁定 | 发出 CIO HOLD 通知，先完成 TDD |
| 里程碑节点到达 | 输出进度摘要给组长（徐世昌）|

---

## 贡献度评分（3.06—3.13）| Contribution Score (3.06—3.13)

> 来源：Project log.docx，评分标准为工作重要性、工作量、项目影响力。
> Source: Project log.docx. Scored by work importance, workload, and project impact.

| 成员 / Member | 角色 / Role | 得分 / Score | 核心依据 / Justification |
|---|---|---|---|
| 徐世昌 | CEO | 26% | 总体规划协调、会议主持、里程碑制定、美术协作 |
| 张嘉毓 | Programmer | 21% | 核心代码开发调试、解决 C-Raylib 兼容性、提供技术参数 |
| 戴博远 | Drawer | 17% | 初版/修订地图绘制、适配 Raylib 窗口、建立视觉基础 |
| 符雅翔 | CIO | 12% | Raylib 技术调研、项目文档、协助代码测试 |
| 张一奥 | Communicator | 8% | 进度追踪、内部沟通、会议管理 |
| 胡昊泽 | Designer | 4% | 初版玩法提案、主题分析、设计反馈 |

---

## 会议记录摘要 | Meeting Summary

### 第1次会议 / 1st Meeting — 2026-03-06 21:00（线上）

- **议题**：角色分工、主题确认、初步玩法讨论
- **决策**：确定各角色分工；投票确定游戏主题；会后开始代码/地图开发
- **问题**：主题存在小分歧 → 通过 C/Raylib 技术可行性分析 + 投票解决
- **下一步**：3.13（第2次会议）前完成初始代码与地图

### 第2次会议 / 2nd Meeting — 2026-03-13 14:30（线上）

- **议题**：进度评审、代码/地图效果检查、下周任务明确
- **决策**：初始进度达标；确认地图/元素设计技术标准；聚焦地图优化/元素绘制/代码嵌入
- **问题**：无；代码与地图达到初始预期
- **下一步**：3.20 前完成地图改进/元素绘制/代码嵌入；实现防御塔/怪物的基本显示

---

*CIO-TD | Defence.Inc Project v2.0*
