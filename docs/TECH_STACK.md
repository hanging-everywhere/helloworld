# 技术选型记录 | Technology Stack Decisions

**维护者 / Maintainer**: CIO-TD
**最后更新 / Last Updated**: 2026-03-14

---

## 已锁定决策 | Locked Decisions

### 1. 图形库：Raylib | Graphics Library: Raylib

**决定时间 / Decision Date**: 项目启动前（2026-03-06）/ Before project kickoff
**状态 / Status**: ✅ 已锁定 / Locked

#### 中文理由

- 真正的纯 C 接口（C99），无任何 C++ 依赖
- 静态库内嵌项目（`lib/raylib/`），全员零额外配置
- 完整的窗口管理、输入、渲染、音效 API，适合小型游戏全流程
- 有完整塔防游戏开发教程与社区资源
- 跨平台，Windows 静态编译友好

#### English Rationale

- Truly pure C interface (C99), zero C++ dependencies
- Static library bundled in repository (`lib/raylib/`), zero setup for all team members
- Complete API coverage: window management, input, rendering, audio
- Active community with tower defense game tutorials
- Cross-platform; Windows static compilation is well-supported

---

### 2. 编程语言：C99 | Programming Language: C99

**状态 / Status**: ✅ 已锁定 / Locked

- 所有 `.c` / `.h` 文件，GCC 以 `-std=c99` 编译
  All `.c` / `.h` files, compiled by GCC with `-std=c99`
- **严格禁止** 任何 C++ 特性（class、namespace、STL、模板等）
  **Strictly forbidden**: any C++ features
- **禁止** 创建任何 `.cpp` 文件
  **Forbidden**: creating any `.cpp` files

---

### 3. 编译工具链 | Compiler Toolchain

**状态 / Status**: ✅ 已锁定 / Locked

- 编译器 / Compiler: GCC（MinGW-w64，Windows 64-bit）
- 构建工具 / Build tool: GNU Make + `Makefile`（项目根目录 / project root）
- 编译标志 / Flags: `-std=c99 -Wall -Wextra -g`
- 目标平台 / Target: Windows 64-bit EXE

---

### 4. 版本控制：Git + GitHub | Version Control: Git + GitHub

**状态 / Status**: ✅ 已锁定 / Locked

- 工作流详见 / Workflow details: [GIT_WORKFLOW.md](GIT_WORKFLOW.md)
- 分支模型：`main`（稳定）→ `dev`（集成）→ `feature/*`（功能）
  Branch model: `main` (stable) → `dev` (integration) → `feature/*` (feature)

---

### 5. 游戏视角：半俯视视角 | Game Perspective: Semi-Overhead (Isometric)

**决定时间 / Decision Date**: 2026-03-14（第2次会议后确认 / Confirmed after 2nd meeting）
**状态 / Status**: ✅ 已锁定 / Locked

#### 中文说明

- 采用半俯视（等轴测）视角，适合塔防游戏的全局战场展示
- 地图以网格形式渲染，格子大小 64×64 像素
- 视角固定，不支持旋转或缩放（当前阶段）

#### English Description

- Semi-overhead (isometric) perspective, suitable for tower defense battlefield overview
- Map rendered in grid form, cell size 64×64 pixels
- Fixed perspective, no rotation or zoom support (current phase)

---

### 6. 美术风格：手绘像素风格 | Art Style: Hand-Drawn Pixel Art

**决定时间 / Decision Date**: 2026-03-14
**状态 / Status**: ✅ 已锁定 / Locked

#### 中文说明

- 美术风格为**手绘像素风格**，所有游戏素材（地图贴图、怪物、炮塔、特效）均采用此风格
- 美术负责人：戴博远（Drawer），组长徐世昌协助地图与炮塔绘制
- 素材格式：PNG（透明背景）
- 参考工具：Unity（用于素材预览与参考，**非游戏引擎**——游戏引擎为 Raylib）

#### English Description

- Art style is **hand-drawn pixel art**; all game assets (map tiles, monsters, towers, VFX) follow this style
- Art lead: Dai Boyuan (Drawer); CEO Xu Shichang assists with map and tower art
- Asset format: PNG (transparent background)
- Reference tool: Unity (for asset preview and reference, **NOT the game engine** — game engine is Raylib)

#### 素材技术规格 | Asset Technical Specifications

| 属性 / Attribute | 规格 / Specification | 备注 / Notes |
|---|---|---|
| 窗口分辨率 / Window Resolution | 1024×768 像素 / pixels | 已锁定 |
| 单格尺寸 / Cell Size | 64×64 像素 / pixels | 已锁定 |
| 网格尺寸 / Grid Size | 12 行 × 16 列 / 12 rows × 16 cols | 地图覆盖全窗口 |
| 素材格式 / Asset Format | PNG（透明背景 / transparent background）| 必须 |
| 命名规范 / Naming | 英文小写 + 下划线 / lowercase + underscore | 如 `enemy_beast_walk_01.png` |
| 颜色深度 / Color Depth | 32-bit RGBA | 标准 |

---

### 7. AI 工具使用 | AI Tools Usage

**状态 / Status**: ✅ 已确认使用方式 / Usage confirmed

| 工具 / Tool | 用途 / Purpose | 使用方式 / Usage |
|---|---|---|
| GitHub Copilot | Raylib C 代码框架生成 | 生成后人工删除冗余 + 补充游戏逻辑 |
| Gemini | C-Raylib 兼容性方案、地图参数、玩法创意 | 筛选有效信息、精简方案后采纳 |

> 原则：AI 作为辅助工具，所有输出经人工审查与重构后方可使用；核心逻辑由团队成员主导。
> Principle: AI is auxiliary; all output must be reviewed and refactored before use; core logic is team-led.

---

## 已决策事项 | Resolved Decisions

| 事项 / Item | 决策结果 / Decision | 决策时间 / Date |
|---|---|---|
| 关卡数量 / Map Count | 目前已完成 2 张，后续持续扩展 / 2 completed, more planned | 2026-03-14 🟡 |
| 炮塔种类 / Tower Types | 13 种 + 1 基地塔 / 13 types + 1 base tower | 2026-03-14 ✅ |

## 待决策事项 | Pending Decisions

| 事项 / Item | 决策者 / Owner | 截止 / Deadline | 影响 / Impact |
|---|---|---|---|
| GitHub 仓库是否公开 | 组长（徐世昌）| M2 结束 | 影响团队协作方式 |
| 最终关卡总数 | 组长 + 机制设计者 | 持续更新 | 影响美术与工程工作量 |

---

*CIO-TD | Defence.Inc Project*
