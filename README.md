# 千年城影 · 旧石器时代防线
# Millennium Shadow · Paleolithic Defense Line

**版本 / Version**: 0.1.0
**语言 / Language**: C99（纯C / Pure C）
**图形库 / Graphics**: Raylib（静态库内嵌 / Statically Linked）
**平台 / Platform**: Windows（GCC / MinGW-w64）

---

## 项目简介 | Project Overview

一款以旧石器时代为背景的塔防游戏。玩家在黑夜中建造石质防御塔，保护部落大本营的篝火不被史前猛兽侵袭。共 3 波进攻，考验玩家的金钱管理与阵地布局。

A tower defense game set in the Paleolithic era. Players build stone towers in the dark of night to protect the tribe's bonfire from prehistoric beasts. Survive 3 waves with smart resource management and strategic placement.

---

## 技术栈 | Tech Stack

| 项目 / Item | 内容 / Detail |
|---|---|
| 语言 / Language | C99（GCC / MinGW-w64） |
| 图形库 / Graphics | Raylib 5.x（静态库，随仓库提交 / Static, bundled） |
| 构建 / Build | GNU Make + Makefile |
| 版本控制 / VCS | Git + GitHub |
| 目标平台 / Target | Windows 64-bit |

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
tower-defense/
├── src/                  # 源代码 / Source code
│   ├── main.c            # 程序入口 / Entry point
│   ├── core/             # 游戏状态机 / Game state machine
│   ├── enemy/            # 敌人系统 / Enemy system
│   ├── tower/            # 炮塔系统 / Tower system
│   ├── projectile/       # 子弹系统 / Projectile system
│   ├── map/              # 地图系统 / Map system
│   ├── ui/               # UI 渲染 / UI rendering
│   └── utils/            # 公共工具 / Utilities
├── assets/               # 游戏资源 / Game assets
├── lib/raylib/           # Raylib 静态库 / Raylib static library
├── docs/                 # 项目文档 / Documentation
├── Makefile
├── README.md
├── CHANGELOG.md
└── TODO.md
```

详见 [docs/](docs/) 目录下的技术文档。
See [docs/](docs/) for detailed technical documentation.

---

## 团队成员与分工 | Team Members & Roles

| 角色 / Role | 职责 / Responsibility |
|---|---|
| 组长 / Team Lead | 总体决策、里程碑把控 / Overall decisions, milestone control |
| 主工程师 / Lead Engineer | 核心模块实现、技术难点 / Core modules, technical challenges |
| 副工程师 / Junior Engineer | 辅助模块、配合主工程师 / Support modules, assisting lead |
| 机制设计者 / Game Designer | 数值、规则、关卡平衡 / Mechanics, balance, level design |
| 故事设计者 / Narrative Designer | 世界观、剧情、UI文字 / Lore, narrative, UI text |
| CIO | 架构、文档、规范、审查 / Architecture, docs, standards, review |

详见 [docs/TEAM_ROLES.md](docs/TEAM_ROLES.md)。
See [docs/TEAM_ROLES.md](docs/TEAM_ROLES.md) for details.

---

## 开发进度 | Development Progress

当前里程碑 / Current Milestone: **M1 — 架构完成 / Architecture Setup**

详见 [TODO.md](TODO.md)。
See [TODO.md](TODO.md) for the full task board.

---

*CIO-TD | Defence.Inc Project*
