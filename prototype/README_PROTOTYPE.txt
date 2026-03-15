======================================================
  defense.Inc/ — 原型代码（只读参考）
  Prototype Code (Read-Only Reference)
======================================================

此目录保留原始原型代码，仅供参考对照。
所有开发工作请在 src/ 目录下进行。
This directory preserves the original prototype.
All development work should be done in src/.

资源文件已迁移 / Assets have been moved:
  explosion.png  -->  assets/images/explosion.png
  boom.wav       -->  assets/sounds/boom.wav
  app.ico        -->  assets/images/app.ico

IDE 工件 / IDE artifacts:
  ide/           -->  Red Panda IDE 项目文件（不参与编译）

------------------------------------------------------
原型 → 新代码 对照表 / Prototype → New Code Mapping
------------------------------------------------------

  原型文件                    新位置                      关键变更
  Prototype File              New Location                Key Changes
  ─────────────────────────── ─────────────────────────── ────────────────────────
  main.c                      src/main.c                  GameManager → Game
  Map.h / Map.c               src/map/map.h / map.c       Map_init → map_init
                                                           ROWS → MAP_ROWS
                                                           新增: map_can_place_tower
                                                           新增: map_place_tower
                                                           新增: map_get_waypoints
  Enemy.h / Enemy.c           src/enemy/enemy.h / enemy.c Enemy → EnemyState
                                                           double → float
                                                           新增: enemy_take_damage
                                                           新增: enemy_get_waypoint_index
  Tower.h (FloatingText,      src/projectile/             拆分: 飘字/抛射物独立模块
    VisualProjectile)          projectile.h / projectile.c Split: text/projectile separate
  Tower.h / Tower.c           src/tower/tower.h / tower.c Tower_init(t,x,y) → tower_init(t,center)
                                                           不再直接访问 enemy.hp
                                                           Uses enemy_take_damage() instead
  GameManager.h / .c          src/core/game.h / game.c    GameManager → Game
                                                           enum GameState → typedef enum GameState
                                                           UI 渲染拆分到 src/ui/
  GameManager.c (UI部分)      src/ui/ui.h / ui.c          纯渲染函数，不持有状态
                                                           Pure render functions, no state

  （新增模块 / New modules）
  —                           src/utils/utils.h / utils.c 距离计算、clamp、lerp
