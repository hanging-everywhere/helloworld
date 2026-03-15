# 编码规范 | Coding Standard

**维护者 / Maintainer**: CIO-TD
**版本 / Version**: 1.0
**状态 / Status**: ✅ 已发布，全员强制执行 / Published, mandatory for all members
**最后更新 / Last Updated**: 2026-03-14

> 本文档为全员必读文档。违反任何一条规范，PR 将被标记为 NEEDS WORK 并要求修正。
> This document is mandatory reading for all members. Any violation will result in a NEEDS WORK review and required correction.

---

## 1. 命名规范 | Naming Conventions

### 1.1 常量和宏 | Constants and Macros

全大写 + 下划线分隔。
UPPER_CASE with underscores.

```c
#define MAX_ENEMY_COUNT   64
#define TOWER_RANGE       150
#define CELL_SIZE         64
#define BASE_INITIAL_HP   10
```

### 1.2 类型名 | Type Names

大驼峰（struct、enum、typedef 均适用）。
UpperCamelCase (applies to struct, enum, typedef).

```c
typedef struct EnemyState   EnemyState;
typedef struct TowerConfig  TowerConfig;
typedef enum   GameState    GameState;
```

### 1.3 函数名 | Function Names

格式：`模块名_动词_名词`，小写下划线分隔。
Format: `module_verb_noun`, lowercase with underscores.

```c
void  enemy_init(EnemyState* e, Point start_pos);
void  enemy_update(EnemyState* e, float dt);
void  enemy_draw(const EnemyState* e);
int   tower_can_shoot(const Tower* t);
void  map_render(const Map* m);
Point map_get_center(int row, int col);
```

### 1.4 变量名 | Variable Names

小写下划线分隔。
Lowercase with underscores.

```c
int   current_wave = 0;
float delta_time   = 0.0f;
int   enemy_count  = 0;
```

### 1.5 全局变量 | Global Variables

使用 `g_` 前缀（尽量避免使用全局变量）。
Use `g_` prefix (avoid global variables where possible).

```c
int g_player_gold = 100;
```

---

## 2. 文件规范 | File Standards

### 2.1 文件头注释 | File Header Comment

每个 `.c` 和 `.h` 文件的首行必须包含以下注释：
Every `.c` and `.h` file must begin with the following comment:

```c
/**
 * @file    enemy.c
 * @brief   敌人系统：移动、状态更新、死亡处理
 *          Enemy system: movement, state updates, death cleanup.
 * @author  [成员姓名 / Member Name]
 * @date    YYYY-MM-DD
 * @version 1.0
 */
```

### 2.2 头文件守卫 | Header Guard

所有 `.h` 文件必须有 `#ifndef` 防重复包含守卫：
All `.h` files must have `#ifndef` include guards:

```c
#ifndef ENEMY_H
#define ENEMY_H

/* ... 内容 / content ... */

#endif /* ENEMY_H */
```

### 2.3 文件大小限制 | File Size Limit

- **单个 `.c` 文件不得超过 400 行**
  A single `.c` file must not exceed 400 lines
- 若超出，须拆分为子模块并通知 CIO
  If exceeded, split into sub-modules and notify CIO

---

## 3. 函数规范 | Function Standards

### 3.1 函数注释 | Function Comment

每个函数（包括 `.h` 中声明的）必须有：
Every function (including declarations in `.h`) must have:

```c
/**
 * @brief  更新单个敌人的位置和状态
 *         Update the position and state of a single enemy entity.
 * @param  e   指向敌人状态的指针，不得为 NULL
 *             Pointer to enemy state, must not be NULL.
 * @param  dt  本帧时间差（秒）/ Frame delta time in seconds.
 * @return 若敌人已到达终点返回 1，否则返回 0
 *         Returns 1 if enemy reached the base, 0 otherwise.
 */
int enemy_update(EnemyState* e, float dt);
```

### 3.2 函数长度限制 | Function Length Limit

- **单个函数不得超过 60 行**
  A single function must not exceed 60 lines
- 若超出，须提取子函数。提取前通知 CIO，确认命名符合规范
  If exceeded, extract sub-functions. Notify CIO before extracting to confirm naming

### 3.3 函数职责单一 | Single Responsibility

