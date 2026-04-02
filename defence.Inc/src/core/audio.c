/**
 * @file    audio.c
 * @brief   独立音效系统实现
 *          Independent audio system implementation.
 */

#include "audio.h"
#include <stdio.h>
#include <string.h>

#define CFG_PATH  "assets/sounds/sounds.cfg"
#define BGM_CFG   "assets/sounds/bgm.cfg"
#define FILE_BUFLEN 64

/* ============================================================
 * audio_init
 * ============================================================ */
void audio_init(AudioSystem* a, float volume, int sfx_enabled) {
    FILE* f;
    char  line[128], name_buf[SFX_NAME_LEN], file_buf[FILE_BUFLEN];
    char  full_path[FILE_BUFLEN + 16];
    int   i;

    /* 初始化字段 / Zero fields */
    a->sfx_count  = 0;
    a->bgm_count  = 0;
    a->bgm_current = -1;
    a->volume     = (volume < 0.0f) ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    a->sfx_enabled = sfx_enabled;
    for (i = 0; i < MAX_SFX; i++) {
        a->names[i][0] = '\0';
        a->sfx[i] = (Sound){0};
    }
    for (i = 0; i < MAX_BGM; i++) {
        a->bgm_names[i][0] = '\0';
        a->bgm_loaded[i] = 0;
    }

    /* 应用初始音量 / Apply initial volume */
    SetMasterVolume(a->volume);

    /* 读取配置文件 / Read config file */
    f = fopen(CFG_PATH, "r");
    if (!f) {
        /* 配置文件缺失时静默失败，不崩溃 / Fail silently if cfg missing */
        return;
    }

    while (fgets(line, sizeof(line), f) && a->sfx_count < MAX_SFX) {
        /* 跳过注释行和空行 / Skip comment lines and blank lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        /* 解析 name=filename / Parse name=filename */
        if (sscanf(line, " %31[^=]=%63s", name_buf, file_buf) == 2) {
            /* 拼完整路径：assets/sounds/<filename> */
            snprintf(full_path, sizeof(full_path), "assets/sounds/%s", file_buf);

            a->sfx[a->sfx_count] = LoadSound(full_path);
            strncpy(a->names[a->sfx_count], name_buf, SFX_NAME_LEN - 1);
            a->names[a->sfx_count][SFX_NAME_LEN - 1] = '\0';
            a->sfx_count++;
        }
    }

    fclose(f);

    /* 读取 BGM 配置 / Read BGM config */
    f = fopen(BGM_CFG, "r");
    if (f) {
        while (fgets(line, sizeof(line), f) && a->bgm_count < MAX_BGM) {
            if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
            if (sscanf(line, " %31[^=]=%63s", name_buf, file_buf) == 2) {
                snprintf(full_path, sizeof(full_path), "assets/sounds/%s", file_buf);
                a->bgm[a->bgm_count] = LoadMusicStream(full_path);
                if (a->bgm[a->bgm_count].ctxData != NULL) {
                    a->bgm[a->bgm_count].looping = 1;
                    strncpy(a->bgm_names[a->bgm_count], name_buf, BGM_NAME_LEN - 1);
                    a->bgm_names[a->bgm_count][BGM_NAME_LEN - 1] = '\0';
                    a->bgm_loaded[a->bgm_count] = 1;
                    a->bgm_count++;
                }
            }
        }
        fclose(f);
    }
}

/* ============================================================
 * audio_play
 * ============================================================ */
void audio_play(AudioSystem* a, const char* name) {
    int i;
    if (!a->sfx_enabled) return; /* 静音时跳过 / Skip when muted */

    for (i = 0; i < a->sfx_count; i++) {
        if (strncmp(a->names[i], name, SFX_NAME_LEN) == 0) {
            PlaySound(a->sfx[i]);
            return;
        }
    }
    /* 找不到指定音效时静默失败，防止崩溃 / Silently ignore unknown names */
}

/* ============================================================
 * audio_set_volume
 * ============================================================ */
void audio_set_volume(AudioSystem* a, float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    a->volume = volume;
    SetMasterVolume(volume); /* 立即生效 / Takes effect immediately */
}

/* ============================================================
 * audio_set_sfx_enabled
 * ============================================================ */
void audio_set_sfx_enabled(AudioSystem* a, int enabled) {
    a->sfx_enabled = enabled ? 1 : 0;
}

/* ============================================================
 * audio_play_bgm
 * ============================================================ */
void audio_play_bgm(AudioSystem* a, const char* name) {
    int i;
    /* 先停止当前 BGM / Stop current BGM first */
    audio_stop_bgm(a);

    for (i = 0; i < a->bgm_count; i++) {
        if (a->bgm_loaded[i] && strncmp(a->bgm_names[i], name, BGM_NAME_LEN) == 0) {
            SetMusicVolume(a->bgm[i], a->volume * 0.6f);
            PlayMusicStream(a->bgm[i]);
            a->bgm_current = i;
            return;
        }
    }
}

/* ============================================================
 * audio_stop_bgm
 * ============================================================ */
void audio_stop_bgm(AudioSystem* a) {
    if (a->bgm_current >= 0 && a->bgm_current < a->bgm_count) {
        StopMusicStream(a->bgm[a->bgm_current]);
    }
    a->bgm_current = -1;
}

/* ============================================================
 * audio_update
 * ============================================================ */
void audio_update(AudioSystem* a) {
    if (a->bgm_current >= 0 && a->bgm_current < a->bgm_count) {
        UpdateMusicStream(a->bgm[a->bgm_current]);
        SetMusicVolume(a->bgm[a->bgm_current], a->volume * 0.6f);
    }
}

/* ============================================================
 * audio_unload
 * ============================================================ */
void audio_unload(AudioSystem* a) {
    int i;
    audio_stop_bgm(a);
    for (i = 0; i < a->sfx_count; i++) {
        UnloadSound(a->sfx[i]);
    }
    a->sfx_count = 0;
    for (i = 0; i < a->bgm_count; i++) {
        if (a->bgm_loaded[i]) {
            UnloadMusicStream(a->bgm[i]);
            a->bgm_loaded[i] = 0;
        }
    }
    a->bgm_count = 0;
}
