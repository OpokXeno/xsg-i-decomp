#include "common.h"
#include "main/party.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "main/xgl_studio.h"
#include "main/xgl_2.h"
#include "main/menu_1.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/data.h"
#include "main/e_battle_win_open.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_model.h"
#include "main/menu_top_command_check.h"
#include "main/open_close_main.h"
#include "main/xgl_camera.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#define MENU_WORK_END 0xFF

/* MenuWork.character.state >> 4 for the 0xB0..0xBF states (jump-table cases 0xB2..0xB6
 * and 0xBF), the only ones in which the spotlight fades in. */
#define MENU_STATE_GROUP_FORMATION 11

#define SPOTLIGHT_START    0
#define SPOTLIGHT_HIDDEN   2
#define SPOTLIGHT_SHOWN    10
#define SPOTLIGHT_FADE_OUT 20

/* Screen-centre origin of the 12.4 fixed-point coordinates
 * xglRotTransPersN returns. */
#define CIRCLE_ORIGIN_X 0x700

#define CIRCLE_ORIGIN_Y 0x720

/* Scaffold .lit4 constants: the pulse step (1/24) and the easing factor
 * (0.4) tskCharSpotLight loads gp-relative. */
#define SPOT_LIGHT_PULSE_STEP (1.0f / 24.0f)

#define SPOT_LIGHT_EASE_RATE (0.4f)

/* Accessory ids after which MenuCharParaSet restores an hp/ep that was above
 * the recalculated maximum. */
#define ACCESSORY_KEEP_HP 95

#define ACCESSORY_KEEP_EP 96

/* MenuWork.character.state, the MenuCharactor state machine. Every screen
 * has an entry state that falls into its running state one higher (_RUN). */
enum {
    CHAR_STATE_STATUS = 0x10,           /* pick the character */
    CHAR_STATE_STATUS_RUN = 0x11,
    CHAR_STATE_COMMAND = 0x20,          /* the character's command menu */
    CHAR_STATE_COMMAND_RUN = 0x21,
    CHAR_STATE_EQUIP = 0x40,            /* equipment: pick the slot */
    CHAR_STATE_EQUIP_RUN = 0x41,
    CHAR_STATE_EQUIP_LIST = 0x52,       /* equipment: weapon or accessory list */
    CHAR_STATE_EQUIP_LIST_RUN = 0x53,
    CHAR_STATE_BULLET_LIST = 0x54,      /* equipment: bullet list */
    CHAR_STATE_BULLET_LIST_RUN = 0x55,
    CHAR_STATE_PARA = 0x80,             /* parameter points: pick the parameter */
    CHAR_STATE_PARA_RUN = 0x81,
    CHAR_STATE_PARA_AMOUNT = 0x82,      /* choose how many levels */
    CHAR_STATE_PARA_AMOUNT_RUN = 0x83,
    CHAR_STATE_PARA_CONFIRM = 0x84,
    CHAR_STATE_PARA_CONFIRM_RUN = 0x85,
    CHAR_STATE_PARA_DONE = 0x86,
    CHAR_STATE_PARA_DENIED = 0x87,      /* not enough points / at the maximum */
    CHAR_STATE_FORMATION = 0xA0,        /* formation (from battle): pick the attacker */
    CHAR_STATE_FORMATION_RUN = 0xA1,
    CHAR_STATE_FORMATION_MENU = 0xA2,   /* position / swap / back */
    CHAR_STATE_FORMATION_MENU_RUN = 0xA3,
    CHAR_STATE_FORMATION_MOVE = 0xB2,   /* move over the six positions */
    CHAR_STATE_FORMATION_MOVE_RUN = 0xB3,
    CHAR_STATE_FORMATION_EXCHANGE = 0xB4,   /* exchange with the occupant */
    CHAR_STATE_FORMATION_EXCHANGE_RUN = 0xB5,
    CHAR_STATE_FORMATION_PLACE = 0xB6,  /* move to the empty position */
    CHAR_STATE_FORMATION_DONE = 0xBF,
    CHAR_STATE_RESERVE_PICK = 0xC0,     /* only CharInfoMain handles these two */
    CHAR_STATE_RESERVE_PICK_RUN = 0xC1,
    CHAR_STATE_RESERVE = 0xC2,          /* swap with a reserve friend */
    CHAR_STATE_RESERVE_RUN = 0xC3,
    CHAR_STATE_RESERVE_MODEL = 0xC4,    /* rebuild the swapped-in model */
    CHAR_STATE_RESERVE_MODEL_RUN = 0xC5,
    CHAR_STATE_EXIT = 0xF0,
    CHAR_STATE_EXIT_RUN = 0xF1
};

/* MenuWork.character.state >> 4 of the parameter-point and reserve-swap states. */
#define CHAR_STATE_GROUP_PARA 8
#define CHAR_STATE_GROUP_RESERVE 12

#define CHAR_INFO_SETUP 0
#define CHAR_INFO_MAIN  2

/* The frame colour of every command window: opaque white with the lowest
 * intensity bits cleared. */
#define CHAR_MENU_COLOR 0x00FFFFF0

/* The grey level of a row the player may pick and of one that is locked. */
#define CHAR_MENU_ROW_ON  0x80

#define CHAR_MENU_ROW_OFF 0x40
#define CHAR_EX_START 0
#define CHAR_EX_SLIDE 2

/* The bar's draw order, the same form as the packed print-colour word
 * xglFontPrintf takes. */
#define CHAR_EX_DRAW_ORDER 0x00FFFFF0

/* The ribbon slides in from the right edge to this column while the screen is
 * open, and back out again when it is not. */
#define CHAR_EX_X_OUT 512

#define CHAR_EX_X_IN  264

/* The window's content colours: the party slot under the cursor is brighter. */
#define CHAR_STATUS_COLOR_SELECTED 0x00F07F00

#define CHAR_STATUS_COLOR_NORMAL   0x00F07E00

/* The state the swap menu's reserve cursor runs with, and the work word both
 * status cursors are given after eCursolSet has cleared the record. */
#define CHAR_STATUS_CURSOR_STATE 0x20

#define CHAR_STATUS_CURSOR_WORK  0x00FFFFFF
#define CHAR_STATUS_BUILD 0
#define CHAR_STATUS_RUN   2

/* The on-screen and off-screen window geometry of the party slots. */
#define STATUS_WINDOW_WIDTH   240

#define STATUS_WINDOW_HEIGHT  82
#define STATUS_WINDOW_OPEN_H  99
#define STATUS_ROW_HEIGHT     104
#define STATUS_ROW_TOP        64
#define STATUS_COLUMN_WIDTH   240
#define STATUS_COLUMN_LEFT    16
#define STATUS_OFF_LEFT       (-272)
#define STATUS_OFF_RIGHT      816

/* The window slides in from x = -512 and rests at x = 16. */
#define PARA_WINDOW_HIDDEN (-512)

#define PARA_WINDOW_SHOWN  16

/* The display word of the equipment windows: opaque, farthest back. */
#define MENU_WEAPON_COLOR 0x00FFFFF0

/* The row the cursor and the accessory lines advance by, and the offset the
 * weapon line drops to when the weapon has no bullet row. */
#define MENU_WEAPON_ROW_HEIGHT 24

#define MENU_WEAPON_LINE_DROP  12

/* The sprite drawn for the bullet line and for an empty accessory row. */
#define SPRITE_BULLET        780

#define SPRITE_NO_ACCESSORY  781

/*
 * The sizes of the work blocks MenuCharactor carves out of the menu heap
 * after MainMenuWorkEnd, one per sub-screen. Each sub-screen starts idle when
 * the first byte of its block is 0; CharEx keeps that byte at +0x10.
 */
#define CHAR_PAS_SIZE       0x550

#define CHAR_CATEGORY_SIZE  0x244
#define CHAR_MENU_SIZE      0x1E38
#define CHAR_INFO_SIZE      0x450
#define CHAR_STATUS_SIZE    0x4EF4
#define CHAR_PARAMETER_SIZE 0x8D0
#define CHAR_EX_SIZE        0x378
#define CHAR_WEAPON_SIZE    0x770
#define CHAR_LIST_SIZE      0x3404
#define CHAR_SWITCH_SIZE    0x74
#define CHAR_L1R1_SIZE      0x58
#define CHAR_POINT_SIZE     0x3D8

/* Sound effect ids. */
#define SE_SLIDE  10
#define SE_SWAP   23
#define SE_PLACE  24

/* The id of the character with two weapon slots (both are equipped). */
#define CHR_DUAL_WEAPON 7

/* The menu id that cannot be switched to or equipped (the guest). */
#define CHR_GUEST 12

/* The character whose weapon cannot be changed (only the bullet). */
#define CHR_FIXED_WEAPON 3

/* MenuKeepSelect holds one 5-byte cursor snapshot per list: the three
 * accessory sort orders first (0..2), then these fixed lists. */
#define KEEP_WEAPON     3

#define KEEP_BULLET     4
#define KEEP_PARAMETER  5
#define CHAR_MODEL_DEFAULT_MOVE_RATE (0.20000000298023224f)
#define CHAR_MODEL_RESET_MOVE_RATE (0.20000000298023224f)
#define CHAR_CAMERA_POSITION_RATE (0.10000000149011612f)
#define CHAR_CAMERA_ROTATION_RATE (0.05000000074505806f)
#define CHAR_CAMERA_TARGET_X_SCALE (0.10000000149011612f)
#define NEG_QUARTER_PI (-0.78539818525314331f)
#define BATTLE_MODEL_MOVE_RATE (0.20000000298023224f)
#define LINE_FIELD_GROW_STEP (0.20000000298023224f)
#define LINE_FIELD_FADE_IN_STEP (0.019999999552965164f)
#define LINE_FIELD_SHRINK_STEP (0.20000000298023224f)
#define LINE_FIELD_FADE_OUT_STEP (0.019999999552965164f)

/*
 * The menu work block (window_tex_load's MenuWork data, 0x80 bytes). Only the
 * members tskCharSpotLight reads are modelled.
 * - state (+0x03): the MenuCharactor state (its jump table covers 0x10..0xFF);
 *   tskCharSpotLight and tskCharLineField remove themselves when it is 0xFF.
 * - partyMode (+0x31): MenuCharactor stores its argument here (sb at
 *   0x00297ccc); MainMenu calls MenuCharactor(0) and MenuCharactor(1).
 * - formationSlot (+0x46): the 1-based party position the spotlight marks,
 *   indexing the six-entry position table.
 */
/*
 * MenuCharactor keeps
 * every cursor and selection here and the sub-screens only read them:
 * - weaponId/bulletId/accessoryId (+0x12/+0x14/+0x16): the item under the
 *   open list's cursor; bulletFits (+0x18) records that bulletId belongs to
 *   the equipped weapon.
 * - leaderLocked (+0x19): why the shown character cannot become the
 *   on-screen one (0 free, 1 already on screen, 2 a reserve, 3 a guest), the
 *   MenuCharLeaderMask result; guestDenied (+0x1A) marks a refused guest.
 * - swapOutId/swapInId (+0x20/+0x21): the attacker and the reserve friend of
 *   the pending formation swap; modelSlot (+0x22) is the menu model slot and
 *   weaponChanged (+0x23) that the weapon line has to be rebuilt.
 * - partyCount/menuCount/reserveCount (+0x30/+0x32/+0x50): the number of
 *   rows of the party, command and reserve lists.
 * - equipSlot (+0x35): the equipment slot under the cursor, indexing the
 *   three accessories of CharParaData and selecting the bullet over the
 *   weapon; equipKind (+0x4A) is 0 for a weapon or bullet and 1 for an
 *   accessory, and accessoryList (+0x4F) the accessory sort order 0..2.
 * - partyCursor/commandCursor/menuCursor/confirmCursor/reserveCursor
 *   (+0x40/+0x41/+0x49/+0x7C/+0x51): the row of each list, and listSelect
 *   (+0x43) the row of the open item list, negative while nothing is picked.
 * - formationColumn/formationRow (+0x44/+0x45) build formationSlot (+0x46).
 * - wait (+0x48): frames before the pad is read again.
 * - chrNo (+0x60): the character the screens show (1-based).
 * - flags (+0x79), paraIndex (+0x7A), paraUpCount (+0x7B) and paraCost
 *   (+0x7E): the parameter being improved, how many levels are pending and
 *   what they cost.
 * The unknown_* members are only printed by MenuCharactor's debug dump.
 */
typedef struct CharacterMenuWorkData {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x12 - 0x04];
    unsigned short weaponId;        /* +0x12 */
    unsigned short bulletId;        /* +0x14 */
    unsigned short accessoryId;     /* +0x16 */
    unsigned char bulletFits;       /* +0x18 */
    signed char leaderLocked;       /* +0x19 */
    unsigned char guestDenied;      /* +0x1A */
    unsigned char unmodeled_1b[0x20 - 0x1B];
    signed char swapOutId;          /* +0x20 */
    signed char swapInId;           /* +0x21 */
    signed char modelSlot;          /* +0x22 */
    signed char weaponChanged;      /* +0x23 */
    unsigned char unmodeled_24[0x30 - 0x24];
    signed char partyCount;         /* +0x30 */
    signed char partyMode;          /* +0x31 */
    signed char menuCount;          /* +0x32 */
    unsigned char unmodeled_33[0x35 - 0x33];
    signed char equipSlot;          /* +0x35 */
    unsigned char unmodeled_36[0x40 - 0x36];
    signed char partyCursor;        /* +0x40 */
    signed char commandCursor;      /* +0x41 */
    unsigned char unmodeled_42;
    signed char listSelect;         /* +0x43 */
    signed char formationColumn;    /* +0x44 */
    signed char formationRow;       /* +0x45 */
    signed char formationSlot;      /* +0x46 */
    signed char unknown_47;
    unsigned char wait;             /* +0x48 */
    signed char menuCursor;         /* +0x49 */
    signed char equipKind;          /* +0x4A */
    signed char unknown_4b[2];
    unsigned char unmodeled_4d[0x4F - 0x4D];
    signed char accessoryList;      /* +0x4F */
    signed char reserveCount;       /* +0x50 */
    signed char reserveCursor;      /* +0x51 */
    signed char unknown_52[4];
    unsigned char unmodeled_56[0x60 - 0x56];
    short chrNo; /* +0x60: one-based character id */
    unsigned char unmodeled_62[0x79 - 0x62];
    unsigned char flags;            /* +0x79 */
    signed char paraIndex;          /* +0x7A */
    signed char paraUpCount;        /* +0x7B */
    signed char confirmCursor;      /* +0x7C */
    unsigned char unmodeled_7d;
    unsigned short paraCost;        /* +0x7E */
} CharacterMenuWorkData;

/* The 0x80-byte MenuWork storage as the character screens view it. */
typedef union MenuWorkData {
    CharacterMenuWorkData character;
} MenuWorkData;

/*
 * The spotlight task: the scheduler prefix, then the fields tskCharSpotLight
 * owns. MenuCharactor creates it with xglTaskEntryNext and clears mode
 * (sb zero,0x10 at 0x00297ca8).
 */
typedef struct CharSpotLightTask {
    XglTaskPrefix task;             /* +0x00 */
    unsigned char mode;             /* +0x10 */
    unsigned char visible;          /* +0x11 */
    unsigned char counter;          /* +0x12: cleared with visible on start */
    unsigned char unmodeled_13[0x20 - 0x13];
    Vector4 position;               /* +0x20: eases toward the table entry */
    float size;                     /* +0x30: 0..1 fade, radius weight 48 */
    float pulse;                    /* +0x34: 0..1 swing, radius weight 16 */
    float pulseStep;                /* +0x38: added to pulse every frame */
} CharSpotLightTask;

/*
 * The circle endPrintDirectCircle draws. PrintCircleCore reads x/y (lh 0/2,
 * then adds 0x700/0x720 back at 0x0027dd94/0x0027ddf8), z (lw 4), the centre
 * RGBA (lbu 8..0xB), the radius (lw 0xC), the segment count (lw 0x10, the
 * loop bound) and the rim RGBA (lbu 0x14..0x17).
 */
typedef struct DirectCircle {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    int z;                          /* +0x04 */
    unsigned char innerColor[4];    /* +0x08 */
    int radius;                     /* +0x0C */
    int segments;                   /* +0x10 */
    unsigned char outerColor[4];    /* +0x14 */
} DirectCircle;

/*
 * The six spotlight positions (x = 1, 0, -1 on the z = 0.6 and z = -0.6 rows),
 * scaffold .rodata at 0x004C6770. tskCharSpotLight copies the whole table to
 * its frame with ld/sd and no alignment test (0x00297440..0x00297470), which
 * 2.96 emits only for an aligned source type; the quadword-aligned vectors
 * carry that alignment.
 */
typedef struct SpotLightTable {
    AlignedVector4 position[6];
} SpotLightTable;

/*
 * One character's 0x180-byte parameter record. dataUnitOrgGet (ov01 VA
 * 0x00a191c0) returns orgData + (chrNo - 1) * 0x180, and calcTotalParaMenu
 * (ov01 VA 0x00a11108) returns a recalculated copy of the same record.
 * calcTotalParaMenuSub clamps the stored hp (+0x34) and ep (+0x36) to the
 * recalculated maxHp (+0x00) and maxEp (+0x02) (lh/slt/sh at
 * 0x00a11180..0x00a111ac), and it fills the two
 * caller buffers MenuCharParaSet adds into attack (+0x04), phyDefense (+0x06)
 * and magDefense (+0x0A): three weapon attack words from calcWpnAtkGet and the
 * calcPhyDefGet/calcMagDefGet pair. MenuCharParaSet compares the three
 * halfwords at +0x64 with the item ids 95 and 96.
 */
/*
 * The parameter window shows two more values past the statIndex table
 * MenuParaPtNowGet covers (+0x0E and +0x1E), and the equipment screens read
 * the three equipment slots of the same record: the weapon (+0x5E) and the
 * bullet (+0x6A) of the slot setSlotTbl gives the character, and the three
 * accessories (+0x64), a zero id meaning an empty slot.
 */
typedef struct CharParaData {
    short maxHp;                /* +0x00 */
    short maxEp;                /* +0x02 */
    short attack;               /* +0x04 */
    short phyDefense;           /* +0x06 */
    short stat4;                /* +0x08: statIndex 4 of MenuParaPtNowGet */
    short magDefense;           /* +0x0A */
    signed char stat6;          /* +0x0C: statIndex 6 */
    signed char stat7;          /* +0x0D: statIndex 7 */
    signed char stat8;          /* +0x0E */
    unsigned char unmodeled_0f[0x1E - 0x0F];
    short stat9;                /* +0x1E */
    unsigned char unmodeled_20[0x34 - 0x20];
    short hp;                   /* +0x34 */
    short ep;                   /* +0x36 */
    unsigned char unmodeled_38[0x5E - 0x38];
    short weapon[3];            /* +0x5E: indexed by setSlotTbl */
    short accessory[3];         /* +0x64 */
    unsigned short bullet[3];   /* +0x6A: indexed by setSlotTbl */
    unsigned char unmodeled_70[0x180 - 0x70];
} CharParaData;

/* The shared character point pools used by parameter spending. */
typedef struct CharPointData {
    unsigned char unmodeled_00[0x0C];
    int points;
    int etherPoints;
    int skillPoints;
} CharPointData;

/* The inset frame endPrintExtFunc draws behind the tab strip. */
typedef struct CharPasBox {
    short x;                           /* +0x00 */
    short y;                           /* +0x02 */
    int kind;                          /* +0x04 */
    short width;                       /* +0x08 */
    short height;                      /* +0x0A */
} CharPasBox;

/*
 * The character-pass screen's own work block: MenuCharactor carves it as a
 * 0x550-byte block of the menu heap (CHAR_PAS_SIZE) and clears only its
 * first byte (state) before handing it to CharPasMain. Only the members
 * CharPasMain itself reads or writes are modeled.
 */
typedef struct CharPasWork {
    unsigned char state;               /* +0x00 */
    unsigned char unmodeled_01[0x03 - 0x01];
    unsigned char tabCount;            /* +0x03 */
    int kind;                          /* +0x04 */
    WindowDX window;                   /* +0x08: CharPasMain points its close
                                        * callback at the empty MenuPasWindow
                                        * stub and at callbackWork */
    EMessageParam messages[12];        /* +0x19C: the category-tab captions */
    CharPasBox box;                    /* +0x4CC */
    unsigned char unmodeled_4d8[0x4E0 - 0x4D8];
    unsigned char callbackWork[0x550 - 0x4E0];  /* +0x4E0 */
} CharPasWork;

/*
 * PARTIAL ACCESSED PREFIX of PadData (the 0xD0-byte pad block at 0x00490d90):
 * the halfwords this menu reads. held/pressed are the two the shared
 * include/shared.h PadPrefix view names (half_28/half_2a); repeat carries the
 * direction auto-repeat and repeatFirst equals it only on the frame a
 * direction is first pressed, so a list wraps only on a fresh press.
 *
 * TU-local view: a local extension of PadData past
 * PadPrefix, the same situation documented in src/main/game.h's
 * PadDataDebugLayout and src/main/set_path.c's PadDataEffectLayout.
 */
typedef struct MenuPadData {
    unsigned char unmodeled_00[0x28];
    unsigned short held;            /* +0x28 */
    unsigned short pressed;         /* +0x2A */
    unsigned char unmodeled_2c[0x32 - 0x2C];
    unsigned short repeatFirst;     /* +0x32 */
    unsigned short repeat;          /* +0x34 */
    unsigned char unmodeled_36[0xD0 - 0x36];
} MenuPadData;

/*
 * The record MenuInfoWindow draws one line from. Only the two coordinates the
 * setup clears and the text pointer are modelled; from +0x10 on it is the
 * eMessage block MenuInfoWindow fills itself.
 */
typedef struct CharInfoText {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    unsigned char unmodeled_04[0x0C - 0x04];
    char *text;                     /* +0x0C */
    unsigned char unmodeled_10[0x1B0 - 0x10];
} CharInfoText;

/*
 * The help window's work block, the 0x450 bytes MenuCharactor carves out of
 * the menu heap. state is 0 for the frame it was carved in and 2 from then on.
 * opened and selected are written once by the setup and no recovered function
 * reads them back. reasonShown remembers that the line in the buffer is the
 * explanation of a locked character, which stays until the next pad input.
 */
/*
 * The help line at the bottom of the character menu: the window is built on
 * the frame MenuCharactor carves its work block, and from then on every frame
 * writes the line MenuCharactor's current state asks for into the block's own
 * message buffer and slides the window to that state's height.
 */
typedef struct CharInfoWork {
    unsigned char state;            /* +0x00 */
    unsigned char opened;           /* +0x01 */
    signed char selected;           /* +0x02 */
    unsigned char reasonShown;      /* +0x03 */
    int color;                      /* +0x04 */
    /* +0x08: WindowDXMain calls MenuInfoWindow(window, &info) every frame;
     * it switches on window->state (lb 0x10 at 0x0027a12c) and hands
     * info.text (lw 0xC at 0x0027a158) to eMessageSet. */
    WindowDX window;
    unsigned char unmodeled_19c[0x1A0 - 0x19C];
    CharInfoText info;              /* +0x1A0 */
    char message[0x100];            /* +0x350 */
} CharInfoWork;

/*
 * One of CharCategory's five equip-category icons (weapon/bullet/accessory/
 * status/point). eSpriteSet and eSpriteMain (main/e_number_main.c, not yet
 * published) own the fields CharCategoryMain does not touch; the sprite
 * object they operate on starts at +0x00 of this record (no header bytes
 * precede it), so the array stride is the record size.
 */
typedef struct CharCategoryIcon {
    unsigned char unmodeled_00[0x04]; /* eSpriteSet's own fields */
    short x;                          /* +0x04 */
    short y;                          /* +0x06 */
    int work;                         /* +0x08 */
    signed char color[3];             /* +0x0C: r,g,b tint */
    unsigned char alpha;              /* +0x0F */
    unsigned char unmodeled_10[0x28 - 0x10]; /* eSpriteMain's own fields */
} CharCategoryIcon;

/*
 * The 0x244-byte block MenuCharactor carves for the category screen.
 * edgeDelta[0]/[1] nudge icons[0]/icons[1] left/right for a few frames after
 * the pad repeats PAD_LEFT/PAD_RIGHT; icons[2..4] dim to 0x40 or brighten to
 * -0x80 to mark the one matching MenuWork.character.accessoryList.
 */
typedef struct CharCategoryWork {
    unsigned char state;          /* +0x00: 0 needs init, 2 active */
    unsigned char phase;          /* +0x01: cleared with state on init */
    unsigned char pulseCounter;   /* +0x02: 0..8, drives the icon nudge and fade */
    unsigned char tickCounter;    /* +0x03: increments every call */
    int pendingReset;             /* +0x04: nonzero selects the wide icon layout */
    int iconResetWork;            /* +0x08: seeds every icon's work field */
    unsigned short flashLevel;    /* +0x0C: 0..10 bounce, stepped every other tick */
    unsigned short flashStep;     /* +0x0E: 1 while rising, 0xFFFF while falling */
    signed char edgeDelta[2];     /* +0x10: icons[0]/icons[1] pad nudge */
    unsigned char unmodeled_12[0x14 - 0x12];
    CharCategoryIcon icons[5];    /* +0x14 */
    unsigned char unmodeled_b4[0x244 - 0xB4];
} CharCategoryWork;

/* Scaffold .rodata: the five icon sprite ids CharCategoryMain assigns at init. */
typedef struct CharCategoryIconIdTable {
    short id[5];
} CharCategoryIconIdTable;

/*
 * The character screen's command windows: CharMenuMain builds one WindowDX
 * per command list and hands MenuSelectWindow the matching list block as the
 * window's close callback argument.
 */
typedef struct CharMenuList CharMenuList;

