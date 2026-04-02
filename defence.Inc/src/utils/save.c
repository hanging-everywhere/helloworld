/**
 * @file    save.c
 * @brief   存档系统实现：手写极简 JSON 读写，无外部依赖
 *          Save system implementation: hand-rolled minimal JSON I/O, no external deps.
 */

#include "save.h"
#include <stdio.h>
#include <string.h>

/* ============================================================
 * save_write
 * ============================================================ */
int save_write(const SaveData* data) {
    FILE* f = fopen(SAVE_FILE, "w");
    if (!f) return 0; /* 无写权限时静默失败 / Silently fail if no write access */

    fprintf(f, "{\n");
    fprintf(f, "  \"max_level_cleared\": %d,\n", data->max_level_cleared);
    fprintf(f, "  \"master_volume\": %.2f,\n",    data->master_volume);
    fprintf(f, "  \"sfx_enabled\": %d\n",         data->sfx_enabled);
    fprintf(f, "}\n");

    fclose(f);
    return 1;
}

/* ============================================================
 * save_read
 * ============================================================ */
int save_read(SaveData* data) {
    FILE* f;
    char  line[128];
    int   ok = 0;

    /* 填充默认值，确保调用者始终获得合法数据 / Defaults — caller always gets valid data */
    data->max_level_cleared = 0;
    data->master_volume     = 0.5f;
    data->sfx_enabled       = 1;

    f = fopen(SAVE_FILE, "r");
    if (!f) return 0; /* 文件不存在，使用默认值 / File absent — use defaults */

    while (fgets(line, sizeof(line), f)) {
        /*
         * 逐行尝试匹配每个字段。
         * sscanf 的前导空格跳过缩进，字段顺序无关，单行损坏不影响其他行。
         * Each line is tried against each field.
         * Leading space skips indentation; field order-independent;
         * a corrupted line doesn't break other fields.
         */
        sscanf(line, " \"max_level_cleared\": %d",  &data->max_level_cleared);
        sscanf(line, " \"master_volume\": %f",       &data->master_volume);
        sscanf(line, " \"sfx_enabled\": %d",         &data->sfx_enabled);
        ok = 1;
    }

    fclose(f);

    /* 钳位保护 / Clamp values to valid ranges */
    if (data->max_level_cleared < 0) data->max_level_cleared = 0;
    if (data->max_level_cleared > 3) data->max_level_cleared = 3;
    if (data->master_volume < 0.0f)  data->master_volume = 0.0f;
    if (data->master_volume > 1.0f)  data->master_volume = 1.0f;
    data->sfx_enabled = data->sfx_enabled ? 1 : 0;

    return ok;
}

/* ============================================================
 * save_exists
 * ============================================================ */
int save_exists(void) {
    SaveData d;
    return save_read(&d) && d.max_level_cleared > 0;
}
