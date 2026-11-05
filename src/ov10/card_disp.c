/*
 * OV10 original TU 6: 0x00a1e5d8..0x00a21348 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"

/*
 * 16-byte float vector the original copies with doubleword moves (ld/sd pairs): the u64 member gives
 * the aggregate the 8-byte alignment that avoids ldl/ldr pairs (same shape as CardTutorialTranslation
 * in card_tutorial.c).
 */
typedef union CardHelpFloat4 {
    float v[4];
    u64 words[2];
} CardHelpFloat4;

void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                            const float scale[4]);
void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                            const float translation[4]);
void CardDispLight(Matrix4 matrix, s16 card, s16 enabled, u8 color);
extern void nmlModelEntry(u32 model);
extern void CardDisp(Matrix4 matrix, s16 kind, s16 flagA, s16 flagB);
extern void nmlModelEntryCard(void *model);
extern void nmlModelSetPlace(const Vector4 *matrix);
extern void nmlModelSetRenderLevel(int level);
extern void nmlModelSetTexture(const char *texture);

extern const CardHelpFloat4 cardDispCtrTranslation;
extern const CardHelpFloat4 cardDispTurnTranslation;
extern const CardHelpFloat4 cardDispTurnScale;
extern const CardHelpFloat4 cardDispShineCurryScale;
extern const CardHelpFloat4 cardDispAttrTranslation;
extern const CardHelpFloat4 cardDispNazoTranslation;
extern const CardHelpFloat4 cardDispGunoTranslation;
extern const CardHelpFloat4 cardDispCommTranslation;
extern const CardHelpFloat4 cardDispPhaseTranslation;
extern const char *cardxtx_tbl[];
extern u32 cardlex_tbl[];

extern const CardHelpFloat4 cardDispDeckMakeTranslation;
extern const CardHelpFloat4 cardDispTejyunTranslation;
extern const CardHelpFloat4 cardDispTejyunOffset;
extern const CardHelpFloat4 cardDispTejyunScale;
extern const CardHelpFloat4 cardDispDeckMakeScale;

