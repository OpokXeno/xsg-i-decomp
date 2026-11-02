/*
 * TU-local declarations of main/tu172 (src/main/seisan_count_init1.c).
 */

#ifndef SRC_MAIN_SEISAN_COUNT_INIT1_H
#define SRC_MAIN_SEISAN_COUNT_INIT1_H

#include "shared.h"
#include "main/menu_1.h"
#include "main/data.h"
#include "main/e_battle_win_open.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/file.h"
#include "main/menu_top_command_check.h"
#include "main/party.h"
#include "main/xgl_cd.h"
#include "main/xgl_dma.h"

/*
 * The closing state subSeisanCheck selects when no check selected one:
 * subSeisanMain's case for it (li 0xf0 at 0x002a166c) waits 0x10 frames,
 * calls MenuBgTaskBreak and then sets the current and requested states to
 * 0xff, the finished state SeisanMain reports as "no longer running".
 */
#define SEISAN_STATE_CLOSING 0xf0

#define SEISAN_STATE_FINISHED 0xff

/* The settlement screen's 64-byte control block, cleared by SeisanInit. */
typedef struct SeisanControlState {
    unsigned char unmodeled_00;
    unsigned char state;
    unsigned char flags;
    unsigned char requestedState;
    unsigned char nextState;
    unsigned char transitionTicks;
    unsigned char partySize;
    unsigned char unmodeled_07[64 - 7];
} SeisanControlState;

/*
 * dataPlChaGet (ov01/data_unit_org_get.c VA 0x00a19210, config/symbols/ov01.txt):
 * the running character's persistent record. SeisanCountInit1 reads the word
 * at +0x04 (next_exp, see src/main/menu_skill.h and src/main/menu_para_pt_rate_get.c,
 * which model this same record under the same scaffold label). Kept under the
 * scaffold's undefined_funcs_auto.txt placeholder name func_A19210 until ov01
 * recovers dataPlChaGet itself.
 */
typedef struct SeisanCharRecord {
    int experience;
    int nextExp;                    /* +0x04 */
    unsigned char unmodeled_08[4];
    int techPoints;
    int skillPoints;
    int specialPoints;
} SeisanCharRecord;

typedef struct SeisanCountTarget {
    int level;
    int hp;
    int ep;
    int nextExp;
    int experience;
    int techPoints;
    int skillPoints;
    int specialPoints;
} SeisanCountTarget;

/*
 * SeisanCN (0x00534f00, config/symbols/main.txt, size 0xb0): SeisanCountInit1
 * zeroes the whole block (memset at 0x0029f468) and then fills twelve
 * consecutive words at +0x70 (addiu 0x0029f46c, sw 0x0029f484) with each
 * character's dataPlChaGet(chrNo)->nextExp, for chrNo 1..12.
 */
typedef struct SeisanCountData {
    unsigned int state;             /* +0x00: selected by subSeisanMain */
    SeisanCountTarget targets[3];    /* +0x04: three 32-byte snapshots */
    unsigned char unmodeled_64[4];
    long countedMoney;              /* +0x68: ld/sd in SeisanCountMain */
    int nextExp[12];                /* +0x70: character 1..12's next_exp, set by SeisanCountInit1 */
    unsigned char unmodeled_A0[0x10];
} SeisanCountData;

/*
 * SeisanResult (0x004db0cc, config/symbols/main.txt, size 0x4): a pointer to
 * the settlement screen's per-entry result table. subSeisanHissatuCheck00 and
 * subSeisanHissatuCheck01 each walk SeisanWork[6] entries of 0x3c bytes
 * with the count at +0x38 (addiu 0x002a1428/0x002a1478,
 * lw 0x002a142c/0x002a147c): Check00 returns 0x50 once any entry's
 * count is nonzero, Check01 returns 0x58 once one reaches 5 or more.
 */
typedef struct SeisanHissatuEntry {
    int characterNo;
    int experience;
    int techPoints;
    int skillPoints;
    int specialPoints;
    int level;
    int hp;
    int ep;
    int learnedIds[6];               /* +0x20: names fetched by tskSeisanStatus */
    int count;                      /* +0x38 in each 0x3c-byte result row */
} SeisanHissatuEntry;

typedef struct SeisanResultData {
    SeisanHissatuEntry entries[3];  /* Three rows: SeisanInit clears 180 bytes. */
} SeisanResultData;

/* The unit record read by SeisanCountInit2/SeisanCountMain: signed
 * halfword HP/EP at +0/+2 and a signed level byte at +0x14. */
typedef struct SeisanUnitRecord {
    short hp;
    short ep;
    unsigned char unmodeled_04[0x14 - 4];
    signed char level;
} SeisanUnitRecord;