/*
 * One selection list MenuSelectWindow (main/menu_top_command_check.c) draws
 * inside such a window: it reads hasPrompt (lhu 2), the highlighted row
 * (lw 4, multiplied by the 24-pixel row height), the newline-separated item
 * text (lw 8) and the prompt line above the items (lw 0xC), and keeps its
 * cursor and twelve message objects in the rest of the 0x5F8 bytes.
 */
struct CharMenuList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt;           /* +0x02 */
    int cursor;                         /* +0x04 */
    const char *items;                  /* +0x08 */
    const char *prompt;                 /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
};

/*
 * The character screen's work block. state 0 builds the windows and turns
 * into state 2, the frame CharMenuMain runs every frame afterwards; subState
 * is cleared when the windows are built and set to 16 while MenuWork.character.state
 * sits in the 0xB2..0xB5 and 0xBF range.
 */
typedef struct CharMenuWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[0x02 - 0x01];
    unsigned char windowCount;          /* +0x002 */
    unsigned char subState;             /* +0x003 */
    int color;                          /* +0x004 */
    WindowDX window[3];                 /* +0x008 */
    unsigned char unmodeled_4c4[0x658 - 0x4C4];
    CharMenuList list[3];               /* +0x658 */
} CharMenuWork;

/*
 * The character screen's point bar: three rows, each one ribbon with its
 * label and its value. The word that follows x and y in each element (work)
 * is the draw order the element's own Main copies verbatim into its render
 * record (eNumberMain: lw 0x4, sw 0x6C at 0x0027648c); this screen gives
 * every ribbon the bar's base order and puts the label and the value two in
 * front of it.
 */
typedef struct CharExWork {
    unsigned char unmodeled_00[0x10];
    unsigned char state;            /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    unsigned int drawOrder;         /* +0x14 */
    ERibbonMainData ribbon[3];      /* +0x18 */
    ETagFontMainData label[3];      /* +0x168 */
    ENumberMainData number[3];      /* +0x1C8 */
} CharExWork;

/*
 * Partial view of the object eSpriteSet (src/main/e_number_main.c, a
 * different TU) initialises: only the fields CharL1R1Main itself reads or
 * writes after the call.
 */
typedef struct L1R1Sprite {
    unsigned char unmodeled_00[0x04];
    short x;                 /* +0x04 */
    short y;                 /* +0x06 */
    int work;                /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} L1R1Sprite;

/*
 * CharL1R1's 0x58-byte work block: MenuCharactor carves it from the menu
 * heap with state left at 0. CharL1R1Main runs the one-time setup while
 * state is 0, leaves it at 2 and keeps re-running the per-frame update on
 * every later call while it stays 2; any other value makes it return
 * immediately.
 */
typedef struct CharL1R1Work {
    unsigned char state;         /* +0x00 */
    unsigned char unmodeled_01;
    signed char slideOffset[2];  /* +0x02: added into sprite[i].x while non-zero */
    int spriteWork;              /* +0x04: shared initial value for both sprites' work */
    L1R1Sprite sprite[2];        /* +0x08: L1, then R1 */
} CharL1R1Work;

/*
 * Scaffold .sdata tables read by CharL1R1Main. 0x004DADF0 holds the L1 and
 * R1 sprite ids (0x0112, 0x0110), 0x004DADF8 the per-side step (+1, -1) that
 * walks a pressed side's slide offset back to zero. Both are copied as a
 * pair; their extent past that pair belongs to the scaffold.
 */
typedef struct L1R1SpriteIdPair {
    short id[2];
} L1R1SpriteIdPair;

typedef struct L1R1SlideStep {
    signed char side[2];
} L1R1SlideStep;

/*
 * A swap cursor reached as a list node: twelve bytes of node header, then the
 * cursor record (the node pointer is the cursor address minus 12). Only the
 * state byte is written through the node.
 */
typedef struct CharStatusCursorNode {
    unsigned char header[0x0C];         /* +0x00 */
    unsigned char state;                /* +0x0C: the cursor's state */
    unsigned char unmodeled_0d[0x24 - 0x0D];
} CharStatusCursorNode;

/*
 * The character-status work block MenuCharactor carves from the menu heap
 * (0x4EF4 bytes, state 0 means it still has to be built): the control fields,
 * the eight status windows (the three party slots followed by the five
 * reserve slots), their eight status panels, then the two swap cursors
 * (cursor[0] marks the party slot and cursor[1] the reserve slot). Same
 * layout as MenuSkillStatusWork.
 */
typedef struct CharStatusWork {
    unsigned char state;                /* +0x0000 */
    unsigned char cursorOn;             /* +0x0001 */
    unsigned char unmodeled_02[0x04 - 0x02];
    unsigned char moveVert;             /* +0x0004: slide y before x */
    unsigned char flags;                /* +0x0005: bit 0 = the slide finished */
    unsigned char subState;             /* +0x0006 */
    signed char lastCursor;             /* +0x0007 */
    int color;                          /* +0x0008 */
    WindowDX window[8];                 /* +0x000C */
    MenuStatusPanel panel[8];           /* +0x0CAC */
    ECursolMainData cursor[2];          /* +0x4EAC */
} CharStatusWork;

/*
 * The block is also walked as list nodes (a twelve-byte header followed by the
 * record): the reserve cursor's state is armed through its node, 0x18 bytes
 * into the cursor pair where the node header starts, while every other access
 * goes through CharStatusWork.
 */
typedef union CharStatusBlock {
    CharStatusWork work;
    struct {
        unsigned char unmodeled_00[0x4EC4];
        CharStatusCursorNode swap;      /* +0x4EC4 */
    } node;
} CharStatusBlock;




/*
 * CharListWork is the equip-list screen's work block.
 * - state (+0x00): the CharListMain state its jump table dispatches on.
 * - active (+0x01): CharListMain runs the window only while it is set.
 * - cursorIndex/scrollIndex (+0x02/+0x03): cleared when the list opens.
 * - color (+0x08): the packed 0xRRGGBB tint CharListMain sets once, matching
 *   the list window's own tint.
 */
typedef struct CharListWork {
    unsigned char state;        /* +0x00 */
    unsigned char active;       /* +0x01 */
    unsigned char cursorIndex;  /* +0x02 */
    unsigned char scrollIndex;  /* +0x03 */
    unsigned char unmodeled_04[0x08 - 0x04];
    int color;                  /* +0x08 */
    ESprite icons[3];      /* +0x0C */
    ENumberMainData rows[8];    /* +0x84 */
    /* +0x504: the list window CharListMake_Wpn/_Gun/_Acc/_Para fill for
     * their category; CharListMain keeps its state at 0 while building it
     * and sets 0x11 once a category is populated. */
    WindowSPCursor weapon;
    WindowSPCursor paraWindow;   /* +0x1C3C: the parameter-up list window */
    /* +0x3374: the twelve twelve-byte list rows CharListMake_Para builds in
     * place (addiu $16,$16,0xC at 0x00295900) and hands to the window as its
     * item list; CharListMain clears them when the screen opens. */
    MenuListEntry paraItems[12];
} CharListWork;

/*
 * The bullet record dataAttGet (ov01 VA 0x00a1a428, still assembler) returns
 * for one ammunition id; only the id of the weapon it belongs to (+0xC) is
 * read here, the same member main/menu_1.c's MenuBulletCheck compares. Kept
 * under the scaffold's undefined_funcs_auto.txt placeholder name func_A1A428
 * until ov01 recovers dataAttGet itself.
 */
typedef struct MenuBulletData {
    unsigned char unmodeled_00[0x0C];
    short weapon;                   /* +0x0C */
} MenuBulletData;


/*
 * CharParameter is the parameter window's work block. MenuCharactor carves it
 * out of the menu heap and CharParameterMain drives it: state 0 builds the
 * window and its widgets, state 2 runs one frame. The sub-window sits at +0x08
 * and is the same 0x194-byte control block WindowDXSet/WindowDXMain share, so
 * the eight numbers start at +0x19C. This screen packs each number's current
 * value in the low halfword of its value word and the recalculated one in the
 * high halfword, and reads both halves back to pick the arrow (2 above, 1
 * below, 0 equal, and then no arrow at all).
 */
typedef struct CharParameterWork {
    unsigned char state;        /* +0x00 */
    /* +0x01: 1 while the window is built, then 7, or 15 once the equip list
     * reports state 3; +0x02 is reset to -1 with it. No recovered function
     * reads either back yet. */
    signed char mask;           /* +0x01 */
    signed char selectNo;       /* +0x02 */
    unsigned char unmodeled_03[1];
    int color;                  /* +0x04 */
    WindowDX window;          /* +0x08 */
    ENumberMainData number[8];  /* +0x19C */
    unsigned char unmodeled_61c[0x75C - 0x61C];
    ETagFontMainData tag[8];    /* +0x75C */
    unsigned char ribbonShown;  /* +0x85C */
    ERibbonMainData ribbon;     /* +0x860 */
} CharParameterWork;

/* The digit count of each of the eight numbers (three digits for the halfword
 * stats, two for the byte ones), copied into the frame as one record. */
typedef struct ParaDigitCounts {
    unsigned char count[8];
} ParaDigitCounts;



/*
 * The weapon icon of every party character (chrNo 1..7), as the scaffold's
 * .rodata piece at 0x004C6398 holds it; the fourth character carries no
 * weapon of its own. CharWeaponMain reads it through a copy on its frame.
 */
typedef struct WeaponIconTable {
    short icon[7];
} WeaponIconTable;

/*
 * The accessory icon of every icon group, as the scaffold's small-data piece
 * at 0x004DAE48 holds it (the third group shares the empty-row sprite).
 * CharWeaponMain reads it through a copy on its frame.
 */
typedef struct AccessoryIconTable {
    short icon[3];
} AccessoryIconTable;

/*
 * The accessory record, as far as the equipment display needs it: the icon
 * group its sprite comes from. src/ov01/calc.h owns the AccessoryData type
 * and models its +0x0D term, so this screen keeps its own bounded view of
 * the same record until the two are harvested into one header.
 */
typedef struct MenuAccessoryData {
    unsigned char unmodeled_00[0x0A];
    unsigned short iconKind;            /* +0x0A */
} MenuAccessoryData;

/*
 * The weapon/accessory display of the character menu (0x770 bytes carved
 * from the menu heap). window[0] is the weapon window with the weapon line
 * (message/sprite 0) and the bullet line (message/sprite 1); window[1] is
 * the accessory window with its three rows (message/sprite 2..4); the cursor
 * marks the row MenuWork.character.equipSlot in the window MenuWork.character.equipKind selects.
 * color is the display word every widget copies, the foreground ones with 2
 * added.
 */
typedef struct CharWeaponWork {
    unsigned char state;                /* +0x000 */
    unsigned char subState;             /* +0x001: cleared with the move to state 2 */
    unsigned char unmodeled_002[0x04 - 0x02];
    int color;                          /* +0x004 */
    WindowDX window[3];             /* +0x008 */
    ECursolMainData cursor;                   /* +0x4C4 */
    EMessageParam message[6];                 /* +0x4E8 */
    ESprite sprite[6];               /* +0x680 */
} CharWeaponWork;

/*
 * Partial view of the sprite object eSpriteSet/eSpriteMain (main/e_number_main.c,
 * still assembler at this TU's boundary) receive: the x/y/work fields this
 * call overwrites right after eSpriteSet, at the same offsets eSpriteSet's
 * own ESprite type uses for them.
 */
typedef struct CharSwitchArrow {
    unsigned char unmodeled_00[0x04];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int work;                       /* +0x08 */
} CharSwitchArrow;

/*
 * The switch-character arrow indicator MenuCharactor carves from the menu
 * heap (still assembler there); only the members CharSwitchMain touches are
 * modelled.
 */
typedef struct CharSwitchWork {
    unsigned char state;            /* +0x00 */
    unsigned char unmodeled_01[0x04 - 0x01];
    int color;                      /* +0x04 */
    CharSwitchArrow arrow;          /* +0x08 */
    unsigned char unmodeled_14[0x30 - 0x14];
    EMessageParam message;          /* +0x30 */
} CharSwitchWork;

/*
 * The panel's background quad (eSpriteSet/eSpriteMain object): x/y position
 * it, id is the same kind of per-row handle, and printX/printY are the
 * first of the two points endPrintExtFunc's caption line reads.
 */
typedef struct CharPointSprite {
    unsigned char unmodeled_00[0x04];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int id;                         /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0C];
    short printX;                   /* +0x28 */
    short printY;                   /* +0x2A */
} CharPointSprite;

/*
 * A tint record: id is the same per-row handle, r/g/b/a are the colour
 * eSpriteMain/eTagFontMain blend into their render colour (the byte order
 * MenuBgColor uses), and extraX/extraY (only ever set on the first record)
 * are the second of the two points endPrintExtFunc's caption line reads.
 */
typedef struct CharPointColorSlot {
    int id;                         /* +0x00 */
    signed char r;                  /* +0x04 */
    signed char g;                  /* +0x05 */
    signed char b;                  /* +0x06 */
    signed char a;                  /* +0x07 */
    short extraX;                   /* +0x08 */
    short extraY;                   /* +0x0A */
} CharPointColorSlot;

/*
 * CharPointMain's own work block: window_tex_load carves it from the menu
 * heap (0x3D8 bytes) and CharPoint points at it. state (+0x000) is 0 before
 * the panel has been built and 1 once CharPointMain has built it once.
 * The window's position is what every child slot below is positioned
 * relative to; the two tag slots carry the row titles and the three number
 * slots the point values (format 0, four digits).
 */
typedef struct CharPointPanel {
    WindowDX window;                                /* +0x000 */
    ETagFontMainData tagSlots[2];                   /* +0x194 */
    ENumberMainData numberSlots[3];                 /* +0x1D4 */
    CharPointSprite sprite;                         /* +0x384 */
    CharPointColorSlot colors[2];                   /* +0x3B0 */
    int frameStart;                                 /* +0x3C8 */
    unsigned char unmodeled_3cc[0x3D0 - 0x3CC];
} CharPointPanel;

typedef struct CharPointWork {
    unsigned char state;                            /* +0x000 */
    unsigned char unmodeled_001[3];
    int tagBase;                                    /* +0x004 */
    CharPointPanel panel;                           /* +0x008 */
} CharPointWork;

/* The weapon record, as far as this menu needs it: the bullet it is loaded
 * with by default (ov01's calc code owns the whole record). */
typedef struct MenuWeaponData {
    unsigned char unmodeled_00[0x1A];
    short bullet;                   /* +0x1A */
} MenuWeaponData;

/* The model task invokes callbacks with its model in a0 and the stored
 * context word in a1 (main 0x0028124c..0x00281254). */
typedef void (*MenuCharacterModelCallback)(MenuModelUnit *unit, void *argument);

typedef union MenuCameraPair {
    AlignedVector4 vector[2];
    float component[2][4];
} MenuCameraPair;

typedef struct MenuCameraRotationPair {
    AlignedVector4 vector[2];
} MenuCameraRotationPair;

/* TairetuRot/TairetuPos (.bss +0x320/+0x330) are copied as two doublewords,
 * but the original .bss is only 8-aligned, so their copy type is the 8-aligned
 * DoublewordAlignedVector4 rather than AlignedVector4. */
typedef struct MenuCharacterPositions {
    AlignedVector4 vector[6];
} MenuCharacterPositions;

typedef struct MenuCameraSetPositionVectors {
    Vector4 vector[2];
} MenuCameraSetPositionVectors;

typedef struct MenuCameraSetRotationVectors {
    Vector4 vector[2];
} MenuCameraSetRotationVectors;

typedef struct MenuCameraSetPair {
    union {
        MenuCameraPair representation;
        MenuCameraSetPositionVectors values;
    } storage;
} MenuCameraSetPair;

typedef struct MenuCameraSetRotationPair {
    union {
        MenuCameraRotationPair representation;
        MenuCameraSetRotationVectors values;
    } storage;
} MenuCameraSetRotationPair;

typedef struct MenuCameraSetVector {
    DoublewordAlignedVector4 storage;
} MenuCameraSetVector;

typedef struct CharLineFieldTask {
    XglTaskPrefix task;
    unsigned char state;
    unsigned char visible;
    signed char wait;
    unsigned char unmodeled_13[0x20 - 0x13];
    AlignedVector4 scale;
    float alpha;
} CharLineFieldTask;

typedef struct LineProjection {
    int component[4];
} LineProjection;

typedef struct DirectLineColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} DirectLineColor;

typedef struct DirectLineVertex {
    LineProjection coordinates;
    DirectLineColor color;
    unsigned char unmodeled_14[12];
} DirectLineVertex;

/* This TU defines these menu labels below; their pointer tables precede
 * the storage definitions. Declare their array type before use; the definitions retain each extent. */
extern const char accessoryCommandLabel[];

extern const char unequipCommandLabel[];
extern const char charactersLabel[];
extern const char replaceCommandLabel[];
extern const char formationCommandLabel[];
extern const char battleFormationLabel[];
extern const char drawSetCancelText[];
extern const char formationReplaceCancelText[];
extern const char equipUnequipUsePtsText[];
extern const char confirmPromptText[];
extern const char setLeadCharacterText[];
extern const char currentText[];
extern const char setCommandLabel[];
extern const char drawCommandLabel[];
extern const char otherCommandLabel[];
extern const char bodyCommandLabel[];
extern const char ammoCommandLabel[];
extern const char weaponCommandLabel[];
extern const char listCommandLabel[];
extern const char skillCommandLabel[];
extern const char equipCommandLabel[];
extern const char yesNoChoicesText[];
extern const char menuParaPtRateGetSPtsText[];
extern const char menuParaPtRateGetEPtsText[];
extern const char menuParaPtRateGetTPtsText[];
extern const char weightText[];
extern const char agilityText[];
extern const char menuParaPtRateGetEvasionText[];
extern const char menuParaPtRateGetDexterityText[];
extern const char menuParaPtRateGetEdefText[];
extern const char menuParaPtRateGetEatkText[];
extern const char menuParaPtRateGetVitalityText[];
extern const char menuParaPtRateGetStrengthText[];
extern const char totalText[];

/*
 * CharPasMain reads two more bytes of the same trailing span: menuCursor
 * (+0x49), the category the command cursor stands on, and equipKind (+0x4A),
 * the equip sub-tab. It slides the tab of the selected category, and of the
 * sub-tab it opens, out of the strip (states 0x40-0x41, 0x52-0x53, 0x54-0x55).
 */
extern MenuWorkData MenuWork;

extern const SpotLightTable spotLightPositionTable;
/* The points one level-up of a parameter adds. */
static signed char ParaPointTbl[];

/*
 * ParaDataBuf is a pointer to a block of parallel per-character halfword
 * tables (one such table per stat display, e.g. base); each character's row
 * is 8 halfwords wide. ParaDataChangeTbl (8 bytes) remaps a stat index
 * (statIndex) to its halfword slot within that row. MenuParaPtBaseGet reads
 * the computed slot from a table 56 halfwords after ParaDataBuf.
 */
extern unsigned short *ParaDataBuf;

extern unsigned char ParaDataChangeTbl[];

extern int MenuScenarioNo;

extern CharPasWork *CharPas;
extern MenuPadData PadData;
extern CharInfoWork *CharInfo;
extern CharCategoryWork *CharCategory;
extern const CharCategoryIconIdTable charCategoryIconTable;
extern CharMenuWork *CharMenu;

/* The window titles, the small .sdata string constants "Menu" (0x004DADC8)
 * and "Select" (0x004DADD0) the scaffold still owns. */
extern const char menuTitleLabel[];

extern const char selectTitleLabel[];

/* The confirmation window's list ("Yes\nNo") and the question above it
 * (" Is this okay?"). */
static const char *msg01_3[];

extern CharExWork *CharEx;
extern CharL1R1Work *CharL1R1;
extern const L1R1SpriteIdPair l1r1SpriteIdTable[];
extern const L1R1SlideStep l1r1SlideStepTable[];

/* The block MenuCharactor allocated for the character status menu. */
extern CharStatusBlock *CharStatus;

extern CharListWork *CharList;

/* Saved WindowSP selections, one 5-byte snapshot per list; index 15 = the
 * 4th slot, this screen's weapon list, and index 20 the 5th, its bullet
 * list. */
extern unsigned char MenuKeepSelect[0x64];

extern CharParameterWork *CharParameter;
extern ParaDigitCounts paraDigitCountTable[];
extern const WeaponIconTable charWeaponIconTable;
extern const AccessoryIconTable charAccessoryIconTable[];
extern CharWeaponWork *CharWeapon;
extern char listWindowTag[];
extern CharSwitchWork *CharSwitch;

/* The character-switch prompt's text pointer (config/symbols/main.txt: msg_6,
 * size 4), whose stored address is the string "Set lead character"; the
 * pointer itself is still part of this TU's assembler data. */
static char * msg_6[];

/* The "T.Pts" panel title window_tex_load's string table holds. */
extern const char tPtsTitleLabel[];

/* The two eTagFontSet values CharPointMain's title-row loop passes. */
static const char *msg_7_0036D938[];

extern CharPointWork *CharPoint;

/* First free byte of the menu heap after the main menu's own blocks. */
extern unsigned char *MainMenuWorkEnd;

extern XglTaskScheduler *MenuTask_XMX;

/* The parameter table file read into ParaDataBuf (2 KB aligned). */
extern const char paraUpBinPath[32];

/* The debug caption printed above the state dump. */
extern const char charactorText[16];

/* The three menu model handles, one per attacker slot. */
extern void *MenuModelOut[];

extern const MenuCameraPair singleCameraPositionTable;
extern const MenuCameraPair singleCameraRotationTable;
extern const MenuCameraPair formationCameraRotationTable;
extern const MenuCameraPair formationCameraPositionTable;
extern const MenuCharacterPositions battleModelPositionTable;
extern const AlignedVector4 lineFieldInitialScaleVector;
extern const unsigned char rowOpacityTable[];
extern const unsigned char columnOpacityTable[];

/* Weapon slot per party character (chrNo 1..7), indexing calcTotalParaMenu's
 * attack words. */
static unsigned char setSlotTbl[];

/* xglRotTransPersN (VA 0x00229ab0): projects count points through the
 * optional matrix and the camera, writing four words per point. */
void xglRotTransPersN(int *screen, Matrix4 *matrix, Vector4 *points, int count, int cameraId);

/* endPrintDirectCircle (main/end_print.c) passes its argument on to
 * PrintCircleCore, which reads it as the DirectCircle above; this call site
 * passes the address of one. */
void endPrintDirectCircle(DirectCircle *circle);

/* dataUnitOrgGet (ov01 VA 0x00a191c0), linked by the scaffold's
 * undefined_funcs_auto.txt placeholder name. */
CharParaData *func_A191C0(int chrNo);

/* calcTotalParaMenu (ov01 VA 0x00a11108), linked by the scaffold's label for
 * that address. */
CharParaData *func_00A11108(int chrNo, int *attack, int *defense);

/* dataPlChaGet (ov01 VA 0x00a19210; see src/main/menu_skill.h and
 * src/main/menu_tec.c, which model this same per-character record under the
 * same scaffold label). Kept under the scaffold's undefined_funcs_auto.txt
 * placeholder name func_A19210 until ov01 recovers dataPlChaGet itself. */
CharPointData *func_A19210(int chrNo);
int MenuParaNextPointGet(int chrNo, int statIndex, int count);
/* main/party.c: non-zero while the character fights in the active party. */
int PartyAttackerCheck(int chrNo);

/* MoveSlide (main/window_tex_load.c): eases a short position toward its
 * target at 1/rate of the remaining distance. */
void MoveSlide(short *current, short *target, float rate);

void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);
int MenuCharEquipCalcPointGet(int chrNo);
void WindowDXFlagChange(void *window, int flag);
char *MenuCharNameGet(int chrNo);
char *MenuParaNameGet(int statIndex);
char *MenuNumberTextGet(int value, int width, int mode);
int MenuSortGet(int listIndex, int entryIndex);
int strcmp(const char *first, const char *second);
/* main/menu_tag.c: the caption string of one window kind. */
char *MenuTagTextGet(int kind);

MenuListEntry *MenuListMake(int listIndex, int mode);
MenuListEntry *MenuListGet(int listIndex);

MenuBulletData *func_A1A428(int ammoId);

/*
 * MenuSortAddrGet and MenuSortCheck (main/menu_1.c) return the sort row of
 * one list and the number of entries in it. A row entry packs the item id in
 * its low halfword (MenuIdChange adds the high part).
 */
void *MenuSortAddrGet(int listIndex);

int MenuSortCheck(int listIndex);

/* Whether the weapon takes the bullet row of the weapon window as well. */
int MenuBulletCheck2(int weapon, int chrNo);

/* dataAccGet (ov01 VA 0x00a1a548), linked by the scaffold's
 * undefined_funcs_auto.txt placeholder name. */
MenuAccessoryData *func_A1A548(int accessory);

char *MenuParaNameGet2(int statIndex);

/* WindowSPSet and WindowSPMain are not yet recovered
 * (main/e_battle_win_open.c, still assembler); both read only the window
 * pointer in $4 (WindowSPSet overwrites $5 and $6 with its own values at
 * 0x00283b88 before reading them), so the window geometry this state stores
 * stays in the argument registers across the call without being passed. */
void WindowSPSet(WindowSPCursor *window);

void WindowSPMain(WindowSPCursor *window);

/* CharListMake_Acc and CharListMake_Para are still assembler in this TU;
 * like CharListMake_Wpn they take no arguments. */
void CharListMake_Acc(void);

void CharListMake_Para(void);

/* dataWpnGet (ov01 VA 0x00a1a3d8), linked by the scaffold's
 * undefined_funcs_auto.txt placeholder name. */
MenuWeaponData *func_A1A3D8(int weapon);

void xglFontDebugHex(int x, int y, int value, int digits);

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

