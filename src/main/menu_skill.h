/*
 * TU-local declarations of main/tu178 (src/main/menu_skill.c).
 */

#ifndef SRC_MAIN_MENU_SKILL_H
#define SRC_MAIN_MENU_SKILL_H

#include "main/menu_1.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/menu_skill.h"
#include "main/menu_top_command_check.h"
#include "main/open_close_main.h"
#include "main/party.h"
#include "main/window_tex_load.h"
#include "main/xgl_2.h"
#include "main/xgl_cd.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#define MENU_FLAG_DRAW 0x02
#define SKILL_DATA_SIZE      0x800
#define SKILL_PAS_SIZE       0x330
#define SKILL_INFO_SIZE      0x3D0
#define SKILL_STATUS_SIZE    0x4EF4
#define SKILL_MENU_SIZE      0xF20
#define SKILL_EX_SIZE        0x28C
#define SKILL_L1R1_SIZE      0x58
#define SKILL_SET_LIST_SIZE  0x2AC
#define SKILL_CATEGORY_SIZE  0xDC
#define KEEP_EQUIP 4
#define SE_LEARN  22
#define SKILL_CHARACTER 0x10
#define SKILL_COMMAND   0x20
#define SKILL_LEARN     0x30
#define SKILL_CONFIRM   0x32
#define SKILL_EXTRACT   0x34
#define SKILL_EQUIP     0x40
#define SKILL_LEAVE     0xF0
#define SKILL_EXIT      0xFF

/* Skill record limits. */
#define SKILL_CHARACTER_ID_END 9 /* first character id that has no skill data */
#define SKILL_LEVEL_MAX 5        /* a character's skill level is 0..4 until it reaches 5 */
#define SKILL_ALIGN 0x800        /* alignment of the skill data block */

/* Screen layout. */
#define SKILL_PARTY_MAX 8        /* party member slots of the status panels */
#define SKILL_PAS_MESSAGE_COUNT 3 /* title, extract and set captions */
#define SKILL_MENU_PANEL_COUNT 2 /* command window and extract confirmation */
#define SKILL_LIST_ROWS 8        /* visible rows of the skill list */
#define SKILL_ROW_HEIGHT 24
#define SKILL_COMMAND_COUNT 3    /* learn, equip, back */
#define SKILL_CONFIRM_CHOICE_COUNT 2 /* yes, no */
#define SKILL_LIST_TAB_COUNT 3   /* tabs of the learn list (MenuWork.mode) */
#define SKILL_KEEP_SLOT_SIZE 5   /* bytes of MenuKeepSelect per list */
#define SKILL_OFFSCREEN_RIGHT 528 /* slide target that hides a window at the right */
#define SKILL_OFFSCREEN_BOTTOM 512 /* slide target that hides the info window */

/* MenuWork.wait frames. */
#define SKILL_WAIT_CHOICE 8      /* between command, list and confirmation */
#define SKILL_WAIT_OPEN 12       /* after the screen opens or returns to the character pick */
#define SKILL_WAIT_LEAVE 16
#define SKILL_WAIT_COMMAND 24    /* character picked: command window slides in */

/* Draw order keys (window style / work fields). */
#define SKILL_STYLE_WINDOW 0x00fffff0
#define SKILL_STYLE_STATUS 0x00FFF000
#define SKILL_STYLE_LIST 0x00fffff8
#define SKILL_STYLE_STATUS_SELECTED 0x7F00
#define SKILL_STYLE_STATUS_IDLE 0x7E00
#define SKILL_STYLE_SPRITE 0x00FFFFFF
#define SKILL_STYLE_ICON 0x00FFFFFE
#define SKILL_STYLE_LEVEL_NUMBER 0x00fffffa

/* Colour channels of sprites and text. */
#define SKILL_TINT_DIM 64
#define SKILL_TINT_BRIGHT (-128) /* 0x80 stored in a signed char */

/* Text id: type 7 (skill) in the high halfword, skill id in the low one. */
#define SKILL_TEXT_TYPE 0x80000