/*
 * seisanRibbonTable (.rodata, 0x40 bytes): SeisanDisp copies it onto its stack with
 * the ldl/ldr/sdl/sdr unaligned-doubleword sequence at 0x002a1900..0x002a1974
 * and passes the copy's address to endPrintDirectRibbon (still asm, main/tu164
 * src/main/end_print.c); the fields it holds are not otherwise evidenced here.
 */
typedef struct SeisanRibbonData {
    unsigned int unmodeled_00[15];
} SeisanRibbonData;

typedef struct SeisanButtonSprite {
    unsigned char unmodeled_00[4];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int color;                      /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} SeisanButtonSprite;

/*
 * Prompt-button layout used by tskSeisanButton. Its original instructions
 * pass the message object at +0x08 and the sprite at +0x4c.
 */
typedef struct SeisanButtonWork {
    unsigned char unmodeled_00[4];
    int color;                      /* +0x04 */
    EMessageParam message;          /* +0x08 */
    SeisanButtonSprite sprite;      /* +0x4c */
} SeisanButtonWork;

typedef struct SeisanFadeVertex {
    short x;
    short y;
    int depth;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} SeisanFadeVertex;

/* SeisanFadeMain initializes four 12-byte vertices beginning at +0x04. */
typedef struct SeisanFadeData {
    unsigned char state;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    SeisanFadeVertex vertices[4];
    unsigned char unmodeled_34[4];
    int terminatorDepth;
    unsigned char unmodeled_3c[4];  /* SeisanInit advances 64 bytes after this block. */
} SeisanFadeData;




typedef struct SeisanTimeSprite {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int depth;
    unsigned char unmodeled_0c[0x28 - 0x0c];
} SeisanTimeSprite;

/* The time-panel callback embeds a 0x194-byte window, two 0x20-byte tags,
 * three 0x90-byte numbers and two 0x28-byte sprites. */
typedef struct SeisanTimePanel {
    unsigned char unmodeled_00[4];
    int depth;
    WindowDX window;
    ETagFontMainData tags[2];
    ENumberMainData numbers[3];
    SeisanTimeSprite sprites[2];
} SeisanTimePanel;

/* SeisanInit reserves 1428 bytes for the item panel: two 404-byte windows,
 * one 68-byte title message and eight 68-byte item messages. */
typedef struct SeisanItemPanel {
    unsigned char unmodeled_00;
    unsigned char active;
    unsigned char itemCount;
    unsigned char unmodeled_03;
    int depth;
    WindowDX windows[2];
    EMessageParam title;
    EMessageParam items[8];
} SeisanItemPanel;

typedef struct SeisanItemEntry {
    int itemId;
    int count;
} SeisanItemEntry;

typedef struct SeisanMoneyData {
    unsigned char unmodeled_00[0x10];
    int money;
    int itemTypes[8];                /* +0x14, written by monsGainSet */
    int itemIds[8];                  /* +0x34, low halfword read by SeisanInit */
    int itemCount;
} SeisanMoneyData;

typedef struct SeisanStatusProfile {
    signed char mode;
    unsigned char flags;
    short characterNo;
    short x;
    short y;
    unsigned char unmodeled_08[0x840 - 8];
} SeisanStatusProfile;

/* The 20-byte sprite record consumed by endSpriteSet/PrintSprite. */
typedef struct SeisanStatusSprite {
    short x;
    short y;
    int depth;
    short width;
    short height;
    unsigned char color[4];
    short uv;
    unsigned char column;
    unsigned char row;
} SeisanStatusSprite;

/* MenuStatusDisp receives the window and this embedded 0x840-byte work
 * block. The prefix is initialized at +420 by tskSeisanStatus. */
typedef struct SeisanStatusPanel {
    unsigned char state;
    unsigned char displayFlags;
    unsigned char enabled;
    unsigned char partySlot;
    unsigned char learnedVisible;
    unsigned char unmodeled_05[3];
    unsigned short previousLevel;
    unsigned short levelFlashTimer;
    int depth;
    WindowDX window;
    SeisanStatusProfile profile;
    ENumberMainData numbers[10];
    ETagFontMainData tags[4];
    EMessageParam learnedTitle;
    EMessageParam learned[6];
    SeisanFadeVertex separator[3];
    SeisanStatusSprite sprites[14];
    SeisanFadeVertex highlight[5];
} SeisanStatusPanel;

typedef struct SeisanRewardSnapshot {
    int techPoints;
    int skillPoints;
    int specialPoints;
    int experience;
} SeisanRewardSnapshot;

typedef unsigned int SeisanQuadword __attribute__((mode(TI)));

