# TDD: Map 模块 | TDD: Map Module

**版本 / Version**: 1.0
**作者 / Author**: CIO-TD
**状态 / Status**: [ ] 草稿  [x] 已审阅  [ ] 已锁定
**最后更新 / Last Updated**: 2026-03-10

> **状态说明**：本 TDD 已基于原型代码分析完成草稿，等待主工程师确认路径接口后锁定。
> **Status Note**: Draft completed based on prototype code analysis. Awaiting lead engineer confirmation of path interface before locking.

---

## 1. 模块职责 | Module Responsibility

**中文**

负责游戏地图的数据结构定义、初始化、渲染，以及路径（路点）数据的存储与查询。
**不**负责：敌人移动逻辑、炮塔放置逻辑（这些在各自模块中处理）。

**English**

Manages game map data structure definition, initialization, rendering, and storage/querying of path waypoint data.
**Does NOT** handle: enemy movement logic, tower placement logic (handled in their respective modules).

---

## 2. 数据结构定义 | Data Structure Definitions

```c
/* 地图格子类型 / Map cell types */
#define CELL_GRASS   0   /* 可建塔的草地 / Buildable grass */
#define CELL_PATH    1   /* 敌人行走的路 / Enemy path */
#define CELL_TOWER   2   /* 已放置炮塔的格子 / Occupied by tower */

/* 地图尺寸常量 / Map dimension constants */
#define MAP_ROWS        12
#define MAP_COLS        16
#define CELL_SIZE       64      /* 每格像素大小 / Pixels per cell */
#define MAX_WAYPOINTS   20      /* 最大路点数量 / Maximum waypoint count */

/* 整数坐标点（网格坐标 or 像素坐标）/ Integer coordinate point (grid or pixel) */
typedef struct {
    int x;  /* 列/像素横坐标 / Column or pixel x */
    int y;  /* 行/像素纵坐标 / Row or pixel y */
} Point;

/* 地图主结构体 / Main map struct */
typedef struct {
    int   grid[MAP_ROWS][MAP_COLS];  /* 格子类型二维数组 / 2D array of cell types */
    Point waypoints[MAX_WAYPOINTS];  /* 路点像素坐标数组（中心点）/ Waypoint pixel coords (centers) */
    int   waypoint_count;            /* 实际路点数量 / Actual waypoint count */
} Map;
```

**路点说明 / Waypoint Notes**：
- `waypoints[0]` = 敌人出生点（地图左侧）/ Enemy spawn (left side of map)
- `waypoints[waypoint_count-1]` = 大本营位置（篝火）/ Base position (bonfire)
- 路点坐标为**像素中心坐标**（非网格坐标）/ Waypoint coords are **pixel center coords** (not grid coords)

---

## 3. 对外接口（Public API）| Public API

```c
/**
 * @brief  初始化地图，设置网格数据和路点数组
 *         Initialize the map: set up grid data and waypoint array.
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 */
void map_init(Map* m);

/**
 * @brief  渲染地图所有格子（草地 / 路径 / 炮塔格）
 *         Render all map cells (grass / path / tower tiles).
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 */
void map_draw(const Map* m);

/**
 * @brief  获取指定网格坐标的像素中心点
 *         Get the pixel center of a given grid cell.
 * @param  row  行号（0 到 MAP_ROWS-1）/ Row index (0 to MAP_ROWS-1)
 * @param  col  列号（0 到 MAP_COLS-1）/ Col index (0 to MAP_COLS-1)
 * @return 该格子的像素中心坐标 / Pixel center coordinates of the cell
 */
Point map_get_center(int row, int col);

/**
 * @brief  获取路点数组指针（供 enemy 模块使用）
 *         Get pointer to the waypoints array (used by enemy module).
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 * @return 路点数组首地址 / Pointer to first waypoint
 */
const Point* map_get_waypoints(const Map* m);

/**
 * @brief  获取路点数量
 *         Get the number of waypoints.
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 * @return 路点总数 / Total waypoint count
 */
int map_get_waypoint_count(const Map* m);

/**
 * @brief  检查指定格子是否可以放置炮塔
 *         Check if a cell is available for tower placement.
 * @param  m    指向地图结构体的指针 / Pointer to Map struct
 * @param  row  行号 / Row index
 * @param  col  列号 / Col index
 * @return 1 = 可放置 / can place；0 = 不可放置 / cannot place
 */
int map_can_place_tower(const Map* m, int row, int col);

/**
 * @brief  在指定格子标记已放置炮塔（将 grid[row][col] 设为 CELL_TOWER）
 *         Mark a cell as occupied by a tower.
 * @param  m    指向地图结构体的指针 / Pointer to Map struct
 * @param  row  行号 / Row index
 * @param  col  列号 / Col index
 */
void map_place_tower(Map* m, int row, int col);
```

