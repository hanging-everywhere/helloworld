/**
 * @file    save.h
 * @brief   存档系统：将关卡进度与设置持久化为 JSON 文件
 *          Save system: persist level progress and settings to a JSON file.
 *
 * 存档文件位置 / Save file location: savegame.json（游戏可执行文件同目录）
 * 格式 / Format:
 *   {
 *     "max_level_cleared": 1,
 *     "master_volume": 0.50,
 *     "sfx_enabled": 1
 *   }
 */

#ifndef SAVE_H
#define SAVE_H

/** 存档文件路径（相对于工作目录）/ Save file path (relative to working dir) */
#define SAVE_FILE "savegame.json"

/**
 * @brief 存档数据结构 / Save data structure
 */
typedef struct {
    int   max_level_cleared; /**< 最高已通关关卡（0=未通关，1/2/3）/ Highest cleared level */
    float master_volume;     /**< 主音量 0.0~1.0 / Master volume */
    int   sfx_enabled;       /**< 音效开关 1=开 0=静音 / SFX on/off */
} SaveData;

/**
 * @brief  将存档写入 savegame.json
 *         Write save data to savegame.json.
 * @return 1=成功 0=失败（无写权限等）/ 1=success 0=failure
 */
int save_write(const SaveData* data);

/**
 * @brief  从 savegame.json 读取存档；文件不存在或损坏时填充默认值
 *         Read save data from savegame.json; fills defaults on failure.
 * @return 1=文件存在且成功读取 0=使用默认值 / 1=read ok 0=defaults used
 */
int save_read(SaveData* data);

/**
 * @brief  检查有效存档是否存在（max_level_cleared > 0）
 *         Check if a valid save exists (max_level_cleared > 0).
 * @return 1=存在 0=不存在 / 1=exists 0=not found
 */
int save_exists(void);

#endif /* SAVE_H */
