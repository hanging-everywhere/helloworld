/**
 * @file    scale.h
 * @brief   全局缩放/偏移参数（由 main.c 每帧更新，供各模块鼠标坐标换算使用）
 *          Global scale/offset params updated each frame in main.c,
 *          used for mouse-coordinate correction across all modules.
 */

#ifndef SCALE_H
#define SCALE_H

/* 由 main.c 定义，其余模块通过此头文件访问 */
extern float g_scaleX;   /* 实际窗口宽 / 逻辑宽（1024） */
extern float g_scaleY;   /* 实际窗口高 / 逻辑高（768）  */
extern int   g_offsetX;  /* 黑边水平偏移（像素）         */
extern int   g_offsetY;  /* 黑边垂直偏移（像素）         */

/* 鼠标坐标换算宏 — 将屏幕坐标转为逻辑游戏坐标 */
#define MOUSE_LX()  ((int)((GetMouseX() - g_offsetX) / g_scaleX))
#define MOUSE_LY()  ((int)((GetMouseY() - g_offsetY) / g_scaleY))

#endif /* SCALE_H */
