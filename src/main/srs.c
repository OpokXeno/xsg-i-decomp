#include "common.h"
#include "shared.h"
#include "main/srs.h"
#include "srs.h"
#include "main/sef_print.h"
#include "main/sm_initilize.h"
#include "main/xgl_cd.h"

/* Table sizes. */
#define SRS_BOSS_WAIT_ROWS 36        /* _srsChrEfTbl3 */
#define SRS_WEAPON_ROWS 45           /* _weaponTbl */
#define SRS_EFFECT2_INDEX_ROWS 117   /* _srsEffect2IndexTbl */
#define SRS_EFFECT2_INDEX_FIRST 601  /* effect number of its first row */
#define SRS_CHR_EF_ROWS 12           /* _srsChrEfTbl */
#define SRS_CHR_EF2_ROWS 161         /* _srsChrEfTbl2 */
#define SRS_COMBO_DATA_COUNT 195     /* _loadComboData */
#define SRS_PATH_LENGTH 256          /* resolved file path and view path */
#define SRS_NAME_LENGTH 128          /* "esd/NAME.esd" style file name buffers */

/* The effect-name (.esd) table: one 16-byte name per effect number. */
#define SRS_EFFECT_NO_COUNT 0xC00    /* effect numbers are below this */
#define SRS_ESD_ENTRY_SIZE 0x10
#define SRS_ESD_TABLE_SIZE 0xC800

/* srsLoadMode values. */
#define SRS_LOAD_MODE_CD 0
#define SRS_LOAD_MODE_HOST 1
#define SRS_LOAD_MODE_USB 2
#define SRS_USB_PREFIX_LENGTH 5      /* "usb0:" */

/* sceOpen flags and sceLseek origins used by the host loader. */
#define SRS_OPEN_RDONLY 1
#define SRS_SEEK_SET 0
#define SRS_SEEK_END 2

/* xglCdReadFile completion callback: the read finished. */
#define SRS_CD_READ_COMPLETE 4

/* Memory images (bytes). */
#define SRS_COMMON_IMAGE_SIZE 0x25800   /* default.esp */
#define SRS_CF_IMAGE_SIZE 0x3E000       /* cf_def.esp */
#define SRS_WEAPON_IMAGE_SIZE 0x12800   /* one character's weapon effects */
#define SRS_WEAPON_SLOT_STRIDE 0x6000   /* start of each weapon effect in that image */
#define SRS_COMBO_IMAGE_SIZE 0x12000    /* one enemy combo */

/* svAddImageMapper "height" (the fourth argument) registered for each resource. */
#define SRS_MAPPER_HEIGHT_IMAGE 3000
#define SRS_MAPPER_HEIGHT_EFFECT_IMAGE 3100
#define SRS_MAPPER_HEIGHT_WEAPON 2000
#define SRS_MAPPER_HEIGHT_COMBO 2100
#define SRS_MAPPER_HEIGHT_BATTLE_IMAGE 3400
#define SRS_MAPPER_HEIGHT_CF_IMAGE 3500
#define SRS_MAPPER_HEIGHT_EFFECT 3600

/*
 * Character numbers (battle party and enemy slots): 17..32 own weapon effect
 * data, boss numbers start at 151 (the effect numbers of _srsChrEfTbl3).
 */
#define SRS_CHAR_WEAPON_FIRST 17
#define SRS_CHAR_WEAPON_END 33
#define SRS_CHAR_BOSS_FIRST 151

/*
 * Effect number ranges (first number and count). Each range resolves to one
 * of the SrsResourceType categories in srsAnalyzeEftNo/srsGetEffectType.
 */
#define SRS_EFFECT_COMMON_FIRST 2600       /* default.esp */
#define SRS_EFFECT_COMMON_COUNT 62
#define SRS_EFFECT_CF_COMMON_FIRST 600     /* cf_def.esp */
#define SRS_EFFECT_CF_COMMON_COUNT 400
#define SRS_EFFECT_CF_FIRST 1000           /* loaded cf effects */
#define SRS_EFFECT_CF_COUNT 1000
#define SRS_EFFECT_BATTLE_FIRST 2000       /* battle image */
#define SRS_EFFECT_BATTLE_COUNT 219
#define SRS_EFFECT_COMBO_FIRST 240         /* enemy combo data */
#define SRS_EFFECT_COMBO_COUNT 157
#define SRS_EFFECT_BATTLE2_FIRST 2300      /* battle image */
#define SRS_EFFECT_BATTLE2_COUNT 182
#define SRS_EFFECT_WEAPON_FIRST 2800       /* weapon effects of character 17 */
#define SRS_EFFECT_WEAPON_COUNT 200
/* Weapon effects 2900..2998 share the data of effects 100 lower. */
#define SRS_EFFECT_WEAPON_ALT_FIRST 2900
#define SRS_EFFECT_WEAPON_ALT_COUNT 99
#define SRS_EFFECT_WEAPON_ALT_OFFSET 100

/*
 * One _srsChrEfTbl3 row (6 bytes; the 0xD8-byte symbol holds 36 rows).
 * srsGetBossWaitEftNo scans effectNo and returns the matching bossWaitEftNo.
 */
typedef struct SrsChrEfTbl3Entry {
    short effectNo;
    short bossWaitEftNo;
    unsigned char unmodeled_04[2];
} SrsChrEfTbl3Entry;

/*
 * One _weaponTbl row (8 bytes; the 0x168-byte symbol holds 45 rows).
 * srsEftNo2WeaponID reads weaponID at the row start; srsEftNoWeaponEffectID
 * reads weaponEffectID right after it.
 */
typedef struct WeaponTblEntry {
    short weaponID;
    short weaponEffectID;
    short weaponEffectID2;
    short weaponEffectID3;
} WeaponTblEntry;

/*
 * One _srsChrEfTbl row (18 bytes; the 0xD8-byte symbol holds 12 rows): a
 * character number followed by four inclusive effect-number ranges.  The
 * first and fourth ranges resolve to the character's combo data, the second
 * and third only to the character number.
 */
typedef struct SrsChrEfTblEntry {
    short charNo;
    short comboMin;
    short comboMax;
    short firstMin;
    short firstMax;
    short secondMin;
    short secondMax;
    short alternateMin;
    short alternateMax;
} SrsChrEfTblEntry;

/*
 * One _srsChrEfTbl2 row (6 bytes; the 0x3C6-byte symbol holds 161 rows): a
 * character number and the inclusive effect-number range it owns.
 */
typedef struct SrsChrEfTbl2Entry {
    short charNo;
    short effectMin;
    short effectMax;
} SrsChrEfTbl2Entry;

typedef struct SrsBattlePrm {
    short charNo[3];
    short enemyNo[3];
    unsigned char unmodeled_0C[6];
    short weaponEffect[3][3];
    short charReload[3];
    short enemyReload[3];
    unsigned char unmodeled_30[4];
} SrsBattlePrm;