/* MenuSkillListWork.state besides SKILL_BLOCK_INIT / SKILL_BLOCK_RUN (idle). */
#define SKILL_LIST_LEARN_OPEN 10
#define SKILL_LIST_LEARN_IN 11   /* learn list sliding in */
#define SKILL_LIST_SHOWN 12      /* list on screen until the command menu returns */
#define SKILL_LIST_OUT 13        /* list sliding out */
#define SKILL_LIST_EQUIP_OPEN 20
#define SKILL_LIST_EQUIP_IN 21   /* equip list sliding in */

/* MenuSkillListWork.displayMode. */
#define SKILL_LIST_DISPLAY_NONE 0
#define SKILL_LIST_DISPLAY_LEARN 1 /* level numbers */
#define SKILL_LIST_DISPLAY_EQUIP 2 /* equipped marks */

/* Category icons: two arrows (L/R) and three category marks. */
#define SKILL_CATEGORY_ICON_COUNT 5
#define SKILL_CATEGORY_ARROW_COUNT 2
#define SKILL_ARROW_KICK 6        /* slide offset set when a shoulder or direction key is pressed */
#define SKILL_PULSE_MAX 8         /* pulseCounter when the icons are hidden */
#define SKILL_FLASH_MAX 11

/* MenuSkillStatusWork.flags */
#define SKILL_STATUS_FLAG_SLID 0x01 /* the selected panel finished sliding */

/* Block states shared by every skill-menu part. */
#define SKILL_BLOCK_INIT 0
#define SKILL_BLOCK_RUN 2


/*
 * The player character's persistent record, recovered head
 *
 * `dataPlChaGet` (ov01 0x00a19210) is the accessor: it multiplies the
 * character id by 0xa8 and indexes `plChaData` (main 0x00425ac0, witnessed
 * size 0xa80 = ten 0xa8-byte entries) from one entry below it, so id 1 is the
 * first entry. `dataPlUnitInit` (0x00a19988) memsets one whole 0xa8-byte entry
 * before filling it in (0x00a19ba8).
 *
 * The decisive witness for the names is the game's own debug page, the
 * character block of `debug_entry` (ov01 0x00a30420..0x00a30560), which reads
 * the entry `dataPlChaGet` just returned and prints each field through
 * `xglFontDebugPrintf` with its own caption in ov01 .rodata:
 *
 *   +0x00 exp            "\x0b EXP      = %8d"  D_00A50DF8, lw  0x00
 *   +0x04 next_exp       "\x0b NEXT EXP = %8d"  D_00A50E08, lw  0x04
 *   +0x08 wait_offset    "\x0b W OFS    = %4d"  D_00A50E18, lh  0x08
 *   +0x0a wait_count     "\x0b W CNT    = %4d"  D_00A50E28, lh  0x0a
 *   +0x0c tech_points    "\x0b TP       = %4d"  D_00A50E38, lw  0x0c
 *   +0x10 ether_points   "\x0b EP       = %4d"  D_00A50E48, lw  0x10
 *   +0x14 skill_points   "\x0b SP       = %4d"  D_00A50E58, lw  0x14
 *   +0x18 growth[0..3]   "\x0b GROW PARA=%4d %4d %4d %4d"  D_00A50E68,
 *   +0x20 growth[4..7]   "\x0b          =%4d %4d %4d %4d"  D_00A50E88,
 *                        lh 0x18/0x1a/0x1c/0x1e and lh 0x20/0x22/0x24/0x26
 *
 * `next_exp` and `growth` are corroborated by dataPlUnitInit, which seeds
 * +0x04 from the experience table at the character's starting level
 * (0x00a19c18) and writes the eight halfwords from +0x18 upwards out of the
 * parameter table in a loop (0x00a19c30..0x00a19c54). `tech_points`,
 * `ether_points` and `skill_points` are the three point pools the item code in
 * ov01 `calc.s` raises and caps at 9999 (0x00a11a28 raises SP, 0x00a11a78 EP,
 * 0x00a11ac8 TP), and `skill_points` is the pool SkillCharPointPlus below
 * spends.
 *
 * The type stops at +0x28. The entry is 0xa8 bytes and the rest of it is not
 * this TU's evidence: +0x28 is the learned-ether bitmap dataEtherLearnGet
 * indexes (0x00a195e0), +0x38 the learned-skill bitmap dataSkillLearnGet
 * indexes (0x00a19700), and dataPlUnitInit writes eight further halfwords
 * 0xc bytes apart from +0x4e to +0xa2 (0x00a19bc8). No member is invented to
 * reach the entry's size.
 */
