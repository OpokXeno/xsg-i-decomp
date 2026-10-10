/*
 * TU-local declarations of main/tu178 (src/main/menu_skill.c).
 */

#ifndef SRC_MAIN_MENU_SKILL_H
#define SRC_MAIN_MENU_SKILL_H

int SkillCharSkillLvGet(int character_id);

int SkillCharPointPlus(int character_id, int point_delta);

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

extern void *dataPlChaGet(unsigned int character_id);

/* Populated with the loaded skill table by MenuSkill (INCLUDE_ASM below). */
extern unsigned char *SkillDataBuf;

/*
 * The skill-data block starts with one byte per skill ID (the level getter
 * indexes ID - 1). The point-cost getter addresses halfwords from byte offset
 * 0x7e using the ID itself; SkillNextLvGet reads the following halfword table
 * at 0x17e, bounding this cost table at 0x80 entries.
 */
typedef struct MenuSkillData {
    unsigned char level_cap_by_skill_id[0x7e];
    unsigned short point_cost_by_skill_id[0x80];
} MenuSkillData;

extern unsigned short SkillNextLvGet(int level);

extern void xglSoundEffectNormalID(int sound_id, int variant);


/* Skill-menu screen records and controller fields read by this TU. */
typedef struct SkillPasWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    signed char state;
    unsigned char unmodeled_11[3];
    void (*callback)(void);
    void *callbackArg;
    unsigned char unmodeled_1c[0x194 - 0x1c];
} SkillPasWindow;

typedef struct SkillPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x44 - 0x0c];
} SkillPasMessage;

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
    SkillPasWindow window;
    SkillPasMessage message[3];
    unsigned char unmodeled_268[0x2ac - 0x268];
    SkillPasBox box;
    unsigned char unmodeled_2b8[8];
    unsigned char callbackWork[1]; /* Only its starting address is used here. */
} SkillPasWork;

int MenuPasLengthGet(const char *text);

void MenuPasWindow(void);

void endPrintExtFunc(int color, int operation, void *box);

extern SkillPasWork *MenuSkillPas;

extern const char *msg_0_0036DD58[];

typedef struct MenuSkillWorkState {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x10 - 0x04];
    unsigned char char_denied;      /* +0x10 */
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
    unsigned char list_accessory;   /* +0x44 */
    unsigned char unmodeled_45[0x50 - 0x45];
    int list_select;                /* +0x50 */
    unsigned char unmodeled_54[0x80 - 0x54];
} MenuSkillWorkState;

extern MenuSkillWorkState MenuWork;

/* Button state fields of the PadData record owned by xgl_pad. */
typedef struct SkillPadData {
    unsigned char unmodeled_00[0x2A];
    unsigned short pressed;
    unsigned char unmodeled_2c[0x34 - 0x2C];
    unsigned short repeat;
} SkillPadData;
extern SkillPadData PadData;

void MoveSlide(short *current, short *target, float rate);

void eSpriteSet(void *sprite, short spriteId);

void eSpriteMain(void *sprite);

#define PAD_L1 0x0004

#define PAD_R1 0x0008

#define PAD_RIGHT 0x2000

#define PAD_LEFT 0x8000

struct SkillSetWindow;
struct SkillMenuList;

typedef struct SkillSetWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    unsigned char state;
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*select)(struct SkillSetWindow *, struct SkillMenuList *); /* +0x14 */
    void *selectArg;                    /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
} SkillSetWindow;

typedef struct SkillSetMessage {
    unsigned char unmodeled_00;
    unsigned char mode;                 /* +0x01 */
    unsigned char unmodeled_02[2];
    short x;                            /* +0x04 */
    short y;                            /* +0x06 */
    int color;                          /* +0x08 */
    unsigned char unmodeled_0c[0x0C];
    const char *text;                   /* +0x18 */
    unsigned char unmodeled_1c[0x44 - 0x1C];
} SkillSetMessage;

