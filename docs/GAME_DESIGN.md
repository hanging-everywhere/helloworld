# 游戏机制设计文档 | Game Design Document

**维护者 / Maintainer**: 胡昊泽（Hu Haoze）/ Designer
**版本 / Version**: 1.0（机制设计初稿 / Mechanism Design v1.0）
**状态 / Status**: 🟡 CIO 审查中 / Under CIO review — 数值待主工程师评估可行性
**最后更新 / Last Updated**: 2026-03-14
**数据来源 / Source**: Defense · Tower.docx（胡昊泽提交）+ 原型代码逆向分析

> **CIO 说明**：v1.0 由机制设计者胡昊泽提交，包含完整的 3 时代 × 13 炮塔 + 基地塔设计。
> 原型代码参考值保留在第 8 节。数值尚未锁定，须经主工程师评估实现可行性后方可进入 TDD。
> **CIO Note**: v1.0 submitted by designer Hu Haoze. Contains full 3-era × 13 tower + base tower design.
> Prototype reference values kept in Section 8. Values NOT locked — requires engineer feasibility review before TDD.

---

## 0. 美术与视角决策（已锁定）| Art & Perspective Decisions (Locked)

| 决策 / Decision | 内容 / Content | 状态 / Status |
|---|---|---|
| 游戏视角 / Perspective | 半俯视视角 / Semi-overhead (Isometric) | ✅ 已锁定 |
| 美术风格 / Art Style | 手绘像素风格 / Hand-drawn Pixel Art | ✅ 已锁定 |
| 窗口分辨率 / Resolution | 1024×768 像素 / pixels | ✅ 已锁定 |
| 格子大小 / Cell Size | 64×64 像素 / pixels | ✅ 已锁定 |
| 帧率 / FPS | 60 | ✅ 已锁定 |
| 关卡数量 / Map Count | 目前已完成 2 张，后续持续扩展 / 2 completed so far, more planned | 🟡 进行中 |
| 炮塔种类 / Tower Types | 13 种 + 1 基地塔 / 13 types + 1 base tower | ✅ 已确认 |

---

## 1. 世界观与时代体系 | World & Era System

游戏横跨三个时代，每个时代有独特的主题、材料体系和炮塔设计哲学。
The game spans three eras, each with a unique theme, material system, and tower design philosophy.

| 时代 / Era | 中文名 | 副标题 | 关键词 | 炮塔数量 |
|---|---|---|---|---|
| 第一时代 / Era 1 | 石器时代 | 蒙昧 | 石、骨、木、火 | 4 座（01-04）|
| 第二时代 / Era 2 | 古代文明 | 理性 | 金属、机械、工艺、秩序 | 4 座（05-08）|
| 第三时代 / Era 3 | 科幻未来 | 星辰 | 能量、力场、意识、维度 | 5 座（09-13）|
| 跨时代 / Cross-Era | 基地塔 | — | 三形态切换 | 1 座 |

### 时代设计哲学 | Era Design Philosophy

**第一时代 · 石器时代 · 蒙昧**
原始人的蒙昧认知，造就了最朴素的防御，同时对自然的崇拜，获得了神的青睐。
Primitive humanity's naive understanding creates the most basic defenses, while nature worship earns divine favor.

**第二时代 · 古代文明 · 理性**
逐渐理解物理规律，用数学和智慧搭建攻防工程。
Gradually understanding physical laws, using mathematics and wisdom to build offensive and defensive engineering.

**第三时代 · 科幻未来 · 星辰**
用概念和规律本身作为武器。
Using concepts and laws themselves as weapons.

---

## 2. 炮塔总览 | Tower Overview

