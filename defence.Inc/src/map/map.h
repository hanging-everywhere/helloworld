/**
 * @file    map.h
 * @brief   地图模块：网格数据、路径路点、背景图、渲染接口
 *          Map module: grid data, path waypoints, background image, rendering.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 2.0  迁移自 prototype/v2/Map.h，更新为精细 32px 网格（24×32）
 *               Migrated from prototype/v2/Map.h, updated to fine 32px grid (24×32).
 */

#ifndef MAP_H
#define MAP_H

#include "raylib.h"

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

#define CELL_SIZE       32      /* 每格像素大小 / Pixels per cell */
#define MAP_ROWS        24      /* 地图行数（24×32=768px）/ Map rows */
#define MAP_COLS        32      /* 地图列数（32×32=1024px）/ Map cols */
#define MAX_WAYPOINTS   40      /* 最大路点数量 / Maximum waypoint count */

/* 格子类型 / Cell types */
#define CELL_TYPE_PATH       0  /* 敌人行走路径 / Enemy walk path */
#define CELL_TYPE_BUILDABLE  1  /* 可建造炮塔区域 / Buildable area */
#define CELL_TYPE_OBSTACLE   2  /* 障碍物/已占用 / Obstacle or occupied */

/* ============================================================
 * 类型定义 | Type Definitions
 * ============================================================ */

/**
 * @brief 整数二维坐标点（网格坐标或像素坐标）
 *        Integer 2D coordinate point (grid or pixel coordinate).
 */
typedef struct {
    int x;  /* 列索引或像素横坐标 / Column index or pixel x */
    int y;  /* 行索引或像素纵坐标 / Row index or pixel y */
} Point;

/**
 * @brief 地图主结构体，包含网格、路点与背景图
 *        Main map structure: grid, waypoints, and background image.
 */
typedef struct {
    int       grid[MAP_ROWS][MAP_COLS];  /* 格子类型二维数组 / 2D cell type array */
    Point     waypoints[MAX_WAYPOINTS];  /* 路点像素中心坐标 / Waypoint pixel center coords */
    int       waypoint_count;            /* 实际路点数量 / Actual waypoint count */
    Texture2D bg_img;                    /* 关卡背景贴图 / Level background texture */
} Map;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  初始化地图，加载指定关卡的网格与路点数据
 *         Initialize the map for the given level.
 * @param  m      指向 Map 结构体的指针 / Pointer to Map struct
 * @param  level  关卡编号（1、2、3）/ Level number (1, 2, 3)
 */
void map_init(Map* m, int level);

/**
 * @brief  渲染地图（背景图 + 按住 SPACE 显示调试网格）
 *         Render the map (background + debug grid on SPACE).
 * @param  m  指向 Map 结构体的指针 / Pointer to Map struct
 */
void map_draw(const Map* m);

/**
 * @brief  获取指定网格格子的像素中心坐标
 *         Get the pixel center of a given grid cell.
 * @param  row  行号 / Row index
 * @param  col  列号 / Col index
 * @return 像素中心坐标 / Pixel center
 */
Point map_get_center(int row, int col);

/**
 * @brief  获取路点数组指针（只读）
 *         Get a read-only pointer to the waypoints array.
 * @param  m  指向 Map 结构体的指针 / Pointer to Map struct
 * @return 路点数组首地址 / Pointer to first waypoint
 */
const Point* map_get_waypoints(const Map* m);

/**
 * @brief  获取路点总数量
 *         Get the total waypoint count.
 * @param  m  指向 Map 结构体的指针 / Pointer to Map struct
 * @return 路点数量 / Waypoint count
 */
int map_get_waypoint_count(const Map* m);

/**
 * @brief  检查指定格子是否可以放置炮塔（CELL_TYPE_BUILDABLE）
 *         Check if a cell is buildable (CELL_TYPE_BUILDABLE).
 * @param  m    指向 Map 的指针 / Pointer to Map
 * @param  row  行号 / Row
 * @param  col  列号 / Col
 * @return 1=可放置 / placeable；0=不可放置 / not placeable
 */
int map_can_place_tower(const Map* m, int row, int col);

/**
 * @brief  检查指定格子是否为路径格（CELL_TYPE_PATH）
 *         Check if a cell is a path tile.
 * @param  m    指向 Map 的指针 / Pointer to Map
 * @param  row  行号 / Row
 * @param  col  列号 / Col
 * @return 1=路径格 / path；0=非路径格 / not path
 */
int map_is_path(const Map* m, int row, int col);

/**
 * @brief  在指定格子标记已放置炮塔（改为 CELL_TYPE_OBSTACLE）
 *         Mark a cell as tower-occupied (CELL_TYPE_OBSTACLE).
 * @param  m    指向 Map 的指针 / Pointer to Map
 * @param  row  行号 / Row
 * @param  col  列号 / Col
 */
void map_place_tower(Map* m, int row, int col);

/**
 * @brief  恢复格子类型为可建造（炮塔拆除时调用）
 *         Restore a cell to buildable type (called when tower is removed).
 * @param  m    指向 Map 的指针 / Pointer to Map
 * @param  row  行号 / Row
 * @param  col  列号 / Col
 */
void map_restore_cell(Map* m, int row, int col);

#endif /* MAP_H */