extern char sefBoss033Name[];
extern char sefBoss032Name[];
extern char sefBoss031Name[];
extern char sefBoss030Name[];
extern char sefBoss029Name[];
extern char sefBoss028Name[];
extern char sefBoss027Name[];
extern char sefBoss026Name[];
extern char sefBoss025Name[];
extern char sefBoss024Name[];
extern char sefBoss023Name[];
extern char sefBoss022Name[];
extern char sefBoss021Name[];
extern char sefBoss020Name[];
extern char sefBoss019Name[];
extern char sefBoss018Name[];
extern char sefBoss017Name[];
extern char sefBoss016Name[];
extern char sefBoss015Name[];
extern char sefBoss014Name[];
extern char sefBoss013Name[];
extern char sefBoss012Name[];
extern char sefBoss011Name[];
extern char sefBoss010Name[];
extern char sefBoss009Name[];
extern char sefBoss008Name[];
extern char sefBoss007Name[];
extern char sefBoss006Name[];
extern char sefBoss005Name[];
extern char sefBoss004Name[];
extern char sefBoss003Name[];
extern char sefBoss002Name[];
extern char sefBoss001Name[];
extern char sefUtso007Name[];
extern char sefUtso006Name[];
extern char sefUtso005Name[];
extern char sefUtso004Name[];
extern char sefUtso003Name[];
extern char sefUtso002Name[];
extern char sefUtso001Name[];
extern char sefUtro007Name[];
extern char sefUtro006Name[];
extern char sefUtro005Name[];
extern char sefUtro004Name[];
extern char sefUtro003Name[];
extern char sefUtro002Name[];
extern char sefUtro001Name[];
extern char sefUtre006Name[];
extern char sefUtre005Name[];
extern char sefUtre004Name[];
extern char sefUtre003Name[];
extern char sefUtre002Name[];
extern char sefUtre001Name[];
extern char sefUtma014Name[];
extern char sefUtma013Name[];
extern char sefUtma012Name[];
extern char sefUtma011Name[];
extern char sefUtma010Name[];
extern char sefUtma009Name[];
extern char sefUtma008Name[];
extern char sefUtma007Name[];
extern char sefUtma006Name[];
extern char sefUtma005Name[];
extern char sefUtma004Name[];
extern char sefUtma003Name[];
extern char sefUtma002Name[];
extern char sefUtma001Name[];
extern char sefFeso004Name[];
extern char sefFeso003Name[];
extern char sefFeso002Name[];
extern char sefFeso001Name[];
extern char sefFero001Name[];
extern char sefFere003Name[];
extern char sefFere002Name[];
extern char sefFere001Name[];
extern char sefFema006Name[];
extern char sefFema005Name[];
extern char sefFema004Name[];
extern char sefFema003Name[];
extern char sefFema002Name[];
extern char sefFema001Name[];
extern char sefGuno114Name[];
extern char sefGuno025Name[];
extern char sefGuno024Name[];
extern char sefGuno023Name[];
extern char sefGuno022Name[];
extern char sefGuno021Name[];
extern char sefGuno020Name[];
extern char sefGuno019Name[];
extern char sefGuno018Name[];
extern char sefGuno017Name[];
extern char sefGuno016Name[];
extern char sefGuno015Name[];
extern char sefGuno014Name[];
extern char sefGuno013Name[];
extern char sefGuno012Name[];
extern char sefGuno011Name[];
extern char sefGuno010Name[];
extern char sefGuno009Name[];
extern char sefGuno008Name[];
extern char sefGuno007Name[];
extern char sefGuno006Name[];
extern char sefGuno005Name[];
extern char sefGuno004Name[];
extern char sefGuno003Name[];
extern char sefGuno002Name[];
extern char sefGuno001Name[];
extern char sefCatherinName[];
extern char cfDefaultEffectName[];
extern char defaultEffectName[];
extern char chaosEffectName[];
extern char kosmosEffectName[];
extern char shionEffectName[];
extern char shitanEffectName[];
extern char ziggyEffectName[];
extern char momoEffectName[];
extern char jrEffectName[];
extern char ceciliaEffectName[];
extern char virgilEffectName[];
extern char maryEffectName[];
extern char shelleyEffectName[];
extern char emptyEffectName[];
extern char _loadEsdData[SRS_ESD_TABLE_SIZE];
extern char *_loadComboData[SRS_COMBO_DATA_COUNT];
extern char *_loadCfCommonData;

extern char *strcpy(char *destination, const char *source);
extern int srsGetWeaponEffectIdx(int effectNo);
extern void sresDataMapping(void);
/*
 * svAddImageMapper takes the address of a chunk list; svAnalyzeChunk walks it
 * from its first entry, which is the only one these two loaders fill.
 */
extern void svAddImageMapper(int type, int width, void *chunk, int height);

extern void smFree(void *block);
extern void svDeleteImageMapper(int type);
extern int strcmp(const char *s1, const char *s2);
extern char *strcat(char *destination, const char *source);
extern int sceOpen(const char *path, int flags, ...);
extern int sceLseek(int descriptor, int offset, int whence);
extern int sceRead(int descriptor, void *buffer, int bytes);
extern int sceClose(int descriptor);
extern int func_00A103C8(const char *path, void *buffer);
extern void srsCdReadCallback(int code);
extern void *memcpy(void *destination, const void *source, unsigned int count);

/* JP: #### ＣＦエフェクトメモリが足りません!!
 * EN: #### Not enough CF effect memory!! */
static const char srsCfMemoryFullText[40] = "#### \243\303\243\306\245\250\245\325\245\247\245\257\245\310\245\341\245\342\245\352\244\254\302\255\244\352\244\336\244\273\244\363!!";

static const char srsErrorCategoryFormat[32] = "-- err category(%d) eft(%d)";

static const char srsEffectNotLoadedText[40] = "-- err : eftNo is not cf load";

static SrsChrEfTbl3Entry _srsChrEfTbl3[SRS_BOSS_WAIT_ROWS];

static char srsViewPath[SRS_PATH_LENGTH];

static WeaponTblEntry _weaponTbl[SRS_WEAPON_ROWS];

static signed char _srsEffect2IndexTbl[SRS_EFFECT2_INDEX_ROWS];

static SrsChrEfTblEntry _srsChrEfTbl[SRS_CHR_EF_ROWS];

static SrsChrEfTbl2Entry _srsChrEfTbl2[SRS_CHR_EF2_ROWS];

const void *_loadCommonData[1] = { defaultEffectName };

char defaultEffectName[8] = "default";

char *_loadCfCommonData = cfDefaultEffectName;

char cfDefaultEffectName[8] = "cf_def";

char emptyEffectName[8] = "";

char shelleyEffectName[8] = "shelley";

char maryEffectName[8] = "mary";

char virgilEffectName[8] = "virgil";

char ceciliaEffectName[8] = "cecilia";

char jrEffectName[8] = "jr";

char momoEffectName[8] = "momo";

char ziggyEffectName[8] = "ziggy";

char shitanEffectName[8] = "shitan";

char shionEffectName[8] = "shion";

char kosmosEffectName[8] = "kosmos";

char chaosEffectName[8] = "chaos";

/* Directory prefix that srsMakeFileName appends after the host and USB roots. */
static char srsViewPath[SRS_PATH_LENGTH] = {0};

