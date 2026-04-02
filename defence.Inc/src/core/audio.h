/**
 * @file    audio.h
 * @brief   独立音效系统：配置文件驱动，支持动态扩展
 *          Independent audio system: config-file driven, easily extensible.
 *
 * 使用方式 / Usage:
 *   1. 在 assets/sounds/sounds.cfg 中按 name=filename 格式添加音效
 *   2. 调用 audio_init() 自动加载所有条目
 *   3. 运行时调用 audio_play(&g->audio, "explosion") 即可播放
 *      添加新音效只需改 sounds.cfg，无需重新编译
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

/* ── 容量常量 / Capacity constants ── */
#define MAX_SFX      16   /* 最多同时注册的音效数量 / Max registered sounds */
#define SFX_NAME_LEN 32   /* 音效标识名最大长度 / Max length of sound name key */
#define MAX_BGM       4   /* 最多同时注册的背景音乐数量 / Max registered BGM tracks */
#define BGM_NAME_LEN 32   /* BGM 标识名最大长度 / Max length of BGM name key */

/**
 * @brief 音效系统 / Audio system
 *
 * 所有 Sound/Music 句柄和名称表集中管理，Game struct 持有一个此结构体实例。
 * All Sound/Music handles and name table are managed centrally.
 * The Game struct owns one instance of this struct.
 */
typedef struct {
    Sound sfx[MAX_SFX];                /* Raylib Sound 句柄数组 / Sound handle array */
    char  names[MAX_SFX][SFX_NAME_LEN];/* 对应的标识键名 / Corresponding name keys */
    int   sfx_count;                   /* 已加载数量 / Number of loaded sounds */
    float volume;                      /* 当前主音量 0.0~1.0 / Current master volume */
    int   sfx_enabled;                 /* 1=开启音效 0=静音 / SFX on/off */

    /* 背景音乐 / Background music */
    Music bgm[MAX_BGM];                /* Raylib Music 句柄数组 / Music handle array */
    char  bgm_names[MAX_BGM][BGM_NAME_LEN]; /* BGM 标识键名 / BGM name keys */
    int   bgm_loaded[MAX_BGM];         /* 1=已加载 0=未加载 / Load flags */
    int   bgm_count;                   /* 已注册 BGM 数量 / Registered BGM count */
    int   bgm_current;                 /* 当前播放索引 (-1=无) / Currently playing (-1=none) */
} AudioSystem;

/**
 * @brief  初始化音效系统：读取 assets/sounds/sounds.cfg 并加载所有音效
 *         Initialize audio system: reads sounds.cfg and loads all listed sounds.
 * @param  a           指向 AudioSystem 的指针 / Pointer to AudioSystem
 * @param  volume      初始主音量 (0.0~1.0) / Initial master volume
 * @param  sfx_enabled 初始音效开关 (1=开 0=静音) / Initial SFX enabled flag
 */
void audio_init(AudioSystem* a, float volume, int sfx_enabled);

/**
 * @brief  按名称播放音效（sfx_enabled==0 时静默，找不到时不崩溃）
 *         Play a sound by name (silent when sfx_enabled==0, no crash if not found).
 * @param  a    指向 AudioSystem 的指针 / Pointer to AudioSystem
 * @param  name 音效标识键名，如 "explosion" / Sound name key, e.g. "explosion"
 */
void audio_play(AudioSystem* a, const char* name);

/**
 * @brief  调整主音量（立即调用 SetMasterVolume 生效）
 *         Set master volume (calls SetMasterVolume immediately).
 * @param  a      指向 AudioSystem 的指针 / Pointer to AudioSystem
 * @param  volume 新音量 0.0~1.0 / New volume level
 */
void audio_set_volume(AudioSystem* a, float volume);

/**
 * @brief  开关音效（不影响音量数值，仅控制 audio_play 是否执行）
 *         Toggle SFX on/off (does not alter volume value).
 * @param  a       指向 AudioSystem 的指针 / Pointer to AudioSystem
 * @param  enabled 1=开启 0=静音 / 1=enabled 0=muted
 */
void audio_set_sfx_enabled(AudioSystem* a, int enabled);

/**
 * @brief  按名称播放背景音乐（循环播放，会停止当前 BGM）
 *         Play BGM by name (loops, stops current BGM first).
 * @param  a    指向 AudioSystem 的指针 / Pointer to AudioSystem
 * @param  name BGM 标识键名，如 "stone" / BGM name key, e.g. "stone"
 */
void audio_play_bgm(AudioSystem* a, const char* name);

/**
 * @brief  停止当前背景音乐
 *         Stop current BGM.
 */
void audio_stop_bgm(AudioSystem* a);

/**
 * @brief  每帧调用：更新 Music 流（必须在主循环中调用）
 *         Call every frame: updates Music stream (must be called in main loop).
 */
void audio_update(AudioSystem* a);

/**
 * @brief  卸载所有 Sound 和 Music 资源（CloseAudioDevice 在 game.c 中单独调用）
 *         Unload all Sound/Music resources (CloseAudioDevice is called separately in game.c).
 */
void audio_unload(AudioSystem* a);

#endif /* AUDIO_H */