void MenuWorkEndCheck(unsigned char *end);
void MenuKeepSelectReset(void);
int MenuCursorKeepCheck(void);
int MenuSelectMove(int cursor, int count, int wrap);
/*
 * Whether chrNo may hold the party leader slot in the current scenario:
 * every scenario locks the leader to character 3 except 159-160, which
 * additionally allows 1 or 5; any other character is denied (1). A scenario
 * outside every named range skips the character check entirely. The
 * eligible character then still needs to be an active attacker (1..7),
 * an attacker in the current party (2), and the actual leader (0 or 3).
 */
int MenuCharLeaderMask(unsigned short chrNo);
int MenuBulletCheck(int weapon, int bullet);
void MenuModelCreate(void **model, int chrNo);
void MenuModelExtFuncSet(void *model, MenuCharacterModelCallback func, void *argument);
void ChangeTopLevel(int level);
/* MenuParaUp (this TU, still assembler): adds one level of the parameter. */
void MenuParaUp(int chrNo, int statIndex);

void MenuCharCameraSet(void);
void MenuCharCameraMove(void);
void MenuCharModelMain(MenuModelUnit *unit, void *argument);
void MenuBattleModelMain(MenuModelUnit *unit, void *argument);
static int tskCharLineField(XglTaskPrefix *task);
void MenuModelUnitOpen(void *unit, int drawType);
int MenuModelWeaponCreate(void *unit, int drawType, int weaponIndex);
void MenuModelControl2(void *unit, int mode);
int MenuShionMwsCheck(void);
void endPrintDirectLine(DirectLineVertex *vertices);

/* The twelve normal-menu category captions ("Characters", "/Equip", ...). */
static const char *msg_0_0036D8A8[12];

/* The three battle-formation category captions. */
static const char *msg01_1[3];

/*
 * The command lists, one entry per party mode and window: "Equip\nUnequip\nUse
 * T.Pts" behind a colour escape, "Formation\nReplace\nCancel" and
 * "Draw\nSet\nCancel". The three bytes after the escape of the first entry are
 * the grey level the Equip row is drawn in.
 */
static char * msg00_2_0036D8E8[3];

/* The three row-label text pointers at original .data address 0x0036D900. */
static const char *msg00_4_0036D900[3];

/* The text pointer of each of the eight stat labels, in the order the window
 * lays them out. */
static const char *msg_5[8];

/* Initialized data, in the original section order of each object (git HEAD 9cb54bc placement). */
static unsigned char setSlotTbl[] = {
    0, 1, 0, 0, 1, 0, 0, 0
};

static signed char ParaPointTbl[] = {
    10, 2, 1, 1, 1, 1, 1, 1
};

static const char *msg_0_0036D8A8[12] = {
    charactersLabel, equipCommandLabel, unequipCommandLabel, skillCommandLabel, listCommandLabel, weaponCommandLabel, accessoryCommandLabel, ammoCommandLabel, bodyCommandLabel, otherCommandLabel, drawCommandLabel, setCommandLabel
};

static const char *msg01_1[3] = {
    battleFormationLabel, formationCommandLabel, replaceCommandLabel
};

static char * msg00_2_0036D8E8[3] = {
    (char *)equipUnequipUsePtsText, (char *)formationReplaceCancelText, (char *)drawSetCancelText
};

static const char * msg01_3[] = {
    yesNoChoicesText, confirmPromptText
};

static const char *msg00_4_0036D900[3] = {
    menuParaPtRateGetTPtsText, menuParaPtRateGetEPtsText, menuParaPtRateGetSPtsText
};

static const char *msg_5[8] = {
    menuParaPtRateGetStrengthText, menuParaPtRateGetVitalityText, menuParaPtRateGetEatkText, menuParaPtRateGetEdefText, menuParaPtRateGetDexterityText, menuParaPtRateGetEvasionText, agilityText, weightText
};

static char * msg_6[] = {
    (char *)setLeadCharacterText
};

static const char *msg_7_0036D938[] = {
    currentText, totalText
};

/* The step one improvement adds to each parameter, and the character ids of
 * the party positions and of the reserve list. */
static unsigned short UpPara[8];

static unsigned short MenuCharParty[8];

/* The menu's two parameter records: MenuCharParaSet always writes the
 * recalculated record to [1] and, unless previewOnly, copies it to [0] too. */
static CharParaData paraUnit[2];

static unsigned short MenuCharPartyReser[4];

/* The two list windows of CharList the selection calls work on: the
 * weapon/accessory list (+0x504) and the parameter-up list (+0x1C3C). */
static WindowSPCursor *CharWinSP[2];

static DoublewordAlignedVector4 TairetuRot;

static DoublewordAlignedVector4 TairetuPos;

static inline int MenuCharCameraViewGet(int state)
{
    int view = 0;

    switch (state) {
    case 64:
    case 65:
    case 82:
    case 83:
    case 84:
    case 85:
        view = 1;
        break;
    }
    return view;
}

/* The rate table starts one character row (8 halfwords) before the row slot
 * ParaDataBuf addresses, so character chrNo (1-based) reads row chrNo - 1. */
unsigned short MenuParaPtRateGet(int chrNo, int statIndex)
{
    unsigned short *entry = ParaDataBuf + chrNo * 8 + ParaDataChangeTbl[statIndex] - 8;
    return *entry;
}

unsigned short MenuParaPtBaseGet(int chrNo, int statIndex)
{
    unsigned short *table = ParaDataBuf + 56;
    int slot = chrNo * 8 + ParaDataChangeTbl[statIndex];
    return table[slot];
}

/*
 * The character's current value of one displayed parameter (statIndex 0..7:
 * maxHp, maxEp, attack, phyDefense, stat4, magDefense, stat6, stat7); 0 for
 * any other index.
 */
unsigned short MenuParaPtNowGet(int chrNo, int statIndex)
{
    CharParaData *org = func_A191C0(chrNo);

    switch (statIndex) {
    case 2:
        return org->attack;
    case 3:
        return org->phyDefense;
    case 4:
        return org->stat4;
    case 5:
        return org->magDefense;
    case 6:
        return org->stat6;
    case 7:
        return org->stat7;
    case 0:
        return org->maxHp;
    case 1:
        return org->maxEp;
    }
    return 0;
}

/*
 * The highest current value of one parameter among the characters in the
 * party (ids 1..7), rounded down to a multiple of 10 for maxHp and of 2 for
 * maxEp.
 */
unsigned short MenuParaUpMaxGet(int statIndex)
{
    unsigned short max = 0;
    int chrNo;

    for (chrNo = 1; chrNo < 8; chrNo++) {
        if (PartyFriendCheck(chrNo) != 0) {
            unsigned short now = MenuParaPtNowGet(chrNo, statIndex);

            if (max < now) {
                max = now;
            }
        }
    }
    switch (statIndex) {
    case 0:
        max = max / 10 * 10;
        break;
    case 1:
        max = max / 2 * 2;
        break;
    }
    return max;
}

int MenuParaNextPointGet(int chrNo, int statIndex, int count)
{
    unsigned short base;
    unsigned short rate;
    unsigned short now;
    int increment;

    base = MenuParaPtBaseGet(chrNo, statIndex);
    rate = MenuParaPtRateGet(chrNo, statIndex);
    now = MenuParaPtNowGet(chrNo, statIndex);
    switch (statIndex) {
    case 0:
        increment = count * 10;
        break;
    case 1:
        increment = count * 2;
        break;
    default:
        now += count;
        goto calculateCost;
    }
    now += increment;

calculateCost:
    if (statIndex == 0) {
        unsigned short normalized = now / 10;
        return base + rate * normalized / 99;
    }
    return base + rate * now / 99;
}

/*
 * Spends one point raising statIndex by one step: the plain fields just
 * increment, while maxHp (+10) and maxEp (+2) are clamped back to the
 * party's current MenuParaUpMaxGet ceiling.
 */
void MenuParaUp(int chrNo, int statIndex)
{
    CharParaData *org = func_A191C0(chrNo);
    CharPointData *unit = func_A19210(chrNo);
    int next = MenuParaNextPointGet(chrNo, statIndex, 0);

    switch (statIndex) {
    case 2:
        org->attack++;
        break;
    case 3:
        org->phyDefense++;
        break;
    case 4:
        org->stat4++;
        break;
    case 5:
        org->magDefense++;
        break;
    case 6:
        org->stat6++;
        break;
    case 7:
        org->stat7++;
        break;
    case 0:
        org->maxHp += 10;
        if (org->maxHp > MenuParaUpMaxGet(statIndex)) {
            org->maxHp = MenuParaUpMaxGet(statIndex);
        }
        break;
    case 1:
        org->maxEp += 2;
        if (org->maxEp > MenuParaUpMaxGet(statIndex)) {
            org->maxEp = MenuParaUpMaxGet(statIndex);
        }
        break;
    }
    unit->points -= next;
}

/*
 * Whether the parameter can be raised once more: -2 when it has reached the
 * party maximum, -1 when the character lacks the points, 0 otherwise.
 */
int MenuParaUpCheck(int chrNo, int statIndex)
{
    CharPointData *unit;

    if (MenuParaPtNowGet(chrNo, statIndex) >= MenuParaUpMaxGet(statIndex)) {
        return -2;
    }
    unit = func_A19210(chrNo);
    if (unit->points < MenuParaNextPointGet(chrNo, statIndex, 0)) {
        return -1;
    }
    return 0;
}

int MenuCharWeaponAttCheck(void) {
    return 0;
}

int MenuCharLeaderMask(unsigned short chrNo)
{
    int scenario = MenuScenarioNo;

    if ((unsigned int)(scenario - 1) < 34 ||
        (unsigned int)(scenario - 46) < 3 ||
        scenario == 126 ||
        (unsigned int)(scenario - 142) < 3) {
        if (chrNo != 3) {
            return 1;
        }
    } else if ((unsigned int)(scenario - 159) < 2) {
        if (chrNo != 3 && chrNo != 1 && chrNo != 5) {
            return 1;
        }
    }

    if ((unsigned short)(chrNo - 1) >= 7) {
        return 1;
    }
    if (PartyAttackerCheck(chrNo) == 0) {
        return 2;
    }
    return (PartyLeaderCheck(chrNo) == 0) ? 0 : 3;
}