/* 0x30C-byte table of 195 pointer-sized slots. */
char *_loadComboData[SRS_COMBO_DATA_COUNT] = {
    defaultEffectName, chaosEffectName, kosmosEffectName, shionEffectName, shitanEffectName,
    ziggyEffectName, momoEffectName, jrEffectName, ceciliaEffectName, sefCatherinName,
    virgilEffectName, maryEffectName, shelleyEffectName, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, sefGuno001Name, sefGuno001Name,
    sefGuno002Name, sefGuno003Name, sefGuno004Name, sefGuno005Name, sefGuno006Name,
    sefGuno007Name, sefGuno008Name, sefGuno009Name, sefGuno010Name, sefGuno011Name,
    sefGuno012Name, sefGuno013Name, sefGuno014Name, sefGuno015Name, sefGuno016Name,
    sefGuno017Name, sefGuno018Name, sefGuno019Name, sefGuno020Name, sefGuno021Name,
    sefGuno022Name, sefGuno023Name, sefGuno024Name, sefGuno025Name, sefGuno001Name,
    sefGuno002Name, sefGuno003Name, sefGuno114Name, sefFema001Name, sefFema002Name,
    sefFema003Name, sefFema004Name, sefFema005Name, sefFema006Name, sefFere001Name,
    sefFere002Name, sefFere003Name, sefFero001Name, sefFeso001Name, sefFeso002Name,
    sefFeso003Name, sefFeso004Name, sefUtma001Name, sefUtma002Name, sefUtma003Name,
    sefUtma004Name, sefUtma005Name, sefUtma006Name, sefUtma007Name, sefUtma008Name,
    sefUtma009Name, sefUtma010Name, sefUtma011Name, sefUtma012Name, sefUtma013Name,
    sefUtma014Name, sefUtre001Name, sefUtre002Name, sefUtre003Name, sefUtre004Name,
    sefUtre005Name, sefUtre006Name, sefUtro001Name, sefUtro002Name, sefUtro003Name,
    sefUtro004Name, sefUtro005Name, sefUtro006Name, sefUtro007Name, sefUtso001Name,
    sefUtso002Name, sefUtso003Name, sefUtso004Name, sefUtso005Name, sefUtso006Name,
    sefUtso007Name, sefUtma002Name, sefUtma002Name, sefUtma006Name, sefGuno006Name,
    sefFema001Name, sefFeso001Name, sefFeso002Name, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, sefFeso001Name, sefFeso001Name, sefFeso001Name, sefFeso001Name,
    sefFeso001Name, sefBoss001Name, sefBoss002Name, sefBoss003Name, sefBoss004Name,
    sefBoss005Name, sefBoss006Name, sefBoss007Name, sefBoss008Name, sefBoss009Name,
    sefBoss010Name, sefBoss011Name, sefBoss012Name, sefBoss013Name, sefBoss014Name,
    sefBoss015Name, sefBoss016Name, sefBoss017Name, sefBoss018Name, sefBoss019Name,
    sefBoss020Name, sefBoss021Name, sefBoss022Name, sefBoss023Name, sefBoss024Name,
    sefBoss025Name, sefBoss026Name, sefBoss027Name, sefBoss028Name, sefBoss029Name,
    sefBoss030Name, sefBoss031Name, sefBoss032Name, sefBoss033Name, sefBoss003Name,
    sefBoss004Name, sefBoss020Name, emptyEffectName, emptyEffectName, emptyEffectName,
    emptyEffectName, emptyEffectName, emptyEffectName, emptyEffectName, emptyEffectName,
};

static signed char _srsEffect2IndexTbl[SRS_EFFECT2_INDEX_ROWS] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3,
    4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1,
    2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3,
    4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6,
    7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
};

static SrsChrEfTblEntry _srsChrEfTbl[SRS_CHR_EF_ROWS] = {
    {1, 28, 33, 34, 41, 2030, 2041, 2509, 2512},
    {2, 15, 20, 21, 23, 2017, 2029, 2505, 2508},
    {3, 1, 6, 7, 14, 2001, 2016, 2501, 2504},
    {4, 82, 87, 88, 95, 2072, 2084, 2525, 2528},
    {5, 56, 61, 62, 68, 2054, 2061, 2517, 2520},
    {6, 42, 47, 48, 55, 2042, 2053, 2513, 2516},
    {7, 69, 74, 75, 81, 2062, 2071, 2521, 2524},
    {8, 0, 0, 0, 0, 0, 0, 0, 0},
    {9, 0, 0, 0, 0, 0, 0, 0, 0},
    {10, 96, 98, 0, 0, 0, 0, 2529, 2532},
    {11, 0, 0, 0, 0, 0, 0, 0, 0},
    {12, 0, 0, 0, 0, 0, 0, 0, 0},
};

static SrsChrEfTbl2Entry _srsChrEfTbl2[SRS_CHR_EF2_ROWS] = {
    {34, 267, 267},
    {35, 268, 269},
    {36, 270, 271},
    {37, 272, 274},
    {38, 275, 277},
    {39, 278, 280},
    {40, 281, 281},
    {41, 282, 283},
    {42, 284, 285},
    {43, 286, 288},
    {44, 289, 290},
    {45, 291, 293},
    {46, 294, 295},
    {47, 296, 297},
    {48, 298, 300},
    {49, 301, 303},
    {50, 304, 305},
    {51, 306, 307},
    {52, 308, 310},
    {53, 311, 312},
    {54, 313, 314},
    {55, 315, 316},
    {56, 317, 317},
    {57, 318, 318},
    {58, 319, 319},
    {59, 267, 267},
    {60, 268, 269},
    {61, 270, 271},
    {62, 325, 326},
    {63, 241, 242},
    {64, 243, 245},
    {65, 246, 246},
    {66, 248, 249},
    {67, 250, 251},
    {68, 252, 254},
    {69, 255, 255},
    {70, 256, 258},
    {71, 259, 261},
    {72, 262, 262},
    {73, 263, 263},
    {74, 264, 264},
    {75, 265, 265},
    {76, 266, 266},
    {77, 327, 327},
    {78, 328, 328},
    {79, 329, 330},
    {80, 331, 332},
    {81, 333, 334},
    {82, 335, 337},
    {83, 338, 339},
    {84, 340, 342},
    {85, 343, 345},
    {86, 346, 347},
    {87, 348, 349},
    {88, 350, 351},
    {89, 352, 353},
    {90, 354, 356},
    {91, 357, 357},
    {92, 358, 360},
    {93, 361, 363},
    {94, 364, 366},
    {95, 367, 369},
    {96, 370, 372},
    {97, 373, 373},
    {98, 374, 374},
    {99, 375, 375},
    {100, 376, 376},
    {101, 377, 379},
    {102, 380, 382},
    {103, 383, 384},
    {104, 385, 385},
    {105, 386, 386},
    {106, 387, 388},
    {107, 389, 389},
    {108, 390, 391},
    {109, 392, 393},
    {110, 394, 395},
    {151, 2326, 2330},
    {152, 2331, 2335},
    {153, 2311, 2315},
    {154, 2306, 2310},
    {155, 2341, 2345},
    {156, 2336, 2340},
    {157, 2301, 2305},
    {158, 2346, 2350},
    {159, 2351, 2355},
    {160, 2356, 2360},
    {161, 2361, 2365},
    {162, 2366, 2370},
    {163, 2371, 2375},
    {164, 2376, 2380},
    {165, 2381, 2385},
    {166, 2386, 2390},
    {167, 2391, 2395},
    {168, 2396, 2400},
    {169, 2401, 2405},
    {170, 2406, 2410},
    {171, 2411, 2415},
    {172, 2416, 2420},
    {173, 2426, 2430},
    {174, 2431, 2435},
    {175, 2436, 2440},
    {176, 2441, 2445},
    {177, 2446, 2450},
    {178, 2451, 2455},
    {179, 2456, 2460},
    {180, 2461, 2465},
    {181, 2466, 2470},
    {182, 2476, 2480},
    {183, 2421, 2425},
    {184, 2311, 2315},
    {185, 2306, 2310},
    {186, 2406, 2410},
    {151, 2681, 2683},
    {152, 2684, 2686},
    {153, 2687, 2689},
    {154, 2690, 2692},
    {155, 2693, 2695},
    {156, 2696, 2698},
    {157, 2699, 2701},
    {158, 2702, 2704},
    {159, 2705, 2707},
    {160, 2708, 2710},
    {161, 2711, 2713},
    {162, 2714, 2716},
    {163, 2717, 2719},
    {164, 2720, 2722},
    {165, 2723, 2725},
    {166, 2726, 2728},
    {167, 2729, 2731},
    {168, 2732, 2734},
    {169, 2735, 2737},
    {170, 2738, 2740},
    {171, 2741, 2743},
    {172, 2744, 2746},
    {173, 2747, 2749},
    {174, 2750, 2752},
    {175, 2753, 2755},
    {176, 2756, 2758},
    {177, 2759, 2761},
    {178, 2762, 2764},
    {179, 2765, 2767},
    {180, 2768, 2770},
    {181, 2771, 2773},
    {182, 2774, 2776},
    {183, 2777, 2779},
    {184, 2687, 2689},
    {185, 2690, 2692},
    {186, 2738, 2740},
    {111, 328, 328},
    {112, 328, 328},
    {113, 335, 337},
    {114, 278, 280},
    {63, 241, 242},
    {73, 263, 263},
    {74, 264, 264},
    {146, 263, 263},
    {147, 263, 263},
    {148, 263, 263},
    {149, 263, 263},
    {150, 263, 263},
};

