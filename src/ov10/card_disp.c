/*
 * OV10 original TU 6: 0x00a1e5d8..0x00a21348 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_id.h"
#include "ov10/card_play_sound1.h"
#include "ov10/card_disp.h"
#include "ov10/card_fread.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

/* Void here (measured): declaring the int it returns changes the object at its call sites. */
extern void nmlModelSetRenderLevel(int level);

/*
 * 16-byte float vector the original copies with doubleword moves (ld/sd pairs): the u64 member gives
 * the aggregate the 8-byte alignment that avoids ldl/ldr pairs (same shape as CardTutorialTranslation
 * in card_tutorial.c).
 */
typedef union CardHelpFloat4 {
    float v[4];
    u64 words[2];
} CardHelpFloat4;

/* 16-byte float vector with doubleword alignment (see CardDispHelpSlide2/3). */
typedef union CardHelpVector {
    float v[4];
    Quadword q;
} CardHelpVector;

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
 * Help screen layout (pixels): the header line sits at HELP_HEADER_Y; the index (category)
 * list and the page text start at HELP_INDEX_X, the topic list at HELP_TOPICS_X, both lists
 * at HELP_LIST_Y; the category name of a page is printed
 * at HELP_CATEGORY_X. HELP_TITLE_COLOR is the CardDispTxtSub colour index of titles.
 */
#define HELP_INDEX_X        6
#define HELP_TOPICS_X       266
#define HELP_CATEGORY_X     286
#define HELP_HEADER_Y       25
#define HELP_LIST_Y         50
#define HELP_LIST_TEXT_LENGTH 24  /* characters printed per list row */
#define HELP_TITLE_COLOR    12

/* CardHelpFileLoad: the help files are read into one area, each file 16-byte aligned. */
#define CARD_HELP_FILE_BUFFER   0x01720000  /* lui s0,0x172 at 0x00a1e7f0 */
#define CARD_HELP_FILE_ALIGN    16          /* addiu 15 / and -16 at 0x00a1e884 */
#define CARD_HELP_FILE_MAX      64          /* loop bound s6 + 256 at 0x00a1e8e4 = sizeof cardlex_tbl / 4 */
#define CARD_HELP_NAME_COUNT    35          /* 34 names plus the NULL terminator (140-byte copy) */

/*
 * Help-slide animation (shared by CardDispHelpSlide1/2/3): the counter ChangeCnt wraps
 * around the list length and the entry shown changes every HELP_SLIDE_ENTRY_TICKS.
 */
#define HELP_SLIDE_ENTRY_TICKS      64                                  /* sra 6 at 0x00a1ec6c */
#define HELP_SLIDE_CYCLE_4          (4 * HELP_SLIDE_ENTRY_TICKS)        /* 256: sra 8 / sll 8 at 0x00a1ec4c */
#define HELP_SLIDE_CYCLE_3          (3 * HELP_SLIDE_ENTRY_TICKS)        /* 192: li a3,0xc0 at 0x00a1ec9c */
#define HELP_SLIDE_CYCLE_6          (6 * HELP_SLIDE_ENTRY_TICKS)        /* 384: six-entry lists of CardDispHelpSlide2 */

/*
 * Pulse of the lit help card: the light level is 1 + sin(ChangeCnt * 11.25 degrees) / 2
 * (angle converted to radians as degrees / 180 * pi, in float).
 */
#define HELP_PULSE_DEGREES_PER_TICK 11.25f
#define HELP_PULSE_HALF_TURN        180.0f
#define HELP_PULSE_PI               3.1415927f

/* CardDispTejyun: the phase cards are stacked, each this much lower than the one above. */
#define HELP_PHASE_ROW_STEP         0.3f

/*
 * Card resource models and textures at fixed addresses (loaded by the caller). The icon
 * models are 0x400 apart from CARD_ICON_MODEL_BASE (the generation markers of
 * CardCountGeneration in card_fread.h are the same bank); CardDispAttr, CardDispNazo and
 * CardDispGuno draw them by index. The frame models and textures belong to the help menu
 * panels (letters follow the address order).
 */