int MenuPasLengthGet(const char *text)
{
    char message[64];
    /* Text: {style 3} */
    char label[3] = "\x19\x03";

    eMessageCpy(message, label);
    eMessageCat(text);
    return xglFontGetStringWidth(message);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char accessoryCommandLabel[16] = "/Accessory";

const char unequipCommandLabel[16] = "/Unequip";

const char charactersLabel[16] = "Characters";

const char replaceCommandLabel[16] = "/Replace";

const char formationCommandLabel[16] = "/Formation";

const char battleFormationLabel[32] = "Battle Formation";

const char setCommandLabel[8] = "/Set";

const char drawCommandLabel[8] = "/Draw";

const char otherCommandLabel[8] = "/Other";

const char bodyCommandLabel[8] = "/Body";

const char ammoCommandLabel[8] = "/Ammo";

const char weaponCommandLabel[8] = "/Weapon";

const char listCommandLabel[8] = "/List";

const char skillCommandLabel[8] = "/Skill";

const char equipCommandLabel[8] = "/Equip";

/*
 * The category-tab strip at the top of the character menu. MenuWork.character.state
 * selects which sub-screen currently owns the display; CharPasMain hides
 * its own strip (slides it fully off-screen) unless MenuWork.character.state falls in
 * its own range, and highlights the incoming tab during the handful of
 * transition states that carry a category index.
 */
void CharPasMain(void)
{
    CharPasWork *pas = CharPas;
    int tabLength[12];
    int formTabLength[3];
    short tabTarget[16];
    short slideTarget;
    int i;

    for (i = 0; i < 12; i++)
        tabLength[i] = MenuPasLengthGet(msg_0_0036D8A8[i]);
    for (i = 0; i < 3; i++)
        formTabLength[i] = MenuPasLengthGet(msg01_1[i]);

    switch (pas->state) {
    case 0:
        pas->kind = 0x00FFFFF0;
        WindowDXSet(&pas->window);
        pas->window.x = -0x120;
        pas->window.y = 8;
        pas->window.style = pas->kind;
        if (MenuWork.character.partyMode == 1) {
            pas->window.width = 0x130;
        } else {
            pas->window.width = 0x110;
        }
        pas->window.height = 0x1E;
        /* The generic close callback slot holds the empty MenuPasWindow stub. */
        pas->window.close_callback = MenuPasWindow;
        pas->window.close_callback_arg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        if (MenuWork.character.partyMode == 0) {
            for (i = 0; i < 12; i++) {
                eMessageSet(&pas->messages[i], msg_0_0036D8A8[i]);
                pas->messages[i].mode = 0x20;
            }
            for (i = 0; i < 12; i++) {
                pas->messages[i].x = 0x120;
                pas->messages[i].y = 0xB;
                pas->messages[i].draw_parameter = pas->kind + 2;
            }
            pas->tabCount = 12;
        } else {
            for (i = 0; i < 3; i++) {
                eMessageSet(&pas->messages[i], msg01_1[i]);
                pas->messages[i].mode = 0x20;
            }
            for (i = 0; i < 3; i++) {
                pas->messages[i].x = 0x120;
                pas->messages[i].y = 0xB;
                pas->messages[i].draw_parameter = pas->kind + 2;
            }
            pas->tabCount = 3;
        }
        pas->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -0x10;
    for (i = 0; i < pas->tabCount; i++)
        tabTarget[i] = 0x120;

    switch (MenuWork.character.state) {
    case CHAR_STATE_EQUIP: case CHAR_STATE_EQUIP_RUN: {
        int curTab = MenuWork.character.menuCursor;

        tabTarget[0] = 0x10;
        tabTarget[curTab + 1] = tabLength[0] + 0x10;
        break;
    }
    case CHAR_STATE_EQUIP_LIST: case CHAR_STATE_EQUIP_LIST_RUN: {
        int curTab = MenuWork.character.menuCursor;

        tabTarget[0] = -(tabLength[0] + 0x10);
        tabTarget[curTab + 1] = 0x10;
        tabTarget[MenuWork.character.equipKind + 5] = tabLength[curTab + 1] + 0x10;
        break;
    }
    case CHAR_STATE_BULLET_LIST: case CHAR_STATE_BULLET_LIST_RUN: {
        int curTab = MenuWork.character.menuCursor;

        tabTarget[0] = -(tabLength[0] + 0x10);
        tabTarget[curTab + 1] = 0x10;
        tabTarget[7] = tabLength[curTab + 1] + 0x10;
        break;
    }
    case CHAR_STATE_STATUS: case CHAR_STATE_STATUS_RUN: case CHAR_STATE_COMMAND: case CHAR_STATE_COMMAND_RUN:
    case CHAR_STATE_PARA: case CHAR_STATE_PARA_RUN: case CHAR_STATE_PARA_AMOUNT: case CHAR_STATE_PARA_AMOUNT_RUN:
    case CHAR_STATE_PARA_CONFIRM: case CHAR_STATE_PARA_CONFIRM_RUN: case CHAR_STATE_PARA_DONE: case CHAR_STATE_PARA_DENIED:
    case CHAR_STATE_FORMATION: case CHAR_STATE_FORMATION_RUN: case CHAR_STATE_FORMATION_MENU: case CHAR_STATE_FORMATION_MENU_RUN:
        tabTarget[0] = 0x10;
        break;
    case CHAR_STATE_FORMATION_MOVE: case CHAR_STATE_FORMATION_MOVE_RUN: case CHAR_STATE_FORMATION_EXCHANGE: case CHAR_STATE_FORMATION_EXCHANGE_RUN: case CHAR_STATE_FORMATION_PLACE: case CHAR_STATE_FORMATION_DONE:
        tabTarget[0] = 0x10;
        tabTarget[1] = formTabLength[0] + 0x10;
        break;
    case CHAR_STATE_RESERVE: case CHAR_STATE_RESERVE_RUN: case CHAR_STATE_RESERVE_MODEL: case CHAR_STATE_RESERVE_MODEL_RUN:
        tabTarget[0] = 0x10;
        tabTarget[2] = formTabLength[0] + 0x10;
        break;
    default:
        slideTarget = -0x120;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain(&pas->window);
    pas->box.x = pas->window.x + 3;
    pas->box.y = pas->window.y + 3;
    pas->box.kind = pas->kind;
    pas->box.width = pas->window.width - 6;
    pas->box.height = pas->window.height - 6;
    endPrintExtFunc(pas->kind, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&pas->box);
    for (i = 0; i < pas->tabCount; i++) {
        MoveSlide(&pas->messages[i].x, &tabTarget[i], 5.0f);
        if (pas->messages[i].x < 0x100)
            eMessageMain(&pas->messages[i]);
    }
    endPrintExtFunc(pas->kind, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET,0);
}

/*
 * What the equipment the character wears adds to the parameter
 * MenuWork.character.paraIndex selects: the recalculated record minus the stored one,
 * plus the weapon attack word of the character's weapon slot or the defense
 * word calcTotalParaMenu reports separately. 0 for no character and for any
 * parameter outside the eight the window lists. The halfwords of the record
 * are read signed here (lh at 0x00291ee8 onwards), so the difference of the
 * three that CharParaData holds unsigned is taken on their signed value.
 */
int MenuCharEquipCalcPointGet(int chrNo)
{
    CharParaData *org;
    CharParaData *total;
    int attack[3];
    int defense[2];
    int points = 0;

    if (chrNo == 0) {
        return 0;
    }
    org = func_A191C0(chrNo);
    total = func_00A11108(chrNo, attack, defense);
    switch (MenuWork.character.paraIndex) {
        case 0:
            points = total->maxHp;
            points -= org->maxHp;
            break;
        case 1:
            points = total->maxEp;
            points -= org->maxEp;
            break;
        case 2:
            points = total->attack;
            points -= org->attack;
            points += attack[setSlotTbl[MenuWork.character.chrNo - 1]];
            break;
        case 3:
            points = total->phyDefense;
            points -= org->phyDefense;
            points += defense[0];
            break;
        case 4:
            points = total->stat4;
            points -= org->stat4;
            break;
        case 5:
            points = total->magDefense;
            points -= org->magDefense;
            points += defense[1];
            break;
        case 6:
            points = total->stat6;
            points -= org->stat6;
            break;
        case 7:
            points = total->stat7;
            points -= org->stat7;
            break;
    }
    return points;
}

void CharInfoMain(void)
{
    char *menuHelp[3] = {
        "Equip weapons and accessories.",
        "Unequip accessories.",
        "Use T.Pts to improve character stats."
    };
    char *equipHelp[4] = {
        "/",
        "Empty slot. A weapon can be equipped.",
        "Empty slot. An accessory can be equipped.",
        "Accessory"
    };
    char *charHelp[2] = {
        "Please select a character.",
        " can't be chosen."
    };
    char *partyHelp[3] = {
        "Change the battle formation of the combatant.",
        "Replace the combatant with reserves.",
        "Cancel."
    };
    char *formationHelp[6] = {
        /* Text: Select a new location for your character and press \x1E\x01. */
        "Select a new location for your character and press \x1E\x01.",
        "Change places with ",
        /* Text: Select a character to replace and press \x1E\x01. */
        "Select a character to replace and press \x1E\x01.",
        "Move ",
        /* Text: ?\nPress \x1E\x01 to accept. */
        "?\nPress \x1E\x01 to accept.",
        /* Text:  here?\nPress \x1E\x01 to accept. */
        " here?\nPress \x1E\x01 to accept."
    };
    /*
     * The confirmation of a reserve swap names two characters, and the table
     * keeps the pieces that go around those names twice: entry 0 what precedes
     * a name and entry 1 what follows it. The accept line comes in both rows
     * because a name that already ends in a period needs none of its own.
     */
    struct ReserveHelp {
        char *firstName;            /* the piece printed with the first name */
        char *secondName;           /* the piece printed with the second name */
        char *accept;               /* the line that asks for the button */
    } reserveHelp[2] = {
        /* Text: .\nPress \x1E\x01 to accept. */
        { "Remove ", "and replace with ", ".\nPress \x1E\x01 to accept." },
        /* Text: \nPress \x1E\x01 to accept. */
        { " ",       ".",                 "\nPress \x1E\x01 to accept." }
    };
    char *improveHelp[10] = {
        "Improve ",
        "Please specify points. \n",
        "The maximum for ",
        " is ",
        " will be increased from ",
        " to ",
        "",
        /* Text:  has been increased. \x1F */
        " has been increased. \x1F",
        "You do not have enough T.Pts for further improvements.",
        "This is the maximum for "
    };
    char *leaderHelp[5] = {
        "Select a character.",
        "A new on-screen character has been chosen.",
        "Reserves cannot be chosen as on-screen characters.",
        "This is the current on-screen character.",
        " is not available as an on-screen character."
    };
    CharInfoWork *work = CharInfo;
    int totals[3];
    short slideTarget;
    CharParaData *para;

    switch (work->state) {
    case CHAR_INFO_SETUP:
        work->color = 0x00FFFFF0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.style = work->color;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        /* The generic close callback slot holds the typed MenuInfoWindow handler. */
        work->window.close_callback = MenuInfoWindow;
        work->window.close_callback_arg = &work->info;
        work->info.x = 0;
        work->info.y = 0;
        work->info.text = 0;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        work->selected = -1;
        work->opened = 1;
        work->reasonShown = 0;
        memset(work->message, 0, sizeof(work->message));
        work->state = CHAR_INFO_MAIN;
        /* fallthrough */
    case CHAR_INFO_MAIN:
        para = func_00A11108(MenuWork.character.chrNo, totals, totals);
        slideTarget = 386;
        switch (MenuWork.character.state) {
        case CHAR_STATE_STATUS:
        case CHAR_STATE_STATUS_RUN:
            if (work->reasonShown == 0) {
                eMessageCpy(work->message, leaderHelp[0]);
                if (PadData.repeatFirst & PAD_SQUARE) {
                    switch (MenuWork.character.leaderLocked) {
                    case 0:
                        eMessageCpy(work->message, leaderHelp[1]);
                        break;
                    case 1:
                        eMessageCpy(work->message, MenuCharNameGet(MenuWork.character.chrNo));
                        eMessageCat(leaderHelp[4]);
                        break;
                    case 2:
                        eMessageCpy(work->message, leaderHelp[2]);
                        break;
                    case 3:
                        eMessageCpy(work->message, leaderHelp[3]);
                        break;
                    }
                    work->reasonShown = 1;
                }
            } else if (PadData.repeatFirst != 0) {
                work->reasonShown = 0;
            }
            break;
        case CHAR_STATE_COMMAND:
        case CHAR_STATE_COMMAND_RUN:
            eMessageCpy(work->message, menuHelp[MenuWork.character.menuCursor]);
            break;
        case CHAR_STATE_EQUIP:
        case CHAR_STATE_EQUIP_RUN:
            if (MenuWork.character.equipKind == 0) {
                int slot = setSlotTbl[MenuWork.character.chrNo - 1];
                short weapon = para->weapon[slot];

                if (weapon != 0) {
                    if (MenuWork.character.equipSlot == 0) {
                        eMessageCpy(work->message,
                                    MenuTextGet(MENU_TEXT_WEAPON + (unsigned short)weapon)->auxiliary);
                    } else {
                        eMessageCpy(work->message,
                                    MenuTextGet(MENU_TEXT_BULLET
                                                + para->bullet[slot])->auxiliary);
                    }
                } else {
                    eMessageCpy(work->message, equipHelp[1]);
                }
            } else {
                short accessory = para->accessory[MenuWork.character.equipSlot];

                if (accessory != 0) {
                    eMessageCpy(work->message,
                                MenuTextGet(MENU_TEXT_ACCESSORY + (unsigned short)accessory)->auxiliary);
                } else {
                    eMessageCpy(work->message, equipHelp[2]);
                }
            }
            break;
        case CHAR_STATE_EQUIP_LIST:
        case CHAR_STATE_EQUIP_LIST_RUN:
        case CHAR_STATE_BULLET_LIST:
        case CHAR_STATE_BULLET_LIST_RUN:
            if (MenuWork.character.listSelect >= 0) {
                eMessageCpy(work->message,
                            MenuTextGet(MenuSortGet(0, MenuWork.character.listSelect))->auxiliary);
            } else {
                eMessageCpy(work->message, "");
            }
            break;
        case CHAR_STATE_PARA:
        case CHAR_STATE_PARA_RUN:
            if (MenuWork.character.listSelect >= 0) {
                eMessageCpy(work->message, improveHelp[0]);
                eMessageCat(MenuParaNameGet(MenuWork.character.listSelect));
                eMessageCat(".");
            } else {
                eMessageCpy(work->message, "");
            }
            break;
        case CHAR_STATE_PARA_AMOUNT:
        case CHAR_STATE_PARA_AMOUNT_RUN:
            {
                int max = MenuCharEquipCalcPointGet(MenuWork.character.chrNo)
                          + MenuParaUpMaxGet(MenuWork.character.paraIndex);

                eMessageCpy(work->message, improveHelp[1]);
                eMessageCat(improveHelp[2]);
                eMessageCat(MenuParaNameGet(MenuWork.character.listSelect));
                eMessageCat(improveHelp[3]);
                eMessageCat(MenuNumberTextGet(max, -1, 0));
                eMessageCat(".");
            }
            break;
        case CHAR_STATE_PARA_CONFIRM:
        case CHAR_STATE_PARA_CONFIRM_RUN:
            {
                int now = MenuParaPtNowGet(MenuWork.character.chrNo, MenuWork.character.paraIndex)
                          + MenuCharEquipCalcPointGet(MenuWork.character.chrNo);
                int next = now + UpPara[MenuWork.character.paraIndex];

                eMessageCpy(work->message, MenuParaNameGet(MenuWork.character.paraIndex));
                eMessageCat(improveHelp[4]);
                eMessageCat(MenuNumberTextGet(now, -1, 0));
                eMessageCat(improveHelp[5]);
                eMessageCat(MenuNumberTextGet(next, -1, 0));
                eMessageCat(".");
            }
            break;
        case CHAR_STATE_PARA_DONE:
            eMessageCpy(work->message, MenuParaNameGet(MenuWork.character.listSelect));
            eMessageCat(improveHelp[7]);
            break;
        case CHAR_STATE_PARA_DENIED:
            if (MenuParaUpCheck(MenuWork.character.chrNo, MenuWork.character.paraIndex) == -1) {
                eMessageCpy(work->message, improveHelp[8]);
            } else {
                eMessageCpy(work->message, improveHelp[9]);
                eMessageCat(MenuParaNameGet(MenuWork.character.paraIndex));
                eMessageCat(".");
            }
            break;
        case CHAR_STATE_FORMATION:
            eMessageCpy(work->message, charHelp[0]);
            WindowDXFlagChange(&work->window, 1);
            break;
        case CHAR_STATE_FORMATION_RUN:
            break;
        case CHAR_STATE_FORMATION_MENU:
        case CHAR_STATE_FORMATION_MENU_RUN:
            eMessageCpy(work->message, partyHelp[MenuWork.character.commandCursor]);
            break;
        case CHAR_STATE_FORMATION_MOVE:
            eMessageCpy(work->message, formationHelp[0]);
            break;
        case CHAR_STATE_FORMATION_MOVE_RUN:
            break;
        case CHAR_STATE_FORMATION_EXCHANGE:
        case CHAR_STATE_FORMATION_EXCHANGE_RUN:
            eMessageCpy(work->message, formationHelp[1]);
            eMessageCat(MenuCharNameGet(PartyAttackPosCheck(MenuWork.character.formationSlot)));
            eMessageCat(formationHelp[4]);
            break;
        case CHAR_STATE_FORMATION_PLACE:
            eMessageCpy(work->message, formationHelp[3]);
            eMessageCat(MenuCharNameGet(MenuWork.character.chrNo));
            eMessageCat(formationHelp[5]);
            break;
        case CHAR_STATE_FORMATION_DONE:
            break;
        case CHAR_STATE_RESERVE_PICK:
            eMessageCpy(work->message, formationHelp[2]);
            break;
        case CHAR_STATE_RESERVE_PICK_RUN:
            break;
        case CHAR_STATE_RESERVE:
        case CHAR_STATE_RESERVE_RUN:
            {
                int partyText = MENU_TEXT_CHARACTER + MenuCharParty[MenuWork.character.partyCursor];
                int reserveText = MENU_TEXT_CHARACTER + MenuCharPartyReser[MenuWork.character.reserveCursor];

                eMessageCpy(work->message, reserveHelp[0].firstName);
                eMessageCat(MenuTextGet(partyText)->name);
                eMessageCat(reserveHelp[1].firstName);
                eMessageCat(reserveHelp[0].secondName);
                eMessageCat(MenuTextGet(reserveText)->name);
                if (strcmp(MenuTextGet(reserveText)->name, "Jr.") != 0) {
                    eMessageCat(reserveHelp[0].accept);
                } else {
                    eMessageCat(reserveHelp[1].accept);
                }
            }
            break;
        case CHAR_STATE_RESERVE_MODEL:
        case CHAR_STATE_RESERVE_MODEL_RUN:
            {
                int partyText = MENU_TEXT_CHARACTER + MenuCharParty[MenuWork.character.partyCursor];
                int reserveText = MENU_TEXT_CHARACTER + MenuCharPartyReser[MenuWork.character.reserveCursor];

                eMessageCpy(work->message, reserveHelp[0].firstName);
                eMessageCat(MenuTextGet(reserveText)->name);
                eMessageCat(reserveHelp[1].firstName);
                eMessageCat(reserveHelp[0].secondName);
                eMessageCat(MenuTextGet(partyText)->name);
                if (strcmp(MenuTextGet(partyText)->name, "Jr.") != 0) {
                    eMessageCat(reserveHelp[1].secondName);
                }
            }
            break;
        default:
            slideTarget = 512;
            eMessageCpy(work->message, "");
            break;
        }
        work->info.text = work->message;
        MoveSlide(&work->window.y, &slideTarget, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const CharCategoryIconIdTable charCategoryIconTable = {{0x0109, 0x010A, 0x0304, 0x0305, 0x0306}};

const char drawSetCancelText[16] = "Draw\nSet\nCancel";

const char formationReplaceCancelText[32] = "Formation\nReplace\nCancel";

/* Text: {color #808080}Equip\nUnequip\nUse T.Pts */
const char equipUnequipUsePtsText[32] = "\014\200\200\200Equip\nUnequip\nUse T.Pts";

const char confirmPromptText[24] = " Is this okay\?";

void CharCategoryMain(void)
{
    CharCategoryWork *work = CharCategory;
    int i;

    switch (work->state) {
    case 0: {
        CharCategoryIconIdTable ids = charCategoryIconTable;

        work->iconResetWork = 0x00FFFFFE;
        for (i = 0; i < 5; i++) {
            eSpriteSet(&work->icons[i], ids.id[i]);
            work->icons[i].work = work->iconResetWork;
        }
        work->pulseCounter = 8;
        work->flashStep = 1;
        work->state = 2;
        work->phase = 0;
        work->edgeDelta[1] = 0;
        work->edgeDelta[0] = 0;
        work->tickCounter = 0;
        work->flashLevel = 0;
        work->pendingReset = 0;
    }
    /* fallthrough */
    case 2: {
        int arcSpan;
        int rowBase;

        if (work->pendingReset == 0) {
            arcSpan = 58;
            rowBase = 45;
        } else {
            arcSpan = 288;
            rowBase = 32;
        }

        switch (MenuWork.character.state) {
        case CHAR_STATE_EQUIP_LIST:
            work->pendingReset = 0;
            /* fallthrough */
        case CHAR_STATE_EQUIP_LIST_RUN: {
            int equipKind = MenuWork.character.equipKind;

            if (equipKind == 1) {
                if (work->pulseCounter != 0) {
                    work->pulseCounter--;
                }
                work->flashStep = equipKind;
            } else {
                if (work->pulseCounter != 8) {
                    work->pulseCounter++;
                }
                work->flashStep = 0;
            }
            if (MenuWork.character.wait == 0) {
                if (PadData.repeat == PAD_LEFT) {
                    work->edgeDelta[0] = -6;
                }
                if (PadData.repeat == PAD_RIGHT) {
                    work->edgeDelta[1] = 6;
                }
            }
            break;
        }
        default:
            if (work->pulseCounter != 8) {
                work->pulseCounter++;
            }
            work->flashStep = 0;
            break;
        }

        if (work->edgeDelta[0] != 0) {
            work->edgeDelta[0]++;
        }
        if (work->edgeDelta[1] != 0) {
            work->edgeDelta[1]--;
        }

        if (work->tickCounter & 1) {
            work->flashLevel += work->flashStep;
            if (work->flashLevel >= 11) {
                work->flashStep = -work->flashStep;
            }
        }
        work->tickCounter++;

        {
            short rowY = rowBase + 83;

            for (i = 0; i < 2; i++) {
                float cosVal;
                int negTwicePulse;
                unsigned char edge; /* the pad nudge byte, applied signed */

                work->icons[i].x = arcSpan + i * 110;
                cosVal = xglCos((float)i * 3.1415927f);
                negTwicePulse = -(work->pulseCounter * 2);
                work->icons[i].x = (short)((float)work->icons[i].x + cosVal * (float)negTwicePulse);
                edge = work->edgeDelta[i];
                work->icons[i].y = rowY;
                work->icons[i].x += (signed char)edge;
                work->icons[i].alpha = (unsigned char)(128.0f - (float)work->pulseCounter * 0.125f * 128.0f);
            }

            for (i = 2; i < 5; i++) {
                float cosVal;
                int negTwicePulse;
                short x;

                work->icons[i].x = arcSpan + 36 + (i - 2) * 24;
                cosVal = xglCos((float)(i - 2) * 1.0471976f);
                negTwicePulse = -(work->pulseCounter * 2);
                work->icons[i].y = rowY;
                x = (short)((float)work->icons[i].x + cosVal * (float)negTwicePulse);
                work->icons[i].x = x;
                work->icons[i].alpha = (unsigned char)(128.0f - (float)work->pulseCounter * 0.125f * 128.0f);
                if (i - 2 == MenuWork.character.accessoryList) {
                    work->icons[i].color[2] = -0x80;
                    work->icons[i].color[1] = -0x80;
                    work->icons[i].color[0] = -0x80;
                } else {
                    work->icons[i].color[2] = 0x40;
                    work->icons[i].color[1] = 0x40;
                    work->icons[i].color[0] = 0x40;
                }
            }
        }

        for (i = 0; i < 5; i++) {
            eSpriteMain(&work->icons[i]);
        }
        break;
    }
    default:
        return;
    }
}

static void CharMenuMain(void)
{
    CharMenuWork *work = CharMenu;
    short target[3];
    int i;

    switch (work->state) {
    case 0:
        work->color = CHAR_MENU_COLOR;
        if (MenuWork.character.partyMode == 0) {
            work->windowCount = 3;
        } else {
            work->windowCount = 1;
        }
        for (i = 0; i < work->windowCount; i++) {
            WindowDXSet(&work->window[i]);
            work->window[i].x = 528;
            work->window[i].width = 126;
            work->window[i].height = 78;
            work->window[i].style = work->color;
            work->window[i].tag_id = menuTitleLabel;
            work->window[i].close_callback = MenuSelectWindow;
            work->window[i].close_callback_arg = &work->list[i];
            work->list[i].cursor = 0;
            if (i != 2) {
                work->list[i].hasPrompt = 0;
                work->list[i].items = msg00_2_0036D8E8[MenuWork.character.partyMode + i * 2];
            } else {
                work->window[i].x = 528;
                work->window[i].width = 166;
                work->window[i].height = 102;
                work->window[i].tag_id = selectTitleLabel;
                work->list[i].hasPrompt = 1;
                work->list[i].items = msg01_3[0];
                work->list[i].prompt = msg01_3[1];
            }
            work->window[i].state = 1;
            WindowDXMain(&work->window[i]);
            work->window[i].state = 3;
        }
        if (MenuWork.character.partyMode == 0) {
            work->window[0].y = 226;
            work->window[1].y = 224;
            work->window[2].y = 160;
        } else {
            work->window[0].y = 208;
        }
        work->subState = 0;
        work->state = 2;
        /* fallthrough */
    case 2:
        target[0] = target[1] = target[2] = 528;
        switch (MenuWork.character.state) {
        case 32:
        case 33:
            {
                char *row = msg00_2_0036D8E8[0] + 1;

                target[0] = 264;
                work->list[0].cursor = MenuWork.character.menuCursor;
                if (MenuMainCharCheck(MenuWork.character.chrNo) != 0) {
                    row[0] = row[1] = row[2] = CHAR_MENU_ROW_ON;
                } else {
                    row[0] = row[1] = row[2] = CHAR_MENU_ROW_OFF;
                }
            }
            break;
        case 132:
        case 133:
            target[2] = 330;
            work->list[2].cursor = MenuWork.character.confirmCursor;
            break;
        case 162:
        case 163:
            target[0] = 320;
            work->list[0].cursor = MenuWork.character.commandCursor;
            break;
        case 178:
        case 179:
        case 180:
        case 181:
        case 191:
            work->subState = 16;
            break;
        }
        for (i = 0; i < work->windowCount; i++) {
            MoveSlide(&work->window[i].x, &target[i], 3.0f);
            WindowDXMain(&work->window[i]);
        }
        break;
    }
}

void CharExMain(void)
{
    CharExWork *ex = CharEx;
    short slide[3];
    int i;

    switch (ex->state) {
    case CHAR_EX_START:
        ex->drawOrder = CHAR_EX_DRAW_ORDER;
        for (i = 0; i < 3; i++) {
            eRibbonSet(&ex->ribbon[i], 3);
            ex->ribbon[i].x = CHAR_EX_X_OUT;
            ex->ribbon[i].y = 128 + i * 28;
            ex->ribbon[i].width = 248;
            ex->ribbon[i].height = 23;
            ex->ribbon[i].work = ex->drawOrder;
            eTagFontSet(&ex->label[i], msg00_4_0036D900[i]);
            ex->label[i].work = ex->drawOrder + 2;
            eNumberSet(&ex->number[i], 0);
            ex->number[i].work = ex->drawOrder + 2;
        }
        ex->state = CHAR_EX_SLIDE;
        /* fallthrough */
    case CHAR_EX_SLIDE:
        for (i = 0; i < 3; i++) {
            slide[i] = CHAR_EX_X_OUT;
        }
        switch (MenuWork.character.state) {
        case CHAR_STATE_COMMAND_RUN:
        case CHAR_STATE_PARA:
        case CHAR_STATE_PARA_RUN:
        case CHAR_STATE_PARA_DONE:
        case CHAR_STATE_PARA_DENIED:
            slide[0] = slide[1] = slide[2] = CHAR_EX_X_IN;
            break;
        }
        for (i = 0; i < 3; i++) {
            CharPointData *unit;

            MoveSlide(&ex->ribbon[i].x, &slide[i], 3.0f);
            eRibbonMain(&ex->ribbon[i]);
            ex->label[i].x = ex->ribbon[i].x + 48;
            ex->label[i].y = ex->ribbon[i].y + 4;
            eTagFontMain(&ex->label[i]);
            unit = func_A19210(MenuWork.character.chrNo);
            ex->number[i].x = ex->label[i].x + 8;
            ex->number[i].y = ex->ribbon[i].y + 4;
            ex->number[i].digits = 4;
            ex->number[i].format = 0;
            switch (i) {
            case 0:
                ex->number[0].value = unit->points;
                break;
            case 1:
                ex->number[1].value = unit->etherPoints;
                break;
            case 2:
                ex->number[2].value = unit->skillPoints;
                break;
            }
            eNumberMain(&ex->number[i]);
        }
        break;
    }
}

void CharL1R1Main(void)
{
    CharL1R1Work *self = CharL1R1;
    signed char *slideOffset;
    int i;

    switch (self->state) {
    case 0: {
        L1R1SpriteIdPair spriteIds = l1r1SpriteIdTable[0];

        self->spriteWork = 0x00FFFFFF;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&self->sprite[i], spriteIds.id[i]);
            self->sprite[i].work = self->spriteWork;
            self->sprite[i].y = 214;
        }
        self->sprite[0].x = -45;
        self->sprite[1].x = 528;
        slideOffset = self->slideOffset;
        self->slideOffset[1] = 0;
        self->slideOffset[0] = 0;
        i = 2;
        self->state = i;
        break;
    }
    case 2:
        slideOffset = self->slideOffset;
        break;
    default:
        return;
    }

    {
        short target[2];
        int menuState = MenuWork.character.state;

        /* Both sprites sit just off screen unless the character menu is on
         * one of the two pages that show the L1/R1 page hints. */
        target[0] = -45;
        target[1] = 528;
        if (menuState < 34) {
            if (menuState >= 32) {
                target[0] = 8;
                target[1] = 475;
                if (PadData.pressed & PAD_L1) {
                    self->slideOffset[0] = -6;
                } else if (PadData.pressed & PAD_R1) {
                    self->slideOffset[1] = 6;
                }
            }
        }

        {
            L1R1SlideStep step = l1r1SlideStepTable[0];

            for (i = 0; i < 2; i++) {
                if (self->slideOffset[i] != 0) {
                    self->slideOffset[i] = self->slideOffset[i] + step.side[i];
                }
            }
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&self->sprite[i].x, &target[i], 3.0f);
            self->sprite[i].x = self->sprite[i].x + slideOffset[i];
            eSpriteMain(&self->sprite[i]);
        }
    }
}

void CharStatusMain(void)
{
    CharStatusBlock *block = CharStatus;
    CharStatusWork *status = &block->work;
    short targetX[8];
    short targetY[8];
    short targetHeight[8];
    short targetOffset[8];
    CharStatusCursorNode *swapNode;
    int para[4];
    int i;

    switch (status->state) {
    case CHAR_STATUS_BUILD:
        status->color = 0x00FFF000;
        if (MenuWork.character.partyMode == 0) {
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                short chrNo = MenuCharParty[i];

                WindowDXSet(&status->window[i]);
                status->window[i].x = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                status->window[i].y = (i / 2) * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                status->window[i].style = status->color;
                status->window[i].width = STATUS_WINDOW_WIDTH;
                status->window[i].height = STATUS_WINDOW_HEIGHT;
                status->window[i].ribbon_initialized = 1;
                if (PartyAttackerCheck(chrNo) != 0) {
                    status->window[i].tag_id = MenuTagTextGet(1);
                } else {
                    status->window[i].tag_id = MenuTagTextGet(2);
                }
                status->window[i].close_callback = MenuStatusDisp;
                status->window[i].close_callback_arg = &status->panel[i];
                status->panel[i].offsetX = 0;
                status->panel[i].enabled = 1;
                status->panel[i].chrNo = chrNo;
                status->panel[i].offsetY = 0;
                status->window[i].state = 1;
                WindowDXMain(&status->window[i]);
                status->window[i].state = 3;
            }
        } else {
            for (i = 0; i < 3; i++) {
                short chrNo = MenuCharParty[i];

                WindowDXSet(&status->window[i]);
                status->window[i].x = STATUS_OFF_LEFT;
                status->window[i].y = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                status->window[i].style = status->color;
                status->window[i].width = STATUS_WINDOW_WIDTH;
                status->window[i].height = STATUS_WINDOW_HEIGHT;
                status->window[i].ribbon_initialized = 1;
                status->window[i].tag_id = MenuTagTextGet(1);
                status->window[i].close_callback = MenuStatusDisp;
                status->window[i].close_callback_arg = &status->panel[i];
                status->panel[i].chrNo = chrNo;
                status->panel[i].enabled = 0;
                status->panel[i].offsetX = 0;
                status->panel[i].offsetY = 0;
                status->window[i].state = 1;
                WindowDXMain(&status->window[i]);
                status->window[i].state = 3;
            }
            for (i = 0; i < MenuWork.character.reserveCount; i++) {
                short chrNo = MenuCharPartyReser[i];

                WindowDXSet(&status->window[3 + i]);
                status->window[3 + i].x = 544;
                status->window[3 + i].width = STATUS_WINDOW_WIDTH;
                status->window[3 + i].height = STATUS_WINDOW_HEIGHT;
                status->window[3 + i].style = status->color;
                status->window[3 + i].y = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                status->window[3 + i].ribbon_initialized = 1;
                status->window[3 + i].tag_id = MenuTagTextGet(2);
                status->window[3 + i].close_callback = MenuStatusDisp;
                status->window[3 + i].close_callback_arg = &status->panel[3 + i];
                status->panel[3 + i].chrNo = chrNo;
                status->panel[3 + i].enabled = 0;
                status->panel[3 + i].offsetX = 0;
                status->panel[3 + i].offsetY = 0;
                status->window[3 + i].state = 1;
                WindowDXMain(&status->window[3 + i]);
                status->window[3 + i].state = 3;
            }
        }
        for (i = 0; i < 2; i++) {
            eCursolSet(&status->cursor[i], 0);
            if (i == 1) {
                swapNode = &block->node.swap;
                swapNode->state = CHAR_STATUS_CURSOR_STATE;
            } else {
                status->cursor[i].state = 0;
            }
            status->cursor[i].work = CHAR_STATUS_CURSOR_WORK;
        }
        status->cursorOn = 0;
        status->flags = 0;
        status->subState = 0;
        status->moveVert = 0;
        status->lastCursor = MenuWork.character.partyCursor;
        status->state = CHAR_STATUS_RUN;
        /* fall through */
    case CHAR_STATUS_RUN:
        if (MenuWork.character.partyMode == 0) {
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                targetX[i] = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                targetY[i] = (i / 2) * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                targetHeight[i] = STATUS_WINDOW_HEIGHT;
                targetOffset[i] = 0;
            }
        } else {
            for (i = 0; i < 3; i++) {
                targetX[i] = STATUS_OFF_LEFT;
                targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                targetHeight[i] = STATUS_WINDOW_HEIGHT;
                targetOffset[i] = 0;
            }
            for (i = 0; i < MenuWork.character.reserveCount; i++) {
                targetX[3 + i] = 544;
                targetY[3 + i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                targetHeight[3 + i] = STATUS_WINDOW_HEIGHT;
                targetOffset[3 + i] = 0;
            }
        }
        status->panel[MenuWork.character.partyCursor].hpMark =
            status->panel[MenuWork.character.partyCursor].epMark = 0;
        switch (MenuWork.character.state) {
        case CHAR_STATE_STATUS:
            status->cursorOn = 1;
            status->cursor[0].state = CHAR_STATUS_CURSOR_STATE;
            status->moveVert = 1;
            if ((status->flags & 1) != 0) {
                if (i != MenuWork.character.partyCursor) {
                    status->window[i].x = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                    status->window[i].y = (i / 2) * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                }
                status->flags &= ~1;
            }
            /* fall through */
        case CHAR_STATE_STATUS_RUN:
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                targetX[i] = (i & 1) * STATUS_COLUMN_WIDTH + STATUS_COLUMN_LEFT;
            }
            break;
        case CHAR_STATE_COMMAND:
            status->lastCursor = MenuWork.character.partyCursor;
            if (MenuWork.character.state != CHAR_STATE_COMMAND) {
                for (i = 0; i < MenuWork.character.partyCount; i++) {
                    targetHeight[i] = STATUS_WINDOW_HEIGHT;
                }
            }
            /* fall through */
        case CHAR_STATE_PARA:
        case CHAR_STATE_PARA_RUN:
        case CHAR_STATE_PARA_AMOUNT:
        case CHAR_STATE_PARA_AMOUNT_RUN:
        case CHAR_STATE_PARA_CONFIRM:
        case CHAR_STATE_PARA_CONFIRM_RUN:
        case CHAR_STATE_PARA_DONE:
        case CHAR_STATE_PARA_DENIED: {
            CharParaData *total;
            int menuState;
            int mark;

            total = func_00A11108(MenuWork.character.chrNo, para, para);
            mark = 1;
            status->panel[MenuWork.character.partyCursor].maxHp = total->maxHp + UpPara[0];
            status->panel[MenuWork.character.partyCursor].maxEp = total->maxEp + UpPara[1];
            menuState = MenuWork.character.state;
            if (menuState < 0x84) {
                if (menuState >= 0x82) {
                    mark = 2;
                }
            }
            switch (MenuWork.character.paraIndex) {
            case 0:
                status->panel[MenuWork.character.partyCursor].hpMark = mark;
                break;
            case 1:
                status->panel[MenuWork.character.partyCursor].epMark = mark;
                break;
            }
        }
            /* fall through */
        case CHAR_STATE_COMMAND_RUN:
            if (status->lastCursor != MenuWork.character.partyCursor) {
                status->window[MenuWork.character.partyCursor].x = 264;
                status->window[MenuWork.character.partyCursor].y = 24;
                status->window[MenuWork.character.partyCursor].height = STATUS_WINDOW_OPEN_H;
                status->lastCursor = MenuWork.character.partyCursor;
            }
            if ((status->flags & 1) != 0) {
                for (i = 0; i < MenuWork.character.partyCount; i++) {
                    if (i == MenuWork.character.partyCursor) {
                        targetX[i] = 264;
                        targetY[i] = 24;
                        targetHeight[i] = STATUS_WINDOW_OPEN_H;
                    } else {
                        targetX[i] = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                        status->window[i].x = targetX[i];
                    }
                }
            } else {
                for (i = 0; i < MenuWork.character.partyCount; i++) {
                    if (i == MenuWork.character.partyCursor) {
                        targetX[i] = 264;
                        targetY[i] = 24;
                        targetHeight[i] = STATUS_WINDOW_OPEN_H;
                    }
                }
                if (targetY[MenuWork.character.partyCursor] == status->window[MenuWork.character.partyCursor].y) {
                    status->flags |= 1;
                }
            }
            status->cursorOn = 0;
            status->moveVert = 0;
            break;
        case CHAR_STATE_EQUIP:
        case CHAR_STATE_EQUIP_RUN:
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                if (i == MenuWork.character.partyCursor) {
                    targetX[i] = 544;
                    targetY[i] = 24;
                    targetHeight[i] = STATUS_WINDOW_OPEN_H;
                    targetOffset[i] = -80;
                } else {
                    targetX[i] = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                    status->window[i].x = targetX[i];
                }
            }
            break;
        case CHAR_STATE_EQUIP_LIST:
        case CHAR_STATE_EQUIP_LIST_RUN:
        case CHAR_STATE_BULLET_LIST:
        case CHAR_STATE_BULLET_LIST_RUN:
            status->moveVert = 0;
            status->flags &= ~1;
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                targetX[i] = 544;
                if (i == MenuWork.character.partyCursor) {
                    targetY[i] = 192;
                    targetHeight[i] = STATUS_WINDOW_OPEN_H;
                } else {
                    status->window[i].x = 544;
                }
            }
            targetOffset[MenuWork.character.partyCursor] = -80;
            break;
        case CHAR_STATE_FORMATION:
        case CHAR_STATE_FORMATION_RUN:
        case CHAR_STATE_FORMATION_MENU:
        case CHAR_STATE_FORMATION_MENU_RUN:
        case CHAR_STATE_FORMATION_MOVE:
        case CHAR_STATE_FORMATION_MOVE_RUN:
        case CHAR_STATE_FORMATION_EXCHANGE:
        case CHAR_STATE_FORMATION_EXCHANGE_RUN:
        case CHAR_STATE_FORMATION_PLACE:
        case CHAR_STATE_FORMATION_DONE:
            status->cursor[0].state = CHAR_STATUS_CURSOR_STATE;
            for (i = 0; i < 3; i++) {
                targetX[i] = STATUS_COLUMN_LEFT;
                targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
            }
            break;
        case CHAR_STATE_RESERVE:
        case CHAR_STATE_RESERVE_MODEL_RUN:
            if (MenuWork.character.state == CHAR_STATE_RESERVE) {
                for (i = 0; i < MenuWork.character.reserveCount; i++) {
                    status->panel[3 + i].chrNo = MenuCharPartyReser[i];
                    status->window[3 + i].state = 1;
                    WindowDXMain(&status->window[3 + i]);
                    status->window[3 + i].state = 3;
                }
            } else {
                status->panel[MenuWork.character.partyCursor].chrNo = MenuWork.character.swapInId;
                status->window[MenuWork.character.partyCursor].state = 1;
                WindowDXMain(&status->window[MenuWork.character.partyCursor]);
                status->window[MenuWork.character.partyCursor].state = 3;
            }
            /* fall through */
        case CHAR_STATE_RESERVE_RUN:
            for (i = 0; i < 3; i++) {
                targetX[i] = STATUS_COLUMN_LEFT;
                targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
            }
            if (MenuWork.character.state == CHAR_STATE_RESERVE_RUN) {
                for (i = 0; i < MenuWork.character.reserveCount; i++) {
                    targetX[3 + i] = 258;
                    targetY[3 + i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                }
            }
            break;
        case CHAR_STATE_RESERVE_MODEL:
            for (i = 0; i < 3; i++) {
                if (i != MenuWork.character.partyCursor) {
                    targetX[i] = STATUS_COLUMN_LEFT;
                    targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                }
            }
            break;
        }
        if (MenuWork.character.partyMode == 0) {
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                if (status->moveVert == 0) {
                    MoveSlide(&status->window[i].height, &targetHeight[i], 3.0f);
                    MoveSlide(&status->window[i].x, &targetX[i], 3.0f);
                    if (status->window[i].x == targetX[i]) {
                        MoveSlide(&status->window[i].y, &targetY[i], 3.0f);
                    }
                } else {
                    MoveSlide(&status->window[i].y, &targetY[i], 3.0f);
                    if (status->window[i].y == targetY[i]) {
                        MoveSlide(&status->window[i].height, &targetHeight[i], 3.0f);
                        MoveSlide(&status->window[i].x, &targetX[i], 3.0f);
                    }
                }
                if (i == MenuWork.character.partyCursor) {
                    status->window[i].style = CHAR_STATUS_COLOR_SELECTED;
                } else {
                    status->window[i].style = CHAR_STATUS_COLOR_NORMAL;
                }
                WindowDXMain(&status->window[i]);
            }
            if (status->cursorOn != 0) {
                int open = 0;

                for (i = 0; i < MenuWork.character.partyCount; i++) {
                    if (status->window[i].state == 3) {
                        open = 1;
                        break;
                    }
                }
                if (open != 0) {
                    status->cursor[0].x = status->window[MenuWork.character.partyCursor].x + 3;
                    status->cursor[0].y = status->window[MenuWork.character.partyCursor].y
                                        + status->window[MenuWork.character.partyCursor].height / 2 - 4;
                    status->cursor[0].work = CHAR_STATUS_CURSOR_WORK;
                    eCursolMain(&status->cursor[0]);
                }
            }
        } else {
            for (i = 0; i < MenuWork.character.partyCount; i++) {
                MoveSlide(&status->window[i].x, &targetX[i], 3.0f);
                WindowDXMain(&status->window[i]);
            }
            for (i = 0; i < MenuWork.character.reserveCount; i++) {
                MoveSlide(&status->window[3 + i].x, &targetX[3 + i], 3.0f);
                WindowDXMain(&status->window[3 + i]);
            }
            status->cursor[0].x = status->window[MenuWork.character.partyCursor].x + 3;
            status->cursor[0].y = status->window[MenuWork.character.partyCursor].y
                                + status->window[MenuWork.character.partyCursor].height / 2 - 4;
            eCursolMain(&status->cursor[0]);
            if (MenuWork.character.partyMode == 1 && MenuWork.character.reserveCount != 0) {
                status->cursor[1].x = status->window[3 + MenuWork.character.reserveCursor].x + 3;
                status->cursor[1].y = status->window[3 + MenuWork.character.reserveCursor].y
                                    + status->window[3 + MenuWork.character.reserveCursor].height / 2 - 4;
                eCursolMain(&status->cursor[1]);
            }
        }
        break;
    }
}