typedef struct PlayerCharacter {
    int exp;              /* +0x00 */
    int next_exp;         /* +0x04 */
    short wait_offset;    /* +0x08 */
    short wait_count;     /* +0x0a */
    int tech_points;      /* +0x0c */
    int ether_points;     /* +0x10 */
    int skill_points;     /* +0x14 */
    short growth[8];      /* +0x18 */
} PlayerCharacter;

/*
 * The skill-data block starts with one byte per skill ID (the level getter
 * indexes ID - 1). The point-cost getter addresses halfwords from byte offset
 * 0x7e using the ID itself; SkillNextLvGet reads the following halfword table
 * at 0x17e, bounding this cost table at 0x80 entries.
 */
struct MenuSkillData {
    unsigned char level_cap_by_skill_id[0x7e];
    unsigned short point_cost_by_skill_id[0x80];
    unsigned short next_level_points[SKILL_LEVEL_MAX];  /* +0x17e: indexed by skill level 0..4 */
};

/* Skill-menu screen records and controller fields read by this TU. */
typedef struct SkillPasBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} SkillPasBox;

typedef struct SkillPasWork {
    unsigned char state;
    unsigned char unmodeled_01[3];
    int color;
    WindowDX window;
    EMessageParam message[SKILL_PAS_MESSAGE_COUNT];
    unsigned char unmodeled_268[0x2ac - 0x268];
    SkillPasBox box;
    unsigned char unmodeled_2b8[8];
    unsigned char callbackWork[1]; /* Only its starting address is used here. */
} SkillPasWork;

typedef struct MenuSkillWorkState {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x10 - 0x04];
    signed char char_denied;        /* +0x10 */
    unsigned char unmodeled_11[0x20 - 0x11];
    unsigned char flags;            /* +0x20 */
    unsigned char next_state;       /* +0x21 */
    unsigned char unmodeled_22[0x2A - 0x22];
    unsigned char wait;             /* +0x2A */
    unsigned char unmodeled_2b[0x30 - 0x2B];
    signed char commandCursor;        /* +0x30 */
    signed char mode;           /* +0x31 */
    signed char itemCursor;     /* +0x32 */
    unsigned char unmodeled_33[0x40 - 0x33];
    signed char characterNo;       /* +0x40 */
    signed char partyCount;        /* +0x41 */
    signed char partyCursor;       /* +0x42 */
    unsigned char unmodeled_43;
    signed char list_accessory;     /* +0x44 */
    unsigned char unmodeled_45[0x50 - 0x45];
    int list_select;                /* +0x50 */
    unsigned char unmodeled_54[0x80 - 0x54];
} MenuSkillWorkState;

/* Button state fields of the PadData record owned by xgl_pad. */
typedef struct SkillPadData {
    unsigned char unmodeled_00[0x2A];
    unsigned short pressed;
    unsigned char unmodeled_2c[0x34 - 0x2C];
    unsigned short repeat;
} SkillPadData;

typedef struct SkillMenuList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt;           /* +0x02 */
    int cursor;                         /* +0x04 */
    const char *items;                  /* +0x08 */
    const char *prompt;                 /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
} SkillMenuList;

typedef struct SkillUnitOrg {
    unsigned char unmodeled_00[0xA6];
    short setSkill[3];                  /* +0xA6 */
} SkillUnitOrg;

typedef struct SkillRecord {
    unsigned char unmodeled_00[0x0E];
    unsigned short prerequisite;   /* +0x0E */
    unsigned char unmodeled_10[0x04];
    int pointLimit;                /* +0x14 */
} SkillRecord;

/* The party status panels of the skill menu: one WindowDX per member whose
 * close callback MenuStatusDisp draws the member's MenuStatusPanel, plus the
 * selection cursor (SKILL_STATUS_SIZE = 0x4EF4). */
