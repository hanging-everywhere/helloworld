/**
 * @file    dataload.c
 * @brief   数据驱动加载实现
 *          Data-driven loading implementation.
 */

#include "dataload.h"
#include <stdio.h>
#include <string.h>

#define TOWERS_CFG "data/towers.cfg"
#define WAVES_CFG  "data/waves.cfg"

/* ============================================================
 * dataload_towers
 * ============================================================ */
int dataload_towers(TowerDef* defs, int max_count) {
    FILE* f;
    char  line[128];
    int   count = 0;

    f = fopen(TOWERS_CFG, "r");
    if (!f) return 0; /* 文件缺失，调用方使用硬编码兜底 / Caller falls back to hardcoded */

    while (fgets(line, sizeof(line), f) && count < max_count) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        TowerDef* d = &defs[count];
        /*
         * 解析：name range damage fire_rate cost_wood cost_stone cost_metal cost_material
         * Parse: name range damage fire_rate cost_wood cost_stone cost_metal cost_material
         */
        if (sscanf(line, " %15s %f %f %f %d %d %d %d",
                   d->name,
                   &d->range, &d->damage, &d->fire_rate,
                   &d->cost_wood, &d->cost_stone,
                   &d->cost_metal, &d->cost_material) == 8) {
            count++;
        }
    }

    fclose(f);
    return count;
}

/* ============================================================
 * dataload_waves
 * ============================================================ */
int dataload_waves(WaveDef* defs, int max_count) {
    FILE* f;
    char  line[128];
    int   count = 0;

    f = fopen(WAVES_CFG, "r");
    if (!f) return 0;

    while (fgets(line, sizeof(line), f) && count < max_count) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        WaveDef* d = &defs[count];
        /* 解析：level wave enemy_count spawn_interval */
        if (sscanf(line, " %d %d %d %f",
                   &d->level, &d->wave,
                   &d->enemy_count, &d->spawn_interval) == 4) {
            count++;
        }
    }

    fclose(f);
    return count;
}

/* ============================================================
 * dataload_find_wave
 * ============================================================ */
const WaveDef* dataload_find_wave(const WaveDef* defs, int count,
                                   int level, int wave) {
    int i;
    for (i = 0; i < count; i++) {
        if (defs[i].level == level && defs[i].wave == wave)
            return &defs[i];
    }
    return NULL; /* 找不到时调用方使用硬编码兜底 / Caller uses hardcoded fallback */
}

/* ============================================================
 * dataload_find_tower
 * ============================================================ */
const TowerDef* dataload_find_tower(const TowerDef* defs, int count,
                                     const char* name) {
    int i;
    for (i = 0; i < count; i++) {
        if (strncmp(defs[i].name, name, TOWER_NAME_LEN) == 0)
            return &defs[i];
    }
    return NULL;
}