void CharParameterMain(void)
{
    CharParameterWork *work = CharParameter;
    int i;

    switch (work->state) {
    case 0:
        {
            ParaDigitCounts digits;
            int corner;

            work->color = 0x00FFFFF0;
            WindowDXSet(&work->window);
            work->window.x = PARA_WINDOW_HIDDEN;
            work->window.style = work->color;
            work->window.y = 313;
            work->window.width = 480;
            work->window.height = 64;
            work->window.state = 1;
            WindowDXMain(&work->window);
            work->window.state = 3;
            digits = paraDigitCountTable[0];
            for (i = 0; i < 8; i++) {
                eNumberSet(&work->number[i], 0);
                work->number[i].work = work->color + 2;
                work->number[i].format = 5;
                work->number[i].digits = digits.count[i];
            }
            for (i = 0; i < 8; i++) {
                eTagFontSet(&work->tag[i], msg_5[i]);
                work->tag[i].work = work->color + 2;
                work->tag[i].color[0] = 112;
            }
            work->ribbonShown = 0;
            eRibbonSet(&work->ribbon, 0);
            for (corner = 0; corner < 6; corner++) {
                ERibbonPoint *point = &work->ribbon.points[corner];

                switch (corner) {
                case 0:
                case 1:
                case 2:
                case 3:
                    point->red = 32;
                    point->green = 52;
                    point->blue = 52;
                    break;
                case 4:
                case 5:
                    point->blue = 0;
                    point->green = 0;
                    point->red = 0;
                    break;
                }
                if (corner == 2 || corner == 3) {
                    point->alpha = 32;
                } else {
                    point->alpha = 0;
                }
            }
            work->mask = 1;
            work->selectNo = -1;
            work->state = 2;
        }
        /* fallthrough */

    case 2:
        {
            short targetX;

            targetX = PARA_WINDOW_HIDDEN;
            switch (MenuWork.character.state) {
            case 82:
                work->mask = 7;
                /* fallthrough */
            case 83:
                if (CharWinSP[0]->state == 3) {
                    work->mask = 15;
                } else {
                    work->mask = 7;
                }
                targetX = PARA_WINDOW_SHOWN;
                break;
            case 32:
                work->mask = 7;
                /* fallthrough */
            case 33:
                targetX = PARA_WINDOW_SHOWN;
                break;
            case 64:
            case 65:
            case 84:
            case 85:
            case 128:
            case 129:
            case 130:
            case 131:
            case 132:
            case 133:
            case 134:
            case 135:
                targetX = PARA_WINDOW_SHOWN;
                break;
            }
            switch (MenuWork.character.state) {
            case 130:
            case 131:
                if (MenuWork.character.paraIndex >= 2) {
                    work->ribbonShown = 1;
                } else {
                    work->ribbonShown = 0;
                }
                break;
            default:
                work->ribbonShown = 0;
                break;
            }
            MoveSlide(&work->window.x, &targetX, 3.0f);
            WindowDXMain(&work->window);
            if (work->ribbonShown == 1) {
                const int rows = 3;
                ERibbonMainData *ribbon;

                work->ribbon.x = (MenuWork.character.paraIndex - 2) / rows * 160 -
                                 (rows - (MenuWork.character.paraIndex - 2) / 4) * 11 + 104;
                work->ribbon.y = (MenuWork.character.paraIndex - 2) % rows * 20 + 316;
                work->ribbon.width = (rows - (MenuWork.character.paraIndex - 2) / 4) * 22 + 23;
                work->ribbon.height = 16;
                work->ribbon.work = work->color;
                ribbon = &work->ribbon;
                for (i = 5; i >= 0; i--) {
                    eRibbonMain(ribbon);
                }
            }
            if (work->window.state == 3) {
                const int rows = 3;

                for (i = 0; i < 7; i++) {
                    work->tag[i].x = work->window.x + i / rows * 160 + 48;
                    work->tag[i].y = work->window.y + i % rows * 20 + 3;
                    eTagFontMain(&work->tag[i]);
                }
                /* The four battle parameters are stored unsigned (lhu in
                 * MenuParaPtNowGet and MenuCharParaSet) but the pair of
                 * halfwords eNumberMain shows is read signed here (the eight
                 * lh at 0x00294cb0..0x00294cf8), so a value below its base
                 * shows as a negative difference. */
                work->number[0].value = paraUnit[0].attack +
                                        ((paraUnit[1].attack + UpPara[2]) << 16);
                work->number[1].value = paraUnit[0].phyDefense +
                                        ((paraUnit[1].phyDefense + UpPara[3]) << 16);
                work->number[2].value = paraUnit[0].stat4 +
                                        ((paraUnit[1].stat4 + UpPara[4]) << 16);
                work->number[3].value = paraUnit[0].magDefense +
                                        ((paraUnit[1].magDefense + UpPara[5]) << 16);
                work->number[4].value = paraUnit[0].stat6 +
                                        ((paraUnit[1].stat6 + UpPara[6]) << 16);
                work->number[5].value = paraUnit[0].stat7 +
                                        ((paraUnit[1].stat7 + UpPara[7]) << 16);
                work->number[6].value = paraUnit[0].stat8 + (paraUnit[1].stat8 << 16);
                work->number[7].value = paraUnit[0].stat9 + (paraUnit[1].stat9 << 16);
                for (i = 0; i < 7; i++) {
                    short shown[2];

                    shown[0] = work->number[i].value;
                    shown[1] = work->number[i].value >> 16;
                    if (shown[0] < shown[1]) {
                        work->number[i].firstOnly = 0;
                        work->number[i].colorModeSecond = 2;
                    } else if (shown[1] < shown[0]) {
                        work->number[i].colorModeSecond = 1;
                        work->number[i].firstOnly = 0;
                    } else {
                        work->number[i].colorModeSecond = 0;
                        if (MenuWork.character.paraIndex - 2 == i) {
                            switch (MenuWork.character.state) {
                            case 128:
                            case 129:
                            case 130:
                            case 131:
                            case 132:
                            case 133:
                            case 135:
                                work->number[i].firstOnly = 0;
                                break;
                            default:
                                work->number[i].firstOnly = 1;
                                break;
                            }
                        } else {
                            work->number[i].firstOnly = 1;
                        }
                    }
                    work->number[i].x = work->window.x + i / rows * 160 + 89;
                    work->number[i].y = work->window.y + i % rows * 20 + 3;
                    eNumberMain(&work->number[i]);
                }
            }
        }
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const WeaponIconTable charWeaponIconTable = {{0x0307, 0x0308, 0x032C, 0, 0x0308, 0x0309, 0x030A}};

void CharWeaponMain(void)
{
    CharWeaponWork *work = CharWeapon;
    CharParaData *total;
    int attack[3];
    short targetX[2];
    short targetY[2];
    int bulletFits;
    short weaponId;
    short spriteId;
    int i;

    switch (work->state) {
    case 0:
        work->color = MENU_WEAPON_COLOR;
        WindowDXSet(&work->window[0]);
        work->window[0].x = -238;
        work->window[0].y = 48;
        work->window[0].style = work->color;
        work->window[0].width = 228;
        work->window[0].height = 54;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&work->sprite[i], 0);
            eMessageSet(&work->message[i], 0);
            work->message[i].mode = 32;
        }
        work->window[0].state = 1;
        WindowDXMain(&work->window[0]);
        work->window[0].state = 3;
        WindowDXSet(&work->window[1]);
        work->window[1].x = -238;
        work->window[1].y = 102;
        work->window[1].style = work->color;
        work->window[1].width = 228;
        work->window[1].height = 78;
        for (i = 0; i < 3; i++) {
            eSpriteSet(&work->sprite[i + 2], 0);
            eMessageSet(&work->message[i + 2], 0);
            work->message[i + 2].mode = 32;
        }
        work->window[1].state = 1;
        WindowDXMain(&work->window[1]);
        work->window[1].state = 3;
        eCursolSet(&work->cursor, 0);
        work->cursor.work = work->color + 2;
        work->cursor.state = 32;
        work->state = 2;
        work->subState = 0;
        /* fallthrough */
    case 2:
        total = func_00A11108(MenuWork.character.chrNo, attack, attack);
        for (i = 0; i < 2; i++) {
            targetX[i] = -238;
            targetY[i] = i * 54 + 48;
        }
        switch (MenuWork.character.state) {
        case CHAR_STATE_STATUS:
        case CHAR_STATE_STATUS_RUN:
        case CHAR_STATE_COMMAND:
        case CHAR_STATE_COMMAND_RUN:
            work->cursor.state = 0;
            break;
        case CHAR_STATE_EQUIP:
            work->cursor.state = 32;
            /* fallthrough */
        case CHAR_STATE_EQUIP_RUN:
            for (i = 0; i < 2; i++) {
                targetX[i] = 16;
            }
            break;
        case CHAR_STATE_EQUIP_LIST:
        case CHAR_STATE_EQUIP_LIST_RUN:
        case CHAR_STATE_BULLET_LIST:
        case CHAR_STATE_BULLET_LIST_RUN:
            targetX[MenuWork.character.equipKind] = 16;
            targetY[MenuWork.character.equipKind] = 48;
            break;
        }
        weaponId = total->weapon[setSlotTbl[MenuWork.character.chrNo - 1]];
        bulletFits = MenuBulletCheck2(weaponId, MenuWork.character.chrNo);
        MoveSlide(&work->window[0].x, &targetX[0], 3.0f);
        MoveSlide(&work->window[0].y, &targetY[0], 3.0f);
        if (MenuMainCharCheck(MenuWork.character.chrNo) != 0) {
            WindowDXMain(&work->window[0]);
            work->message[0].text = MenuTextGet(MENU_TEXT_WEAPON + (unsigned short)weaponId)->name;
            work->message[0].x = work->window[0].x + 39;
            work->message[0].draw_parameter = work->color + 2;
            work->message[0].y = work->window[0].y + (bulletFits ^ 1) * MENU_WEAPON_LINE_DROP + 3;
            eMessageMain(&work->message[0]);
            {
                WeaponIconTable weaponIcon = charWeaponIconTable;

                eSpriteSet(&work->sprite[0], weaponIcon.icon[MenuWork.character.chrNo - 1]);
            }
            work->sprite[0].x = work->window[0].x + 19;
            work->sprite[0].work = work->color + 2;
            work->sprite[0].y = work->window[0].y + (bulletFits ^ 1) * MENU_WEAPON_LINE_DROP + 3;
            eSpriteMain(&work->sprite[0]);
            if (bulletFits != 0) {
                work->message[1].text =
                    MenuTextGet(MENU_TEXT_BULLET + total->bullet[setSlotTbl[MenuWork.character.chrNo - 1]])->name;
                work->message[1].x = work->window[0].x + 39;
                work->message[1].draw_parameter = work->color + 2;
                work->message[1].y = work->window[0].y + 27;
                eMessageMain(&work->message[1]);
                eSpriteSet(&work->sprite[1], SPRITE_BULLET);
                work->sprite[1].x = work->window[0].x + 19;
                work->sprite[1].work = work->color + 2;
                work->sprite[1].y = work->window[0].y + 27;
                eSpriteMain(&work->sprite[1]);
            }
        }
        MoveSlide(&work->window[1].x, &targetX[1], 3.0f);
        MoveSlide(&work->window[1].y, &targetY[1], 3.0f);
        WindowDXMain(&work->window[1]);
        if (work->window[1].state == 3) {
            for (i = 0; i < 3; i++) {
                work->message[i + 2].text =
                    MenuTextGet(MENU_TEXT_ACCESSORY + (unsigned short)total->accessory[i])->name;
                work->message[i + 2].x = work->window[1].x + 39;
                work->message[i + 2].y = work->window[1].y + i * MENU_WEAPON_ROW_HEIGHT + 3;
                work->message[i + 2].draw_parameter = work->color + 2;
                eMessageMain(&work->message[i + 2]);
                spriteId = SPRITE_NO_ACCESSORY;
                if (total->accessory[i] != 0) {
                    AccessoryIconTable accessoryIcon = charAccessoryIconTable[0];

                    spriteId = accessoryIcon.icon[func_A1A548(total->accessory[i])->iconKind];
                }
                eSpriteSet(&work->sprite[i + 2], spriteId);
                work->sprite[i + 2].x = work->window[1].x + 19;
                work->sprite[i + 2].work = work->color + 2;
                work->sprite[i + 2].y = work->window[1].y + i * MENU_WEAPON_ROW_HEIGHT + 3;
                eSpriteMain(&work->sprite[i + 2]);
            }
        }
        work->cursor.x = work->window[MenuWork.character.equipKind].x + 3;
        work->cursor.y = work->window[MenuWork.character.equipKind].y +
                         MenuWork.character.equipSlot * MENU_WEAPON_ROW_HEIGHT + 7;
        if (MenuWork.character.equipKind == 0) {
            work->cursor.y += (bulletFits ^ 1) * MENU_WEAPON_LINE_DROP;
        }
        eCursolMain(&work->cursor);
        break;
    }
}

void CharListMake_Wpn(void)
{
    WindowSPCursor *window = &CharList->weapon;

    /* MenuSortSet's order argument is MenuWork.character.chrNo (+0x60), the character
     * whose equipment the screen lists. */
    MenuSortSet(0, 4, MenuWork.character.chrNo);
    MenuListMake(0, 0);
    window->width = 228;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->draw_flags = 7;
    window->title = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[15]);
}

/* The bullet list: the same window and geometry as the weapon list, with each
 * row flagged when its bullet does not fit the weapon under the cursor. */
void CharListMake_Gun(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *entries = MenuListGet(0);
    WindowSPCursor *window = &CharList->weapon;
    int count;
    int i;

    MenuSortSet(0, 8, MenuWork.character.chrNo);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (func_A1A428((unsigned short)sortRow[i])->weapon != MenuWork.character.weaponId) {
            entries[i].flag = 1;
        } else {
            entries[i].flag = 0;
        }
    }
    window->width = 228;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->draw_flags = 7;
    window->title = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[20]);
}

/* The accessory list: the same window as the weapon list, five shorter rows,
 * with each row flagged when that accessory is not already equipped in the
 * slot under the cursor. The sort order and the saved selection both follow
 * MenuWork.character.accessoryList, the accessory sub-tab. */
void CharListMake_Acc(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *entries = MenuListGet(0);
    WindowSPCursor *window = &CharList->weapon;
    int count;
    int i;

    MenuSortSet(0, 16, MenuWork.character.accessoryList);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuAccessoryEquipCheck(MenuWork.character.chrNo, (short)sortRow[i],
                                    MenuWork.character.equipSlot) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 228;
    window->height = 126;
    window->columns = 1;
    window->rows = 5;
    window->draw_flags = 7;
    window->title = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[MenuWork.character.accessoryList * 5]);
}

/* The parameter-up list: one row per improvable parameter, flagged while the
 * shown character can still raise it, then the cost row and a terminating
 * empty row. Its rows live in CharList itself, so the window takes them
 * directly instead of through MenuListMake. */
void CharListMake_Para(void)
{
    MenuListEntry *row = CharList->paraItems;
    WindowSPCursor *window = &CharList->weapon;
    int i;

    for (i = 0; i < 8; i++) {
        row->name = MenuParaNameGet2(i);
        if (MenuParaUpCheck(MenuWork.character.chrNo, i) == 0) {
            row->flag = 0;
        } else {
            row->flag = 1;
        }
        row++;
    }
    row->name = "";
    row[1].name = 0;
    window->columns = 1;
    window->rows = 8;
    window->draw_flags = 3;
    window->title = listWindowTag;
    window->width = 105;
    window->height = 198;
    window->items = CharList->paraItems;
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[25]);
}

/*
 * The equip-list screen's frame step: it opens the list window, fills it for
 * the category MenuWork.character.state asks for, slides it in and out, and runs the
 * window while the list is up.
 */
void CharListMain(void)
{
    CharListWork *list = CharList;
    int i;

    switch (list->state) {
    case 0:
        list->weapon.x = 528;
        list->weapon.y = 64;
        list->color = 0x00FFFFF8;
        list->weapon.style = 0x00FFFFF8;
        list->weapon.draw_flags = 7;
        list->weapon.title = "";
        list->weapon.rows = 11;
        list->weapon.height = 270;
        list->weapon.state = 0;
        list->weapon.items = 0;
        WindowSPSet(&list->weapon);
        for (i = 0; i < 3; i++) {
            eSpriteSet(&list->icons[i], 1546);
            list->icons[i].work = 0x00FFFFFF;
        }
        for (i = 0; i < 8; i++) {
            eNumberSet(&list->rows[i], 0);
            list->rows[i].work = 0x00FFFFFA;
        }
        list->cursorIndex = 0;
        list->state = 2;
        list->active = 0;
        list->scrollIndex = 0;
        memset(list->paraItems, 0, sizeof(list->paraItems));
        /* fallthrough */
    case 2:
        list->active = 0;
        switch (MenuWork.character.state) {
        case 85:
            list->state = 20;
            break;
        case 83:
            list->state = 10;
            break;
        case 129:
            list->state = 30;
            break;
        }
        break;

    case 10:
    case 20:
    case 30:
        list->weapon.x = -244;
        if (list->state == 10) {
            if (MenuWork.character.equipKind == 0) {
                list->weapon.y = 102;
                CharListMake_Wpn();
            } else {
                list->weapon.y = 153;
                CharListMake_Acc();
            }
            list->state = 11;
        } else if (list->state == 20) {
            list->weapon.y = 102;
            CharListMake_Gun();
            list->state = 21;
        } else {
            list->weapon.y = 80;
            CharListMake_Para();
            list->state = 11;
        }
        list->active = 1;
        list->weapon.state = 0x11;
        /* fallthrough */
    case 11:
    case 13:
    case 21:
    {
        short target = 16;

        if (list->state == 13) {
            target = -244;
        }
        MoveSlide(&list->weapon.x, &target, 3.0f);
        if (list->weapon.x == target) {
            switch (list->state) {
            case 13:
                list->state = 2;
                break;
            case 11:
                list->state = 12;
                break;
            case 21:
                list->state = 22;
                break;
            }
        }
        break;
    }

    case 12:
        switch (MenuWork.character.state) {
        case 65:
        case 33:
            list->state = 13;
            break;
        case 85:
            list->state = 20;
            break;
        }
        break;

    case 22:
        if (MenuWork.character.state == CHAR_STATE_EQUIP_RUN) {
            list->state = 13;
        }
        break;
    }

    if (list->active) {
        WindowSPMain(&list->weapon);
    }
}

/*
 * The switch-character arrow and its "press to confirm" prompt: state 0
 * initialises both and starts sliding the arrow toward its resting x (or a
 * nearer one while MenuWork is on states 0x10/0x11); state 2 keeps
 * sliding it and tracks the prompt's position and colour off the arrow.
 */
void CharSwitchMain(void)
{
    CharSwitchWork *csw = CharSwitch;

    switch (csw->state) {
    case 0:
        csw->color = 0x00FFFFF0;
        eSpriteSet(&csw->arrow, 0x201);
        csw->arrow.x = 0x210;
        csw->arrow.y = 0x0C;
        csw->arrow.work = csw->color;
        eMessageSet(&csw->message, *msg_6);
        csw->message.draw_parameter = csw->color;
        csw->message.mode = 0x20;
        csw->state = 2;
        /* fallthrough */
    case 2:
    {
        short target = 0x210;

        switch (MenuWork.character.state) {
        case CHAR_STATE_STATUS:
        case CHAR_STATE_STATUS_RUN:
            target = 0x110;
            break;
        }
        MoveSlide(&csw->arrow.x, &target, 3.0f);
        eSpriteMain(&csw->arrow);
        csw->message.x = csw->arrow.x + 20;
        csw->message.y = csw->arrow.y - 2;
        csw->message.draw_parameter = csw->arrow.work;
        eMessageMain(&csw->message);
        break;
    }
    }
}