typedef struct MenuSkillStatusWork {
    unsigned char state;                /* +0x0000 */
    unsigned char cursorOn;             /* +0x0001 */
    unsigned char unmodeled_02[0x04 - 0x02];
    unsigned char moveVert;             /* +0x0004: slide y before x */
    unsigned char flags;                /* +0x0005: bit 0 = the slide finished */
    unsigned char subState;             /* +0x0006 */
    signed char lastCursor;             /* +0x0007 */
    int color;                          /* +0x0008 */
    WindowDX window[SKILL_PARTY_MAX];                 /* +0x000C */
    MenuStatusPanel panel[SKILL_PARTY_MAX];           /* +0x0CAC */
    ECursolMainData cursor[2];          /* +0x4EAC */
} MenuSkillStatusWork;

typedef struct MenuSkillMenuWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[0x02];
    unsigned char subState;             /* +0x003 */
    int color;                          /* +0x004 */
    WindowDX window[SKILL_MENU_PANEL_COUNT];                 /* +0x008 */
    SkillMenuList list[SKILL_MENU_PANEL_COUNT];              /* +0x330 */
} MenuSkillMenuWork;

typedef struct SkillL1R1Sprite {
    unsigned char unmodeled_00[0x04];
    short x;                 /* +0x04 */
    short y;                 /* +0x06 */
    int work;                /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} SkillL1R1Sprite;

typedef struct MenuSkillL1R1Work {
    unsigned char state;         /* +0x00 */
    unsigned char unmodeled_01;
    signed char slideOffset[2];  /* +0x02 */
    int spriteWork;              /* +0x04 */
    SkillL1R1Sprite sprite[2];   /* +0x08: L1, then R1 */
} MenuSkillL1R1Work;

typedef struct SkillL1R1SpriteIdPair {
    short id[2];
} SkillL1R1SpriteIdPair;

typedef struct SkillL1R1SlideStep {
    signed char side[2];
} SkillL1R1SlideStep;

typedef struct MenuSkillSetListWork {
    unsigned char state;                /* +0x00 */
    unsigned char unmodeled_01[3];
    int color;                          /* +0x04 */
    WindowDX window;                    /* +0x08 */
    EMessageParam message[4];           /* +0x19C */
} MenuSkillSetListWork;

typedef struct SkillCategorySprite {
    unsigned char unmodeled_00[0x04];
    short x;                          /* +0x04 */
    short y;                          /* +0x06 */
    int work;                         /* +0x08 */
    signed char color[3];             /* +0x0C */
    unsigned char alpha;              /* +0x0F */
    unsigned char unmodeled_10[0x24 - 0x10];
} SkillCategorySprite;

typedef struct SkillCategoryIcon {
    unsigned short flashLevel;        /* +0x00 */
    unsigned short flashStep;         /* +0x02 */
    SkillCategorySprite sprite;       /* +0x04 */
} SkillCategoryIcon;

typedef struct MenuSkillCategoryWork {
    unsigned char state;              /* +0x00 */
    unsigned char phase;              /* +0x01 */
    unsigned char pulseCounter;       /* +0x02 */
    unsigned char tickCounter;        /* +0x03 */
    int pendingReset;                 /* +0x04 */
    signed char edgeDelta[2];         /* +0x08 */
    unsigned char unmodeled_0a[0x0C - 0x0A];
    int iconResetWork;                /* +0x0C */
    SkillCategoryIcon icons[SKILL_CATEGORY_ICON_COUNT];       /* +0x10 */
} MenuSkillCategoryWork;

typedef struct SkillCategoryIconIds {
    short id[SKILL_CATEGORY_ICON_COUNT];
} SkillCategoryIconIds;

typedef struct SkillCategoryEdgeStep {
    signed char side[2];
} SkillCategoryEdgeStep;

/* Two ribbons, tag labels and number displays form the extraction screen. */
typedef struct MenuSkillExWork {
    unsigned char state;
    unsigned char unmodeled_01[3];
    int color;
    ERibbonMainData ribbon[2];
    ETagFontMainData tag[2];
    ENumberMainData number[2];
    EMessageParam message;
} MenuSkillExWork;

typedef struct MenuSkillListWork {
    unsigned char state;
    unsigned char visible;
    unsigned char resetPending;
    unsigned char displayMode;
    int color;
    unsigned char unmodeled_08[4];
    WindowSPCursor window;          /* +0x0C */
    unsigned char unmodeled_1744[0x5cec - 0x1744];
    SkillL1R1Sprite equipped[3];
    unsigned char unmodeled_5d64[0x5e2c - 0x5d64];
    ENumberMainData levelNumber[SKILL_LIST_ROWS];
} MenuSkillListWork;