void WindowDXSet(void *window);

void WindowDXMain(void *window);

void eMessageSet(void *message, const char *text);

void eMessageMain(void *message);

typedef struct SkillUnitOrg {
    unsigned char unmodeled_00[0xA6];
    short setSkill[3];                  /* +0xA6 */
} SkillUnitOrg;

typedef struct SkillMenuText {
    char *text;                     /* +0x00 */
    char *description;              /* +0x04 */
} SkillMenuText;

extern SkillUnitOrg *func_A191C0(int characterNo);

SkillMenuText *MenuTextGet(int textId);

typedef struct SkillRecord {
    unsigned char unmodeled_00[0x0E];
    unsigned short prerequisite;   /* +0x0E */
    unsigned char unmodeled_10[0x04];
    int pointLimit;                /* +0x14 */
} SkillRecord;

extern SkillRecord *func_A1A548(int id);

extern int func_A19698(int characterNo, unsigned short skillId);

typedef struct SkillStatusHeader {
    unsigned char state;                /* +0x00 */
    unsigned char cursorOn;             /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    unsigned char moveVert;             /* +0x04: slide y before x */
    unsigned char flags;                /* +0x05: bit 0 = the slide finished */
    unsigned char subState;             /* +0x06 */
    signed char lastCursor;             /* +0x07 */
    int color;                          /* +0x08 */
} SkillStatusHeader;

typedef struct SkillStatusRecord {
    unsigned char chrNo;                /* +0x00 */
    unsigned char fullStatus;           /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short offsetX;                      /* +0x04 */
    short offsetY;                      /* +0x06 */
    unsigned char unmodeled_08[0x834 - 0x08];
} SkillStatusRecord;

void MenuStatusDisp(SkillStatusRecord *record);

typedef struct SkillStatusWindow {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
    char *title;                        /* +0x0C */
    signed char state;                  /* +0x10: 1 while opening, 3 when open */
    unsigned char visible;              /* +0x11 */
    unsigned char unmodeled_12[0x14 - 0x12];
    void (*disp)(SkillStatusRecord *record);  /* +0x14 */
    SkillStatusRecord *dispWork;        /* +0x18 */
    unsigned char unmodeled_1c[0x188 - 0x1C];
} SkillStatusWindow;

typedef struct SkillStatusCursor {
    signed char state;                  /* +0x00 */
    unsigned char unmodeled_01[0x04 - 0x01];
    short x;                            /* +0x04 */
    short y;                            /* +0x06 */
    int work;                           /* +0x08 */
    unsigned char unmodeled_0c[0x10 - 0x0C];
} SkillStatusCursor;

typedef struct SkillStatusWindowEntry {
    SkillStatusHeader header;           /* +0x0000 */
    SkillStatusWindow window;           /* +0x000C */
} SkillStatusWindowEntry;

typedef struct SkillStatusRecordEntry {
    SkillStatusHeader header;           /* +0x0000 */
    SkillStatusRecord work;             /* +0x000C */
} SkillStatusRecordEntry;

typedef struct SkillStatusCursorEntry {
    SkillStatusHeader header;           /* +0x0000 */
    SkillStatusCursor cursor;           /* +0x000C */
    unsigned char unmodeled_1c[0x24 - 0x1C];
} SkillStatusCursorEntry;

typedef struct MenuSkillStatusWork {
    SkillStatusWindowEntry window[8];   /* +0x0000 */
    SkillStatusRecordEntry disp[8];     /* +0x0CA0 */
    SkillStatusCursorEntry cursor[2];   /* +0x4EA0 */
    SkillStatusHeader endHeader;        /* +0x4EE8 */
} MenuSkillStatusWork;

extern MenuSkillStatusWork *MenuSkillStatus;

extern short MenuSkillParty[];

char *MenuTagTextGet(int kind);

void eCursolSet(void *cursor, int width);

void eCursolMain(void *cursor);