| ID | 名称 / Name | 时代 / Era | 伤害类型 / Damage Type | 攻击方式 / Attack Style |
|---|---|---|---|---|
| 01 | 投石索 / Slingshot | 石器时代 | 钝击 | 直线弹道，命中率70% |
| 02 | 骨矛阵 / Bone Spear Trap | 石器时代 | 穿刺 | 陷阱触发，3次后失效 |
| 03 | 图腾 / Totem | 石器时代 | 精神/恐惧 | 范围怒吼，7秒周期 |
| 04 | 火堆 / Bonfire | 石器时代 | 火焰 | 自动灼烧，消耗柴火 |
| 05 | 诸葛连弩 / Zhuge Crossbow | 古代文明 | 穿刺/物理 | 巨箭，3秒上弦 |
| 06 | 震天雷 / Thunder Mine | 古代文明 | 爆炸/破片 | 地雷，一次性 |
| 07 | 炼丹炉 / Alchemy Furnace | 古代文明 | 火焰/毒素/随机 | 附魔/药剂，消耗材料 |
| 08 | 神火飞鸦 / Fire Crow | 古代文明 | 穿刺+火药/火焰+爆炸 | 双形态切换，12秒CD |
| 09 | 脉冲塔 / Pulse Tower | 科幻未来 | 能量 | 电弧连锁，3秒充能 |
| 10 | 重力塔 / Gravity Tower | 科幻未来 | 控制/碾压 | 重力场，8秒冷却 |
| 11 | 传送门 / Portal | 科幻未来 | 无（位移） | 成对部署，空间传送 |
| 12 | 时间塔 / Time Tower | 科幻未来 | 时间/因果 | 时间波，3模式切换 |
| 13 | 激光塔 / Laser Tower | 科幻未来 | 热能/光辐射 | 持续照射，递增伤害 |
| — | 基地塔 / Base Tower | 跨时代 | 多种 | 三时代形态，可升级 |

---

## 3. 第一时代 · 石器时代 · 蒙昧 | Era 1: Stone Age

### 3.1 基础塔01：投石索 / Tower 01: Slingshot

**基础造型 / Base Appearance**
一根兽皮绳索，绑在两棵被压弯的幼树之间，一旁堆着鹅卵石。
A hide rope tied between two bent saplings, with a pile of pebbles nearby.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 甩出石头，直线弹道 / Throws stone, linear trajectory |
| 命中率 / Hit Rate | 70% |
| 伤害类型 / Damage Type | 钝击 / Blunt |
| 弹药限制 / Ammo Limit | 12 粒石头 / 12 stones |
| 特殊机制 / Special | 未命中时石头滚到地图存留5秒，敌人踩到滑倒（眩晕1秒）|

**升级路径A：精准投掷 / Upgrade Path A: Precision Throw**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 投石索换成更长的兽筋 | 命中率提升至85% |
| 2级 | 更大的石头 | 伤害+50%，射程+20% |

**升级路径B：连环石雨 / Upgrade Path B: Stone Rain**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 石头装在漏斗中 | 攻速+30% |
| 2级·专精 | 三根兽皮索绑在一起，连发模式 | 每秒倾泻5颗小石头（散射，伤害分散）|

---

### 3.2 基础塔02：骨矛阵 / Tower 02: Bone Spear Trap

**基础造型 / Base Appearance**
几根尖锐的猛犸象肋骨斜插在地面，露出地面半人高。骨头上有干涸的血迹。
Sharp mammoth rib bones thrust diagonally into the ground, half a person tall. Dried blood on the bones.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 刺痛 / Stab |
| 伤害类型 / Damage Type | 穿刺 / Pierce |
| 特殊机制 / Special | 布置后需3秒"伪装"时间（敌人不触发）；用三次后自动失效 |

**升级路径A：陷阱大师 / Upgrade Path A: Trap Master**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 骨矛上涂抹黑色油脂 | 被刺中的敌人减速20% |
| 2级 | 油脂换成蜂蜜，吸引蚂蚁 | 攻击附带流血效果（3秒持续伤害）|
| 3级·专精 | 敌人经过时触发 | 骨矛从地下斜刺而出 |

**升级路径B：工程改良 / Upgrade Path B: Engineering Improvement**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 增加木制触发机关 | 可手动引爆（消耗资源）|
| 2级 | 三根骨矛并联，地底有藤蔓连接 | 触发时三根连爆，范围伤害，可破甲 |

---

### 3.3 基础塔03：图腾 / Tower 03: Totem