/* Original 16-byte vectors (translation, scale and place constants) of the help screen. */
const CardHelpFloat4 cardDispCtrTranslation = {{ 1.48f, 0.16f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispDeckMakeTranslation = {{ 1.37f, 0.2f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispDeckMakeScale = {{ 0.9f, 0.9f, 1.0f, 1.0f }};
const CardHelpFloat4 cardDispTurnTranslation = {{ 1.5f, 0.18f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispTurnScale = {{ 0.9f, 0.9f, 1.0f, 1.0f }};
const CardHelpFloat4 cardDispTejyunTranslation = {{ 1.397f, -0.317f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispTejyunOffset = {{ 1.397f, 1.457f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispTejyunScale = {{ 0.9f, 0.9f, 1.0f, 1.0f }};
const CardHelpFloat4 cardDispGunTranslation = {{ 1.397f, 0.863f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispGunScale = {{ 0.6f, 0.6f, 1.0f, 1.0f }};
const CardHelpFloat4 cardDispShineCurryScale = {{ 0.17f, 0.17f, 1.0f, 1.0f }};
const CardHelpFloat4 cardDispAttrTranslation = {{ -2.64f, 1.16f, 0.02f, 1.0f }};
const CardHelpFloat4 cardDispNazoTranslation = {{ -2.2f, 0.58f, 0.02f, 1.0f }};
const CardHelpFloat4 cardDispGunoTranslation = {{ -1.1f, 1.53f, 0.02f, 1.0f }};
const CardHelpFloat4 cardDispOnceTranslation = {{ 1.397f, 0.179f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispCommTranslation = {{ 1.36f, 0.15f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispPhaseTranslation = {{ 1.36f, 0.15f, 0.0f, 1.0f }};
const char *cardxtx_tbl[64] = {0};
u32 cardlex_tbl[64] = {0};
extern char *strcpy(char *destination, const char *source);

/* 16-byte float vector with doubleword alignment (see CardDispHelpSlide2/3). */
typedef unsigned int Quadword __attribute__((mode(TI)));
typedef union CardHelpVector {
    float v[4];
    Quadword q;
} CardHelpVector;
extern void nmlModelSetLight(Matrix4 color, Matrix4 direction);
extern float xglSin(float angle);
/*
 * Help resources: index of each file CardHelpFileLoad reads, in the order of its name list,
 * which is also the index into cardlex_tbl[] / cardxtx_tbl[]. The names are the disc file
 * names without the "h_" prefix.
 */
enum CardHelpResource {
    CARD_HELP_RES_TITTLE,
    CARD_HELP_RES_PH1,
    CARD_HELP_RES_PH2,
    CARD_HELP_RES_TEXT,
    CARD_HELP_RES_EVENT,
    CARD_HELP_RES_BATTLE1,
    CARD_HELP_RES_BATTLE2,
    CARD_HELP_RES_DOWN,
    CARD_HELP_RES_EMARK,
    CARD_HELP_RES_EBLAKE,
    CARD_HELP_RES_OPEN,
    CARD_HELP_RES_VIEWER,
    CARD_HELP_RES_DECKSAVE,
    CARD_HELP_RES_HELP5,
    CARD_HELP_RES_HELP6,
    CARD_HELP_RES_H1P2P,
    CARD_HELP_RES_BASE,
    CARD_HELP_RES_BATTL,
    CARD_HELP_RES_FIELD,
    CARD_HELP_RES_HAND,
    CARD_HELP_RES_HAND2,
    CARD_HELP_RES_JUNK,
    CARD_HELP_RES_LOST,
    CARD_HELP_RES_SITUATION,
    CARD_HELP_RES_STANDBY,
    CARD_HELP_RES_DECKMAKE,
    CARD_HELP_RES_DECKLIST,
    CARD_HELP_RES_PHASE1,
    CARD_HELP_RES_PHASE2,
    CARD_HELP_RES_PHASE3,
    CARD_HELP_RES_PHASE4,
    CARD_HELP_RES_PHASE5,
    CARD_HELP_RES_PHASE6,
    CARD_HELP_RES_PHASE7,
};

/*
 * Help pages: page ids are the indices of CardTitleTxtLst (0x00a483e8, 59 entries) and of the
 * jump table in CardDispHelpCard; each name spells that page's title string. Pages whose
 * title repeats get an _A/_B/_C suffix in id order.
 */
enum CardHelpPage {
    CARD_HELP_PAGE_WHAT_IS_XENO_CARD = 0,
    CARD_HELP_PAGE_CONTROLS = 1,
    CARD_HELP_PAGE_CARD_OUTLINE = 2,
    CARD_HELP_PAGE_GAME_PROGRESSION = 3,
    CARD_HELP_PAGE_PROGRESSION_PHASE = 4,
    CARD_HELP_PAGE_PROGRESSION_TURN = 5,
    CARD_HELP_PAGE_POSITIONING = 6,
    CARD_HELP_PAGE_DECK = 7,
    CARD_HELP_PAGE_LOST_PILE = 8,
    CARD_HELP_PAGE_HAND = 9,
    CARD_HELP_PAGE_JUNK_PILE = 10,
    CARD_HELP_PAGE_STANDBY_AREA = 11,
    CARD_HELP_PAGE_SITUATION_AREA = 12,
    CARD_HELP_PAGE_BATTLEFIELD = 13,
    CARD_HELP_PAGE_FIELD = 14,
    CARD_HELP_PAGE_CARD = 15,
    CARD_HELP_PAGE_BATTLE_CARD = 16,
    CARD_HELP_PAGE_EVENT_CARD = 17,
    CARD_HELP_PAGE_SITUATION_CARD = 18,
    CARD_HELP_PAGE_DETAILS = 19,
    CARD_HELP_PAGE_TYPES = 20,
    CARD_HELP_PAGE_NAMES = 21,
    CARD_HELP_PAGE_ATTRIBUTES = 22,
    CARD_HELP_PAGE_REQUIREMENTS_A = 23,
    CARD_HELP_PAGE_REQUIREMENTS_B = 24,
    CARD_HELP_PAGE_COST = 25,
    CARD_HELP_PAGE_HP = 26,
    CARD_HELP_PAGE_ATK = 27,
    CARD_HELP_PAGE_ATTACK_TYPE = 28,
    CARD_HELP_PAGE_TEXT = 29,
    CARD_HELP_PAGE_HOW_TO_PLAY = 30,
    CARD_HELP_PAGE_TURNS_A = 31,
    CARD_HELP_PAGE_TURNS_B = 32,
    CARD_HELP_PAGE_DRAW_PHASE = 33,
    CARD_HELP_PAGE_MOVE_PHASE = 34,
    CARD_HELP_PAGE_EVENT_PHASE_A = 35,
    CARD_HELP_PAGE_EVENT_PHASE_B = 36,
    CARD_HELP_PAGE_EVENT_PHASE_C = 37,
    CARD_HELP_PAGE_SET_PHASE_A = 38,
    CARD_HELP_PAGE_SET_PHASE_B = 39,
    CARD_HELP_PAGE_BLOCK_PHASE = 40,
    CARD_HELP_PAGE_BATTLE_PHASE_A = 41,
    CARD_HELP_PAGE_BATTLE_PHASE_B = 42,
    CARD_HELP_PAGE_ADJUST_PHASE = 43,
    CARD_HELP_PAGE_SPECIAL_CARD = 44,
    CARD_HELP_PAGE_WEAPONS_A = 45,
    CARD_HELP_PAGE_WEAPONS_B = 46,
    CARD_HELP_PAGE_GNOSIS = 47,
    CARD_HELP_PAGE_TERMINOLOGY = 48,
    CARD_HELP_PAGE_PENETRATE = 49,
    CARD_HELP_PAGE_DOWN = 50,
    CARD_HELP_PAGE_E_MARK = 51,
    CARD_HELP_PAGE_EVENT_BLOCK = 52,
    CARD_HELP_PAGE_OPENING_CARDS = 53,
    CARD_HELP_PAGE_VIEWING_CARDS = 54,
    CARD_HELP_PAGE_CREATING_DECKS = 55,
    CARD_HELP_PAGE_SAVING_DECKS = 56,
    CARD_HELP_PAGE_TWO_PLAYER_BATTLE = 57,
    CARD_HELP_PAGE_SELECTING_DECKS = 58,
    CARD_HELP_PAGE_COUNT = 59  /* slti 0x3B at 0x00a20d68 */
};
#define CARD_HELP_PAGE_LAST (CARD_HELP_PAGE_COUNT - 1)  /* li 0x3A at 0x00a20da8 */

static int ChangeCnt = 0;
extern Matrix4 asDir;

/*
 * Help-title list state CHPTitleInitSub resets when CardHelpProc
 * (0x00a20908) opens a help category; CHPMenuInitSub (0x00a1e640, not part
 * of this TU's claimed source) runs immediately after it. TitleMaxList
 * (config/symbols/ov10.txt, 7 bytes) holds one entry-count byte per
 * category, read here through a pointer captured early and dereferenced
 * once per field (count and limit each get their own read). Only the
 * members CHPTitleInitSub writes are named; +0x04 stays unmodeled because
 * this function does not touch it.
 */
typedef struct CHPTitleList {
    s16 cursor;      /* +0x00: selected row, reset to the top */
    s8 flagA;        /* +0x02 */
    s8 flagB;        /* +0x03 */
    u8 unmodeled_04;
    s8 flagC;        /* +0x05 */
    s16 x;           /* +0x06: screen column */
    s16 y;           /* +0x08: screen row */
    s16 rowHeight;   /* +0x0A */
    s16 scroll;      /* +0x0C: top row offset, reset */
    s16 step;        /* +0x0E: row step */
    s16 count;       /* +0x10: TitleMaxList[category], sign-extended */
    s16 limit;       /* +0x12: TitleMaxList[category], sign-extended */
    u32 entries[151]; /* +0x14: list rows, not read through this view */
} CHPTitleList;

extern const u8 TitleMaxList[7];

void CHPTitleInitSub(CHPTitleList *list, int category)
{
    const u8 *entry;
    u8 raw;

    list->x = 266;
    list->y = 50;
    entry = &TitleMaxList[category];
    list->rowHeight = 24;
    list->cursor = 0;
    list->flagA = 0;
    list->flagB = 0;
    list->flagC = 0;
    list->scroll = 0;
    raw = *entry;
    list->step = 1;
    list->count = (s8)raw;
    raw = *entry;
    list->limit = (s8)raw;
}

/*
 * CHPMenuInitSub (0x00a1e640, 0x64 bytes): resets the help-menu list at
 * cld0_17 (same field layout as CHPTitleList), then re-initialises the
 * title list cld1_18 for category 0 (TitleMaxList[0] = 6) by a tail jump
 * (0x00a1e69c `j CHPTitleInitSub`, a0 = 2nd argument, a1 = 0).
 * Callers: CardHelpProc 0x00a209ec and 0x00a21df0 (a0 = cld0_17, a1 = cld1_18).
 * CHPTitleInitSub is defined in this TU and returns void.
 */
void CHPMenuInitSub(CHPTitleList *menu, CHPTitleList *title)
{
    menu->x = 6;            /* 0x00a1e674 sh $6,0x6 */
    menu->y = 50;           /* 0x00a1e678 sh $7,0x8 */
    menu->rowHeight = 24;   /* 0x00a1e67c sh $8,0xA */
    menu->cursor = 0;
    menu->flagA = 0;
    menu->flagB = 0;
    menu->flagC = 0;
    menu->scroll = 0;
    menu->count = 9;        /* 0x00a1e680 sh $9,0x10 */
    menu->step = 1;         /* 0x00a1e684 sh $10,0xE */
    menu->limit = 7;        /* 0x00a1e670 sh $2,0x12 */
    CHPTitleInitSub(title, 0);
}

/*
 * CardHelpFileLoad (0x00a1e6a8, 0x294 bytes): loads the help-viewer resources.
 * For every name in the NULL-terminated list it reads "<dir><name>.lex" and then
 * "<dir><name>.xtx" from the disc into consecutive 16-byte aligned areas starting
 * at 0x01720000 and records each start address in cardlex_tbl[] / cardxtx_tbl[].
 * Same shape as CardTutorialInit (ov10/tu007).
 * Caller: CardMainInit 0x00a01a9c (no arguments, result unused). xglCdReadFile
 * returns the byte count (tested with blez v0 at 0x00a1e878 / 0x00a1e8e0); memset,
 * strcpy and strcat results are not read. `dir` and `names` are initialised
 * automatics: the compiler copies them from rodata (20 bytes + memset of the rest
 * at 0x00a1e6e4..0x00a1e714, 35 words at 0x00a1e738..0x00a1e7e4).
 */
#define CARD_HELP_FILE_BUFFER   0x01720000  /* lui s0,0x172 at 0x00a1e7f0 */
#define CARD_HELP_FILE_ALIGN    16          /* addiu 15 / and -16 at 0x00a1e884 */
#define CARD_HELP_FILE_MAX      64          /* loop bound s6 + 256 at 0x00a1e8e4 = sizeof cardlex_tbl / 4 */
#define CARD_HELP_NAME_COUNT    35          /* 34 names plus the NULL terminator (140-byte copy) */

void CardHelpFileLoad(void)
{
    char dir[64] = "data\\carddata\\help\\";
    char path[128];
    const char *names[CARD_HELP_NAME_COUNT] = {
        "h_tittle", "h_ph1", "h_ph2", "h_text", "h_event", "h_battle1", "h_battle2",
        "h_down", "h_Emark", "h_Eblake", "h_open", "h_viewer", "h_decksave", "help5",
        "help6", "h_1P2P", "h_base", "h_battl", "h_field", "h_hand", "h_hand2", "h_junk",
        "h_lost", "h_situation", "h_standby", "h_deckmake", "h_decklist", "h_Phase1",
        "h_Phase2", "h_Phase3", "h_Phase4", "h_Phase5", "h_Phase6", "h_Phase7", 0
    };
    u32 dest;
    int size;
    int i;

    dest = CARD_HELP_FILE_BUFFER;
    for (i = 0; i < CARD_HELP_FILE_MAX && names[i] != 0; i++) {
        memset(path, 0, sizeof(path));
        strcpy(path, dir);
        strcat(path, names[i]);
        strcat(path, ".lex");
        cardlex_tbl[i] = dest;
        size = xglCdReadFile(path, (void *)dest, 0, 1);
        if (size > 0) {
            dest += size;
            dest = (dest + (CARD_HELP_FILE_ALIGN - 1)) & ~(CARD_HELP_FILE_ALIGN - 1);
        }

        memset(path, 0, sizeof(path));
        strcpy(path, dir);
        strcat(path, names[i]);
        strcat(path, ".xtx");
        cardxtx_tbl[i] = (const char *)dest;
        size = xglCdReadFile(path, (void *)dest, 0, 1);
        if (size > 0) {
            dest += size;
            dest = (dest + (CARD_HELP_FILE_ALIGN - 1)) & ~(CARD_HELP_FILE_ALIGN - 1);
        }
    }
}

/*
 * CHPMenuBackDispSub (0x00a1e940, 0x130 bytes): three help-menu back panels, each
 * placed with xglMatrixTrans and entered with nmlModelEntryCard after
 * nmlModelSetTexture / nmlModelSetPlace / nmlModelSetRenderLevel(2).
 * Caller: CardHelpProc 0x00a20d34 (a0 = sp). All callees void (no $v0 reads).
 * The two float locals `left` and `zero` stay in callee-saved FP registers across
 * the calls; z is assigned before y in each position block.
 */
static void CHPMenuBackDispSub(Matrix4 matrix)
{
    Matrix4 transformed;
    Vector4 position;
    float left = -1.4184f;     /* 0xBFB58E22 */
    float zero = 0.0f;

    position.x = left;
    position.z = zero;
    position.y = zero;
    position.w = 1.0f;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, (const float *)&position);
    nmlModelSetTexture((const char *)0x01CB0000);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetRenderLevel(2);
    nmlModelEntryCard((void *)0x01CA2B00);

    position.x = 1.4184f;      /* 0x3FB58E22 */
    position.z = zero;
    position.y = zero;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, (const float *)&position);
    nmlModelSetTexture((const char *)0x01CB0000);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetRenderLevel(2);
    nmlModelEntryCard((void *)0x01CA1000);

    position.x = left;
    position.y = -1.832f;      /* 0xBFEA7EFA */
    xglMatrixTrans(transformed, (const float (*)[4])matrix, (const float *)&position);
    nmlModelSetTexture((const char *)0x01CC0050);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetRenderLevel(2);
    nmlModelEntryCard((void *)0x01CA4000);
}

/*
 * CardDispHelpCurrySet (0x00a1ea70, 0x140 bytes): three curry card models drawn at
 * translation (1.5143, y, 0, 1), scale (0.46, 0.46, 1, 1); y = 1.186, 0.215, -0.757
 * with card kinds 0, 0x54 (84), 0x78 (120) passed to CardDisp(.., 1, 0).
 * Caller: CardDispHelpCard 0x00a205f8. CardDisp is void (no $v0 read after
 * 0x00a1eb08/0x00a1eb4c/0x00a1eb90).
 */
void CardDispHelpCurrySet(Matrix4 matrix)
{
    Matrix4 transformed;
    float translation[4];
    float scale[4];

    translation[0] = 1.5143f;     /* 0x3FC1D495 */
    translation[1] = 1.186f;      /* 0x3F97CED9 */
    translation[2] = 0.0f;        /* sw $0 */
    translation[3] = 1.0f;
    scale[0] = 0.46f;             /* 0x3EEB851F */
    scale[1] = 0.46f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
    CardDisp(transformed, 0, 1, 0);

    translation[1] = 0.215f;      /* 0x3E5C28F6 */
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
    CardDisp(transformed, 84, 1, 0);

    translation[1] = -0.757f;     /* 0xBF41CAC1 */
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
    CardDisp(transformed, 120, 1, 0);
}

/*
 * Help-slide animation (shared by CardDispHelpSlide1/2/3): the counter ChangeCnt wraps
 * around the list length and the entry shown changes every HELP_SLIDE_ENTRY_TICKS.
 */
#define HELP_SLIDE_ENTRY_TICKS      64                                  /* sra 6 at 0x00a1ec6c */
#define HELP_SLIDE_CYCLE_4          (4 * HELP_SLIDE_ENTRY_TICKS)        /* 256: sra 8 / sll 8 at 0x00a1ec4c */
#define HELP_SLIDE_CYCLE_3          (3 * HELP_SLIDE_ENTRY_TICKS)        /* 192: li a3,0xc0 at 0x00a1ec9c */
#define HELP_SLIDE_CYCLE_6          (6 * HELP_SLIDE_ENTRY_TICKS)        /* 384: six-entry lists of CardDispHelpSlide2 */

/*
 * CardDispHelpSlide1 (0x00a1ebb0, 0x1e0 bytes): help pages 16..18 draw one card of a
 * short list at translation `trans`, scale (0.368, 0.368, 1, 1), picking the entry from
 * ChangeCnt. The lists (card kinds) and `trans` are the function's initialised statics
 * (list16_0 .. trans_3 at 0x00a43bd0..0x00a43bf0 in the original symbol table).
 * Callers: CardDispHelpCard 0x00a2060c, 0x00a20654, 0x00a20684 (a0 = page id from the
 * page jump table, a1 = matrix). CardDisp is void: no $v0 read after the calls at
 * 0x00a1ed00 and 0x00a1ed78.
 */
void CardDispHelpSlide1(s8 page, Matrix4 matrix)
{
    static s16 list16[4] = { 0, 42, 2, 29 };
    static s16 list17[3] = { 84, 85, 86 };
    static s16 list18[3] = { 120, 121, 122 };
    static float trans[4] = { 0.55715f, 1.246f, 0.0f, 1.0f };
    Matrix4 transformed;
    float scale[4];
    int i;

    scale[0] = 0.368f;     /* 0x3EBC6A7F, lui/ori at 0x00a1ebb4 */
    scale[1] = 0.368f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    switch (page) {
    case CARD_HELP_PAGE_BATTLE_CARD:               /* body at 0x00a1ec28 */
        ChangeCnt %= HELP_SLIDE_CYCLE_4;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, trans);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list16[i], 1, 0);
        break;
    case CARD_HELP_PAGE_EVENT_CARD:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, trans);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list17[i], 1, 0);
        break;
    case CARD_HELP_PAGE_SITUATION_CARD:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, trans);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list18[i], 1, 0);
        break;
    }
}

/*
 * CardDispHelpSlide2 (0x00a1ed90, 0x578 bytes): help pages 19..22, 25..28 and 45..47 draw
 * one card of a list at translation (1.397, 0.179, 0, 1), scale (0.9, 0.9, 1, 1); pages
 * 22 and 25..28 light it through CardDispLight with a colour mask. The cycle is the entry
 * count times HELP_SLIDE_ENTRY_TICKS: 192 for three entries (li 0xc0, e.g. 0x00a1ee0c),
 * 384 for six (li 0x180 at 0x00a1ef44) and 256 for four (sra/sll 8 at 0x00a1f144/48). As in
 * CardDispHelpSlide3, the place is a CardHelpVector so the copy is the doubleword
 * ld/sd pair (sd $3,0x40($29) at 0x00a1edd4). Callers: CardDispHelpCard 0x00a206bc,
 * 0x00a207f4, 0x00a2080c.
 */
void CardDispHelpSlide2(s8 page, Matrix4 matrix)
{
    static s16 list19[3] = { 0, 84, 120 };
    static s16 list20[3] = { 0, 84, 120 };
    static s16 list21[3] = { 0, 84, 120 };
    static s16 list22[6] = { 0, 39, 67, 3, 49, 29 };
    static s16 list25[6] = { 0, 84, 120 };
    static s16 list26[6] = { 0, 3, 18, 33, 25, 66 };
    static s16 list27[6] = { 0, 3, 18, 33, 43, 66 };
    static s16 list28[4] = { 2, 0, 40, 41 };
    static s16 list45[6] = { 49, 50, 51, 42, 43, 44 };
    static s16 list46[6] = { 49, 50, 51, 42, 43, 44 };
    static s16 list47[4] = { 67, 69, 74, 75 };
    Matrix4 transformed;
    CardHelpVector translation = { { 1.397f, 0.179f, 0.0f, 1.0f } };
    float scale[4];
    int i;

    scale[0] = 0.9f;
    scale[1] = 0.9f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    switch (page) {
    case CARD_HELP_PAGE_DETAILS:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list19[i], 1, 0);
        break;
    case CARD_HELP_PAGE_TYPES:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list20[i], 1, 0);
        break;
    case CARD_HELP_PAGE_NAMES:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list21[i], 1, 0);
        break;
    case CARD_HELP_PAGE_ATTRIBUTES:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDispLight(transformed, list22[i], 1, 0x10);
        break;
    case CARD_HELP_PAGE_COST:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDispLight(transformed, list25[i], 1, 0x8);
        break;
    case CARD_HELP_PAGE_HP:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDispLight(transformed, list26[i], 1, 0x2);
        break;
    case CARD_HELP_PAGE_ATK:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDispLight(transformed, list27[i], 1, 0x4);
        break;
    case CARD_HELP_PAGE_ATTACK_TYPE:
        ChangeCnt %= HELP_SLIDE_CYCLE_4;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDispLight(transformed, list28[i], 1, 0x40);
        break;
    case CARD_HELP_PAGE_WEAPONS_A:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list45[i], 1, 0);
        break;
    case CARD_HELP_PAGE_WEAPONS_B:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list46[i], 1, 0);
        break;
    case CARD_HELP_PAGE_GNOSIS:
        ChangeCnt %= HELP_SLIDE_CYCLE_4;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list47[i], 1, 0);
        break;
    }
}

/*
 * CardDispHelpSlide3 (0x00a1f308, 0x164 bytes): help pages 23 and 24 draw one of the
 * three cards of list23/list24 (both {0, 84, 120}, the card kinds CardDispHelpCurrySet
 * draws) at a fixed place, scale (0.7, 0.7, 1, 1); the entry follows ChangeCnt like
 * CardDispHelpSlide1. The place is an automatic initialised from a constant 16-byte
 * vector; the u64 member of CardHelpVector gives the aggregate the 8-byte alignment
 * that yields the doubleword copy (ld/ld/sd/sd at 0x00a1f330..0x00a1f34c) instead of
 * ldl/ldr pairs. No caller exists in the overlay (the jal word 0x0c287cc2 does not
 * occur): CardDispHelpCard reaches pages 23/24 through CardDispFieldPoint. CardDisp is
 * void: no $v0 read after the calls at 0x00a1f3dc and 0x00a1f454.
 */
void CardDispHelpSlide3(s8 page, Matrix4 matrix)
{
    static s16 list23[3] = { 0, 84, 120 };
    static s16 list24[3] = { 0, 84, 120 };
    Matrix4 transformed;
    CardHelpVector translation = { { 1.733f, 0.671f, 0.0f, 1.0f } };
    float scale[4];
    int i;

    scale[0] = 0.7f;       /* 0x3F333333, lui/ori at 0x00a1f320 */
    scale[1] = 0.7f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    switch (page) {
    case CARD_HELP_PAGE_REQUIREMENTS_A:               /* body at 0x00a1f378 */
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list23[i], 1, 0);
        break;
    case CARD_HELP_PAGE_REQUIREMENTS_B:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation.v);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        CardDisp(transformed, list24[i], 1, 0);
        break;
    }
}

