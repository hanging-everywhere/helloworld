/**
 * @file    map.h
 * @brief   地图模块：网格数据、路径路点、渲染接口
 *          Map module: grid data, path waypoints, rendering interface.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 1.0
 */

#ifndef MAP_H
#define MAP_H

#include "raylib.h"

/* ============================================================
 * 常量定义 | Constants
 * ============================================================ */

/* 格子类型 / Cell types */
#define CELL_GRASS   0   /* 可建塔的草地 / Buildable grass tile */
#define CELL_PATH    1   /* 敌人行走路径 / Enemy path tile */
#define CELL_TOWER   2   /* 已放置炮塔   / Tower-occupied tile */

/* 地图尺寸 / Map dimensions */
#define MAP_ROWS        12
#define MAP_COLS        16
#define CELL_SIZE       64      /* 每格像素大小 / Pixels per cell */
#define MAX_WAYPOINTS   20      /* 最大路点数量 / Maximum waypoint count */

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
 * @brief 地图主结构体，包含网格数据与路点信息
 *        Main map structure containing grid data and waypoint info.
 */
typedef struct {
    int   grid[MAP_ROWS][MAP_COLS];  /* 格子类型二维数组 / 2D array of cell types */
    Point waypoints[MAX_WAYPOINTS];  /* 路点像素中心坐标 / Waypoint pixel center coords */
    int   waypoint_count;            /* 实际路点数量     / Actual waypoint count */
} Map;

/* ============================================================
 * 公开接口 | Public API
 * ============================================================ */

/**
 * @brief  初始化地图，设置网格与路点数据
 *         Initialize the map: set up grid and waypoint data.
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 */
void map_init(Map* m);

/**
 * @brief  渲染地图所有格子
 *         Render all map cells.
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 */
void map_draw(const Map* m);

/**
 * @brief  获取指定网格格子的像素中心坐标
 *         Get the pixel center coordinates of a given grid cell.
 * @param  row  行号（0 到 MAP_ROWS-1）/ Row index (0 to MAP_ROWS-1)
 * @param  col  列号（0 到 MAP_COLS-1）/ Col index (0 to MAP_COLS-1)
 * @return 该格子的像素中心坐标 / Pixel center coordinates of the cell
 */
Point map_get_center(int row, int col);

/**
 * @brief  获取路点数组指针（只读）
 *         Get a read-only pointer to the waypoints array.
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 * @return 路点数组首地址 / Pointer to first waypoint
 */
const Point* map_get_waypoints(const Map* m);

/**
 * @brief  获取路点总数量
 *         Get the total number of waypoints.
 * @param  m  指向地图结构体的指针，不得为 NULL
 *            Pointer to Map struct, must not be NULL.
 * @return 路点数量 / Waypoint count
 */
int map_get_waypoint_count(const Map* m);

/**
 * @brief  检查指定格子是否可以放置炮塔
 *         Check whether a cell is available for tower placement.
 * @param  m    指向地图结构体的指针 / Pointer to Map struct
 * @param  row  行号 / Row index
 * @param  col  列号 / Col index
 * @return 1 = 可放置 / placeable；0 = 不可放置 / not placeable
 */
int map_can_place_tower(const Map* m, int row, int col);

/**
 * @brief  在指定格子标记已放置炮塔（设为 CELL_TOWER）
 *         Mark a cell as occupied by a tower (sets to CELL_TOWER).
 * @param  m    指向地图结构体的指针 / Pointer to Map struct
 * @param  row  行号 / Row index
 * @param  col  列号 / Col index
 */
void map_place_tower(Map* m, int row, int col);

#endif /* MAP_H */