#define CARD_HELP_STATUS_MODEL_BASE 0x01C8A800  /* page mark, indexed by PageStatusLst[3 * helpTopic] */
#define CARD_HELP_FRAME_MODEL_A     0x01CA1000
#define CARD_HELP_FRAME_MODEL_B     0x01CA1C00
#define CARD_HELP_FRAME_MODEL_C     0x01CA2B00
#define CARD_HELP_FRAME_MODEL_D     0x01CA4000
#define CARD_HELP_FRAME_MODEL_E     0x01CA6000
#define CARD_HELP_FRAME_TEXTURE_A   ((const char *)0x01CB0000)
#define CARD_HELP_FRAME_TEXTURE_B   ((const char *)0x01CC0050)

#define CARD_HELP_CARD_REQUIREMENTS_A    CARD_ID_LV_10_SHION    /* first REQUIREMENTS page (its three H cards): li a1,7 at 0x00a206e0 */
#define CARD_HELP_CARD_REQUIREMENTS_B    CARD_ID_BUNNIE    /* second REQUIREMENTS page (the any-attribute requirement): li a1,0x1D at 0x00a20700 */

/*
 * CardHelpProc states: the high nibble of CardGameWork.helpState selects the screen
 * (`andi $4,$2,0xF0` at 0x00a209ac); every other value shows a help page.
 */
#define CARD_HELP_STATE_MASK    0xF0    /* 0x00a209ac */
#define CARD_HELP_STATE_OFF     0x00    /* sb $0 at 0x00a20e34: back out of the first page, help ends */
#define CARD_HELP_STATE_PAGE    0x01    /* sb 1 at 0x00a20a20, 0x00a20c60: show helpTopic */
#define CARD_HELP_STATE_INDEX   0x10    /* sb at 0x00a209f4, 0x00a20b88, 0x00a20e0c: category list ("Help Index") */
#define CARD_HELP_STATE_TOPICS  0x20    /* sb at 0x00a20abc, case at 0x00a209a8: topic list of one category */

/*
 * Pad bits CardHelpProc tests (input[].pressed; the d-pad ones are input[].directions).
 * Next/previous page are R1/L1 as in CardTutorialProc; the compound masks accept the
 * d-pad direction beside the button.
 */
#define CARD_HELP_PAD_BACK_OR_LEFT    (PAD_CROSS | PAD_LEFT)    /* andi 0x8040 at 0x00a20b70: cancel the topic list */
#define CARD_HELP_PAD_SELECT_OR_RIGHT (PAD_CIRCLE | PAD_RIGHT)  /* andi 0x2020 at 0x00a20a5c: open the chosen category's topics */

#define CARD_HELP_CATEGORY_COUNT      7     /* TitleMaxList[7]; the last category index is 6 (slti 6 at 0x00a20a68) */
#define CARD_HELP_INDEX_ROWS          9     /* `rows` of the category list (sh 9 at 0x00a1e680) */
#define CARD_HELP_TOPICS_PER_CATEGORY 14    /* SkipPageLst row stride, x14 at 0x00a20c3c..0x00a20c48 (7 rows, 0x62 bytes) */
#define CARD_HELP_HISTORY_MAX       100     /* sltiu 0x64 at 0x00a20c4c, 0x00a20db4: bound of activeHelpPage */
#define CARD_HELP_STATUS_STRIDE     3       /* PageStatusLst bytes per page (177 = 59 pages x 3): mark, number in the category, one more */
#define CARD_HELP_RADIX             10     /* page-number digits: addiu 0xA at 0x00a20e90 */

/* nmlModelSetRenderLevel is declared void here although main/nml_model_set.h returns int (measured: the
 * int return changes the scheduling around its calls in CardHelpProc, so the header is not included). */