**基础造型 / Base Appearance**
一根粗壮的树干，上面用赭石颜料画着扭曲的兽脸。树干底部堆着头骨和野果。
A thick tree trunk painted with distorted beast faces in ochre. Skulls and wild fruits piled at the base.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 不直接攻击，每隔7秒发出一次"图腾怒吼" / No direct attack, emits "Totem Roar" every 7s |
| 伤害类型 / Damage Type | 精神/恐惧 / Mental/Fear |
| 特殊机制 / Special | 怒吼让范围内敌人"敬畏"，眩晕3秒；靠近图腾的友方单位攻击欲望上升 |

**升级路径A：嗜血图腾 / Upgrade Path A: Bloodthirst Totem**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 图腾上挂了一串兽牙 | 怒吼不再眩晕敌人，改为激怒己方（范围内塔攻速+20%）|
| 2级 | 兽牙换成还在滴血的心脏 | 己方击杀敌人时，图腾获得1层"血怒"（攒满5层，下一次怒吼附带范围伤害）|

**升级路径B：先祖庇佑 / Upgrade Path B: Ancestral Blessing**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 图腾顶部多了一个骷髅头 | 怒吼时，敌人眩晕5秒 |
| 2级 | 骷髅头眼眶里出现幽蓝火焰 | 被照到的敌人被魅惑，自相残杀 |
| 3级·专精 | 火焰从蓝色变成白色，图腾微微颤抖 | 怒吼会召唤一个先祖虚影（持续10秒，自动攻击敌人）|

---

### 3.4 基础塔04：火堆 / Tower 04: Bonfire

**基础造型 / Base Appearance**
一圈石头围成的火塘，中央篝火燃烧。
A ring of stones forming a fire pit, with a central bonfire burning.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 敌人靠近时火焰蹿高灼烧 / Flames surge to burn nearby enemies |
| 伤害类型 / Damage Type | 火焰 / Fire |
| 特殊机制 / Special | 不需要人操作，但消耗"柴火"资源。耗尽后熄灭，重燃消耗双倍 |

**升级路径A：不灭之火 / Upgrade Path A: Eternal Flame**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 火焰颜色变蓝 | 伤害+40%，且敌人被烧后会点燃周围其他敌人 |
| 2级·神力 | 火焰彻底变成白色，没有烟 | 不再消耗柴火，自动燃烧 |

**升级路径B：火焰掌控 / Upgrade Path B: Flame Control**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 驯服一个火精灵 | 可主动喷火（消耗柴火+50%，射程+100%，持续2秒）|
| 2级 | 学会了用油脂 | 喷出的火焰落地后燃烧5秒，地面烫脚 |
| 3级·专精 | 火焰可以短暂成形 | 喷出的火焰凝聚成火鸟，追踪最远的敌人（一次性，消耗+50%）|

---

## 4. 第二时代 · 古代文明 · 理性 | Era 2: Ancient Civilization

### 4.1 基础塔05：诸葛连弩 / Tower 05: Zhuge Crossbow

**基础造型 / Base Appearance**
木质炮架，粗壮的弓臂，绞盘和绳索。
Wooden frame, thick bow arms, winch and ropes.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 发射巨箭 / Launches giant bolts |
| 伤害类型 / Damage Type | 穿刺/物理 / Pierce/Physical |
| 特殊机制 / Special | 需要3秒上弦时间，可提前手动上弦（消耗金属）|

**升级路径A：穿甲 / Upgrade Path A: Armor-Piercing**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 箭头换成精钢锻造 | 对路径上8格以内所有敌人造成伤害 |
| 2级 | 箭头带倒钩 | 穿透后留在敌人身上，造成持续伤害 |
| 3级·专精 | 箭杆上刻满符文 | 对同一目标命中第三次必暴击且穿透+1 |

**升级路径B：散射改造 / Upgrade Path B: Spread Shot**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 弓臂加宽，可搭三支箭 | 同时发射三支箭，扇形散射 |
| 2级 | 箭上加火 | 造成范围伤害 |
| 3级·专精 | 加装连发装置 | 连续发射5次（每次伤害降低），期间可移动炮口 |