void CardDispCtr(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispCtrTranslation;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_HELP5]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_HELP5]);
}

void CardDispField(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;

    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_FIELD]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_FIELD]);
}

void CardDispDeckMake(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    CardHelpFloat4 scale;

    translation = cardDispDeckMakeTranslation;
    scale = cardDispDeckMakeScale;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   scale.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_DECKMAKE]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_DECKMAKE]);
}

void CardDispH1P2P(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;

    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_H1P2P]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_H1P2P]);
}

void CardDispTurn(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    CardHelpFloat4 scale;

    translation = cardDispTurnTranslation;
    scale = cardDispTurnScale;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   scale.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_HELP6]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_HELP6]);
}

void CardDispFieldPointScale(Matrix4 matrix, const float translation[4],
                              const float scale[4], int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    float light;

    light = (xglSin((((float)ChangeCnt * 11.25f) / 180.0f) * 3.1415927f) * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
    nmlModelSetLight(color, asDir);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

/*
 * CardDispTejyun (0x00a1f780, 0x5f0 bytes): the "tejyun" (procedure) help pages
 * DRAW_PHASE..ADJUST_PHASE (CardDispHelpCard passes the phase resource CARD_HELP_RES_PHASE1..7
 * as `kind`). Every page draws the frame card CARD_HELP_RES_FIELD at
 * cardDispTejyunTranslation scaled by cardDispTejyunScale, then a column of three phase cards starting at
 * kind, placed at cardDispTejyunOffset, each 0.3 lower than the one above it, with
 * the page's own card lit by nmlModelSetLight and the extra cards the page
 * explains drawn by CardDispFieldPointScale. kind indexes a seven-entry jump
 * table (jtbl_00A4E120); an out-of-range kind draws the frame only.
 * The two card tables are read directly, so GCC rematerialises their bases per
 * case, and the three-card tails of the PHASE1 and PHASE2 kinds are identical and get
 * cross-jumped.
 */
void CardDispTejyun(Matrix4 matrix, int kind)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    CardHelpFloat4 offset;
    CardHelpFloat4 scale;
    Matrix4 color;
    float light;

    translation = cardDispTejyunTranslation;
    offset = cardDispTejyunOffset;
    scale = cardDispTejyunScale;
    light = (xglSin((((float)ChangeCnt * 11.25f) / 180.0f) * 3.1415927f) * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   scale.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_FIELD]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_FIELD]);
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   offset.v);

    switch (kind) {
    case CARD_HELP_RES_PHASE1:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_BASE);
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        nmlModelSetLight(color, asDir);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE1]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE1]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE2]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE2]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        break;
    case CARD_HELP_RES_PHASE2:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_BATTL);
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_STANDBY);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE1]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE1]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetLight(color, asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE2]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE2]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        break;
    case CARD_HELP_RES_PHASE3:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE2]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE2]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetLight(color, asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE4]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE4]);
        break;
    case CARD_HELP_RES_PHASE4:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_STANDBY);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetLight(color, asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE4]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE4]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        break;
    case CARD_HELP_RES_PHASE5:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND2);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE4]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE4]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetLight(color, asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE6]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE6]);
        break;
    case CARD_HELP_RES_PHASE6:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_BATTL);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetLight(color, asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE6]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE6]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE7]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE7]);
        break;
    case CARD_HELP_RES_PHASE7:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE6]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE6]);
        transformed[3][1] -= 0.3f;
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE7]);
        nmlModelSetLight(color, asDir);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE7]);
        break;
    }
}

void CardDispFieldPoint(Matrix4 matrix, const float translation[4], int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    float light;

    light = (xglSin((((float)ChangeCnt * 11.25f) / 180.0f) * 3.1415927f) * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    nmlModelSetLight(color, asDir);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

/*
 * CardDispGun (0x00a1fe60, 0xb8 bytes): translation cardDispGunTranslation {1.397, 0.863, 0, 1},
 * scale cardDispGunScale {0.6, 0.6, 1, 1}; CardDisp(kind 0x31, 1, 0); then the model is
 * moved down by 1.33 (double: fptodp 0x00a1fecc, ld the double literal at 0x00a4e160, dpsub, dptofp,
 * stored back at sp+0x34 = transformed[3][1]) and CardDisp(kind 0x44, 1, 0).
 * Caller: CardDispHelpCard 0x00a207e0. CardDisp (0x00a0a2f0) is void (no $v0 read
 * at 0x00a1fec8/0x00a1ff08).
 */
void CardDispGun(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    CardHelpFloat4 scale;

    translation = cardDispGunTranslation;
    scale = cardDispGunScale;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   scale.v);
    CardDisp(transformed, 49, 1, 0);      /* 0x00a1feb8 addiu a1,0x31 */
    transformed[3][1] -= 1.33;            /* double literal at 0x00a4e160 */
    CardDisp(transformed, 68, 1, 0);      /* 0x00a1fef4 addiu a1,0x44 */
}


void CardDispShineCurry(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;
    CardHelpFloat4 scale;

    scale = cardDispShineCurryScale;
    xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
    xglMatrixScale(transformed, (const float (*)[4])transformed,
                   scale.v);
    CardDispLight(transformed, 84, 1, 255);
}

void CardDispAttr(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    const float step = -0.38f;

    translation = cardDispAttrTranslation;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    nmlModelSetPlace((const Vector4 *)transformed);

    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C87400);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C87C00);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88000);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C87800);
    transformed[3][1] += step;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88800);
    transformed[3][1] += -0.57f;

    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88400);
}

void CardDispNazo(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispNazoTranslation;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C89000);
}

void CardDispGuno(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispGunoTranslation;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture((const char *)0x01C91000);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(0x01C88000);
}

/*
 * CardDispOnce (0x00a201d0, 0x94 bytes): draws one card model at a fixed
 * offset (cardDispOnceTranslation = {1.397, 0.179, 0, 1}) with scale 0.9/0.9/1/1 and lights
 * it through CardDispLight(card id, enabled = 1, colour 32).
 * Callers: CardDispHelpCard 0x00a206dc (card 7) and 0x00a206fc (card 0x1D).
 * `card` is a plain int: the sll at 0x00a20210 and sra at 0x00a20240 widen it to
 * the s16 `card` parameter of CardDispLight, so the callers pass a plain int.
 */