extern char *CardSkipTitleTxtLst[];
extern char *CardBookMarkTxtLst[];
extern char *CardTitleTxtLst[];
extern char *CardHelpTxtLst[];

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
static int ChangeCnt = 0;

/*
 * CHPTitleInitSub resets the help-title list when CardHelpProc (0x00a20908) opens a
 * help category; CHPMenuInitSub (0x00a1e640) runs immediately after it. TitleMaxList
 * (7 bytes) holds one entry-count byte per category, read here through a pointer
 * captured early and dereferenced once per field (rows and count each get their own
 * read). The help lists are CardListWindow records: the title string table is the one
 * the list draws (`rows` strings from `top`), `cursor` the selected row.
 */
void CHPTitleInitSub(CardListWindow *list, int category)
{
    const u8 *entry;
    u8 raw;

    list->x = HELP_TOPICS_X;
    list->y = HELP_LIST_Y;
    entry = &TitleMaxList[category];
    list->textLength = HELP_LIST_TEXT_LENGTH;
    list->active = 0;
    list->baseColor = 0;
    list->cursorColor = 0;
    list->cursor = 0;
    list->top = 0;
    raw = *entry;
    list->shownCount = 1;
    list->rows = (s8)raw;
    raw = *entry;
    list->count = (s8)raw;
}

/*
 * CHPMenuInitSub (0x00a1e640, 0x64 bytes): resets the help-menu list `menu`
 * (its `rows` field is CARD_HELP_INDEX_ROWS, its `count` the seven categories), then re-initialises the
 * title list `title` for category 0 (TitleMaxList[0] = 6) by a tail jump
 * (0x00a1e69c `j CHPTitleInitSub`, a0 = 2nd argument, a1 = 0).
 * Callers: CardHelpProc 0x00a209ec and 0x00a21df0.
 */
void CHPMenuInitSub(CardListWindow *menu, CardListWindow *title)
{
    menu->x = HELP_INDEX_X;         /* 0x00a1e674 sh $6,0x6 */
    menu->y = HELP_LIST_Y;          /* 0x00a1e678 sh $7,0x8 */
    menu->textLength = HELP_LIST_TEXT_LENGTH; /* 0x00a1e67c sh $8,0xA */
    menu->active = 0;
    menu->baseColor = 0;
    menu->cursorColor = 0;
    menu->cursor = 0;
    menu->top = 0;
    menu->rows = CARD_HELP_INDEX_ROWS;       /* 0x00a1e680 sh $9,0x10 */
    menu->shownCount = 1;              /* 0x00a1e684 sh $10,0xE */
    menu->count = CARD_HELP_CATEGORY_COUNT;  /* 0x00a1e670 sh $2,0x12 */
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
 * nmlModelSetTexture / nmlModelSetPlace / nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL).
 * Caller: CardHelpProc 0x00a20d34 (a0 = sp). All callees void (no $v0 reads).
 * The two float locals `left` and `zero` stay in callee-saved FP registers across
 * the calls; z is assigned before y in each position block.
 */
static void CHPMenuBackDispSub(Matrix4 matrix)
{
    Matrix4 transformed;
    float position[4];
    float left = -1.4184f;     /* 0xBFB58E22 */
    float zero = 0.0f;

    position[0] = left;
    position[2] = zero;
    position[1] = zero;
    position[3] = 1.0f;
    xglMatrixTrans(transformed, matrix, position);
    nmlModelSetTexture(CARD_HELP_FRAME_TEXTURE_A);
    nmlModelSetPlace(&transformed);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(CARD_HELP_FRAME_MODEL_C);

    position[0] = 1.4184f;      /* 0x3FB58E22 */
    position[2] = zero;
    position[1] = zero;
    xglMatrixTrans(transformed, matrix, position);
    nmlModelSetTexture(CARD_HELP_FRAME_TEXTURE_A);
    nmlModelSetPlace(&transformed);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(CARD_HELP_FRAME_MODEL_A);

    position[0] = left;
    position[1] = -1.832f;      /* 0xBFEA7EFA */
    xglMatrixTrans(transformed, matrix, position);
    nmlModelSetTexture(CARD_HELP_FRAME_TEXTURE_B);
    nmlModelSetPlace(&transformed);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(CARD_HELP_FRAME_MODEL_D);
}

