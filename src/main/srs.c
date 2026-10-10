#include "common.h"

#include "shared.h"

#include "srs.h"

extern char D_004CBFC0[];
extern char D_004CBFD0[];
extern char D_004CBFE0[];
extern char D_004CBFF0[];
extern char D_004CC000[];
extern char D_004CC010[];
extern char D_004CC020[];
extern char D_004CC030[];
extern char D_004CC040[];
extern char D_004CC050[];
extern char D_004CC060[];
extern char D_004CC070[];
extern char D_004CC080[];
extern char D_004CC090[];
extern char D_004CC0A0[];
extern char D_004CC0B0[];
extern char D_004CC0C0[];
extern char D_004CC0D0[];
extern char D_004CC0E0[];
extern char D_004CC0F0[];
extern char D_004CC100[];
extern char D_004CC110[];
extern char D_004CC120[];
extern char D_004CC130[];
extern char D_004CC140[];
extern char D_004CC150[];
extern char D_004CC160[];
extern char D_004CC170[];
extern char D_004CC180[];
extern char D_004CC190[];
extern char D_004CC1A0[];
extern char D_004CC1B0[];
extern char D_004CC1C0[];
extern char D_004CC1D0[];
extern char D_004CC1E0[];
extern char D_004CC1F0[];
extern char D_004CC200[];
extern char D_004CC210[];
extern char D_004CC220[];
extern char D_004CC230[];
extern char D_004CC240[];
extern char D_004CC250[];
extern char D_004CC260[];
extern char D_004CC270[];
extern char D_004CC280[];
extern char D_004CC290[];
extern char D_004CC2A0[];
extern char D_004CC2B0[];
extern char D_004CC2C0[];
extern char D_004CC2D0[];
extern char D_004CC2E0[];
extern char D_004CC2F0[];
extern char D_004CC300[];
extern char D_004CC310[];
extern char D_004CC320[];
extern char D_004CC330[];
extern char D_004CC340[];
extern char D_004CC350[];
extern char D_004CC360[];
extern char D_004CC370[];
extern char D_004CC380[];
extern char D_004CC390[];
extern char D_004CC3A0[];
extern char D_004CC3B0[];
extern char D_004CC3C0[];
extern char D_004CC3D0[];
extern char D_004CC3E0[];
extern char D_004CC3F0[];
extern char D_004CC400[];
extern char D_004CC410[];
extern char D_004CC420[];
extern char D_004CC430[];
extern char D_004CC440[];
extern char D_004CC450[];
extern char D_004CC460[];
extern char D_004CC470[];
extern char D_004CC480[];
extern char D_004CC490[];
extern char D_004CC4A0[];
extern char D_004CC4B0[];
extern char D_004CC4C0[];
extern char D_004CC4D0[];
extern char D_004CC4E0[];
extern char D_004CC4F0[];
extern char D_004CC500[];
extern char D_004CC510[];
extern char D_004CC520[];
extern char D_004CC530[];
extern char D_004CC540[];
extern char D_004CC550[];
extern char D_004CC560[];
extern char D_004CC570[];
extern char D_004CC580[];
extern char D_004CC590[];
extern char D_004CC5A0[];
extern char D_004CC5B0[];
extern char D_004CC5C0[];
extern char D_004CC5D0[];
extern char D_004CC5E0[];
extern char D_004CC5F0[];
extern char D_004CC600[];
extern char D_004CC610[];
extern char D_004CC620[];
extern char D_004CC630[];
extern char D_004CC640[];
extern char D_004CC650[];
extern char D_004CC660[];
extern char D_004CC670[];
extern char D_004DBAB0[];
extern char D_004DBAA0[];
extern char D_004DBB10[];
extern char D_004DBB08[];
extern char D_004DBB00[];
extern char D_004DBAF8[];
extern char D_004DBAF0[];
extern char D_004DBAE8[];
extern char D_004DBAE0[];
extern char D_004DBAD8[];
extern char D_004DBAD0[];
extern char D_004DBAC8[];
extern char D_004DBAC0[];
extern char D_004DBAB8[];