typedef struct SeisanRenderPrefix {
    unsigned char unmodeled_00[0x12];
    unsigned short height;
    unsigned short flipBase;
} SeisanRenderPrefix;

typedef struct SeisanItemCategoryTable {
    short entries[4];
} SeisanItemCategoryTable;

/* A five-byte glyph label template: two two-byte glyphs and a terminator. */
typedef struct SeisanLabelBytes {
    unsigned char bytes[5];
} SeisanLabelBytes;

/* A printed glyph label: the template is copied in, then one glyph replaced. */
typedef union SeisanGlyphText {
    SeisanLabelBytes label;
    unsigned short glyphs[3];
    char text[6];
} SeisanGlyphText;

/*
 * The settlement (seisan) screen's control block. subSeisanMain (0x002a15c0)
 * drives it as a state machine:
 *   state (+1): subSeisanMain copies requestedState (+3) into it when they
 *       differ (lbu 0x3/lbu 0x1/sb 0x1 at 0x002a15cc..0x002a15dc), and
 *       subSeisanCheck and SeisanMain switch on or test it (0x002a14d0,
 *       0x002a18d8).
 *   flags (+2): bit 0 is set on the frame the state changes and cleared
 *       otherwise (0x002a15e4..0x002a15f4); subSeisanCheck sets bit 2 when no
 *       check selected a state of its own.
 *   nextState (+4): subSeisanCheck stores each check's result here
 *       (sb 0x4 at 0x002a1524..0x002a1584) and subSeisanMain copies it into
 *       requestedState (lbu 0x4/sb 0x3 at 0x002a170c/0x002a1714).
 *   transitionTicks (+5): reloaded on each state change and counted down by
 *       subSeisanMain.
 *   partySize (+6): incremented by SeisanInit for each party slot with a
 *       character.
 */
extern SeisanControlState *SeisanWork;

extern SeisanCountData SeisanCN;
extern SeisanResultData *SeisanResult;
extern const SeisanRibbonData seisanRibbonTable;
extern XglTaskScheduler *SeisanBgTask;
extern XglTaskScheduler *SeisanTask;

extern SeisanFadeData *SeisanFade;
extern SeisanItemEntry *SeisanItemBox;
extern const char *msg00_2_0036DAD0[];
extern const unsigned short *msg_number[10];
extern const SeisanLabelBytes seisanOrdinalPrefixText[];
extern const SeisanLabelBytes seisanQuantityMarkText[];
extern SeisanMoneyData *SeisanMg;
extern const char *msg00_0_0036DAB8[3];
extern const char combatantTitleText[];
extern const char *msg00_3_0036DAD8[];
extern const char *msg01_4_0036DAE8[];
extern SeisanRenderPrefix sRender;
extern unsigned int GameLoopState[];
extern const SeisanItemCategoryTable seisanItemCategoryTable[];

void tskTskMain2(TskObject *task);
int subSeisanHissatuCheck00(SeisanControlState *seisanWork);
int subSeisanHissatuCheck01(void);
void subSeisanMain(void);
extern SeisanCharRecord *func_A19210(int chrNo);
extern SeisanUnitRecord *func_A191C0(int chrNo);
extern void endPrintDirectRibbon(SeisanRibbonData *ribbon);

extern int MenuLoadSync(void);
extern void MenuBgTaskMain(void);
void SeisanFadeMain(void);
extern void MoveSlide(short *current, short *target, float rate);
extern void eSpriteSet(void *sprite, short spriteId);
extern void eSpriteMain(void *sprite);
extern void MenuBgTaskBreak(void);
void SeisanCountMain(int mode);
extern unsigned int func_A22908(void);
extern void PartyTimeDispChange(XglClock *clock);
/* SeisanInit allocates these objects consecutively after xglTaskInitial. */
/* SQ masks the low four address bits. The containing storage block is aligned
 * explicitly before typed access; SeisanWork retains the control-byte address. */
const struct BattleExperienceAndParameters *func_A19338(int tableId);

extern void endSpriteSet(void *sprite, int flags);
extern MenuTextEntry *func_A2C7D8(int abilityId);
extern void *MenuWorkEndGet(void);
extern unsigned char *MenuBgTaskInit(void *work, int cameraId);
extern void *xglTaskInitial(void *pool, int capacity, int flags);
extern void func_A17C08(SeisanResultData *result);
extern SeisanMoneyData *func_00A0F110(void);
extern void MenuLoadInit(void);
extern void MenuFontLoad(int font, int option);
extern int WindowTexLoad(unsigned char *buffer, unsigned int request);
extern void GameResourcePreLoad(int state);
extern void MenuWorkEndCheck(void *workEnd);

#endif /* SRC_MAIN_SEISAN_COUNT_INIT1_H */