static SrsChrEfTbl3Entry _srsChrEfTbl3[SRS_BOSS_WAIT_ROWS] = {
    {151, 2681, {0x7B, 0x0A}},
    {152, 2684, {0x7E, 0x0A}},
    {153, 2687, {0x81, 0x0A}},
    {154, 2690, {0x84, 0x0A}},
    {155, 2693, {0x87, 0x0A}},
    {156, 2696, {0x8A, 0x0A}},
    {157, 2699, {0x8D, 0x0A}},
    {158, 2702, {0x90, 0x0A}},
    {159, 2705, {0x93, 0x0A}},
    {160, 2708, {0x96, 0x0A}},
    {161, 2711, {0x99, 0x0A}},
    {162, 2714, {0x9C, 0x0A}},
    {163, 2717, {0x9F, 0x0A}},
    {164, 2720, {0xA2, 0x0A}},
    {165, 2723, {0xA5, 0x0A}},
    {166, 2726, {0xA8, 0x0A}},
    {167, 2729, {0xAB, 0x0A}},
    {168, 2732, {0xAE, 0x0A}},
    {169, 2735, {0xB1, 0x0A}},
    {170, 2738, {0xB4, 0x0A}},
    {171, 2741, {0xB7, 0x0A}},
    {172, 2744, {0xBA, 0x0A}},
    {173, 2747, {0xBD, 0x0A}},
    {174, 2750, {0xC0, 0x0A}},
    {175, 2753, {0xC3, 0x0A}},
    {176, 2756, {0xC6, 0x0A}},
    {177, 2759, {0xC9, 0x0A}},
    {178, 2762, {0xCC, 0x0A}},
    {179, 2765, {0xCF, 0x0A}},
    {180, 2768, {0xD2, 0x0A}},
    {181, 2771, {0xD5, 0x0A}},
    {182, 2774, {0xD8, 0x0A}},
    {183, 2777, {0xDB, 0x0A}},
    {184, 2687, {0x81, 0x0A}},
    {185, 2690, {0x84, 0x0A}},
    {186, 2738, {0xB4, 0x0A}},
};

static WeaponTblEntry _weaponTbl[SRS_WEAPON_ROWS] = {
    {70, 2801, 2802, 2801},
    {71, 2803, 2803, 2804},
    {72, 2805, 2806, 2807},
    {73, 2808, 2862, 2863},
    {74, 2809, 2809, 2810},
    {75, 2811, 2812, 2813},
    {76, 2814, 2864, 2865},
    {77, 2815, 2815, 2816},
    {78, 2817, 2817, 2867},
    {79, 2818, 2818, 2818},
    {80, 2819, 2819, 2820},
    {81, 2821, 2821, 2866},
    {82, 2822, 2822, 2823},
    {83, 2824, 2824, 2861},
    {84, 2825, 2825, 2825},
    {85, 2826, 2826, 2827},
    {86, 2828, 2828, 2829},
    {87, 2830, 2830, 2831},
    {88, 2832, 2832, 2832},
    {89, 2833, 2833, 2834},
    {90, 2835, 2835, 2836},
    {91, 2837, 2837, 2838},
    {112, 2837, 2837, 2838},
    {92, 2839, 2839, 2840},
    {113, 2839, 2839, 2840},
    {93, 2841, 2841, 2841},
    {94, 2842, 2842, 2842},
    {114, 2842, 2842, 2842},
    {95, 2843, 2843, 2843},
    {96, 2844, 2844, 2844},
    {97, 2845, 2845, 2845},
    {98, 2846, 2846, 2846},
    {99, 2847, 2847, 2847},
    {100, 2848, 2848, 2848},
    {101, 2849, 2849, 2849},
    {102, 2850, 2850, 2850},
    {103, 2851, 2851, 2851},
    {104, 2852, 2852, 2852},
    {105, 2853, 2853, 2853},
    {108, 2854, 2854, 2854},
    {110, 2855, 2855, 2855},
    {106, 2856, 2856, 2856},
    {107, 2857, 2857, 2857},
    {111, 2858, 2858, 2858},
    {109, 2859, 2859, 2860},
};

SrsMemRes _srsMemRes = {0};

/* 0xC800-byte effect-name table. */
char _loadEsdData[SRS_ESD_TABLE_SIZE] = {0};

/*
 * Returns the offset, in effect numbers, between an effect number and the
 * number whose .esd entry it shares (0 when it has no shared entry).
 */
int srsGetEffect2Idx(int effectNo)
{
    int idx;

    if ((unsigned int) (effectNo - SRS_EFFECT2_INDEX_FIRST) < SRS_EFFECT2_INDEX_ROWS) {
        idx = _srsEffect2IndexTbl[effectNo - SRS_EFFECT2_INDEX_FIRST];
    } else if ((unsigned int) (effectNo - SRS_EFFECT_COMMON_FIRST) < 12) {
        idx = effectNo - SRS_EFFECT_COMMON_FIRST;
    } else if ((unsigned int) (effectNo - 2612) < 9) {
        idx = effectNo - 2612;
    } else if ((unsigned int) (effectNo - 2651) < 10) {
        idx = effectNo - 2651;
    } else if ((unsigned int) (effectNo - 2626) < 4) {
        idx = effectNo - 2626;
    } else if ((unsigned int) (effectNo - 2630) < 7) {
        idx = effectNo - 2630;
    } else {
        idx = ((unsigned int) (effectNo - SRS_EFFECT_WEAPON_ALT_FIRST) < SRS_EFFECT_WEAPON_ALT_COUNT) ? SRS_EFFECT_WEAPON_ALT_OFFSET : 0;
    }
    return idx;
}

/* Returns the .esd name entry for an effect number, or 0 when it is out of range. */
char *srsGetEsdData(int index)
{
    int idx;

    if ((unsigned int) index >= SRS_EFFECT_NO_COUNT) {
        return 0;
    }
    idx = srsGetEffect2Idx(index);
    return _loadEsdData + (index - idx) * SRS_ESD_ENTRY_SIZE;
}

char *srsGetEsdData2(int effectNo)
{
    if ((unsigned int) effectNo >= SRS_EFFECT_NO_COUNT) {
        return 0;
    }
    return _loadEsdData + effectNo * SRS_ESD_ENTRY_SIZE;
}

char *sefGetEffectName(int effectNo)
{
    if ((unsigned int) effectNo >= SRS_EFFECT_NO_COUNT) {
        return 0;
    }
    return _loadEsdData + effectNo * SRS_ESD_ENTRY_SIZE;
}

/*
 * Classifies an effect number: stores its character number and SrsResourceType
 * in *charNo and *effectType. Returns the name data to load for the effect,
 * or 0 when there is none.
 */
