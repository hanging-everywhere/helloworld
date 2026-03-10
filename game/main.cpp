#include <graphics.h>
#include <time.h>
#include "GameManager.h" // 引入游戏大管家

int main() {
	// 1. 初始化图形窗口 (宽 1024, 高 768)
	setinitmode(INIT_RENDERMANUAL | INIT_ANIMATION);
	initgraph(1024, 768);
	setcaption("千年城影：华夏防御史"); // 游戏标题
	
	// 实例化游戏大管家并初始化
	GameManager game;
	game.init();
	
	// 记录上一帧的时间
	long lastTime = clock();
	
	// 2. 游戏主循环 (Game Loop)
	while (is_run()) {
		// 计算 deltaTime，保证 10~30 分钟的游玩时长不会因为掉帧而变慢
		long currentTime = clock();
		double deltaTime = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
		lastTime = currentTime;
		
		// 将三大核心任务委托给 GameManager
		game.processInput();
		game.updateLogic(deltaTime);
		game.renderGraphics();
		
		// 3. 帧率控制，锁定 60 FPS
		delay_fps(60); 
	}
	
	// 清理并关闭窗口
	closegraph();
	return 0;
}