---

### 4.2 基础塔06：震天雷 / Tower 06: Thunder Mine

**基础造型 / Base Appearance**
生铁铸造的球形地雷，表面有凸起的疙瘩。顶部有引信孔，埋在土里，只露引信，引信连着绊索。
Cast iron spherical mine with bumps on the surface. Buried with only the fuse exposed, connected to tripwire.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 敌人靠近绊到引线引爆，喷射铁片 / Tripwire detonation, shrapnel burst |
| 伤害类型 / Damage Type | 爆炸/破片 / Explosive/Shrapnel |
| 特殊机制 / Special | 可埋设多个（每个消耗金属）；敌人有概率发现并拆除；一次性 |

**升级路径A：连珠雷 / Upgrade Path A: Chain Mines**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 单个地雷变大 | 爆炸半径+70% |
| 2级 | 引线可并联多个地雷 | 一个绊索可引爆多个地雷（连环爆）|
| 3级·专精 | — | 主雷引爆后喷出多个小雷（二次爆炸）|

---

### 4.3 基础塔07：炼丹炉 / Tower 07: Alchemy Furnace

**基础造型 / Base Appearance**
石砌圆塔，顶部有巨大的铜炉，底部有炭火加热，炉中冒出各色烟雾。
Stone tower with a large copper furnace on top, charcoal heating below, multicolored smoke rising.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 给附近建筑附魔 / Enchants nearby structures |
| 伤害类型 / Damage Type | 火焰/毒素/随机（取决于配方）/ Fire/Poison/Random (recipe-dependent) |
| 特殊机制 / Special | 需消耗"材料"资源制作药剂；不消耗时只倾倒普通沸水（伤害极低）|

**升级路径A：毒药学 / Upgrade Path A: Toxicology**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 铜釜变成绿色 | 解锁"剧毒配方"：给周围箭、石附魔，持续伤害5秒 |
| 2级 | 冒出绿色浓烟 | 毒液蒸发成毒雾（范围效果，破隐身）|
| 3级·专精 | 铜釜上爬满藤蔓 | 解锁"枯萎配方"：敌人中毒期间无法被治疗/强化 |

**升级路径B：爆炸学 / Upgrade Path B: Explosives**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 铜釜变厚，加了铁盖 | 解锁"炸药配方"：范围爆炸+击退（3次使用，每次伤害+40%）|
| 2级·专精 | 塔顶出现避雷针 | 解锁"硝化配方"：周围箭、石命中连锁爆炸 |

---

### 4.4 基础塔08：神火飞鸦 / Tower 08: Fire Crow

**基础造型 / Base Appearance**
青砖高台，台上有两名神机营士兵。一人端着细长鸟铳，跪姿瞄准远方；另一人蹲在地上，摆弄一只竹篾扎成的乌鸦——乌鸦腹部鼓起，内填火药，体外涂着防潮的桐油。
Blueish-brick platform with two soldiers. One kneels aiming a musket; the other handles a bamboo crow packed with gunpowder and coated in tung oil.

**双形态切换（CD 12秒）/ Dual-Mode Switch (12s CD)**

| 形态 / Mode | 弹药 / Ammo | 射程 / Range | 攻速 / Rate | 伤害类型 / Damage | 特殊 / Special |
|---|---|---|---|---|---|
| 火枪 / Musket | 单发铅弹 | 极远（半径22格）| 5秒/发 | 穿刺+火药 | 高精度，高单体伤害 |
| 飞鸦 / Fire Crow | 火药乌鸦 | 中远（可越障碍）| 8秒/发 | 火焰+爆炸 | 抛物线，中等爆炸半径，留下燃烧区域4秒 |

**升级路径A：连环合击 / Upgrade Path A: Combo Strike**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 高台上多了个简易转盘 | 切换时间缩短至4秒 |
| 2级 | 塔上悬旗，上写"神机" | 切换后第一次攻击必暴击 |
| 3级·专精 | — | 解锁"连环合击"：每20秒一次机会，两种攻击同时命中同一目标触发二次爆炸 |