char _loadEsdData[0xC800] = {0};

/* 0xC800-byte effect-name table. */

/* 0xC800-byte table (config/symbols/main.txt) */

extern int srsGetEffect2Idx(int effectNo);

/*
 * One _srsChrEfTbl3 row (6 bytes; the 0xD8-byte symbol holds 36 rows).
 * srsGetBossWaitEftNo scans effectNo and returns the matching bossWaitEftNo.
 */

typedef struct SrsChrEfTbl3Entry {
    short effectNo;
    short bossWaitEftNo;
    unsigned char unmodeled_04[2];
} SrsChrEfTbl3Entry;

static SrsChrEfTbl3Entry _srsChrEfTbl3[36] = {
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

/*
 * The original srs prefix is retained; its historical expansion is not
 * established by the available evidence. The mode word is local to this TU.
 */

static unsigned char srsViewPath[256] = {0};

extern unsigned char *strcpy(unsigned char *destination, const unsigned char *source);

/*
 * 0x30C-byte table of 195 pointer-sized slots (config/symbols/main.txt).
 */

char *_loadComboData[195] = {
    D_004DBAA0, D_004DBB10, D_004DBB08, D_004DBB00, D_004DBAF8,
    D_004DBAF0, D_004DBAE8, D_004DBAE0, D_004DBAD8, D_004CC670,
    D_004DBAD0, D_004DBAC8, D_004DBAC0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, D_004CC660, D_004CC660,
    D_004CC650, D_004CC640, D_004CC630, D_004CC620, D_004CC610,
    D_004CC600, D_004CC5F0, D_004CC5E0, D_004CC5D0, D_004CC5C0,
    D_004CC5B0, D_004CC5A0, D_004CC590, D_004CC580, D_004CC570,
    D_004CC560, D_004CC550, D_004CC540, D_004CC530, D_004CC520,
    D_004CC510, D_004CC500, D_004CC4F0, D_004CC4E0, D_004CC660,
    D_004CC650, D_004CC640, D_004CC4D0, D_004CC4C0, D_004CC4B0,
    D_004CC4A0, D_004CC490, D_004CC480, D_004CC470, D_004CC460,
    D_004CC450, D_004CC440, D_004CC430, D_004CC420, D_004CC410,
    D_004CC400, D_004CC3F0, D_004CC3E0, D_004CC3D0, D_004CC3C0,
    D_004CC3B0, D_004CC3A0, D_004CC390, D_004CC380, D_004CC370,
    D_004CC360, D_004CC350, D_004CC340, D_004CC330, D_004CC320,
    D_004CC310, D_004CC300, D_004CC2F0, D_004CC2E0, D_004CC2D0,
    D_004CC2C0, D_004CC2B0, D_004CC2A0, D_004CC290, D_004CC280,
    D_004CC270, D_004CC260, D_004CC250, D_004CC240, D_004CC230,
    D_004CC220, D_004CC210, D_004CC200, D_004CC1F0, D_004CC1E0,
    D_004CC1D0, D_004CC3D0, D_004CC3D0, D_004CC390, D_004CC610,
    D_004CC4C0, D_004CC420, D_004CC410, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, D_004CC420, D_004CC420, D_004CC420, D_004CC420,
    D_004CC420, D_004CC1C0, D_004CC1B0, D_004CC1A0, D_004CC190,
    D_004CC180, D_004CC170, D_004CC160, D_004CC150, D_004CC140,
    D_004CC130, D_004CC120, D_004CC110, D_004CC100, D_004CC0F0,
    D_004CC0E0, D_004CC0D0, D_004CC0C0, D_004CC0B0, D_004CC0A0,
    D_004CC090, D_004CC080, D_004CC070, D_004CC060, D_004CC050,
    D_004CC040, D_004CC030, D_004CC020, D_004CC010, D_004CC000,
    D_004CBFF0, D_004CBFE0, D_004CBFD0, D_004CBFC0, D_004CC1A0,
    D_004CC190, D_004CC090, D_004DBAB8, D_004DBAB8, D_004DBAB8,
    D_004DBAB8, D_004DBAB8, D_004DBAB8, D_004DBAB8, D_004DBAB8,
};

static char msg_0_00795120[128];

extern int srsGetWeaponEffectIdx(int effectNo);

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

static WeaponTblEntry _weaponTbl[45] = {
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

static char msg_1_007951A0[128];

static char msg_2_00795220[128];

extern void sresDataMapping(void);

extern void *smAlloc(unsigned int size);

/*
 * svAddImageMapper takes the address of a chunk list; svAnalyzeChunk walks it
 * from its first entry, which is the only one these two loaders fill.
 */

extern void svAddImageMapper(int type, int width, void *chunk, int height);

extern void smFree(void *block);

extern void svDeleteImageMapper(int type);

extern void sresFreeReloaderMemory(int reload_bgm);

extern int strcmp(const char *s1, const char *s2);

char D_004DBAA0[8] = "default";

char D_004DBAB8[8] = "";

char D_004DBAC0[8] = "shelley";

char D_004DBAC8[8] = "mary";

char D_004DBAD0[8] = "virgil";

char D_004DBAD8[8] = "cecilia";

char D_004DBAE0[8] = "jr";

char D_004DBAE8[8] = "momo";

char D_004DBAF0[8] = "ziggy";

char D_004DBAF8[8] = "shitan";

char D_004DBB00[8] = "shion";

char D_004DBB08[8] = "kosmos";

char D_004DBB10[8] = "chaos";

SrsMemRes _srsMemRes = {0};

/* 0xC800-byte effect-name table. */

/* 0xC800-byte table (config/symbols/main.txt) */

static signed char _srsEffect2IndexTbl[117] = {
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

static SrsChrEfTblEntry _srsChrEfTbl[12] = {
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

static SrsChrEfTbl2Entry _srsChrEfTbl2[161] = {
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

char *_loadCfCommonData = D_004DBAB0;

extern char *strcat(char *destination, const char *source);

extern int xglCdGetFileSize(const char *name);

extern int sceOpen(const char *path, int flags, ...);

extern int sceLseek(int descriptor, int offset, int whence);

extern int sceRead(int descriptor, void *buffer, int bytes);

extern int sceClose(int descriptor);

extern int func_00A103C8(const char *path, void *buffer);

extern void srsCdReadCallback(int code);

typedef struct SrsBattlePrm {
    short charNo[3];
    short enemyNo[3];
    unsigned char unmodeled_0C[6];
    short weaponEffect[3][3];
    short charReload[3];
    short enemyReload[3];
    unsigned char unmodeled_30[4];
} SrsBattlePrm;

extern void tracePrint(const char *format, ...);

static const char D_004CC720[40] = "#### \243\303\243\306\245\250\245\325\245\247\245\257\245\310\245\341\245\342\245\352\244\254\302\255\244\352\244\336\244\273\244\363!!";

static const char D_004CC748[32] = "-- err category(%d) eft(%d)";

static const char D_004CC768[40] = "-- err : eftNo is not cf load";

extern void *memcpy(void *destination, const void *source, unsigned int count);

int srsGetEffect2Idx(int effectNo)
{
    int idx;

    if ((unsigned int) (effectNo - 601) < 117) {
        idx = _srsEffect2IndexTbl[effectNo - 601];
    } else if ((unsigned int) (effectNo - 2600) < 12) {
        idx = effectNo - 2600;
    } else if ((unsigned int) (effectNo - 2612) < 9) {
        idx = effectNo - 2612;
    } else if ((unsigned int) (effectNo - 2651) < 10) {
        idx = effectNo - 2651;
    } else if ((unsigned int) (effectNo - 2626) < 4) {
        idx = effectNo - 2626;
    } else if ((unsigned int) (effectNo - 2630) < 7) {
        idx = effectNo - 2630;
    } else {
        idx = ((unsigned int) (effectNo - 2900) < 99) ? 100 : 0;
    }
    return idx;
}

char *srsGetEsdData(int index)
{
    int idx;

    if ((unsigned int) index >= 0xc00) {
        return 0;
    }
    idx = srsGetEffect2Idx(index);
    return _loadEsdData + (index - idx) * 0x10;
}

char *srsGetEsdData2(int effectNo)
{
    if ((unsigned int) effectNo >= 0xc00) {
        return 0;
    }
    return _loadEsdData + effectNo * 0x10;
}

char *sefGetEffectName(int effectNo)
{
    if ((unsigned int) effectNo >= 0xc00) {
        return 0;
    }
    return _loadEsdData + effectNo * 0x10;
}

char *srsAnalyzeEftNo(int effectNo, int *charNo, int *effectType)
{
    int i;
    char *result;
    SrsChrEfTbl2Entry *entry;
    short *rangeMax;

    *charNo = 0;
    *effectType = 14;
    if ((unsigned int) (effectNo - 1) >= 3071) {
        return 0;
    }
    if (effectNo >= 3042) {
        *effectType = 14;
        return 0;
    }
    if ((unsigned int) (effectNo - 2600) < 62) {
        *charNo = 0;
        *effectType = 0;
        return 0;
    }
    if ((unsigned int) (effectNo - 2800) < 200) {
        *effectType = 2;
        *charNo = 17;
        return 0;
    }
    if ((unsigned int) (effectNo - 600) < 400) {
        result = _loadCfCommonData;
        *effectType = 15;
        return result;
    }
    if ((unsigned int) (effectNo - 2200) < 10) {
        *charNo = 2;
        *effectType = 14;
        return 0;
    }
    if ((unsigned int) (effectNo - 2210) < 8) {
        *charNo = 5;
        *effectType = 14;
        return 0;
    }
    if ((unsigned int) (effectNo - 1000) < 1000) {
        *charNo = 0;
        *effectType = 16;
        return 0;
    }
    if ((unsigned int) (effectNo - 2000) < 219) {
        *charNo = 0;
        *effectType = 14;
        return 0;
    }
    for (i = 0; i < 12; i++) {
        if (effectNo >= _srsChrEfTbl[i].comboMin && effectNo <= _srsChrEfTbl[i].comboMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = 2;
            return _loadComboData[*charNo];
        }
        if (effectNo >= _srsChrEfTbl[i].firstMin && effectNo <= _srsChrEfTbl[i].firstMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = 14;
            return 0;
        }
        if (effectNo >= _srsChrEfTbl[i].secondMin && effectNo <= _srsChrEfTbl[i].secondMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = 14;
            return 0;
        }
        if (effectNo >= _srsChrEfTbl[i].alternateMin && effectNo <= _srsChrEfTbl[i].alternateMax) {
            *charNo = _srsChrEfTbl[i].charNo;
            *effectType = 2;
            return _loadComboData[*charNo];
        }
    }
    entry = _srsChrEfTbl2;
    rangeMax = &entry->effectMax;
    for (i = 0; i < 161; i++, rangeMax += 3) {
        if (effectNo >= rangeMax[-1] && effectNo <= *rangeMax) {
            *charNo = rangeMax[-2];
            if (*charNo < 151) {
                *effectType = 11;
                return _loadComboData[*charNo];
            }
            if (effectNo >= 2680) {
                *effectType = 11;
                return _loadComboData[*charNo];
            }
            *effectType = 14;
            return 0;
        }
    }
    return 0;
}

short srsGetBossWaitEftNo(int effectNo)
{
    int i;

    if ((unsigned int) (effectNo - 151) < 36) {
        for (i = 0; i < 36; i++) {
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

void srsSetViewPath(unsigned char *path)
{
    strcpy(srsViewPath, path);
}

static int srsMakeFileName(char *buffer, const char *fileName)
{
    if (fileName == 0) {
        buffer[0] = '\0';
        return 0;
    }
    if (srsLoadMode == 0) {
        __builtin_strcpy(buffer, "data/simajiri/");
        strcat(buffer, fileName);
    } else if (srsLoadMode == 1) {
        __builtin_strcpy(buffer, "host0:/home/xeno/work/");
        strcat(buffer, srsViewPath);
        strcat(buffer, fileName);
    } else if (srsLoadMode == 2) {
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

static int srsGetFileLen(const char *name)
{
    char path[256];
    int descriptor;
    int size;

    if (srsMakeFileName(path, name) == 0) {
        return -1;
    }
    if (srsLoadMode == 0) {
        srsChangeSeparator(path);
        size = xglCdGetFileSize(path);
        if (size <= 0) {
            return -1;
        }
    } else if (srsLoadMode == 1) {
        descriptor = sceOpen(path, 1);
        if (descriptor < 0) {
            return -1;
        }
        size = sceLseek(descriptor, 0, 2);
        sceLseek(descriptor, 0, 0);
        sceClose(descriptor);
    } else {
        return -1;
    }
    return size;
}

static int fileLoad(void *buffer, const char *name, int mode)
{
    char path[256];
    int descriptor;
    int size;
    int result;


    size = 0;
    if (srsMakeFileName(path, name) == 0) {
        return -1;
    }
    if (srsLoadMode == 0) {
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
    } else if (srsLoadMode == 1) {
        descriptor = sceOpen(path, 1);
        if (descriptor < 0) {
            return -1;
        }
        size = sceLseek(descriptor, 0, 2);
        sceLseek(descriptor, 0, 0);
        size = sceRead(descriptor, buffer, size);
        sceClose(descriptor);
        sresDataMapping();
    } else if (srsLoadMode == 2) {
        size = func_00A103C8(path + 5, buffer);
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
    if (index >= 195) {
        return 0;
    }
    return (_loadComboData[index] != 0) ? &_loadComboData[index] : 0;
}

char *srsGetEffectName(int effectNo)
{
    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    if (*name == 0) {
        return 0;
    }
    sprintf(msg_0_00795120, "esd/%s%s", name, ".esd");
    return msg_0_00795120;
}

int srsGetWeaponEffectIdx(int effectNo)
{
    int i;
    int no;

    no = effectNo;
    if ((unsigned int) (no - 2900) < 99) {
        no = no - 100;
    }
    if (no > 0) {
        for (i = 0; i < 45; i++) {
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
        for (i = 0; i < 45; i++) {
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
    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    sprintf(msg_1_007951A0, "esp/%s%s", name, ".esp");
    return msg_1_007951A0;
}

static char *srsGetComboName(int index)
{
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
    sprintf(msg_2_00795220, "esp/%s%s", value, ".esp");
    return msg_2_00795220;
}

int srsGetEffectType(int effectNo)
{
    int effectType;

    effectType = 0;
    if ((u32) (effectNo - 0xA28) >= 0x3E) {
        effectType = 2;
        if ((u32) (effectNo - 0x9C4) >= 0x64 && effectNo >= 0x64) {
            effectType = 0xE;
            if ((u32) (effectNo - 0x7D0) >= 0xDB) {
                effectType = 0xB;
                if ((u32) (effectNo - 0xF0) >= 0x9D) {
                    effectType = 0xE;
                    if ((u32) (effectNo - 0x8FC) >= 0xB6) {
                        effectType = 2;
                        if ((u32) (effectNo - 0xAF0) >= 0xC8) {
                            effectType = ((u32) (effectNo - 0x258) < 0x190) ? 0xF : 0xE;
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

void srsCdReadCallback(int code)
{
    switch (code) {
    case 4:
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

void sresLoadCommonMemory(void)
{
    SrsMemRes *memRes;
    int size;
    void *chunk[8];

    memRes = &_srsMemRes;
    if (memRes->image == 0) {
        memRes->image = smAlloc(0x25800);
        if (memRes->image != 0) {
            size = fileLoad(memRes->image, "esp/default.esp", 0);
            if (size >= 0) {
                if (size <= 0x25800) {
                    chunk[0] = memRes->image;
                    svAddImageMapper(0, 0, chunk, 3000);
                }
            }
        }
        memset(_loadEsdData, 0, 0xC800);
        fileLoad(_loadEsdData, "seffect.esp", 0);
    }
}

void sresLoadCfMemory(void)
{
    SrsMemRes *memRes;
    int size;
    void *chunk[8];

    memRes = &_srsMemRes;
    if (memRes->cfImage == 0) {
        memRes->cfImage = smAlloc(0x3E000);
        if (memRes->cfImage != 0) {
        size = fileLoad(memRes->cfImage, "esp/cf_def.esp", 0);
            if (size >= 0) {
                chunk[0] = memRes->cfImage;
                svAddImageMapper(15, 0, chunk, 3500);
            }
        }
    }
}

void sresFreeReloaderMemoryNo(int no)
{
    if (no < 3) {
        if (_srsMemRes.weaponData[no][0] != 0) {
            svDeleteImageMapper(no * 3 + 2);
            smFree(_srsMemRes.weaponData[no][0]);
            _srsMemRes.weaponData[no][0] = 0;
            _srsMemRes.weaponData[no][1] = 0;
            _srsMemRes.weaponData[no][2] = 0;
        }
    } else {
        if (_srsMemRes.comboData[no - 3] != 0) {
            svDeleteImageMapper(no + 8);
            smFree(_srsMemRes.comboData[no - 3]);
            _srsMemRes.comboData[no - 3] = 0;
        }
    }
}

void sresFreeReloaderMemory(int freeCf)
{
    int i;
    int type;

    if (_srsMemRes.battleImage != 0) {
        svDeleteImageMapper(14);
        smFree(_srsMemRes.battleImage);
        _srsMemRes.battleImage = 0;
    }
    for (i = 0; i < 3; i++) {
        if (_srsMemRes.comboData[i] != 0) {
            svDeleteImageMapper(i + 11);
            smFree(_srsMemRes.comboData[i]);
            _srsMemRes.comboData[i] = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (_srsMemRes.weaponData[i][0] != 0) {
            svDeleteImageMapper(i * 3 + 2);
            svDeleteImageMapper(i * 3 + 3);
            svDeleteImageMapper(i * 3 + 4);
            smFree(_srsMemRes.weaponData[i][0]);
            _srsMemRes.weaponData[i][0] = 0;
            _srsMemRes.weaponData[i][1] = 0;
            _srsMemRes.weaponData[i][2] = 0;
            _srsMemRes.no[SRS_RES_WEAPON + i * 3] = -1;
            _srsMemRes.no[SRS_RES_WEAPON + i * 3 + 1] = -1;
            _srsMemRes.no[SRS_RES_WEAPON + i * 3 + 2] = -1;
        }
    }
    for (i = 0; i < 24; i++) {
        type = i + 16;
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
            svDeleteImageMapper(15);
            smFree(_srsMemRes.cfImage);
            _srsMemRes.cfImage = 0;
        }
    }
}

void sresFreeMemoryRes(void)
{
    SrsMemRes *memRes;

    sresFreeReloaderMemory(1);
    memRes = &_srsMemRes;
    if (memRes->image != 0) {
        svDeleteImageMapper(0);
        smFree(memRes->image);
        memRes->image = 0;
    }
}

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
            svAddImageMapper(0, 0, chunk, 3000);
            _srsMemRes.pending[SRS_RES_IMAGE] = 0;
        }
    }
    if (_srsMemRes.effectImage != 0) {
        if (_srsMemRes.pending[SRS_RES_EFFECT_IMAGE] != 0) {
            chunk[0] = _srsMemRes.effectImage;
            svAddImageMapper(1, 0, chunk, 3100);
            _srsMemRes.pending[SRS_RES_EFFECT_IMAGE] = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (_srsMemRes.weaponData[i][j] != 0) {
                pending = _srsMemRes.pending[SRS_RES_WEAPON + i * 3 + j];
                if (pending != 0) {
                    chunk[0] = _srsMemRes.weaponData[i][j];
                    svAddImageMapper(i * 3 + j + 2, pending, chunk, 2000);
                    _srsMemRes.pending[SRS_RES_WEAPON + i * 3 + j] = 0;
                }
            }
        }
    }
    for (i = 0; i < 3; i++) {
        type = i + 11;
        if (_srsMemRes.comboData[i] != 0) {
            pending = _srsMemRes.pending[SRS_RES_COMBO + i];
            if (pending != 0) {
                chunk[0] = _srsMemRes.comboData[i];
                svAddImageMapper(type, pending, chunk, 2100);
                _srsMemRes.pending[SRS_RES_COMBO + i] = 0;
            }
        }
    }
    if (_srsMemRes.battleImage != 0) {
        if (_srsMemRes.pending[SRS_RES_BATTLE_IMAGE] != 0) {
            chunk[0] = _srsMemRes.battleImage;
            svAddImageMapper(14, 0, chunk, 3400);
            _srsMemRes.pending[SRS_RES_BATTLE_IMAGE] = 0;
        }
    }
    if (_srsMemRes.cfImage != 0) {
        if (_srsMemRes.pending[SRS_RES_CF_IMAGE] != 0) {
            chunk[0] = _srsMemRes.cfImage;
            svAddImageMapper(15, 0, chunk, 3500);
            _srsMemRes.pending[SRS_RES_CF_IMAGE] = 0;
        }
    }
    for (i = 0; i < 24; i++) {
        type = i + 16;
        if (_srsMemRes.effectData[i] != 0) {
            pending = _srsMemRes.pending[SRS_RES_EFFECT + i];
            if (pending != 0) {
                chunk[0] = _srsMemRes.effectData[i];
                svAddImageMapper(type, pending, chunk, 3600);
                _srsMemRes.pending[SRS_RES_EFFECT + i] = 0;
            }
        }
    }
}

void sresLoadBattleData(SrsBattlePrm *battlePrm)
{
    int i;
    int j;
    char *name;
    int effectNo;

    for (i = 0; i < 24; i++) {
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
    for (i = 0; i < 3; i++) {
        if (battlePrm->charNo[i] > 0 && battlePrm->charReload[i] != 0) {
            battlePrm->charReload[i] = 0;
            if (battlePrm->charNo[i] < 151) {
                if (battlePrm->charNo[i] >= 17 && battlePrm->charNo[i] < 33) {
                    if (_srsMemRes.weaponData[i][0] == 0) {
                        _srsMemRes.weaponData[i][0] = smAlloc(0x12800);
                    }
                    if (_srsMemRes.weaponData[i][0] != 0) {
                        _srsMemRes.weaponData[i][1] = (char *) _srsMemRes.weaponData[i][0] + 0x6000;
                        _srsMemRes.weaponData[i][2] = (char *) _srsMemRes.weaponData[i][0] + 0xC000;
                    }
                    _srsMemRes.no[SRS_RES_WEAPON + i * 3] = -1;
                    _srsMemRes.no[SRS_RES_WEAPON + i * 3 + 1] = -1;
                    _srsMemRes.no[SRS_RES_WEAPON + i * 3 + 2] = -1;
                    for (j = 0; j < 3; j++) {
                        if (battlePrm->weaponEffect[i][j] > 0) {
                            effectNo = srsWeapon2EffectID(battlePrm->weaponEffect[i][j]);
                            name = srsGetEffectName2(effectNo);
                            if (name != 0) {
                                _srsMemRes.pending[SRS_RES_WEAPON + i * 3 + j] = battlePrm->charNo[i];
                                _srsMemRes.no[SRS_RES_WEAPON + i * 3 + j] = effectNo;
                                if (fileLoad(_srsMemRes.weaponData[i][j], name, 1) < 0) {
                                    _srsMemRes.pending[SRS_RES_WEAPON + i * 3 + j] = 0;
                                }
                            }
                        }
                    }
                } else {
                    name = srsGetComboName(battlePrm->charNo[i]);
                    if (_srsMemRes.weaponData[i][0] == 0) {
                        _srsMemRes.weaponData[i][0] = smAlloc(0x12800);
                    }
                    _srsMemRes.weaponData[i][1] = 0;
                    _srsMemRes.weaponData[i][2] = 0;
                    _srsMemRes.pending[SRS_RES_WEAPON + i * 3] = battlePrm->charNo[i];
                    if (fileLoad(_srsMemRes.weaponData[i][0], name, 1) < 0) {
                        _srsMemRes.pending[SRS_RES_WEAPON + i * 3] = 0;
                    }
                }
            }
        }
    }
    for (i = 0; i < 3; i++) {
        if (battlePrm->enemyNo[i] > 0 && battlePrm->enemyReload[i] != 0) {
            battlePrm->enemyReload[i] = 0;
            if (battlePrm->enemyNo[i] < 17 || battlePrm->enemyNo[i] >= 33) {
                name = srsGetComboName(battlePrm->enemyNo[i]);
                if (_srsMemRes.comboData[i] == 0) {
                    _srsMemRes.comboData[i] = smAlloc(0x12000);
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

int srsFileLoadCf(int cfId, int effectNo)
{
    int charNo;
    int effectType;
    char *name;
    void *buffer;
    int i;
    int size;

    srsAnalyzeEftNo(effectNo, &charNo, &effectType);
    if (effectType == 15) {
        return 15;
    }
    if ((unsigned int) (effectType - 16) < 24) {
        name = srsGetEffectName(effectNo);
        if (name == 0) {
            return 0;
        }
        for (i = 0; i < 24; i++) {
            if (_srsMemRes.effectData[i] != 0 && _srsMemRes.no[SRS_RES_EFFECT + i] == effectNo) {
                return 0;
            }
        }
        for (i = 0; i < 24; i++) {
            if (_srsMemRes.effectData[i] == 0) {
                size = srsGetFileLen(name);
                if (size <= 0) {
                    return -1;
                }
                buffer = smAlloc(size);
                _srsMemRes.effectData[i] = buffer;
                if (buffer == 0) {
                    tracePrint(D_004CC720);
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
        tracePrint(D_004CC748, effectType, effectNo);
    }
    tracePrint(D_004CC768);
    return -1;
}

int srsMemoryLoadCf(void *data, int effectNo, int size)
{
    int charNo;
    int effectType;
    void *buffer;
    int i;

    srsAnalyzeEftNo(effectNo, &charNo, &effectType);
    if (effectType == 15) {
        return 15;
    }
    if ((unsigned int) (effectType - 16) < 24) {
        for (i = 0; i < 24; i++) {
            if (_srsMemRes.effectData[i] != 0 && _srsMemRes.no[SRS_RES_EFFECT + i] == effectNo) {
                return 0;
            }
        }
        for (i = 0; i < 24; i++) {
            if (_srsMemRes.effectData[i] == 0) {
                if (size <= 0) {
                    return -1;
                }
                buffer = smAlloc(size);
                _srsMemRes.effectData[i] = buffer;
                if (buffer == 0) {
                    tracePrint(D_004CC720);
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
        tracePrint(D_004CC748, effectType, effectNo);
    }
    tracePrint(D_004CC768);
    return -1;
}

int srsEffectNameToID(char *effectName)
{
    int index;
    char *name;

    if (effectName == 0) {
        return 0;
    }
    for (index = 1; index < 3072; index++) {
        name = srsGetEffectData(index);
        if (name != 0 && strcmp(effectName, name) == 0) {
            return index;
        }
    }
    return 0;
}
