# 千年城影 · 旧石器时代防线
# Millennium Shadow · Paleolithic Defense Line

**版本 / Version**: 0.2.0
**语言 / Language**: C99（纯C / Pure C）
**图形库 / Graphics**: Raylib（静态库内嵌 / Statically Linked）
**平台 / Platform**: Windows（GCC / MinGW-w64）
**视角 / Perspective**: 半俯视视角 / Semi-overhead (Isometric)
**美术风格 / Art Style**: 手绘像素风格 / Hand-drawn Pixel Art

---

## 项目简介 | Project Overview

一款横跨三个时代的塔防游戏——从石器时代的蒙昧，到古代文明的理性，再到科幻未来的星辰。玩家在多张精心设计的地图上，建造 13 种独特炮塔（每种含双升级路径），守护基地的最后火种。采用半俯视视角，手绘像素美术风格。

A tower defense game spanning three eras — from the Stone Age's Ignorance, through Ancient Civilization's Rationality, to the Sci-Fi Future's Stars. Players deploy 13 unique tower types (each with dual upgrade paths) across multiple handcrafted maps to protect their base. Features a semi-overhead perspective with hand-drawn pixel art style.

---

## 技术栈 | Tech Stack

| 项目 / Item | 内容 / Detail |
|---|---|
| 语言 / Language | C99（GCC / MinGW-w64） |
| 图形库 / Graphics | Raylib 5.x（静态库，随仓库提交 / Static, bundled） |
| 构建 / Build | GNU Make + Makefile |
| 版本控制 / VCS | Git + GitHub |
| 目标平台 / Target | Windows 64-bit |
| 美术参考工具 / Art Reference | Unity（仅用于素材参考，非游戏引擎 / Reference only, not the game engine） |

---

## 如何编译 | How to Build

**前提条件 / Prerequisites**：已安装 GCC（MinGW-w64）并加入 PATH。
**Prerequisite**: GCC (MinGW-w64) installed and added to PATH.

```bash
# 在项目根目录执行 / Run from project root
make

# 清理编译产物 / Clean build artifacts
make clean
```

编译成功后，可执行文件为 `tower_defense.exe`。
After successful build, the executable is `tower_defense.exe`.

---

## 如何运行 | How to Run

```bash
./tower_defense.exe
```

**操作说明 / Controls**：

- 鼠标左键点击草地格子建造石塔（花费 50 金币）
  Left-click on a grass tile to build a stone tower (costs 50 gold)
- 等待波次自动开始，守住基地篝火
  Waves start automatically — protect the bonfire!

---

## 项目结构 | Project Structure

```
Defence.Inc/
├── src/                  # 源代码 / Source code
│   ├── main.c            # 程序入口 / Entry point
│   ├── core/             # 游戏状态机 / Game state machine
│   ├── enemy/            # 敌人系统 / Enemy system
│   ├── tower/            # 炮塔系统 / Tower system
│   ├── projectile/       # 子弹系统 / Projectile system
│   ├── map/              # 地图系统 / Map system
│   ├── ui/               # UI 渲染 / UI rendering
│   └── utils/            # 公共工具 / Utilities
├── assets/               # 游戏资源（手绘像素素材）/ Game assets (hand-drawn pixel art)
│   ├── images/           # 图片资源 / Image assets
│   └── sounds/           # 音效资源 / Sound assets
├── lib/raylib/           # Raylib 静态库 / Raylib static library
├── docs/                 # 项目文档 / Documentation
├── defense.Inc/          # 原型代码（保留参考）/ Prototype code (kept as reference)
├── Makefile
├── README.md
├── CHANGELOG.md
└── TODO.md
```

详见 [docs/](docs/) 目录下的技术文档。
See [docs/](docs/) for detailed technical documentation.

---

## 团队成员与分工 | Team Members & Roles

| 成员 / Member | 角色 / Role | 职责 / Responsibility |
|---|---|---|
| 徐世昌（Xu Shichang）| CEO / 组长 | 总体规划、协调、里程碑把控；兼美术（地图/炮塔绘制）|
| 张嘉毓（Zhang Jiayu）| Programmer / 主工程师 | 全部代码模块开发与调试 |
| 戴博远（Dai Boyuan）| Drawer / 美术画师 | 地图、怪物、炮塔、动画素材设计（手绘像素风格）|
| 符雅翔（Fu Yaxiang）| CIO | 架构、文档、技术调研、规范、审查 |
| 张一奥（Zhang Yi'ao）| Communicator / 沟通者 | 进度追踪、内部沟通、会议管理 |
| 胡昊泽（Hu Haoze）| Designer / 机制设计者 | 游戏数值、规则设计、玩法提案 |

详见 [docs/TEAM_ROLES.md](docs/TEAM_ROLES.md)。
See [docs/TEAM_ROLES.md](docs/TEAM_ROLES.md) for details.

---

## 开发进度 | Development Progress

当前里程碑 / Current Milestone: **M2 — 核心机制可运行 / Core Mechanics Runnable**

- ✅ M1 架构完成（2026-03-06 至 2026-03-13）
- 🟡 M2 核心机制可运行（2026-03-14 至 2026-03-20）

详见 [TODO.md](TODO.md)。
See [TODO.md](TODO.md) for the full task board.

---

## AI 使用说明 | AI Usage Disclosure

本项目使用了以下 AI 工具作为辅助，核心逻辑由团队成员主导完成：
The following AI tools were used as assistants; core logic was led by team members:

- **GitHub Copilot**：代码片段/框架生成（经人工重构后采用）
- **Gemini**：C-Raylib 兼容性方案、地图参数、玩法创意（经筛选与精简后采用）

所有 AI 生成内容均经过人工审查、修改后方采纳。
All AI-generated content was reviewed and modified before adoption.

---

*CIO-TD | Defence.Inc Project*