**升级路径B：飞鸦载弹 / Upgrade Path B: Crow Payload**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 乌鸦腹部多了个小弹仓 | 飞行途中洒落铅弹（对路径上敌人造成穿刺伤害）|
| 2级·专精 | — | 乌鸦落地爆炸后分裂成三只小火鸦，各自散射自爆 |

---

## 5. 第三时代 · 科幻未来 · 星辰 | Era 3: Sci-Fi Future

### 5.1 基础塔09：脉冲塔 / Tower 09: Pulse Tower

**基础造型 / Base Appearance**
纯白基底，上空悬浮纯白塔尖（针状），中间一根蓝色琴弦发光。
Pure white base with a floating white needle-tip above, a glowing blue string in between.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 发射电弧，瞬间命中，可连锁跳跃至附近敌人 / Arc discharge, instant hit, chains to nearby enemies |
| 伤害类型 / Damage Type | 能量 / Energy |
| 特殊机制 / Special | 每次攻击后需3秒"充能"，期间无法攻击 |

**升级路径A：连锁强化 / Upgrade Path A: Chain Enhancement**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 能量核心变亮 | 连锁跳跃+1次 |
| 2级 | 塔身出现更多导流槽 | 跳跃不衰减伤害 |
| 3级·专精 | 核心变成金色 | 跳跃时敌人被眩晕 |

**升级路径B：蓄能爆发 / Upgrade Path B: Charged Burst**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | — | 可手动蓄力（充能越久，伤害越高）|
| 2级·专精 | 核心可分离 | 核心飞出，直线穿透所有敌人（消耗蓄力，CD 20秒），然后返回 |

---

### 5.2 基础塔10：重力塔 / Tower 10: Gravity Tower

**基础造型 / Base Appearance**
倒锥形金属结构，底面悬浮着一个黑色球体。周围空气微微扭曲，光线经过时发生弯曲。
Inverted cone metal structure with a floating black sphere at the base. Air distorts around it, bending light.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 创造重力场，区域内敌人被拉向塔中心 / Creates gravity field, pulls enemies toward center |
| 伤害类型 / Damage Type | 控制/碾压 / Control/Crush |
| 特殊机制 / Special | 重力场持续3秒，冷却8秒。敌人越靠近中心，移动速度越慢 |

**升级路径A：引力坍塌 / Upgrade Path A: Gravitational Collapse**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 黑色球体变大 | 重力场范围+30% |
| 2级 | 球体表面出现裂纹 | 重力场中心敌人每秒受碾压伤害（CD 10秒触发，四次后不升级自动损坏，每次伤害-20%）|
| 3级·专精 | 球体偶尔闪烁星光 | 可主动引爆重力场：范围伤害+击飞（两次后损毁，最后一次全图伤害）|

**升级路径B：反重力 / Upgrade Path B: Anti-Gravity**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 塔身倒置 | 重力场变反重力：敌人飘起（无法攻击/移动，半径7格）|
| 2级·专精 | 塔开始缓缓自转 | 重力场可移动（拖拽敌人），进行黑洞吞噬 |

---

### 5.3 基础塔11：传送门 / Tower 11: Portal

**基础造型 / Base Appearance**
紫色环形空洞，门内是一片扭曲的虚空，偶尔闪过星光。
Purple ring-shaped void, twisted emptiness within, occasional starlight flashes.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 成对出现，敌人进A门从B门出来 / Deployed in pairs — enemies enter A, exit B |
| 伤害类型 / Damage Type | 无（位移）/ None (displacement) |
| 特殊机制 / Special | 需两座塔同时存在；敌人穿过时眩晕1秒（空间迷失）|

**升级路径A：空间切割 / Upgrade Path A: Space Cut**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 门框上增加锋利边缘 | 敌人穿过时受切割伤害 |
| 2级 | 门内出现旋转刀片 | 伤害+10%，可触发暴击 |

**升级路径B：维度折叠 / Upgrade Path B: Dimensional Fold**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | — | 敌人随机从一扇门出现 |
| 2级 | — | 敌人出传送门时被拖回去，然后逆行 |
| 3级·专精 | 门内变成红色 | 五个敌人进入后自动触发坍缩，触发重力塔进行黑洞吞噬 |