char *srsAnalyzeEftNo(int effectNo, int *charNo, int *effectType)
{
    int i;
    char *result;
    SrsChrEfTbl2Entry *entry;
    short *rangeMax;

    *charNo = 0;
    *effectType = SRS_RES_BATTLE_IMAGE;
    if ((unsigned int) (effectNo - 1) >= SRS_EFFECT_NO_COUNT - 1) {
        return 0;
    }
    if (effectNo >= 3042) {
        *effectType = SRS_RES_BATTLE_IMAGE;
        return 0;
    }
    if ((unsigned int) (effectNo - SRS_EFFECT_COMMON_FIRST) < SRS_EFFECT_COMMON_COUNT) {
        *charNo = 0;
        *effectType = SRS_RES_IMAGE;
        return 0;
    }
    if ((unsigned int) (effectNo - SRS_EFFECT_WEAPON_FIRST) < SRS_EFFECT_WEAPON_COUNT) {
        *effectType = SRS_RES_WEAPON;
        *charNo = SRS_CHAR_WEAPON_FIRST;
        return 0;
    }
    if ((unsigned int) (effectNo - SRS_EFFECT_CF_COMMON_FIRST) < SRS_EFFECT_CF_COMMON_COUNT) {
        result = _loadCfCommonData;
        *effectType = SRS_RES_CF_IMAGE;
        return result;
    }
    if ((unsigned int) (effectNo - 2200) < 10) {
        *charNo = 2;
        *effectType = SRS_RES_BATTLE_IMAGE;
        return 0;
    }
    if ((unsigned int) (effectNo - 2210) < 8) {
        *charNo = 5;
        *effectType = SRS_RES_BATTLE_IMAGE;
        return 0;
    }
    if ((unsigned int) (effectNo - SRS_EFFECT_CF_FIRST) < SRS_EFFECT_CF_COUNT) {
        *charNo = 0;
        *effectType = SRS_RES_EFFECT;
        return 0;
    }
    if ((unsigned int) (effectNo - SRS_EFFECT_BATTLE_FIRST) < SRS_EFFECT_BATTLE_COUNT) {
        *charNo = 0;
        *effectType = SRS_RES_BATTLE_IMAGE;
        return 0;
    }
    /* Per _srsChrEfTbl row: the first and fourth ranges give the character's combo data, the second and third a battle image of that character. */
    for (i = 0; i < SRS_CHR_EF_ROWS; i++) {
        if (effectNo >= _srsChrEfTbl[i].comboMin && effectNo <= _srsChrEfTbl[i].comboMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = SRS_RES_WEAPON;
            return _loadComboData[*charNo];
        }
        if (effectNo >= _srsChrEfTbl[i].firstMin && effectNo <= _srsChrEfTbl[i].firstMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = SRS_RES_BATTLE_IMAGE;
            return 0;
        }
        if (effectNo >= _srsChrEfTbl[i].secondMin && effectNo <= _srsChrEfTbl[i].secondMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = SRS_RES_BATTLE_IMAGE;
            return 0;
        }
        if (effectNo >= _srsChrEfTbl[i].alternateMin && effectNo <= _srsChrEfTbl[i].alternateMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = SRS_RES_WEAPON;
            return _loadComboData[*charNo];
        }
    }
    entry = _srsChrEfTbl2;
    rangeMax = &entry->effectMax;
    /* Character-owned ranges from _srsChrEfTbl2. */
    for (i = 0; i < SRS_CHR_EF2_ROWS; i++, rangeMax += 3) {
        if (effectNo >= rangeMax[-1] && effectNo <= *rangeMax) {
            *charNo = rangeMax[-2];
            if (*charNo < SRS_CHAR_BOSS_FIRST) {
                *effectType = SRS_RES_COMBO;
                return _loadComboData[*charNo];
            }
            if (effectNo >= 2680) {
                *effectType = SRS_RES_COMBO;
                return _loadComboData[*charNo];
            }
            *effectType = SRS_RES_BATTLE_IMAGE;
            return 0;
        }
    }
    return 0;
}

/* Returns the boss wait effect number for a boss effect number, or 0 when it has no row. */
short srsGetBossWaitEftNo(int effectNo)
{
    int i;

    if ((unsigned int) (effectNo - SRS_CHAR_BOSS_FIRST) < SRS_BOSS_WAIT_ROWS) {
        for (i = 0; i < SRS_BOSS_WAIT_ROWS; i++) {
            if (effectNo == _srsChrEfTbl3[i].effectNo) {
                return _srsChrEfTbl3[i].bossWaitEftNo;
            }
        }
    }
    return 0;
}

void srsSetLoadMode(int load_mode)
{
    srsLoadMode = load_mode;
}

int srsGetLoadMode(void)
{
    return srsLoadMode;
}

void srsSetViewPath(char *path)
{
    strcpy(srsViewPath, path);
}

/*
 * Builds the path of fileName into buffer for the current load mode (CD, host
 * or USB). Returns 0 when fileName is null.
 */
static int srsMakeFileName(char *buffer, const char *fileName)
{
    if (fileName == 0) {
        buffer[0] = '\0';
        return 0;
    }
    if (srsLoadMode == SRS_LOAD_MODE_CD) {
        __builtin_strcpy(buffer, "data/simajiri/");
        strcat(buffer, fileName);
    } else if (srsLoadMode == SRS_LOAD_MODE_HOST) {
        __builtin_strcpy(buffer, "host0:/home/xeno/work/");
        strcat(buffer, srsViewPath);
        strcat(buffer, fileName);
    } else if (srsLoadMode == SRS_LOAD_MODE_USB) {
        __builtin_strcpy(buffer, "usb0:/");
        strcat(buffer, srsViewPath);
        strcat(buffer, fileName);
    }
    return 1;
}

void srsChangeSeparator(char *path)
{
    int index;

    if (path != 0) {
        for (index = 0; path[index] != '\0'; index++) {
            if (path[index] == '/')
                path[index] = '\\';
        }
    }
}

/* Returns the size of a file in the current load mode, or -1 when it cannot be found. */
static int srsGetFileLen(const char *name)
{
    char path[SRS_PATH_LENGTH];
    int descriptor;
    int size;

    if (srsMakeFileName(path, name) == 0) {
        return -1;
    }
    if (srsLoadMode == SRS_LOAD_MODE_CD) {
        srsChangeSeparator(path);
        size = xglCdGetFileSize(path);
        if (size <= 0) {
            return -1;
        }
    } else if (srsLoadMode == SRS_LOAD_MODE_HOST) {
        descriptor = sceOpen(path, SRS_OPEN_RDONLY);
        if (descriptor < 0) {
            return -1;
        }
        size = sceLseek(descriptor, 0, SRS_SEEK_END);
        sceLseek(descriptor, 0, SRS_SEEK_SET);
        sceClose(descriptor);
    } else {
        return -1;
    }
    return size;
}

/*
 * Loads a file into buffer using the current load mode and returns its size,
 * or -1 on failure. In CD mode, mode is passed to xglCdReadFile.
 */
static int fileLoad(void *buffer, const char *name, int mode)
{
    char path[SRS_PATH_LENGTH];
    int descriptor;
    int size;
    int result;

    size = 0;
    if (srsMakeFileName(path, name) == 0) {
        return -1;
    }
    if (srsLoadMode == SRS_LOAD_MODE_CD) {
        srsChangeSeparator(path);
        size = xglCdGetFileSize(path);
        if (size <= 0) {
            return -1;
        }
        _nRead++;
        result = xglCdReadFile(path, buffer, mode, (int) srsCdReadCallback);
        if (result > 0 && mode == 0) {
            size = result;
            _nRead--;
            if (_nRead < 0) {
                _nRead = 0;
            }
        }
    } else if (srsLoadMode == SRS_LOAD_MODE_HOST) {
        descriptor = sceOpen(path, SRS_OPEN_RDONLY);
        if (descriptor < 0) {
            return -1;
        }
        size = sceLseek(descriptor, 0, SRS_SEEK_END);
        sceLseek(descriptor, 0, SRS_SEEK_SET);
        size = sceRead(descriptor, buffer, size);
        sceClose(descriptor);
        sresDataMapping();
    } else if (srsLoadMode == SRS_LOAD_MODE_USB) {
        size = func_00A103C8(path + SRS_USB_PREFIX_LENGTH, buffer);
        if (size > 0) {
            sresDataMapping();
        }
    }
    return size;
}

