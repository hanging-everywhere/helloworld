# 技术选型记录 | Technology Stack Decisions

**维护者 / Maintainer**: CIO-TD
**状态 / Status**: 已锁定 / LOCKED
**最后更新 / Last Updated**: 2026-03-10

---

## 已锁定决策 | Locked Decisions

### 1. 图形库：Raylib | Graphics Library: Raylib

**决定时间 / Decision Date**: 项目启动前 / Before project kickoff
**状态 / Status**: ✅ 已锁定，不再讨论 / Locked, not open for discussion

#### 中文理由

- 真正的纯 C 接口（C99），无任何 C++ 依赖
- 静态库内嵌项目（`lib/raylib/`），全员零额外配置
- 完整的窗口管理、输入、渲染、音效 API，适合小型游戏全流程
- 有完整塔防游戏开发教程与社区资源
- 跨平台，Windows 静态编译友好

#### English Rationale

- Truly pure C interface (C99), zero C++ dependencies
- Static library bundled in repository (`lib/raylib/`), zero setup for all team members
- Complete API coverage: window management, input, rendering, audio — suitable for full game development
- Active community with tower defense game tutorials
- Cross-platform; Windows static compilation is well-supported

---

### 2. 编程语言：C99 | Programming Language: C99

**状态 / Status**: ✅ 已锁定 / Locked

- 所有 `.c` / `.h` 文件，GCC 以 `-std=c99` 编译
  All `.c` / `.h` files, compiled by GCC with `-std=c99`
- **严格禁止** 任何 C++ 特性（class、namespace、STL、模板等）
  **Strictly forbidden**: any C++ features (class, namespace, STL, templates, etc.)
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
  Branch model: `main` (stable) → `dev` (integration) → `feature/*` (feature branches)

---

## 待决策事项 | Pending Decisions

| 事项 / Item | 决策者 / Owner | 截止 / Deadline | 影响 / Impact |
|---|---|---|---|
| 关卡数量（1关 or 2关）| 组长 | M1 结束 | 影响 M3 工作量 |
| 炮塔种类（1种 or 2种）| 组长 + 机制设计者 | M1 结束 | 影响 TDD_tower.md 范围 |

---

*CIO-TD | Defence.Inc Project*
