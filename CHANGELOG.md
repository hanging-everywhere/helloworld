# Changelog

所有重要变更均记录在此文件。
All notable changes to this project are documented in this file.

格式遵循 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)。
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [Unreleased]

### Added / 新增
- 规范化目录结构（src/ docs/ assets/ lib/）
  Standardized directory structure (src/ docs/ assets/ lib/)
- 完整文档体系：README、CODING_STANDARD、GIT_WORKFLOW、TECH_STACK、TEAM_ROLES
  Full documentation suite: README, CODING_STANDARD, GIT_WORKFLOW, TECH_STACK, TEAM_ROLES
- 所有模块骨架头文件（map/ enemy/ tower/ projectile/ ui/ utils/ core/）
  Skeleton header files for all modules
- TDD_map.md v1.0（第一份技术设计文档）
  TDD_map.md v1.0 (first technical design document)
- TODO.md 里程碑任务看板
  TODO.md milestone task board
- GAME_DESIGN.md 数值文档模板（待机制设计者填写）
  GAME_DESIGN.md mechanics template (pending game designer input)
- STORY_AND_UI_TEXT.md 文案文档模板（待故事设计者填写）
  STORY_AND_UI_TEXT.md narrative template (pending narrative designer input)

---

## [0.1.0] - 2026-03-10

### Added / 新增
- 原始原型代码（Red Panda IDE 项目，defense.Inc/ 目录保留为参考）
  Original prototype code (Red Panda IDE project, kept in defense.Inc/ as reference)
- 基础游戏循环：地图渲染、敌人移动、炮塔攻击、3 波次系统
  Basic game loop: map rendering, enemy movement, tower attack, 3-wave system
- Raylib 图形库集成（窗口 1024×768，锁定 60FPS）
  Raylib graphics integration (1024×768 window, 60 FPS cap)
- 地图：12×16 格子，S 型路径，5 个路点
  Map: 12×16 grid, S-shaped path, 5 waypoints
- 游戏数值：基地血量 10，初始金钱 150，炮塔造价 50
  Game values: base HP 10, starting gold 150, tower cost 50

---

*维护者：CIO-TD*
*Maintained by: CIO-TD*