---

### 5.4 基础塔12：时间塔 / Tower 12: Time Tower

**基础造型 / Base Appearance**
面朝上的钟表盘投影，表盘没有指针，只有三种铭文（对应三级）随机亮起。
Upward-facing clock face projection, no hands, three inscription types light up randomly.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 发射时间波，范围内敌人动作变慢/变快/倒退 / Time wave: slow/accelerate/reverse enemies |
| 伤害类型 / Damage Type | 时间/因果 / Temporal/Causal |
| 基础时间 / Base Duration | 3秒 / 3s |
| 冷却 / Cooldown | 4秒 / 4s |
| 特殊机制 / Special | 三种模式：减速（50%速度）、加速（200%速度，敌人快速穿过防线）、倒退（回到3秒前位置）；**我方设施同样受控** |

**升级路径A：时间断裂 / Upgrade Path A: Time Fracture**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | — | 持续时间+2秒 |
| 2级 | 部分齿轮镀金 | 解锁"时间冻结"：范围内敌人定身2秒（但冷却翻倍）|
| 3级·专精 | 钟表盘开始发光 | 解锁"时间裂隙"：敌人进入后随机出现在过去/未来的某个位置（混乱）|

**升级路径B：因果律武 / Upgrade Path B: Causal Weaponry**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 表盘上出现"因果线" | 可标记一个敌人，3秒后它受到的所有伤害"回溯"再打一次 |
| 2级 | 因果缠绕 | 标记可同时作用于路径上敌人 |

---

### 5.5 基础塔13：激光塔 / Tower 13: Laser Tower

**基础造型 / Base Appearance**
纯白色的流线型塔身，表面覆盖着六边形散热鳞片。塔顶是一个可360度旋转的球形激光发射器，球体正面是深红色的"激光窗"。发射时，一道极细的深红色光束从晶窗射出。
Pure white streamlined tower body covered in hexagonal heat-dissipation scales. 360-degree spherical laser emitter on top with a deep red "laser window". Fires an ultra-thin deep red beam.

| 属性 / Attribute | 数值 / Value |
|---|---|
| 攻击方式 / Attack | 持续照射单一目标，每秒递增伤害 / Continuous beam on single target, escalating damage |
| 射程 / Range | 远（全图80%）/ Long (80% of map) |
| 伤害类型 / Damage Type | 热能/光辐射 / Thermal/Radiation |
| 递增机制 / Escalation | 对同一目标持续照射，每2秒伤害+10%，最多叠加5层 |
| 过热机制 / Overheat | 连续照射8秒后需冷却5秒（否则过热自动停机）|

**升级路径A：光储能 / Upgrade Path A: Light Storage**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 六边形鳞片部分替换为储能器 | 可储存2发，但持续时间缩短为4秒 |
| 2级 | 储能器表面有能量流动 | 储存的攻击可一次性无CD打出 |
| 3级·专精 | 储能器发光 | 消耗所有储存，打出一发超级脉冲，无递增伤害 |

**升级路径B：飞秒激光 / Upgrade Path B: Femtosecond Laser**

| 等级 / Level | 改造 / Change | 效果 / Effect |
|---|---|---|
| 1级 | 发射孔周围出现一圈光晕 | 攻速1.5秒/发 |
| 2级 | 光晕变成双环 | 可同时锁定两个弱点 |
| 3级·专精 | 圆柱表面出现流动光纹 | 解锁"切割链"：光束自动跳转至最近敌人（一次战斗最多跳转3次）|

---

## 6. 跨时代通用 · 基地塔 | Cross-Era: Base Tower

> 基地塔就是玩家要保护的目标。敌人摧毁它，游戏结束。但它也可以做点什么。
> The base tower IS the objective. If enemies destroy it, game over. But it can also fight back...

### 基础造型（三时代形态）| Base Appearance (3 Era Forms)