typedef struct MenuStatusPanelWindow MenuStatusPanelWindow;

typedef struct MenuTopInfoPanel MenuTopInfoPanel;

typedef struct MenuSkillInfoText {
    short x;
    short y;
    unsigned char unmodeled_04[8];
    char *text;
    unsigned char unmodeled_10[0x1B0 - 0x10];
} MenuSkillInfoText;

typedef struct MenuSkillInfoWork {
    unsigned char state;
    unsigned char opened;
    signed char selected;
    unsigned char reasonShown;
    int color;
    WindowDX window;                /* +0x008 */
    unsigned char unmodeled_19c[4];
    MenuSkillInfoText info;         /* +0x1A0 */
    char message[128];
} MenuSkillInfoWork;

extern SkillPasWork *MenuSkillPas;
extern const char *msg_0_0036DD58[];
extern MenuSkillWorkState MenuWork;
extern SkillPadData PadData;
extern MenuSkillStatusWork *MenuSkillStatus;
extern short MenuSkillParty[];
extern MenuSkillMenuWork *MenuSkillMenu;
extern const char skillMenuText[];
extern const char *msg00_1_0036DD68[];
extern const char *msg01_2_0036DD70[];
extern MenuSkillL1R1Work *MenuSkillL1R1;
extern const SkillL1R1SpriteIdPair skillL1R1SpriteTable[];
extern const SkillL1R1SlideStep skillL1R1SlideStep[];
extern MenuSkillSetListWork *MenuSkillSetList;
extern const char skillSetListText[];
extern const char *msg00_4_0036DD88[];
extern MenuSkillCategoryWork *MenuSkillCategory;
extern const SkillCategoryIconIds skillCategoryIconTable;
extern const SkillCategoryEdgeStep skillCategoryEdgeStep[];
extern unsigned char MenuKeepSelect[0x64];

/* Three pointers in the original .data; the final message has mutable color bytes. */
extern const char *msg00_3_0036DD78[];

extern const char skillEmptyText[];
extern MenuSkillListWork *MenuSkillList;
extern const char skillSetText[];
extern const char skillListHeaderText[];
extern const char skillDataPath[];
extern unsigned char *MainMenuWorkEnd;
extern MenuSkillInfoWork *MenuSkillInfo;
extern MenuSkillExWork *MenuSkillEx;
extern short MenuSkillParty[SKILL_PARTY_MAX];

unsigned int SkillCharSkillLvGet(int character_id);
int SkillCharPointPlus(int character_id, int point_delta);
extern PlayerCharacter *dataPlChaGet(unsigned int character_id);
unsigned short SkillNextLvGet(int level);
void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);
extern SkillUnitOrg *func_A191C0(int characterNo);
extern SkillRecord *func_A1A548(int id);
extern int func_A19698(int characterNo, unsigned short skillId);
char *MenuTagTextGet(int kind);
void WindowSPSet(WindowSPCursor *window);
void WindowSPMain(WindowSPCursor *window);
int *MenuSortAddrGet(int listIndex);
MenuListEntry *MenuListGet(int listIndex);
MenuListEntry *MenuListMake(int listIndex, int mode);
int MenuSortCheck(int listIndex);
int MenuSkillEquipCheck(int characterNo, int skillId);
static int MenuSkillListChange00(void);
static int MenuSkillListChange01(void);
void MenuSkillPasMain(void);
void MenuSkillInfoMain(void);
void MenuSkillStatusMain(void);
void MenuSkillMenuMain(void);
void MenuSkillExMain(void);
void MenuSkillL1R1Main(void);
void MenuSkillSetListMain(void);
void MenuSkillCategoryMain(void);
void MenuSkillListMain(void);
int PartyAttackerCheck(int id);
int MenuCursorKeepCheck(void);
int MenuSortGet(int list_index, int entry_index);
void MenuSkillEquip(int character_id, int accessory, int slot);
void WindowSPSelectJump(WindowSPCursor *window, int row);
void func_A19600(int character_id, int skill_id);
char *MenuCharNameGet(int characterNo);
MenuTextEntry *func_A2C648(int skillId);

#endif /* SRC_MAIN_MENU_SKILL_H */