void CardDispOnce(Matrix4 matrix, int card)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    float scale[4];

    translation = cardDispOnceTranslation;      /* ld 0x00a201f8/0x00a20200, sd 0x00a20214/0x00a20218 */
    scale[0] = 0.9f;               /* 0x3F666666 */
    scale[1] = 0.9f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
    CardDispLight(transformed, card, 1, 32);   /* 0x00a2024c: a2 = 1, a3 = 0x20 */
}


void CardDispComm(Matrix4 matrix, int kind)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispCommTranslation;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

void CardDispPhase(Matrix4 matrix, int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    CardHelpFloat4 translation;
    float light;

    translation = cardDispPhaseTranslation;
    light = (xglSin((((float)ChangeCnt * 11.25f) / 180.0f) * 3.1415927f) * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, (const float (*)[4])matrix,
                   translation.v);
    nmlModelSetLight(color, asDir);
    nmlModelSetPlace((const Vector4 *)transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

const CardHelpFloat4 cardDispHelpCardTranslationA = {{ 1.397f, 0.215f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispHelpCardTranslationB = {{ 1.397f, -0.188f, 0.0f, 1.0f }};

void CardDispTejyun(Matrix4 matrix, int kind);

#define CARD_HELP_CARD_REQUIREMENTS_A    7     /* card shown on the first REQUIREMENTS page: li a1,7 at 0x00a206e0 */
#define CARD_HELP_CARD_REQUIREMENTS_B    29    /* card shown on the second REQUIREMENTS page: li a1,0x1D at 0x00a20700 */

/*
 * Help-card page drawer: `page` selects one of 59 page ids; `matrix` is the
 * page's world matrix. Each arm is the original call sequence for that page id,
 * grouped by the callee names; the page ids are the CardHelpPage values, i.e. the
 * index of the original jump table at 0x00a4e200 (59 entries); ids 48, 57 and out-of-range
 * values fall through to the common exit. The two vectors are copied to the
 * stack before the switch: pair[0] at sp+0, pair[1] at sp+16.
 */
void CardDispHelpCard(s8 page, Matrix4 matrix)
{
    CardHelpFloat4 pair[2];

    ChangeCnt++;
    pair[0] = cardDispHelpCardTranslationA;
    pair[1] = cardDispHelpCardTranslationB;
    switch (page) {
    case CARD_HELP_PAGE_WHAT_IS_XENO_CARD:
        CardDispComm(matrix, CARD_HELP_RES_TITTLE);
        break;
    case CARD_HELP_PAGE_CONTROLS:
        CardDispCtr(matrix);
        break;
    case CARD_HELP_PAGE_GAME_PROGRESSION:
        CardDispComm(matrix, CARD_HELP_RES_PH1);
        break;
    case CARD_HELP_PAGE_PROGRESSION_PHASE:
        CardDispComm(matrix, CARD_HELP_RES_PH1);
        CardDispPhase(matrix, CARD_HELP_RES_PH2);
        break;
    case CARD_HELP_PAGE_POSITIONING:
        CardDispField(matrix, pair[0].v);
        CardDispH1P2P(matrix, pair[0].v);
        break;
    case CARD_HELP_PAGE_DECK:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_BASE);
        break;
    case CARD_HELP_PAGE_LOST_PILE:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_LOST);
        break;
    case CARD_HELP_PAGE_HAND:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_HAND);
        break;
    case CARD_HELP_PAGE_JUNK_PILE:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_JUNK);
        break;
    case CARD_HELP_PAGE_STANDBY_AREA:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_STANDBY);
        break;
    case CARD_HELP_PAGE_SITUATION_AREA:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_SITUATION);
        break;
    case CARD_HELP_PAGE_BATTLEFIELD:
        CardDispField(matrix, pair[0].v);
        CardDispFieldPoint(matrix, pair[0].v, CARD_HELP_RES_BATTL);
        break;
    case CARD_HELP_PAGE_FIELD:
        CardDispField(matrix, pair[0].v);
        break;
    case CARD_HELP_PAGE_CARD_OUTLINE:
    case CARD_HELP_PAGE_CARD:
        CardDispHelpCurrySet(matrix);
        break;
    case CARD_HELP_PAGE_BATTLE_CARD:
        CardDispHelpSlide1(page, matrix);
        CardDispField(matrix, pair[1].v);
        CardDispFieldPoint(matrix, pair[1].v, CARD_HELP_RES_BATTL);
        CardDispFieldPoint(matrix, pair[1].v, CARD_HELP_RES_STANDBY);
        break;
    case CARD_HELP_PAGE_EVENT_CARD:
        CardDispHelpSlide1(page, matrix);
        CardDispField(matrix, pair[1].v);
        CardDispShineCurry(matrix, pair[1].v);
        break;
    case CARD_HELP_PAGE_SITUATION_CARD:
        CardDispHelpSlide1(page, matrix);
        CardDispField(matrix, pair[1].v);
        CardDispFieldPoint(matrix, pair[1].v, CARD_HELP_RES_SITUATION);
        break;
    case CARD_HELP_PAGE_ATTRIBUTES:
        CardDispHelpSlide2(page, matrix);
        CardDispAttr(matrix);
        break;
    case CARD_HELP_PAGE_REQUIREMENTS_A:
        CardDispOnce(matrix, CARD_HELP_CARD_REQUIREMENTS_A);
        break;
    case CARD_HELP_PAGE_REQUIREMENTS_B:
        CardDispNazo(matrix);
        CardDispOnce(matrix, CARD_HELP_CARD_REQUIREMENTS_B);
        break;
    case CARD_HELP_PAGE_TEXT:
        CardDispComm(matrix, CARD_HELP_RES_TEXT);
        break;
    case CARD_HELP_PAGE_PROGRESSION_TURN:
    case CARD_HELP_PAGE_HOW_TO_PLAY:
    case CARD_HELP_PAGE_TURNS_A:
    case CARD_HELP_PAGE_TURNS_B:
        CardDispTurn(matrix);
        break;
    case CARD_HELP_PAGE_DRAW_PHASE:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE1);
        break;
    case CARD_HELP_PAGE_MOVE_PHASE:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE2);
        break;
    case CARD_HELP_PAGE_EVENT_PHASE_A:
    case CARD_HELP_PAGE_EVENT_PHASE_B:
    case CARD_HELP_PAGE_EVENT_PHASE_C:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE3);
        break;
    case CARD_HELP_PAGE_SET_PHASE_A:
    case CARD_HELP_PAGE_SET_PHASE_B:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE4);
        break;
    case CARD_HELP_PAGE_BLOCK_PHASE:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE5);
        break;
    case CARD_HELP_PAGE_BATTLE_PHASE_A:
    case CARD_HELP_PAGE_BATTLE_PHASE_B:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE6);
        break;
    case CARD_HELP_PAGE_ADJUST_PHASE:
        CardDispTejyun(matrix, CARD_HELP_RES_PHASE7);
        break;
    case CARD_HELP_PAGE_SPECIAL_CARD:
        CardDispGun(matrix);
        break;
    case CARD_HELP_PAGE_DETAILS:
    case CARD_HELP_PAGE_TYPES:
    case CARD_HELP_PAGE_NAMES:
    case CARD_HELP_PAGE_COST:
    case CARD_HELP_PAGE_HP:
    case CARD_HELP_PAGE_ATK:
    case CARD_HELP_PAGE_ATTACK_TYPE:
    case CARD_HELP_PAGE_WEAPONS_A:
    case CARD_HELP_PAGE_WEAPONS_B:
        CardDispHelpSlide2(page, matrix);
        break;
    case CARD_HELP_PAGE_GNOSIS:
        CardDispHelpSlide2(page, matrix);
        CardDispGuno(matrix);
        break;
    case CARD_HELP_PAGE_PENETRATE:
        CardDispComm(matrix, CARD_HELP_RES_BATTLE2);
        break;
    case CARD_HELP_PAGE_DOWN:
        CardDispComm(matrix, CARD_HELP_RES_DOWN);
        break;
    case CARD_HELP_PAGE_E_MARK:
        CardDispComm(matrix, CARD_HELP_RES_EMARK);
        break;
    case CARD_HELP_PAGE_EVENT_BLOCK:
        CardDispComm(matrix, CARD_HELP_RES_EBLAKE);
        break;
    case CARD_HELP_PAGE_OPENING_CARDS:
        CardDispComm(matrix, CARD_HELP_RES_OPEN);
        break;
    case CARD_HELP_PAGE_VIEWING_CARDS:
        CardDispComm(matrix, CARD_HELP_RES_VIEWER);
        break;
    case CARD_HELP_PAGE_CREATING_DECKS:
        CardDispComm(matrix, CARD_HELP_RES_DECKMAKE);
        break;
    case CARD_HELP_PAGE_SAVING_DECKS:
        CardDispComm(matrix, CARD_HELP_RES_DECKSAVE);
        break;
    case CARD_HELP_PAGE_SELECTING_DECKS:
        CardDispComm(matrix, CARD_HELP_RES_DECKLIST);
        break;
    default:  /* 48, 57 and out-of-range ids: nothing to draw */
        break;
    }
}

/* Help-page data and callees used by CardHelpProc. */

/*
 * CardHelpProc states: the high nibble of CardGameWork.helpState selects the screen
 * (`andi $4,$2,0xF0` at 0x00a209ac); every other value shows a help page.
 */
#define CARD_HELP_STATE_MASK    0xF0    /* 0x00a209ac */
#define CARD_HELP_STATE_OFF     0x00    /* sb $0 at 0x00a20e34: back out of the first page, help ends */
#define CARD_HELP_STATE_PAGE    0x01    /* sb 1 at 0x00a20a20, 0x00a20c60: show helpTopic */
#define CARD_HELP_STATE_INDEX   0x10    /* sb at 0x00a209f4, 0x00a20b88, 0x00a20e0c: category list ("Help Index") */
#define CARD_HELP_STATE_TOPICS  0x20    /* sb at 0x00a20abc, case at 0x00a209a8: topic list of one category */
#define CARD_HELP_STATE_OPEN    0x30    /* case at 0x00a209bc: reset both lists, then run the INDEX screen */

/* Pad bits CardHelpProc tests; input[].pressed unless noted. */
#define CARD_HELP_PAD_NEXT            0x0008    /* andi 0x8 at 0x00a209c8: next page (CardTutorialProc's NEXT bit) */
#define CARD_HELP_PAD_PREV            0x0004    /* andi 0x4 at 0x00a20d80: previous page (CardTutorialProc's PREV bit) */
#define CARD_HELP_PAD_BACK            0x0040    /* andi 0x40 at 0x00a20a08, 0x00a20e10: cancel (sound 2), leave the screen / page */
#define CARD_HELP_PAD_BACK_OR_LEFT    0x8040    /* andi 0x8040 at 0x00a20b70: cancel the topic list (sound 2) */
#define CARD_HELP_PAD_SELECT          0x0020    /* andi 0x20 at 0x00a20bd0: open the chosen topic (sound 1) */
#define CARD_HELP_PAD_SELECT_OR_RIGHT 0x2020    /* andi 0x2020 at 0x00a20a5c: open the chosen category's topics (sound 1) */
#define CARD_HELP_PAD_OPEN_INDEX      0x0080    /* andi 0x80 at 0x00a20d88: open the help index (sound 1) */
#define CARD_HELP_DIR_UP              0x1000    /* input[].directions, andi at 0x00a20a24, 0x00a20b8c: previous row */
#define CARD_HELP_DIR_DOWN            0x4000    /* input[].directions, andi at 0x00a20a2c, 0x00a20bc8: next row */