typedef struct SkillMenuList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt;           /* +0x02 */
    int cursor;                         /* +0x04 */
    const char *items;                  /* +0x08 */
    const char *prompt;                 /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
} SkillMenuList;

typedef struct MenuSkillMenuWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[0x02];
    unsigned char subState;             /* +0x003 */
    int color;                          /* +0x004 */
    SkillSetWindow window[2];           /* +0x008 */
    SkillMenuList list[2];              /* +0x330 */
} MenuSkillMenuWork;

extern MenuSkillMenuWork *MenuSkillMenu;

void MenuSelectWindow(SkillSetWindow *window, SkillMenuList *list);

extern const char D_004DB610[];

extern const char *msg00_1_0036DD68[];

extern const char *msg01_2_0036DD70[];

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

extern MenuSkillL1R1Work *MenuSkillL1R1;

typedef struct SkillL1R1SpriteIdPair {
    short id[2];
} SkillL1R1SpriteIdPair;

extern const SkillL1R1SpriteIdPair D_004DB628[];

typedef struct SkillL1R1SlideStep {
    signed char side[2];
} SkillL1R1SlideStep;

extern const SkillL1R1SlideStep D_004DB630[];

typedef struct MenuSkillSetListWork {
    unsigned char state;                /* +0x00 */
    unsigned char unmodeled_01[3];
    int color;                          /* +0x04 */
    SkillSetWindow window;              /* +0x08 */
    SkillSetMessage message[4];         /* +0x19C */
} MenuSkillSetListWork;

extern MenuSkillSetListWork *MenuSkillSetList;

extern const char D_004C99F8[];

extern const char *msg00_4_0036DD88[];

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
    SkillCategoryIcon icons[5];       /* +0x10 */
} MenuSkillCategoryWork;

extern MenuSkillCategoryWork *MenuSkillCategory;

typedef struct SkillCategoryIconIds {
    short id[5];
} SkillCategoryIconIds;

extern const SkillCategoryIconIds D_004C9A08;

typedef struct SkillCategoryEdgeStep {
    signed char side[2];
} SkillCategoryEdgeStep;

extern const SkillCategoryEdgeStep D_004DB638[];

float xglCos(float radians);

extern unsigned char MenuKeepSelect[0x64];

typedef struct SkillListRow {
    unsigned char unmodeled_00[0x04];
    int value;                     /* +0x04 */
    unsigned char locked;          /* +0x08 */
    signed char level;             /* +0x09 */
    unsigned char unmodeled_0a[0x0C - 0x0A];
} SkillListRow;

typedef struct SkillSPWindow {
    unsigned char state;
    unsigned char rowCount;        /* +0x01 */
    unsigned char unmodeled_02[0x0C - 0x02];
    short width;                   /* +0x0C */
    short height;                  /* +0x0E */
    const char *title;             /* +0x10 */
    unsigned char columns;         /* +0x14 */
    unsigned char rows;            /* +0x15 */
    unsigned char unmodeled_16[0x1C - 0x16];
    SkillListRow *items;           /* +0x1C */
    unsigned char unmodeled_20[0x26 - 0x20];
    unsigned char cursorStyle;     /* +0x26 */
} SkillSPWindow;

typedef struct MenuSkillListWork {
    unsigned char state;
    unsigned char unmodeled_01[0x0C - 1];
    SkillSPWindow window;          /* +0x0C */
} MenuSkillListWork;

extern MenuSkillListWork *MenuSkillList;

int *MenuSortAddrGet(int listIndex);

SkillListRow *MenuListGet(int listIndex);

SkillListRow *MenuListMake(int listIndex, int mode);

int MenuSortCheck(int listIndex);

void MenuSortSet(int listIndex, int type, int order);

int MenuSkillEquipCheck(int characterNo, int skillId);

void WindowSPItemChange(SkillSPWindow *window);