char *srsGetEffectData(int index)
{
    return srsGetEsdData(index);
}

char **srsGetComboData(int index)
{
    if (index >= SRS_COMBO_DATA_COUNT) {
        return 0;
    }
    return (_loadComboData[index] != 0) ? &_loadComboData[index] : 0;
}

/* Returns the "esd/NAME.esd" path for an effect number, or 0 when it has no name. */
char *srsGetEffectName(int effectNo)
{
    static char msg[SRS_NAME_LENGTH];

    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    if (*name == 0) {
        return 0;
    }
    sprintf(msg, "esd/%s%s", name, ".esd");
    return msg;
}

/*
 * Returns the _weaponTbl row whose weapon effect IDs include effectNo (effects
 * 2900..2998 are looked up 100 lower), or -1 when no row matches.
 */
int srsGetWeaponEffectIdx(int effectNo)
{
    int i;
    int no;

    no = effectNo;
    if ((unsigned int) (no - SRS_EFFECT_WEAPON_ALT_FIRST) < SRS_EFFECT_WEAPON_ALT_COUNT) {
        no = no - SRS_EFFECT_WEAPON_ALT_OFFSET;
    }
    if (no > 0) {
        for (i = 0; i < SRS_WEAPON_ROWS; i++) {
            if (no == _weaponTbl[i].weaponEffectID
                || no == _weaponTbl[i].weaponEffectID2
                || no == _weaponTbl[i].weaponEffectID3) {
                return i;
            }
        }
    }
    return -1;
}

short srsEftNoWeaponEffectID(int effectNo)
{
    int index;
    short weaponEffectID;

    index = srsGetWeaponEffectIdx(effectNo);
    weaponEffectID = 0;
    if (index >= 0) {
        weaponEffectID = _weaponTbl[index].weaponEffectID;
    }
    return weaponEffectID;
}

short srsWeapon2EffectID(int weaponID)
{
    int i;

    if (weaponID > 0) {
        for (i = 0; i < SRS_WEAPON_ROWS; i++) {
            if (weaponID == _weaponTbl[i].weaponID) {
                return _weaponTbl[i].weaponEffectID;
            }
        }
    }
    return -1;
}

short srsEftNo2WeaponID(int effectNo)
{
    int index;
    short weaponID;

    index = srsGetWeaponEffectIdx(effectNo);
    weaponID = 0;
    if (index >= 0) {
        weaponID = _weaponTbl[index].weaponID;
    }
    return weaponID;
}

char *srsGetEffectName2(int effectNo)
{
    static char msg[SRS_NAME_LENGTH];

    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    sprintf(msg, "esp/%s%s", name, ".esp");
    return msg;
}

static char *srsGetComboName(int index)
{
    static char msg[SRS_NAME_LENGTH];

    char **entry;
    char *value;

    entry = srsGetComboData(index);
    if (entry == 0) {
        return 0;
    }
    value = *entry;
    if (value == 0) {
        return 0;
    }
    sprintf(msg, "esp/%s%s", value, ".esp");
    return msg;
}

/* Returns the SrsResourceType category for an effect number. */
int srsGetEffectType(int effectNo)
{
    int effectType;

    effectType = SRS_RES_IMAGE;
    if ((u32) (effectNo - SRS_EFFECT_COMMON_FIRST) >= SRS_EFFECT_COMMON_COUNT) {
        effectType = SRS_RES_WEAPON;
        if ((u32) (effectNo - 2500) >= 100 && effectNo >= 100) {
            effectType = SRS_RES_BATTLE_IMAGE;
            if ((u32) (effectNo - SRS_EFFECT_BATTLE_FIRST) >= SRS_EFFECT_BATTLE_COUNT) {
                effectType = SRS_RES_COMBO;
                if ((u32) (effectNo - SRS_EFFECT_COMBO_FIRST) >= SRS_EFFECT_COMBO_COUNT) {
                    effectType = SRS_RES_BATTLE_IMAGE;
                    if ((u32) (effectNo - SRS_EFFECT_BATTLE2_FIRST) >= SRS_EFFECT_BATTLE2_COUNT) {
                        effectType = SRS_RES_WEAPON;
                        if ((u32) (effectNo - SRS_EFFECT_WEAPON_FIRST) >= SRS_EFFECT_WEAPON_COUNT) {
                            effectType = ((u32) (effectNo - SRS_EFFECT_CF_COMMON_FIRST) < SRS_EFFECT_CF_COMMON_COUNT) ? SRS_RES_CF_IMAGE : SRS_RES_BATTLE_IMAGE;
                        }
                    }
                }
            }
        }
    }
    return effectType;
}

void srsInitCdRead(void)
{
    _nRead = 0;
}

int srsLeaveCdRead(void)
{
    return _nRead;
}

/* xglCdReadFile completion callback: tracks pending reads and maps the data when none remain. */
void srsCdReadCallback(int code)
{
    switch (code) {
    case SRS_CD_READ_COMPLETE:
        _nRead--;
        if (_nRead < 0) {
            _nRead = 0;
        }
        if (_nRead == 0) {
            sresDataMapping();
        }
        break;
    case -1:
        _nRead--;
        if (_nRead < 0) {
            _nRead = 0;
        }
        break;
    case -2: {
        int remaining;

        remaining = _nRead - 1;
        if (remaining < 0) {
            _nRead = 0;
        } else {
            _nRead = remaining;
        }
        break;
    }
    case 0:
    case 1:
    case 2:
    case 3:
        break;
    }
}

int srsLoadEffectData(void *buffer, int effectNo)
{
    char *name;

    name = srsGetEffectName(effectNo);
    if (name == 0) {
        return -1;
    }
    return fileLoad(buffer, name, 1);
}

void sresInitMemoryRes(void)
{
    memset(&_srsMemRes, 0, sizeof(_srsMemRes));
}

/* Loads the common effect image (esp/default.esp) and the effect-name table (seffect.esp) once. */
void sresLoadCommonMemory(void)
{
    SrsMemRes *memRes;
    int size;
    void *chunk[8];

    memRes = &_srsMemRes;
    if (memRes->image == 0) {
        memRes->image = smAlloc(SRS_COMMON_IMAGE_SIZE);
        if (memRes->image != 0) {
            size = fileLoad(memRes->image, "esp/default.esp", 0);
            if (size >= 0) {
                if (size <= SRS_COMMON_IMAGE_SIZE) {
                    chunk[0] = memRes->image;
                    svAddImageMapper(SRS_RES_IMAGE, 0, chunk, SRS_MAPPER_HEIGHT_IMAGE);
                }
            }
        }
        memset(_loadEsdData, 0, SRS_ESD_TABLE_SIZE);
        fileLoad(_loadEsdData, "seffect.esp", 0);
    }
}

/* Loads the CF common image (esp/cf_def.esp) once and registers it with the image mapper. */
void sresLoadCfMemory(void)
{
    SrsMemRes *memRes;
    int size;
    void *chunk[8];

    memRes = &_srsMemRes;
    if (memRes->cfImage == 0) {
        memRes->cfImage = smAlloc(SRS_CF_IMAGE_SIZE);
        if (memRes->cfImage != 0) {
        size = fileLoad(memRes->cfImage, "esp/cf_def.esp", 0);
            if (size >= 0) {
                chunk[0] = memRes->cfImage;
                svAddImageMapper(SRS_RES_CF_IMAGE, 0, chunk, SRS_MAPPER_HEIGHT_CF_IMAGE);
            }
        }
    }
}