/* Sound effect ids (first argument of xglSoundEffectNormalID; the second is 0). */
#define CARD_HELP_SE_SELECT     1       /* li a0,1 at 0x00a20aac, 0x00a20c18, 0x00a20df8 */
#define CARD_HELP_SE_CANCEL     2       /* li a0,2 at 0x00a20a10, 0x00a20b78, 0x00a20e18 */
#define CARD_HELP_SE_CURSOR     3       /* li a0,3 at 0x00a20a3c, 0x00a20bf0, 0x00a20d4c */

#define CARD_HELP_CATEGORY_LAST     6       /* slti 6 at 0x00a20a68: seven categories (TitleMaxList[7]) */
#define CARD_HELP_TOPICS_PER_CATEGORY 14    /* SkipPageLst row stride, x14 at 0x00a20c3c..0x00a20c48 (7 rows, 0x62 bytes) */
#define CARD_HELP_HISTORY_MAX       100     /* sltiu 0x64 at 0x00a20c4c, 0x00a20db4: bound of activeHelpPage */
#define CARD_HELP_RADIX             10      /* page-number digits: addiu 0xA at 0x00a20e90 */

static CHPTitleList cld0_17;
static CHPTitleList cld1_18;
extern u8 SkipPageLst[];
extern s8 SkipStartList[];
extern s8 PageStatusLst[];
extern char *CardSkipTitleTxtLst[];
extern char *CardBookMarkTxtLst[];
extern char *CardTitleTxtLst[];
extern char *CardHelpTxtLst[];
extern void CardStrListDisp(CHPTitleList *list, char **strings, int index);
extern void CardDispTxtSub(int x, int y, char *text, int color);
extern void CardDispTxtSub2(u16 x, u16 y, const char *text);
extern void xglMatrixUnit(float matrix[4][4]);

void CardHelpProc(CardGameWork *work)
{
    Matrix4 matrix;
    Matrix4 transformed;
    float translation[4];
    float scale[4];
    u16 pressed;
    u16 directions;
    int state;
    int cardModel;
    int digit;

    xglMatrixUnit(matrix);
    scale[0] = scale[1] = 0.97f;
    scale[2] = scale[3] = 1.0f;
    translation[0] = translation[1] = translation[2] = 0.0f;
    translation[3] = 1.0f;
    xglMatrixScale(matrix, (const float (*)[4])matrix, scale);

    pressed = work->command.input[0].pressed;
    directions = work->command.input[0].directions;
    if (work->menuPlayer != 0) {
        pressed = work->command.input[1].pressed;
        directions = work->command.input[1].directions;
    }

    state = work->helpState & CARD_HELP_STATE_MASK;
    switch (state) {
    case CARD_HELP_STATE_OPEN:
        CHPMenuInitSub(&cld0_17, &cld1_18);
        work->helpState = CARD_HELP_STATE_INDEX;
        /* falls through into the INDEX body */
    case CARD_HELP_STATE_INDEX: {
        if (pressed & CARD_HELP_PAD_BACK) {
            xglSoundEffectNormalID(CARD_HELP_SE_CANCEL, 0);
            work->helpState = CARD_HELP_STATE_PAGE;
        }
        if (directions & CARD_HELP_DIR_UP) {
            if (cld0_17.flagC != 0) {
                xglSoundEffectNormalID(CARD_HELP_SE_CURSOR, 0);
                cld0_17.flagC--;
                CHPTitleInitSub(&cld1_18, cld0_17.flagC);
            }
        } else if (directions & CARD_HELP_DIR_DOWN) {
            if (cld0_17.flagC < CARD_HELP_CATEGORY_LAST) {
                xglSoundEffectNormalID(CARD_HELP_SE_CURSOR, 0);
                cld0_17.flagC++;
                CHPTitleInitSub(&cld1_18, cld0_17.flagC);
            }
        } else if (pressed & CARD_HELP_PAD_SELECT_OR_RIGHT) {
            xglSoundEffectNormalID(CARD_HELP_SE_SELECT, 0);
            work->helpState = CARD_HELP_STATE_TOPICS;
        }
        xglFontPrint(0, 0, 0, "\031\003");
        xglFontPrint(6, 0x19, 0xFFF0, "\r");
        xglFontPrint(6, 0x19, 0xFFF0, "Help Index");
        xglFontPrint(0x10A, 0x19, 0xFFF0, "Topics");
        CardStrListDisp(&cld0_17, CardBookMarkTxtLst, cld0_17.flagC);
        CardStrListDisp(&cld1_18, &CardSkipTitleTxtLst[SkipStartList[cld0_17.flagC]], -1);
        CHPMenuBackDispSub(matrix);
    } break;
    case CARD_HELP_STATE_TOPICS: {
        if (pressed & CARD_HELP_PAD_BACK_OR_LEFT) {
            xglSoundEffectNormalID(CARD_HELP_SE_CANCEL, 0);
            work->helpState = CARD_HELP_STATE_INDEX;
        }
        if (directions & CARD_HELP_DIR_UP) {
            if (cld1_18.flagC != 0) {
                xglSoundEffectNormalID(CARD_HELP_SE_CURSOR, 0);
                cld1_18.flagC--;
            }
        } else if (directions & CARD_HELP_DIR_DOWN) {
            if (cld1_18.flagC < cld1_18.count - 1) {
                xglSoundEffectNormalID(CARD_HELP_SE_CURSOR, 0);
                cld1_18.flagC++;
            }
        } else if (pressed & CARD_HELP_PAD_SELECT) {
            xglSoundEffectNormalID(CARD_HELP_SE_SELECT, 0);
            work->helpTopic = SkipPageLst[cld1_18.flagC + cld0_17.flagC * CARD_HELP_TOPICS_PER_CATEGORY];
            work->helpState = CARD_HELP_STATE_PAGE;
            if (work->activeHelpPage < CARD_HELP_HISTORY_MAX) {
                work->activeHelpPage++;
            }
            work->helpPage[work->activeHelpPage] = work->helpTopic;
        }
        CardStrListDisp(&cld0_17, CardBookMarkTxtLst, cld0_17.flagC);
        CardStrListDisp(&cld1_18, &CardSkipTitleTxtLst[SkipStartList[cld0_17.flagC]], cld1_18.flagC);
        xglFontPrint(0, 0, 0, "\031\003");
        xglFontPrint(6, 0x19, 0xFFF0, "\r");
        xglFontPrint(6, 0x19, 0xFFF0, "Help Index");
        xglFontPrint(0x10A, 0x19, 0xFFF0, "Topics");
        CHPMenuBackDispSub(matrix);
    } break;
    default: {
        if (pressed & CARD_HELP_PAD_NEXT) {
            xglSoundEffectNormalID(CARD_HELP_SE_CURSOR, 0);
            work->helpTopic++;
            if ((s8)work->helpTopic >= CARD_HELP_PAGE_COUNT) {
                work->helpTopic = 0;
            }
            if (work->activeHelpPage < CARD_HELP_HISTORY_MAX) {
                work->activeHelpPage++;
            }
            work->helpPage[work->activeHelpPage] = work->helpTopic;
        } else if (pressed & CARD_HELP_PAD_PREV) {
            xglSoundEffectNormalID(CARD_HELP_SE_CURSOR, 0);
            work->helpTopic = (s8)work->helpTopic - 1;
            if ((s8)work->helpTopic < 0) {
                work->helpTopic = CARD_HELP_PAGE_LAST;
            }
            if (work->activeHelpPage < CARD_HELP_HISTORY_MAX) {
                work->activeHelpPage++;
            }
            work->helpPage[work->activeHelpPage] = work->helpTopic;
        } else if (pressed & CARD_HELP_PAD_OPEN_INDEX) {
            CHPMenuInitSub(&cld0_17, &cld1_18);
            xglSoundEffectNormalID(CARD_HELP_SE_SELECT, 0);
            work->helpState = CARD_HELP_STATE_INDEX;
        } else if (pressed & CARD_HELP_PAD_BACK) {
            xglSoundEffectNormalID(CARD_HELP_SE_CANCEL, 0);
            if (work->activeHelpPage == 0) {
                work->helpState = CARD_HELP_STATE_OFF;
            } else {
                work->activeHelpPage--;
                work->helpTopic = work->helpPage[work->activeHelpPage];
            }
        }

        xglFontPrint(6, 0x19, 0xFFF0, "");
        CardDispTxtSub(6, 0x19, CardTitleTxtLst[(s8)work->helpTopic], 0xC);
        CardDispTxtSub2(6, 0x32, CardHelpTxtLst[(s8)work->helpTopic]);
        CardDispTxtSub(0x11E, 0x19, CardBookMarkTxtLst[PageStatusLst[3 * (s8)work->helpTopic]], 0xC);
        CardDispHelpCard((s8)work->helpTopic, matrix);

        translation[1] = -1.496f;
        translation[2] = 0.0f;
        translation[3] = 1.0f;
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        translation[0] = 1.0064001f;
        cardModel = 0x01C8A800 + (PageStatusLst[3 * (s8)work->helpTopic] << 10);
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture((const char *)0x01C990A0);
        nmlModelSetRenderLevel(2);
        nmlModelEntryCard((void *)cardModel);

        digit = (PageStatusLst[3 * (s8)work->helpTopic + 1] + 1) / CARD_HELP_RADIX;
        if (digit != 0) {
            translation[0] = 1.3564f;
            cardModel = 0x01C8A400 + (digit << 10);
            xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
            xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
            nmlModelSetPlace((const Vector4 *)transformed);
            nmlModelSetTexture((const char *)0x01C990A0);
            nmlModelSetRenderLevel(2);
            nmlModelEntryCard((void *)cardModel);
        }

        cardModel = 0x01C8A400 + (((PageStatusLst[3 * (s8)work->helpTopic + 1] + 1) % CARD_HELP_RADIX) << 10);
        translation[0] = 1.4824001f;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture((const char *)0x01C990A0);
        nmlModelSetRenderLevel(2);
        nmlModelEntryCard((void *)cardModel);

        digit = ((s8)work->helpTopic + 1) / CARD_HELP_RADIX;
        if (digit != 0) {
            translation[0] = 1.8304f;
            cardModel = 0x01C8A400 + (digit << 10);
            xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
            xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
            nmlModelSetPlace((const Vector4 *)transformed);
            nmlModelSetTexture((const char *)0x01C990A0);
            nmlModelSetRenderLevel(2);
            nmlModelEntryCard((void *)cardModel);
        }

        cardModel = 0x01C8A400 + ((((s8)work->helpTopic + 1) % CARD_HELP_RADIX) << 10);
        translation[0] = 1.9564f;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
        xglMatrixScale(transformed, (const float (*)[4])transformed, scale);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetTexture((const char *)0x01C990A0);
        nmlModelSetRenderLevel(2);
        nmlModelEntryCard((void *)cardModel);

        translation[0] = -1.4184f;
        translation[1] = translation[2] = 0.0f;
        translation[3] = 1.0f;
        cardModel = 0x01CA1000;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
        nmlModelSetTexture((const char *)0x01CB0000);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetRenderLevel(2);
        nmlModelEntryCard((void *)cardModel);

        translation[0] = 1.4184f;
        translation[1] = 1.832f;
        cardModel = 0x01CA1C00;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
        nmlModelSetTexture((const char *)0x01CC0050);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetRenderLevel(2);
        nmlModelEntryCard((void *)cardModel);

        translation[0] = 1.4184f;
        translation[1] = -1.832f;
        cardModel = 0x01CA6000;
        xglMatrixTrans(transformed, (const float (*)[4])matrix, translation);
        nmlModelSetTexture((const char *)0x01CC0050);
        nmlModelSetPlace((const Vector4 *)transformed);
        nmlModelSetRenderLevel(2);
        nmlModelEntryCard((void *)cardModel);
    } break;
    }
}