一个函数只做一件事。判断标准：函数名中是否需要用"和/and"连接两个动作？若是，则须拆分。
A function does exactly one thing. Test: does the function name require "and" to connect two actions? If yes, split it.

```c
/* ❌ 错误示例 / Bad example */
void tower_update_and_render(Tower* t, ...);  /* 违反单一职责 */

/* ✅ 正确示例 / Good example */
void tower_update(Tower* t, float dt, ...);
void tower_draw(const Tower* t);
```

---

## 4. C 语言安全规范 | C Safety Rules

### 4.1 内存分配 | Memory Allocation

```c
/* ✅ 正确：检查 NULL */
EnemyState* e = malloc(sizeof(EnemyState));
if (e == NULL) {
    /* 错误处理 / error handling */
    return NULL;
}

/* ❌ 错误：不检查返回值 */
EnemyState* e = malloc(sizeof(EnemyState));
e->hp = 100;  /* 可能崩溃 / potential crash */
```

### 4.2 指针使用 | Pointer Usage

```c
/* ✅ 正确：使用前检查 NULL */
void enemy_update(EnemyState* e, float dt) {
    if (e == NULL) return;
    /* ... */
}
```

### 4.3 数组访问 | Array Access

```c
/* ✅ 正确：越界检查 */
if (index >= 0 && index < MAX_ENEMY_COUNT) {
    enemies[index].hp -= damage;
}
```

---

## 5. 模块边界规范 | Module Boundary Rules

### 5.1 禁止跨模块直接访问内部字段 | No Cross-Module Field Access

```c
/* ❌ 错误：Tower.c 直接访问 Enemy 内部字段 */
enemies[i].targetWaypointIndex;  /* 违规 */

/* ✅ 正确：通过 enemy 模块提供的函数访问 */
int wp = enemy_get_waypoint_index(&enemies[i]);
```

**注意**：当前原型代码中存在此违规，迁移时须修正。
**Note**: Current prototype code has this violation — must be fixed during migration.

### 5.2 禁止在 .h 中定义变量 | No Variable Definitions in .h Files

```c
/* ❌ 错误：在 .h 中定义变量 */
int g_enemy_count = 0;  /* .h 文件中 */

/* ✅ 正确：.h 中声明，.c 中定义 */
/* enemy.h */  extern int g_enemy_count;
/* enemy.c */  int g_enemy_count = 0;
```

---

## 6. 禁止事项完整清单 | Complete Prohibition List

```
❌ 禁止使用全局变量共享模块内部状态（用函数接口通信）
   Forbidden: using global variables to share module-internal state (use function interfaces)

❌ 禁止 malloc 后不检查返回值
   Forbidden: calling malloc without checking return value for NULL

❌ 禁止跨模块直接访问 struct 内部字段（用 getter/setter）
   Forbidden: cross-module direct struct field access (use getter/setter functions)

❌ 禁止在 .h 文件中定义变量（只声明）
   Forbidden: variable definitions in .h files (declarations only)

❌ 禁止 magic number（数字必须用 #define 或 enum 命名）
   Forbidden: magic numbers (all numeric literals must be named via #define or enum)

❌ 禁止单个函数超过 60 行
   Forbidden: functions exceeding 60 lines

❌ 禁止单个 .c 文件超过 400 行
   Forbidden: .c files exceeding 400 lines

❌ 禁止创建 .cpp 文件
   Forbidden: creating .cpp files

❌ 禁止使用任何 C++ 特性（class、namespace、STL、模板）
   Forbidden: any C++ features (class, namespace, STL, templates)

❌ 禁止缺少文件头注释
   Forbidden: missing file header comments

❌ 禁止缺少函数注释
   Forbidden: missing function comments
```

---

## 7. 代码双语注释格式 | Bilingual Comment Format

所有代码注释须中英双语，中文在上，英文在下。
All code comments must be bilingual (Chinese first, English second).

```c
/* 初始化敌人在起始位置
   Initialize enemy at the starting position */
void enemy_init(EnemyState* e, Point start_pos) {
    /* 将路点索引设为 1，因为 waypoints[0] 是出生点
       Set waypoint index to 1, as waypoints[0] is the spawn point */
    e->target_waypoint_index = 1;
}
```

---

*CIO-TD | Defence.Inc Project*