/* Frees the weapon or combo image of one reloader slot and removes its image mapper. */
void sresFreeReloaderMemoryNo(int no)
{
    if (no < SRS_CHARACTER_SLOTS) {
        if (_srsMemRes.weaponData[no][0] != 0) {
            svDeleteImageMapper(no * SRS_WEAPONS_PER_CHARACTER + SRS_RES_WEAPON);
            smFree(_srsMemRes.weaponData[no][0]);
            _srsMemRes.weaponData[no][0] = 0;
            _srsMemRes.weaponData[no][1] = 0;
            _srsMemRes.weaponData[no][2] = 0;
        }
    } else {
        if (_srsMemRes.comboData[no - SRS_CHARACTER_SLOTS] != 0) {
            svDeleteImageMapper(no + (SRS_RES_COMBO - SRS_CHARACTER_SLOTS));
            smFree(_srsMemRes.comboData[no - SRS_CHARACTER_SLOTS]);
            _srsMemRes.comboData[no - SRS_CHARACTER_SLOTS] = 0;
        }
    }
}

/* Frees the reloadable battle, combo, weapon and effect images; the CF image only when freeCf is set. */
void sresFreeReloaderMemory(int freeCf)
{
    int i;
    int type;

    if (_srsMemRes.battleImage != 0) {
        svDeleteImageMapper(SRS_RES_BATTLE_IMAGE);
        smFree(_srsMemRes.battleImage);
        _srsMemRes.battleImage = 0;
    }
    for (i = 0; i < SRS_COMBO_SLOTS; i++) {
        if (_srsMemRes.comboData[i] != 0) {
            svDeleteImageMapper(i + SRS_RES_COMBO);
            smFree(_srsMemRes.comboData[i]);
            _srsMemRes.comboData[i] = 0;
        }
    }
    for (i = 0; i < SRS_CHARACTER_SLOTS; i++) {
        if (_srsMemRes.weaponData[i][0] != 0) {
            svDeleteImageMapper(i * SRS_WEAPONS_PER_CHARACTER + SRS_RES_WEAPON);
            svDeleteImageMapper(i * SRS_WEAPONS_PER_CHARACTER + SRS_RES_WEAPON + 1);
            svDeleteImageMapper(i * SRS_WEAPONS_PER_CHARACTER + SRS_RES_WEAPON + 2);
            smFree(_srsMemRes.weaponData[i][0]);
            _srsMemRes.weaponData[i][0] = 0;
            _srsMemRes.weaponData[i][1] = 0;
            _srsMemRes.weaponData[i][2] = 0;
            _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER] = -1;
            _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + 1] = -1;
            _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + 2] = -1;
        }
    }
    for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
        type = i + SRS_RES_EFFECT;
        if (_srsMemRes.effectData[i] != 0) {
            svDeleteImageMapper(type);
            smFree(_srsMemRes.effectData[i]);
            _srsMemRes.effectData[i] = 0;
            _srsMemRes.effectExtra[i] = 0;
            _srsMemRes.no[SRS_RES_EFFECT + i] = -1;
        }
    }
    if (freeCf != 0) {
        if (_srsMemRes.cfImage != 0) {
            svDeleteImageMapper(SRS_RES_CF_IMAGE);
            smFree(_srsMemRes.cfImage);
            _srsMemRes.cfImage = 0;
        }
    }
}

/* Frees all reloadable images and then the common image. */
void sresFreeMemoryRes(void)
{
    SrsMemRes *memRes;

    sresFreeReloaderMemory(1);
    memRes = &_srsMemRes;
    if (memRes->image != 0) {
        svDeleteImageMapper(SRS_RES_IMAGE);
        smFree(memRes->image);
        memRes->image = 0;
    }
}

/* Registers every loaded image whose mapping is still pending with the image mapper. */
void sresDataMapping(void)
{
    void *chunk[8];
    int i;
    int j;
    int pending;
    int type;

    if (_srsMemRes.image != 0) {
        if (_srsMemRes.pending[SRS_RES_IMAGE] != 0) {
            chunk[0] = _srsMemRes.image;
            svAddImageMapper(SRS_RES_IMAGE, 0, chunk, SRS_MAPPER_HEIGHT_IMAGE);
            _srsMemRes.pending[SRS_RES_IMAGE] = 0;
        }
    }
    if (_srsMemRes.effectImage != 0) {
        if (_srsMemRes.pending[SRS_RES_EFFECT_IMAGE] != 0) {
            chunk[0] = _srsMemRes.effectImage;
            svAddImageMapper(SRS_RES_EFFECT_IMAGE, 0, chunk, SRS_MAPPER_HEIGHT_EFFECT_IMAGE);
            _srsMemRes.pending[SRS_RES_EFFECT_IMAGE] = 0;
        }
    }
    for (i = 0; i < SRS_CHARACTER_SLOTS; i++) {
        for (j = 0; j < SRS_WEAPONS_PER_CHARACTER; j++) {
            if (_srsMemRes.weaponData[i][j] != 0) {
                pending = _srsMemRes.pending[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + j];
                if (pending != 0) {
                    chunk[0] = _srsMemRes.weaponData[i][j];
                    svAddImageMapper(i * SRS_WEAPONS_PER_CHARACTER + j + SRS_RES_WEAPON, pending, chunk, SRS_MAPPER_HEIGHT_WEAPON);
                    _srsMemRes.pending[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + j] = 0;
                }
            }
        }
    }
    for (i = 0; i < SRS_COMBO_SLOTS; i++) {
        type = i + SRS_RES_COMBO;
        if (_srsMemRes.comboData[i] != 0) {
            pending = _srsMemRes.pending[SRS_RES_COMBO + i];
            if (pending != 0) {
                chunk[0] = _srsMemRes.comboData[i];
                svAddImageMapper(type, pending, chunk, SRS_MAPPER_HEIGHT_COMBO);
                _srsMemRes.pending[SRS_RES_COMBO + i] = 0;
            }
        }
    }
    if (_srsMemRes.battleImage != 0) {
        if (_srsMemRes.pending[SRS_RES_BATTLE_IMAGE] != 0) {
            chunk[0] = _srsMemRes.battleImage;
            svAddImageMapper(SRS_RES_BATTLE_IMAGE, 0, chunk, SRS_MAPPER_HEIGHT_BATTLE_IMAGE);
            _srsMemRes.pending[SRS_RES_BATTLE_IMAGE] = 0;
        }
    }
    if (_srsMemRes.cfImage != 0) {
        if (_srsMemRes.pending[SRS_RES_CF_IMAGE] != 0) {
            chunk[0] = _srsMemRes.cfImage;
            svAddImageMapper(SRS_RES_CF_IMAGE, 0, chunk, SRS_MAPPER_HEIGHT_CF_IMAGE);
            _srsMemRes.pending[SRS_RES_CF_IMAGE] = 0;
        }
    }
    for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
        type = i + SRS_RES_EFFECT;
        if (_srsMemRes.effectData[i] != 0) {
            pending = _srsMemRes.pending[SRS_RES_EFFECT + i];
            if (pending != 0) {
                chunk[0] = _srsMemRes.effectData[i];
                svAddImageMapper(type, pending, chunk, SRS_MAPPER_HEIGHT_EFFECT);
                _srsMemRes.pending[SRS_RES_EFFECT + i] = 0;
            }
        }
    }
}

/*
 * Reloads the battle images for the characters and enemies in battlePrm. Each
 * requested slot clears its reload flag and loads its image into the slot's buffer.
 */