/*
 * CardHelpPageInit: clears the 256-byte help page history helpPage[] (+0x59c0..+0x5abf,
 * stored downwards) and then the history index activeHelpPage (+0x59be).
 */
void CardHelpPageInit(CardGameWork *work)
{
    int i;

    for (i = 255; i >= 0; i--) {
        work->helpPage[i] = 0;
    }
    work->activeHelpPage = 0;
}

static char Help7_01_txt[296];
static char Help2_05_txt[160];
static char Help2_03_txt[288];
static char Help2_08_txt[272];
static char Help4_08_txt[392];
static char Help7_02_txt[120];
static char Help7_03_txt[312];
static char Help2_06_txt[224];
static char Help3_14_txt[360];
static char Help6_02_txt[264];
static char Help3_09_txt[392];
static char Help1_02_txt[312];
static char Help3_01_txt[112];
static char Help2_01_txt[312];
static char Help4_09_txt[288];
static char Help5_04_txt[368];
static char Help6_03_txt[272];
static char Help4_10_txt[384];
static char Help1_03_txt[392];
static char Help7_05_txt[184];
static char Help2_07_txt[144];
static char Help3_06_txt[176];
static char Help4_02_txt[384];
static char Help5_01_txt[176];
static char Help7_06_txt[200];
static char Help3_04_txt[296];
static char Help4_11_txt[232];
static char Help3_08_txt[392];
static char Help4_04_txt[360];
static char Help5_03_txt[416];
static char Help4_13_txt[416];
static char Help3_10_txt[392];
static char Help5_02_txt[376];
static char Help2_04_txt[400];
static char Help3_03_txt[248];
static char Help6_01_txt[80];
static char Help2_09_txt[184];
static char Help1_06_txt[152];
static char Help4_01_txt[304];
static char Help4_03_txt[248];
static char Help6_05_txt[336];
static char Help7_04_txt[320];
static char Help4_06_txt[368];
static char Help2_02_txt[344];
static char Help4_05_txt[408];
static char Help1_04_txt[216];
static char Help1_01_txt[288];
static char Help4_12_txt[336];
static char Help3_12_txt[304];
static char Help4_14_txt[272];
static char Help3_11_txt[248];
static char Help3_05_txt[200];
static char Help3_15_txt[232];
static char Help4_07_txt[336];
static char Help3_02_txt[336];
static char Help6_04_txt[320];
static char Help3_07_txt[328];
static char Help1_05_txt[336];
static char Help3_13_txt[192];
static char Title3_07_txt[8];
static char Title4_11_txt[16];
static char Title3_04_txt[16];
static char Title2_01_txt[24];
static char Title2_03_txt[16];
static char Title7_04_txt[16];
static char Title7_01_txt[16];
static char Title4_12_txt[16];
static char Title1_03_txt[16];
static char Title7_02_txt[16];
static char Title7_05_txt[16];
static char Title1_06_txt[24];
static char Title2_07_txt[16];
static char Title3_08_txt[16];
static char Title2_02_txt[8];
static char Title4_13_txt[16];
static char Title3_15_txt[8];
static char Title4_07_txt[16];
static char Title6_04_txt[8];
static char Title6_05_txt[16];
static char Title3_11_txt[8];
static char Title3_06_txt[8];
static char Title4_10_txt[16];
static char Title5_01_txt[16];
static char Title3_10_txt[16];
static char Title5_03_txt[24];
static char Title6_03_txt[8];
static char Title4_04_txt[16];
static char Title2_05_txt[16];
static char Title5_02_txt[24];
static char Title4_03_txt[16];
static char Title2_08_txt[16];
static char Title4_14_txt[16];
static char Title1_01_txt[24];
static char Title4_06_txt[16];
static char Title1_05_txt[24];
static char Title4_05_txt[16];
static char Title4_08_txt[16];
static char Title6_02_txt[16];
static char Title5_04_txt[24];
static char Title3_02_txt[16];
static char Title7_03_txt[16];
static char Title7_06_txt[16];
static char Title3_03_txt[16];
static char Title2_06_txt[16];
static char Title3_01_txt[8];
static char Title1_04_txt[24];
static char Title3_05_txt[8];
static char Title3_12_txt[8];
static char Title3_13_txt[8];
static char Title1_02_txt[16];
static char Title3_14_txt[16];
static char Title2_09_txt[8];
static char Title4_01_txt[16];
static char Title4_09_txt[16];
static char Title6_01_txt[16];
static char Title3_09_txt[16];
static char Title4_02_txt[16];
static char Title2_04_txt[8];
static char BookMark3_txt[8];
static char BookMark5_txt[16];
static char BookMark1_txt[24];
static char BookMark7_txt[8];
static char BookMark6_txt[16];
static char BookMark4_txt[16];
static char BookMark2_txt[24];

static char Help1_01_txt[288] = "This is a trading card game\nbased on the Xenosaga world.\n\nThere are more than 140 cards\nfrom which to create your\nown 40-card set (referred to\nas a \"deck\"), and you and an\nopponent will use these decks\nto battle.\n\nBe sure to play this card game\nto explore the Xenosaga world\nfurther!";

static char Help1_02_txt[312] = "\014\310\010\010\241\373 \013\031\003\r\000Button: Select, hold card,\n                  place card, use\n                  card's power.\n\014(\010\377\241\337 \013\031\003\r\000Button: Cancel.\n\014@\377@\242\244 \013\031\003\r\000Button: View details of the\n                  selected card.\n\014\300(h\242\242 \013\031\003\r\000Button: End phase.\n\n\014XX\300START \013\031\003\r\000Button: Open menu.\n\014XX\300SELECT \013\031\003\r\000Button: View help.";

static char Help1_03_txt[392] = "The cards are divided up into\nthree major groups.\n\n\014XX\300Battle card\013\031\003\r\000: This card is used in\n                   the actual battles.\n\014XX\300Event card\013\031\003\r\000: Recover your cards\n                   or interfere with\n                   your opponent.\n\014XX\300Situation card\013\031\003\r\000: Same as the\n                   event card, except\n                   the effect stays\n                   on the field.";

static char Help1_04_txt[216] = "The game progresses in\nphases and turns.\n\nMoreover, the type of card\ndictates where the card can\nbe used. The first player to\nrun out of cards in their\ndeck loses.\n\n\n\n\n\n\n\n\n\n                                   1/3";

static char Help1_05_txt[336] = "Xeno Card is played in terms\nof phases.\n\nA phase refers to a step in\nthe game, such as draw a card,\nmove a card, play a card, and\nbattle.\n\nThe current phase is displayed\nduring the game. Blue-colored\nphases are for Player 1\n(P1 phase), and red-colored\nphases are for Player 2\n(P2 phase).\n\n\n\n                                   2/3";

static char Help1_06_txt[152] = "A turn refers to one full cycle\nof phases.\n\nYour turn and the opponent's\nturn make up one turn.\n\n\n\n\n\n\n\n\n\n\n\n\n                                   3/3";

static char Help2_01_txt[312] = "There are many places where a\ncard can be placed on the play\nmat, but each card is required\nto be placed in a specific\narea.\n\nBoth sides of the play mat are\nidentical.\n\nFrom here on, directions will be\ngiven based on the P1 side. The\ngame progresses by placing\ncards onto the field according\nto each phase.";

static char Help2_02_txt[344] = "The deck is shuffled and\nplaced faced down where it\nsays DECK.\n\nNormally, one card is drawn\nfrom the deck per turn. If the\ndeck takes a direct hit, the\nsame number of cards as the\ndamage points must be taken\nfrom the deck and moved to\nthe lost pile.\n\nThe object of the game is to\nmake the opponent use up all\nof the cards in their deck.";

static char Help2_03_txt[288] = "The discard area is marked as\nLOST. Cards are discarded\nfrom the deck as a result of\nattacks by opponents or to\npay the cost of an action.\nThose cards are placed face\ndown in this area.\n\nWhen using a card's effect\nto recover the deck, cards\nfrom the lost pile are returned\nto the deck.";

static char Help2_04_txt[400] = "The player's cards in hand are\nplaced where it says HAND.\nThe hand starts with six cards.\nCards are played from this\narea. Normally, one card is\ndrawn from the deck and\nadded to the hand every turn.\nThen, the game is played with\nthe cards in hand.\n\nAt the end of a turn, the\nplayer can have a maximum\nof six cards in hand.\n\nIf a player has more than six\ncards, they must discard the\nextra cards.";

static char Help2_05_txt[160] = "The discarded cards are\nplaced where it says JUNK.\n\nCards destroyed by the\nopponent, used event cards,\nand extra cards in hand are\nplaced face up in this area.";

static char Help2_06_txt[224] = "The standby area is where the\nbattle cards are first moved\nto from the hand.\n\nA regular battle card can only\nbe placed in the standby area.\n\nYou can only put up to four\ncards in this area, so choose\nyour placement wisely.";

static char Help2_07_txt[144] = "The situation area is where\nthe situation cards are placed.\n\nYou can only put up to four\ncards in this area, so choose\nyour placement wisely.";

static char Help2_08_txt[272] = "The battlefield is where the\nbattle cards go into action.\nCards that are placed here will\nautomatically go into battle.\n\nStatus information of the\nbattle cards such as HP and\nATK, as well as counters for\ncards that attack once every\nother turn, are displayed here.";

static char Help2_09_txt[184] = "Except for the hand, junk pile,\ndeck, and lost area, the\nentire play mat is called the\nfield. Placing a card down from\nthe hand is referred to as,\n\"putting the card on the field.\"";

static char Help3_01_txt[112] = "There are three major types of\ncards. Each type of card is\nused in different ways.\n\nLet's look at the details.";

static char Help3_02_txt[336] = "The battle cards are the cards\nused in the actual battles.\n\nThe number of battle cards on\nthe field affect what cards\ncan be used.\n\n\014\377@@(See: Card: Requirements)\n\n\013\031\003\r\000They are used by placing them\non the standby area and the\nbattlefield. You are able to\nmove these cards around.\n\nXeno Card cannot be played\nwithout the battle cards.";

static char Help3_03_txt[248] = "The event card is a vital card\nthat lets you do various things\nsuch as recover your cards'\nHP and interfere with your\nopponent.\n\nThere are many different ways\nof using this card. Once this\ncard is used, the card will be\nplaced in the junk pile.";

static char Help3_04_txt[296] = "Like the event card, the\nsituation card has many uses\nsuch as recovering your cards'\nHP and interfering with your\nopponent. When this card is\nused, it is placed on the\nsituation area. While the event\ncard gets moved to the junk\npile immediately after use, the\nsituation card stays on the\nfield.";

static char Help3_05_txt[200] = "Let's go over the information\nwritten on the cards.\n\nAll cards display the following:\n\014XX\300Type\nName\nRequirement\nCost\nText\n\n\013\031\003\r\000In addition, the battle cards\ndisplay:\n\014XX\300Attribute\nHP\nATK\nAttack Type";