| 时代 / Era | 造型 / Form |
|---|---|
| 原始时代 / Stone Age | 一座火神像，以水绕之 / Fire deity statue surrounded by water |
| 古代文明 / Ancient | 一座中式机关亭，四周有城墙 / Chinese mechanical pavilion with surrounding walls |
| 科幻未来 / Sci-Fi | 悬浮的能量球，周围环绕着防御力场 / Floating energy sphere with defense force field |

### 升级路径：原始路线 / Upgrade Path: Primitive Route

| 等级 / Level | 效果 / Effect |
|---|---|
| 1级 | 生产石料、木材速度+50% |
| 2级 | 发动洪水，减速敌人70% |
| 3级 | 火神发出战吼，周围友方塔攻速提升，且自带火攻 |

### 升级路径：古代路线 / Upgrade Path: Ancient Route

| 等级 / Level | 效果 / Effect |
|---|---|
| 1级 | 每45秒可指定一座建筑，升至想要的形态 |
| 2级 | 机关塔可以1格/2秒进行移动 |
| 3级 | 机关塔前出现三个陷阱，可抵挡3次入侵 |

### 升级路径：未来路线 / Upgrade Path: Future Route

| 等级 / Level | 效果 / Effect |
|---|---|
| 1级 | 能量球变亮，核心自动攻击靠近的敌人（弱）|
| 2级 | 力场变成彩色，核心可开启"护盾"（无敌5秒，冷却60秒）|
| 3级 | 能量球周围出现卫星，核心可释放卫星（自动攻击全图，伤害低）|

---

## 7. 敌人配置（待补充）| Enemy Configuration (Pending)

> **CIO 说明**：v1.0 未包含敌人详细设计。以下为原型参考值，待机制设计者补充。
> **CIO Note**: v1.0 does not include detailed enemy design. Below are prototype reference values, pending designer input.

### 7.1 原型敌人（参考）| Prototype Enemy (Reference)

| 属性 / Attribute | 数值 / Value | 说明 / Notes |
|---|---|---|
| 名称 / Name | 史前猛兽 / Prehistoric Beast | |
| 血量 / HP | 100 | |
| 移动速度 / Speed | 60 像素/秒 / pixels per second | |
| 击杀奖励 / Kill Reward | 10 金 / gold | |

### 7.2 待补充事项 | Pending Items

- [ ] 三时代各有哪些敌人种类？
- [ ] 敌人是否有护甲/魔抗/特殊能力（如隐身、飞行）？
- [ ] BOSS 设计（如有）

---

## 8. 波次与核心数值（原型参考）| Waves & Core Values (Prototype Reference)

> 以下数值来自原型代码逆向分析，仅作参考。机制设计者须根据 13 炮塔体系重新设计。
> Values from prototype code analysis, reference only. Designer must redesign for 13-tower system.

### 8.1 核心数值 | Core Values

| 参数 / Parameter | 原型值 / Prototype | 确认值 / Confirmed | 说明 / Notes |
|---|---|---|---|
| 初始金钱 / Starting Gold | 150 | ❓待确认 | 需适配 13 炮塔体系 |
| 基地初始血量 / Base HP | 10 | ❓待确认 | |
| 波次数量 / Total Waves | 3 | ❓待确认 | 两张地图是否波次数不同？ |

### 8.2 波次配置 | Wave Configuration

| 波次 / Wave | 敌人数量 / Enemy Count | 生成间隔 / Spawn Interval | 波前等待 / Pre-wave Delay |
|---|---|---|---|
| 第1波 / Wave 1 | 5 | 2.0 秒 / s | 3.0 秒 / s（首波）|
| 第2波 / Wave 2 | 10 | 1.5 秒 / s | 5.0 秒 / s |
| 第3波 / Wave 3 | 15 | 1.0 秒 / s | 5.0 秒 / s |

---

## 9. 路径与地图 | Path & Map

| 属性 / Attribute | 数值 / Value | 说明 / Notes |
|---|---|---|
| 关卡数量 / Map Count | 目前已完成 2 张，后续持续扩展 | 🟡 进行中 |
| 网格尺寸 / Grid Size | 12 行 × 16 列 | |
| 格子大小 / Cell Size | 64 × 64 像素 | 已锁定 |
| 路径形状 / Path Shape | S 型（原型）| 两张地图路径待设计者确认 |
| 路点数量 / Waypoint Count | 5（原型）| 包含起点和终点 |