/*
 * CardDispHelpCurrySet (0x00a1ea70, 0x140 bytes): three curry card models drawn at
 * translation (1.5143, y, 0, 1), scale (0.46, 0.46, 1, 1); y = 1.186, 0.215, -0.757
 * with the cards Lv 1 Shion (a battle card), Curry (situation) and Hilbert Effect (event) passed to
 * CardDisp(.., 1, 0).
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
    xglMatrixTrans(transformed, matrix, translation);
    xglMatrixScale(transformed, transformed, scale);
    CardDisp(&transformed, CARD_ID_LV_1_SHION, 1, 0);

    translation[1] = 0.215f;      /* 0x3E5C28F6 */
    xglMatrixTrans(transformed, matrix, translation);
    xglMatrixScale(transformed, transformed, scale);
    CardDisp(&transformed, CARD_ID_CURRY, 1, 0);

    translation[1] = -0.757f;     /* 0xBF41CAC1 */
    xglMatrixTrans(transformed, matrix, translation);
    xglMatrixScale(transformed, transformed, scale);
    CardDisp(&transformed, CARD_ID_HILBERT_EFFECT, 1, 0);
}

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
        xglMatrixTrans(transformed, matrix, trans);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list16[i], 1, 0);
        break;
    case CARD_HELP_PAGE_EVENT_CARD:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, trans);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list17[i], 1, 0);
        break;
    case CARD_HELP_PAGE_SITUATION_CARD:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, trans);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list18[i], 1, 0);
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
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list19[i], 1, 0);
        break;
    case CARD_HELP_PAGE_TYPES:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list20[i], 1, 0);
        break;
    case CARD_HELP_PAGE_NAMES:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list21[i], 1, 0);
        break;
    case CARD_HELP_PAGE_ATTRIBUTES:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDispLight(&transformed, list22[i], 1, CARD_LIGHT_ATTRIBUTE);
        break;
    case CARD_HELP_PAGE_COST:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDispLight(&transformed, list25[i], 1, CARD_LIGHT_RESERVE);
        break;
    case CARD_HELP_PAGE_HP:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDispLight(&transformed, list26[i], 1, CARD_LIGHT_HIT_POINTS);
        break;
    case CARD_HELP_PAGE_ATK:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDispLight(&transformed, list27[i], 1, CARD_LIGHT_POWER);
        break;
    case CARD_HELP_PAGE_ATTACK_TYPE:
        ChangeCnt %= HELP_SLIDE_CYCLE_4;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDispLight(&transformed, list28[i], 1, CARD_LIGHT_ATTACK);
        break;
    case CARD_HELP_PAGE_WEAPONS_A:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list45[i], 1, 0);
        break;
    case CARD_HELP_PAGE_WEAPONS_B:
        ChangeCnt %= HELP_SLIDE_CYCLE_6;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list46[i], 1, 0);
        break;
    case CARD_HELP_PAGE_GNOSIS:
        ChangeCnt %= HELP_SLIDE_CYCLE_4;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list47[i], 1, 0);
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
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list23[i], 1, 0);
        break;
    case CARD_HELP_PAGE_REQUIREMENTS_B:
        ChangeCnt %= HELP_SLIDE_CYCLE_3;
        i = ChangeCnt / HELP_SLIDE_ENTRY_TICKS;
        xglMatrixTrans(transformed, matrix, translation.v);
        xglMatrixScale(transformed, transformed, scale);
        CardDisp(&transformed, list24[i], 1, 0);
        break;
    }
}