void WindowSPSetSelect(SkillSPWindow *window, unsigned char *savedSelection);

int WindowSPSelect(SkillSPWindow *window, int input);

extern const char D_004DB640[];

extern const char D_004C9A18[];

static int MenuSkillListChange00(void);
static int MenuSkillListChange01(void);

typedef struct MenuSkillAttackSlot {
    unsigned short party_id;
    signed char attack_position;
} MenuSkillAttackSlot;

typedef struct MenuSkillPartyData {
    unsigned char unmodeled_00[0x30];
    MenuSkillAttackSlot attack_slots[3]; /* +0x30 */
} MenuSkillPartyData;

#define MENU_FLAG_ENTER 0x01
#define MENU_FLAG_DRAW 0x02

typedef struct SkillScreenState { unsigned char state; } SkillScreenState;

extern const char D_004C9A88[];

extern unsigned char *MainMenuWorkEnd;

#define SKILL_DATA_SIZE      0x800

#define SKILL_PAS_SIZE       0x330

#define SKILL_INFO_SIZE      0x3D0

#define SKILL_STATUS_SIZE    0x4EF4

#define SKILL_MENU_SIZE      0xF20

#define SKILL_EX_SIZE        0x28C

#define SKILL_L1R1_SIZE      0x58

#define SKILL_SET_LIST_SIZE  0x2AC

#define SKILL_CATEGORY_SIZE  0xDC

extern SkillScreenState *MenuSkillInfo;

extern SkillScreenState *MenuSkillEx;

extern short MenuSkillParty[8];

#define KEEP_EQUIP 4

#define SE_DECIDE 1

#define SE_CANCEL 2

#define SE_CURSOR 3

#define SE_BUZZER 5

#define SE_LEARN  22

#define PAD_L1     0x0004

#define PAD_R1     0x0008

#define PAD_CIRCLE 0x0020

#define PAD_CROSS  0x0040

#define PAD_SQUARE 0x0080

#define PAD_UP     0x1000

#define PAD_RIGHT  0x2000

#define PAD_DOWN   0x4000

#define PAD_LEFT   0x8000

#define SKILL_CHARACTER 0x10

#define SKILL_COMMAND   0x20

#define SKILL_LEARN     0x30

#define SKILL_CONFIRM   0x32

#define SKILL_EXTRACT   0x34

#define SKILL_EQUIP     0x40

#define SKILL_LEAVE     0xF0

#define SKILL_EXIT      0xFF

void MenuSkillPasMain(void);

void MenuSkillInfoMain(void);

void MenuSkillStatusMain(void);

void MenuSkillMenuMain(void);

void MenuSkillExMain(void);

void MenuSkillL1R1Main(void);

void MenuSkillSetListMain(void);

void MenuSkillCategoryMain(void);

void MenuSkillListMain(void);

int PartyFriendLockCheck(int id, int flag);

int PartyAttackerCheck(int id);

int MenuMaryIdChange(int id);

int MenuMainCharCheck(int character_id);

void MenuKeepSelectReset(void);

int MenuCursorKeepCheck(void);

signed char MenuSelectMove(int cursor, int count, int wrap);

int MenuSortGet(int list_index, int entry_index);

int MenuSortCheck(int list_index);

int MenuSkillEquipCheck(int character_id, int accessory);

void MenuSkillEquip(int character_id, int accessory, int slot);

void ChangeTopLevel(int level);

int WindowSPSelect(SkillSPWindow *window, int pad);

void WindowSPKeepSelect(SkillSPWindow *window, unsigned char *keep);

void WindowSPKeepSelectCheck(unsigned char *keep);

void WindowSPSelectJump(SkillSPWindow *window, int row);

int xglCdReadFile(const char *name, void *buffer, int offset, int mode);

void xglFontDebugPrintf(int x, int y, const char *format, ...);

void func_A19600(int character_id, int skill_id);

#endif /* SRC_MAIN_MENU_SKILL_H */
