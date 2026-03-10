#include <graphics.h>
#include <time.h>
#include "GameManager.h"

int main() {
	// 1. 初始化 EGE 窗口 (1024x768)，开启手动渲染模式（防止画面闪烁）
	initgraph(1024, 768, INIT_RENDERMANUAL);
	
	// 2. 声明大管家结构体变量
	GameManager gm;
	
	// 3. 传入结构体指针，进行游戏初始化
	GameManager_init(&gm);
	
	// 用于计算每一帧经过的时间 (deltaTime)
	clock_t lastTime = clock();
	double deltaTime = 0.0;
	
	// 4. 游戏主循环 (is_run() 检查窗口是否被关闭)
	for (; is_run(); delay_fps(60)) {
		// 计算两帧之间的时间差（秒）
		clock_t currentTime = clock();
		deltaTime = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
		lastTime = currentTime;
		
		// --- 核心生命周期 ---
		
		// 处理用户输入 (鼠标点击建塔)
		GameManager_processInput(&gm);
		
		// 更新所有游戏逻辑 (塔的攻击、敌人的移动、特效的生命周期)
		GameManager_updateLogic(&gm, deltaTime);
		
		// 渲染画面
		GameManager_renderGraphics(&gm);
	}
	
	// 5. 游戏结束，关闭绘图窗口
	closegraph();
	return 0;
}