void CardDispCtr(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispCtrTranslation;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_HELP5]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_HELP5]);
}

void CardDispField(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;

    xglMatrixTrans(transformed, matrix, translation);
    nmlModelSetPlace(&transformed);
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
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    xglMatrixScale(transformed, transformed,
                   scale.v);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_DECKMAKE]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_DECKMAKE]);
}

void CardDispH1P2P(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;

    xglMatrixTrans(transformed, matrix, translation);
    nmlModelSetPlace(&transformed);
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
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    xglMatrixScale(transformed, transformed,
                   scale.v);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_HELP6]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_HELP6]);
}

void CardDispFieldPointScale(Matrix4 matrix, const float translation[4],
                              const float scale[4], int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    float light;

    light = (xglSin((((float)ChangeCnt * HELP_PULSE_DEGREES_PER_TICK) / HELP_PULSE_HALF_TURN) * HELP_PULSE_PI)
             * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, matrix, translation);
    xglMatrixScale(transformed, transformed, scale);
    nmlModelSetLight(&color, &asDir);
    nmlModelSetPlace(&transformed);
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
    light = (xglSin((((float)ChangeCnt * HELP_PULSE_DEGREES_PER_TICK) / HELP_PULSE_HALF_TURN) * HELP_PULSE_PI)
             * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    xglMatrixScale(transformed, transformed,
                   scale.v);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_FIELD]);
    nmlModelEntry(cardlex_tbl[CARD_HELP_RES_FIELD]);
    xglMatrixTrans(transformed, matrix,
                   offset.v);

    switch (kind) {
    case CARD_HELP_RES_PHASE1:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_BASE);
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        nmlModelSetLight(&color, &asDir);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE1]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE1]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE2]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE2]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        break;
    case CARD_HELP_RES_PHASE2:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_BATTL);
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_STANDBY);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE1]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE1]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetLight(&color, &asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE2]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE2]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        break;
    case CARD_HELP_RES_PHASE3:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE2]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE2]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetLight(&color, &asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE4]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE4]);
        break;
    case CARD_HELP_RES_PHASE4:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_STANDBY);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE3]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE3]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetLight(&color, &asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE4]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE4]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        break;
    case CARD_HELP_RES_PHASE5:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND2);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE4]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE4]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetLight(&color, &asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE6]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE6]);
        break;
    case CARD_HELP_RES_PHASE6:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_BATTL);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetLight(&color, &asDir);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE6]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE6]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE7]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE7]);
        break;
    case CARD_HELP_RES_PHASE7:
        CardDispFieldPointScale(matrix, translation.v,
                                scale.v, CARD_HELP_RES_HAND);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE5]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE5]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE6]);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE6]);
        transformed[3][1] -= HELP_PHASE_ROW_STEP;
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(cardxtx_tbl[CARD_HELP_RES_PHASE7]);
        nmlModelSetLight(&color, &asDir);
        nmlModelEntry(cardlex_tbl[CARD_HELP_RES_PHASE7]);
        break;
    }
}

void CardDispFieldPoint(Matrix4 matrix, const float translation[4], int kind)
{
    Matrix4 transformed;
    Matrix4 color;
    float light;

    light = (xglSin((((float)ChangeCnt * HELP_PULSE_DEGREES_PER_TICK) / HELP_PULSE_HALF_TURN) * HELP_PULSE_PI)
             * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, matrix, translation);
    nmlModelSetLight(&color, &asDir);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

/*
 * CardDispGun (0x00a1fe60, 0xb8 bytes): translation cardDispGunTranslation {1.397, 0.863, 0, 1},
 * scale cardDispGunScale {0.6, 0.6, 1, 1}; CardDisp(Handgun, 1, 0); then the model is
 * moved down by 1.33 (double: fptodp 0x00a1fecc, ld the double literal at 0x00a4e160, dpsub, dptofp,
 * stored back at sp+0x34 = transformed[3][1]) and CardDisp(Ogre, 1, 0).
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
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    xglMatrixScale(transformed, transformed,
                   scale.v);
    CardDisp(&transformed, CARD_ID_HANDGUN, 1, 0);      /* 0x00a1feb8 addiu a1,0x31 */
    transformed[3][1] -= 1.33;            /* double literal at 0x00a4e160 */
    CardDisp(&transformed, CARD_ID_OGRE, 1, 0);      /* 0x00a1fef4 addiu a1,0x44 */
}

