# Changelog

所有重要变更均记录在此文件。
All notable changes to this project are documented in this file.

格式遵循 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)。
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [Unreleased]

### Added / 新增
- 美术素材目录规范化（等待美术画师提交首批素材）
  Art asset directory standardized (awaiting first batch from drawer)

---

## [0.3.0] - 2026-03-14

### Added / 新增
- GAME_DESIGN.md v1.0：机制设计者胡昊泽提交完整机制设计
  GAME_DESIGN.md v1.0: designer Hu Haoze submitted full mechanism design
  - 3 时代体系：石器时代·蒙昧、古代文明·理性、科幻未来·星辰
    3-era system: Stone Age, Ancient Civilization, Sci-Fi Future
  - 13 种炮塔 + 1 基地塔（跨时代三形态），每座含双升级路径
    13 tower types + 1 base tower (3 era forms), each with dual upgrade paths

- STORY_AND_UI_TEXT.md v0.3：三时代世界观已填入
  STORY_AND_UI_TEXT.md v0.3: 3-era world lore added

### Changed / 变更
- 关卡数量：目前已完成 2 张地图，后续持续扩展
  Map count: 2 maps completed so far, more planned
- 确认炮塔种类：13 种 + 1 基地塔（已解决待决策项）
  Confirmed tower types: 13 + 1 base tower (resolved pending decision)
- TODO.md：机制设计 v1.0 标记已完成，新增敌人/资源系统待办项
  TODO.md: mechanism design v1.0 marked complete, added enemy/resource pending items
- TECH_STACK.md：炮塔种类移至已决策
  TECH_STACK.md: tower types moved to resolved decisions

---

## [0.2.0] - 2026-03-14

### Added / 新增
- 确认游戏视角：半俯视视角（Semi-overhead / Isometric）
  Confirmed game perspective: semi-overhead (isometric)
- 确认美术风格：手绘像素风格（Hand-drawn Pixel Art）
  Confirmed art style: hand-drawn pixel art
- 确认美术参考工具：Unity（仅参考，非游戏引擎）
  Confirmed art reference tool: Unity (reference only, not game engine)
- TECH_STACK.md 新增视角、美术风格、AI 工具使用三项已锁定决策
  TECH_STACK.md: added perspective, art style, and AI tools as locked decisions
- 素材技术规格定义（64×64 格子、PNG 透明背景、命名规范）
  Asset technical specifications defined (64×64 cells, PNG transparent, naming convention)

### Changed / 变更
- TEAM_ROLES.md v2.0：团队角色以 Project log.docx 为准重写（实名 + 实际分工）
  TEAM_ROLES.md v2.0: team roles rewritten based on Project log.docx (real names + actual roles)
  - ~~副工程师~~ → 美术画师 戴博远 / ~~Junior Engineer~~ → Drawer Dai Boyuan
  - ~~故事设计者~~ → 沟通者 张一奥 / ~~Narrative Designer~~ → Communicator Zhang Yi'ao
  - 所有代码模块归主工程师张嘉毓 / All code modules assigned to programmer Zhang Jiayu
- README.md v0.2.0：更新团队实名信息、美术风格、AI 使用说明
  README.md v0.2.0: updated with real team info, art style, AI usage disclosure
- TODO.md：里程碑进入 M2，M1 已标记完成；任务按实际角色重新分配
  TODO.md: milestone advanced to M2, M1 marked complete; tasks reassigned to actual roles
- GIT_WORKFLOW.md v1.1：合并规则与权限配置更新为实际团队角色
  GIT_WORKFLOW.md v1.1: merge rules and permissions updated for actual team roles
- GAME_DESIGN.md：补充美术视角与风格信息
  GAME_DESIGN.md: added art perspective and style info

### Documented / 登记
- 第1次团队会议记录（2026-03-06）：角色分工、主题确认、玩法讨论
  1st team meeting notes (2026-03-06): role division, theme confirmation, gameplay discussion
- 第2次团队会议记录（2026-03-13）：进度评审、技术标准确认、下周任务
  2nd team meeting notes (2026-03-13): progress review, tech standards, next week tasks
- 团队贡献度评分（3.06—3.13）记录至 TEAM_ROLES.md
  Team contribution scores (3.06—3.13) recorded in TEAM_ROLES.md
- AI 工具使用记录：GitHub Copilot、Gemini（使用方式与修改说明）
  AI tool usage log: GitHub Copilot, Gemini (usage and modifications)

---

## [0.1.0] - 2026-03-10

### Added / 新增
- 规范化目录结构（src/ docs/ assets/ lib/）
  Standardized directory structure (src/ docs/ assets/ lib/)
- 完整文档体系：README、CODING_STANDARD、GIT_WORKFLOW、TECH_STACK、TEAM_ROLES
  Full documentation suite
- 所有模块骨架头文件（map/ enemy/ tower/ projectile/ ui/ utils/ core/）
  Skeleton header files for all modules
- TDD_map.md v1.0（第一份技术设计文档）
  TDD_map.md v1.0 (first TDD)
- TODO.md 里程碑任务看板
  TODO.md milestone task board
- GAME_DESIGN.md / STORY_AND_UI_TEXT.md 模板
  GAME_DESIGN.md / STORY_AND_UI_TEXT.md templates
- 原始原型代码（Red Panda IDE 项目，defense.Inc/ 目录保留为参考）
  Original prototype code (defense.Inc/ kept as reference)
- 基础游戏循环：地图渲染、敌人移动、炮塔攻击、3 波次系统
  Basic game loop: map rendering, enemy movement, tower attack, 3-wave system
- Raylib 图形库集成（窗口 1024×768，锁定 60FPS）
  Raylib graphics integration (1024×768 window, 60 FPS cap)

---

*维护者：CIO-TD（符雅翔）*
*Maintained by: CIO-TD (Fu Yaxiang)*