---

## 10. 资源系统（待细化）| Resource System (Pending Detail)

> **CIO 说明**：多座炮塔涉及资源消耗机制（柴火、金属、材料），需设计者明确资源体系。
> **CIO Note**: Multiple towers consume resources (firewood, metal, materials). Designer must define the resource system.

| 资源类型 / Resource | 涉及炮塔 / Related Towers | 待确认事项 / Pending |
|---|---|---|
| 金钱 / Gold | 所有炮塔建造 | 建造费用待定 |
| 柴火 / Firewood | 火堆（04）| 获取方式？初始数量？|
| 金属 / Metal | 诸葛连弩（05）、震天雷（06）| 获取方式？每单位消耗？|
| 材料 / Materials | 炼丹炉（07）| 配方消耗量？|

---

## 11. CIO 翻译为代码常量（参考）| CIO Translation to Code Constants (Reference)

> 以下由 CIO 根据原型数值生成。13 炮塔体系的完整常量将在数值锁定后生成。
> Generated by CIO based on prototype values. Full constants for 13-tower system generated after value lock.

```c
/* 核心数值 / Core values — 原型参考，待锁定 */
#define INITIAL_GOLD        150
#define BASE_INITIAL_HP     10
#define TOTAL_WAVES         3
#define TOTAL_MAPS          2   /* 当前已完成，后续扩展 / completed so far, more planned */

/* 地图 / Map */
#define CELL_SIZE           64
#define MAP_ROWS            12
#define MAP_COLS            16
#define MAX_WAYPOINTS       20

/* 炮塔种类 / Tower types */
#define TOWER_TYPE_COUNT    13
#define BASE_TOWER_COUNT    1

/* 时代 / Eras */
#define ERA_STONE_AGE       0
#define ERA_ANCIENT         1
#define ERA_SCIFI           2
#define ERA_COUNT           3

/* 原型炮塔参考值 / Prototype tower reference — 待按13种重新定义 */
#define TOWER_COST          50
#define TOWER_DAMAGE        35
#define TOWER_RANGE         150
#define TOWER_COOLDOWN_SEC  1.0f

/* 原型敌人参考值 / Prototype enemy reference */
#define ENEMY_HP            100
#define ENEMY_SPEED         60.0f    /* pixels per second */
#define ENEMY_KILL_REWARD   10

/* 波次 / Wave */
#define WAVE1_ENEMY_COUNT   5
#define WAVE1_SPAWN_INTERVAL 2.0f
#define WAVE2_ENEMY_COUNT   10
#define WAVE2_SPAWN_INTERVAL 1.5f
#define WAVE3_ENEMY_COUNT   15
#define WAVE3_SPAWN_INTERVAL 1.0f
#define WAVE_DELAY_SECONDS  5.0f
```

---

## 12. 待确认事项汇总 | Pending Items Summary

| 事项 / Item | 优先级 / Priority | 截止 / Deadline | 负责人 / Owner |
|---|---|---|---|
| 确认/修改核心数值（金钱、血量、波次）| 🔴 高 | M2 结束（3.20）| 胡昊泽 |
| 补充敌人种类设计 | 🔴 高 | M2 结束（3.20）| 胡昊泽 |
| 定义资源系统（柴火、金属、材料的获取与消耗）| 🟡 中 | M2 结束（3.20）| 胡昊泽 |
| 每座炮塔的建造费用 | 🟡 中 | M2 结束（3.20）| 胡昊泽 |
| 两张地图的路径设计（路点/形状差异）| 🟡 中 | M3 期间 | 胡昊泽 + 戴博远 |
| 难度曲线调整 | 🟡 中 | M3 期间 | 胡昊泽 |

---

*维护者：胡昊泽 / Designer（模板与审查由 CIO-TD 符雅翔创建并更新）*
*Maintained by: Hu Haoze / Designer (template and review by CIO-TD Fu Yaxiang)*