void CardDispShineCurry(Matrix4 matrix, const float translation[4])
{
    Matrix4 transformed;
    CardHelpFloat4 scale;

    scale = cardDispShineCurryScale;
    xglMatrixTrans(transformed, matrix, translation);
    xglMatrixScale(transformed, transformed,
                   scale.v);
    CardDispLight(&transformed, CARD_ID_CURRY, 1, CARD_LIGHT_ALL);
}

void CardDispAttr(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;
    const float step = -0.38f;

    translation = cardDispAttrTranslation;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    nmlModelSetPlace(&transformed);

    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE);
    transformed[3][1] += step;

    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 2 * CARD_MODEL_STRIDE);
    transformed[3][1] += step;

    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 3 * CARD_MODEL_STRIDE);
    transformed[3][1] += step;

    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 1 * CARD_MODEL_STRIDE);
    transformed[3][1] += step;

    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 5 * CARD_MODEL_STRIDE);
    transformed[3][1] += -0.57f;

    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 4 * CARD_MODEL_STRIDE);
}

void CardDispNazo(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispNazoTranslation;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 7 * CARD_MODEL_STRIDE);
}

void CardDispGuno(Matrix4 matrix)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispGunoTranslation;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntry(CARD_ICON_MODEL_BASE + 3 * CARD_MODEL_STRIDE);
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
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    xglMatrixScale(transformed, transformed, scale);
    CardDispLight(&transformed, card, 1, CARD_LIGHT_NEED);   /* 0x00a2024c: a2 = 1, a3 = 0x20 */
}

void CardDispComm(Matrix4 matrix, int kind)
{
    Matrix4 transformed;
    CardHelpFloat4 translation;

    translation = cardDispCommTranslation;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    nmlModelSetPlace(&transformed);
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
    light = (xglSin((((float)ChangeCnt * HELP_PULSE_DEGREES_PER_TICK) / HELP_PULSE_HALF_TURN) * HELP_PULSE_PI)
             * 0.5f) + 1.0f;
    color[3][0] = color[3][1] = color[3][2] = light;
    xglMatrixTrans(transformed, matrix,
                   translation.v);
    nmlModelSetLight(&color, &asDir);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(cardxtx_tbl[kind]);
    nmlModelEntry(cardlex_tbl[kind]);
}

const CardHelpFloat4 cardDispHelpCardTranslationA = {{ 1.397f, 0.215f, 0.0f, 1.0f }};
const CardHelpFloat4 cardDispHelpCardTranslationB = {{ 1.397f, -0.188f, 0.0f, 1.0f }};

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