void sresLoadBattleData(SrsBattlePrm *battlePrm)
{
    int i;
    int j;
    char *name;
    int effectNo;

    /* Free the previous battle's effect and CF images. */
    for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
        if (_srsMemRes.effectData[i] != 0) {
            smFree(_srsMemRes.effectData[i]);
            _srsMemRes.effectData[i] = 0;
            _srsMemRes.effectExtra[i] = 0;
        }
    }
    if (_srsMemRes.cfImage != 0) {
        smFree(_srsMemRes.cfImage);
        _srsMemRes.cfImage = 0;
    }
    /* Character slots: weapon effects for numbers 17..32, combo data below the boss range. */
    for (i = 0; i < SRS_CHARACTER_SLOTS; i++) {
        if (battlePrm->charNo[i] > 0 && battlePrm->charReload[i] != 0) {
            battlePrm->charReload[i] = 0;
            if (battlePrm->charNo[i] < SRS_CHAR_BOSS_FIRST) {
                if (battlePrm->charNo[i] >= SRS_CHAR_WEAPON_FIRST && battlePrm->charNo[i] < SRS_CHAR_WEAPON_END) {
                    if (_srsMemRes.weaponData[i][0] == 0) {
                        _srsMemRes.weaponData[i][0] = smAlloc(SRS_WEAPON_IMAGE_SIZE);
                    }
                    if (_srsMemRes.weaponData[i][0] != 0) {
                        _srsMemRes.weaponData[i][1] = (char *) _srsMemRes.weaponData[i][0] + SRS_WEAPON_SLOT_STRIDE;
                        _srsMemRes.weaponData[i][2] = (char *) _srsMemRes.weaponData[i][0] + SRS_WEAPON_SLOT_STRIDE * 2;
                    }
                    _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER] = -1;
                    _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + 1] = -1;
                    _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + 2] = -1;
                    for (j = 0; j < SRS_WEAPONS_PER_CHARACTER; j++) {
                        if (battlePrm->weaponEffect[i][j] > 0) {
                            effectNo = srsWeapon2EffectID(battlePrm->weaponEffect[i][j]);
                            name = srsGetEffectName2(effectNo);
                            if (name != 0) {
                                _srsMemRes.pending[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + j] = battlePrm->charNo[i];
                                _srsMemRes.no[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + j] = effectNo;
                                if (fileLoad(_srsMemRes.weaponData[i][j], name, 1) < 0) {
                                    _srsMemRes.pending[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER + j] = 0;
                                }
                            }
                        }
                    }
                } else {
                    name = srsGetComboName(battlePrm->charNo[i]);
                    if (_srsMemRes.weaponData[i][0] == 0) {
                        _srsMemRes.weaponData[i][0] = smAlloc(SRS_WEAPON_IMAGE_SIZE);
                    }
                    _srsMemRes.weaponData[i][1] = 0;
                    _srsMemRes.weaponData[i][2] = 0;
                    _srsMemRes.pending[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER] = battlePrm->charNo[i];
                    if (fileLoad(_srsMemRes.weaponData[i][0], name, 1) < 0) {
                        _srsMemRes.pending[SRS_RES_WEAPON + i * SRS_WEAPONS_PER_CHARACTER] = 0;
                    }
                }
            }
        }
    }
    /* Enemy slots: combo data for numbers outside the weapon range. */
    for (i = 0; i < SRS_COMBO_SLOTS; i++) {
        if (battlePrm->enemyNo[i] > 0 && battlePrm->enemyReload[i] != 0) {
            battlePrm->enemyReload[i] = 0;
            if (battlePrm->enemyNo[i] < SRS_CHAR_WEAPON_FIRST || battlePrm->enemyNo[i] >= SRS_CHAR_WEAPON_END) {
                name = srsGetComboName(battlePrm->enemyNo[i]);
                if (_srsMemRes.comboData[i] == 0) {
                    _srsMemRes.comboData[i] = smAlloc(SRS_COMBO_IMAGE_SIZE);
                }
                _srsMemRes.pending[SRS_RES_COMBO + i] = battlePrm->enemyNo[i];
                if (fileLoad(_srsMemRes.comboData[i], name, 1) <= 0) {
                    _srsMemRes.pending[SRS_RES_COMBO + i] = 0;
                }
            }
        }
    }
}

int srsFileLoad(void *buffer, const char *name, int mode)
{
    return fileLoad(buffer, name, mode);
}

/*
 * Loads the CF effect for effectNo into a free effect slot and maps it. Returns
 * the file size, SRS_RES_CF_IMAGE for CF-image effects, 0 when the effect is
 * already loaded, or -1 on failure.
 */
int srsFileLoadCf(int cfId, int effectNo)
{
    int charNo;
    int effectType;
    char *name;
    void *buffer;
    int i;
    int size;

    srsAnalyzeEftNo(effectNo, &charNo, &effectType);
    if (effectType == SRS_RES_CF_IMAGE) {
        return SRS_RES_CF_IMAGE;
    }
    if ((unsigned int) (effectType - SRS_RES_EFFECT) < SRS_EFFECT_SLOTS) {
        name = srsGetEffectName(effectNo);
        if (name == 0) {
            return 0;
        }
        for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
            if (_srsMemRes.effectData[i] != 0 && _srsMemRes.no[SRS_RES_EFFECT + i] == effectNo) {
                return 0;
            }
        }
        for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
            if (_srsMemRes.effectData[i] == 0) {
                size = srsGetFileLen(name);
                if (size <= 0) {
                    return -1;
                }
                buffer = smAlloc(size);
                _srsMemRes.effectData[i] = buffer;
                if (buffer == 0) {
                    tracePrint(srsCfMemoryFullText);
                    return -1;
                }
                size = srsFileLoad(buffer, name, 0);
                if (size > 0) {
                    _srsMemRes.pending[SRS_RES_EFFECT + i] = 1;
                    _srsMemRes.no[SRS_RES_EFFECT + i] = effectNo;
                    sresDataMapping();
                    return size;
                }
                return -1;
            }
        }
    } else {
        tracePrint(srsErrorCategoryFormat, effectType, effectNo);
    }
    tracePrint(srsEffectNotLoadedText);
    return -1;
}

/*
 * Copies size bytes of data into a free effect slot for effectNo and maps it.
 * Return values follow srsFileLoadCf.
 */
int srsMemoryLoadCf(void *data, int effectNo, int size)
{
    int charNo;
    int effectType;
    void *buffer;
    int i;

    srsAnalyzeEftNo(effectNo, &charNo, &effectType);
    if (effectType == SRS_RES_CF_IMAGE) {
        return SRS_RES_CF_IMAGE;
    }
    if ((unsigned int) (effectType - SRS_RES_EFFECT) < SRS_EFFECT_SLOTS) {
        for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
            if (_srsMemRes.effectData[i] != 0 && _srsMemRes.no[SRS_RES_EFFECT + i] == effectNo) {
                return 0;
            }
        }
        for (i = 0; i < SRS_EFFECT_SLOTS; i++) {
            if (_srsMemRes.effectData[i] == 0) {
                if (size <= 0) {
                    return -1;
                }
                buffer = smAlloc(size);
                _srsMemRes.effectData[i] = buffer;
                if (buffer == 0) {
                    tracePrint(srsCfMemoryFullText);
                    return -1;
                }
                memcpy(buffer, data, size);
                _srsMemRes.pending[SRS_RES_EFFECT + i] = 1;
                _srsMemRes.no[SRS_RES_EFFECT + i] = effectNo;
                sresDataMapping();
                return size;
            }
        }
    } else {
        tracePrint(srsErrorCategoryFormat, effectType, effectNo);
    }
    tracePrint(srsEffectNotLoadedText);
    return -1;
}

int srsEffectNameToID(char *effectName)
{
    int index;
    char *name;

    if (effectName == 0) {
        return 0;
    }
    for (index = 1; index < SRS_EFFECT_NO_COUNT; index++) {
        name = srsGetEffectData(index);
        if (name != 0 && strcmp(effectName, name) == 0) {
            return index;
        }
    }
    return 0;
}