static char Help3_06_txt[176] = "This shows the card type.\nThere are three types of cards:\nBattle, event, and situation.\n\n\014\377@@(See: Card: Battle card)\n(See: Card: Event card)\n(See: Card: Situation card)";

static char Help3_07_txt[328] = "This is the name of the card.\nOnly three cards with the\nsame name can be put in the\ndeck.\n\nAlso, when the battle cards\nhave proper nouns such as\nZiggy and Shion, you can only\nplace one of those cards on\nthe field at one time.\n\nIf the card is destroyed or\nmoved off the field, you are\nable to put another card down\nagain.";

static char Help3_08_txt[392] = "Each battle card has an\nattribute, marked by an icon:\n   \241\341\014XX\300\"H\" \013\031\003\r\000Human\n    Shion and other people.\n   \241\341\014XX\300\"M\" \013\031\003\r\000Mech\n    A.G.W.S. and battleships.\n   \241\341\014XX\300\"G\" \013\031\003\r\000Gnosis\n    The Gnosis.\n   \241\341\014XX\300\"R\" \013\031\003\r\000Realian\n    MOMO and other Realians.\n   \241\341\014XX\300\"W\" \013\031\003\r\000Weapon\n    You can use these cards\n    together with other cards.\n   \241\341\014XX\300\"U\" \013\031\003\r\000Unknown\n    Something unknown.";

static char Help3_09_txt[392] = "There are certain conditions\nthat need to be met prior to\nplaying a card.\n\nThose conditions are\nindicated with attribute marks.\nYou need to have cards with\nthe required attributes on your\nown battlefield or standby\narea.\n\nFor example, when putting\ndown a Lv 10 Shion card on the\nfield, three \"H\" cards must be\non the battlefield or the\nstandby area.\n\n                                   1/2";

static char Help3_10_txt[392] = "There are some special\nrequirements. For example,\nthe Bunnie card on the right\nshows an attribute condition\nthat looks like this:\n\nThis means that any attribute\nwill meet the condition. In this\ncase, there must be two\nbattle cards on the field,\nregardless of the attribute.\nSome cards have the\nconditions written in the\ntext, so read carefully.\n\n\n\n                                   2/2";

static char Help3_11_txt[248] = "You need to pay a cost to\nplay your cards on the field.\n\nThe number next to \"cost\"\nindicates the number of cards\nto be moved from the deck to\nthe lost pile. You need to be\ncareful here, because if you\nrun out of cards in the deck,\nyou lose.";

static char Help3_12_txt[304] = "The strength of the battle\ncards is shown next to \"HP.\"\n\nThe number decreases after\neach attack. Once it runs out,\nthe card is destroyed and\nmoved to the junk pile.\n\nIt is a good idea to maintain\nthe cards' HP by using cards\nthat recover HP. The HP number\ndisplayed on the card is the\nmaximum value.";

static char Help3_13_txt[192] = "ATK denotes attack power.\n\nThe number next to \"ATK\" is\nthe damage point the card can\ninflict.\n\nDamages decrease the\nopponent's battle card HP or\nthe number of cards in the\nopponent's deck.";

static char Help3_14_txt[360] = "The battle cards come with\ndifferent types of attack.\n\nThere are four types:\n\n\014XX\300Hand\n\013\031\003\r\000Attack enemy directly in front.\n\014XX\300Ballistic\n\013\031\003\r\000Attack enemy directly in the\nline of fire. If no enemies are\nin the line of fire, attack\nopponent's deck.\n\014XX\300Spread\n\013\031\003\r\000Inflict damage over opponent's\nentire battlefield.\n\014XX\300Homing\n\013\031\003\r\000Attack the deck directly.";

static char Help3_15_txt[232] = "Each card comes with some\ntext. You can view this text\nanytime during the game by\npressing the \014\010@\010\242\244 \013\031\003\r\000Button.\n\nThe text describes the special\npowers of each card.\n\nThe effects of the text have\npriority over the game rules.";

static char Help4_01_txt[304] = "This game progresses in\nphases and turns.\n\nIn the beginning, you and your\nopponent's decks are shuffled\nand placed face down on the\ndeck.\n\nWhen playing alone, you are\nPLAYER 1. When playing with\ntwo people, the player going\nfirst is \014XX\300PLAYER 1\013\031\003\r\000, and the\nplayer following is \014XX\300PLAYER 2\013\031\003\r\000.";

static char Help4_02_txt[384] = "As shown on the right, seven\nphases make up a turn.\n\nThe first player will see\n\014XX\300\243\261P TURN\013\031\003\r\000, and the second player\nwill see \014XX\300\243\262P TURN\013\031\003\r\000.\n\nSometimes both players' turns\nare combined and referred to\nas a \"turn.\"\n\nPlayers alternately go through\ntheir turns in the game.\n\nThe actions taken in each\nphase are identical for both\nplayers.\n                                   1/2";

static char Help4_03_txt[248] = "A player can differentiate\nbetween the player's own\nturn and the opponent's by\nreferring to them as \"my\nturn\" and \"opponent's turn.\"\n\nPhases can also be called\n\"my phase\" or \"opponent's\nphase.\"\n\n\n\n\n\n\n\n\n                                   2/2";

static char Help4_04_txt[360] = "The draw phase will run\nautomatically. One card is\ndrawn from the deck and\nadded to your hand. If there\nare no cards in the deck after\nthe card is drawn, you lose.\nEach player's draw phases\nare specified as \"my draw\nphase\" and \"opponent's draw\nphase.\"\n\nWhen it is not specified, the\nterm refers to both draw\nphases. The same applies for\nthe other phases.";

static char Help4_05_txt[408] = "In this phase, you can move\nthe battle cards in the\nstandby and battle areas.\nMoving a card to the\nbattlefield is called\ndeployment, and sending a\ncard back to the standby\narea is called withdrawing.\n\nPick a card by using the\nDirectional Buttons and\npressing the \014\310\010\010\241\373 \013\031\003\r\000Button.\n\nUse the Directional Buttons to\nmove the card to the desired\nspot, then press the \014\310\010\010\241\373 \013\031\003\r\000Button\nto finish the move.";

static char Help4_06_txt[368] = "You can use any number of\nevent cards from your hand\nduring this phase. Of course,\nyou can choose not to use\nany cards.\n\nPick an event card from the\nhand by using the Directional\nButtons, and pressing the\n\014\310\010\010\241\373 \013\031\003\r\000Button.\n\nSpecific conditions must be\nmet in order to use the event\ncards.\n\n\014\377@@(See: Card: Condition)\n\n                                   \013\031\003\r\000 1/3";

static char Help4_07_txt[336] = "Also, there are instances when\nthe opponent uses an event\ncard that disables your event\ncard and vice versa.\n\nWhen an event card is disabled,\nit is called \"event block.\"\nIt is also possible to block an\nevent block card.\n\nEvent cards can be blocked as\nlong as there are cards to\nblock them.\n\n\n\n\n                                   2/3";

static char Help4_08_txt[392] = "In addition to using an event\ncard, you can use the special\ncards on the field during the\nevent phase. These are the\ncards with text such as,\n\"Discard this card during the\nevent phase to...\"\n\nPick a special card by using\nthe Directional Buttons, and\npress the \014\310\010\010\241\373 \013\031\003\r\000Button to use\nits special power. Then, follow\nthe directions on the screen.\n\n\n\n\n                                   3/3";

static char Help4_09_txt[288] = "You can put down a battle or\nsituation card in this phase.\nEach card's requirements\nmust be met in order to put\nthe card on the field.\n\n\014\377@@(See: Card: Condition)\n\n\013\031\003\r\000You can only use one situation\ncard and one battle card per\nturn (phase).\n\n\n\n\n\n\n                                   1/2";

static char Help4_10_txt[384] = "To take a card out of your\nhand, choose the card by using\nthe Directional Buttons and\npress the \014\310\010\010\241\373 \013\031\003\r\000Button.\n\nThe battle cards are\nautomatically placed on the\nstandby area, and the\nsituation cards are placed\non the situation area.\n\nThe exceptions are the cards\nwith G and W attributes. You\ncan select where to place\nthese cards.\n\n\n                                   2/2";

static char Help4_11_txt[232] = "In this phase, a player can use\nan event card during the\nopponent's turn. If the\nopponent has an event card\nthat can block that card and\nthe requirements are met,\nthe opponent can block that\nevent.\n\n\014\377@@(See: Card: Condition) \013\031\003\r";

static char Help4_12_txt[336] = "The battle between the battle\ncards run automatically.\n\nThe order of attack depends\non the attack type.\n\nThe order is Hand, Ballistic,\nSpread, and Homing. If the\nattack types are the same,\nthey go in the order displayed\non the battlefield from 1 to 4.\n\n\014\377@@(See: Card: Type of Attack)\n\n\n\n\n                                   \013\031\003\r\000 1/2";

static char Help4_13_txt[416] = "If the HP of a battle card\nreaches 0 after a battle, it is\nconsidered destroyed and is\nplaced in the junk pile.\n\nIf the deck receives damage,\nthe same number of cards as\nthe damage points is taken\nfrom the deck. If there are\nno more cards in the deck,\nyou lose.\n\nThe key to the game is trying\nto keep the deck from being\nattacked and the battle cards\nfrom being destroyed.\n\n                                   2/2";

static char Help4_14_txt[272] = "If there are more than six\ncards in your hand, you must\ndiscard the extra cards.\n\nIf the game lets you draw a\ncard at the end of a turn, the\ncards in hand are adjusted\nafter drawing the card.\n\nThis marks the end of the turn.\nThe opponent's turn will run\nthe same way.";

static char Help5_01_txt[176] = "We have explained how to use\nmost of the cards.\n\n\014\377@@(See: Card)\n(See: Play Order)\n\n\013\031\003\r\000We will now explain how to use\ntwo types of special cards:\nW and G battle cards.";

static char Help5_02_txt[376] = "Unlike other battle cards, the\nweapon card cannot be used\nby itself.\n\nWhen a compatible battle card\nis on the field, you can combine\nit with that battle card\n(equip). You can put the card\non either the standby area or\nthe battlefield.\n\nOnce the weapon is equipped,\nit cannot be taken off unless\nit is replaced by another\nweapon.\n\n\n                                   1/2";

static char Help5_03_txt[416] = "To use the weapon card, pick\nthe card from the hand using\nthe Directional Buttons and\npress the \014\310\010\010\241\373 \013\031\003\r\000Button. Then, pick\nthe cards you want to equip\nfrom the battlefield or standby\narea and press the \014\310\010\010\241\373 \013\031\003\r\000Button.\n\nWhen the battle card is\nequipped with a weapon card,\nthe attack power of the battle\ncard is lost, and only the\nweapon's attack power is used.\n\n\n\n\n                                   2/2";

static char Help5_04_txt[368] = "Unlike other cards,   \nG battle cards (Gnosis) can\nbe placed in areas other than\nthe standby area.\n\nThe drawback is that a\nnumber of battle cards must\nbe discarded in order to set it.\n\nThe number of cards to be\ndiscarded is displayed in the\ntext. Follow the directions on\nthe screen and select the\ncards to be discarded. Then,\nselect the area to place\nthe card.";

static char Help6_01_txt[80] = "We will now explain some\nspecial terms:\n\nPenetrate\n\nDown\n\nE mark\n\nEvent block";