---

## 4. 内部实现说明 | Internal Implementation Notes

### 4.1 地图初始化 | Map Initialization

**中文**

`map_init` 将所有格子初始化为 `CELL_GRASS`（0），然后按照预设的 S 型路径将对应格子设置为 `CELL_PATH`（1），最后计算各路点的像素中心坐标并存入 `waypoints` 数组。

当前 S 型路径设计（来自原型）：
- 第 2 行，第 0-12 列（横向）
- 第 3-8 行，第 12 列（竖向向下）
- 第 8 行，第 3-12 列（横向向左）
- 第 9-11 行，第 3 列（竖向向下到底）

**English**

`map_init` sets all cells to `CELL_GRASS` (0), then marks the S-shaped path cells as `CELL_PATH` (1), and finally computes pixel center coordinates for each waypoint stored in `waypoints`.

Current S-path design (from prototype):
- Row 2, cols 0–12 (horizontal)
- Rows 3–8, col 12 (vertical down)
- Row 8, cols 3–12 (horizontal left)
- Rows 9–11, col 3 (vertical down to base)

### 4.2 像素中心坐标计算 | Pixel Center Calculation

```
pixel_x = col * CELL_SIZE + CELL_SIZE / 2
pixel_y = row * CELL_SIZE + CELL_SIZE / 2
```

---

## 5. 依赖关系 | Dependencies

**中文**

- **依赖（需要）**：`raylib.h`（用于 `DrawRectangle`、`Color` 等渲染函数）
- **被依赖（提供给）**：`enemy` 模块（路点数组）、`core` 模块（炮塔放置判断）、`tower` 模块（格子中心坐标）
- **禁止循环依赖**：`map` 模块**不得** `#include` 任何游戏逻辑模块（enemy、tower、core 等）

**English**

- **Depends on**: `raylib.h` (for `DrawRectangle`, `Color`, etc.)
- **Depended on by**: `enemy` module (waypoints), `core` module (tower placement check), `tower` module (cell center coords)
- **No circular dependency**: `map` module must **NOT** `#include` any game logic module (enemy, tower, core, etc.)

**依赖图 / Dependency Graph**:
```
raylib.h
    ↓
map.h  ←  enemy.h
       ←  core.h (game.h)
       ←  tower.h
```

---

## 6. 已知限制与 TODO | Known Limitations & TODO

**中文**

- [ ] 当前路径是硬编码的 S 型，若需多关卡需重构为数据驱动（M3+ 考虑）
- [ ] 路点坐标仅为整数，若需子像素精度需改为 `float`（当前精度已足够）
- [ ] `MAX_WAYPOINTS` 硬限制为 20，当前 5 个路点远未达上限，无风险
- [ ] 渲染仅使用纯色矩形；若美术资源就绪，可扩展为贴图渲染（M4 考虑）

**English**

- [ ] Current path is hardcoded S-shape; multi-level support requires data-driven refactor (consider in M3+)
- [ ] Waypoint coords are integer only; subpixel precision would require `float` (current precision sufficient)
- [ ] `MAX_WAYPOINTS` hard limit is 20; current 5 waypoints far below limit, no risk
- [ ] Rendering uses solid color rectangles only; texture rendering can be added when art assets are ready (consider in M4)

---

## 7. 原型代码迁移说明 | Prototype Migration Notes

> 本节供主工程师迁移时参考。
> For lead engineer reference during migration.

**原型文件 / Prototype files**: `defense.Inc/Map.c` + `defense.Inc/Map.h`

**需要变更的内容 / Changes required**:

| 原型 / Prototype | 新规范 / New Standard | 原因 / Reason |
|---|---|---|
| `Map_init`, `Map_draw` | `map_init`, `map_draw` | 命名规范：小写 + 下划线 |
| `Map_getCenter` | `map_get_center` | 命名规范 |
| 无 `map_get_waypoints` | 需新增 | 避免 enemy.c 直接访问 map 内部字段 |
| 无 `map_can_place_tower` | 需新增 | 封装 grid 访问逻辑 |
| `#define ROWS`, `COLS` | `#define MAP_ROWS`, `MAP_COLS` | 避免命名冲突 |
| `typedef struct { int x, y; } Point` | 同上，但注意只在 `map.h` 定义一次 | 防止多次定义错误 |

**文件头注释 / File header**: 按 CODING_STANDARD.md 格式添加。

---

*CIO-TD | Defence.Inc Project*