void CharPointMain(void)
{
    CharPointWork *work = CharPoint;

    switch (work->state) {
    case 0:
    {
        int i;

        work->tagBase = 0xF00000;
        WindowDXSet(&work->panel.window);
        work->panel.window.x = 0x210;
        work->panel.window.y = 0xA0;
        work->panel.window.style = work->tagBase;
        work->panel.window.tag_id = tPtsTitleLabel;
        work->panel.window.width = 0x80;
        work->panel.window.height = 0x50;
        work->panel.window.state = 1;
        WindowDXMain(&work->panel.window);
        work->panel.window.state = 3;

        for (i = 0; i < 2; i++) {
            eTagFontSet(&work->panel.tagSlots[i], msg_7_0036D938[i]);
            work->panel.tagSlots[i].work = work->tagBase + 2;
        }

        for (i = 0; i < 3; i++) {
            eNumberSet(&work->panel.numberSlots[i], 0);
            work->panel.numberSlots[i].work = work->tagBase + 2;
        }

        eSpriteSet(&work->panel.sprite, 1798);
        work->panel.sprite.id = work->tagBase + 2;

        for (i = 0; i < 2; i++) {
            work->panel.colors[i].b = -96;
            work->panel.colors[i].g = -96;
            work->panel.colors[i].r = -96;
            work->panel.colors[i].a = -128;
            work->panel.colors[i].id = work->tagBase + 2;
        }

        work->panel.frameStart = 0;
        work->state = 1;
        break;
    }
    case 1:
        break;
    default:
        return;
    }

    {
        /* The menu state drives the panel's target width; it is compared as
         * a signed int (slti), so it is read into one. */
        int menuState = MenuWork.character.state;
        short targetWidth = 0x210;
        CharPointData *record;
        int points;
        int cost;
        int i;

        if (menuState >= 130) {
            if (menuState >= 132) {
                if (menuState < 134) {
                    targetWidth = 0xC0;
                }
            } else {
                targetWidth = 0x110;
            }
        }

        MoveSlide(&work->panel.window.x, &targetWidth, 3.0f);
        WindowDXMain(&work->panel.window);

        for (i = 0; i < 2; i++) {
            work->panel.tagSlots[i].x = work->panel.window.x + 68;
            work->panel.tagSlots[i].y = work->panel.window.y + ((i % 2) * 3 + (i / 2) * 5) * 16 + 4;
            eTagFontMain(&work->panel.tagSlots[i]);
        }

        work->panel.sprite.x = work->panel.window.x + 57;
        work->panel.sprite.y = work->panel.window.y + 28;
        eSpriteMain(&work->panel.sprite);

        record = func_A19210(MenuWork.character.chrNo);
        points = record->points;
        work->panel.numberSlots[0].value = points;
        cost = MenuWork.character.paraCost;
        work->panel.numberSlots[1].value = cost;
        work->panel.numberSlots[2].value = points - cost;

        for (i = 0; i < 3; i++) {
            work->panel.numberSlots[i].x = work->panel.window.x + 72;
            work->panel.numberSlots[i].y = work->panel.window.y + ((i % 3) * 24 + (i / 3) * 80) + 4;
            work->panel.numberSlots[i].format = 0;
            work->panel.numberSlots[i].digits = 4;
            eNumberMain(&work->panel.numberSlots[i]);
        }

        work->panel.sprite.printX = work->panel.window.x + 11;
        work->panel.colors[0].extraX = work->panel.window.x + 115;
        work->panel.colors[0].extraY = work->panel.window.y + 48;
        work->panel.sprite.printY = work->panel.window.y + 48;
        endPrintExtFunc(work->tagBase, END_PRINT_CMD_LINE,&work->panel.sprite.printX);
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: ：Set lead character
 * EN: : Set lead character */
const char setLeadCharacterText[24] = "\241\247Set lead character";

/* Text: \x01Current */
const char currentText[24] = "\001Current";

void MenuCharCameraSet(void)
{
    MenuCameraSetPair position = { .storage = { .representation = { .vector = { { .x = 0.44999998807907104f, .y = 0.89999997615814209f, .z = 2.2999999523162842f, .w = 1.0f }, { .x = -1.5f, .y = 0.80000001192092896f, .z = 6.5f, .w = 1.0f } } } } };
    MenuCameraSetRotationPair rotation = { .storage = { .representation = { .vector = { { .x = -0.40000000596046448f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f } } } } };
    MenuCameraSetPair formationRotation;
    MenuCameraSetVector formationPosition;
    MenuCameraSetVector zeroRotation;
    MenuCameraSetPositionVectors *positionValues;
    MenuCameraSetRotationVectors *rotationValues;
    StudioCamera *camera = xglStudioGetCamera2(0);
    Vector4 *cameraRotation;
    Vector4 *cameraPosition;

    xglCameraInit(camera);
    positionValues = &position.storage.values;
    rotationValues = &rotation.storage.values;
    cameraRotation = &camera->rotation.vector;
    *cameraRotation = rotationValues->vector[MenuWork.character.partyMode];
    cameraPosition = &camera->position.vector;
    *cameraPosition = positionValues->vector[MenuWork.character.partyMode];
    camera->state = 1;
    formationRotation.storage.representation = (MenuCameraPair){ .vector = { { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.52359879016876221f, .y = 0.0f, .z = 0.0f, .w = 1.0f } } };
    TairetuRot = formationRotation.storage.representation.vector[MenuWork.character.partyMode];
    formationPosition = (MenuCameraSetVector){ .storage = { .x = -1.7000000476837158f, .y = -0.20000000298023224f, .z = 1.5f, .w = 0.0f } };
    memset(&zeroRotation, 0, sizeof(zeroRotation));
    zeroRotation.storage.x = NEG_QUARTER_PI;
    TairetuPos = formationPosition.storage;
    TairetuRot = zeroRotation.storage;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const MenuCameraPair singleCameraPositionTable = { .vector = { { .x = 0.44999998807907104f, .y = 0.89999997615814209f, .z = 2.2999999523162842f, .w = 1.0f }, { .x = -0.80000001192092896f, .y = 0.69999998807907104f, .z = 3.5999999046325684f, .w = 1.0f } } };

const MenuCameraPair singleCameraRotationTable = { .vector = { { .x = -0.40000000596046448f, .y = 0.0f, .z = 0.0f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f } } };

const MenuCameraPair formationCameraRotationTable = { .vector = { { .x = -0.78539818525314331f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = -1.5707963705062866f, .y = 0.0f, .z = 1.5707963705062866f, .w = 0.0f } } };

const MenuCameraPair formationCameraPositionTable = { .vector = { { .x = -1.7000000476837158f, .y = -0.20000000298023224f, .z = 1.5f, .w = 0.0f }, { .x = -1.7999999523162842f, .y = -0.30000001192092896f, .z = 1.0f, .w = 0.0f } } };

void MenuCharCameraMove(void)
{
    MenuCameraPair singlePositions;
    MenuCameraPair singleRotations;
    MenuCameraPair formationRotations;
    MenuCameraPair formationPositions;
    float translation[4];
    float rotationMatrix[4][4];
    float positionMatrix[4][4];
    StudioCamera *camera = xglStudioGetCamera2(0);

    if (MenuWork.character.partyMode == 0) {
        int view;
        int state = MenuWork.character.state;
        Vector4 *position = &camera->position.vector;
        Vector4 *rotation = &camera->rotation.vector;
        float x;
        float y;
        float z;

        singlePositions = singleCameraPositionTable;
        singleRotations = singleCameraRotationTable;
        view = MenuCharCameraViewGet(state);
        x = position->x;
        y = position->y;
        z = position->z;
        position->x = x + (singlePositions.component[view][0] - x) * CHAR_CAMERA_POSITION_RATE;
        position->y = y + (singlePositions.component[view][1] - y) * CHAR_CAMERA_POSITION_RATE;
        position->z = z + (singlePositions.component[view][2] - z) * CHAR_CAMERA_POSITION_RATE;
        rotation->x += (singleRotations.component[view][0] - rotation->x) * CHAR_CAMERA_ROTATION_RATE;
    } else {
        Vector4 *cameraRotation = &camera->rotation.vector;
        Vector4 *cameraPosition = &camera->position.vector;
        Vector4 *formationRotation = &TairetuRot;
        Vector4 *formationPosition = &TairetuPos;
        int view = (MenuWork.character.state >> 4) == MENU_STATE_GROUP_FORMATION;
        float x;
        float y;
        float z;
        float targetX;
        float targetY;
        float targetZ;

        formationRotations = formationCameraRotationTable;
        formationPositions = formationCameraPositionTable;
        targetX = formationRotations.component[view][0];
        targetY = formationRotations.component[view][1];
        targetZ = formationRotations.component[view][2];
        x = formationRotation->x;
        y = formationRotation->y;
        z = formationRotation->z;
        /* Convert target coordinates into per-frame displacements. */
        targetX -= x;
        targetY -= y;
        targetZ -= z;
        targetX *= CHAR_CAMERA_TARGET_X_SCALE;
        targetY *= CHAR_CAMERA_TARGET_X_SCALE;
        targetZ *= CHAR_CAMERA_TARGET_X_SCALE;
        x += targetX;
        y += targetY;
        z += targetZ;
        formationRotation->x = x;
        formationRotation->y = y;
        formationRotation->z = z;
        memset(translation, 0, sizeof(translation));
        translation[2] = 6.5f;
        xglMatrixStackUnit();
        xglMatrixStackRotY(formationRotation->y);
        xglMatrixStackRotX(formationRotation->x);
        xglMatrixStackRotZ(formationRotation->z);
        xglMatrixStackTrans(translation);
        xglMatrixStackSave(rotationMatrix);
        cameraRotation->x = formationRotation->x;
        cameraRotation->y = formationRotation->y;
        cameraRotation->z = formationRotation->z;
        cameraPosition->x = rotationMatrix[3][0];
        cameraPosition->y = rotationMatrix[3][1];
        cameraPosition->z = rotationMatrix[3][2];
        {
            float targetPositionX = formationPositions.component[view][0];
            float targetPositionY = formationPositions.component[view][1];
            float targetPositionZ = formationPositions.component[view][2];
            float positionX = formationPosition->x;
            float positionY = formationPosition->y;
            float positionZ = formationPosition->z;

            targetPositionX -= positionX;
            targetPositionY -= positionY;
            targetPositionZ -= positionZ;
            targetPositionX *= CHAR_CAMERA_TARGET_X_SCALE;
            targetPositionY *= CHAR_CAMERA_TARGET_X_SCALE;
            targetPositionZ *= CHAR_CAMERA_TARGET_X_SCALE;
            positionX += targetPositionX;
            positionY += targetPositionY;
            positionZ += targetPositionZ;
            formationPosition->x = positionX;
            formationPosition->y = positionY;
            formationPosition->z = positionZ;
        }
        xglMatrixStackUnit();
        xglMatrixStackRotY(formationRotation->y);
        xglMatrixStackRotX(formationRotation->x);
        xglMatrixStackRotZ(formationRotation->z);
        xglMatrixStackTrans((float *)formationPosition);
        xglMatrixStackSave(positionMatrix);
        cameraPosition->x += positionMatrix[3][0];
        cameraPosition->y += positionMatrix[3][1];
        cameraPosition->z += positionMatrix[3][2];
    }
}

void MenuCharModelMain(MenuModelUnit *unit, void *argument)
{
    AlignedVector4 position;
    Vector4 rotation;
    AlignedVector4 scale;
    Actor *actor = unit->actor;
    ActorMenuControl *transform;
    unsigned char slot;

    if (actor == 0 && (MenuWork.character.swapOutId != 0 || MenuWork.character.state == CHAR_STATE_STATUS ||
        MenuWork.character.state == CHAR_STATE_STATUS_RUN || MenuWork.character.state == CHAR_STATE_EXIT || unit != MenuModelOut[MenuWork.character.modelSlot])) {
        MenuModelUnitBreak(unit);
        MenuWork.character.swapOutId = 0;
    }
    if (unit->active == 0) {
        return;
    }
    slot = setSlotTbl[MenuWork.character.chrNo - 1];
    transform = &actor->menu;
    if (MenuWork.character.swapOutId != 0 || MenuWork.character.state == CHAR_STATE_STATUS || MenuWork.character.state == CHAR_STATE_STATUS_RUN || MenuWork.character.state == CHAR_STATE_EXIT) {
        unit->extFuncFlag = 30;
        MenuWork.character.swapOutId = 0;
    }
    switch (unit->extFuncFlag) {
    case 0:
        unit->extFuncFlag = 10;
        transform->offset = transform->position;
        transform->offset.y = -transform->offset.y;
        MenuModelUnitOpen(unit, 2);
        if (MenuEquipStealMaskCheck() == 0) {
            if (unit->modelId != 3 || MenuShionMwsCheck() != 0) {
                MenuModelWeaponCreate(unit, 0, slot);
            }
        }
        break;
    case 10:
        if (MenuEquipStealMaskCheck() == 0 && (unit->modelId != 3 || MenuShionMwsCheck() != 0)) {
            if (MenuWork.character.weaponChanged != 0) {
                short weapon = unit->org->weapon[slot];
                unit->org->weapon[slot] = MenuWork.character.weaponId;
                MenuModelWeaponCreate(unit, 0, slot);
                unit->org->weapon[slot] = weapon;
                MenuWork.character.weaponChanged = 0;
            }
        }
        break;
    case 30:
        MenuModelUnitOpen(unit, 1);
        unit->extFuncFlag = 31;
        /* fallthrough */
    case 31:
        if (transform->transparency == 0.0f) {
            unit->taskPhase = -1;
        }
        break;
    }
    if (MenuWork.character.state == CHAR_STATE_COMMAND_RUN || (MenuWork.character.state >> 4) == CHAR_STATE_GROUP_PARA) {
        position = transform->position;
        position.y = -position.y;
        {
            Vector4 *current = &transform->offset;
            float x = current->x;
            float y = current->y;
            float z = current->z;
            current->x = x + (position.x - x) * CHAR_MODEL_DEFAULT_MOVE_RATE;
            current->y = y + (position.y - y) * CHAR_MODEL_DEFAULT_MOVE_RATE;
            current->z = z + (position.z - z) * CHAR_MODEL_DEFAULT_MOVE_RATE;
        }
        if (MenuWork.character.state == CHAR_STATE_COMMAND_RUN) {
            MenuModelControl2(unit, 0);
        }
    } else {
        memset(&position, 0, 16);
        position.w = 1.0f;
        memset(&rotation, 0, 16);
        rotation.w = 1.0f;
        scale = (AlignedVector4){ .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f };
        {
            Vector4 *current = &transform->offset;
            float x = current->x;
            float y = current->y;
            float z = current->z;
            current->x = x + (position.x - x) * CHAR_MODEL_RESET_MOVE_RATE;
            current->y = y + (position.y - y) * CHAR_MODEL_RESET_MOVE_RATE;
            current->z = z + (position.z - z) * CHAR_MODEL_RESET_MOVE_RATE;
        }
        {
            Vector4 *current = &transform->rotation;
            float x = current->x;
            float y = current->y;
            float z = current->z;
            current->x = x + (rotation.x - x) * CHAR_MODEL_RESET_MOVE_RATE;
            current->y = y + (rotation.y - y) * CHAR_MODEL_RESET_MOVE_RATE;
            current->z = z + (rotation.z - z) * CHAR_MODEL_RESET_MOVE_RATE;
        }
        {
            Vector4 *current = &transform->scale;
            float x = current->x;
            float y = current->y;
            float z = current->z;
            current->x = x + (scale.x - x) * CHAR_MODEL_RESET_MOVE_RATE;
            current->y = y + (scale.y - y) * CHAR_MODEL_RESET_MOVE_RATE;
            current->z = z + (scale.z - z) * CHAR_MODEL_RESET_MOVE_RATE;
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const MenuCharacterPositions battleModelPositionTable = { .vector = { { .x = 1.0f, .y = 0.0f, .z = 0.60000002384185791f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.60000002384185791f, .w = 1.0f }, { .x = -1.0f, .y = 0.0f, .z = 0.60000002384185791f, .w = 1.0f }, { .x = 1.0f, .y = 0.0f, .z = -0.60000002384185791f, .w = 1.0f }, { .x = 0.0f, .y = 0.0f, .z = -0.60000002384185791f, .w = 1.0f }, { .x = -1.0f, .y = 0.0f, .z = -0.60000002384185791f, .w = 1.0f } } };

void MenuBattleModelMain(MenuModelUnit *unit, void *argument)
{
    Actor *actor = unit->actor;

    if (actor == 0 && MenuWork.character.state == CHAR_STATE_EXIT) {
        MenuModelUnitBreak(unit);
    }
    if (unit->active != 0) {
        MenuCharacterPositions positions;
        ActorMenuControl *transform;
        PartyDataPrefix *party;
        signed char formationSlot;
        int i;
        Vector4 *target;

        positions = battleModelPositionTable;
        transform = &actor->menu;
        party = PartyDataGet();
        formationSlot = 1;
        for (i = 0; i < 3; i++) {
            if (party->attack_slots[i].party_id == unit->modelId) {
                formationSlot = party->attack_slots[i].attack_position;
                break;
            }
        }
        switch (unit->extFuncFlag) {
        case 0:
            transform->offset = positions.vector[formationSlot - 1];
            unit->extFuncFlag = 1;
            break;
        case 1:
            MenuModelUnitOpen(unit, 2);
            unit->extFuncFlag = 20;
            /* fallthrough */
        case 20:
            if ((MenuWork.character.state >> 4) == MENU_STATE_GROUP_FORMATION && unit->modelId != MenuWork.character.chrNo) {
                unit->extFuncFlag = 22;
                MenuModelUnitOpen(unit, 4);
            }
            if ((MenuWork.character.state >> 4) == CHAR_STATE_GROUP_RESERVE) {
                unit->extFuncFlag = 24;
                MenuModelUnitOpen(unit, 3);
            }
            break;
        case 22:
            if ((MenuWork.character.state >> 4) != MENU_STATE_GROUP_FORMATION) {
                MenuModelUnitOpen(unit, 2);
                unit->extFuncFlag = 20;
            }
            break;
        case 24:
            if ((MenuWork.character.state >> 4) != CHAR_STATE_GROUP_RESERVE) {
                MenuModelUnitOpen(unit, 2);
                unit->extFuncFlag = 20;
            }
            break;
        case 30:
            MenuModelUnitOpen(unit, 3);
            unit->extFuncFlag = 31;
            /* fallthrough */
        case 31:
            if (transform->transparency == 0.0f) {
                unit->taskPhase = -1;
            }
            break;
        }
        if (MenuWork.character.state == CHAR_STATE_EXIT) {
            unit->extFuncFlag = 30;
        }
        if (unit->modelId == MenuWork.character.swapOutId) {
            MenuWork.character.swapOutId = 0;
            unit->extFuncFlag = 30;
        }
        target = &positions.vector[formationSlot - 1];
        if (MenuWork.character.state != CHAR_STATE_FORMATION_EXCHANGE_RUN) {
            if (MenuWork.character.state == CHAR_STATE_FORMATION_PLACE&& unit->modelId == MenuWork.character.chrNo) {
                target = &positions.vector[MenuWork.character.formationSlot - 1];
            }
        } else {
            int occupant = PartyAttackPosCheck(MenuWork.character.formationSlot);
            if (unit->modelId == MenuWork.character.chrNo) {
                target = &positions.vector[MenuWork.character.formationSlot - 1];
            } else if (unit->modelId == occupant) {
                target = &positions.vector[PartyAttackPosGet(MenuWork.character.chrNo) - 1];
            }
        }
        {
            Vector4 *current = &transform->offset;
            float x = current->x;
            float y = current->y;
            float z = current->z;
            current->x = x + (target->x - x) * BATTLE_MODEL_MOVE_RATE;
            current->y = y + (target->y - y) * BATTLE_MODEL_MOVE_RATE;
            current->z = z + (target->z - z) * BATTLE_MODEL_MOVE_RATE;
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const AlignedVector4 lineFieldInitialScaleVector = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f };

const SpotLightTable spotLightPositionTable = {{
    {1.0f, 0.0f, 0.6f, 1.0f},
    {0.0f, 0.0f, 0.6f, 1.0f},
    {-1.0f, 0.0f, 0.6f, 1.0f},
    {1.0f, 0.0f, -0.6f, 1.0f},
    {0.0f, 0.0f, -0.6f, 1.0f},
    {-1.0f, 0.0f, -0.6f, 1.0f},
}};

const char paraUpBinPath[32] = "data\\endou\\para\\paraup.bin";

/* Text: {reset}charactor */
const char charactorText[16] = "\013charactor";

static int tskCharLineField(XglTaskPrefix *task)
{
    CharLineFieldTask *line = (CharLineFieldTask *)task;
    AlignedVector4 initialScale;
    Vector4 vertices[6][5];
    LineProjection screen[6][5];
    DirectLineVertex points[8];
    Matrix4 matrix;
    unsigned char rowOpacity[5];
    unsigned char columnOpacity[6];
    int lineIndex;
    int vertexIndex;

    if (MenuWork.character.state == MENU_WORK_END || MenuWork.character.partyMode == 0) {
        return xglTaskWaitRemove(task);
    }
    if (MenuWork.character.state == CHAR_STATE_EXIT_RUN) {
        line->state = 99;
    }
    switch (line->state) {
    case 0:
        initialScale = lineFieldInitialScaleVector;
        line->wait = 8;
        line->scale = initialScale;
        line->state = 2;
        line->visible = 0;
        line->alpha = 0.0f;
        /* fallthrough */
    case 2:
        if (line->wait == 0) {
            line->visible = 1;
            line->state = 10;
        }
        break;
    case 10:
        {
            float step = LINE_FIELD_GROW_STEP;
            float x = line->scale.x;
            float deltaX = 1.0f - x;
            float z = line->scale.z;
            float deltaZ = 1.0f - z;
            float alpha = line->alpha;
            deltaX *= step;
            alpha += LINE_FIELD_FADE_IN_STEP;
            deltaZ *= step;
            x += deltaX;
            z += deltaZ;
            line->alpha = alpha;
            line->scale.x = x;
            line->scale.z = z;
            if (alpha >= 1.0f) {
                line->alpha = 1.0f;
                line->state = 20;
            }
        }
        break;
    case 20:
        if ((MenuWork.character.state >> 4) == CHAR_STATE_GROUP_RESERVE) {
            line->state = 30;
        }
        break;
    case 30:
    case 99:
        if (line->state == 30 && (MenuWork.character.state >> 4) != CHAR_STATE_GROUP_RESERVE) {
            line->state = 10;
        }
        {
            float step = LINE_FIELD_SHRINK_STEP;
            float x = line->scale.x;
            float deltaX = 0.0f - x;
            float z = line->scale.z;
            float deltaZ = 0.0f - z;
            float alpha = line->alpha;
            deltaX *= step;
            alpha -= LINE_FIELD_FADE_OUT_STEP;
            deltaZ *= step;
            x += deltaX;
            z += deltaZ;
            line->alpha = alpha;
            line->scale.x = x;
            line->scale.z = z;
            if (alpha <= 0.0f) {
                line->alpha = 0.0f;
                if (line->state == 99) {
                    return xglTaskWaitRemove(task);
                }
            }
            if (line->alpha <= 0.0f) {
                line->alpha = 0.0f;
            }
        }
        break;
    }
    if (line->wait != 0) {
        line->wait--;
    }
    if (line->visible != 0) {
        {
            for (vertexIndex = 0; vertexIndex < 6; vertexIndex++) {
                float x = 2.5f - vertexIndex;
                lineIndex = 0;
                do {
                    Vector4 *vertex = &vertices[vertexIndex][lineIndex];
                    vertex->x = x;
                    vertex->y = 0.0f;
                    vertex->z = lineIndex * 1.2f;
                    vertex->z = 2.4f - vertex->z;
                    vertex->w = 1;
                    lineIndex++;
                } while (lineIndex < 5);
            }
        }
        xglMatrixStackUnit();
        xglMatrixStackScale(&line->scale.x);
        xglMatrixStackSave(matrix);
        xglRotTransPersN(screen[0][0].component, &matrix, vertices[0], 30, 0);
        __builtin_memcpy(rowOpacity, rowOpacityTable, sizeof(rowOpacity));
        __builtin_memcpy(columnOpacity, columnOpacityTable, sizeof(columnOpacity));
        lineIndex = 0;
        do {
            float lineAlpha = line->alpha;
            vertexIndex = 0;
            do {
                points[vertexIndex].coordinates = screen[lineIndex + 1][vertexIndex];
                points[vertexIndex].color.red = 96;
                points[vertexIndex].color.blue = 160;
                points[vertexIndex].color.green = 160;
                points[vertexIndex].color.alpha = (unsigned char)(rowOpacity[vertexIndex] * lineAlpha);
                vertexIndex++;
            } while (vertexIndex < 5);
            points[vertexIndex].coordinates.component[2] = 0;
            endPrintDirectLine(points);
            lineIndex++;
        } while (lineIndex < 4);
        lineIndex = 0;
        do {
            float lineAlpha = line->alpha;
            vertexIndex = 0;
            do {
                points[vertexIndex].coordinates = screen[vertexIndex][lineIndex + 1];
                points[vertexIndex].color.red = 96;
                points[vertexIndex].color.blue = 160;
                points[vertexIndex].color.green = 160;
                points[vertexIndex].color.alpha = (unsigned char)(columnOpacity[vertexIndex] * lineAlpha);
                vertexIndex++;
            } while (vertexIndex < 6);
            points[vertexIndex].coordinates.component[2] = 0;
            endPrintDirectLine(points);
            lineIndex++;
        } while (lineIndex < 3);
    }
}

static void tskCharSpotLight(CharSpotLightTask *light)
{
    SpotLightTable positions = spotLightPositionTable;
    DirectCircle circle;
    int screen[4];

    if (MenuWork.character.state == MENU_WORK_END || MenuWork.character.partyMode == 0) {
        xglTaskWaitRemove(&light->task);
        return;
    }
    switch (light->mode) {
    case SPOTLIGHT_START:
        light->mode = SPOTLIGHT_HIDDEN;
        light->visible = 0;
        light->counter = 0;
        /* fallthrough */
    case SPOTLIGHT_HIDDEN:
        if ((MenuWork.character.state >> 4) == MENU_STATE_GROUP_FORMATION) {
            light->mode = SPOTLIGHT_SHOWN;
            light->pulse = 1.0f;
            light->pulseStep = SPOT_LIGHT_PULSE_STEP;
            light->size = 0.0f;
            light->position = positions.position[MenuWork.character.formationSlot - 1];
            light->visible = 1;
        }
        break;
    case SPOTLIGHT_SHOWN:
        if (light->size < 1.0f) {
            light->size += 0.125f;
        } else {
            light->size = 1.0f;
        }
        if ((MenuWork.character.state >> 4) != MENU_STATE_GROUP_FORMATION) {
            light->mode = SPOTLIGHT_FADE_OUT;
        }
        break;
    case SPOTLIGHT_FADE_OUT:
        if (light->size != 0.0f) {
            light->size -= 0.125f;
        } else {
            light->mode = SPOTLIGHT_HIDDEN;
            light->visible = 0;
        }
        break;
    }
    light->pulse += light->pulseStep;
    if (light->pulseStep > 0.0f) {
        if (light->pulse >= 1.0f) {
            light->pulseStep = -light->pulseStep;
        }
    } else if (light->pulse <= 0.0f) {
        light->pulseStep = -light->pulseStep;
    }
    if (light->visible) {
        light->position.x += (positions.position[MenuWork.character.formationSlot - 1].x - light->position.x) * SPOT_LIGHT_EASE_RATE;
        light->position.y += (positions.position[MenuWork.character.formationSlot - 1].y - light->position.y) * SPOT_LIGHT_EASE_RATE;
        light->position.z += (positions.position[MenuWork.character.formationSlot - 1].z - light->position.z) * SPOT_LIGHT_EASE_RATE;
        xglRotTransPersN(screen, 0, &light->position, 1, 0);
        screen[0] = screen[0] / 16 - CIRCLE_ORIGIN_X;
        screen[1] = screen[1] / 16 - CIRCLE_ORIGIN_Y;
        circle.x = screen[0];
        circle.y = screen[1];
        circle.z = screen[2];
        circle.innerColor[0] = 0xA0;
        circle.innerColor[1] = 0xA0;
        circle.innerColor[2] = 0x60;
        circle.innerColor[3] = 0x80;
        circle.radius = light->size * 48.0f + light->pulse * 16.0f;
        circle.segments = 32;
        circle.outerColor[0] = 0xA0;
        circle.outerColor[1] = 0xA0;
        circle.outerColor[2] = 0x60;
        circle.outerColor[3] = 0;
        endPrintDirectCircle(&circle);
    }
}

void MenuCharParaSet(int chrNo, int previewOnly)
{
    CharParaData *org;
    CharParaData *total;
    int attack[3];
    int defense[2];
    short keptHp;
    short keptEp;
    int i;

    if (chrNo > 0) {
        org = func_A191C0(chrNo);
        keptHp = 0;
        keptEp = 0;
        if (chrNo >= 1 && chrNo <= 7) {
            if (org->maxHp < org->hp) {
                keptHp = org->hp;
            }
            if (org->maxEp < org->ep) {
                keptEp = org->ep;
            }
        }
        total = func_00A11108(chrNo, attack, defense);
        if (total != 0) {
            total->attack += attack[setSlotTbl[chrNo - 1]];
            total->phyDefense += defense[0];
            total->magDefense += defense[1];
        }
        if (previewOnly == 0) {
            paraUnit[0] = paraUnit[1] = *total;
            for (i = 0; i < 3; i++) {
                if (org->accessory[i] == ACCESSORY_KEEP_HP && keptHp != 0) {
                    org->hp = keptHp;
                }
                if (org->accessory[i] == ACCESSORY_KEEP_EP && keptEp != 0) {
                    org->ep = keptEp;
                }
            }
        } else {
            paraUnit[1] = *total;
            if (keptHp != 0) {
                org->hp = keptHp;
            }
            if (keptEp != 0) {
                org->ep = keptEp;
            }
        }
    }
}

/*
 * The character menu (status, equipment, parameter points and formation).
 * The first call lays out the sub-screen work blocks and builds the party
 * list; every later call advances the MenuWork.character.state machine and runs the
 * sub-screens. partyMode is 0 from the main menu and 1 from battle.
 */
void MenuCharactor(int partyMode)
{
    if (MenuWork.character.state == 0) {
        void *heap = MainMenuWorkEnd;
        PartyAttackPosition *slot;
        int i;
        int id;

        if (partyMode == 0) {
            ParaDataBuf = (unsigned short *)(((int)heap + 0x7FF) & ~0x7FF);
            heap = (unsigned char *)ParaDataBuf + 0x800;
            xglCdReadFile(paraUpBinPath, ParaDataBuf, 0, 1);
        }
        CharPas = heap;
        heap += CHAR_PAS_SIZE;
        CharPas->state = 0;
        CharCategory = heap;
        heap += CHAR_CATEGORY_SIZE;
        CharCategory->state = 0;
        CharMenu = heap;
        heap += CHAR_MENU_SIZE;
        CharMenu->state = 0;
        CharInfo = heap;
        heap += CHAR_INFO_SIZE;
        CharInfo->state = 0;
        CharStatus = heap;
        heap += CHAR_STATUS_SIZE;
        CharStatus->work.state = 0;
        CharParameter = heap;
        heap += CHAR_PARAMETER_SIZE;
        CharParameter->state = 0;
        CharEx = heap;
        heap += CHAR_EX_SIZE;
        CharEx->state = 0;
        CharWeapon = heap;
        heap += CHAR_WEAPON_SIZE;
        CharWeapon->state = 0;
        CharList = heap;
        heap += CHAR_LIST_SIZE;
        CharList->state = 0;
        CharSwitch = heap;
        heap += CHAR_SWITCH_SIZE;
        CharWinSP[0] = &CharList->weapon;
        CharWinSP[1] = &CharList->paraWindow;
        CharSwitch->state = 0;
        CharL1R1 = heap;
        heap += CHAR_L1R1_SIZE;
        CharL1R1->state = 0;
        CharPoint = heap;
        heap += CHAR_POINT_SIZE;
        CharPoint->state = 0;
        {
            CharSpotLightTask *task;

            task = (CharSpotLightTask *)xglTaskEntryNext(
                MenuTask_XMX, tskCharLineField,
                MenuTask_XMX != 0 ? MenuTask_XMX->active_tail : 0);
            if (task != 0) {
                task->mode = 0;
            }
            task = (CharSpotLightTask *)xglTaskEntryNext(
                MenuTask_XMX, (int (*)(XglTaskPrefix *))tskCharSpotLight,
                MenuTask_XMX != 0 ? MenuTask_XMX->active_tail : 0);
            if (task != 0) {
                task->mode = 0;
            }
        }
        memset(UpPara, 0, sizeof(UpPara));
        MenuWorkEndCheck(heap);
        MenuWork.character.partyMode = partyMode;
        slot = (PartyDataGet())->attack_slots;
        for (i = 0; i < 3; i++, slot++) {
            id = slot->party_id;
            if (id != 0) {
                MenuCharParty[MenuWork.character.partyCount] = MenuMaryIdChange(id);
                MenuWork.character.partyCount++;
            }
        }
        for (id = 1; id < 8; id++) {
            if (PartyFriendLockCheck(id, 1) != 0 && PartyAttackerCheck(id) == 0) {
                if (MenuWork.character.partyMode == 0) {
                    MenuCharParty[MenuWork.character.partyCount] = MenuMaryIdChange(id);
                    MenuWork.character.partyCount++;
                }
                MenuCharPartyReser[MenuWork.character.reserveCount] = MenuMaryIdChange(id);
                MenuWork.character.reserveCount++;
            }
        }
        MenuWork.character.chrNo = MenuCharParty[0];
        if (partyMode == 0) {
            MenuWork.character.state = CHAR_STATE_STATUS;
            MenuModelMemorySet(10);
        } else {
            int n;

            MenuWork.character.state = CHAR_STATE_FORMATION;
            for (n = 0; n < 3; n++) {
                MenuModelExtFuncSet(MenuModelOut[n], MenuBattleModelMain, 0);
            }
        }
        MenuModelMenuMotionLoad();
        MenuCharCameraSet();
        MenuCharParaSet(MenuWork.character.chrNo, 0);
        MenuKeepSelectReset();
        MenuWork.character.wait = 12;
    } else {
        if (MenuWork.character.wait != 0) {
            MenuWork.character.wait--;
        }
        MenuCharCameraMove();
        switch (MenuWork.character.state) {
        /* The status screen: pick the character. */
        case CHAR_STATE_STATUS:
            MenuWork.character.state = CHAR_STATE_STATUS_RUN;
        case CHAR_STATE_STATUS_RUN:
            if (MenuWork.character.wait != 0) {
                break;
            }
            if (PadData.repeat & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
                if (MenuWork.character.partyCount == 1) {
                    break;
                }
                switch (PadData.repeat) {
                case PAD_UP:
                    MenuWork.character.partyCursor -= 2;
                    break;
                case PAD_DOWN:
                    MenuWork.character.partyCursor += 2;
                    break;
                case PAD_LEFT:
                    MenuWork.character.partyCursor -= 1;
                    break;
                case PAD_RIGHT:
                    MenuWork.character.partyCursor += 1;
                    break;
                }
                if (MenuWork.character.partyCursor < 0) {
                    MenuWork.character.partyCursor += MenuWork.character.partyCount;
                }
                if (MenuWork.character.partyCursor >= MenuWork.character.partyCount) {
                    MenuWork.character.partyCursor -= MenuWork.character.partyCount;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
                MenuWork.character.chrNo = MenuCharParty[MenuWork.character.partyCursor];
            }
            if (PadData.pressed & PAD_SQUARE) {
                MenuWork.character.leaderLocked = MenuCharLeaderMask(MenuWork.character.chrNo);
                if (MenuWork.character.leaderLocked == 0) {
                    PartyLeaderSet(MenuWork.character.chrNo);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (PadData.pressed & PAD_CIRCLE) {
                if (MenuWork.character.chrNo == CHR_GUEST) {
                    MenuWork.character.guestDenied = 1;
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                } else {
                    MenuWork.character.state = CHAR_STATE_COMMAND;
                    MenuWork.character.wait = 24;
                    MenuWork.character.guestDenied = 0;
                    MenuWork.character.equipKind = 0;
                    MenuWork.character.equipSlot = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.character.state = CHAR_STATE_EXIT;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            break;

        /* The command menu of the chosen character. */
        case CHAR_STATE_COMMAND:
            MenuWork.character.state = CHAR_STATE_COMMAND_RUN;
            MenuWork.character.menuCount = 3;
            MenuCharParaSet(MenuWork.character.chrNo, 0);
        case CHAR_STATE_COMMAND_RUN:
            if (MenuWork.character.wait != 0 || (PadData.held & (PAD_L2 | PAD_R2))) {
                break;
            }
            MenuWork.character.menuCursor = MenuSelectMove(MenuWork.character.menuCursor, MenuWork.character.menuCount, 0);
            if (PadData.pressed & (PAD_L1 | PAD_R1)) {
                /* L1/R1 step to the next party member, skipping the guest. The
                 * step is re-entered through its own head: written as a
                 * do-while, the whole tail of the function is allocated
                 * differently from 0x002980c4 on and grows by one
                 * instruction. */
            retry:
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (PadData.pressed == PAD_L1) {
                    MenuWork.character.partyCursor--;
                }
                if (PadData.pressed == PAD_R1) {
                    MenuWork.character.partyCursor++;
                }
                if (MenuWork.character.partyCursor < 0) {
                    MenuWork.character.partyCursor = MenuWork.character.partyCount - 1;
                }
                if (MenuWork.character.partyCursor > MenuWork.character.partyCount - 1) {
                    MenuWork.character.partyCursor = 0;
                }
                MenuWork.character.chrNo = MenuCharParty[MenuWork.character.partyCursor];
                MenuCharParaSet(MenuWork.character.chrNo, 0);
                if (MenuWork.character.chrNo == CHR_GUEST) {
                    goto retry;
                }
                MenuWork.character.wait = 4;
                MenuWork.character.swapOutId = 1;
                MenuWork.character.modelSlot = 0;
                MenuWork.character.equipKind = 0;
                MenuWork.character.equipSlot = 0;
                MenuKeepSelectReset();
            } else if (PadData.pressed & PAD_CROSS) {
                MenuWork.character.state = CHAR_STATE_STATUS;
                MenuWork.character.wait = 12;
                MenuKeepSelectReset();
                xglSoundEffectNormalID(SE_CANCEL, 0);
            } else if (PadData.pressed & PAD_CIRCLE) {
                if (MenuModelOut[MenuWork.character.modelSlot] == 0) {
                    /* Wait for the model; it is created below. */
                } else if (MenuWork.character.chrNo >= 1 && MenuWork.character.chrNo <= 7) {
                    if (MenuWork.character.menuCursor < 2) {
                        MenuWork.character.state = CHAR_STATE_EQUIP;
                    } else {
                        MenuWork.character.state = CHAR_STATE_PARA;
                        MenuWork.character.listSelect = -1;
                    }
                    MenuWork.character.wait = 8;
                    MenuWork.character.accessoryList = 0;
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.character.equipKind = 0;
                        MenuWork.character.equipSlot = 0;
                    }
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (MenuModelOut[MenuWork.character.modelSlot] == 0) {
                MenuModelCreate(&MenuModelOut[MenuWork.character.modelSlot], MenuWork.character.chrNo);
                MenuModelExtFuncSet(MenuModelOut[MenuWork.character.modelSlot], MenuCharModelMain, 0);
            }
            break;

        /* Equipment: pick the slot. */
        case CHAR_STATE_EQUIP:
            MenuWork.character.state = CHAR_STATE_EQUIP_RUN;
            MenuWork.character.listSelect = -1;
        case CHAR_STATE_EQUIP_RUN: {
            CharParaData *org;

            if (MenuWork.character.wait != 0) {
                break;
            }
            org = func_A191C0(MenuWork.character.chrNo);
            if (PadData.repeat == PAD_UP) {
                if (MenuWork.character.equipKind == 0) {
                    if (MenuWork.character.equipSlot == 0) {
                        if (PadData.repeatFirst == PadData.repeat) {
                            MenuWork.character.equipKind = 1;
                            MenuWork.character.equipSlot = 2;
                            xglSoundEffectNormalID(SE_CURSOR, 0);
                        }
                    } else {
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                        MenuWork.character.equipSlot = 0;
                    }
                } else {
                    if (MenuWork.character.equipSlot == 0) {
                        MenuWork.character.equipKind = 0;
                        if (MenuBulletCheck2(org->weapon[setSlotTbl[MenuWork.character.chrNo - 1]], MenuWork.character.chrNo) == 0) {
                            MenuWork.character.equipSlot = 0;
                        } else {
                            MenuWork.character.equipSlot = 1;
                        }
                    } else {
                        MenuWork.character.equipSlot--;
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            if (PadData.repeat == PAD_DOWN) {
                if (MenuWork.character.equipKind == 0) {
                    if (MenuBulletCheck2(org->weapon[setSlotTbl[MenuWork.character.chrNo - 1]], MenuWork.character.chrNo) != 0) {
                        if (MenuWork.character.equipSlot == 0) {
                            MenuWork.character.equipSlot = 1;
                        } else {
                            MenuWork.character.equipSlot = 0;
                            MenuWork.character.equipKind = 1;
                        }
                    } else {
                        MenuWork.character.equipKind = 1;
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    if (MenuWork.character.equipSlot == 2) {
                        if (PadData.repeatFirst == PadData.repeat) {
                            MenuWork.character.equipKind = 0;
                            MenuWork.character.equipSlot = 0;
                            xglSoundEffectNormalID(SE_CURSOR, 0);
                        }
                    } else {
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                        MenuWork.character.equipSlot++;
                    }
                }
            }
            if (PadData.repeat == PAD_CIRCLE) {
                if (MenuWork.character.menuCursor == 1) {
                    if (MenuWork.character.equipKind == 1) {
                        short accessory = org->accessory[MenuWork.character.equipSlot];

                        org->accessory[MenuWork.character.equipSlot] = 0;
                        MenuCharParaSet(MenuWork.character.chrNo, 1);
                        dataAccBoxInc(accessory);
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    if (MenuWork.character.equipKind != 0) {
                        MenuWork.character.state = CHAR_STATE_EQUIP_LIST;
                    } else if (MenuWork.character.equipSlot == 1) {
                        MenuWork.character.state = CHAR_STATE_BULLET_LIST;
                    } else if (MenuWork.character.chrNo != CHR_FIXED_WEAPON) {
                        MenuWork.character.state = CHAR_STATE_EQUIP_LIST;
                    }
                    MenuWork.character.wait = 12;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.repeat == PAD_CROSS) {
                MenuWork.character.state = CHAR_STATE_COMMAND;
                MenuWork.character.wait = 8;
                xglSoundEffectNormalID(SE_CANCEL, 0);
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.character.menuCursor = 0;
                }
            }
            break;
        }

        /* Equipment: pick the weapon or accessory from the list. */
        case CHAR_STATE_EQUIP_LIST:
            MenuWork.character.state = CHAR_STATE_EQUIP_LIST_RUN;
            MenuWork.character.listSelect = -1;
            MenuWork.character.flags |= 2;
            MenuWork.character.bulletFits = 0;
            MenuWork.character.weaponId = 0;
            MenuWork.character.accessoryId = 0;
        case CHAR_STATE_EQUIP_LIST_RUN: {
            int selected;
            int n;

            if (MenuWork.character.wait != 0) {
                break;
            }
            if (PadData.pressed == PAD_CIRCLE && MenuWork.character.listSelect >= 0) {
                CharParaData *org = func_A191C0(MenuWork.character.chrNo);

                if (MenuWork.character.equipKind == 0) {
                    if (MenuWork.character.equipSlot == 0) {
                        int weaponSlot = setSlotTbl[MenuWork.character.chrNo - 1];
                        short weapon = org->weapon[weaponSlot];
                        short bullet = org->bullet[weaponSlot];

                        org->weapon[weaponSlot] = MenuWork.character.weaponId;
                        org->bullet[weaponSlot] = func_A1A3D8(MenuWork.character.weaponId)->bullet;
                        MenuCharParaSet(MenuWork.character.chrNo, 1);
                        dataWpnBoxDec(MenuWork.character.weaponId);
                        if (weapon != 0) {
                            dataWpnBoxInc(weapon);
                            if (bullet != 0 && MenuBulletCheck(weapon, -1) != 0) {
                                if (func_A1A3D8(weapon)->bullet == bullet) {
                                    if (dataBltBoxChk(bullet) == 0) {
                                        dataBltBoxInc(bullet);
                                    }
                                } else {
                                    dataBltBoxInc(bullet);
                                }
                            }
                        }
                        if (MenuWork.character.chrNo == CHR_DUAL_WEAPON) {
                            org->weapon[1] = MenuWork.character.weaponId;
                        }
                        MenuWork.character.state = CHAR_STATE_EQUIP;
                        MenuWork.character.wait = 12;
                    } else {
                        MenuWork.character.state = CHAR_STATE_EQUIP;
                        MenuWork.character.wait = 8;
                    }
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    if (MenuAccessoryEquipCheck(MenuWork.character.chrNo, MenuWork.character.accessoryId, MenuWork.character.equipSlot) != 0) {
                        short old = org->accessory[MenuWork.character.equipSlot];

                        org->accessory[MenuWork.character.equipSlot] = MenuWork.character.accessoryId;
                        MenuCharParaSet(MenuWork.character.chrNo, 0);
                        dataAccBoxDec(MenuWork.character.accessoryId);
                        if (old != 0) {
                            dataAccBoxInc(old);
                        }
                        MenuWork.character.state = CHAR_STATE_EQUIP;
                        MenuWork.character.wait = 8;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
            }
            selected = WindowSPSelect(CharWinSP[0], PadData.repeat);
            if (MenuWork.character.listSelect != selected) {
                CharParaData *org = func_A191C0(MenuWork.character.chrNo);

                if (MenuWork.character.equipKind == 0) {
                    int weaponSlot = setSlotTbl[MenuWork.character.chrNo - 1];
                    short bullet;
                    short weapon;

                    MenuWork.character.weaponId = MenuSortGet(0, selected);
                    weapon = org->weapon[weaponSlot];
                    bullet = org->bullet[weaponSlot];
                    org->weapon[weaponSlot] = MenuWork.character.weaponId;
                    org->bullet[weaponSlot] = func_A1A3D8(MenuWork.character.weaponId)->bullet;
                    MenuCharParaSet(MenuWork.character.chrNo, 1);
                    org->weapon[weaponSlot] = weapon;
                    org->bullet[weaponSlot] = bullet;
                    if (MenuWork.character.equipSlot == 0) {
                        MenuWork.character.weaponChanged = 1;
                    }
                    WindowSPKeepSelect(&CharList->weapon, &MenuKeepSelect[KEEP_WEAPON * 5]);
                } else {
                    MenuWork.character.accessoryId = MenuSortGet(0, selected);
                    if (MenuAccessoryEquipCheck(MenuWork.character.chrNo, MenuWork.character.accessoryId, MenuWork.character.equipSlot) != 0) {
                        short old = org->accessory[MenuWork.character.equipSlot];

                        org->accessory[MenuWork.character.equipSlot] = MenuWork.character.accessoryId;
                        MenuCharParaSet(MenuWork.character.chrNo, 1);
                        org->accessory[MenuWork.character.equipSlot] = old;
                    } else {
                        MenuCharParaSet(MenuWork.character.chrNo, 0);
                    }
                    WindowSPKeepSelect(&CharList->weapon, &MenuKeepSelect[MenuWork.character.accessoryList * 5]);
                }
                MenuWork.character.listSelect = selected;
            }
            if (PadData.pressed == PAD_CROSS) {
                MenuCharParaSet(MenuWork.character.chrNo, 0);
                if (MenuWork.character.equipKind == 0 && MenuWork.character.equipSlot == 0) {
                    MenuWork.character.weaponId = func_A191C0(MenuWork.character.chrNo)->weapon[setSlotTbl[MenuWork.character.chrNo - 1]];
                    MenuWork.character.weaponChanged = 1;
                }
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.character.state = CHAR_STATE_EQUIP;
                MenuWork.character.wait = 8;
                for (n = 0; n < 8; n++) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[n * 5]);
                }
            }
            if (MenuWork.character.equipKind == 1) {
                if (PadData.repeat == PAD_RIGHT) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[MenuWork.character.accessoryList * 5]);
                    if (MenuWork.character.accessoryList == 2) {
                        MenuWork.character.accessoryList = 0;
                    } else {
                        MenuWork.character.accessoryList++;
                    }
                    MenuWork.character.listSelect = -1;
                    CharListMake_Acc();
                    MenuCharParaSet(MenuWork.character.chrNo, 0);
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData.repeat == PAD_LEFT) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[MenuWork.character.accessoryList * 5]);
                    if (MenuWork.character.accessoryList == 0) {
                        MenuWork.character.accessoryList = 2;
                    } else {
                        MenuWork.character.accessoryList--;
                    }
                    CharListMake_Acc();
                    MenuCharParaSet(MenuWork.character.chrNo, 1);
                    MenuWork.character.listSelect = -1;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            break;
        }

        /* Equipment: pick the bullet for the weapon. */
        case CHAR_STATE_BULLET_LIST:
            MenuWork.character.equipSlot = 1;
            MenuWork.character.listSelect = -1;
            MenuWork.character.bulletFits = 0;
            MenuWork.character.bulletId = 0;
            MenuWork.character.weaponId = func_A191C0(MenuWork.character.chrNo)->weapon[setSlotTbl[MenuWork.character.chrNo - 1]];
            MenuWork.character.state = CHAR_STATE_BULLET_LIST_RUN;
        case CHAR_STATE_BULLET_LIST_RUN: {
            CharParaData *org;
            int selected;

            if (MenuWork.character.wait != 0) {
                break;
            }
            org = func_A191C0(MenuWork.character.chrNo);
            selected = WindowSPSelect(CharWinSP[0], PadData.repeat);
            WindowSPKeepSelect(&CharList->weapon, &MenuKeepSelect[KEEP_BULLET * 5]);
            if (MenuWork.character.listSelect != selected) {
                MenuBulletData *bullet;
                int fitsWeapon;

                MenuWork.character.bulletId = MenuSortGet(0, selected);
                bullet = func_A1A428(MenuWork.character.bulletId);
                if (bullet != 0) {
                    fitsWeapon = bullet->weapon;
                } else {
                    fitsWeapon = 0;
                }
                if (org->weapon[setSlotTbl[MenuWork.character.chrNo - 1]] == fitsWeapon) {
                    MenuWork.character.bulletFits = 1;
                } else {
                    MenuWork.character.bulletFits = 0;
                }
                if (MenuWork.character.bulletFits != 0) {
                    short old = org->bullet[setSlotTbl[MenuWork.character.chrNo - 1]];

                    org->bullet[setSlotTbl[MenuWork.character.chrNo - 1]] = MenuWork.character.bulletId;
                    MenuCharParaSet(MenuWork.character.chrNo, 1);
                    org->bullet[setSlotTbl[MenuWork.character.chrNo - 1]] = old;
                } else {
                    MenuCharParaSet(MenuWork.character.chrNo, 0);
                }
                MenuWork.character.listSelect = selected;
            }
            if ((PadData.pressed & PAD_CIRCLE) && selected >= 0 && MenuWork.character.bulletFits != 0) {
                short old = org->bullet[setSlotTbl[MenuWork.character.chrNo - 1]];
                short weapon;

                org->bullet[setSlotTbl[MenuWork.character.chrNo - 1]] = MenuWork.character.bulletId;
                MenuCharParaSet(MenuWork.character.chrNo, 0);
                if (MenuWork.character.bulletId != 0) {
                    weapon = org->weapon[setSlotTbl[MenuWork.character.chrNo - 1]];
                    if (weapon != 0 && func_A1A3D8(weapon)->bullet != MenuWork.character.bulletId) {
                        dataBltBoxDec(MenuWork.character.bulletId);
                    }
                }
                if (old != 0) {
                    weapon = org->weapon[setSlotTbl[MenuWork.character.chrNo - 1]];
                    if (weapon != 0) {
                        if (func_A1A3D8(weapon)->bullet != old || dataBltBoxChk(old) == 0) {
                            dataBltBoxInc(old);
                        }
                    } else {
                        dataBltBoxInc(old);
                    }
                }
                if (MenuWork.character.chrNo == CHR_DUAL_WEAPON) {
                    org->bullet[1] = MenuWork.character.bulletId;
                }
                MenuWork.character.state = CHAR_STATE_EQUIP;
                MenuWork.character.wait = 8;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuCharParaSet(MenuWork.character.chrNo, 0);
                MenuWork.character.state = CHAR_STATE_EQUIP;
                MenuWork.character.wait = 8;
                xglSoundEffectNormalID(SE_CANCEL, 0);
                WindowSPKeepSelectCheck(&MenuKeepSelect[KEEP_BULLET * 5]);
            }
            break;
        }

        /* Parameter points: pick the parameter. */
        case CHAR_STATE_PARA:
            MenuWork.character.state = CHAR_STATE_PARA_RUN;
        case CHAR_STATE_PARA_RUN:
            if (MenuWork.character.wait == 0) {
                MenuWork.character.listSelect = WindowSPSelect(CharWinSP[0], PadData.repeat);
                WindowSPKeepSelect(CharWinSP[0], &MenuKeepSelect[KEEP_PARAMETER * 5]);
                if (MenuWork.character.listSelect >= 0) {
                    MenuWork.character.paraIndex = MenuWork.character.listSelect;
                }
                if ((PadData.pressed & PAD_CIRCLE) && MenuWork.character.listSelect >= 0) {
                    if (MenuParaUpCheck(MenuWork.character.chrNo, MenuWork.character.paraIndex) == 0) {
                        MenuWork.character.state = CHAR_STATE_PARA_AMOUNT;
                        MenuWork.character.wait = 4;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                        MenuWork.character.state = CHAR_STATE_PARA_DENIED;
                    }
                }
                if (PadData.pressed & PAD_CROSS) {
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.character.menuCursor = 0;
                    }
                    WindowSPKeepSelectCheck(&MenuKeepSelect[KEEP_PARAMETER * 5]);
                    MenuWork.character.state = CHAR_STATE_COMMAND_RUN;
                    MenuWork.character.wait = 8;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            } else {
                MenuWork.character.listSelect = WindowSPSelect(CharWinSP[0], 0);
                if (MenuWork.character.listSelect >= 0) {
                    MenuWork.character.paraIndex = MenuWork.character.listSelect;
                }
            }
            break;

        /* Parameter points: choose how many levels to add. */
        case CHAR_STATE_PARA_AMOUNT:
            memset(UpPara, 0, sizeof(UpPara));
            MenuWork.character.paraCost = MenuParaNextPointGet(MenuWork.character.chrNo, MenuWork.character.paraIndex, 0);
            MenuWork.character.paraUpCount = 1;
            UpPara[MenuWork.character.paraIndex] = ParaPointTbl[MenuWork.character.paraIndex];
            if (UpPara[MenuWork.character.paraIndex] + MenuParaPtNowGet(MenuWork.character.chrNo, MenuWork.character.paraIndex)
                >= MenuParaUpMaxGet(MenuWork.character.paraIndex)) {
                UpPara[MenuWork.character.paraIndex] = MenuParaUpMaxGet(MenuWork.character.paraIndex)
                                           - MenuParaPtNowGet(MenuWork.character.chrNo, MenuWork.character.paraIndex);
            }
            MenuWork.character.state = CHAR_STATE_PARA_AMOUNT_RUN;
        case CHAR_STATE_PARA_AMOUNT_RUN:
            if (MenuWork.character.wait != 0) {
                break;
            }
            if (PadData.repeat & PAD_DOWN) {
                if (MenuWork.character.paraUpCount != 1) {
                    MenuWork.character.paraUpCount--;
                    MenuWork.character.paraCost -= MenuParaNextPointGet(MenuWork.character.chrNo, MenuWork.character.paraIndex, MenuWork.character.paraUpCount);
                    UpPara[MenuWork.character.paraIndex] = ParaPointTbl[MenuWork.character.paraIndex] * MenuWork.character.paraUpCount;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (PadData.repeat & PAD_UP) {
                CharPointData *unit = func_A19210(MenuWork.character.chrNo);
                int cost = MenuWork.character.paraCost;
                int next = UpPara[MenuWork.character.paraIndex] + ParaPointTbl[MenuWork.character.paraIndex]
                         + MenuParaPtNowGet(MenuWork.character.chrNo, MenuWork.character.paraIndex);
                int max = MenuParaUpMaxGet(MenuWork.character.paraIndex);

                next = next / ParaPointTbl[MenuWork.character.paraIndex] * ParaPointTbl[MenuWork.character.paraIndex];
                cost += MenuParaNextPointGet(MenuWork.character.chrNo, MenuWork.character.paraIndex, MenuWork.character.paraUpCount);
                if (max >= next && unit->points >= cost) {
                    MenuWork.character.paraCost = cost;
                    MenuWork.character.paraUpCount++;
                    UpPara[MenuWork.character.paraIndex] = ParaPointTbl[MenuWork.character.paraIndex] * MenuWork.character.paraUpCount;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
                if (UpPara[MenuWork.character.paraIndex] + MenuParaPtNowGet(MenuWork.character.chrNo, MenuWork.character.paraIndex)
                    >= MenuParaUpMaxGet(MenuWork.character.paraIndex)) {
                    UpPara[MenuWork.character.paraIndex] = MenuParaUpMaxGet(MenuWork.character.paraIndex)
                                               - MenuParaPtNowGet(MenuWork.character.chrNo, MenuWork.character.paraIndex);
                }
            }
            if (PadData.pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.character.state = CHAR_STATE_PARA_CONFIRM;
                MenuWork.character.wait = 8;
            }
            if (PadData.pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.character.state = CHAR_STATE_PARA_RUN;
                MenuWork.character.wait = 4;
                memset(UpPara, 0, sizeof(UpPara));
            }
            break;

        /* Parameter points: confirm. */
        case CHAR_STATE_PARA_CONFIRM:
            MenuWork.character.state = CHAR_STATE_PARA_CONFIRM_RUN;
            MenuWork.character.confirmCursor = 0;
        case CHAR_STATE_PARA_CONFIRM_RUN: {
            int n;

            if (MenuWork.character.wait != 0) {
                break;
            }
            MenuWork.character.confirmCursor = MenuSelectMove(MenuWork.character.confirmCursor, 2, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                if (MenuWork.character.confirmCursor == 0) {
                    for (n = 0; n < MenuWork.character.paraUpCount; n++) {
                        MenuParaUp(MenuWork.character.chrNo, MenuWork.character.paraIndex);
                    }
                    memset(UpPara, 0, sizeof(UpPara));
                    MenuCharParaSet(MenuWork.character.chrNo, 0);
                    MenuWork.character.state = CHAR_STATE_PARA_DONE;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    MenuWork.character.state = CHAR_STATE_PARA_AMOUNT_RUN;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.character.state = CHAR_STATE_PARA_AMOUNT_RUN;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            break;
        }

        case CHAR_STATE_PARA_DONE:
            if (MenuWork.character.wait == 0 && (PadData.pressed & PAD_CIRCLE)) {
                MenuWork.character.state = CHAR_STATE_PARA_RUN;
                CharListMake_Para();
            }
            break;

        case CHAR_STATE_PARA_DENIED:
            if (MenuWork.character.wait == 0 && PadData.pressed != 0) {
                MenuWork.character.state = CHAR_STATE_PARA_RUN;
            }
            break;

        /* Formation (from battle): pick the attacker. */
        case CHAR_STATE_FORMATION:
            MenuWork.character.state = CHAR_STATE_FORMATION_RUN;
            MenuWork.character.chrNo = MenuCharParty[MenuWork.character.partyCursor];
        case CHAR_STATE_FORMATION_RUN:
            if (MenuWork.character.wait == 0) {
                if (PadData.repeat == PAD_CROSS) {
                    MenuWork.character.state = CHAR_STATE_EXIT;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
                if (PadData.repeat == PAD_CIRCLE) {
                    if (MenuWork.character.chrNo != 11 && MenuWork.character.chrNo != CHR_GUEST) {
                        MenuWork.character.state = CHAR_STATE_FORMATION_MENU;
                        MenuWork.character.wait = 8;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                MenuWork.character.partyCursor = MenuSelectMove(MenuWork.character.partyCursor, MenuWork.character.partyCount, 1);
                MenuWork.character.chrNo = MenuCharParty[MenuWork.character.partyCursor];
            }
            break;

        /* Formation: position or swap. */
        case CHAR_STATE_FORMATION_MENU:
            MenuWork.character.state = CHAR_STATE_FORMATION_MENU_RUN;
            MenuWork.character.chrNo = MenuCharParty[MenuWork.character.partyCursor];
        case CHAR_STATE_FORMATION_MENU_RUN:
            if (MenuWork.character.wait == 0) {
                MenuWork.character.commandCursor = MenuSelectMove(MenuWork.character.commandCursor, 3, 0);
                if (PadData.pressed & PAD_CROSS) {
                    MenuWork.character.state = CHAR_STATE_FORMATION;
                    MenuWork.character.wait = 8;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
                if (PadData.pressed & PAD_CIRCLE) {
                    switch (MenuWork.character.commandCursor) {
                    case 0:
                        MenuWork.character.formationSlot = (PartyDataGet())
                                                     ->attack_slots[MenuWork.character.partyCursor].attack_position;
                        MenuWork.character.formationColumn = (MenuWork.character.formationSlot - 1) % 3;
                        if (MenuWork.character.formationSlot < 4) {
                            MenuWork.character.formationRow = 0;
                        } else {
                            MenuWork.character.formationRow = 1;
                        }
                        MenuWork.character.state = CHAR_STATE_FORMATION_MOVE;
                        MenuWork.character.wait = 16;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        break;
                    case 1:
                        if (MenuWork.character.reserveCount != 0
                            && PartyFriendLockCheck(MenuWork.character.chrNo, 2) != 0) {
                            MenuWork.character.state = CHAR_STATE_RESERVE;
                            MenuWork.character.wait = 8;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                        break;
                    case 2:
                        MenuWork.character.state = CHAR_STATE_FORMATION;
                        MenuWork.character.wait = 8;
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        break;
                    }
                }
            }
            break;

        /* Formation: move the cursor over the six positions. */
        case CHAR_STATE_FORMATION_MOVE:
            MenuWork.character.state = CHAR_STATE_FORMATION_MOVE_RUN;
        case CHAR_STATE_FORMATION_MOVE_RUN:
            if (MenuWork.character.wait == 0) {
                if (PadData.repeat & PAD_DOWN) {
                    if (MenuWork.character.formationColumn == 0) {
                        MenuWork.character.formationColumn = 2;
                    } else {
                        MenuWork.character.formationColumn--;
                    }
                    xglSoundEffectNormalID(SE_SLIDE, 0);
                }
                if (PadData.repeat & PAD_UP) {
                    if (MenuWork.character.formationColumn == 2) {
                        MenuWork.character.formationColumn = 0;
                    } else {
                        MenuWork.character.formationColumn++;
                    }
                    xglSoundEffectNormalID(SE_SLIDE, 0);
                }
                if (PadData.repeat & (PAD_RIGHT | PAD_LEFT)) {
                    MenuWork.character.formationRow = (MenuWork.character.formationRow + 1) & 1;
                    xglSoundEffectNormalID(SE_SLIDE, 0);
                }
                MenuWork.character.formationSlot = MenuWork.character.formationColumn + MenuWork.character.formationRow * 3 + 1;
                if (PadData.repeat & PAD_CIRCLE) {
                    int occupant = PartyAttackPosCheck(MenuWork.character.formationSlot);

                    if (occupant == 0) {
                        MenuWork.character.state = CHAR_STATE_FORMATION_PLACE;
                        MenuWork.character.wait = 4;
                        xglSoundEffectNormalID(SE_PLACE, 0);
                    } else if (occupant != MenuWork.character.chrNo) {
                        MenuWork.character.state = CHAR_STATE_FORMATION_EXCHANGE;
                        MenuWork.character.wait = 4;
                        xglSoundEffectNormalID(SE_PLACE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                if (PadData.repeat & PAD_CROSS) {
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.character.commandCursor = 0;
                    }
                    MenuWork.character.state = CHAR_STATE_FORMATION;
                    MenuWork.character.wait = 4;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            break;

        /* Formation: exchange positions with the occupant. */
        case CHAR_STATE_FORMATION_EXCHANGE:
            MenuWork.character.state = CHAR_STATE_FORMATION_EXCHANGE_RUN;
        case CHAR_STATE_FORMATION_EXCHANGE_RUN:
            if (MenuWork.character.wait == 0) {
                if (PadData.pressed & PAD_CIRCLE) {
                    PartyAttackPosChange(MenuWork.character.chrNo, PartyAttackPosCheck(MenuWork.character.formationSlot));
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuWork.character.state = CHAR_STATE_FORMATION_DONE;
                    MenuWork.character.wait = 8;
                }
                if (PadData.pressed & PAD_CROSS) {
                    MenuWork.character.state = CHAR_STATE_FORMATION_MOVE;
                    MenuWork.character.wait = 4;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            break;

        /* Formation: move to the empty position. */
        case CHAR_STATE_FORMATION_PLACE:
            if (MenuWork.character.wait == 0) {
                if (PadData.pressed & PAD_CIRCLE) {
                    PartyAttackPosSet(MenuWork.character.chrNo, MenuWork.character.formationSlot);
                    MenuWork.character.wait = 8;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuWork.character.state = CHAR_STATE_FORMATION_DONE;
                }
                if (PadData.pressed & PAD_CROSS) {
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    MenuWork.character.state = CHAR_STATE_FORMATION_MOVE;
                }
            }
            break;

        case CHAR_STATE_FORMATION_DONE:
            if (MenuWork.character.wait == 0) {
                MenuWork.character.state = CHAR_STATE_FORMATION_MENU;
                MenuWork.character.wait = 8;
            }
            break;

        /* Formation: swap the attacker with a reserve friend. */
        case CHAR_STATE_RESERVE:
            MenuWork.character.state = CHAR_STATE_RESERVE_RUN;
            if (MenuCursorKeepCheck() == 0) {
                MenuWork.character.reserveCursor = 0;
            }
        case CHAR_STATE_RESERVE_RUN: {
            if (MenuWork.character.wait != 0) {
                break;
            }
            if (PadData.repeat & PAD_UP) {
                if (MenuWork.character.reserveCursor == 0) {
                    MenuWork.character.reserveCursor = MenuWork.character.reserveCount - 1;
                } else {
                    MenuWork.character.reserveCursor--;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.repeat & PAD_DOWN) {
                if (MenuWork.character.reserveCursor == MenuWork.character.reserveCount - 1) {
                    MenuWork.character.reserveCursor = 0;
                } else {
                    MenuWork.character.reserveCursor++;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.character.commandCursor = 0;
                }
                MenuWork.character.state = CHAR_STATE_FORMATION_MENU;
                MenuWork.character.wait = 4;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            if (PadData.pressed & PAD_CIRCLE) {
                /* The slot's byte offset in the party state (4-byte slots
                 * from +0x30) is computed as an unsigned integer before it
                 * is added to the pointer (0x00299a18..0x00299a24). */
                PartyAttackPosition *slot = (PartyAttackPosition *)((unsigned char *)PartyDataGet()
                    + (unsigned int)(MenuWork.character.partyCursor * 4 + 0x30));
                short inId = MenuCharPartyReser[MenuWork.character.reserveCursor];
                unsigned short outId = slot->party_id;
                unsigned short *attacker;
                unsigned short *reserve;
                unsigned short swap;
                int attackers[8];
                int n;

                slot->party_id = inId;
                if (PartyLeaderCheck(outId) != 0) {
                    PartyLeaderSet(inId);
                }
                attacker = &MenuCharParty[MenuWork.character.partyCursor];
                reserve = &MenuCharPartyReser[MenuWork.character.reserveCursor];
                swap = *attacker;
                *attacker = *reserve;
                *reserve = swap;
                MenuWork.character.swapOutId = outId;
                MenuWork.character.swapInId = inId;
                MenuWork.character.partyCount = PartyAttackerGet(attackers);
                for (n = 0; n < MenuWork.character.partyCount; n++) {
                    MenuCharParty[n] = attackers[n];
                }
                xglSoundEffectNormalID(SE_SWAP, 0);
                MenuWork.character.state = CHAR_STATE_RESERVE_MODEL;
                MenuWork.character.wait = 8;
            }
            break;
        }

        /* Formation: rebuild the swapped-in character's model. */
        case CHAR_STATE_RESERVE_MODEL:
            if (MenuWork.character.wait != 0 || MenuModelOut[MenuWork.character.partyCursor] != 0) {
                break;
            }
            MenuModelCreate(&MenuModelOut[MenuWork.character.partyCursor], MenuWork.character.swapInId);
            MenuModelExtFuncSet(MenuModelOut[MenuWork.character.partyCursor], MenuBattleModelMain, 0);
            MenuWork.character.wait = 1;
            MenuWork.character.state = CHAR_STATE_RESERVE_MODEL_RUN;
        case CHAR_STATE_RESERVE_MODEL_RUN:
            if (MenuWork.character.wait == 0) {
                MenuWork.character.state = CHAR_STATE_FORMATION_MENU;
                MenuWork.character.wait = 4;
            }
            break;

        case CHAR_STATE_EXIT:
            MenuWork.character.state = CHAR_STATE_EXIT_RUN;
            MenuWork.character.wait = 12;
        case CHAR_STATE_EXIT_RUN:
            if (MenuWork.character.wait == 0) {
                MenuWork.character.state = MENU_WORK_END;
            }
            break;

        case MENU_WORK_END:
            ChangeTopLevel(0);
            MenuModelAllBreak();
            break;
        }
    }
    CharPasMain();
    CharInfoMain();
    CharPointMain();
    CharMenuMain();
    CharParameterMain();
    CharListMain();
    CharStatusMain();
    CharExMain();
    CharWeaponMain();
    CharSwitchMain();
    CharCategoryMain();
    CharL1R1Main();
    xglFontDebugPrintf(0, 0x40, charactorText);
    xglFontDebugHex(0, 0x48, MenuWork.character.state, 2);
    xglFontDebugHex(0x10, 0x48, MenuWork.character.partyMode, 2);
    xglFontDebugHex(0, 0x38, MenuWork.character.accessoryList, 2);
    xglFontDebugHex(0, 0x50, MenuWork.character.listSelect, 2);
    xglFontDebugHex(0, 0x58, MenuWork.character.partyCursor, 2);
    xglFontDebugHex(0, 0x60, MenuWork.character.formationColumn, 2);
    xglFontDebugHex(0, 0x68, MenuWork.character.formationRow, 2);
    xglFontDebugHex(0, 0x70, MenuWork.character.formationSlot, 2);
    xglFontDebugHex(0, 0x78, MenuWork.character.unknown_47, 2);
    xglFontDebugHex(0, 0x88, MenuWork.character.reserveCount, 2);
    xglFontDebugHex(0, 0x90, MenuWork.character.reserveCursor, 2);
    xglFontDebugHex(0x40, 0x88, MenuWork.character.equipKind, 2);
    xglFontDebugHex(0x40, 0x90, MenuWork.character.equipSlot, 2);
    xglFontDebugHex(0, 0x98, MenuWork.character.unknown_52[0], 2);
    xglFontDebugHex(0, 0xA0, MenuWork.character.unknown_52[1], 2);
    xglFontDebugHex(0, 0xA8, MenuWork.character.unknown_52[2], 2);
    xglFontDebugHex(0, 0xB0, MenuWork.character.unknown_52[3], 2);
    xglFontDebugHex(0, 0xC0, MenuWork.character.wait, 2);
    xglFontDebugHex(0, 0xC8, MenuWork.character.unknown_4b[0], 2);
    xglFontDebugHex(0, 0xD0, MenuWork.character.unknown_4b[1], 2);
    xglFontDebugHex(0, 0xD8, MenuWork.character.partyCount, 2);
    xglFontDebugHex(0x20, 0xB0, MenuWork.character.weaponChanged, 8);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Original LOCAL NOBITS storage, 16 bytes each; the existing union keeps
 * the named Vector4 view and the compiler's aggregate-copy view together. */
const char MenuAgwsAssignPilotCommandLabel[16] = "/Assign Pilot";

const char MenuAgwsEngineUnitCommandLabel[16] = "/Engine Unit";

const char MenuAgwsAccessoryCommandLabel[16] = "/Accessory";

const char MenuAgwsLeftShoulderCommandLabel[16] = "/L. Shoulder";

const char MenuAgwsRightShoulderCommandLabel[16] = "/R. Shoulder";

const char MenuAgwsUnequipCommandLabel[16] = "/Unequip";

const char MenuAgwsNameLabel[16] = "A.G.W.S.";

const char MenuAgwsSelectPromptText[19] = "Select an A.G.W.S.";

const char MenuAgwsViewHelpText[42] = "View weapons and accessories for A.G.W.S.";

const char MenuAgwsAssignHelpText[26] = "Assign pilots to A.G.W.S.";

const char MenuAgwsUnequipHelpText[41] = "Unequip current weapons and accessories.";

const char MenuAgwsEquipHelpText[31] = "Equip weapons and accessories.";

const char MenuAgwsLeftShoulderLabel[12] = "L. Shoulder";

const char MenuAgwsRightShoulderLabel[12] = "R. Shoulder";

const char MenuAgwsAccessoryAvailableText[31] = "Available for accessory equip.";

const char MenuAgwsAmmoRemoveBlockedText[30] = "Ammo alone cannot be removed.";

const char MenuAgwsNothingEquippedText[18] = "Nothing equipped.";

const char MenuAgwsWeaponAvailableText[28] = "Available for weapon equip.";

const char MenuAgwsEquipWeaponOnText[17] = "Equip weapon on ";

const char MenuAgwsAndEquipOnText[15] = " and equip on ";

const char MenuAgwsRemoveWeaponFromText[20] = "Remove weapon from ";

const char MenuAgwsAndEquipAnotherText[27] = " and equip another weapon.";

const char MenuAgwsAsPilotOfThisText[31] = " as the pilot of this A.G.W.S.";

const char MenuAgwsIsAssignedToText[17] = " is assigned to ";

const char MenuAgwsLeftShoulderSlotText[13] = "L. Shoulder\n";

const char MenuAgwsRightShoulderSlotText[13] = "R. Shoulder\n";

const char MenuAgwsAltSlotChoicesText[16] = "R. Alt\nLft. Alt";

const char MenuAgwsShoulderSlotChoicesText[24] = "R. Shoulder\nL. Shoulder";

const char MenuAgwsArmSlotChoicesText[16] = "R. Arm\nLft. Arm";

/* Text: {color #808080}R. Arm\n{color #808080}L. Arm\n{color #808080}R. Shoulder\n{color #808080}L. Shoulder\n{color #808080}R. Alt\n{color #808080}L. Alt */
const char MenuAgwsGrayedSlotChoicesText[76] = "\014\200\200\200R. Arm\n\014\200\200\200L. Arm\n\014\200\200\200R. Shoulder\n\014\200\200\200L. Shoulder\n\014\200\200\200R. Alt\n\014\200\200\200L. Alt";

const char MenuAgwsConfirmPromptText[15] = " Is this okay\?";

const char MenuAgwsPilotChoicesText[26] = "Assign pilot\nRemove pilot";

const char MenuAgwsEquipUnequipCancelText[21] = "Equip\nUnequip\nCancel";

const char MenuAgwsEquipUnequipPilotText[20] = "Equip\nUnequip\nPilot";

const char MenuAgwsAssignRemoveCancelText[21] = "Assign\nRemove\nCancel";

const char yesNoChoicesText[8] = "Yes\nNo";

const char menuTitleLabel[8] = "Menu";

const char selectTitleLabel[8] = "Select";

/* Text: \x01S.Pts */
const char menuParaPtRateGetSPtsText[8] = "\001S.Pts";

/* Text: \x01E.Pts */
const char menuParaPtRateGetEPtsText[8] = "\001E.Pts";

/* Text: \x01T.Pts */
const char menuParaPtRateGetTPtsText[8] = "\001T.Pts";

const L1R1SpriteIdPair l1r1SpriteIdTable[2] = {{0x0112, 0x0110}, {0, 0}};

const L1R1SlideStep l1r1SlideStepTable[4] = {{1, -1}, {0, 0}, {0, 0}, {0, 0}};

/* Text: \x01Weight */
const char weightText[8] = "\001Weight";

/* Text: \x01Agl */
const char agilityText[8] = "\001Agl";

/* Text: \x01Eva */
const char menuParaPtRateGetEvasionText[8] = "\001Eva";

/* Text: \x01Dex */
const char menuParaPtRateGetDexterityText[8] = "\001Dex";

/* Text: \x01Edef */
const char menuParaPtRateGetEdefText[8] = "\001Edef";

/* Text: \x01Eatk */
const char menuParaPtRateGetEatkText[8] = "\001Eatk";

/* Text: \x01Vit */
const char menuParaPtRateGetVitalityText[8] = "\001Vit";

/* Text: \x01Str */
const char menuParaPtRateGetStrengthText[8] = "\001Str";

ParaDigitCounts paraDigitCountTable[1] = {{{3, 3, 3, 3, 2, 2, 2, 3}}};

const AccessoryIconTable charAccessoryIconTable[1] = {{{0x032E, 0x032D, 0x030D}}};

char listWindowTag[8] = "List";

const char tPtsTitleLabel[8] = "T.Pts";

/* Text: \x01Total */
const char totalText[8] = "\001Total";

/* Text:  | @x@ */
const unsigned char rowOpacityTable[5] = "\000@x@";

/* Text:  | @xx@ */
const unsigned char columnOpacityTable[6] = "\000@xx@";

unsigned short *ParaDataBuf = 0;

CharPasWork *CharPas = 0;

CharInfoWork *CharInfo = 0;

CharCategoryWork *CharCategory = 0;

CharMenuWork *CharMenu = 0;

CharExWork *CharEx = 0;

CharL1R1Work *CharL1R1 = 0;

CharStatusBlock *CharStatus = 0;

CharParameterWork *CharParameter = 0;

CharWeaponWork *CharWeapon = 0;

CharListWork *CharList = 0;

CharSwitchWork *CharSwitch = 0;

CharPointWork *CharPoint = 0;

const char MenuAgwsRemoveCommandLabel[8] = "/Remove";

const char MenuAgwsAssignCommandLabel[8] = "/Assign";

const char MenuAgwsAmmoCommandLabel[8] = "/Ammo";

const char MenuAgwsLeftAltCommandLabel[8] = "/L. Alt";

const char MenuAgwsRightAltCommandLabel[8] = "/R. Alt";

const char MenuAgwsLeftArmCommandLabel[8] = "/L. Arm";

const char MenuAgwsRightArmCommandLabel[8] = "/R. Arm";

const char MenuAgwsListCommandLabel[8] = "/List";

const char MenuAgwsPilotCommandLabel[8] = "/Pilot";

const char MenuAgwsEquipCommandLabel[8] = "/Equip";

const char MenuAgwsLeftAltLabel[7] = "L. Alt";

const char MenuAgwsRightAltLabel[7] = "R. Alt";

const char MenuAgwsLeftArmLabel[7] = "L. Arm";

const char MenuAgwsRightArmLabel[7] = "R. Arm";

const char MenuAgwsSeparatorText[3] = ": ";