static char Help6_02_txt[264] = "The text on a battle card may\ninclude the word \"penetrate.\"\n\nNormally during a battle, an\nattack cannot go beyond\nthe opponent being hit. If it\nsays, \"penetrate,\" however,\nany extra damage points will\ngo through the opponent to\nthe cards or the deck behind it.";

static char Help6_03_txt[272] = "The text on a battle card may\ninclude the words \"down card\"\nor \"down opponent's card.\"\n\nA card in a downed state will\ndisplay a down mark.\n\nA downed card cannot attack\nand its text effect is\nneutralized. A downed card\nreturns to normal at the end\nof your battle phase.";

static char Help6_04_txt[320] = "A battle card or situation card\nthat was just placed on the\nbattlefield will display an\nE mark.\n\nThe E mark disappears right\nbefore your battle phase.\nThese cards are at risk\nbecause there are attacks\nthat specifically target cards\nwith this mark.\n\nCards with an E mark cannot\nmove, and their text are\nineffective.";

static char Help6_05_txt[336] = "Neutralizing an event card is\ncalled \"event block.\"\n\nThe two cards that can block\nevents are \"Miyuki's Email\"\nand \"Song of the Mysterious\nGirl.\"\n\nIt is also possible to block an\nevent block. When this occurs,\nthe card that was blocked\nfirst will be enabled. Players\ncan keep blocking events as\nlong as these cards are\navailable.";

static char Help7_01_txt[296] = "The cards used in this game\ncan be obtained at certain\nshops.\n\nObtained cards must be\nopened before they can be\nused. Select \"Open Card\"\nfrom the card game menu to\nopen new card packs. \n\nOne thing to keep in mind is\nthat you can only have three\nof each card -- any\nextras will be sold back.";

static char Help7_02_txt[120] = "You can see all of your cards\nby selecting \"View Cards.\"\n\nThere are a total of 148 cards,\nso try to collect them all!";

static char Help7_03_txt[312] = "Creating a good deck is\ncrucial to winning this game.\nThe window on the left shows\nthe cards you possess, and\nthe window on the right\ndisplays the deck being\ncreated.\n\nSelect cards by using the\nDirectional Buttons, and press\nthe \014\310\010\010\241\373 \013\031\003\r\000Button to add cards\nor the \014(\010\377\241\337 \013\031\003\r\000Button to\nsubtract cards.";

static char Help7_04_txt[320] = "You can save up to five decks\ncreated during the game.\nThese decks are not saved on\nthe Memory Card (PS2) during\nthe card game, so the data\nwill be lost if the power is\nturned off or the RESET\nButton is pressed.\n\nTo save the deck information\non the Memory Card (PS2), you\nmust go to a save point and\nsave the game.";

static char Help7_05_txt[184] = "When playing 2-player battle,\nthe second player's moves\ncan be controlled using the 2P\ncontroller.\n\nWhen a player is looking\nat their cards, the other player\nshould try to look away.";

static char Help7_06_txt[200] = "Before starting a card\ngame, you must select a\ndeck.\n\nUse the Directional Buttons\nto choose a deck. A deck is\nchosen from the five decks\nthat are already registered.\n\n\014\377@@(See: Others: Create Deck)";

static char Title1_01_txt[24] = "WHAT IS XENO CARD\?";

static char Title1_02_txt[16] = "CONTROLS";

static char Title1_03_txt[16] = "CARD OUTLINE";

static char Title1_04_txt[24] = "GAME PROGRESSION";

static char Title1_05_txt[24] = "GAME PROGRESSION: PHASE";

static char Title1_06_txt[24] = "GAME PROGRESSION: TURN";

static char Title2_01_txt[24] = "CARD POSITIONING";

static char Title2_02_txt[8] = "DECK";

static char Title2_03_txt[16] = "LOST PILE";

static char Title2_04_txt[8] = "HAND";

static char Title2_05_txt[16] = "JUNK PILE";

static char Title2_06_txt[16] = "STANDBY AREA";

static char Title2_07_txt[16] = "SITUATION AREA";

static char Title2_08_txt[16] = "BATTLEFIELD";

static char Title2_09_txt[8] = "FIELD";

static char Title3_01_txt[8] = "CARD";

static char Title3_02_txt[16] = "BATTLE CARD";

static char Title3_03_txt[16] = "EVENT CARD";

static char Title3_04_txt[16] = "SITUATION CARD";

static char Title3_05_txt[8] = "DETAILS";

static char Title3_06_txt[8] = "TYPES";

static char Title3_07_txt[8] = "NAMES";

static char Title3_08_txt[16] = "ATTRIBUTES";

static char Title3_09_txt[16] = "REQUIREMENTS";

static char Title3_10_txt[16] = "REQUIREMENTS";

static char Title3_11_txt[8] = "COST";

static char Title3_12_txt[8] = "HP";

static char Title3_13_txt[8] = "ATK";

static char Title3_14_txt[16] = "ATTACK TYPE";

static char Title3_15_txt[8] = "TEXT";

static char Title4_01_txt[16] = "HOW TO PLAY";

static char Title4_02_txt[16] = "1P AND 2P TURNS";

static char Title4_03_txt[16] = "1P AND 2P TURNS";

static char Title4_04_txt[16] = "DRAW PHASE";

static char Title4_05_txt[16] = "MOVE PHASE";

static char Title4_06_txt[16] = "EVENT PHASE";

static char Title4_07_txt[16] = "EVENT PHASE";

static char Title4_08_txt[16] = "EVENT PHASE";

static char Title4_09_txt[16] = "SET PHASE";

static char Title4_10_txt[16] = "SET PHASE";

static char Title4_11_txt[16] = "BLOCK PHASE";

static char Title4_12_txt[16] = "BATTLE PHASE";

static char Title4_13_txt[16] = "BATTLE PHASE";

static char Title4_14_txt[16] = "ADJUST PHASE";

static char Title5_01_txt[16] = "SPECIAL CARD";

static char Title5_02_txt[24] = "BATTLE CARD: WEAPONS";

static char Title5_03_txt[24] = "BATTLE CARD: WEAPONS";

static char Title5_04_txt[24] = "BATTLE CARD: GNOSIS";

static char Title6_01_txt[16] = "TERMINOLOGY";

static char Title6_02_txt[16] = "PENETRATE";

static char Title6_03_txt[8] = "DOWN";

static char Title6_04_txt[8] = "E MARK";

static char Title6_05_txt[16] = "EVENT BLOCK";

static char Title7_01_txt[16] = "OPENING CARDS";

static char Title7_02_txt[16] = "VIEWING CARDS";

static char Title7_03_txt[16] = "CREATING DECKS";

static char Title7_04_txt[16] = "SAVING DECKS";

static char Title7_05_txt[16] = "2-PLAYER BATTLE";

static char Title7_06_txt[16] = "SELECTING DECKS";

static char BookMark1_txt[24] = "XENO CARD OUTLINE";

static char BookMark2_txt[24] = "CARD POSITIONING";

static char BookMark3_txt[8] = "CARDS";

static char BookMark4_txt[16] = "HOW TO PLAY";

static char BookMark5_txt[16] = "SPECIAL CARDS";

static char BookMark6_txt[16] = "TERMINOLOGY";

static char BookMark7_txt[8] = "OTHERS";

char * CardHelpTxtLst[59] = { Help1_01_txt, Help1_02_txt, Help1_03_txt, Help1_04_txt, Help1_05_txt, Help1_06_txt, Help2_01_txt, Help2_02_txt, Help2_03_txt, Help2_04_txt, Help2_05_txt, Help2_06_txt, Help2_07_txt, Help2_08_txt, Help2_09_txt, Help3_01_txt, Help3_02_txt, Help3_03_txt, Help3_04_txt, Help3_05_txt, Help3_06_txt, Help3_07_txt, Help3_08_txt, Help3_09_txt, Help3_10_txt, Help3_11_txt, Help3_12_txt, Help3_13_txt, Help3_14_txt, Help3_15_txt, Help4_01_txt, Help4_02_txt, Help4_03_txt, Help4_04_txt, Help4_05_txt, Help4_06_txt, Help4_07_txt, Help4_08_txt, Help4_09_txt, Help4_10_txt, Help4_11_txt, Help4_12_txt, Help4_13_txt, Help4_14_txt, Help5_01_txt, Help5_02_txt, Help5_03_txt, Help5_04_txt, Help6_01_txt, Help6_02_txt, Help6_03_txt, Help6_04_txt, Help6_05_txt, Help7_01_txt, Help7_02_txt, Help7_03_txt, Help7_04_txt, Help7_05_txt, Help7_06_txt };

char * CardTitleTxtLst[59] = { Title1_01_txt, Title1_02_txt, Title1_03_txt, Title1_04_txt, Title1_05_txt, Title1_06_txt, Title2_01_txt, Title2_02_txt, Title2_03_txt, Title2_04_txt, Title2_05_txt, Title2_06_txt, Title2_07_txt, Title2_08_txt, Title2_09_txt, Title3_01_txt, Title3_02_txt, Title3_03_txt, Title3_04_txt, Title3_05_txt, Title3_06_txt, Title3_07_txt, Title3_08_txt, Title3_09_txt, Title3_10_txt, Title3_11_txt, Title3_12_txt, Title3_13_txt, Title3_14_txt, Title3_15_txt, Title4_01_txt, Title4_02_txt, Title4_03_txt, Title4_04_txt, Title4_05_txt, Title4_06_txt, Title4_07_txt, Title4_08_txt, Title4_09_txt, Title4_10_txt, Title4_11_txt, Title4_12_txt, Title4_13_txt, Title4_14_txt, Title5_01_txt, Title5_02_txt, Title5_03_txt, Title5_04_txt, Title6_01_txt, Title6_02_txt, Title6_03_txt, Title6_04_txt, Title6_05_txt, Title7_01_txt, Title7_02_txt, Title7_03_txt, Title7_04_txt, Title7_05_txt, Title7_06_txt };

char * CardSkipTitleTxtLst[52] = { Title1_01_txt, Title1_02_txt, Title1_03_txt, Title1_04_txt, Title1_05_txt, Title1_06_txt, Title2_01_txt, Title2_02_txt, Title2_03_txt, Title2_04_txt, Title2_05_txt, Title2_06_txt, Title2_07_txt, Title2_08_txt, Title2_09_txt, Title3_01_txt, Title3_02_txt, Title3_03_txt, Title3_04_txt, Title3_05_txt, Title3_06_txt, Title3_07_txt, Title3_08_txt, Title3_09_txt, Title3_11_txt, Title3_12_txt, Title3_13_txt, Title3_14_txt, Title3_15_txt, Title4_01_txt, Title4_02_txt, Title4_04_txt, Title4_05_txt, Title4_06_txt, Title4_09_txt, Title4_11_txt, Title4_12_txt, Title4_14_txt, Title5_01_txt, Title5_02_txt, Title5_04_txt, Title6_01_txt, Title6_02_txt, Title6_03_txt, Title6_04_txt, Title6_05_txt, Title7_01_txt, Title7_02_txt, Title7_03_txt, Title7_04_txt, Title7_05_txt, Title7_06_txt };

char * CardBookMarkTxtLst[7] = { BookMark1_txt, BookMark2_txt, BookMark3_txt, BookMark4_txt, BookMark5_txt, BookMark6_txt, BookMark7_txt };

static CHPTitleList cld0_17;

static CHPTitleList cld1_18;
