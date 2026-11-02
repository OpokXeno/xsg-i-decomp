#ifndef INCLUDE_MAIN_WINDOW_TEX_LOAD_H
#define INCLUDE_MAIN_WINDOW_TEX_LOAD_H

#include "main/xgl_studio.h"
#include "main/xgl_2.h"

/*
 * The UMN database's saved state: the 0x9c-byte window of the save record
 * that build/main/aliases.ld names `UmnDataBaseStateData = SaveData + 66140`
 * (0x1025c).  Two bit sets of the same 0x1d monster ids (ids 0x22..0x3e, the
 * `monster_id - 0x22` and `< 0x1d` test both functions of this pair share)
 * are evidenced, each read and written one byte at a time as
 * `bits[id / 8] & (1 << (id % 8))`:
 * - +0x88, whether the monster has been met: written by
 *   UmnDataBaseMonsterSet (main:0x002761f0, `lbu/sb 0x0($5)` with
 *   $5 = base + id/8 + 0x88) and read by UmnDataBaseMonsterCheck
 *   (main:0x00276248, `lbu $2,0x88($3)`).
 * - +0x98, whether it has been scanned: the same pair of functions one
 *   displacement further on, UmnDataBaseAnalisisSet (main:0x00276298,
 *   `addiu $5,$3,0x98`) and UmnDataBaseAnalisisCheck (main:0x002762f0,
 *   `lbu $2,0x98($3)`), which this candidate recovers as C.  It is what the
 *   0x9c extent of the published UmnDataBaseStateData declaration ends on.
 * The spans around them stay unmodeled byte ranges with no meaning claimed
 * (docs/naming.md), and this is what was searched to leave them so. Every
 * access through this window's base in main and the overlays was enumerated:
 * the head (+0x00, +0x02, +0x03, +0x10..+0x12, +0x2a, +0x31, +0x56, +0x59,
 * +0x84, +0x86) belongs to the UMN mail and plugin screens (UmnMail,
 * UmnTopMenu, UmnMailBoxSet, UmnMailFolderSet, UmnMailAttachSet, UmnPlugin,
 * Java_xeno_util_Runtime_mailFlag), which this allocation does not recover,
 * and +0x8c..+0x97 is touched by nothing at all. The nearby +0x10254,
 * +0x102dc, +0x102e2 and +0x10304 accesses (MenuSaveDataGet, MenuBoxMoneyGet,
 * tyaUmlDispLoad, UmnMailDataGet) address the save record directly rather
 * than this alias, so they witness neighbours of the window, not members.
 */
typedef struct UmnDataBase {
    unsigned char _unmodeled_00[0x88];      /* +0x00..+0x87 */
    unsigned char monster_discovered[4];    /* +0x88: one bit per monster id */
    unsigned char _unmodeled_8c[0x0c];      /* +0x8c..+0x97 */
    unsigned char monster_analysed[4];      /* +0x98: one bit per monster id */
} UmnDataBase;

/* Top-level screens: MenuWork.menu_state[1] and the argument of ChangeTopLevel
 * (the TopMenu command table maps its rows onto these). */
enum {
    MENU_LEVEL_TOP = 0,
    MENU_LEVEL_ITEM = 1,
    MENU_LEVEL_ETHER = 2,
    MENU_LEVEL_CHARACTER = 3,
    MENU_LEVEL_AGWS = 4,
    MENU_LEVEL_FORMATION = 5,
    MENU_LEVEL_UMN = 6,
    MENU_LEVEL_SYSTEM = 7,
    MENU_LEVEL_TEC = 9,
    MENU_LEVEL_SKILL = 10,
    MENU_LEVEL_EXIT_REQUEST = 12, /* leave the menu with no screen to run */
    MENU_LEVEL_QUIT = 0xFF        /* MainMenu's loop ends */
};

void *MenuWorkEndGet(void);
void UmnMailBoxSet(int box_id);
unsigned char *UmnMailDataGet(int box_id);
void MoveSlide(short *current, short *target, float rate);
int MenuLoadSync(void);
void MenuLoadInit(void);
void MenuLoadCancel(void);
extern unsigned char *MenuBgTaskInit(unsigned char *, int);
extern void MenuBgTaskBreak(void);
void MenuWorkEndCheck(void);
void ChangeTopLevel(int top_level);
void MenuKeepSelectReset(void);
int MenuSelectMove(int selection, int count, int wrap);
void MenuFontLoad(int original_font, int asynchronous);
void MenuCfTaikiPush(void);
void MenuCfTaikiPop(void);
void MenuBgTaskMain(void);

/*
 * MenuWork, the menu state block window_tex_load.c defines and menu_tec.c
 * drives. window_tex_load's view: menu_state[1] is the top-level state,
 * menu_state[3] and item_selection/sub_selection are its cursors, and
 * ChangeTopLevel clears the area from +0x10 on as seven quadwords. menu_tec's
 * view names the fields of that area. The state byte menu_tec calls `state` is
 * menu_state[3].
 */
typedef struct MenuWorkState {
    unsigned char menu_state[4];    /* +0x00 */
    unsigned char item_selection;   /* +0x04 */
    signed char sub_selection;      /* +0x05 */
    unsigned char unmodeled_06[0x10 - 0x06];
    union {
        XglQuadword reset_work[7];  /* +0x10, cleared 16 bytes at a time */
        struct {
            signed char characterHelp;  /* +0x10 */
            unsigned char unmodeled_11[0x20 - 0x11];
            unsigned char resetFlags;   /* +0x20 */
            unsigned char nextState;    /* +0x21 */
            unsigned char unmodeled_22[0x2a - 0x22];
            unsigned char inputDelay;   /* +0x2a */
            unsigned char unmodeled_2b[0x30 - 0x2b];
            signed char listCursor;     /* +0x30 */
            signed char mode;           /* +0x31 */
            signed char rowCursor;      /* +0x32 */
            signed char selectCursor;   /* +0x33 */
            unsigned char unmodeled_34[0x0C];
            signed char characterNo;    /* +0x40 */
            signed char rosterCount;    /* +0x41 */
            signed char rosterCursor;   /* +0x42 */
            signed char tecBit;         /* +0x43 */
            signed char point;          /* +0x44 */
            unsigned char unmodeled_45[0x50 - 0x45];
            int selectedTechnique;      /* +0x50 */
            unsigned char unmodeled_54[0x80 - 0x54];
        };
    };
} MenuWorkState;


#endif /* INCLUDE_MAIN_WINDOW_TEX_LOAD_H */
