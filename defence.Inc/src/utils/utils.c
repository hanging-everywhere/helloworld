/**
 * @file    utils.c
 * @brief   公共工具函数实现
 *          Utility function implementations.
 */

#include "utils.h"
#include <math.h>

float utils_distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

float utils_clampf(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float utils_lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}
