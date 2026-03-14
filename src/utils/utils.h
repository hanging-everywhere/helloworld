/**
 * @file    utils.h
 * @brief   公共工具函数：数学辅助、距离计算等通用函数
 *          Utility functions: math helpers, distance calculation, and other common functions.
 * @author  主工程师 / Lead Engineer
 * @date    2026-03-10
 * @version 1.0
 */

#ifndef UTILS_H
#define UTILS_H

/* ============================================================
 * 数学工具 | Math Utilities
 * ============================================================ */

/**
 * @brief  计算两点间的欧几里得距离
 *         Compute Euclidean distance between two points.
 * @param  x1  起点 x / Start x
 * @param  y1  起点 y / Start y
 * @param  x2  终点 x / End x
 * @param  y2  终点 y / End y
 * @return 两点间的浮点距离 / Float distance between the two points
 */
float utils_distance(float x1, float y1, float x2, float y2);

/**
 * @brief  将浮点数限制在 [min, max] 区间内
 *         Clamp a float value to the [min, max] range.
 * @param  value  输入值 / Input value
 * @param  min    最小值 / Minimum value
 * @param  max    最大值 / Maximum value
 * @return 限制后的值 / Clamped value
 */
float utils_clampf(float value, float min, float max);

/**
 * @brief  线性插值（lerp）
 *         Linear interpolation (lerp).
 * @param  a    起始值 / Start value
 * @param  b    结束值 / End value
 * @param  t    插值参数（0.0 = a，1.0 = b）/ Interpolation parameter (0.0 = a, 1.0 = b)
 * @return 插值结果 / Interpolated result
 */
float utils_lerpf(float a, float b, float t);

#endif /* UTILS_H */