static CardListWindow cld0_17;
static CardListWindow cld1_18;

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
    xglMatrixScale(matrix, matrix, scale);

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
        if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->helpState = CARD_HELP_STATE_PAGE;
        }
        if (directions & PAD_UP) {
            if (cld0_17.cursor != 0) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                cld0_17.cursor--;
                CHPTitleInitSub(&cld1_18, cld0_17.cursor);
            }
        } else if (directions & PAD_DOWN) {
            if (cld0_17.cursor < CARD_HELP_CATEGORY_COUNT - 1) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                cld0_17.cursor++;
                CHPTitleInitSub(&cld1_18, cld0_17.cursor);
            }
        } else if (pressed & CARD_HELP_PAD_SELECT_OR_RIGHT) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->helpState = CARD_HELP_STATE_TOPICS;
        }
        xglFontPrint(0, 0, 0, "\031\003");
        xglFontPrint(HELP_INDEX_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "\r");
        xglFontPrint(HELP_INDEX_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "Help Index");
        xglFontPrint(HELP_TOPICS_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "Topics");
        CardStrListDisp(&cld0_17, CardBookMarkTxtLst, cld0_17.cursor);
        CardStrListDisp(&cld1_18, &CardSkipTitleTxtLst[SkipStartList[cld0_17.cursor]], -1);
        CHPMenuBackDispSub(matrix);
    } break;
    case CARD_HELP_STATE_TOPICS: {
        if (pressed & CARD_HELP_PAD_BACK_OR_LEFT) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->helpState = CARD_HELP_STATE_INDEX;
        }
        if (directions & PAD_UP) {
            if (cld1_18.cursor != 0) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                cld1_18.cursor--;
            }
        } else if (directions & PAD_DOWN) {
            if (cld1_18.cursor < cld1_18.rows - 1) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                cld1_18.cursor++;
            }
        } else if (pressed & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->helpTopic = SkipPageLst[cld1_18.cursor + cld0_17.cursor * CARD_HELP_TOPICS_PER_CATEGORY];
            work->helpState = CARD_HELP_STATE_PAGE;
            if (work->activeHelpPage < CARD_HELP_HISTORY_MAX) {
                work->activeHelpPage++;
            }
            work->helpPage[work->activeHelpPage] = work->helpTopic;
        }
        CardStrListDisp(&cld0_17, CardBookMarkTxtLst, cld0_17.cursor);
        CardStrListDisp(&cld1_18, &CardSkipTitleTxtLst[SkipStartList[cld0_17.cursor]], cld1_18.cursor);
        xglFontPrint(0, 0, 0, "\031\003");
        xglFontPrint(HELP_INDEX_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "\r");
        xglFontPrint(HELP_INDEX_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "Help Index");
        xglFontPrint(HELP_TOPICS_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "Topics");
        CHPMenuBackDispSub(matrix);
    } break;
    default: {
        if (pressed & PAD_R1) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            work->helpTopic++;
            if (work->helpTopic >= CARD_HELP_PAGE_COUNT) {
                work->helpTopic = 0;
            }
            if (work->activeHelpPage < CARD_HELP_HISTORY_MAX) {
                work->activeHelpPage++;
            }
            work->helpPage[work->activeHelpPage] = work->helpTopic;
        } else if (pressed & PAD_L1) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            work->helpTopic = work->helpTopic - 1;
            if (work->helpTopic < 0) {
                work->helpTopic = CARD_HELP_PAGE_LAST;
            }
            if (work->activeHelpPage < CARD_HELP_HISTORY_MAX) {
                work->activeHelpPage++;
            }
            work->helpPage[work->activeHelpPage] = work->helpTopic;
        } else if (pressed & PAD_SQUARE) {
            CHPMenuInitSub(&cld0_17, &cld1_18);
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->helpState = CARD_HELP_STATE_INDEX;
        } else if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            if (work->activeHelpPage == 0) {
                work->helpState = CARD_HELP_STATE_OFF;
            } else {
                work->activeHelpPage--;
                work->helpTopic = work->helpPage[work->activeHelpPage];
            }
        }

        xglFontPrint(HELP_INDEX_X, HELP_HEADER_Y, CARD_TEXT_COLOR, "");
        CardDispTxtSub(HELP_INDEX_X, HELP_HEADER_Y, CardTitleTxtLst[work->helpTopic], HELP_TITLE_COLOR);
        CardDispTxtSub2(HELP_INDEX_X, HELP_LIST_Y, CardHelpTxtLst[work->helpTopic]);
        CardDispTxtSub(HELP_CATEGORY_X, HELP_HEADER_Y, CardBookMarkTxtLst[PageStatusLst[CARD_HELP_STATUS_STRIDE * work->helpTopic]],
                       HELP_TITLE_COLOR);
        CardDispHelpCard(work->helpTopic, matrix);

        translation[1] = -1.496f;
        translation[2] = 0.0f;
        translation[3] = 1.0f;
        scale[0] = scale[1] = 0.9f;
        scale[2] = scale[3] = 1.0f;
        translation[0] = 1.0064001f;
        cardModel = CARD_HELP_STATUS_MODEL_BASE + (PageStatusLst[CARD_HELP_STATUS_STRIDE * work->helpTopic] << CARD_DIGIT_MODEL_SHIFT);
        xglMatrixTrans(transformed, matrix, translation);
        xglMatrixScale(transformed, transformed, scale);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(cardModel);

        digit = (PageStatusLst[CARD_HELP_STATUS_STRIDE * work->helpTopic + 1] + 1) / CARD_HELP_RADIX;
        if (digit != 0) {
            translation[0] = 1.3564f;
            cardModel = CARD_DIGIT_MODEL_BASE + (digit << CARD_DIGIT_MODEL_SHIFT);
            xglMatrixTrans(transformed, matrix, translation);
            xglMatrixScale(transformed, transformed, scale);
            nmlModelSetPlace(&transformed);
            nmlModelSetTexture(CARD_DIGIT_TEXTURE);
            nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
            nmlModelEntryCard(cardModel);
        }

        cardModel = CARD_DIGIT_MODEL_BASE
                    + (((PageStatusLst[CARD_HELP_STATUS_STRIDE * work->helpTopic + 1] + 1) % CARD_HELP_RADIX) << CARD_DIGIT_MODEL_SHIFT);
        translation[0] = 1.4824001f;
        xglMatrixTrans(transformed, matrix, translation);
        xglMatrixScale(transformed, transformed, scale);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(cardModel);

        digit = (work->helpTopic + 1) / CARD_HELP_RADIX;
        if (digit != 0) {
            translation[0] = 1.8304f;
            cardModel = CARD_DIGIT_MODEL_BASE + (digit << CARD_DIGIT_MODEL_SHIFT);
            xglMatrixTrans(transformed, matrix, translation);
            xglMatrixScale(transformed, transformed, scale);
            nmlModelSetPlace(&transformed);
            nmlModelSetTexture(CARD_DIGIT_TEXTURE);
            nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
            nmlModelEntryCard(cardModel);
        }

        cardModel = CARD_DIGIT_MODEL_BASE + (((work->helpTopic + 1) % CARD_HELP_RADIX) << CARD_DIGIT_MODEL_SHIFT);
        translation[0] = 1.9564f;
        xglMatrixTrans(transformed, matrix, translation);
        xglMatrixScale(transformed, transformed, scale);
        nmlModelSetPlace(&transformed);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(cardModel);

        translation[0] = -1.4184f;
        translation[1] = translation[2] = 0.0f;
        translation[3] = 1.0f;
        cardModel = CARD_HELP_FRAME_MODEL_A;
        xglMatrixTrans(transformed, matrix, translation);
        nmlModelSetTexture(CARD_HELP_FRAME_TEXTURE_A);
        nmlModelSetPlace(&transformed);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(cardModel);

        translation[0] = 1.4184f;
        translation[1] = 1.832f;
        cardModel = CARD_HELP_FRAME_MODEL_B;
        xglMatrixTrans(transformed, matrix, translation);
        nmlModelSetTexture(CARD_HELP_FRAME_TEXTURE_B);
        nmlModelSetPlace(&transformed);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(cardModel);

        translation[0] = 1.4184f;
        translation[1] = -1.832f;
        cardModel = CARD_HELP_FRAME_MODEL_E;
        xglMatrixTrans(transformed, matrix, translation);
        nmlModelSetTexture(CARD_HELP_FRAME_TEXTURE_B);
        nmlModelSetPlace(&transformed);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(cardModel);
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

static CardListWindow cld0_17;

static CardListWindow cld1_18;
