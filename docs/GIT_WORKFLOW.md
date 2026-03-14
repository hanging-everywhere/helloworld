# Git 工作流规范 | Git Workflow

**维护者 / Maintainer**: CIO-TD
**版本 / Version**: 1.1
**最后更新 / Last Updated**: 2026-03-14

> 本文档面向所有团队成员，包括 Git 基础薄弱的非工程角色。所有操作均提供完整命令行指令。
> This document targets all team members, including non-engineers with limited Git experience. All operations include complete command-line instructions.

---

## 1. 分支模型 | Branch Model

```
main          ← 永远可编译、可运行的稳定版本，受保护，禁止直接推送
               Always-compilable stable version. Protected. Direct push forbidden.
  └── dev     ← 集成分支，各功能分支合并到此处
               Integration branch. All feature branches merge here.
        ├── feature/enemy-pathfinding    ← 功能分支 / Feature branch
        ├── feature/tower-attack
        ├── art/map-tileset              ← 美术资源分支 / Art asset branch
        ├── fix/bullet-collision-offset  ← Bug 修复 / Bug fix
        └── docs/update-tdd-enemy        ← 文档更新 / Docs update
```

**规则 / Rules**:
- `main` 和 `dev` 受保护，**禁止直接 push**
  `main` and `dev` are protected — **direct push is forbidden**
- 所有改动通过 Pull Request 合并，且必须经过 CIO review
  All changes merged via Pull Request, CIO review is mandatory

---

## 2. 分支命名规范 | Branch Naming

| 类型 / Type | 格式 / Format | 示例 / Example |
|---|---|---|
| 新功能 / New feature | `feature/[描述]` | `feature/enemy-pathfinding` |
| 美术资源 / Art assets | `art/[描述]` | `art/map-tileset` |
| Bug 修复 / Bug fix | `fix/[描述]` | `fix/tower-range-calc` |
| 紧急修复 / Hotfix | `hotfix/[描述]` | `hotfix/game-crash-wave2` |
| 文档更新 / Docs update | `docs/[描述]` | `docs/update-tdd-map` |

**命名规则 / Naming rules**:
- 全小写，单词用连字符 `-` 分隔
  All lowercase, words separated by hyphens
- 描述使用英文，简短清晰（3-5 个单词）
  Description in English, brief and clear (3-5 words)

---

## 3. Commit Message 规范 | Commit Message Format

```
<类型>: <简短描述>（不超过50字）

[可选正文：说明做了什么、为什么这样做]
```

**类型前缀 / Type Prefixes**:

| 前缀 / Prefix | 含义 / Meaning |
|---|---|
| `feat` | 新功能 / New feature |
| `fix` | Bug 修复 / Bug fix |
| `docs` | 仅文档修改 / Documentation only |
| `art` | 美术资源变更 / Art asset changes |
| `refactor` | 重构，不改变行为 / Refactor, no behavior change |
| `style` | 格式调整，不影响逻辑 / Formatting only |
| `chore` | 构建/工具链相关 / Build/toolchain related |

**示例 / Examples**:

```bash
feat: 完成敌人沿路径移动逻辑

使用线段插值实现敌人在路径节点间的平滑移动。
速度由 EnemyConfig.speed 控制，单位为像素/秒。

---

art: 添加地图草地和路径的像素贴图

---

docs: 更新 TDD_map.md v1.1，补充路径接口说明
```

---

## 4. 每日标准操作流程 | Daily Standard Workflow

以下是每个成员每天工作的完整命令序列：
The complete command sequence for each member's daily work:

```bash
# ① 开始工作前：切换到 dev 并同步最新代码
#    Before starting: switch to dev and sync latest
git checkout dev
git pull origin dev

# ② 新建自己的分支（每个功能/美术/文档更新一个分支）
#    Create your branch (one branch per feature/art/doc update)
git checkout -b feature/your-feature-name
# 美术示例 / Art example:
git checkout -b art/monster-sprites
# 文档示例 / Docs example:
git checkout -b docs/update-game-design-v1

# ③ 编写代码或制作素材...完成一个小阶段后存档
#    Write code or create assets... commit after each small stage
git add src/enemy/enemy.c src/enemy/enemy.h
# 或者 / or:
git add assets/images/enemy_beast_walk_01.png
git commit -m "feat: 完成 enemy_init 和 enemy_update 函数"

# ④ 推送分支到远程（首次推送）
#    Push branch to remote (first push)
git push -u origin feature/your-feature-name

# ⑤ 后续继续提交后推送（无需 -u）
#    Subsequent pushes after commits
git push

# ⑥ 在 GitHub 上发起 Pull Request
#    Open Pull Request on GitHub
# → 目标分支 / Target branch: dev
# → 标题格式 / Title format: [角色] 简短描述
# → Reviewer 必须指定 CIO（符雅翔）
#   Reviewer must include CIO (Fu Yaxiang)
# → 代码类 PR 无需额外 reviewer（仅一名程序员）
#   Code PRs: no additional reviewer needed (single programmer)
```

---

## 5. 处理冲突（合并冲突）| Handling Merge Conflicts

若 PR 提示有冲突，按以下步骤解决：
If a PR shows conflicts, follow these steps:

```bash
# ① 切换到你的分支
git checkout feature/your-feature-name

# ② 拉取最新的 dev
git fetch origin dev

# ③ 将 dev 的最新内容合并进来
git merge origin/dev

# ④ 打开冲突文件，手动解决冲突
#    （冲突标记为 <<<<<<< HEAD ... ======= ... >>>>>>> origin/dev）

# ⑤ 标记为已解决
git add <解决后的文件>
git commit -m "fix: 解决与 dev 的合并冲突"

# ⑥ 推送更新
git push
```

> 遇到复杂冲突时，**先联系 CIO（符雅翔）**，不要自己强行解决。
> For complex conflicts, **contact CIO (Fu Yaxiang) first** — don't force-resolve alone.

---

## 6. 合并规则（CIO 强制执行）| Merge Rules (CIO Enforced)

```
✅ 所有合并到 dev 的 PR 必须经过 CIO（符雅翔）review
   All PRs merging to dev require CIO (Fu Yaxiang) review

✅ 代码类 PR（张嘉毓）CIO 逐项 review
   Code PRs (Zhang Jiayu): CIO performs item-by-item review

✅ 美术类 PR（戴博远）CIO 确认素材规格（尺寸、格式、命名），组长确认视觉效果
   Art PRs (Dai Boyuan): CIO confirms asset specs, CEO confirms visual quality

✅ 文档类 PR（张一奥/胡昊泽）CIO 登记确认
   Docs PRs (Zhang Yi'ao / Hu Haoze): CIO acknowledgment

✅ 数值类 PR（胡昊泽）CIO 须检查是否影响 TDD 常量
   Mechanics PRs (Hu Haoze): CIO checks TDD constant impact

✅ PR 必须附上简短的功能说明（2-3 句话）
   PR must include a brief description (2-3 sentences)

✅ PR 合并前代码必须能正常编译
   Code must compile before PR can be merged

✅ 重大架构变更须额外获得组长（徐世昌）确认
   Major architectural changes require CEO (Xu Shichang) approval

❌ 禁止直接 push 到 main 或 dev
   Forbidden: direct push to main or dev

❌ 禁止在未经 review 的情况下自行合并
   Forbidden: self-merging without review
```

---

## 7. PR 标题格式 | PR Title Format

```
[角色] 动作描述
```

示例 / Examples:
```
[Programmer] 完成 enemy 模块：init、update、draw 函数
[Drawer] 添加地图草地/路径像素贴图（64×64）
[Designer] 提交 GAME_DESIGN.md v1.0（数值初稿）
[Communicator] 更新进度追踪表（3.14—3.20）
[CIO] 发布 TDD_enemy.md v1.0
[CEO] 更新里程碑计划
```

---

## 8. GitHub 仓库初始化指引（供组长执行）| GitHub Setup Guide (CEO)

### 8.1 分支保护规则 | Branch Protection Rules

GitHub → Settings → Branches → Add rule，对 `main` 和 `dev` 分别配置：

**main 分支 / main branch**:
```
✅ Require a pull request before merging
✅ Require approvals: 2
✅ Dismiss stale pull request approvals when new commits are pushed
❌ Allow force pushes（关闭）
❌ Allow deletions（关闭）
```

**dev 分支 / dev branch**:
```
✅ Require a pull request before merging
✅ Require approvals: 1
✅ Dismiss stale pull request approvals when new commits are pushed
❌ Allow force pushes（关闭）
❌ Allow deletions（关闭）
```

### 8.2 团队权限配置 | Team Permission Setup

| 成员 / Member | 角色 / Role | GitHub 权限 / Permission | 说明 / Note |
|---|---|---|---|
| 徐世昌 | CEO / 组长 | Admin | 可管理仓库设置 |
| 符雅翔 | CIO | Maintain | 可合并 PR、管理分支 |
| 张嘉毓 | Programmer | Write | 可推送分支、发起 PR |
| 戴博远 | Drawer | Write | 建议操作 assets/ 与 docs/ |
| 张一奥 | Communicator | Write | 建议仅操作 docs/ 目录 |
| 胡昊泽 | Designer | Write | 建议仅操作 docs/ 目录 |

### 8.3 推荐开启的功能 | Recommended Features

```
✅ Issues（追踪 Bug 和任务）
✅ Projects（可视化看板）
❌ GitHub Actions（暂不开启，对此阶段过重）
```

---

## 9. 仓库初始化命令（供组长在本地执行）| Repo Init Commands (CEO, Local)

```bash
# 在项目根目录执行

# 初始化 Git 仓库
git init

# 添加远程仓库（将 URL 替换为实际的 GitHub 仓库地址）
git remote add origin https://github.com/[组织或用户名]/[仓库名].git

# 首次提交所有文档和骨架文件
git add .
git commit -m "chore: 初始化项目结构，添加文档与骨架文件"

# 推送到 main 分支
git push -u origin main

# 创建 dev 分支并推送
git checkout -b dev
git push -u origin dev

# 所有成员从此开始在 dev 分支的 feature/* / art/* / docs/* 上工作
```

---

*CIO-TD | Defence.Inc Project*
