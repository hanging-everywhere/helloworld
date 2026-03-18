#include "raylib.h"
#include "GameManager.h"

int main(void) {
	// 初始化窗口，1024x768
	InitWindow(1024, 768, "千年城影  - 旧石器时代防线");
	
	// 自动锁定 60 帧！不再需要手动 delay
	SetTargetFPS(60);
	
	GameManager gm;
	GameManager_init(&gm,1);
	
	// 主游戏循环，只要窗口没被关闭就一直运行
	while (!WindowShouldClose()) {
		
		// Raylib 直接提供 GetFrameTime() 获取两帧之间的时间差 (deltaTime)
		double deltaTime = GetFrameTime();
		
		// 核心逻辑更新
		GameManager_processInput(&gm);
		GameManager_updateLogic(&gm, deltaTime);
		
		// 渲染图形
		GameManager_renderGraphics(&gm);
	}
	GameManager_unloadAllTextures(&gm);
	// 关闭窗口，释放资源
	CloseWindow();
	
	return 0;
}
