/**
 * @file    dataload.h
 * @brief   数据驱动加载：从外部配置文件读取炮塔属性和波次参数
 *          Data-driven loading: read tower stats and wave params from external config files.
 *
 * 配置文件 / Config files:
 *   data/towers.cfg  — 炮塔初始属性（射程/伤害/冷却/建造费用）
 *   data/waves.cfg   — 波次参数（敌人数量/生成间隔）
 *
 * 格式均为纯文本，# 开头为注释，字段空格分隔。
 * 修改配置文件后重启游戏即可生效，无需重新编译。
 * Both files are plain text; lines starting with # are comments; fields space-separated.
 * Changes take effect on next game launch — no recompilation needed.
 */

#ifndef DATALOAD_H
#define DATALOAD_H

#define TOWER_NAME_LEN 16  /* 炮塔名字符串最大长度 / Max length of tower name string */

/**
 * @brief 炮塔初始属性定义 / Tower initial stat definition
 */
typedef struct {
    char  name[TOWER_NAME_LEN]; /**< 炮塔名，对应 TowerType 枚举，如 "sling" */
    float range;                /**< 攻击射程（像素）/ Attack range (px) */
    float damage;               /**< 基础伤害 / Base damage */
    float fire_rate;            /**< 攻击冷却（秒）/ Attack cooldown (s) */
    int   cost_wood;            /**< 建造费用：木材 / Build cost: wood */
    int   cost_stone;           /**< 建造费用：石料 / Build cost: stone */
    int   cost_metal;           /**< 建造费用：金属 / Build cost: metal */
    int   cost_material;        /**< 建造费用：科技材料 / Build cost: material */
} TowerDef;

/**
 * @brief 波次参数定义 / Wave parameter definition
 */
typedef struct {
    int   level;          /**< 关卡（1-3）/ Level number (1-3) */
    int   wave;           /**< 波次（1-3）/ Wave number (1-3) */
    int   enemy_count;    /**< 该波生成敌人总数 / Total enemies to spawn */
    float spawn_interval; /**< 敌人生成间隔（秒）/ Spawn interval (s) */
} WaveDef;

/**
 * @brief  从 data/towers.cfg 加载炮塔属性表
 *         Load tower definitions from data/towers.cfg.
 * @param  defs      输出数组 / Output array
 * @param  max_count 数组容量 / Array capacity
 * @return 成功加载的条目数；0 表示文件不存在（调用方应使用硬编码兜底）
 *         Number of entries loaded; 0 = file absent (caller should use hardcoded fallback)
 */
int dataload_towers(TowerDef* defs, int max_count);

/**
 * @brief  从 data/waves.cfg 加载波次参数表
 *         Load wave definitions from data/waves.cfg.
 * @param  defs      输出数组 / Output array
 * @param  max_count 数组容量 / Array capacity
 * @return 成功加载的条目数 / Number of entries loaded
 */
int dataload_waves(WaveDef* defs, int max_count);

/**
 * @brief  在波次定义表中查找指定关卡+波次的条目
 *         Find wave definition by level and wave number.
 * @return 指向匹配 WaveDef 的指针；找不到返回 NULL
 *         Pointer to matching WaveDef, or NULL if not found
 */
const WaveDef* dataload_find_wave(const WaveDef* defs, int count,
                                   int level, int wave);

/**
 * @brief  在炮塔定义表中按名称查找条目
 *         Find tower definition by name.
 * @return 指向匹配 TowerDef 的指针；找不到返回 NULL
 */
const TowerDef* dataload_find_tower(const TowerDef* defs, int count,
                                     const char* name);

#endif /* DATALOAD_H */
