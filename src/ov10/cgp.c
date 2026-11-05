/*
 * OV10 original TU 8: 0x00a21768..0x00a324b8 (68 functions)
 */
#include "common.h"
#include "cgp.h"

static char OLMess0000Txt[8];
static char OLMess0010Txt[32];
static char OLMess0011Txt[48];
static char OLMess0020Txt[56];
static char OLMess0030Txt[48];
static char OLMess0040Txt[56];
static char OLMess0041Txt[40];
static char OLMess0042Txt[40];
static char OLMess0050Txt[32];
static char OLMess0060Txt[32];
static char OLMess0061Txt[48];
static char OLMess0070Txt[56];
static char OLMess0080Txt[48];
static char OLMess0090Txt[56];
extern char PermMess000Txt[];
extern char PermMess001Txt[];
extern char PermMess002Txt[];
extern char PermMess003Txt[];
extern char PermMess004Txt[];
extern char PermMess005Txt[];
extern char PermMess006Txt[];
extern char PermMess007Txt[];
extern char PermMess008Txt[];
extern char PermMess009Txt[];
extern char PermMess010Txt[];
extern char PermMess011Txt[];
extern char PermMess012Txt[];
extern char PermMess013Txt[];
extern char PermMess014Txt[];
extern const char D_00A4E4F0[];
extern const char D_00A4E500[];
extern const char D_00A4E508[];
extern const char D_00A4E510[];
extern const char D_00A4E520[];
extern const char D_00A4E530[8];
extern const char D_00A4E538[8];

/*
 * Card models are loaded into a fixed EE runtime arena.  card_play_sound1.c
 * fills that arena with CardFread at 0x01900000 and 0x01930000, then passes
 * those addresses to nmlModelEntryCard.  These tables retain the original
 * arena addresses; they do not define storage for the model payloads.
 */
#define CARD_MODEL_ARENA_BASE 0x01900000
#define CARD_MODEL_AT(offset) ((void *)(CARD_MODEL_ARENA_BASE + (offset)))

static void *CPISMdlLst[14] = {
    CARD_MODEL_AT(0xA800), CARD_MODEL_AT(0xB800), CARD_MODEL_AT(0xD800),
    CARD_MODEL_AT(0xC800), CARD_MODEL_AT(0x10800), CARD_MODEL_AT(0xE800),
    CARD_MODEL_AT(0xF800), CARD_MODEL_AT(0x1A800), CARD_MODEL_AT(0x1B800),
    CARD_MODEL_AT(0x1D800), CARD_MODEL_AT(0x1C800), CARD_MODEL_AT(0x20800),
    CARD_MODEL_AT(0x1E800), CARD_MODEL_AT(0x1F800)
};

static void *CPIPhaseMdlLst[14] = {
    CARD_MODEL_AT(0x22800), CARD_MODEL_AT(0x23800), CARD_MODEL_AT(0x25800),
    CARD_MODEL_AT(0x24800), CARD_MODEL_AT(0x26800), CARD_MODEL_AT(0x27800),
    CARD_MODEL_AT(0x28800), CARD_MODEL_AT(0x22800), CARD_MODEL_AT(0x23800),
    CARD_MODEL_AT(0x25800), CARD_MODEL_AT(0x24800), CARD_MODEL_AT(0x26800),
    CARD_MODEL_AT(0x27800), CARD_MODEL_AT(0x28800)
};

float Player1Color[4][4] = {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.8f, 0.8f, 1.2f, 1.0f}
};

float Player2Color[4][4] = {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.2f, 0.8f, 0.8f, 1.0f}
};

char *PermMessTbl[16] = {
    PermMess000Txt, PermMess001Txt, PermMess002Txt, PermMess003Txt, PermMess004Txt, PermMess005Txt, PermMess006Txt, PermMess007Txt, PermMess008Txt, PermMess009Txt, PermMess010Txt, PermMess011Txt, PermMess012Txt, PermMess013Txt, PermMess014Txt, PermMess000Txt
};

char *OLMessTbl[16] = {
    OLMess0010Txt, OLMess0020Txt, OLMess0040Txt, OLMess0030Txt, OLMess0042Txt, OLMess0050Txt, OLMess0000Txt, OLMess0060Txt, OLMess0070Txt, OLMess0090Txt, OLMess0080Txt, OLMess0041Txt, OLMess0050Txt, OLMess0000Txt, OLMess0011Txt, OLMess0061Txt
};

static s16 CGPCPTbl[9] = { 0x003F, 0x0040, 0x0041, 0x0083, 0x0085, 0x008B, 0x008D, 0x008E, 0x008F };

unsigned char SampleDeck0[40] = {
    0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x04, 0x04, 0x93, 0x93, 0x93, 0x54, 0x93, 0x00, 0x21, 0x32, 0x03, 0x00, 0x21, 0x02
};

unsigned char SampleDeck1[40] = {
    0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x16, 0x16, 0x1B, 0x1C, 0x1C, 0x1C, 0x1B, 0x17, 0x16
};

unsigned char SampleDeck2[40] = {
    0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x18, 0x1B, 0x1B, 0x1B, 0x1C, 0x1C, 0x1C, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x57, 0x57, 0x57, 0x6A, 0x6A, 0x6A, 0x6D, 0x6D, 0x6D, 0x11, 0x81, 0x17, 0x17, 0x17, 0x18, 0x18, 0x18
};

unsigned char SampleDeck3[40] = {
    0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x11, 0x11, 0x11, 0x12, 0x12, 0x1F, 0x1F, 0x1F, 0x21, 0x21, 0x21, 0x24, 0x24, 0x24, 0x27, 0x27, 0x27, 0x2B, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 0x56, 0x56, 0x56
};

unsigned char SampleDeck4[40] = {
    0x00, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x43, 0x43, 0x43, 0x45, 0x45, 0x45, 0x4A, 0x4A, 0x4A, 0x4B, 0x4B, 0x4B, 0x58, 0x58, 0x58, 0x5A, 0x5A, 0x5A, 0x61, 0x61, 0x61, 0x72, 0x72, 0x72
};

unsigned char SampleDeck5[40] = {
    0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x0F, 0x0F, 0x0F, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x1E, 0x1E, 0x1F, 0x1F, 0x24, 0x24, 0x24, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x7B, 0x7B, 0x7B
};

unsigned char SampleDeck6[40] = {
    0x00, 0x00, 0x04, 0x04, 0x07, 0x0F, 0x0F, 0x14, 0x14, 0x15, 0x16, 0x16, 0x16, 0x1E, 0x1E, 0x1E, 0x1F, 0x1F, 0x20, 0x23, 0x25, 0x27, 0x2C, 0x2C, 0x2C, 0x52, 0x52, 0x52, 0x55, 0x55, 0x56, 0x56, 0x5B, 0x5B, 0x5D, 0x5D, 0x5D, 0x78, 0x7A, 0x7A
};

unsigned char SampleDeck7[40] = {
    0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x1F, 0x1F, 0x24, 0x24, 0x24, 0x38, 0x38, 0x38, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x58, 0x6A, 0x6A, 0x6A, 0x6D, 0x6D, 0x6D, 0x80, 0x80, 0x8C, 0x8C
};

unsigned char SampleDeck8[40] = {
    0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x76, 0x76, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x76, 0x76
};

unsigned char SampleDeck9[40] = {
    0x10, 0x10, 0x10, 0x16, 0x5F, 0x5F, 0x5F, 0x5F, 0x18, 0x18, 0x18, 0x37, 0x37, 0x37, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3E, 0x3E, 0x3E, 0x3D, 0x3D, 0x3D, 0x1D, 0x1D, 0x1D, 0x5B, 0x74, 0x74, 0x65, 0x76, 0x18, 0x0C, 0x0D, 0x8B, 0x8B, 0x8B
};

char *EnemyDeckLst[48] = {0};

char *DeckDataLst[48] = {0};

char *DeckGeneLst[48] = {0};

char *EnemyDeckListStr[48] = {0};

typedef struct CpuDeckRecord {
    char name[0x20];
    char gene[0x10];
    char deck[0x28];
    char data[0x28];
} CpuDeckRecord;

CpuDeckRecord cpudeck[48] = {{0}};

unsigned char CardDat[0x100] = {
    0x3C, 0x1E, 0x3C, 0x37, 0x50, 0x50, 0x41, 0x64, 0x1E, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x3C, 0x1E, 0x1E, 0x1E, 0x1E, 0x32, 0x32, 0x4B, 0x14, 0x14, 0x14, 0x14, 0x28, 0x28, 0x0F, 0x50, 0x69, 0x82, 0x4B, 0x6E, 0x64, 0x46, 0x4F, 0x50, 0x50, 0x50, 0x50, 0x1E, 0x3C, 0x19, 0x32, 0x32, 0x14, 0x0A, 0x1E, 0x28, 0x32, 0x32, 0x32, 0x64, 0x3C, 0x64, 0x32, 0x32, 0x32, 0x46, 0x46, 0x32, 0x2D, 0x1E, 0x2D, 0x3C, 0x32, 0x46, 0x32, 0x3C, 0x3C, 0x6E, 0x3C, 0x64, 0x5A, 0x8C, 0x3C, 0x50, 0x32, 0x64, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

typedef struct CGPCursorPosition {
    s16 zone;
    s16 index;
    s16 cardId;
    s16 stackedCard;
} CGPCursorPosition;


/* Movement record fields used by the slot swap at 0x00a25b70. */
typedef struct CardMoveLayer {
    u8 state;
    u8 preservedByte01;
    u8 boostCount;
    u8 life;
    u8 preservedByte04;
    u8 damageTimer;
    s16 cardId;
    u8 unmodeled_08[2];
} CardMoveLayer;

typedef struct CardMoveWork {
    u8 flags;
    u8 moveStep;
    u8 power;
    u8 powerLimit;
    u8 unmodeled_04[4];
    CardMoveLayer layers[40];
    float position[3];
    float velocity[3];
    float shuntPosition[3];
} CardMoveWork;

/* The same hand bytes have two evidenced owner views in this TU. */
typedef union CardPlayHandViews {
    CardHand costView;
    CardPlayHand maintenanceView;
} CardPlayHandViews;

typedef struct CardPlayDeploymentStack {
    u8 flags;
    u8 moveTimer;
    u8 power;
    u8 powerLimit;
    u8 unmodeled_04[4];
    CardBattleLayer layers[40];
    float position[3];
    float step[3];
} CardPlayDeploymentStack;

typedef struct CardPlayDeploymentSlot {
    u8 unmodeled_00[0x0C];
    CardPlayDeploymentStack stack;
} CardPlayDeploymentSlot;

typedef struct CardMoveRecord {
    u8 unmodeled_00[2];
    u8 kind;                       /* +0x02: zero for the moves into the discard pile (sb 0x00a296f0) */
    u8 moveTimer;
    float position[3];
    float step[3];
} CardMoveRecord;

typedef struct CardPlayField {
    CardPlayHandViews hand;
    u8 unmodeled_019A[0x0880 - 0x019A];
    CardPlayDeploymentSlot disposal[4];
    u8 unmodeled_0F70[0x0F7C - 0x0F70];
    s16 operations[40];
    s16 operationFlags[40];
    s16 operationDuration[40];
    u8 unmodeled_106C[0x14CC - 0x106C];
    CardMoveRecord operationMove[4];
    u8 unmodeled_153C[0x192C - 0x153C];
    CardMoveRecord handMove[40];
    u8 unmodeled_1D8C[0x21EC - 0x1D8C];
    CardMoveRecord suteMove[40];
    CardMoveRecord junkMove[40];
} CardPlayField;

extern float HandPos1P[4][3];
extern float HandPos2P[4][3];
extern float JunkPos1P[][3];
extern float JunkPos2P[][3];

extern void CGPCursor2Pos(CGPCursorPosition *position, CardGameWork *work);

extern int CardPlayIdentityCheck(CardGameWork *work, CardHand *hand, u16 identity);
extern int CardPlayRecavery(CardHand *hand);
extern s16 *CPCSearchCommWk(u16 side, CardGameWork *work);
extern s32 CardPlayCommandPlay(u16 side, u16 command, CardGameWork *work,
                               s16 operand, s16 target);
extern int CardPlayCntOpe(CardPlaySide *side, int opeId);
extern float DisposePos1P[][3];
extern float OperationPos1P[][3];

/* One line of a card list window: a card id and how many copies it holds. */
typedef struct CardListEntry {
    s16 cardId;
    u16 count;
} CardListEntry;

/*
 * The scrolling card list window CGPCheckTeMax lays out for the hand; it is the
 * window CardMainProc passes to CardGameProc (D_00A573F8), which hands it on to
 * CGPEndFaseProc.
 */
typedef struct CardListWindow {
    s16 active;
    u8 baseColor;                  /* +0x02 */
    u8 cursorColor;                /* +0x03 */
    s8 mode;                       /* +0x04 */
    s8 cursor;                     /* +0x05 */
    s16 x;                         /* +0x06 */
    s16 y;                         /* +0x08 */
    s16 lineHeight;                /* +0x0A */
    s16 top;                       /* +0x0C */
    s16 columns;                   /* +0x0E */
    s16 rows;                      /* +0x10 */
    s16 count;                     /* +0x12: entries in use */
    CardListEntry entries[151];    /* +0x14 */
} CardListWindow;

/* Cards a hand holds at most before one must be discarded (`slti $2,$8,7`, 0x00a250d4). */
#define CGP_HAND_LIMIT 7

/* Layout CGPCheckTeMax gives the hand's card list window (stores at 0x00a250f8..0x00a25118). */
#define CGP_HAND_LIST_BASE_COLOR 7
#define CGP_HAND_LIST_CURSOR_COLOR 15
#define CGP_HAND_LIST_X 0x150
#define CGP_HAND_LIST_Y 0x30
#define CGP_HAND_LIST_LINE_HEIGHT 14
#define CGP_HAND_LIST_ROWS 10

/*
 * CardGameWork.flags bits that mark a side as not controlled by the player at
 * the pad: CardPlayDispInfo tests 0x40 for side 0 and 0x20 for the other side
 * (0x00a23120 `andi $2,$2,0x40`, 0x00a2312c `andi $2,$2,0x20`), and the card
 * game start for a match against the computer sets 0x20.
 */
#define CGP_FLAG_SIDE1_AUTO 0x20
#define CGP_FLAG_SIDE0_AUTO 0x40

/* Phases of a turn: 0..6 belong to side 0 and 7..13 to the other side. */
#define CGP_PHASE_COUNT 14           /* `slti $2,$3,0xE`, 0x00a25a74 */
#define CGP_SIDE1_FIRST_PHASE 7      /* `addiu $2,$0,7`, 0x00a25aa0 */

/* Sound ids CGPNextTurnSub plays. */
#define CGP_SE_NEXT_TURN 0x20001     /* a new turn and the switch to the other side (0x00a25a90, 0x00a25ab4) */
#define CGP_SE_NEXT_PHASE 6          /* every other phase change (`addiu $4,$0,6`, 0x00a25aa8) */

/* Sound ids of the card game's confirmation and refusal feedback (xglSoundEffectNormalID). */
#define CGP_SE_DECIDE 1
#define CGP_SE_BUZZER 5
#define CGP_SE_CANCEL 2
#define CGP_SE_CURSOR 3

/* Confirm button (circle) in a controller's pressed halfword (`andi $2,$17,0x20`, 0x00a28070). */
#define CGP_PAD_CONFIRM 0x0020
#define CGP_PAD_PREVIEW 0x0010       /* triangle, held (`andi $2,$23,0x10`, 0x00a313c8) */
#define CGP_PAD_CANCEL 0x0040        /* cross (`andi $2,$21,0x40`, 0x00a314d0) */
#define CGP_PAD_RIGHT 0x2000         /* `andi $2,$21,0x2000`, 0x00a31364 */
#define CGP_PAD_LEFT 0x8000          /* `andi $2,$21,0x8000`, 0x00a31398 */

/* Error message id CGPDispoSub0 and CGPDisposeSub use for "play points fall short" (CGPSetErrorMess reason 12). */
#define CGP_ERR_NOT_ENOUGH_POINTS 12

/* Cards 7, 9 and 10..13 are paired with the card seven ids below them (`addiu $8,$5,-0x7`, 0x00a2817c). */
#define CGP_PAIRED_CARD_OFFSET 7

/*
 * Board areas of the cursor, as CGPCursor2Pos reports them in its zone field
 * (the cases that index disposal and battle slots): each side's disposal row
 * and battle row.
 */
#define CGP_AREA_ENEMY_HAND 0
#define CGP_AREA_ENEMY_DISPOSAL 2
#define CGP_AREA_ENEMY_BATTLE 4
#define CGP_AREA_PLAYER_BATTLE 5
#define CGP_AREA_PLAYER_DISPOSAL 6
#define CGP_AREA_PLAYER_HAND 9

/* Frames the interrupt message of a new phase stays up (`addiu $3,$0,0x2D`, 0x00a25ad4). */
#define CGP_INTERRUPT_MESS_FRAMES 45

extern void CardPlayCleanJunk(CardPlaySide *side);
extern void CGCPJunkDispSub(CardPlaySide *side, s32 index);
extern float *CursorTbl_00A49B58[];

#define CGP_PAD_UP 0x1000            /* `andi $2,$18,0x1000`, 0x00a268c0 */
#define CGP_PAD_DOWN 0x4000          /* `andi $2,$18,0x4000`, 0x00a26ba8 */
#define CGP_PAD_DIRECTIONS 0xF000    /* `andi $2,$18,0xF000`, 0x00a26b50 */
#define CGP_PAD_ZOOM_LEFT 0x0004     /* held: `andi $2,$21,0x4`, 0x00a27148 */
#define CGP_PAD_ZOOM_RIGHT 0x0008    /* held: `andi $2,$21,0x8`, 0x00a27134 */
extern void CardCursorPassive(CardCursorControl *cursor);
extern u8 HelpPageList[14];
extern s32 CGPCursor2Area(CardGameWork *work);
extern void CardDisp(Matrix4 matrix, s16 kind, s16 flagA, s16 flagB);
extern void xglMatrixUnit(float matrix[4][4]);
extern void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                           const float position[4]);
extern void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                           const float scale[4]);
extern void nmlModelSetLight(float color[4][4], float direction[4][4]);
extern void nmlModelSetPlace(float matrix[4][4]);
extern void nmlModelSetTexture(void *texture);
extern void nmlModelSetToumei(s32 enabled);
extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetAlpha(u64 alpha);
extern void nmlModelEntry(s32 model);
extern void nmlModelEntryCard(void *model);
extern float asColor[4][4];
extern float asDir[4][4];
extern float CLP1Color[4][4];
extern float CLP2Color[4][4];
extern const char D_00A4E530[];
extern const char D_00A4E538[];

/*
 * Shunt one side's cards aside before the computer opponent moves
 * (CardEnemyMove, ov10 0x00a32570, passes it the player's or the enemy's
 * CardPlaySide). Every occupied slot hands its effect position to the shunt
 * position one slot record further on, the disposal board first and the
 * battle board after it; the last disposal slot writes CardPlaySide's
 * lastShuntPosition, the record that follows the disposal board.
 */
void CGPShuntPosSub(CardPlaySide *side)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (side->disposal[i].battle.layers[0].cardId >= 0) {
            side->disposal[i + 1].shuntPosition[0] = side->disposal[i].battle.position[0];
            side->disposal[i + 1].shuntPosition[1] = side->disposal[i].battle.position[1];
            side->disposal[i + 1].shuntPosition[2] = side->disposal[i].battle.position[2];
        }
    }

    for (i = 0; i < 4; i++) {
        if (side->battle[i].battle.layers[0].cardId >= 0) {
            side->battle[i + 1].shuntPosition[0] = side->battle[i].battle.position[0];
            side->battle[i + 1].shuntPosition[1] = side->battle[i].battle.position[1];
            side->battle[i + 1].shuntPosition[2] = side->battle[i].battle.position[2];
        }
    }
}

extern s32 CardPlayHandCnt(CardHand *hand);
extern s32 CardPlayCntBtl(CardHand *hand, s16 cardId);

/*
 * Where the cards in play are moving to after the board shifted. Each occupied
 * slot keeps the point it was heading for, takes the shunt position of the slot
 * after it as its new effect position, and covers what is left of the way in ten
 * steps.
 */
void CGPRecalcPosSub(CardPlaySide *side)
{
    CardLayerStack *stack;
    float dest[3];
    s32 i;

    for (i = 0; i < 4; i++) {
        stack = &side->disposal[i].battle;
        if (stack->layers[0].cardId >= 0) {
            dest[0] = stack->position[0] + stack->step[0] * stack->moveFrames;
            dest[1] = stack->position[1] + stack->step[1] * stack->moveFrames;
            dest[2] = stack->position[2] + stack->step[2] * stack->moveFrames;
            stack->position[0] = side->disposal[i + 1].shuntPosition[0];
            stack->position[1] = side->disposal[i + 1].shuntPosition[1];
            stack->position[2] = side->disposal[i + 1].shuntPosition[2];
            stack->step[0] = (dest[0] - stack->position[0]) / 10.0f;
            stack->step[1] = (dest[1] - stack->position[1]) / 10.0f;
            stack->step[2] = (dest[2] - stack->position[2]) / 10.0f;
        }
    }
    for (i = 0; i < 4; i++) {
        stack = &side->battle[i].battle;
        if (stack->layers[0].cardId >= 0) {
            dest[0] = stack->position[0] + stack->step[0] * stack->moveFrames;
            dest[1] = stack->position[1] + stack->step[1] * stack->moveFrames;
            dest[2] = stack->position[2] + stack->step[2] * stack->moveFrames;
            stack->position[0] = side->battle[i + 1].shuntPosition[0];
            stack->position[1] = side->battle[i + 1].shuntPosition[1];
            stack->position[2] = side->battle[i + 1].shuntPosition[2];
            stack->step[0] = (dest[0] - stack->position[0]) / 10.0f;
            stack->step[1] = (dest[1] - stack->position[1]) / 10.0f;
            stack->step[2] = (dest[2] - stack->position[2]) / 10.0f;
        }
    }
}

extern void CardSetEffect(s32 side, CardGameWork *work, u8 code, s8 value, s32 option,
                          float *position, CardLayerStack *stack);
extern void xglSoundEffectNormalID(int sound_id, int variant);

/*
 * Command 143 takes two life off every card standing on a battle or disposal
 * slot of either side, clamped at zero, and starts effects 5 and 13 at each of
 * those stacks, the card's own side for its own cards and the other side for
 * the opponent's.
 */
void CCO143ExecSub(s32 side, CardGameWork *work, CardPlaySide *ownSide,
                   CardPlaySide *foeSide)
{
    s32 otherSide;
    s32 i;

    xglSoundEffectNormalID(0x20008, 0);
    otherSide = (side == 0);
    for (i = 0; i < 4; i++) {
        if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
            ownSide->battle[i].battle.layers[0].life -= 2;
            if (ownSide->battle[i].battle.layers[0].life < 0) {
                ownSide->battle[i].battle.layers[0].life = 0;
            }
            CardSetEffect(side, work, 5, 0, 0, ownSide->battle[i].battle.position, 0);
            CardSetEffect(side, work, 13, 2, 0, ownSide->battle[i].battle.position,
                          &ownSide->battle[i].battle);
        }
        if (foeSide->battle[i].battle.layers[0].cardId >= 0) {
            foeSide->battle[i].battle.layers[0].life -= 2;
            if (foeSide->battle[i].battle.layers[0].life < 0) {
                foeSide->battle[i].battle.layers[0].life = 0;
            }
            CardSetEffect(otherSide, work, 5, 0, 0, foeSide->battle[i].battle.position, 0);
            CardSetEffect(otherSide, work, 13, 2, 0, foeSide->battle[i].battle.position,
                          &foeSide->battle[i].battle);
        }
    }
    for (i = 0; i < 4; i++) {
        if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
            ownSide->disposal[i].battle.layers[0].life -= 2;
            if (ownSide->disposal[i].battle.layers[0].life < 0) {
                ownSide->disposal[i].battle.layers[0].life = 0;
            }
            CardSetEffect(side, work, 5, 0, 0, ownSide->disposal[i].battle.position, 0);
            CardSetEffect(side, work, 13, 2, 0, ownSide->disposal[i].battle.position,
                          &ownSide->disposal[i].battle);
        }
        if (foeSide->disposal[i].battle.layers[0].cardId >= 0) {
            foeSide->disposal[i].battle.layers[0].life -= 2;
            if (foeSide->disposal[i].battle.layers[0].life < 0) {
                foeSide->disposal[i].battle.layers[0].life = 0;
            }
            CardSetEffect(otherSide, work, 5, 0, 0, foeSide->disposal[i].battle.position, 0);
            CardSetEffect(otherSide, work, 13, 2, 0, foeSide->disposal[i].battle.position,
                          &foeSide->disposal[i].battle);
        }
    }
}

extern int printf(const char *format, ...);

void CGPPrintYama(CardPlaySide *side)
{
    s32 col;
    s32 i;

    col = 0;
    for (i = 0; i < 40; i++) {
        if (col == 0) {
            printf(D_00A4E4F0, i);
        }
        col++;
        printf(D_00A4E500, side->deck[i]);
        if (col == 10) {
            col = 0;
            printf(D_00A4E508);
        }
    }
    printf(D_00A4E508);
}

void CGPPrintJunk(CardPlaySide *side)
{
    s32 col;
    s32 i;

    col = 0;
    for (i = 0; i < 40; i++) {
        if (col == 0) {
            printf(D_00A4E510, i);
        }
        col++;
        printf(D_00A4E500, side->junk[i]);
        if (col == 10) {
            col = 0;
            printf(D_00A4E508);
        }
    }
    printf(D_00A4E508);
}





void CGPPrintSute(CardPlaySide *side)
{
    s32 col;
    s32 i;

    col = 0;
    for (i = 0; i < 40; i++) {
        if (col == 0) {
            printf(D_00A4E520, i);
        }
        col++;
        printf(D_00A4E500, side->sute[i]);
        if (col == 10) {
            col = 0;
            printf(D_00A4E508);
        }
    }
    printf(D_00A4E508);
}

void CGPSetMessage(CardGameWork *work, s32 index)
{
    work->mess = OLMessTbl[index];
}

void CGPSetInterruptMess(CardGameWork *work, s32 index)
{
    work->interruptMess = CardPlayTMessList[index];
}

void CGPSetPermanentMess(CardGameWork *work, s32 index)
{
    if (index == 0) {
        work->permanentMess = 0;
        return;
    }
    work->permanentMess = PermMessTbl[index];
}

void CGPSetErrorMess(s32 side, CardGameWork *work, s32 reason)
{
    work->errorMessSide = side;
    work->errorMessKind = 1;
    work->errorMessReason = reason;
    work->errorMessValue = -1;
}

void CGPSetErrorMessPlus(s32 side, CardGameWork *work, u8 reason, s16 value)
{
    work->errorMessSide = side;
    work->flags |= CGP_FLAG_ERROR_PLUS;
    work->errorMessKind = 3;
    work->errorMessReason = reason;
    work->errorMessValue = value;
}

extern char *ErrorMessTbl[][5];

/* Text colour of the card game's font strings (`ori $6,$0,0xFFF0`, 0x00a21efc and 0x00a2ddc8). */
#define CGP_TEXT_COLOR 0xFFF0

/* Vertical distance between the text lines of one error message (`addiu $16,$20,0x18`, 0x00a21f7c). */
#define CGP_ERROR_MESS_LINE_PITCH 24

/* Fixed EE runtime addresses of the message window texture and model (`lui $4,0x1DB5` + `ori $4,$4,0x5800`, 0x00a22140..0x00a22148;
 * `lui $4,0x1DA2` + `ori $4,$4,0x2B00`, 0x00a2214c..0x00a22154). */
#define CGP_ERROR_MESS_TEXTURE 0x01DB5800
#define CGP_ERROR_MESS_MODEL 0x01DA2B00

/*
 * Draws the message of one error reason: the lines of its ErrorMessTbl row
 * (at most five, each printed between the font control strings D_00A4E530 and
 * D_00A4E538 as CardPlayDispInfo does for the phase message), then the message
 * window model at the given position (x, y, depth, w = 1), scaled by 1.35
 * across and lit for the side that caused the message (a negative side keeps
 * the current light). The model and its texture sit at fixed EE runtime addresses.
 */
void CGPDispErrorMessCore(s32 side, u16 reason, u16 x, u16 y, const float position[4])
{
    float matrix[4][4];
    float scale[4];

    xglFontPrint(x, y, CGP_TEXT_COLOR, D_00A4E530);
    xglFontPrint(0, 0, 0, D_00A4E538);
    xglFontPrint(x, y, CGP_TEXT_COLOR, ErrorMessTbl[reason][0]);
    if (ErrorMessTbl[reason][1] != 0) {
        xglFontPrint(x, y + 1 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, D_00A4E530);
        xglFontPrint(0, 0, 0, D_00A4E538);
        xglFontPrint(x, y + 1 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][1]);
    }
    if (ErrorMessTbl[reason][2] != 0) {
        xglFontPrint(x, y + 2 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, D_00A4E530);
        xglFontPrint(0, 0, 0, D_00A4E538);
        xglFontPrint(x, y + 2 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][2]);
    }
    if (ErrorMessTbl[reason][3] != 0) {
        xglFontPrint(x, y + 3 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, D_00A4E530);
        xglFontPrint(0, 0, 0, D_00A4E538);
        xglFontPrint(x, y + 3 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][3]);
    }
    if (ErrorMessTbl[reason][4] != 0) {
        xglFontPrint(x, y + 4 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, D_00A4E530);
        xglFontPrint(0, 0, 0, D_00A4E538);
        xglFontPrint(x, y + 4 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][4]);
    }
    xglMatrixUnit(matrix);
    xglMatrixTrans(matrix, (const float (*)[4]) matrix, position);
    scale[0] = 1.35f;
    scale[1] = scale[2] = scale[3] = 1.0f;
    xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
    nmlModelSetPlace(matrix);
    if (side >= 0) {
        if (side == 0) {
            nmlModelSetLight(Player1Color, asDir);
        } else {
            nmlModelSetLight(Player2Color, asDir);
        }
    }
    nmlModelSetTexture((void *) CGP_ERROR_MESS_TEXTURE);
    nmlModelEntryCard((void *) CGP_ERROR_MESS_MODEL);
}

void CGPDispErrorMess3(s32 side, s32 reason)
{
    float position[4];

    position[0] = 1.1f;
    position[1] = 0.0f;
    position[2] = 0.5f;
    position[3] = 1.0f;
    CGPDispErrorMessCore(side, reason, 230, 164, position);
}

/*
 * Shows the message of CGPDispErrorMess3 (the right-hand message line) and, in
 * the lower left corner of that line, the card the message is about, at 0.8 scale.
 * CardDisp's last two arguments are the card's "owned" flag (1) and its display
 * mode (5); the other callers of CardDisp that draw a card icon pass 1 and 0.
 */
void CGPDispErrorMessPlus(s32 side, u16 reason, s32 cardId)
{
    float matrix[4][4];
    float vec[4];

    CGPDispErrorMess3(side, reason);
    xglMatrixUnit(matrix);
    vec[0] = -1.5f;
    vec[1] = 0.2f;
    vec[2] = 0.3f;
    vec[3] = 1.0f;
    xglMatrixTrans(matrix, (const float (*)[4]) matrix, vec);
    vec[0] = vec[1] = 0.8f;
    vec[2] = 1.0f;
    xglMatrixScale(matrix, (const float (*)[4]) matrix, vec);
    CardDisp(matrix, cardId, 1, 5);
}

void CGPDispErrorMess(s32 side, s32 reason)
{
    float position[4];

    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = 0.5f;
    position[3] = 1.0f;
    CGPDispErrorMessCore(side, reason, 124, 164, position);
}

void CGPDispErrorMess2(s32 side, s32 reason)
{
    float position[4];

    position[0] = 0.0f;
    position[1] = 0.48f;
    position[2] = 0.5f;
    position[3] = 1.0f;
    CGPDispErrorMessCore(side, reason, 124, 116, position);
}

/*
 * Draws everything the card game shows beside the board: the message line, the
 * error message of the side that caused it, the information panel of the phase
 * being played with the turn counter on it, and the seven phase-guide models of
 * the side whose phase is running. Phases 0..6 belong to the player and 7..13
 * to the opponent, and a negative phase means no side is playing.
 */
void CardPlayDispInfo(CardGameWork *workAddress)
{
    CardGameWork *work = workAddress;
    float place[4][4];
    float guidePlace[4][4];
    float shadowPos[4];
    float pos[4];
    float guideScale[4];
    float color1P[4][4];
    float color2P[4][4];
    float turnColor[4][4];
    char *text;
    s32 flag;
    void *model;
    s32 area;
    float transparency;
    s32 tens;
    s32 i;

    text = work->mess;
    if (work->interruptMess != 0) {
        text = work->interruptMess;
    }
    if (work->permanentMess != 0) {
        text = work->permanentMess;
    }

    if (work->errorMessKind != 0) {
        s32 bothSides;

        bothSides = 0;
        if (work->errorMessSide == 0) {
            if (work->flags & 0x40) {
                bothSides = 1;
            }
        } else {
            if (work->flags & 0x20) {
                bothSides = 1;
            }
        }
        if (bothSides == 0) {
            if (work->errorMessKind & CGP_FLAG_ERROR_PLUS) {
                CGPDispErrorMessPlus(work->errorMessSide, work->errorMessReason,
                                     work->errorMessValue);
            } else {
                CGPDispErrorMess(work->errorMessSide,
                                 work->errorMessReason);
            }
        } else if (work->errorMessKind & CGP_FLAG_ERROR_PLUS) {
            CGPDispErrorMessPlus(-1, work->errorMessReason, work->errorMessValue);
        } else {
            CGPDispErrorMess(-1, work->errorMessReason);
        }
    }

    flag = 1;
    if (work->infoPanelPhase == 1) {
        flag = work->resultWait < 4;
    }
    switch ((s16) work->command.phase) {
    case 2:
    case 4:
    case 9:
    case 11:
        if ((u32) (work->command.phaseTimer - 20) < 10) {
            flag = 0;
        }
        break;
    }
    if (flag && text != 0) {
        xglFontPrint(2, 2, CGP_TEXT_COLOR, D_00A4E530);
        xglFontPrint(0, 0, 0, D_00A4E538);
        xglFontPrint(2, 2, CGP_TEXT_COLOR, text);
    }

    model = CPISMdlLst[(s16) work->command.phase];
    flag = 1;
    if ((s16) work->command.phase < 7) {
        if ((s16) work->command.phase >= 0) {
            flag = 0;
        }
    }
    switch (work->infoPanelPhase) {
    case 0:
    default:
        work->infoPanelPos[0] = 0.0f;
        work->infoPanelPos[1] = 0.0f;
        work->infoPanelPos[2] = 0.5f;
        work->infoPanelPos[3] = 1.0f;
        work->infoPanelPosStep[0] = 0.0f;
        work->infoPanelPosStep[1] = 0.0f;
        work->infoPanelPosStep[2] = 0.0f;
        work->infoPanelPosStep[3] = 1.0f;
        work->infoPanelScale[0] = 20.0f;
        work->infoPanelScale[1] = 20.0f;
        work->infoPanelScale[2] = 1.0f;
        work->infoPanelScale[3] = 1.0f;
        work->infoPanelScaleStep[0] = -1.9f;
        work->infoPanelScaleStep[1] = -1.9f;
        work->infoPanelScaleStep[2] = 0.0f;
        work->infoPanelScaleStep[3] = 1.0f;
        work->resultWait = 10;
        work->infoPanelPhase = 1;
        break;
    case 1:
        if (work->resultWait <= 0) {
            work->resultWait = 15;
            work->infoPanelPhase++;
        } else {
            work->resultWait--;
            work->infoPanelScale[0] += work->infoPanelScaleStep[0];
            work->infoPanelScale[1] += work->infoPanelScaleStep[1];
            work->infoPanelScale[2] += work->infoPanelScaleStep[2];
            work->infoPanelPos[0] += work->infoPanelPosStep[0];
            work->infoPanelPos[1] += work->infoPanelPosStep[1];
            work->infoPanelPos[2] += work->infoPanelPosStep[2];
        }
        break;
    case 2:
        if (work->resultWait <= 0) {
            work->infoPanelPhase++;
            work->resultWait = 6;
            if ((s16) work->command.phase < 7) {
                if ((s16) work->command.phase >= 0) {
                    work->infoPanelPosStep[0] = -0.25833334f;
                    work->infoPanelPosStep[1] = 0.27166668f;
                } else {
                    work->infoPanelPosStep[0] = 0.25833334f;
                    work->infoPanelPosStep[1] = -0.305f;
                }
            } else {
                work->infoPanelPosStep[0] = 0.25833334f;
                work->infoPanelPosStep[1] = -0.305f;
            }
            work->infoPanelScaleStep[0] = -0.083333336f;
            work->infoPanelPosStep[2] = -0.033333333f;
            work->infoPanelScaleStep[1] = -0.083333336f;
        } else {
            work->resultWait--;
        }
        break;
    case 3:
        if (work->resultWait <= 0) {
            work->infoPanelPhase++;
        } else {
            work->resultWait--;
            work->infoPanelPos[0] += work->infoPanelPosStep[0];
            work->infoPanelPos[1] += work->infoPanelPosStep[1];
            work->infoPanelPos[2] += work->infoPanelPosStep[2];
            work->infoPanelScale[0] += work->infoPanelScaleStep[0];
            work->infoPanelScale[1] += work->infoPanelScaleStep[1];
            work->infoPanelScale[2] += work->infoPanelScaleStep[2];
        }
        break;
    case 4:
        if (work->resultWait > 0) {
            work->resultWait--;
            work->infoPanelPos[0] += work->infoPanelPosStep[0];
            work->infoPanelPos[1] += work->infoPanelPosStep[1];
        }
        break;
    }

    if (((s16) work->command.phase == 5 || (s16) work->command.phase == 12) &&
        work->infoPanelPhase >= 4 && (s16) work->command.phaseTimer >= 10) {
        return;
    }
    if (work->command.flowRequest == 3) {
        return;
    }
    if (work->command.flowRequest == 4) {
        return;
    }
    if (work->command.flowRequest == 5) {
        return;
    }

    xglMatrixUnit(place);
    xglMatrixTrans(place, (const float (*)[4]) place, work->infoPanelPos);
    xglMatrixScale(place, (const float (*)[4]) place, work->infoPanelScale);
    nmlModelSetLight(asColor, asDir);
    nmlModelSetPlace(place);
    nmlModelSetTexture((void *) 0x01F70200);
    if (work->infoPanelPhase == 1) {
        transparency = 1.0f;
        transparency -= (work->resultWait * 0.3f) / 10.0f;
        nmlModelSetToumei(1);
        nmlModelSetTransparency(transparency);
    }
    nmlModelEntryCard(model);

    memset(color1P, 0, sizeof(color1P));
    color1P[0][3] = 1.0f;
    color1P[1][3] = 1.0f;
    color1P[2][3] = 1.0f;
    color1P[3][0] = 0.5f;
    color1P[3][1] = 0.5f;
    color1P[3][2] = 1.0f;
    color1P[3][3] = 1.0f;
    memset(color2P, 0, sizeof(color2P));
    color2P[0][3] = 1.0f;
    color2P[1][3] = 1.0f;
    color2P[2][3] = 1.0f;
    color2P[3][0] = 1.0f;
    color2P[3][1] = 0.4f;
    color2P[3][2] = 0.4f;
    color2P[3][3] = 1.0f;
    memset(turnColor, 0, sizeof(turnColor));
    turnColor[0][3] = 1.0f;
    turnColor[1][3] = 1.0f;
    turnColor[2][3] = 1.0f;
    turnColor[3][0] = 1.0f;
    turnColor[3][1] = 0.9f;
    turnColor[3][2] = 0.4f;
    turnColor[3][3] = 1.0f;

    xglMatrixUnit(place);
    shadowPos[0] = work->infoPanelPos[0];
    shadowPos[1] = work->infoPanelPos[1];
    shadowPos[2] = work->infoPanelPos[2] - 0.01f;
    shadowPos[3] = 1.0f;
    xglMatrixTrans(place, (const float (*)[4]) place, shadowPos);
    xglMatrixScale(place, (const float (*)[4]) place, work->infoPanelScale);
    if (flag == 0) {
        nmlModelSetLight(color1P, asDir);
    } else {
        nmlModelSetLight(color2P, asDir);
    }
    nmlModelSetPlace(place);
    nmlModelSetTexture((void *) 0x01DB5800);
    if (work->infoPanelPhase == 1) {
        transparency = 1.0f;
        transparency -= (work->resultWait * 0.3f) / 10.0f;
        nmlModelSetToumei(1);
        nmlModelSetTransparency(transparency);
    }
    nmlModelEntry(0x01DAD700);

    pos[0] = 0.0f;
    pos[1] = 1.72f;
    pos[2] = 0.0f;
    pos[3] = 1.0f;
    xglMatrixUnit(place);
    xglMatrixTrans(place, (const float (*)[4]) place, pos);
    nmlModelSetLight(turnColor, asDir);
    nmlModelSetPlace(place);
    nmlModelSetTexture((void *) 0x01DB5800);
    nmlModelEntryCard((void *) 0x01DB2D00);

    pos[0] = 0.0f;
    pos[1] = -0.02f;
    pos[2] = 0.05f;
    pos[3] = 1.0f;
    xglMatrixTrans(place, (const float (*)[4]) place, pos);
    nmlModelSetPlace(place);
    nmlModelSetTexture((void *) 0x01F70200);
    nmlModelEntryCard((void *) 0x01921800);

    tens = (work->command.turn + 1) / 10;
    if (tens != 0) {
        model = (void *) (0x01C80000 + (tens << 10));
        pos[0] = 0.213f;
        pos[1] = -0.003f;
        pos[2] = 0.0f;
        xglMatrixTrans(guidePlace, (const float (*)[4]) place, pos);
        nmlModelSetPlace(guidePlace);
        nmlModelSetTexture((void *) 0x01C91000);
        nmlModelEntryCard(model);
    }
    model = (void *) (0x01C80000 + (((work->command.turn + 1) % 10) << 10));
    pos[0] = 0.311f;
    pos[1] = -0.003f;
    pos[2] = 0.0f;
    xglMatrixTrans(guidePlace, (const float (*)[4]) place, pos);
    nmlModelSetPlace(guidePlace);
    nmlModelSetTexture((void *) 0x01C91000);
    nmlModelEntryCard(model);

    pos[0] = -2.3f;
    pos[2] = 0.0f;
    pos[3] = 1.0f;
    guideScale[0] = 0.5f;
    guideScale[1] = 0.5f;
    guideScale[2] = 1.0f;
    guideScale[3] = 1.0f;
    if (work->save->flags & 0x02) {
        return;
    }

    if ((s16) work->command.phase < 7) {
        area = CGPCursor2Area(workAddress);
        pos[1] = 1.53f;
        if (work->rotStageId == 0) {
            if (area == 3 || work->cursorMoveStep[1] != 0) {
                pos[1] = -0.31000006f;
            }
        } else {
            pos[1] = -0.31000006f;
            if (area == 2) {
                pos[1] = 1.53f;
            }
        }
        xglMatrixUnit(place);
        for (i = 0; i < 7; i++) {
            model = CPIPhaseMdlLst[i];
            xglMatrixTrans(guidePlace, (const float (*)[4]) place, pos);
            xglMatrixScale(guidePlace, (const float (*)[4]) guidePlace, guideScale);
            nmlModelSetPlace(guidePlace);
            nmlModelSetLight(asColor, asDir);
            nmlModelSetTexture((void *) 0x01F70200);
            nmlModelEntryCard(model);
            xglMatrixTrans(guidePlace, (const float (*)[4]) place, pos);
            xglMatrixScale(guidePlace, (const float (*)[4]) guidePlace, guideScale);
            if (i == 4) {
                if ((s16) work->command.phase == i) {
                    nmlModelSetLight(CLP2Color, asDir);
                } else {
                    nmlModelSetLight(color2P, asDir);
                }
            } else if ((s16) work->command.phase == i) {
                nmlModelSetLight(CLP1Color, asDir);
            } else {
                nmlModelSetLight(color1P, asDir);
            }
            nmlModelSetPlace(guidePlace);
            nmlModelSetTexture((void *) 0x01DB5800);
            nmlModelEntryCard((void *) 0x01DB0200);
            pos[1] -= 0.23f;
        }
    } else {
        pos[0] = 2.3f;
        area = CGPCursor2Area(workAddress);
        pos[1] = -0.31000006f;
        if (work->rotStageId == 0) {
            if (area == 7 || work->cursorMoveStep[0] != 0) {
                pos[1] = 1.53f;
            }
        } else {
            pos[1] = 1.53f;
            if (area == 6) {
                pos[1] = -0.31000006f;
            }
        }
        xglMatrixUnit(place);
        for (i = 0; i < 7; i++) {
            model = CPIPhaseMdlLst[i];
            xglMatrixTrans(guidePlace, (const float (*)[4]) place, pos);
            xglMatrixScale(guidePlace, (const float (*)[4]) guidePlace, guideScale);
            nmlModelSetPlace(guidePlace);
            nmlModelSetLight(asColor, asDir);
            nmlModelSetTexture((void *) 0x01F70200);
            nmlModelEntryCard(model);
            xglMatrixTrans(guidePlace, (const float (*)[4]) place, pos);
            xglMatrixScale(guidePlace, (const float (*)[4]) guidePlace, guideScale);
            if (i == 4) {
                if ((s16) work->command.phase - 7 == i) {
                    nmlModelSetLight(CLP1Color, asDir);
                } else {
                    nmlModelSetLight(color1P, asDir);
                }
            } else if ((s16) work->command.phase - 7 == i) {
                nmlModelSetLight(CLP2Color, asDir);
            } else {
                nmlModelSetLight(color2P, asDir);
            }
            nmlModelSetPlace(guidePlace);
            nmlModelSetTexture((void *) 0x01DB5800);
            nmlModelEntryCard((void *) 0x01DB0200);
            pos[1] -= 0.23f;
        }
    }
}

/*
 * Whether the Square request for the confirm prompt (flowRequest 1) is allowed
 * in the current phase; CardGameProc asks before it sets it (0x00a31ca8,
 * 0x00a31cec). Phases 0, 5, 6, 7, 12 and 13 never allow it and a phase outside
 * 0..13 always does. Phases 1 and 8 refuse it when their flag is set; phases 2,
 * 3, 4, 9, 10 and 11 refuse it then too and also once the phase timer has
 * reached 3.
 */
s32 CGPChkFaseLock(CardGameWork *work)
{
    s32 allowed;

    allowed = 1;
    switch ((s16) work->command.phase) {
    case 0:
    case 5:
    case 6:
    case 7:
    case 12:
    case 13:
        allowed = 0;
        break;
    case 1:
        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
            allowed = 0;
        }
        break;
    case 8:
        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
            allowed = 0;
        }
        break;
    case 2:
    case 3:
    case 11:
        if ((work->flags & CGP_FLAG_SIDE0_AUTO) || (s16) work->command.phaseTimer >= 3) {
            allowed = 0;
        }
        break;
    case 4:
    case 9:
    case 10:
        if ((work->flags & CGP_FLAG_SIDE1_AUTO) || (s16) work->command.phaseTimer >= 3) {
            allowed = 0;
        }
        break;
    }
    return allowed;
}

/*
 * Register one effect of the card game. The first entry of the work area's
 * effect table whose code is zero is taken, so nothing happens once all 64
 * are busy. position is where the effect starts (NULL leaves the start each
 * code picks for itself), stack the board slot it travels to.
 */
void CardSetEffect(s32 side, CardGameWork *workAddress, u8 code, s8 value, s32 soundId,
                   float *position, CardLayerStack *stack)
{
    CardGameWork *work = workAddress;
    CardEffect *effect;
    s32 i;

    effect = 0;
    for (i = 0; i < 64; i++) {
        if (work->effects[i].code == 0) {
            effect = &work->effects[i];
            break;
        }
    }
    if (effect == 0) {
        return;
    }

    effect->side = side;
    effect->timer = 15;
    effect->soundId = soundId;
    effect->phase = 0;
    effect->code = code;
    effect->stack = 0;
    if (position != 0) {
        effect->position[0] = position[0];
        effect->position[1] = position[1];
        effect->position[2] = position[2] + 0.1f;
    }
    effect->scale[0] = effect->scale[1] = effect->scale[2] = 1.0f;
    effect->velocity[0] = effect->velocity[1] = effect->velocity[2] = 0.0f;
    effect->scale[3] = 1.0f;
    effect->velocity[3] = 1.0f;
    effect->position[3] = 1.0f;

    switch (code) {
    case 10:
        xglSoundEffectNormalID(0x2000D, 0);
        if (position == 0) {
            if (value == 0) {
                effect->position[1] = -1.81f;
            } else {
                effect->position[1] = 1.81f;
            }
            effect->position[2] = 0.1f;
        }
        effect->timer = 30;
        if (stack != 0) {
            effect->stack = stack;
            effect->velocity[0] = (-stack->position[0] - position[0]) / 15.0f;
            effect->velocity[1] = (-stack->position[1] - position[1]) / 15.0f;
        }
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    case 11:
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value != 0) {
            if (value >= 100) {
                value = 99;
            }
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        } else {
            effect->code = 0;
        }
        break;
    case 15:
        effect->timer = 24;
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value != 0) {
            if (value >= 100) {
                value = 99;
            }
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        } else {
            effect->code = 0;
        }
        break;
    case 16:
        effect->timer = 33;
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value != 0) {
            if (value >= 100) {
                value = 99;
            }
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        } else {
            effect->code = 0;
        }
        break;
    case 12:
        effect->stack = stack;
        effect->code = 12;
        effect->position[2] = 0.1f;
        effect->phase = 0;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value >= 0) {
            if (value == 0) {
                effect->code = 0;
            } else {
                effect->value = value;
                effect->velocity[1] = 0.0053333333f;
            }
        } else {
            effect->value = -value;
            effect->velocity[1] = -0.0033333334f;
        }
        break;
    case 13:
        effect->stack = stack;
        effect->phase = 1;
        effect->code = 12;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value >= 0) {
            if (value == 0) {
                effect->code = 0;
            } else {
                effect->value = value;
                effect->velocity[1] = 0.0053333333f;
            }
        } else {
            effect->value = -value;
            effect->velocity[1] = -0.0033333334f;
        }
        break;
    case 14:
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        if (value == 0) {
            effect->code = 0;
        } else {
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        }
        break;
    case 5:
        if (effect->soundId != 0) {
            xglSoundEffectNormalID(effect->soundId, 0);
        }
        if (position == 0) {
            effect->position[1] = 1.81f;
            effect->position[2] = 0.15f;
            effect->position[0] = 0.0f;
        }
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    case 25:
        if (effect->soundId != 0) {
            xglSoundEffectNormalID(effect->soundId, 0);
        }
        if (position == 0) {
            effect->position[1] = 1.81f;
            effect->position[2] = 0.15f;
            effect->position[0] = 0.0f;
        }
        effect->position[0] -= 0.125f;
        effect->position[1] -= 0.125f;
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    default:
        if (effect->soundId != 0) {
            xglSoundEffectNormalID(effect->soundId, 0);
        }
        if (stack != 0) {
            if (code == 23) {
                effect->code = 3;
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (0.7885f - effect->position[1]) / 15.0f;
            } else if (code == 33) {
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (0.7885f - effect->position[1]) / 15.0f;
            }
            effect->stack = stack;
            effect->velocity[0] = (-stack->position[0] - position[0]) / 15.0f;
            effect->velocity[1] = (-stack->position[1] - position[1]) / 15.0f;
        } else {
            switch (code) {
            case 2:
            case 4:
            case 22:
            case 24:
            case 32:
            case 34:
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (1.81f - effect->position[1]) / 15.0f;
                break;
            case 23:
                effect->code = 3;
                /* fallthrough */
            case 33:
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (0.7885f - effect->position[1]) / 15.0f;
                break;
            }
        }
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    }
    work->effectsSet++;
}

/*
 * Resets an effect's transform matrix to identity while preserving its
 * fourth row: CardDispEffect and CardSetEffect keep four color or scale
 * values there, aliased with the row xglMatrixUnit would otherwise clear.
 */
void CDEMatrixSub(float matrix[4][4])
{
    float row3[4];

    row3[0] = matrix[3][0];
    row3[1] = matrix[3][1];
    row3[2] = matrix[3][2];
    row3[3] = matrix[3][3];
    xglMatrixUnit(matrix);
    matrix[3][0] = row3[0];
    matrix[3][1] = row3[1];
    matrix[3][2] = row3[2];
    matrix[3][3] = row3[3];
}

#define CARD_EFFECT_COUNT 64                  /* `slti $2,$23,0x40`, 0x00a2506c */
#define CARD_EFFECT_SE_HIT 0x20008            /* 0x00a23b0c */
#define CARD_EFFECT_SE_BURST 0x2000A
#define CARD_EFFECT_ALPHA 0x48                /* nmlModelSetAlpha argument, 0x00a246ec */
#define CARD_EFFECT_SPRITE_MODEL 0x01A00000   /* entry model of an unscaled effect sprite, 0x00a24a00 */
#define CARD_EFFECT_SCALED_MODEL 0x01A00800   /* entry model after xglMatrixScale, 0x00a246f0 */

/* Animation strips: frame n of a strip is at base + n * frame size (sll/addu chains, 0x00a239d0 and 0x00a23b9c). */
#define CARD_EFFECT_SMALL_FRAME 0x1480        /* 41 * 128 */
#define CARD_EFFECT_LARGE_FRAME 0x1D80        /* 59 * 128 */
#define CARD_EFFECT_FRAME(base, index, size) ((void *) ((base) + (index) * (size)))
#define CARD_EFFECT_STRIP_TRAVEL_1 0x1BF0000  /* 0x00a239e4 */
#define CARD_EFFECT_STRIP_TRAVEL_2 0x1C04000  /* 0x00a23a18 */
#define CARD_EFFECT_STRIP_TRAVEL_3 0x1A14000  /* 0x00a23a4c */
#define CARD_EFFECT_STRIP_TRAVEL_4 0x1A01000  /* 0x00a23a78 */
#define CARD_EFFECT_STRIP_BURST_1 0x1BD8000   /* 0x00a23bac */
#define CARD_EFFECT_STRIP_BURST_2 0x1AA0000   /* 0x00a23c18 */
#define CARD_EFFECT_STRIP_BURST_3 0x1A85000   /* 0x00a23c88 */
#define CARD_EFFECT_STRIP_BURST_4 0x1A67000   /* 0x00a23cfc */
#define CARD_EFFECT_STRIP_FADE 0x1A28000      /* 0x00a23d90 */
#define CARD_EFFECT_STRIP_SWIRL 0x1A40000     /* code 10, 0x00a249b4 */

/* Damage digits: model n of a digit set is at base + (n << 10). */
#define CARD_EFFECT_DIGIT_TEXTURE 0x1C990A0   /* 0x00a24b90 */
#define CARD_EFFECT_DIGITS_FIRST 0x1C8A400    /* code 12 while phase is 0, 0x00a24b60 */
#define CARD_EFFECT_DIGITS 0x1C8CC00          /* 0x00a24b7c */

typedef unsigned int Quadword __attribute__((mode(TI)));

/* The placement matrix as floats, or as four quadwords for the per-effect copy (lq/sq, 0x00a238e8). */
typedef union CardEffectMatrix {
    float m[4][4];
    Quadword q[4];
} CardEffectMatrix;

/*
 * Draws and advances the 64 card-game effects of one side. Each effect starts
 * from a fresh copy of the caller's placement matrix; the matrix is read as
 * volatile, so the copy at the top of the loop is never moved into the loop
 * branch (the original keeps the epilogue `ld $16` there, 0x00a25070).
 */
void CardDispEffect(s32 side, CardGameWork *work, volatile CardEffectMatrix *matrix)
{
    CardEffect *effect;
    s32 i;

    effect = work->effects;
    for (i = 0; i < CARD_EFFECT_COUNT; i++, effect++) {
        CardEffectMatrix local;
        float offset[4];
        CardEffectMatrix quad;
        void *texture;
        s32 digit;
        Quadword row;

        row = matrix->q[0];
        local.q[0] = row;
        row = matrix->q[1];
        local.q[1] = row;
        row = matrix->q[2];
        local.q[2] = row;
        row = matrix->q[3];
        local.q[3] = row;

        switch (effect->code) {
        case 1:
        case 2:
        case 3:
        case 4:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                    switch (effect->code) {
                    case 1:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_1, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    case 2:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_2, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    case 3:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_3, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    default:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_4, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    }
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                    xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
                } else {
                    effect->phase = 1;
                    switch (effect->code) {
                    case 1:
                        effect->timer = 13;
                        xglSoundEffectNormalID(CARD_EFFECT_SE_BURST, 0);
                        break;
                    case 2:
                        xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                        effect->timer = 16;
                        break;
                    case 3:
                        xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                        effect->timer = 11;
                        break;
                    case 4:
                        xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                        effect->timer = 15;
                        break;
                    }
                }
                break;
            case 1:
                switch (effect->code) {
                case 1:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_1, 12 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase = 2;
                    }
                    break;
                case 2:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase = 2;
                    }
                    break;
                case 3:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase = 2;
                    }
                    break;
                case 4:
                    if (effect->timer == 0) {
                        effect->code = 0;
                        break;
                    }
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_4, 14 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                    xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    break;
                }
                break;
            case 2:
                if (effect->stack == 0 || effect->stack->layers[0].life > 0) {
                    effect->code = 0;
                } else {
                    effect->phase = 3;
                    effect->timer = 20;
                }
                break;
            case 3:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_FADE, 10 - effect->timer / 2, CARD_EFFECT_SMALL_FRAME);
                xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
                break;
            }
            break;
        case 21:
        case 22:
        case 23:
        case 24:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                } else {
                    effect->timer = 9;
                    effect->phase++;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                    break;
                }
                effect->phase++;
                switch (effect->code) {
                case 21:
                    effect->timer = 13;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_BURST, 0);
                    break;
                case 22:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 16;
                    break;
                case 23:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 11;
                    break;
                case 24:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 15;
                    break;
                }
                break;
            case 2:
                switch (effect->code) {
                case 21:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_1, 12 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case 22:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case 23:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case 24:
                    if (effect->timer == 0) {
                        effect->code = 0;
                        break;
                    }
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_4, 14 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                    xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    break;
                }
                break;
            case 3:
                if (effect->stack == 0 || effect->stack->layers[0].life > 0) {
                    effect->code = 0;
                } else {
                    effect->timer = 20;
                    effect->phase++;
                }
                break;
            case 4:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_FADE, 10 - effect->timer / 2, CARD_EFFECT_SMALL_FRAME);
                xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
                break;
            }
            break;
        case 31:
        case 32:
        case 34:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                } else {
                    effect->timer = 18;
                    effect->phase++;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                    break;
                }
                effect->phase++;
                switch (effect->code) {
                case 31:
                    effect->timer = 13;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_BURST, 0);
                    break;
                case 32:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 16;
                    break;
                case 33:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 11;
                    break;
                case 34:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 15;
                    break;
                }
                break;
            case 2:
                switch (effect->code) {
                case 31:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_1, 12 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case 32:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case 33:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case 34:
                    if (effect->timer == 0) {
                        effect->code = 0;
                        break;
                    }
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_4, 14 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                    xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                    break;
                }
                break;
            case 3:
                if (effect->stack == 0 || effect->stack->layers[0].life > 0) {
                    effect->code = 0;
                } else {
                    effect->timer = 20;
                    effect->phase++;
                }
                break;
            case 4:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_FADE, 10 - effect->timer / 2, CARD_EFFECT_SMALL_FRAME);
                xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
                break;
            }
            break;
        case 33:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_3, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                    xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
                } else {
                    effect->phase = 1;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 11;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                    xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                } else {
                    effect->timer = 15;
                    effect->phase = 2;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                }
                break;
            case 2:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                offset[2] = effect->position[2];
                offset[3] = 1.0f;
                offset[0] = effect->position[0] - 0.54f;
                offset[1] = effect->position[1] + 0.3915f;
                xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                offset[0] = effect->position[0] + 0.54f;
                offset[1] = effect->position[1] + 0.3915f;
                xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                offset[0] = effect->position[0] - 0.54f;
                offset[1] = effect->position[1] - 0.3915f;
                xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                offset[0] = effect->position[0] + 0.54f;
                offset[1] = effect->position[1] - 0.3915f;
                xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SCALED_MODEL);
                break;
            }
            break;
        case 5:
            if (side != effect->side) {
                break;
            }
            if (effect->timer == 0) {
                effect->code = 0;
                break;
            }
            effect->timer--;
            texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
            xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
            CDEMatrixSub(local.m);
            nmlModelSetTexture(texture);
            nmlModelSetPlace(local.m);
            nmlModelSetToumei(1);
            nmlModelSetAlpha(CARD_EFFECT_ALPHA);
            nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
            break;
        case 25:
            if (side != effect->side) {
                break;
            }
            if (effect->timer != 0) {
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
            } else if (effect->phase != 0) {
                effect->code = 0;
            } else {
                effect->phase++;
                effect->timer = 15;
                xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                effect->position[0] += 0.25f;
                effect->position[1] += 0.25f;
            }
            break;
        case 10:
            if (side != effect->side) {
                break;
            }
            if (effect->timer == 0) {
                effect->code = 0;
                break;
            }
            effect->timer--;
            texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_SWIRL, 29 - effect->timer, CARD_EFFECT_SMALL_FRAME);
            xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
            CDEMatrixSub(local.m);
            nmlModelSetTexture(texture);
            xglMatrixScale(local.m, (const float (*)[4]) local.m, effect->scale);
            nmlModelSetPlace(local.m);
            nmlModelSetToumei(1);
            nmlModelSetAlpha(CARD_EFFECT_ALPHA);
            nmlModelEntryCard((void *) CARD_EFFECT_SPRITE_MODEL);
            break;
        case 11:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = 1;
                    effect->timer = 15;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        offset[0] = 0.15f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                            digit = CARD_EFFECT_DIGITS + (digit << 10);
                            nmlModelSetPlace(quad.m);
                            nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(asColor, asDir);
                            nmlModelEntryCard((void *) digit);
                        }
                        offset[0] = 0.25f;
                        digit = effect->value % 10;
                        digit = CARD_EFFECT_DIGITS + (digit << 10);
                        xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                        nmlModelSetPlace(quad.m);
                        nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(asColor, asDir);
                        nmlModelEntryCard((void *) digit);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        case 15:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = 1;
                    effect->timer = 15;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        offset[0] = 0.15f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                            digit = CARD_EFFECT_DIGITS + (digit << 10);
                            nmlModelSetPlace(quad.m);
                            nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(asColor, asDir);
                            nmlModelEntryCard((void *) digit);
                        }
                        offset[0] = 0.25f;
                        digit = effect->value % 10;
                        digit = CARD_EFFECT_DIGITS + (digit << 10);
                        xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                        nmlModelSetPlace(quad.m);
                        nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(asColor, asDir);
                        nmlModelEntryCard((void *) digit);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        case 12:
            if (side != effect->side) {
                break;
            }
            if (effect->timer == 0) {
                effect->code = 0;
                break;
            }
            effect->timer--;
            offset[3] = 1.0f;
            if (effect->stack != 0) {
                offset[0] = effect->stack->position[0];
                offset[1] = effect->stack->position[1];
                offset[2] = effect->stack->position[2];
            } else {
                if (effect->phase == 0) {
                    offset[1] = -1.81f;
                } else {
                    offset[1] = 1.81f;
                }
                offset[0] = 0.0f;
                offset[2] = 0.0f;
            }
            xglMatrixTrans(local.m, (const float (*)[4]) local.m, offset);
            CDEMatrixSub(local.m);
            xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
            offset[0] = 0.15f;
            offset[1] = 0.0f;
            offset[2] = 0.1f;
            digit = effect->value / 10;
            if (digit != 0) {
                xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                if (effect->phase == 0) {
                    digit = CARD_EFFECT_DIGITS_FIRST + (digit << 10);
                } else {
                    digit = CARD_EFFECT_DIGITS + (digit << 10);
                }
                nmlModelSetPlace(quad.m);
                nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                nmlModelSetLight(asColor, asDir);
                nmlModelEntryCard((void *) digit);
            }
            offset[0] = 0.25f;
            digit = effect->value % 10;
            if (effect->phase == 0) {
                digit = CARD_EFFECT_DIGITS_FIRST + (digit << 10);
            } else {
                digit = CARD_EFFECT_DIGITS + (digit << 10);
            }
            xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
            nmlModelSetPlace(quad.m);
            nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
            nmlModelSetLight(asColor, asDir);
            nmlModelEntryCard((void *) digit);
            effect->position[0] += effect->velocity[0];
            effect->position[1] += effect->velocity[1];
            effect->position[2] += effect->velocity[2];
            break;
        case 14:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = 1;
                    effect->timer = 15;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        offset[0] = -0.35f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                            digit = CARD_EFFECT_DIGITS + (digit << 10);
                            nmlModelSetPlace(quad.m);
                            nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(asColor, asDir);
                            nmlModelEntryCard((void *) digit);
                        }
                        offset[0] = -0.25f;
                        digit = effect->value % 10;
                        digit = CARD_EFFECT_DIGITS + (digit << 10);
                        xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                        nmlModelSetPlace(quad.m);
                        nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(asColor, asDir);
                        nmlModelEntryCard((void *) digit);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        case 16:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case 0:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = 1;
                    effect->timer = 15;
                }
                break;
            case 1:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, (const float (*)[4]) local.m, effect->position);
                        offset[0] = 0.15f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                            digit = CARD_EFFECT_DIGITS + (digit << 10);
                            nmlModelSetPlace(quad.m);
                            nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(asColor, asDir);
                            nmlModelEntryCard((void *) digit);
                        }
                        offset[0] = 0.25f;
                        digit = effect->value % 10;
                        digit = CARD_EFFECT_DIGITS + (digit << 10);
                        xglMatrixTrans(quad.m, (const float (*)[4]) local.m, offset);
                        nmlModelSetPlace(quad.m);
                        nmlModelSetTexture((void *) CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(asColor, asDir);
                        nmlModelEntryCard((void *) digit);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        }
    }
}

/*
 * Opens a card list window on the cards of a hand that has reached the hand
 * limit (CGPEndFaseProc, 0x00a3126c). Returns -1 once the window is laid out and
 * 0 when the hand is below the limit.
 */
s32 CGPCheckTeMax(CardListWindow *list, CardHand *hand)
{
    s32 count;
    s32 i;

    count = CardPlayHandCnt(hand);
    if (count >= CGP_HAND_LIMIT) {
        list->baseColor = CGP_HAND_LIST_BASE_COLOR;
        list->cursorColor = CGP_HAND_LIST_CURSOR_COLOR;
        list->x = CGP_HAND_LIST_X;
        list->y = CGP_HAND_LIST_Y;
        list->lineHeight = CGP_HAND_LIST_LINE_HEIGHT;
        list->rows = CGP_HAND_LIST_ROWS;
        list->active = 0;
        list->cursor = 0;
        list->top = 0;
        list->count = count;
        for (i = 0; i < count; i++) {
            list->entries[i].cardId = hand->cards[i];
        }
        return -1;
    }
    return 0;
}

void CGPCommFinishSub(CGPCardDefSource *src, CardPlayHand *work, s32 index)
{
    s32 cost;
    s32 j;
    s32 junkCnt;

    if (index >= 0) {
        work->lastCardValue = work->cards[index];
        work->cards[index] = -1;
        CardPlayCleanHand(work);
    }

    cost = src->defs[work->lastCardValue].cost;
    for (j = 0; j < cost; j++) {
        CardPlayCostCard(work);
    }

    junkCnt = CardPlayJunkCnt(work);
    work->junkPile[junkCnt] = work->lastCardValue;
    work->lastCardValue = -1;
}

/* Arena the card game loads its model, texture and effect files into (see CPISMdlLst). */
#define CGP_ARENA(offset) (CARD_MODEL_ARENA_BASE + (offset))

#define CGP_FLAG_HELD_TRIANGLE 0x80   /* triangle was held when the game started (`addiu $2,$0,0x80`, 0x00a25254) */
#define CGP_HELP_TOPIC_GAME 0x21      /* `addiu $4,$0,0x21`, 0x00a2525c */
#define CGP_EFFECT_COUNT 64           /* `addiu $6,$0,0x3F`, 0x00a2526c counts down to zero */
#define CGP_QUEUE_SLOTS 40            /* `addiu $6,$0,0x27`, 0x00a25308 */
#define CGP_QUEUE_SIDE_DISTANCE 160   /* halfwords from the enemy's queue back to the player's (`sh $4,-0x140($2)`, 0x00a25314) */

/* Rotation vector the stage starts from (lui/ori 0xBF666666 at 0x00a25354, lui 0xC1A00000 at 0x00a25348). */
#define CGP_ROT_STAGE_START_X (-0.9f)
#define CGP_ROT_STAGE_START_Y (-20.0f)

extern int CardFread(u32 destination, char *filename);

/*
 * Starts a card game: clears the game flags, the command control, the messages,
 * the effect table and both command queues, resets the cursor and the stage
 * models, then reads the game's model, texture and effect files into the arena.
 */
void CardGameInit(CardGameWork *work)
{
    s32 i;
    s32 shown;

    work->flags = 0;
    if (work->command.input[0].held & CGP_PAD_PREVIEW) {
        work->flags = CGP_FLAG_HELD_TRIANGLE;
    }
    work->command.interruptMessId = 1;
    work->command.interruptMessTimer = CGP_INTERRUPT_MESS_FRAMES;
    work->command.turn = 0;
    work->command.phase = 0;
    work->command.phaseTimer = 0;
    work->command.step = 0;
    work->command.flowRequest = 0;
    work->command.turnCountdown = 0;
    work->command.recoveryApplied = 0;
    work->command.commandIndex = 0;
    work->command.exitMenuItem = 0;
    work->aiStep = 0;
    work->command.aiPhase = 0;
    work->errorMessSide = 0;
    work->errorMessKind = 0;
    work->infoPanelPhase = 0;
    work->resultWait = 0;
    work->errorMessReason = 0;
    work->mess = 0;
    work->interruptMess = 0;
    work->permanentMess = 0;
    work->cursorMoveStep[0] = 0;
    work->cursorMoveStep[1] = 0;
    work->effectsActive = 0;
    work->effectsSet = 0;
    work->helpTopic = CGP_HELP_TOPIC_GAME;
    work->errorMessValue = -1;
    for (i = 0; i < CGP_EFFECT_COUNT; i++) {
        work->effects[i].side = 0;
        work->effects[i].code = 0;
        work->effects[i].phase = 0;
        work->effects[i].timer = 0;
    }
    {
        s32 freeMark = -1;
        s16 *queueCommand = &work->commandQueue[1][0][1];
        s16 *queuePhase = &work->commandQueue[1][0][0];

        /* The player's queue lies 40 entries of four halfwords before the enemy's. */
        for (i = 0; i < CGP_QUEUE_SLOTS; i++) {
            queuePhase[-CGP_QUEUE_SIDE_DISTANCE] = freeMark;
            queueCommand[-CGP_QUEUE_SIDE_DISTANCE] = 0;
            queuePhase[0] = freeMark;
            queueCommand[0] = 0;
            queuePhase += 4;
            queueCommand += 4;
        }
    }
    CardCursorPassive(&work->cursor);
    CGPSetMessage(work, 0);
    work->command.slideDirection = 0;
    work->rotStageVector[1] = CGP_ROT_STAGE_START_Y;
    work->rotStageVector[0] = CGP_ROT_STAGE_START_X;
    for (i = 15; i >= 0; i--) {
        work->models[i].visible = 0;
    }
    /* models 4..7 and 15 are drawn from the start, together with the rotating stage */
    shown = 1;
    work->models[4].visible = shown;
    work->models[5].visible = shown;
    work->models[6].visible = shown;
    work->models[7].visible = shown;
    work->models[15].visible = shown;
    work->rotStageActive = shown;
    work->rotStageCooldown = 0;
    work->rotStageFlag = 0;
    work->rotStageId = 0;
    work->rotStageVector[1] = CGP_ROT_STAGE_START_Y;
    work->rotStageVector[2] = 0.0f;
    work->rotStageVector[0] = CGP_ROT_STAGE_START_X;
    CardFread(CGP_ARENA(0x0), "game\\E_L.lex");
    CardFread(CGP_ARENA(0x400), "game\\E_S.lex");
    CardFread(CGP_ARENA(0x800), "game\\down_L.lex");
    CardFread(CGP_ARENA(0xC00), "game\\down_s.lex");
    CardFread(CGP_ARENA(0x1000), "game\\on.lex");
    CardFread(CGP_ARENA(0x1400), "game\\off.lex");
    CardFread(CGP_ARENA(0x1800), "game\\NewYama.lex");
    CardFread(CGP_ARENA(0x2800), "game\\Ptern.lex");
    CardFread(CGP_ARENA(0x3800), "game\\Etern.lex");
    CardFread(CGP_ARENA(0x4800), "game\\P1tern.lex");
    CardFread(CGP_ARENA(0x5800), "game\\P2tern.lex");
    CardFread(CGP_ARENA(0x6800), "game\\Pwin.lex");
    CardFread(CGP_ARENA(0x7800), "game\\Ewin.lex");
    CardFread(CGP_ARENA(0x8800), "game\\P1win.lex");
    CardFread(CGP_ARENA(0x9800), "game\\P2win.lex");
    CardFread(CGP_ARENA(0xA800), "game\\dlow.lex");
    CardFread(CGP_ARENA(0xB800), "game\\move.lex");
    CardFread(CGP_ARENA(0xC800), "game\\set.lex");
    CardFread(CGP_ARENA(0xD800), "game\\event.lex");
    CardFread(CGP_ARENA(0xE800), "game\\battle.lex");
    CardFread(CGP_ARENA(0xF800), "game\\adjust.lex");
    CardFread(CGP_ARENA(0x10800), "game\\block.lex");
    CardFread(CGP_ARENA(0x11800), "game\\NewYama1P.lex");
    CardFread(CGP_ARENA(0x15800), "game\\NewYama2P.lex");
    CardFread(CGP_ARENA(0x19800), "game\\HPbar.lex");
    CardFread(CGP_ARENA(0x1A800), "game\\dlow2.lex");
    CardFread(CGP_ARENA(0x1B800), "game\\move2.lex");
    CardFread(CGP_ARENA(0x1C800), "game\\set2.lex");
    CardFread(CGP_ARENA(0x1D800), "game\\event2.lex");
    CardFread(CGP_ARENA(0x1E800), "game\\battle2.lex");
    CardFread(CGP_ARENA(0x1F800), "game\\adjust2.lex");
    CardFread(CGP_ARENA(0x20800), "game\\block2.lex");
    CardFread(CGP_ARENA(0x21800), "game\\tern.lex");
    CardFread(CGP_ARENA(0x22800), "game\\phase1.lex");
    CardFread(CGP_ARENA(0x23800), "game\\phase2.lex");
    CardFread(CGP_ARENA(0x24800), "game\\phase3.lex");
    CardFread(CGP_ARENA(0x25800), "game\\phase4.lex");
    CardFread(CGP_ARENA(0x26800), "game\\phase5.lex");
    CardFread(CGP_ARENA(0x27800), "game\\phase6.lex");
    CardFread(CGP_ARENA(0x28800), "game\\phase7.lex");
    CardFread(CGP_ARENA(0x29800), "game\\up_R.lex");
    CardFread(CGP_ARENA(0x2B000), "game\\up_L.lex");
    CardFread(CGP_ARENA(0x39800), "game\\up_LR.bin");
    CardFread(CGP_ARENA(0x83400), "game\\yama.xtx");
    CardFread(CGP_ARENA(0x85400), "game\\HPbar.xtx");
    CardFread(CGP_ARENA(0x96000), "game\\mat_mojiJ.lex");
    CardFread(CGP_ARENA(0x9A000), "game\\mat_mojiJ.xtx");
    CardFread(CGP_ARENA(0x30000), "deckmake\\battlewin1.lex");
    CardFread(CGP_ARENA(0x31800), "deckmake\\battlewin2.lex");
    CardFread(CGP_ARENA(0x33000), "deckmake\\battlewin3.lex");
    CardFread(CGP_ARENA(0x34800), "deckmake\\battlewin4.lex");
    CardFread(CGP_ARENA(0x36000), "deckmake\\battlewin5.lex");
    CardFread(CGP_ARENA(0x37800), "deckmake\\battlewin.bin");
    CardFread(CGP_ARENA(0x40000), "game\\pop1.lex");
    CardFread(CGP_ARENA(0x44000), "game\\pop2.lex");
    CardFread(CGP_ARENA(0x49000), "game\\pop.xtx");
    CardFread(CGP_ARENA(0xA8000), "game\\NewPop1P.lex");
    CardFread(CGP_ARENA(0xAC000), "game\\NewPop2P.lex");
    CardFread(CGP_ARENA(0xB0000), "game\\NewPop.xtx");
    CardFread(CGP_ARENA(0x50000), "game\\victory1.lex");
    CardFread(CGP_ARENA(0x50510), "game\\victory2.lex");
    CardFread(CGP_ARENA(0x50A20), "game\\victory3.lex");
    CardFread(CGP_ARENA(0x50F30), "game\\victory4.lex");
    CardFread(CGP_ARENA(0x51440), "game\\victory5.lex");
    CardFread(CGP_ARENA(0x51950), "game\\victory6.lex");
    CardFread(CGP_ARENA(0x51E60), "game\\victory7.lex");
    CardFread(CGP_ARENA(0x52370), "game\\NewLost1.lex");
    CardFread(CGP_ARENA(0x52880), "game\\NewLost2.lex");
    CardFread(CGP_ARENA(0x52D90), "game\\victory.bin");
    CardFread(CGP_ARENA(0x60000), "game\\NewLost1.xtx");
    CardFread(CGP_ARENA(0x63680), "game\\NewLost2.xtx");
    CardFread(CGP_ARENA(0x66D00), "game\\NewLost3.xtx");
    CardFread(CGP_ARENA(0x100000), "effect\\effect0.lex");
    CardFread(CGP_ARENA(0x100800), "effect\\effect1.lex");
    CardFread(CGP_ARENA(0x101000), "effect\\atac_a.bin");
    CardFread(CGP_ARENA(0x114000), "effect\\atac_b.bin");
    CardFread(CGP_ARENA(0x128000), "effect\\bom.bin");
    CardFread(CGP_ARENA(0x140000), "effect\\flas.bin");
    CardFread(CGP_ARENA(0x167000), "effect\\misl.bin");
    CardFread(CGP_ARENA(0x185000), "effect\\rang.bin");
    CardFread(CGP_ARENA(0x1A0000), "effect\\shot.bin");
    CardFread(CGP_ARENA(0x2D8000), "effect\\zan.bin");
    CardFread(CGP_ARENA(0x2F0000), "effect\\atac_c.bin");
    CardFread(CGP_ARENA(0x304000), "effect\\atac_d.bin");
}

/*
 * Starts the next phase: phases 0..6 are one side's and 7..13 the other's, and
 * after phase 13 the turn counter advances and the phases start over. Clears the
 * per-phase state, shows the phase's message and interrupt message and points the
 * help page at the phase's topic. Called by the computer player's phase handlers
 * (card_enemy.c) and the phase routines of this TU.
 */
void CGPNextTurnSub(CardGameWork *work)
{
    work->infoPanelPhase = 0;
    work->command.phase++;
    if ((s16) work->command.phase >= CGP_PHASE_COUNT) {
        work->command.phase = 0;
        work->command.turn++;
        xglSoundEffectNormalID(CGP_SE_NEXT_TURN, 0);
    } else if ((s16) work->command.phase == CGP_SIDE1_FIRST_PHASE) {
        xglSoundEffectNormalID(CGP_SE_NEXT_TURN, 0);
    } else {
        xglSoundEffectNormalID(CGP_SE_NEXT_PHASE, 0);
    }
    work->command.interruptMessId = work->command.phase + 1;
    work->command.interruptMessTimer = CGP_INTERRUPT_MESS_FRAMES;
    work->command.phaseTimer = 0;
    work->command.step = 0;
    work->aiStep = 0;
    work->command.aiPhase = 0;
    CardCursorPassive(&work->cursor);
    work->cursor.controllingSide = -1;
    work->cursorMoveStep[0] = 0;
    work->cursorMoveStep[1] = 0;
    CGPSetMessage(work, (s16) work->command.phase);
    CGPSetInterruptMess(work, (s16) work->command.phase + 1);
    work->helpTopic = HelpPageList[(s16) work->command.phase];
    work->rotStageVector[1] = -20.0f;
    work->command.slideDirection = 0;
    work->rotStageVector[0] = -0.9f;
}



/*
 * Swap a disposal slot's stack with a battle slot's and start the ten-frame
 * slide that carries each of them to the other's position.
 */
void CGPMoveDSBATTLEwork(CardMoveWork *disposal, CardMoveWork *battle)
{
    CardMoveWork moved;
    s32 i;

    if (disposal->moveStep != 0) {
        float steps = disposal->moveStep;

        disposal->position[0] += disposal->velocity[0] * steps;
        disposal->position[1] += disposal->velocity[1] * steps;
        disposal->position[2] += disposal->velocity[2] * steps;
    }
    if (battle->moveStep != 0) {
        float steps = battle->moveStep;

        battle->position[0] += battle->velocity[0] * steps;
        battle->position[1] += battle->velocity[1] * steps;
        battle->position[2] += battle->velocity[2] * steps;
    }

    moved.flags = disposal->flags;
    moved.power = disposal->power;
    moved.powerLimit = disposal->powerLimit;
    moved.position[0] = disposal->position[0];
    moved.position[1] = disposal->position[1];
    moved.position[2] = disposal->position[2];
    moved.shuntPosition[0] = disposal->shuntPosition[0];
    moved.shuntPosition[1] = disposal->shuntPosition[1];
    moved.shuntPosition[2] = disposal->shuntPosition[2];
    for (i = 0; i < 40; i++) {
        moved.layers[i].state = disposal->layers[i].state;
        moved.layers[i].preservedByte01 = disposal->layers[i].preservedByte01;
        moved.layers[i].boostCount = disposal->layers[i].boostCount;
        moved.layers[i].life = disposal->layers[i].life;
        moved.layers[i].preservedByte04 = disposal->layers[i].preservedByte04;
        moved.layers[i].damageTimer = disposal->layers[i].damageTimer;
        moved.layers[i].cardId = disposal->layers[i].cardId;
    }

    disposal->flags = battle->flags;
    disposal->power = battle->power;
    disposal->powerLimit = battle->powerLimit;
    disposal->moveStep = 10;
    disposal->velocity[0] = (disposal->position[0] - battle->position[0]) / 10.0f;
    disposal->velocity[1] = (disposal->position[1] - battle->position[1]) / 10.0f;
    disposal->velocity[2] = 0.0f;
    disposal->position[0] = battle->position[0];
    disposal->position[1] = battle->position[1];
    disposal->position[2] = 0.12f;
    disposal->shuntPosition[0] = battle->shuntPosition[0];
    disposal->shuntPosition[1] = battle->shuntPosition[1];
    disposal->shuntPosition[2] = battle->shuntPosition[2];
    for (i = 0; i < 40; i++) {
        disposal->layers[i].life = battle->layers[i].life;
        disposal->layers[i].cardId = battle->layers[i].cardId;
    }

    battle->flags = moved.flags;
    battle->power = moved.power;
    battle->powerLimit = moved.powerLimit;
    battle->moveStep = 10;
    battle->velocity[0] = (battle->position[0] - moved.position[0]) / 10.0f;
    battle->velocity[1] = (battle->position[1] - moved.position[1]) / 10.0f;
    battle->velocity[2] = 0.0f;
    battle->position[0] = moved.position[0];
    battle->position[1] = moved.position[1];
    battle->position[2] = 0.12f;
    for (i = 0; i < 40; i++) {
        battle->layers[i].state = moved.layers[i].state;
        battle->layers[i].preservedByte01 = moved.layers[i].preservedByte01;
        battle->layers[i].boostCount = moved.layers[i].boostCount;
        battle->layers[i].life = moved.layers[i].life;
        battle->layers[i].preservedByte04 = moved.layers[i].preservedByte04;
        battle->layers[i].damageTimer = moved.layers[i].damageTimer;
        battle->layers[i].cardId = moved.layers[i].cardId;
        battle->shuntPosition[0] = moved.shuntPosition[0];
        battle->shuntPosition[1] = moved.shuntPosition[1];
        battle->shuntPosition[2] = moved.shuntPosition[2];
    }
}

/* A top card of one of these ids attacks with the power of the base card. */
#define CGP_CARD_BASE_POWER_1 44  /* `addiu $4,$0,0x2C`, 0x00a29d00 */
#define CGP_CARD_BASE_POWER_2 47  /* `addiu $2,$0,0x2F`, 0x00a29d30 */
#define CGP_CARD_BASE_POWER_3 53  /* `addiu $2,$0,0x35`, 0x00a29d38 */
/* Cards the steps single out (ids from the immediates around 0x00a2bd04, 0x00a2bdac and 0x00a2bf68). */
#define CGP_CARD_SIDE_WEAPON 52       /* attaches only to card 11 (CGPDisposeWeaponSub) */
#define CGP_CARD_SIDE_WEAPON_TARGET 11
/* Card kinds (CardDefinition.kind). Kind 1 is paid from playPoints[0] and kind 4 from playPoints[1]; kind 4 is
 * the one CardPlayDesertOperation removes; kind 2 cannot be played (error 11) and is the one the hand discard
 * step accepts (`kind & 2`). */
#define CGP_CARD_KIND_COMMAND 1
#define CGP_CARD_KIND_DISCARD_ONLY 2
#define CGP_CARD_KIND_OPERATION 4
/* CardLayerStack.flags bit set on a locked battle slot (`andi $2,$2,0x2`, 0x00a2ea70 and 0x00a2eba8; error CGP_ERR_SLOT_LOCKED). */
#define CGP_BATTLE_FLAG_SLOT_LOCKED 0x02
/* Operation card 122 in play on the defending side lowers the attack power (`addiu $5,$0,0x7A`, 0x00a288b0). */
#define CGP_OPE_DEFENCE_DOWN 122

/*
 * Attaches the equipment card at hand position index to a weapon stack: the
 * weapon's displaced previous equipment card goes to the junk pile, the new
 * card becomes layer one with its operation duration as life, takes the card's
 * reserve cost from the draw pile and its play cost from the play points, and
 * leaves the hand. Cards 44, 47 and 53 keep the stack's power.
 */
void CGPequipSub(CardGameWork *work, CardPlayHand *hand, CardLayerStack *stack, s32 index)
{
    s32 i;
    s32 junkCount;

    if (stack->layers[1].cardId >= 0) {
        junkCount = CardPlayJunkCnt(hand);
        stack->power = work->definitions[stack->layers[0].cardId].basePower;
        stack->powerLimit = work->definitions[stack->layers[0].cardId].basePower;
        hand->junkPile[junkCount] = stack->layers[1].cardId;
    }
    stack->layers[1].life = work->definitions[hand->cards[index]].operationDuration;
    if (hand->cards[index] != CGP_CARD_BASE_POWER_1 && hand->cards[index] != CGP_CARD_BASE_POWER_2 &&
        hand->cards[index] != CGP_CARD_BASE_POWER_3) {
        stack->power = work->definitions[hand->cards[index]].basePower;
        stack->powerLimit = work->definitions[hand->cards[index]].basePower;
    }
    stack->layers[1].state = 0;
    stack->layers[1].flag = 0;
    stack->layers[1].boostCount = 0;
    stack->layers[1].cardId = hand->cards[index];
    for (i = 0; i < work->definitions[hand->cards[index]].reserveCost; i++) {
        CardPlayCostCard(hand);
    }
    hand->playPoints[0] -= work->definitions[hand->cards[index]].playCost;
    hand->cards[index] = -1;
    CardPlayCleanHand(hand);
}

/*
 * Whether one equipment card may be attached to one weapon card. Both cards
 * must carry kind bit 0. A weapon whose flags are 32 takes an equipment card
 * with flag 8, except cards 4, 80 and 65, and card 11 only on weapon 52; a
 * weapon whose flags are 64 takes an equipment card with flag 1 or flag 2,
 * except cards 2, 9 and 14. Every other weapon takes none.
 */
s32 CGPWeaponEquipChk(CardGameWork *work, s32 weaponId, s32 equipId)
{
    u8 weaponKind;
    u8 equipKind;
    u16 weaponFlags;
    u16 equipFlags;
    s32 allowed;

    weaponKind = work->definitions[weaponId].kind;
    equipKind = work->definitions[equipId].kind;
    weaponFlags = work->definitions[weaponId].flags;
    equipFlags = work->definitions[equipId].flags;
    allowed = 0;
    if ((weaponKind & 1) && (equipKind & 1)) {
        switch (weaponFlags) {
        case 32:
            if ((equipFlags & 8) && equipId != 4 && equipId != 80 &&
                equipId != 65)
            {
                if (equipId != 11) {
                    allowed = 1;
                } else if (weaponId == 52) {
                    allowed = 1;
                }
            }
            break;
        case 64:
            if (((equipFlags & 1) || (equipFlags & 2)) &&
                equipId != 2 && equipId != 9 && equipId != 14)
            {
                allowed = 1;
            }
            break;
        }
    }
    return allowed;
}

/*
 * Attaches the picked-up hand card (held: hand position and card id) to the
 * board card under the cursor (target), then confirms with the decide sound
 * and restarts the phase timer. Every refusal plays the buzzer instead.
 * The target must lie in the acting side's disposal or battle row and hold a
 * card. Card 52 only attaches to card 11. A card with definition flags 0x20
 * attaches to a card with flag 8 except cards 4, 11, 80 and 65. Any other card
 * needs flag 0x40 and a target with flag 1 or 2 except cards 2, 9 and 14.
 */
static inline void CGPAttachHeldCard(CardGameWork *work, CardPlaySide *ownSide,
                                     CardLayerStack *stack, s32 handIndex)
{
    CGPequipSub(work, (CardPlayHand *) &ownSide->hand, stack, handIndex);
    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
    work->command.phaseTimer = 1;
}

void CGPDisposeWeaponSub(s32 side, CardGameWork *work, CGPCursorPosition held,
                         CGPCursorPosition target)
{
    CardPlaySide *ownSide;
    CardLayerStack *stack;
    s32 disposalArea;
    s32 battleArea;
    s32 area;
    u16 targetFlags;

    ownSide = &work->player;
    disposalArea = CGP_AREA_PLAYER_DISPOSAL;
    battleArea = CGP_AREA_PLAYER_BATTLE;
    if (side != 0) {
        ownSide = &work->enemy;
        disposalArea = CGP_AREA_ENEMY_DISPOSAL;
        battleArea = CGP_AREA_ENEMY_BATTLE;
    }
    area = target.zone;
    if ((disposalArea == area || battleArea == area) && target.cardId >= 0) {
        if (disposalArea == area) {
            stack = &ownSide->disposal[target.index].battle;
        } else {
            stack = &ownSide->battle[target.index].battle;
        }
        if (held.cardId == CGP_CARD_SIDE_WEAPON) {
            if (target.cardId == CGP_CARD_SIDE_WEAPON_TARGET) {
                CGPAttachHeldCard(work, ownSide, stack, held.index);
            } else {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            }
        } else if ((work->definitions[held.cardId].flags & 0x20) &&
                   (work->definitions[target.cardId].flags & 8)) {
            if (target.cardId != 4 && target.cardId != 11 && target.cardId != 80 &&
                target.cardId != 65) {
                CGPAttachHeldCard(work, ownSide, stack, held.index);
            } else {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            }
        } else if (work->definitions[held.cardId].flags & 0x40) {
            targetFlags = work->definitions[target.cardId].flags;
            if (((targetFlags & 1) || (targetFlags & 2)) && target.cardId != 2 &&
                target.cardId != 9 && target.cardId != 14) {
                CGPAttachHeldCard(work, ownSide, stack, held.index);
            } else {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            }
        } else {
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
        }
    } else {
        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
    }
}

/*
 * Board area the cursor currently points at. The ten areas follow the cursor's
 * zone: the enemy hand is zone 0 (area 0 while the cursor is on one of its
 * cards, 1 once it is past them), zones 1 to 4 are the enemy board row (area 2
 * for its last column, 3 for its first, 4 otherwise), zones 5 to 8 the player
 * board row (areas 7, 6 and 5 the same way round) and zone 9 the player hand
 * (area 9 on a card, 8 past them).
 */
s32 CGPCursor2Area(CardGameWork *work)
{
    s32 area;
    s32 count;

    if (work->cursorZone < 5) {
        area = 4;
        if (work->cursorZone == 0) {
            count = CardPlayHandCnt(&work->enemy.hand);
            area = 1;
            if (work->cursorIndex < count) {
                area = 0;
            }
        } else {
            if (work->cursorIndex == 0) {
                area = 3;
            }
            if (work->cursorIndex == 3) {
                area = 2;
            }
        }
    } else {
        area = 5;
        if (work->cursorZone == 9) {
            count = CardPlayHandCnt(&work->player.hand);
            area = 8;
            if (work->cursorIndex < count) {
                area = 9;
            }
        } else {
            if (work->cursorIndex == 0) {
                area = 6;
            }
            if (work->cursorIndex == 3) {
                area = 7;
            }
        }
    }
    return area;
}

void CGPCursor2Pos(CGPCursorPosition *position, CardGameWork *work)
{
    position->cardId = -1;
    position->stackedCard = -1;
    position->zone = CGPCursor2Area(work);
    switch (position->zone) {
    case 0:
        position->index = work->cursorIndex;
        position->cardId = work->enemy.hand.cards[position->index];
        break;
    case 9:
        position->index = work->cursorIndex;
        position->cardId = work->player.hand.cards[position->index];
        break;
    case 2:
        position->index = 4 - work->cursorZone;
        position->cardId = work->enemy.disposal[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->enemy.disposal[position->index].battle.layers[1].cardId >= 0)
        {
            position->stackedCard = work->enemy.disposal[position->index].battle.layers[1].cardId;
        }
        break;
    case 3:
        position->index = 4 - work->cursorZone;
        position->cardId = work->enemy.operations[position->index];
        break;
    case 4:
        position->index = 5 - work->cursorIndex - work->cursorZone;
        position->cardId = work->enemy.battle[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->enemy.battle[position->index].battle.layers[1].cardId >= 0)
        {
            position->stackedCard = work->enemy.battle[position->index].battle.layers[1].cardId;
        }
        break;
    case 6:
        position->index = work->cursorZone - 5;
        position->cardId = work->player.disposal[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->player.disposal[position->index].battle.layers[1].cardId != 0)
        {
            position->stackedCard = work->player.disposal[position->index].battle.layers[1].cardId;
        }
        break;
    case 7:
        position->index = work->cursorZone - 5;
        position->cardId = work->player.operations[position->index];
        break;
    case 5:
        position->index = work->cursorIndex + work->cursorZone - 6;
        position->cardId = work->player.battle[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->player.battle[position->index].battle.layers[1].cardId != 0)
        {
            position->stackedCard = work->player.battle[position->index].battle.layers[1].cardId;
        }
        break;
    case 8:
        position->index = work->cursorIndex - CardPlayHandCnt(&work->player.hand);
        position->cardId = work->player.junk[position->index];
        break;
    case 1:
        position->index = work->cursorIndex - CardPlayHandCnt(&work->enemy.hand);
        position->cardId = work->enemy.junk[position->index];
        break;
    }
}






/*
 * Clamps the cursor index to 0..3, resolves the cursor's zone/card (
 * CGPCursor2Pos), then for zones 2, 3, 6 and 7 sets the highlight position
 * to 0.175; for zones 4 and 5 sets it to 0.355 and rounds the cursor zone
 * down to the nearest odd value. Returns the resolved card id.
 */
s16 CGPCMSub(CardGameWork *work)
{
    CGPCursorPosition position;

    work->highlightScaleX = work->highlightScaleY = 1.0f;
    if (work->cursorIndex < 0) {
        work->cursorIndex = 3;
    }
    if (work->cursorIndex >= 4) {
        work->cursorIndex = 0;
    }
    CGPCursor2Pos(&position, work);
    switch (position.zone) {
    case 2:
    case 3:
    case 6:
    case 7:
        work->highlightX = work->highlightY = 0.175f;
        break;
    case 4:
    case 5:
        work->cursorZone = ((work->cursorZone - 1) & 0xFE) + 1;
        work->highlightX = work->highlightY = 0.355f;
        break;
    }
    return position.cardId;
}

/*
 * The cursor of a controller steps over the board zones (CGP_AREA_*), four
 * slots per zone. A card pile can also be walked: cursorMoveStep[0] is the
 * player's discard pile and cursorMoveStep[1] the enemy's.
 */
#define CGP_PILE_BOARD 0              /* the cursor is on the board */
#define CGP_PILE_TOP 1                /* the cursor sits on the pile and shows its top card */
#define CGP_PILE_BROWSE 2             /* the cards of the pile are walked one by one */

/*
 * Cursor slot that stands for a pile: slot 3 of the player's hand zone (the
 * `ld` at 0x00a26c14 compares zone and slot with 0x00090003 << 32) and slot 0
 * of the enemy's hand zone (the `ld` at 0x00a26c64 requires both to be zero).
 */
#define CGP_PLAYER_PILE_SLOT 3

/* Highlight of the cursor over a hand (`lui $1,0x3E33`, 0x00a26db4). */
#define CGP_HAND_HIGHLIGHT 0.175f

/* Card id bits 8..11 are cleared before the preview draws the card (`andi $2,$3,0xF0FF`, 0x00a270e0). */
#define CGP_CARD_ID_MASK 0xF0FF

/*
 * Card preview, shown while the preview button is held: the card under the
 * cursor in the middle, the card stacked under it on the right and the card
 * the side is playing on the left. CardDisp's last argument is the display
 * mode: CGP_CARD_MODE_FULL draws the card at full size with its text,
 * CGP_CARD_MODE_ICON a plain card. The two side cards are shown small and
 * enlarged while their zoom button is held (CGP_PAD_ZOOM_LEFT/RIGHT); small
 * they get a frame model, the enlarged ones sit nearer the middle.
 */
#define CGP_CARD_MODE_ICON 0
#define CGP_CARD_MODE_FULL 5
#define CGP_PREVIEW_FULL_Y 0.369f     /* `lui $1,0x3EBC`, 0x00a270f4 */
#define CGP_PREVIEW_FULL_Z 0.5f       /* `lui $1,0x3F00`, 0x00a27100 */
#define CGP_PREVIEW_FULL_SCALE 1.1f   /* `lui $1,0x3F8C`, 0x00a270e4 */
#define CGP_PREVIEW_SMALL_Y 0.096f    /* `lui $1,0x3DC4`, 0x00a27164 */
#define CGP_PREVIEW_SMALL_Z 0.48f     /* `lui $1,0x3EF5`, 0x00a27170 */
#define CGP_PREVIEW_SMALL_SCALE 0.55f /* `lui $1,0x3F0C`, 0x00a27154 */
#define CGP_PREVIEW_SIDE_X 1.904f     /* small side cards stand this far from the middle (`lui $1,0x3FF3`, 0x00a271f8) */
#define CGP_PREVIEW_ZOOMED_X 0.9f     /* enlarged side cards (`lui $1,0x3F66`, 0x00a2724c) */
#define CGP_PREVIEW_FRAME_Z 0.29f     /* frame model behind a small side card (`lui $1,0x3E94`, 0x00a272c4) */
#define CGP_PREVIEW_FRAME_TEXTURE CARD_MODEL_AT(0x39800)  /* `lui $4,0x193`, 0x00a272fc */
#define CGP_PREVIEW_FRAME_RIGHT CARD_MODEL_AT(0x29800)    /* 0x00a27308 */
#define CGP_PREVIEW_FRAME_LEFT CARD_MODEL_AT(0x2B000)     /* 0x00a27448 */

/*
 * Spacing of the cards of a row: the distance from its first to its last
 * position (entries 0 and 3 of the position table) divided by the gaps between
 * count cards. A pile is placed as a row of one card, whose gap count is -1.
 */
static inline float CGPRowSpacing(float pos[][3], s32 count)
{
    return (pos[0][0] - pos[3][0]) / (count - 1);
}

/*
 * Puts the cursor on the table position of its zone and slot (zones 1..8, four
 * slots each) and resolves the zone's highlight (CGPCMSub).
 */
static inline void CGPCursorToSlot(CardGameWork *work)
{
    float *slot;

    CGPCMSub(work);
    slot = CursorTbl_00A49B58[(work->cursorZone - 1) * 4 + work->cursorIndex];
    work->cursorPos[0] = slot[0];
    work->cursorPos[1] = slot[1];
    work->cursorPos[2] = slot[2];
}

/*
 * Moves the cursor of one controller (side 0 or 1) over the board zones and
 * the two card piles and draws the card preview while the preview button is
 * held. Returns 1 when the preview was drawn.
 *
 * With a pile walked (cursorMoveStep) the pile's top card or the card under
 * the pile cursor is previewed; otherwise the pad steps the zone (up/down) and
 * the slot (left/right) and CGPCursor2Pos supplies the card under the cursor.
 * Entering the player's pile slot (zone 9, slot 3) or the enemy's (zone 0,
 * slot 0) starts walking that pile when it holds cards. A move that starts a
 * pile leaves the position's zone unset; the original reads it anyway.
 */
s32 CGPCursorMove2(s32 side, CardGameWork *work)
{
    CGPCursorPosition position;
    CardPlaySide *ownSide;
    u16 held;
    u16 repeat;
    u16 pressed;
    s32 canPreview = 1;

    ownSide = &work->player;
    held = work->command.input[0].held;
    pressed = work->command.input[0].pressed;
    repeat = work->command.input[0].repeat;
    if (side != 0) {
        held = work->command.input[1].held;
        ownSide = &work->enemy;
        pressed = work->command.input[1].pressed;
        repeat = work->command.input[1].repeat;
    }
    if (work->cursorMoveStep[0] != 0) {
        s32 junkCnt;

        junkCnt = CardPlayJunkCnt((CardPlayHand *) &work->player);
        work->cursorPos[0] = JunkPos1P[0][0];
        work->cursorPos[1] = JunkPos1P[0][1];
        work->cursorPos[2] = JunkPos1P[0][2];
        switch (work->cursorMoveStep[0]) {
        case CGP_PILE_TOP:
            if (pressed & CGP_PAD_UP) {
                work->cursorMoveStep[0] = CGP_PILE_BOARD;
                work->cursorZone--;
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
            } else if (pressed & CGP_PAD_CONFIRM) {
                work->junkPick[0] = 0;
                work->cursorMoveStep[0] = CGP_PILE_BROWSE;
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                CardPlayCleanJunk(&work->player);
            }
            position.stackedCard = -1;
            position.cardId = work->player.junk[junkCnt - 1];
            break;
        case CGP_PILE_BROWSE:
            if (repeat & CGP_PAD_LEFT) {
                if (work->junkPick[0] != 0) {
                    work->junkPick[0]--;
                    xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                }
            } else if (repeat & CGP_PAD_RIGHT) {
                if (work->junkPick[0] < junkCnt - 1) {
                    work->junkPick[0]++;
                    xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                }
            }
            if (pressed & CGP_PAD_CANCEL) {
                work->cursorMoveStep[0] = CGP_PILE_TOP;
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            }
            CGCPJunkDispSub(&work->player, work->junkPick[0]);
            position.cardId = work->player.junk[work->junkPick[0]];
            position.stackedCard = -1;
            break;
        }
    } else if (work->cursorMoveStep[1] != 0) {
        CardPlaySide *enemy;
        s32 junkCnt;

        enemy = &work->enemy;
        junkCnt = CardPlayJunkCnt((CardPlayHand *) enemy);
        work->cursorPos[0] = JunkPos2P[0][0];
        work->cursorPos[1] = JunkPos2P[0][1];
        work->cursorPos[2] = JunkPos2P[0][2];
        switch (work->cursorMoveStep[1]) {
        case CGP_PILE_TOP:
            if (pressed & CGP_PAD_DOWN) {
                work->cursorMoveStep[1] = CGP_PILE_BOARD;
                work->cursorZone++;
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
            } else if (pressed & CGP_PAD_CONFIRM) {
                work->junkPick[1] = 0;
                work->cursorMoveStep[1] = CGP_PILE_BROWSE;
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                CardPlayCleanJunk(enemy);
            }
            position.stackedCard = -1;
            position.cardId = work->enemy.junk[junkCnt - 1];
            break;
        case CGP_PILE_BROWSE:
            if (repeat & CGP_PAD_LEFT) {
                if (work->junkPick[1] != 0) {
                    work->junkPick[1]--;
                    xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                }
            } else if (repeat & CGP_PAD_RIGHT) {
                if (work->junkPick[1] < junkCnt - 1) {
                    work->junkPick[1]++;
                    xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                }
            }
            if (pressed & CGP_PAD_CANCEL) {
                work->cursorMoveStep[1] = CGP_PILE_TOP;
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            }
            CGCPJunkDispSub(enemy, work->junkPick[1]);
            position.cardId = work->enemy.junk[work->junkPick[1]];
            position.stackedCard = -1;
            break;
        }
    } else {
        float spacing;
        s16 prevZone;
        s16 area;
        s32 flag;
        s32 started;

        if (pressed & CGP_PAD_DIRECTIONS) {
            xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
        }
        prevZone = work->cursorZone;
        area = CGPCursor2Area(work);
        /* flag: the cursor stands in one of the two middle columns of a board row, which up/down skips */
        flag = area == CGP_AREA_ENEMY_BATTLE || area == CGP_AREA_PLAYER_BATTLE;
        if (pressed & CGP_PAD_UP) {
            work->cursorZone--;
            if (flag && work->cursorZone != 0) {
                work->cursorZone--;
            }
        }
        if (pressed & CGP_PAD_DOWN) {
            work->cursorZone++;
            if (flag && work->cursorZone != CGP_AREA_PLAYER_HAND) {
                work->cursorZone++;
            }
        }
        if (work->cursorZone < 0) {
            work->cursorZone = CGP_AREA_PLAYER_HAND;
        }
        if (work->cursorZone >= 10) {
            work->cursorZone = 0;
        }
        started = 0;
        if (prevZone != CGP_AREA_PLAYER_HAND && work->cursorZone == CGP_AREA_PLAYER_HAND &&
            work->cursorIndex == CGP_PLAYER_PILE_SLOT) {
            s32 junkCnt = CardPlayJunkCnt((CardPlayHand *) &work->player);

            if (junkCnt != 0) {
                started++;
                work->cursorMoveStep[0] = CGP_PILE_TOP;
                position.cardId = work->player.junk[junkCnt - 1];
            }
        }
        if (prevZone != CGP_AREA_ENEMY_HAND && work->cursorZone == CGP_AREA_ENEMY_HAND &&
            work->cursorIndex == 0) {
            s32 junkCnt = CardPlayJunkCnt((CardPlayHand *) &work->enemy);

            if (junkCnt != 0) {
                work->cursorMoveStep[1] = CGP_PILE_TOP;
                started++;
                position.cardId = work->enemy.junk[junkCnt - 1];
            }
        }
        if (started == 0) {
            area = CGPCursor2Area(work);
            /*
             * flag now: the hand row is mirrored by the rotated stage, so left
             * and right swap. Phases 1-3 of a side use the player hand row
             * (zone 9) and phase 4 the other one (zone 0), see
             * CGP_SIDE1_FIRST_PHASE for the phases of the second side.
             */
            flag = 0;
            switch ((s16) work->command.phase) {
            case 1:
            case 2:
            case 3:
            case 11:
                if (work->cursorZone == CGP_AREA_PLAYER_HAND && work->rotStageId != 0) {
                    flag = 1;
                }
                break;
            case 4:
            case 8:
            case 9:
            case 10:
                if (work->cursorZone == CGP_AREA_ENEMY_HAND) {
                    flag = work->rotStageId != 0;
                }
                break;
            }
            if (flag) {
                if (pressed & CGP_PAD_RIGHT) {
                    work->cursorIndex--;
                }
                if (pressed & CGP_PAD_LEFT) {
                    work->cursorIndex++;
                }
            } else {
                if (pressed & CGP_PAD_RIGHT) {
                    work->cursorIndex++;
                }
                if (pressed & CGP_PAD_LEFT) {
                    work->cursorIndex--;
                }
            }
            if (work->cursorZone == CGP_AREA_ENEMY_HAND) {
                CardPlaySide *enemy = &work->enemy;
                s32 handCnt = CardPlayHandCnt(&enemy->hand);
                s32 junkCnt = CardPlayJunkCnt((CardPlayHand *) enemy);

                work->highlightX = work->highlightY = CGP_HAND_HIGHLIGHT;
                work->highlightScaleX = work->highlightScaleY = 1.0f;
                if (handCnt == 0 && junkCnt == 0) {
                    work->cursorZone = 1;
                    CGPCursorToSlot(work);
                } else {
                    if (work->cursorIndex < 0) {
                        work->cursorIndex = handCnt - 1;
                    }
                    if (work->cursorIndex >= handCnt) {
                        work->cursorIndex = 0;
                    }
                    /* the enemy's hand is laid out from the right: slot 0 is the last card */
                    work->cursorPos[0] = HandPos2P[0][0] -
                        (HandPos2P[0][0] - HandPos2P[3][0]) / (handCnt - 1) * (handCnt - work->cursorIndex - 1);
                    work->cursorPos[1] = HandPos2P[0][1];
                    work->cursorPos[2] = HandPos2P[0][2];
                }
            } else if (work->cursorZone == CGP_AREA_PLAYER_HAND) {
                s32 handCnt = CardPlayHandCnt(&work->player.hand);
                s32 junkCnt = CardPlayJunkCnt((CardPlayHand *) &work->player);

                work->highlightX = work->highlightY = CGP_HAND_HIGHLIGHT;
                work->highlightScaleX = work->highlightScaleY = 1.0f;
                if (handCnt == 0 && junkCnt == 0) {
                    work->cursorZone = 1;
                    CGPCursorToSlot(work);
                } else {
                    if (prevZone != work->cursorZone) {
                        /* coming from another zone: the left side lands on the first card */
                        if (work->cursorIndex < 3) {
                            work->cursorIndex = 0;
                        } else {
                            work->cursorIndex = handCnt;
                        }
                    }
                    if (work->cursorIndex < 0) {
                        work->cursorIndex = handCnt - 1;
                    }
                    if (work->cursorIndex >= handCnt) {
                        work->cursorIndex = 0;
                    }
                    if (handCnt != 0 && work->cursorIndex < handCnt) {
                        spacing = CGPRowSpacing(HandPos1P, handCnt);
                        work->cursorPos[0] = HandPos1P[0][0] - spacing * work->cursorIndex;
                        work->cursorPos[1] = HandPos1P[0][1];
                        work->cursorPos[2] = HandPos1P[0][2];
                    } else {
                        spacing = CGPRowSpacing(JunkPos1P, 0);
                        work->cursorPos[0] = JunkPos1P[0][0] - spacing * (work->cursorIndex - handCnt);
                        work->cursorPos[1] = JunkPos1P[0][1];
                        work->cursorPos[2] = JunkPos1P[0][2];
                    }
                }
            } else {
                CGPCursorToSlot(work);
            }
        }
        if (work->cursorMoveAny == 0) {
            CGPCursor2Pos(&position, work);
        }
        /*
         * A card in the hand row that is not shown (the opponent's, or the
         * player's in the phases the zone-0 row is the active one) is not
         * previewed.
         */
        switch ((s16) work->command.phase) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 11:
            if (position.zone == CGP_AREA_ENEMY_HAND) {
                canPreview = 0;
            }
            break;
        default:
            canPreview = position.zone != CGP_AREA_PLAYER_HAND;
            break;
        }
    }
    if (held & CGP_PAD_PREVIEW) {
        if (canPreview && position.cardId >= 0) {
            Matrix4 matrix;
            float place[4];
            float scale[4];
            s32 mode;

            position.cardId &= CGP_CARD_ID_MASK;
            place[0] = 0.0f;
            place[1] = CGP_PREVIEW_FULL_Y;
            place[2] = CGP_PREVIEW_FULL_Z;
            place[3] = 1.0f;
            scale[1] = CGP_PREVIEW_FULL_SCALE;
            scale[0] = CGP_PREVIEW_FULL_SCALE;
            scale[3] = 1.0f;
            scale[2] = 1.0f;
            mode = CGP_CARD_MODE_FULL;
            if ((position.stackedCard >= 0 && (held & CGP_PAD_ZOOM_RIGHT)) ||
                (ownSide->battle[0].cmdView.playedCard >= 0 && (held & CGP_PAD_ZOOM_LEFT))) {
                /* a side card is enlarged: the middle card shrinks to make room */
                mode = CGP_CARD_MODE_ICON;
                place[1] = CGP_PREVIEW_SMALL_Y;
                place[2] = CGP_PREVIEW_SMALL_Z;
                scale[1] = CGP_PREVIEW_SMALL_SCALE;
                scale[0] = CGP_PREVIEW_SMALL_SCALE;
            }
            xglMatrixUnit(matrix);
            xglMatrixTrans(matrix, (const float (*)[4]) matrix, place);
            xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
            CardDisp(matrix, position.cardId, 1, mode);
            if (position.stackedCard >= 0) {
                mode = CGP_CARD_MODE_ICON;
                place[0] = CGP_PREVIEW_SIDE_X;
                place[1] = CGP_PREVIEW_SMALL_Y;
                place[2] = CGP_PREVIEW_SMALL_Z;
                scale[1] = CGP_PREVIEW_SMALL_SCALE;
                scale[0] = CGP_PREVIEW_SMALL_SCALE;
                scale[3] = 1.0f;
                scale[2] = 1.0f;
                if (held & CGP_PAD_ZOOM_RIGHT) {
                    mode = CGP_CARD_MODE_FULL;
                    place[0] = CGP_PREVIEW_ZOOMED_X;
                    place[1] = CGP_PREVIEW_FULL_Y;
                    place[2] = CGP_PREVIEW_FULL_Z;
                    scale[1] = CGP_PREVIEW_FULL_SCALE;
                    scale[0] = CGP_PREVIEW_FULL_SCALE;
                }
                xglMatrixUnit(matrix);
                xglMatrixTrans(matrix, (const float (*)[4]) matrix, place);
                xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
                CardDisp(matrix, position.stackedCard, 1, mode);
                if (!(held & CGP_PAD_ZOOM_RIGHT)) {
                    place[0] = CGP_PREVIEW_SIDE_X;
                    place[1] = CGP_PREVIEW_SMALL_Y;
                    place[2] = CGP_PREVIEW_FRAME_Z;
                    xglMatrixUnit(matrix);
                    xglMatrixTrans(matrix, (const float (*)[4]) matrix, place);
                    nmlModelSetPlace(matrix);
                    nmlModelSetTexture(CGP_PREVIEW_FRAME_TEXTURE);
                    nmlModelEntryCard(CGP_PREVIEW_FRAME_RIGHT);
                }
            }
            if (ownSide->battle[0].cmdView.playedCard >= 0) {
                mode = CGP_CARD_MODE_ICON;
                place[0] = -CGP_PREVIEW_SIDE_X;
                place[1] = CGP_PREVIEW_SMALL_Y;
                place[2] = CGP_PREVIEW_SMALL_Z;
                scale[1] = CGP_PREVIEW_SMALL_SCALE;
                scale[0] = CGP_PREVIEW_SMALL_SCALE;
                scale[3] = 1.0f;
                scale[2] = 1.0f;
                if (held & CGP_PAD_ZOOM_LEFT) {
                    mode = CGP_CARD_MODE_FULL;
                    place[0] = -CGP_PREVIEW_ZOOMED_X;
                    place[1] = CGP_PREVIEW_FULL_Y;
                    place[2] = CGP_PREVIEW_FULL_Z;
                    scale[1] = CGP_PREVIEW_FULL_SCALE;
                    scale[0] = CGP_PREVIEW_FULL_SCALE;
                }
                xglMatrixUnit(matrix);
                xglMatrixTrans(matrix, (const float (*)[4]) matrix, place);
                xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
                CardDisp(matrix, ownSide->battle[0].cmdView.playedCard, 1, mode);
                if (!(held & CGP_PAD_ZOOM_LEFT)) {
                    place[0] = -CGP_PREVIEW_SIDE_X;
                    place[1] = CGP_PREVIEW_SMALL_Y;
                    place[2] = CGP_PREVIEW_FRAME_Z;
                    xglMatrixUnit(matrix);
                    xglMatrixTrans(matrix, (const float (*)[4]) matrix, place);
                    nmlModelSetPlace(matrix);
                    nmlModelSetTexture(CGP_PREVIEW_FRAME_TEXTURE);
                    nmlModelEntryCard(CGP_PREVIEW_FRAME_LEFT);
                }
            }
            return 1;
        }
    }
    return 0;
}

void CGPCursorMove(s32 flag, CardGameWork *work)
{
    CGPCursorMove2(flag, work);
}

/*
 * Put the cursor on one board area, undoing any movement in progress. The area
 * codes are the ones CGPCursor2Area returns, and slot picks the column inside
 * the area: areas 0 and 1 are the enemy hand, 2 and 3 the last and the first
 * column of the enemy board row, 4 its two middle columns, 5 the two middle
 * columns of the player board row, 6 and 7 its first and last column, and 8
 * and 9 the player hand. The cursor then takes the screen position of the hand
 * it stands in.
 */
void CGPSetCursor(CardGameWork *work, u16 area, u16 slot)
{
    work->cursorMoveStep[0] = 0;
    work->cursorMoveStep[1] = 0;
    switch (area) {
    case 0:
    case 1:
        work->cursorZone = 0;
        work->cursorIndex = 0;
        break;
    case 2:
        work->cursorIndex = 3;
        work->cursorZone = 4 - slot;
        break;
    case 3:
        work->cursorIndex = 0;
        work->cursorZone = 4 - slot;
        break;
    case 4:
        if (slot < 2) {
            work->cursorZone = 3;
            work->cursorIndex = 2 - (slot & 1);
        } else {
            work->cursorZone = 2;
            work->cursorIndex = 2 - (slot & 1);
        }
        break;
    case 5:
        if (slot < 2) {
            work->cursorZone = 5;
            work->cursorIndex = (slot & 1) + 1;
        } else {
            work->cursorZone = 6;
            work->cursorIndex = (slot & 1) + 1;
        }
        break;
    case 6:
        work->cursorIndex = 0;
        work->cursorZone = slot + 5;
        break;
    case 7:
        work->cursorIndex = 3;
        work->cursorZone = slot + 5;
        break;
    case 8:
    case 9:
        work->cursorIndex = 0;
        work->cursorZone = 9;
        break;
    }
    CGPCMSub(work);
    if (work->cursorZone == 9) {
        work->cursorPos[0] = HandPos1P[0][0];
        work->cursorPos[1] = HandPos1P[0][1];
        work->cursorPos[2] = HandPos1P[0][2];
    } else {
        work->cursorPos[0] = HandPos2P[0][0];
        work->cursorPos[1] = HandPos2P[0][1];
        work->cursorPos[2] = HandPos2P[0][2];
    }
}

const char D_00A4E4F0[16] = "Yama[%2d]:";
const char D_00A4E500[8] = "%3d,";
const char D_00A4E508[8] = "\n";
const char D_00A4E510[16] = "Junk[%2d]:";
const char D_00A4E520[16] = "Sute[%2d]:";
const char D_00A4E530[8] = "\x0b\x0d";
const char D_00A4E538[8] = "\x19\x03";
s32 CGPChkPlayable(s16 command)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        if (command == CGPCPTbl[i]) {
            return 1;
        }
    }
    return 0;
}

/* Operation commands CGPChkOpePlayable checks the board for (switch cases 0x8B, 0x8D, 0x8F). */
#define CGP_CMD_TARGET_LIFE 139          /* needs a card on a battle slot of either side */
#define CGP_CMD_OPPONENT_OPERATION 141   /* needs an operation card on the opponent's side */
#define CGP_CMD_BOARD_LIFE_LOSS 143      /* needs a card on a disposal or battle slot of either side */

s32 CGPChkOpePlayable(s32 side, CardGameWork *work, s16 command)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    s32 playable;
    s32 found;
    s32 i;

    own = &work->player;
    foe = &work->enemy;
    playable = CGPChkPlayable(command);
    if (playable == 0) {
        return 0;
    }
    /* The two arms below are identical and emit no code. They stand for a join point
     * between the playability test and the switch. The original object code keeps the
     * player and enemy side base addresses in their own registers ($17 = work + 0x45C
     * at 0x00a27680, $18 = work + 0x2F08 at 0x00a27674; the first sits in the delay slot of
     * the CGPChkPlayable call at 0x00a2767c and both are used by the switch cases); without
     * the join the compiler folds those offsets into the work pointer and the registers are
     * lost. */
    if (command == 0) {
        found = 0;
        i = 0;
    } else {
        found = 0;
        i = 0;
    }
    switch (command) {
    case CGP_CMD_TARGET_LIFE:
        found = 0;
        for (i = 0; i < 4; i++) {
            s32 ownCard = own->battle[i].battle.layers[0].cardId;
            if (ownCard >= 0) {
                found = 1;
                break;
            }
            if (foe->battle[i].battle.layers[0].cardId >= 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
        return 0;
    case CGP_CMD_OPPONENT_OPERATION:
        found = 0;
        for (i = 0; i < 4; i++) {
            if (foe->operations[i] >= 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
        return 0;
    case CGP_CMD_BOARD_LIFE_LOSS:
        found = 0;
        for (i = 0; i < 4; i++) {
            s32 ownCard = own->disposal[i].battle.layers[0].cardId;
            if (ownCard >= 0) {
                found = 1;
                break;
            }
            if (foe->disposal[i].battle.layers[0].cardId >= 0) {
                found = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            s32 ownCard = own->battle[i].battle.layers[0].cardId;
            if (ownCard >= 0) {
                found = 1;
                break;
            }
            if (foe->battle[i].battle.layers[0].cardId >= 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
        return 0;
    default:
        return 1;
    }
    return 1;
}

void CardPlayClearEndflg(CardPlaySide *side)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        side->disposal[i].battle.flags &= ~1;
    }
    for (i = 0; i < 4; i++) {
        side->operationFlags[i] &= ~1;
    }
}

/*
 * Cards a command reserves from the draw pile, zero once the command's own
 * card is already in play on the side that would play it: command 37 is free
 * while card 5 or card 12 stands on the board, commands 4 and 39 while card 0,
 * card 1 or card 7 does.
 */
s32 CGPChkCardPlayCost(CardGameWork *work, CardHand *hand, s32 command)
{
    s32 cost;

    cost = work->definitions[command].reserveCost;
    switch (command) {
    case 4:
    case 39:
        if (CardPlayCntBtl(hand, 0) || CardPlayCntBtl(hand, 1) ||
            CardPlayCntBtl(hand, 7)) {
            cost = 0;
        }
        break;
    case 37:
        if (CardPlayCntBtl(hand, 5) || CardPlayCntBtl(hand, 12)) {
            cost = 0;
        }
        break;
    }
    return cost;
}

void CardPlayDisployment(u16 side, CardGameWork *workAddress, CardPlayField *field, s32 index)
{
    CardGameWork *work = workAddress;
    CardDefinition *defs;
    s32 cost;
    s32 i;
    s32 slot;

    defs = work->definitions;
    cost = 0;
    switch (defs[field->hand.maintenanceView.cards[index]].kind) {
    case 1:
        for (i = 0, slot = -1; i < 4; i++) {
            if (field->disposal[i].stack.layers[0].cardId < 0) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            return;
        }
        CardPlayCommandPlay(side, work->commandIds[field->hand.maintenanceView.cards[index]],
                            workAddress, 0, 0);
        field->disposal[slot].stack.flags = 1;
        field->disposal[slot].stack.power =
            defs[field->hand.maintenanceView.cards[index]].basePower;
        field->disposal[slot].stack.powerLimit =
            defs[field->hand.maintenanceView.cards[index]].basePower;
        field->disposal[slot].stack.layers[0].cardId = field->hand.maintenanceView.cards[index];
        field->disposal[slot].stack.layers[0].life =
            defs[field->hand.maintenanceView.cards[index]].operationDuration;
        cost = CGPChkCardPlayCost(workAddress, &field->hand.costView,
                                  field->hand.maintenanceView.cards[index]);
        field->disposal[slot].stack.position[0] = field->handMove[index].position[0];
        field->disposal[slot].stack.position[1] = field->handMove[index].position[1];
        field->disposal[slot].stack.position[2] = field->handMove[index].position[2];
        field->disposal[slot].stack.step[0] =
            (DisposePos1P[slot][0] - field->handMove[index].position[0]) / 10.0f;
        field->disposal[slot].stack.step[1] =
            (DisposePos1P[slot][1] - field->handMove[index].position[1]) / 10.0f;
        field->disposal[slot].stack.step[2] =
            (DisposePos1P[slot][2] - field->handMove[index].position[2]) / 10.0f;
        field->disposal[slot].stack.moveTimer = 10;
        field->hand.maintenanceView.cards[index] = -1;
        break;
    case 4:
        for (i = 0, slot = -1; i < 4; i++) {
            if (field->operations[i] < 0) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            return;
        }
        field->operationFlags[slot] |= 1;
        field->operations[slot] = field->hand.maintenanceView.cards[index];
        field->operationDuration[slot] =
            defs[field->hand.maintenanceView.cards[index]].operationDuration;
        cost = CGPChkCardPlayCost(workAddress, &field->hand.costView,
                                  field->hand.maintenanceView.cards[index]);
        field->operationMove[slot].position[0] = field->handMove[index].position[0];
        field->operationMove[slot].position[1] = field->handMove[index].position[1];
        field->operationMove[slot].position[2] = field->handMove[index].position[2];
        field->operationMove[slot].step[0] =
            (OperationPos1P[slot][0] - field->handMove[index].position[0]) / 10.0f;
        field->operationMove[slot].step[1] =
            (OperationPos1P[slot][1] - field->handMove[index].position[1]) / 10.0f;
        field->operationMove[slot].step[2] =
            (OperationPos1P[slot][2] - field->handMove[index].position[2]) / 10.0f;
        field->operationMove[slot].moveTimer = 10;
        field->hand.maintenanceView.cards[index] = -1;
        break;
    }

    for (i = 0; i < cost; i++) {
        CardPlayCostCard(&field->hand.maintenanceView);
    }
    CardPlayCleanHand(&field->hand.maintenanceView);
}


extern s32 CardPlayChkCost(CardDefinition *definitions, CardPlaySide *side,
                           s32 cardId, CardGameWork *work);
extern s32 CardPlayChkCondition(CardDefinition *definitions, CardPlaySide *side,
                                s32 cardId, CardGameWork *work);
extern s32 CardPlayCntLight(CardGameWork *work, CardPlaySide *side);
extern s32 CardEquipCntHeavy(CardGameWork *work, CardPlaySide *side, s32 cardId);

/*
 * Why one side may not play the card at the given hand position, or zero when
 * it may. Kind 2 is never playable (11) and kinds 1 and 4 need enough play
 * points (12); a card whose definition carries any of the flags 0x64 also has
 * to pass the equipment checks, where flag 4 has no free slot (15), flag 0x40
 * needs a light slot (13) and every other such card a heavy one (14), and 16
 * means the card is equipment the slot search accepted.
 */
s32 CGPDispoSub0(s32 side, CardGameWork *work, s32 index)
{
    s32 error;
    CardPlaySide *ownSide;
    ownSide = &work->player;
    if (side != 0) {
        ownSide = &work->enemy;
    }
    error = 0;
    if (work->definitions[ownSide->hand.cards[index]].kind == CGP_CARD_KIND_DISCARD_ONLY) {
        error = 11;
    } else {
        if (work->definitions[ownSide->hand.cards[index]].kind == CGP_CARD_KIND_COMMAND) {
            if (ownSide->hand.playPoints[0] < work->definitions[ownSide->hand.cards[index]].playCost) {
                error = 12;
            }
        } else if (work->definitions[ownSide->hand.cards[index]].kind == CGP_CARD_KIND_OPERATION) {
            if (ownSide->hand.playPoints[1] < work->definitions[ownSide->hand.cards[index]].playCost) {
                error = 12;
            }
        }
        if (error == 0) {
            if (work->definitions[ownSide->hand.cards[index]].flags & 0x64) {
                error = CardPlayChkCost(work->definitions, ownSide, ownSide->hand.cards[index], work);
                if (error == 0) {
                    error = CardPlayChkCondition(work->definitions, ownSide,
                                                 ownSide->hand.cards[index], work);
                    if (error == 0) {
                        error = 16;
                        if (work->definitions[ownSide->hand.cards[index]].flags & 4) {
                            error = 15;
                        } else if (work->definitions[ownSide->hand.cards[index]].flags & 0x40) {
                            if (CardPlayCntLight(work, ownSide) != 0) {
                                error = 13;
                            }
                        } else if (CardEquipCntHeavy(work, ownSide, ownSide->hand.cards[index]) != 0) {
                            error = 14;
                        }
                    }
                }
            } else {
                error = CardPlayChkCost(work->definitions, ownSide, ownSide->hand.cards[index], work);
                if (error == 0) {
                    error = CardPlayChkCondition(work->definitions, ownSide,
                                                 ownSide->hand.cards[index], work);
                }
            }
        }
    }
    return error;
}

/*
 * Plays the hand card at the given position for the side whose controller just
 * pressed the confirm button, and returns the result code the caller acts on:
 * zero (played, or nothing to do), 13..15 (CGPDispoSub0's codes for a card that
 * still needs a board slot, answered with the decide sound) or 15 as well when
 * the card paired with the played one (the id 7 below it) stands on the battle
 * row. Cards 7, 9 and 10..13 are refused when the play points fall short (12)
 * or when the card, or the card paired with it, is already in the disposal row
 * (5 and 9); a refusal shows its error message with the buzzer and returns zero.
 */
s32 CGPDisposeSub(s32 side, CardGameWork *work, s32 index)
{
    CardPlaySide *ownSide;
    u16 pressed;
    s32 result;
    s32 pairedOnBattle;
    s32 i;

    result = 0;
    ownSide = &work->player;
    pressed = work->command.input[0].pressed;
    if (side != 0) {
        pressed = work->command.input[1].pressed;
        ownSide = &work->enemy;
    }
    if (index < CardPlayHandCnt(&ownSide->hand) && (pressed & CGP_PAD_CONFIRM)) {
        pairedOnBattle = 0;
        switch (ownSide->hand.cards[index]) {
        case 10:
        case 11:
        case 12:
        case 13:
            result = CGPDispoSub0(side, work, index);
            if (result == 5 || result == 1) {
                result = 0;
            }
            /* fall through */
        case 7:
        case 9:
            if (result == 0) {
                if (ownSide->hand.playPoints[0] <
                    work->definitions[ownSide->hand.cards[index]].playCost) {
                    result = CGP_ERR_NOT_ENOUGH_POINTS;
                } else {
                    for (i = 0; i < 4; i++) {
                        s32 slotCard = ownSide->disposal[i].battle.layers[0].cardId;

                        if (ownSide->hand.cards[index] - CGP_PAIRED_CARD_OFFSET == slotCard) {
                            result = 9;
                            break;
                        }
                        if (ownSide->hand.cards[index] == ownSide->disposal[i].battle.layers[0].cardId) {
                            result = 5;
                            break;
                        }
                    }
                    for (i = 0; i < 4; i++) {
                        if (ownSide->hand.cards[index] - CGP_PAIRED_CARD_OFFSET ==
                            ownSide->battle[i].battle.layers[0].cardId) {
                            pairedOnBattle = 1;
                            result = 15;
                            break;
                        }
                    }
                }
            }
        }
        if (!pairedOnBattle) {
            if (result == 0) {
                result = CGPDispoSub0(side, work, index);
            }
            switch (result) {
            case 0:
                if (work->definitions[ownSide->hand.cards[index]].kind == CGP_CARD_KIND_COMMAND) {
                    ownSide->hand.playPoints[0] -=
                        work->definitions[ownSide->hand.cards[index]].playCost;
                } else {
                    ownSide->hand.playPoints[1] -=
                        work->definitions[ownSide->hand.cards[index]].playCost;
                }
                /* CardPlayDisployment views the side record as a CardPlayField. */
                CardPlayDisployment(side, work, (CardPlayField *) ownSide, index);
                /* fall through */
            case 13:
            case 14:
            case 15:
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                return result;
            default:
                if (result == CGP_ERR_NOT_ENOUGH_POINTS) {
                    if (work->definitions[ownSide->hand.cards[index]].kind == CGP_CARD_KIND_COMMAND) {
                        CGPSetErrorMess(side, work, CGP_ERR_NOT_ENOUGH_POINTS);
                    } else {
                        CGPSetErrorMess(side, work, 10);
                    }
                } else {
                    CGPSetErrorMess(side, work, result);
                }
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                result = 0;
                break;
            }
        }
    }
    return result;
}

/*
 * Moves the cursor (CGPCursorMove2) and resolves the card at its new
 * position (CGPCursor2Pos), then clears the resolved card id to -1 unless
 * the zone is 2 or 4 (nonzero flag) or 5 or 6 (zero flag). `unused` (the
 * second parameter) is forwarded by every caller but never read here.
 */
s16 CGPMoveSub(s32 flag, void *unused, CardGameWork *work)
{
    CGPCursorPosition position;

    position.index = -1;
    CGPCursorMove2(flag, work);
    CGPCursor2Pos(&position, work);
    if (flag == 0) {
        if (position.zone != 5 && position.zone != 6) {
            position.cardId = -1;
        }
    } else {
        if (position.zone != 4 && position.zone != 2) {
            position.cardId = -1;
        }
    }
    return position.cardId;
}

/*
 * Raise or lower an attack's power before the damage is worked out. Operation
 * cards in play on either side act through the attacking card's flags, and
 * three attacking cards act on their own: 15 counts the cards in play on its
 * own board that carry flag 8, 14 looks for card 79 there and 76 plays command
 * 73. A power that fell below zero counts as none.
 */
s32 CGPBattleBeforeEffect(s32 side, CardGameWork *work, s32 power, CardLayerStack *stack,
                          s32 slot)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    s32 cardId;
    s32 i;

    cardId = stack->layers[0].cardId;
    if (side == 0) {
        own = &work->player;
        foe = &work->enemy;
    } else {
        own = &work->enemy;
        foe = &work->player;
    }

    if (CardPlayCntOpe(own, 120) != 0 || CardPlayCntOpe(foe, 120) != 0) {
        if (work->definitions[cardId].flags & 4) {
            power--;
        }
    }

    i = CardPlayCntOpe(own, 123);
    if (i != 0) {
        if (work->definitions[cardId].flags & 1) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }

    i = CardPlayCntOpe(own, 134) + CardPlayCntOpe(foe, 134);
    if (i != 0) {
        if (work->definitions[cardId].flags & 4) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }

    i = CardPlayCntOpe(own, 144) + CardPlayCntOpe(foe, 144);
    if (i != 0) {
        while (i > 0) {
            i--;
            power++;
        }
    }

    switch (cardId) {
    case 15:
        if (stack->layers[1].cardId != 53 && stack->layers[1].cardId != 48) {
            for (i = 0; i < 4; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0) {
                    if (work->definitions[own->battle[i].battle.layers[0].cardId].flags & 8) {
                        power++;
                    }
                }
            }
        }
        break;
    case 14:
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId == 79) {
                power++;
                break;
            }
        }
        break;
    case 76:
        CardPlayCommandPlay(side, 73, work, (s16) slot, 0);
        break;
    }

    return (power < 0) ? 0 : power;
}



/*
 * Extra battle effects the attacking card scores, added to the effect count
 * CGPBattleCalcuration keeps. A card whose stack carries flag 0x80 scores
 * nothing at all. Cards 3, 8 and 10 score one for card 79 standing anywhere on
 * their own battle row; they and card 26 score one more when the defending
 * card's definition carries flag 4, and any attacker stacking card 42 scores
 * three for the same definition flag.
 */
s32 CGPBattleAttackEffect(s32 side, CardGameWork *work, s32 effect,
                          CardLayerStack *attacker, CardLayerStack *defender)
{
    CardPlaySide *own;
    s16 attackCard;
    s16 i;

    own = &work->player;
    if (side != 0) {
        own = &work->enemy;
    }
    attackCard = attacker->layers[0].cardId;
    if (attacker->flags & 0x80) {
        effect = 0;
    } else if (defender != 0) {
        switch (attackCard) {
        case 3:
        case 8:
        case 10:
            for (i = 0; i < 4; i++) {
                if (own->battle[i].battle.layers[0].cardId == 79) {
                    effect++;
                    break;
                }
            }
            if (defender->layers[0].cardId >= 0 &&
                (work->definitions[defender->layers[0].cardId].flags & 4)) {
                effect++;
            }
            break;
        case 26:
            if (defender->layers[0].cardId >= 0 &&
                (work->definitions[defender->layers[0].cardId].flags & 4)) {
                effect++;
            }
            break;
        }
        if (attacker->layers[1].cardId == 42 &&
            defender->layers[0].cardId >= 0 &&
            (work->definitions[defender->layers[0].cardId].flags & 4)) {
            effect += 3;
        }
    }
    return effect;
}

/*
 * Attack power left after the defender's protections: it drops by one for every
 * operation card 122 in play on the defending side when the defender's base card
 * carries definition flag 8, and by one for each defender layer that is card 44
 * or 47, or card 7 while the attacker's base card carries flag 2 (the card ids
 * are fixed cards of the card master table). Never below zero.
 */
s32 CGPBattleDiffenceEffect(s32 side, CardGameWork *work, s32 power, CardLayerStack *defender,
                            CardLayerStack *attacker)
{
    CardPlaySide *defenderSide;
    CardDefinition *defs;
    s32 cardId;
    s16 value;
    s16 i;

    defenderSide = (side == 0) ? &work->enemy : &work->player;
    value = CardPlayCntOpe(defenderSide, CGP_OPE_DEFENCE_DOWN);
    cardId = defender->layers[0].cardId;
    defs = work->definitions;
    if (defs[cardId].flags & 8) {
        for (i = 0; i < value; i++) {
            power--;
        }
    }
    for (i = 0; i < 40; i++) {
        value = defender->layers[i].cardId;
        if (value >= 0) {
            switch (value) {
            case 7:
                if (attacker->layers[0].cardId >= 0 &&
                    (defs[attacker->layers[0].cardId].flags & 2)) {
                    power--;
                }
                break;
            case 44:
            case 47:
                power--;
                break;
            }
        }
    }
    return (power < 0) ? 0 : power;
}

/*
 * What a destroyed card leaves behind. The card survives with one life left
 * while the other side has card 81 in play; cards 22, 38, 59 and 60 each play a
 * command of their own, and 60 queues the last card of the other side's junk
 * pile again when a copy of 60 is destroyed there. Once the card is really
 * gone, operation card 126 recovers as many cards as the card reserved and
 * operation card 129 plays command 75 for every side that has one in play.
 */
s32 CGPBAEsub(s32 side, CardGameWork *work, s32 power, CardLayerStack *attacker,
              CardLayerStack *defender)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    s16 *command;
    s16 foeSide;
    s16 ownSlot;
    s16 i;
    s32 card;
    s32 cardId;
    s32 found;
    s32 junkIndex;

    cardId = defender->layers[0].cardId;
    if (side == 0) {
        foeSide = 1;
        own = &work->player;
        foe = &work->enemy;
    } else {
        foeSide = 0;
        own = &work->enemy;
        foe = &work->player;
    }
    if (defender->layers[0].life <= 0) {
        defender->layers[0].life = 0;

        ownSlot = -1;
        for (i = 0; i < 4; i++) {
            if (attacker == &own->battle[i].battle) {
                ownSlot = i;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (defender == &foe->battle[i].battle) {
                break;
            }
        }

        switch (cardId) {
        case 3:
        case 8:
        case 10:
            if (CardPlayCntBtl((CardHand *) foe, 81) != 0) {
                defender->layers[0].life = 1;
                CardPlayCommandPlay(side, 71, work, foeSide, 0);
            }
            break;
        case 22:
            CardPlayCommandPlay(side, 66, work, foeSide, 0);
            break;
        case 38:
            CardPlayCommandPlay(side, 68, work, 0, 0);
            break;
        case 59:
            CardPlayCommandPlay(side, 69, work, ownSlot, 0);
            break;
        case 60:
            found = 0;
            for (i = 0; i < 4; i++) {
                if (foe->battle[i].battle.layers[0].cardId == 60 &&
                    foe->battle[i].battle.layers[0].life <= 0) {
                    found = 1;
                    break;
                }
            }
            if (found != 0) {
                /* the head of a side record is the play record of its hand */
                junkIndex = CardPlayJunkCnt((CardPlayHand *) foe) - 1;
                if (junkIndex >= 0) {
                    card = foe->junk[junkIndex];
                    if (work->definitions[card].kind == CGP_CARD_KIND_COMMAND) {
                        if ((work->definitions[card].flags & 0xB) != 0) {
                            if (CardPlayIdentityCheck(work, (CardHand *) foe,
                                                      work->definitions[card].identity) == 0) {
                                command = CPCSearchCommWk(side, work);
                                if (command != 0) {
                                    command[1] = 10;
                                    command[0] = 6;
                                    if (side != 0) {
                                        command[0] = 13;
                                    }
                                    command[2] = i;
                                    command[3] = junkIndex;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        if (defender->layers[0].life <= 0) {
            if (CardPlayCntOpe(foe, 126) != 0) {
                for (i = 0; i < work->definitions[defender->layers[0].cardId].reserveCost;
                     i++) {
                    CardPlayRecavery((CardHand *) foe);
                }
            }
            if (work->definitions[defender->layers[0].cardId].flags & 2) {
                if (CardPlayCntOpe(own, 129) != 0) {
                    CardPlayCommandPlay(side, 75, work, (s16) side, 0);
                }
                if (CardPlayCntOpe(foe, 129) != 0) {
                    CardPlayCommandPlay(side, 75, work, foeSide, 0);
                }
            }
        }
    }
    return 0;
}



/*
 * Effects that resolve once a battle is over. While the battle produced an
 * effect, the defending cards 2 and 9 mark the attacking stack, cards 30, 31,
 * 67 and 72 mark it as well, and card 0 queues command 3 for the side once the
 * attacking stack has run out of life. Both the attacking and the supporting
 * stack then run their own after-effects.
 */
void CGPBattleAfterEffect(s32 side, CardGameWork *work, s32 effect, s32 operand,
                          CardLayerStack *defender, CardLayerStack *attacker,
                          CardLayerStack *support)
{
    s16 card;

    card = defender->layers[0].cardId;
    if (card == 2 || card == 9) {
        if (effect != 0 && attacker != 0) {
            attacker->flags |= 2;
        }
    }
    if (effect != 0) {
        switch (card) {
        case 0:
            if (attacker->layers[0].life <= 0) {
                CardPlayCommandPlay(side, 3, work, operand, 0);
            }
            break;
        case 30:
        case 31:
            attacker->flags |= 2;
            break;
        case 67:
        case 72:
            attacker->flags |= 2;
            break;
        }
    }
    if (attacker != 0) {
        CGPBAEsub(side, work, effect, defender, attacker);
    }
    if (support != 0) {
        CGPBAEsub(side, work, effect, defender, support);
    }
}

/*
 * Damage one attack deals. The attacker's effects raise the power, the
 * defender's lower it, and what is left is taken off the defending card. A
 * card stacked on the attacker that pierces (34, 35, 37 or 43) lets the damage
 * that destroys the defending card pass to the card behind it, which sets the
 * destroyed code to 1 for the defending card alone and to 2 for both. The
 * damage still left over is returned.
 */
s32 CGPBattleCalcuration(s32 side, CardGameWork *work, s32 power, s32 *battleDone,
                         CardLayerStack *attacker, CardLayerStack *defender,
                         CardLayerStack *behind, s32 *destroyed)
{
    s32 damage;
    s32 taken;
    s32 pierces;
    s32 i;

    power = CGPBattleAttackEffect(side, work, power, attacker, defender);
    damage = CGPBattleDiffenceEffect(side, work, power, defender, attacker);
    *battleDone = 1;
    pierces = 0;
    if (damage != 0) {
        for (i = 39; i >= 0; i--) {
            if (attacker->layers[i].cardId >= 0) {
                if (attacker->layers[i].cardId == 34 || attacker->layers[i].cardId == 35 ||
                    attacker->layers[i].cardId == 37 || attacker->layers[i].cardId == 43) {
                    pierces = 1;
                    break;
                }
            }
        }
        if (pierces != 0 && defender->layers[0].life < damage) {
            *destroyed = 1;
            taken = defender->layers[0].life;
            defender->layers[0].life = 0;
            CardSetEffect(side, work, 11, taken, 0, defender->position, defender);
            damage -= taken;
            if (behind != 0) {
                if (behind->layers[0].life < damage) {
                    taken = behind->layers[0].life;
                    behind->layers[0].life = 0;
                    damage -= taken;
                    *destroyed = 2;
                } else {
                    taken = damage;
                    damage = 0;
                    behind->layers[0].life -= taken;
                }
                CardSetEffect(side, work, 22, 0, 0, attacker->position, behind);
                CardSetEffect(side, work, 15, taken, 0, behind->position, behind);
            }
        } else {
            CardSetEffect(side, work, 11, damage, 0, defender->position, defender);
            defender->layers[0].life -= damage;
            damage = 0;
        }
    }
    if (defender != 0) {
        if (defender->layers[0].life < 0) {
            defender->layers[0].life = 0;
        }
    }
    if (behind != 0) {
        if (behind->layers[0].life < 0) {
            behind->layers[0].life = 0;
        }
    }
    return damage;
}

/*
 * Base attack power of the topmost card on a stack, scanning down from the
 * last layer for the highest occupied one; ids 44, 47 and 53 are placeholder
 * equipment cards, so the base layer (index 0) is used instead when the
 * topmost occupied layer holds one of them.
 */
s32 CGPCalcPower(CardGameWork *work, CardLayerStack *stack)
{
    s32 i;
    s32 cardId;

    for (i = 39; i >= 0; i--) {
        if (stack->layers[i].cardId >= 0) {
            break;
        }
    }
    cardId = stack->layers[i].cardId;
    if (cardId == 44 || cardId == 47 || cardId == 53) {
        i = 0;
    }
    return work->definitions[stack->layers[i].cardId].attackPower;
}

/*
 * Power a card in play attacks with: its base power plus the two bonuses it
 * carries, doubled once per boost. A card whose attack type is 4 spends its
 * boosts instead of doubling.
 */
s32 CGPCalcPowerPlus(CardGameWork *work, CardPowerCard *card, s32 power)
{
    s32 boosts;

    if (card->cardId < 0) {
        power = 0;
    } else {
        power += card->powerPlus[0];
        power += card->powerPlus[1];
        if (CardChkAttackType(work->definitions, (CardLayerStack *) card) == 4) {
            card->boostCount = 0;
        }
        boosts = card->boostCount;
        if (boosts > 0) {
            do {
                boosts--;
                power *= 2;
            } while (boosts != 0);
        }
    }
    return power;
}

/*
 * Power a card gains or loses from the operation cards in play. Operation 120
 * on either side costs a card carrying flag 4 one point; every copy of 123 on
 * this side adds one to a card carrying flag 1, every copy of 134 on either
 * side one to a card carrying flag 4, and every copy of 144 one to any card.
 * Card 14 also gains a point while card 79 stands on one of this side's battle
 * slots, and card 15 one for every battle slot holding a card with flag 8.
 */
s32 CGPCalcPowerEnv(s32 side, CardGameWork *work, s32 power, s32 cardId)
{
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    s32 i;

    if (side == 0) {
        ownSide = &work->player;
        foeSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        foeSide = &work->player;
    }
    if (CardPlayCntOpe(ownSide, 120) != 0 || CardPlayCntOpe(foeSide, 120) != 0) {
        if (work->definitions[cardId].flags & 4) {
            power--;
        }
    }
    i = CardPlayCntOpe(ownSide, 123);
    if (i != 0) {
        if (work->definitions[cardId].flags & 1) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }
    i = CardPlayCntOpe(ownSide, 134) + CardPlayCntOpe(foeSide, 134);
    if (i != 0) {
        if (work->definitions[cardId].flags & 4) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }
    i = CardPlayCntOpe(ownSide, 144) + CardPlayCntOpe(foeSide, 144);
    if (i != 0) {
        while (i > 0) {
            i--;
            power++;
        }
    }
    switch (cardId) {
    case 15:
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & 8)) {
                power++;
            }
        }
        break;
    case 14:
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId == 79) {
                power++;
                break;
            }
        }
        break;
    }
    return power;
}

extern s32 CardPlayYamaCnt(CardPlaySide *side);
extern s32 CardPlaySuteCnt(CardPlaySide *side);

typedef struct CGPBoardArea {
    s16 area;
    s16 slot;
} CGPBoardArea;
extern void CGPHPCheckSub(CardDefinition *definitions, CardPlaySide *side,
                          CardLayerStack *stack, CGPBoardArea *place);

/*
 * CardDefinition.flags bit 4: the card class that several rules single out
 * (operation 130 sends it to the discard pile, operations 120/134/136 change its
 * power or life, error 15 when it is played); what the class means is not
 * established.
 */
#define CGP_CARD_FLAG_CLASS4 0x04

/*
 * While this operation card is in play a dying CGP_CARD_FLAG_CLASS4 card goes to
 * the discard (sute) pile instead of the junk pile (`addiu $5,$0,0x82`, 0x00a29648).
 */
#define CGP_OPE_DISCARD_TO_SUTE 130

/*
 * A card leaving the board slides to its pile in this many frames: the move
 * record's timer (`addiu $7,$0,0xA`, 0x00a296d0) and the divisor of its
 * per-frame step (`div.s` by 10.0f, 0x00a29728).
 */
#define CGP_PILE_MOVE_FRAMES 10
#define CGP_PILE_MOVE_Z 0.12f        /* height of the sliding card, 0x3DF5C28F (`lui`, 0x00a29688) */
#define CGP_SUTE_PILE_X 1.265f       /* discard pile position, 0x3FA1EB85 (`lui`, 0x00a296a8) */
#define CGP_SUTE_PILE_Y (-1.81f)     /* 0xBFE7AE14 (`lui`, 0x00a296c0) */

/*
 * Upkeep of one board slot: refreshes the side's deck and discard counts, counts
 * the damage timer down or copies the life for display, and keeps the power in
 * line with the base card's definition while nothing is stacked on it. A base
 * card without life left (once the damage timer ran out) sends every card of the
 * stack to the discard or junk pile; a stacked card without life goes to the junk
 * pile. Each moved card gets a move record that slides it to the pile.
 * `place` names the board area (CGP_AREA_*) and slot the stack stands on.
 */
void CGPHPCheckSub(CardDefinition *definitions, CardPlaySide *side,
                   CardLayerStack *stack, CGPBoardArea *place)
{
    CardPlayField *field;
    s32 count;
    s32 i;

    field = (CardPlayField *) side;
    side->hand.deckCount = CardPlayYamaCnt(side);
    side->hand.suteCount = CardPlaySuteCnt(side);
    stack->previousFlags = stack->flags;
    if (stack->layers[0].cardId < 0) {
        return;
    }
    if (stack->layers[0].damageTimer != 0) {
        stack->layers[0].damageTimer--;
    } else {
        stack->layers[0].shownLife = stack->layers[0].life;
    }
    if (stack->layers[1].cardId < 0) {
        /* powerLimit is compared as a signed byte (`lb`, 0x00a295f8). */
        if ((s8) stack->powerLimit != definitions[stack->layers[0].cardId].basePower) {
            stack->power = definitions[stack->layers[0].cardId].basePower;
            stack->powerLimit = definitions[stack->layers[0].cardId].basePower;
        }
    }
    if (stack->layers[0].damageTimer == 0 && stack->layers[0].life <= 0) {
        if (CardPlayCntOpe(side, CGP_OPE_DISCARD_TO_SUTE) &&
            (definitions[stack->layers[0].cardId].flags & CGP_CARD_FLAG_CLASS4)) {
            count = CardPlaySuteCnt(side);
            for (i = 0; i < 40; i++) {
                if (stack->layers[i].cardId >= 0) {
                    field->suteMove[count].moveTimer = CGP_PILE_MOVE_FRAMES;
                    field->suteMove[count].kind = 0;
                    field->suteMove[count].position[0] = stack->position[0];
                    side->sute[count] = stack->layers[i].cardId;
                    field->suteMove[count].position[1] = stack->position[1];
                    field->suteMove[count].position[2] = CGP_PILE_MOVE_Z;
                    stack->layers[i].cardId = -1;
                    field->suteMove[count].step[0] =
                        (CGP_SUTE_PILE_X - stack->position[0]) / CGP_PILE_MOVE_FRAMES;
                    field->suteMove[count].step[1] =
                        (CGP_SUTE_PILE_Y - stack->position[1]) / CGP_PILE_MOVE_FRAMES;
                    field->suteMove[count].step[2] = 0.0f;
                    count++;
                }
            }
        } else {
            count = CardPlayJunkCnt((CardPlayHand *) side);
            for (i = 0; i < 40; i++) {
                if (stack->layers[i].cardId >= 0) {
                    if (place->area == CGP_AREA_ENEMY_DISPOSAL ||
                        place->area == CGP_AREA_PLAYER_DISPOSAL) {
                        field->junkMove[count].position[0] =
                            side->disposal[place->slot].battle.position[0];
                        field->junkMove[count].position[1] =
                            side->disposal[place->slot].battle.position[1];
                        field->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
                    } else {
                        field->junkMove[count].position[0] =
                            side->battle[place->slot].battle.position[0];
                        field->junkMove[count].position[1] =
                            side->battle[place->slot].battle.position[1];
                        field->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
                    }
                    field->junkMove[count].step[0] =
                        (JunkPos1P[1][0] - field->junkMove[count].position[0]) / CGP_PILE_MOVE_FRAMES;
                    field->junkMove[count].step[1] =
                        (JunkPos1P[1][1] - field->junkMove[count].position[1]) / CGP_PILE_MOVE_FRAMES;
                    field->junkMove[count].step[2] = 0.0f;
                    field->junkMove[count].moveTimer = CGP_PILE_MOVE_FRAMES;
                    side->junk[count] = stack->layers[i].cardId;
                    stack->layers[i].cardId = -1;
                    count++;
                }
            }
        }
    }
    if (stack->layers[1].cardId >= 0 && stack->layers[1].life <= 0) {
        stack->power = definitions[stack->layers[0].cardId].basePower;
        stack->powerLimit = definitions[stack->layers[0].cardId].basePower;
        count = CardPlayJunkCnt((CardPlayHand *) side);
        if (place->area == CGP_AREA_ENEMY_DISPOSAL ||
            place->area == CGP_AREA_PLAYER_DISPOSAL) {
            field->junkMove[count].position[0] = side->disposal[place->slot].battle.position[0];
            field->junkMove[count].position[1] = side->disposal[place->slot].battle.position[1];
            field->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
        } else {
            field->junkMove[count].position[0] = side->battle[place->slot].battle.position[0];
            field->junkMove[count].position[1] = side->battle[place->slot].battle.position[1];
            field->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
        }
        field->junkMove[count].step[0] =
            (JunkPos1P[1][0] - field->junkMove[count].position[0]) / CGP_PILE_MOVE_FRAMES;
        field->junkMove[count].step[1] =
            (JunkPos1P[1][1] - field->junkMove[count].position[1]) / CGP_PILE_MOVE_FRAMES;
        field->junkMove[count].step[2] = 0.0f;
        field->junkMove[count].moveTimer = CGP_PILE_MOVE_FRAMES;
        side->junk[count] = stack->layers[1].cardId;
        stack->layers[1].cardId = -1;
    }
}

/*
 * Recheck the hit points of every card standing on either board: first the
 * four disposal slots of both sides, then their four battle slots.
 */
void CGPRefreshFieldHP(CardGameWork *work)
{
    CGPBoardArea playerPlace;
    CGPBoardArea enemyPlace;
    CardPlaySide *player;
    CardPlaySide *enemy;
    s32 i;

    player = &work->player;
    enemy = &work->enemy;
    playerPlace.area = 6;
    enemyPlace.area = 2;
    for (i = 0; i < 4; i++) {
        playerPlace.slot = i;
        enemyPlace.slot = i;
        CGPHPCheckSub(work->definitions, player,
                      &player->disposal[i].battle, &playerPlace);
        CGPHPCheckSub(work->definitions, enemy,
                      &enemy->disposal[i].battle, &enemyPlace);
    }
    playerPlace.area = 5;
    enemyPlace.area = 4;
    for (i = 0; i < 4; i++) {
        playerPlace.slot = i;
        enemyPlace.slot = i;
        CGPHPCheckSub(work->definitions, player,
                      &player->battle[i].battle, &playerPlace);
        CGPHPCheckSub(work->definitions, enemy,
                      &enemy->battle[i].battle, &enemyPlace);
    }
}

/*
 * Attack types CardChkAttackType reports for the card in play and the battle
 * command an attack runs under (CGPBattleSub attacks only when they agree).
 */
#define CGP_ATTACK_FRONT 1        /* front slots only, at the opposing card (`addiu $2,$0,1`, 0x00a2a160) */
#define CGP_ATTACK_PIERCE 2       /* also hits the card behind its target (`addiu $2,$0,2`, 0x00a2a168) */
#define CGP_ATTACK_SUPPORT 3      /* strikes the stack passed as `support` (`addiu $2,$0,3`, 0x00a29de8) */
#define CGP_ATTACK_DIRECT 4       /* no target card (`addiu $2,$0,4`, 0x00a29f00) */

#define CGP_STACK_TOP_LAYER 39    /* highest layer searched for the top card (`addiu $16,$0,0x27`, 0x00a29cb4) */

#define CGP_STACK_FLAG_FROZEN 0x02     /* no countdown and no attack (`andi $2,$2,0x2`, 0x00a29e08) */
#define CGP_STACK_FLAG_NO_DAMAGE 0x80  /* the attack deals no damage (`andi $2,$2,0x80`, 0x00a29f54) */
#define CGP_CARD_FLAG_GUARDED 0x01     /* cannot be targeted while CGP_OPE_GUARD is in play (0x00a29fc0) */
#define CGP_CARD_FLAG_ALT_SE 0x08      /* attacks with the alternate sound (`andi $3,$3,0x8`, 0x00a2a1e0) */
#define CGP_CARD_STOPS_DIRECT 39       /* battle card on either board cancels a direct attack (0x00a29f14) */
#define CGP_OPE_GUARD 124              /* `addiu $5,$0,0x7C`, 0x00a29f90 */
#define CGP_OPE_POINT_COMMAND 135      /* `addiu $5,$0,0x87`, 0x00a2a504 */
#define CGP_COMMAND_POINT_ATTACK 17    /* played for a class-4 attacker (`addiu $5,$0,0x11`, 0x00a2a580) */
#define CGP_COMMAND_NO_DAMAGE 79       /* played instead of the damage (`addiu $5,$0,0x4F`, 0x00a2a654) */
#define CGP_HAND_FLAG_NO_DAMAGE 0x01   /* CardHand.commandFlags bit 0 (`andi $2,$2,0x1`, 0x00a2a4f4) */
#define CGP_LIFE_WEAR_LIMIT 64         /* a stacked card below this life wears down (`slti $2,$2,0x40`, 0x00a29eb8) */
#define CGP_POWER_MAX 99               /* `addiu $2,$0,0x63`, 0x00a2a3d8 */

#define CGP_SE_ATTACK 0x20004          /* 0x00a2a244 */
#define CGP_SE_ATTACK_ALT 0x20009      /* 0x00a2a24c */
#define CGP_SE_FRONT_ATTACK 0x20002    /* 0x00a2a218 */
#define CGP_SE_FRONT_ATTACK_ALT 0x20005 /* 0x00a2a1ec */
#define CGP_SE_DIRECT_ATTACK 0x20007   /* 0x00a2a4b4 */
#define CGP_SE_DIRECT_VOLUME 60        /* `addiu $5,$0,0x3C`, 0x00a2a4b8 */
#define CGP_SE_DIRECT_PAN 64           /* `addiu $6,$0,0x40`, 0x00a2a4c4 */

extern int CardPlayCommandCheck(s16 command, s16 *queue);
extern void CardPlayCommandBattleExecute(s16 side, s16 command, CardGameWork *work);
extern void CardCursorActive(CardCursorControl *cursor, CGPCursorPosition *position, int mode);
extern void xglSoundEffectParamDirect(int sound, int volume, int pan);

/*
 * One battle step of the card in battle slot `slot` of `side`. A queued battle
 * command runs first; otherwise the top card of the stack decides the attack
 * type and power. When the command matches that type the slot is highlighted,
 * its countdown runs, and once it reaches zero the card attacks: the opposing
 * card (and for CGP_ATTACK_PIERCE the one behind it), the `support` stack, or the
 * other side directly. The damage left over is shown and paid from the foe's
 * hand, one card per point of damage. Returns 1 when a queued command ran, otherwise 0.
 */
s32 CGPBattleSub(s32 side, u16 command, s32 slot, CardGameWork *work,
                 CardPlaySide *ownSide, CardPlaySide *foeSide, CardLayerStack *support)
{
    CGPCursorPosition cursor;
    s32 battleDone;
    s32 destroyed;
    s16 *queue;
    s32 result = 0;
    s32 otherSide = (side == 0);
    s32 attackType;
    s32 power;
    s32 hasUpperLayer;
    s32 i;

    destroyed = 0;
    queue = &work->commandQueue[0][0][0];
    if (side != 0) {
        queue = &work->commandQueue[1][0][0];
    }
    if (CardPlayCommandCheck(work->command.phase, queue)) {
        CardPlayCommandBattleExecute(side, work->command.phase, work);
        result = 1;
        CGPRefreshFieldHP(work);
    } else if (ownSide->battle[slot].battle.layers[0].cardId >= 0) {
        for (i = CGP_STACK_TOP_LAYER; i >= 0; i--) {
            if (ownSide->battle[slot].battle.layers[i].cardId >= 0) {
                break;
            }
        }
        hasUpperLayer = (i != 0);
        if (ownSide->battle[slot].battle.layers[i].cardId == CGP_CARD_BASE_POWER_1 ||
            ownSide->battle[slot].battle.layers[i].cardId == CGP_CARD_BASE_POWER_2 ||
            ownSide->battle[slot].battle.layers[i].cardId == CGP_CARD_BASE_POWER_3) {
            i = 0;
        }
        power = work->definitions[ownSide->battle[slot].battle.layers[i].cardId].attackPower;
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[slot].battle);
        power = CGPCalcPowerPlus(work, (CardPowerCard *) &ownSide->battle[slot].battle, power);
        if (command == attackType) {
            cursor.zone = CGP_AREA_PLAYER_BATTLE;
            if (side != 0) {
                cursor.zone = CGP_AREA_ENEMY_BATTLE;
            }
            cursor.index = slot;
            CardCursorActive(&work->cursor, &cursor, 2);
            if (attackType != CGP_ATTACK_SUPPORT) {
                if ((s8) ownSide->battle[slot].battle.power > 0) {
                    if (!(ownSide->battle[slot].battle.flags & CGP_STACK_FLAG_FROZEN)) {
                        ownSide->battle[slot].battle.power = (s8)ownSide->battle[slot].battle.power - 1;
                    }
                }
            }
            if ((s8) ownSide->battle[slot].battle.power <= 0 && !(ownSide->battle[slot].battle.flags & CGP_STACK_FLAG_FROZEN)) {
                if (command != CGP_ATTACK_SUPPORT) {
                    ownSide->battle[slot].battle.power = ownSide->battle[slot].battle.powerLimit;
                }
                if (hasUpperLayer) {
                    if (work->definitions[ownSide->battle[slot].battle.layers[1].cardId].attackType != CGP_ATTACK_SUPPORT) {
                        if (ownSide->battle[slot].battle.layers[1].life < CGP_LIFE_WEAR_LIMIT) {
                            ownSide->battle[slot].battle.layers[1].life--;
                        }
                    }
                }
                power = CGPBattleBeforeEffect(side, work, power, &ownSide->battle[slot].battle, slot);
                if (attackType == CGP_ATTACK_DIRECT) {
                    if (CardPlayCntBtl(&ownSide->hand, CGP_CARD_STOPS_DIRECT) != 0 || CardPlayCntBtl(&foeSide->hand, CGP_CARD_STOPS_DIRECT) != 0) {
                        power = 0;
                    }
                }
                if (ownSide->battle[slot].battle.flags & CGP_STACK_FLAG_NO_DAMAGE) {
                    power = 0;
                }
                if (power != 0) {
                    CardLayerStack *target;
                    CardLayerStack *behind;

                    battleDone = 0;
                    target = 0;
                    behind = 0;
                    if (command == CGP_ATTACK_SUPPORT) {
                        if (support != 0 && support->layers[0].cardId >= 0) {
                            target = support;
                            if (CardPlayCntOpe(foeSide, CGP_OPE_GUARD)) {
                                if (work->definitions[target->layers[0].cardId].flags & CGP_CARD_FLAG_GUARDED) {
                                    power = 0;
                                }
                            }
                            if (power != 0) {
                                CGPBattleCalcuration(side, work, power, &battleDone, &ownSide->battle[slot].battle,
                                                     target, 0, &destroyed);
                                CardSetEffect(otherSide, work, 5, 0, CARD_EFFECT_SE_HIT, target->position, target);
                            }
                        }
                        power = 0;
                    } else {
                        if (foeSide->battle[1 - (slot & 1)].battle.layers[0].cardId >= 0) {
                            target = &foeSide->battle[1 - (slot & 1)].battle;
                            if (foeSide->battle[3 - (slot & 1)].battle.layers[0].cardId >= 0) {
                                behind = &foeSide->battle[3 - (slot & 1)].battle;
                            }
                        } else if (!(attackType & 1)) {
                            if (foeSide->battle[3 - (slot & 1)].battle.layers[0].cardId >= 0) {
                                target = &foeSide->battle[3 - (slot & 1)].battle;
                            }
                        }
                        if (CardPlayCntOpe(foeSide, CGP_OPE_GUARD) && target != 0) {
                            if (work->definitions[target->layers[0].cardId].flags & CGP_CARD_FLAG_GUARDED) {
                                if (behind != 0) {
                                    target = behind;
                                    behind = 0;
                                    if (work->definitions[target->layers[0].cardId].flags & CGP_CARD_FLAG_GUARDED) {
                                        target = 0;
                                    }
                                } else {
                                    target = 0;
                                }
                            }
                        }
                        if (slot < 2) {
                            switch (attackType) {
                            case CGP_ATTACK_FRONT:
                                if (target != 0) {
                                    CGPBattleCalcuration(side, work, power, &battleDone, &ownSide->battle[slot].battle,
                                                         target, 0, &destroyed);
                                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CGP_CARD_FLAG_ALT_SE) {
                                        CardSetEffect(side, work, 1, 0, CGP_SE_FRONT_ATTACK_ALT, ownSide->battle[slot].battle.position, target);
                                    } else {
                                        CardSetEffect(side, work, 1, 0, CGP_SE_FRONT_ATTACK, ownSide->battle[slot].battle.position, target);
                                    }
                                }
                                power = 0;
                                break;
                            case CGP_ATTACK_PIERCE:
                                if (target != 0) {
                                    s32 sound;

                                    sound = CGP_SE_ATTACK;
                                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CGP_CARD_FLAG_ALT_SE) {
                                        sound = CGP_SE_ATTACK_ALT;
                                    }
                                    power = CGPBattleCalcuration(side, work, power, &battleDone,
                                                                 &ownSide->battle[slot].battle, target, behind, &destroyed);
                                    CardSetEffect(side, work, 2, 0, sound, ownSide->battle[slot].battle.position, target);
                                }
                                break;
                            }
                        } else {
                            switch (attackType) {
                            case CGP_ATTACK_FRONT:
                                power = 0;
                                break;
                            case CGP_ATTACK_PIERCE:
                                if (target != 0) {
                                    s32 sound;

                                    sound = CGP_SE_ATTACK;
                                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CGP_CARD_FLAG_ALT_SE) {
                                        sound = CGP_SE_ATTACK_ALT;
                                    }
                                    power = CGPBattleCalcuration(side, work, power, &battleDone,
                                                                 &ownSide->battle[slot].battle, target, behind, &destroyed);
                                    CardSetEffect(side, work, 2, 0, sound, ownSide->battle[slot].battle.position, target);
                                }
                                break;
                            }
                        }
                    }
                    CGPBattleAfterEffect(side, work, battleDone, slot, &ownSide->battle[slot].battle, target, behind);
                }
                if (power > CGP_POWER_MAX) {
                    power = CGP_POWER_MAX;
                }
                if (power != 0) {
                    s32 sound;

                    sound = CGP_SE_ATTACK;
                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CGP_CARD_FLAG_ALT_SE) {
                        sound = CGP_SE_ATTACK_ALT;
                    }
                    if (attackType == CGP_ATTACK_PIERCE) {
                        if (destroyed == 0) {
                            CardSetEffect(side, work, 2, 0, sound, ownSide->battle[slot].battle.position, 0);
                        } else if (destroyed == 1) {
                            CardSetEffect(side, work, 22, 0, sound, ownSide->battle[slot].battle.position, 0);
                        } else {
                            CardSetEffect(side, work, 32, 0, sound, ownSide->battle[slot].battle.position, 0);
                        }
                    } else if (attackType == CGP_ATTACK_DIRECT) {
                        xglSoundEffectParamDirect(CGP_SE_DIRECT_ATTACK, CGP_SE_DIRECT_VOLUME, CGP_SE_DIRECT_PAN);
                        CardSetEffect(side, work, 4, 0, 0, ownSide->battle[slot].battle.position, 0);
                    }
                    if (!(foeSide->hand.commandFlags & CGP_HAND_FLAG_NO_DAMAGE)) {
                        i = CardPlayCntOpe(ownSide, CGP_OPE_POINT_COMMAND) + CardPlayCntOpe(foeSide, CGP_OPE_POINT_COMMAND);
                        if (i != 0) {
                            if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CGP_CARD_FLAG_CLASS4) {
                                if (ownSide->hand.playPoints[2] > 0) {
                                    ownSide->hand.playPoints[2]--;
                                    CardPlayCommandPlay(side, CGP_COMMAND_POINT_ATTACK, work, 0, 0);
                                }
                            }
                        }
                        if (destroyed == 0) {
                            CardSetEffect(side, work, 11, (s8) power, CARD_EFFECT_SE_HIT, 0, 0);
                        } else if (destroyed == 1) {
                            CardSetEffect(side, work, 15, (s8) power, CARD_EFFECT_SE_HIT, 0, 0);
                        } else {
                            CardSetEffect(side, work, 16, (s8) power, CARD_EFFECT_SE_HIT, 0, 0);
                        }
                        for (i = 0; i < power; i++) {
                            CardPlayCostCard((CardPlayHand *) &foeSide->hand);
                        }
                    } else {
                        CardPlayCommandPlay(side, CGP_COMMAND_NO_DAMAGE, work, 0, 0);
                    }
                }
            }
        }
    }
    return result;
}

void CGPRotStageSub(CardGameWork *work, u32 stage)
{
    if ((work->save->flags & CARD_SAVE_ROTATE_STAGE) && work->fase == 9 &&
        work->rotStageId != stage && work->rotStageCooldown == 0)
    {
        work->rotStageId = stage;
        work->rotStageCooldown = 20;
    }
}

/* Steps of CGPDrawFaseProc (command.phaseTimer). */
#define CGP_DRAW_STEP_BEGIN 0
#define CGP_DRAW_STEP_EXCHANGE_OFFER 1
#define CGP_DRAW_STEP_EXCHANGE_PROMPT 2
#define CGP_DRAW_STEP_EXCHANGE 3
#define CGP_DRAW_STEP_EFFECT_BEGIN 10
#define CGP_DRAW_STEP_EFFECT_COUNT 11
#define CGP_DRAW_STEP_EFFECT_PAY 12
#define CGP_DRAW_STEP_EFFECT_WAIT 13
#define CGP_DRAW_STEP_OPE_CURSOR 20
#define CGP_DRAW_STEP_ADD_PROMPT 21
#define CGP_DRAW_STEP_BONUS_COUNT 30
#define CGP_DRAW_STEP_DRAW 31
#define CGP_DRAW_STEP_DRAW_WAIT 32

/* Interrupt message the fase shows on entry (`addiu $7,$0,0x16`, 0x00a2a79c). */
#define CGP_DRAW_INTERRUPT_MESS 22
/* Exchanges of the opening hand a side is offered (`addiu $2,$0,0x3`, 0x00a2a78c). */
#define CGP_DRAW_EXCHANGES 3
/* Cards drawn for the opening hand (the loop counts 5 down to 0, 0x00a2aa5c..0x00a2aa70). */
#define CGP_DRAW_HAND_SIZE 6
/* Frames the draw step waits after each card (`addiu $2,$0,0x32`, 0x00a2af44). */
#define CGP_DRAW_WAIT_FRAMES 50

/*
 * Error messages (ErrorMessTbl rows) of the fase: 33..35 "Exchange Cards - Is
 * this hand okay? You have 3/2/1 more exchange(s)", 26 "Select Card - Add this
 * card to your hand?" and 36 "Use Card Effect - Executing this card's text".
 */
#define CGP_ERR_EXCHANGE_THREE 33
#define CGP_ERR_EXCHANGE_TWO 34
#define CGP_ERR_EXCHANGE_ONE 35
#define CGP_ERR_ADD_TO_HAND 26
#define CGP_ERR_USE_CARD_EFFECT 36

/* Cards and operation cards whose presence the fase reacts to (the ids the CardPlayCnt* calls and slot tests use). */
#define CGP_CARD_DRAW_EFFECT 56       /* `addiu $5,$0,0x38`, 0x00a2aaac: on the board it is highlighted and paid for */
#define CGP_CARD_DRAW_FROM_SUTE 78    /* battle card, `addiu $5,$0,0x4E`, 0x00a2ae68: draws from the discard pile */
#define CGP_OPE_DRAW_FROM_SUTE 137    /* `addiu $5,$0,0x89`, 0x00a2af04 */
#define CGP_OPE_EXTRA_DRAW 140        /* `addiu $5,$0,0x8C`, 0x00a2ae38: one more draw per copy */

/*
 * Operation card whose duration is refilled from its definition while a card with
 * CGP_CARD_FLAG_CLASS4 (tested at 0x00a2fbb8) stands on the board (`addiu $20,$0,0x84`, 0x00a2fb50;
 * definitions[132].operationDuration is `lbu 0x12A2($2)`, 0x00a2fc28);
 * CGPDrawFaseProc highlights it on the board (`addiu $23,$0,0x84`, 0x00a2aca8).
 */
#define CGP_OPE_TIMED_CARD 132

/* Operation slot rows of the cursor (CGP_AREA_* continued). */
#define CGP_AREA_ENEMY_OPERATION 3
#define CGP_AREA_PLAYER_OPERATION 7

/* Whether the card list of the side is drawn upside down: the player's hand when the stage is rotated, the other hand when it is not. */
#define CGP_HAND_FLIPPED(side, work) \
    (((side) != 0 && (work)->rotStageId == 0) || ((side) == 0 && (work)->rotStageId != 0))

extern void CardPlayShuffleYama(CardHand *hand);
extern int CardPlayDrawCard(CardHand *hand);
extern int CardPlayDrawSute(CardHand *hand);
extern s32 CardPlayCntDsp(CardPlaySide *side, s16 cardId);
extern s32 CardPlayCntAllOpe(CardGameWork *work, s16 opId);
extern void CGCPHandDispSub2(CardDefinition *definitions, CardHand *hand, int selected, int mode);
extern void CGCPHandDispSub3(CardDefinition *definitions, CardHand *hand, int selected, int mode, int option);
extern void CGCPYamaDispSub(CardHand *hand, int index, int count, int buttons, int rotated);

/* Frames left to wait after a draw (`sb` 0x00a2af48, counted down `lbu`/`sb` 0x00a2afdc / 0x00a2afec). */
extern u8 D_00A57DC4;
/* Exchanges of the opening hand still on offer (set at 0x00a2a7a4, spent at 0x00a2aa94, tested at 0x00a2a81c). */
extern u8 D_00A57DC5;
/* Hand index the exchange prompt and the draw steps highlight (`lb` 0x00a2a8b8, `sb` 0x00a2a7b0). */
extern s8 D_00A57DC6;
/* Cards still to pay for in the effect steps, then cards still to draw (`sw` 0x00a2a7a8, 0x00a2ac58, 0x00a2af6c). */
extern s32 D_00A57DC0;

/*
 * First phase of a turn for one side (called with the side, the work area and
 * both sides' card records): offers the opening hand for up to three exchanges
 * ("Is this hand okay?"; cross takes a new hand, circle keeps it), highlights
 * and pays for the board cards that act at the start of the turn, offers the
 * top card of the deck ("Add this card to your hand?"), then draws the cards
 * of the turn one per step and hands over to CGPNextTurnSub once they are all
 * drawn.
 */
void CGPDrawFaseProc(s32 side, CardGameWork *work, CardPlaySide *ownSide, CardPlaySide *foeSide)
{
    CardPlaySide *activeSide;      /* the record of `side`, the same address as ownSide */
    u16 held;
    u16 pressed;
    s32 autoPlay;
    s32 i;
    CGPCursorPosition position;
    CGPCursorPosition operationPosition;

    autoPlay = 0;
    if (side == 0) {
        activeSide = &work->player;
        held = work->command.input[0].held;
        pressed = work->command.input[0].pressed;
        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
            autoPlay = 1;
        }
    } else {
        activeSide = &work->enemy;
        held = work->command.input[1].held;
        pressed = work->command.input[1].pressed;
        {
            s32 autoFlag = work->flags & CGP_FLAG_SIDE1_AUTO;

            autoPlay = autoFlag != 0;
        }
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_DRAW_STEP_BEGIN:
        D_00A57DC0 = 0;
        D_00A57DC5 = CGP_DRAW_EXCHANGES;
        D_00A57DC6 = 0;
        work->command.interruptMessId = CGP_DRAW_INTERRUPT_MESS;
        work->command.interruptMessTimer = CGP_INTERRUPT_MESS_FRAMES;
        ownSide->hand.playPoints[0] = 1;
        ownSide->hand.playPoints[1] = 1;
        ownSide->hand.playPoints[2] = 1;
        ownSide->hand.commandFlags = 0;
        work->cursor.selectionSubMode = 0;
        work->cursor.selectionMode = 0;
        CGPRotStageSub(work, side);
        work->command.phaseTimer++;
        break;
    case CGP_DRAW_STEP_EXCHANGE_OFFER:
        if (work->command.turn != 0) {
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        } else if (side == 0 ? (work->flags & CGP_FLAG_SIDE0_AUTO) : (work->flags & CGP_FLAG_SIDE1_AUTO)) {
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        } else if (D_00A57DC5 != 0) {
            work->command.phaseTimer++;
        } else {
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        }
        break;
    case CGP_DRAW_STEP_EXCHANGE_PROMPT:
    {
        s32 count = CardPlayHandCnt(&activeSide->hand);
        s32 message = 0;

        switch (D_00A57DC5) {
        case 1:
            message = CGP_ERR_EXCHANGE_ONE;
            break;
        case 2:
            message = CGP_ERR_EXCHANGE_TWO;
            break;
        case 3:
            message = CGP_ERR_EXCHANGE_THREE;
            break;
        }
        /* With the stage rotated the pad's left and right swap their meaning. */
        if ((work->rotStageId == 0 && (pressed & CGP_PAD_RIGHT)) ||
            (work->rotStageId != 0 && (pressed & CGP_PAD_LEFT))) {
            if (D_00A57DC6 < count - 1) {
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                D_00A57DC6++;
            }
        } else if ((work->rotStageId == 0 && (pressed & CGP_PAD_LEFT)) ||
                   (work->rotStageId != 0 && (pressed & CGP_PAD_RIGHT))) {
            if (D_00A57DC6 > 0) {
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                D_00A57DC6--;
            }
        }
        if (pressed & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer++;
        } else if (pressed & CGP_PAD_CONFIRM) {
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        }
        if (held & CGP_PAD_PREVIEW) {
            CGPDispErrorMessPlus(side, message, activeSide->hand.cards[D_00A57DC6]);
        } else {
            CGPDispErrorMess(side, message);
        }
        if (CGP_HAND_FLIPPED(side, work)) {
            CGCPHandDispSub3(work->definitions, &activeSide->hand, D_00A57DC6, 0, 0);
        } else {
            CGCPHandDispSub2(work->definitions, &activeSide->hand, D_00A57DC6, 0);
        }
        break;
    }
    case CGP_DRAW_STEP_EXCHANGE:
    {
        s32 count = CardPlayHandCnt(&activeSide->hand);
        s32 deckCount = CardPlayYamaCnt(activeSide);

        /* The hand goes back under the deck, the deck is shuffled and a new hand is drawn. */
        for (i = 0; i < count; i++) {
            activeSide->deck[deckCount + i] = activeSide->hand.cards[i];
            activeSide->hand.cards[i] = -1;
        }
        CardPlayShuffleYama(&activeSide->hand);
        for (i = CGP_DRAW_HAND_SIZE - 1; i >= 0; i--) {
            CardPlayDrawCard(&activeSide->hand);
        }
        work->command.phaseTimer = CGP_DRAW_STEP_EXCHANGE_OFFER;
        D_00A57DC5--;
        break;
    }
    case CGP_DRAW_STEP_EFFECT_BEGIN:
        work->command.phaseTimer++;
        /* fall through */
    case CGP_DRAW_STEP_EFFECT_COUNT:
    {
        s32 slot;

        /* Count the effect cards on the board: 1 for a disposal slot, 2 for a battle slot, both sides. */
        D_00A57DC0 = 0;
        if (CardPlayCntDsp(ownSide, CGP_CARD_DRAW_EFFECT)) {
            D_00A57DC0 += 1;
        }
        if (CardPlayCntBtl(&ownSide->hand, CGP_CARD_DRAW_EFFECT)) {
            D_00A57DC0 += 2;
        }
        if (CardPlayCntDsp(foeSide, CGP_CARD_DRAW_EFFECT)) {
            D_00A57DC0 += 1;
        }
        if (CardPlayCntBtl(&foeSide->hand, CGP_CARD_DRAW_EFFECT)) {
            D_00A57DC0 += 2;
        }
        if (D_00A57DC0 == 0) {
            work->command.phaseTimer = CGP_DRAW_STEP_OPE_CURSOR;
            break;
        }
        if (!autoPlay) {
            CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CGP_CARD_DRAW_EFFECT);
        }
        for (slot = 0; slot < 4; slot++) {
            if (work->player.battle[slot].battle.layers[0].cardId == CGP_CARD_DRAW_EFFECT) {
                position.zone = CGP_AREA_PLAYER_BATTLE;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
            if (work->enemy.battle[slot].battle.layers[0].cardId == CGP_CARD_DRAW_EFFECT) {
                position.zone = CGP_AREA_ENEMY_BATTLE;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
        }
        for (slot = 0; slot < 4; slot++) {
            if (work->player.disposal[slot].battle.layers[0].cardId == CGP_CARD_DRAW_EFFECT) {
                position.zone = CGP_AREA_PLAYER_DISPOSAL;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
            if (work->enemy.disposal[slot].battle.layers[0].cardId == CGP_CARD_DRAW_EFFECT) {
                position.zone = CGP_AREA_ENEMY_DISPOSAL;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_DRAW_STEP_EFFECT_PAY:
        CardPlayCostCard((CardPlayHand *) &ownSide->hand);
        D_00A57DC0--;
        work->command.phaseTimer++;
        break;
    case CGP_DRAW_STEP_EFFECT_WAIT:
        if (D_00A57DC0 <= 0) {
            work->command.phaseTimer = CGP_DRAW_STEP_OPE_CURSOR;
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        break;
    case CGP_DRAW_STEP_OPE_CURSOR:
        CardCursorPassive(&work->cursor);
        D_00A57DC0 = 0;
        if (!autoPlay) {
            if (CardPlayCntOpe(ownSide, CGP_OPE_TIMED_CARD) != 0) {
                for (i = 0; i < 4; i++) {
                    if (ownSide->operations[i] == CGP_OPE_TIMED_CARD) {
                        operationPosition.zone = CGP_AREA_PLAYER_OPERATION;
                        if (side != 0) {
                            operationPosition.zone = CGP_AREA_ENEMY_OPERATION;
                        }
                        operationPosition.index = i;
                        CardCursorActive(&work->cursor, &operationPosition, 2);
                    }
                }
                work->command.phaseTimer++;
            } else {
                work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
            }
        } else {
            work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
        }
        break;
    case CGP_DRAW_STEP_ADD_PROMPT:
        CGPDispErrorMess3(side, CGP_ERR_ADD_TO_HAND);
        if (pressed & CGP_PAD_CANCEL) {
            s16 top;
            s32 deckCount;
            s32 j;

            /* Declining sends the top card of the deck to the bottom. */
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            deckCount = CardPlayYamaCnt(ownSide);
            top = ownSide->deck[deckCount - 1];
            for (j = deckCount - 1; j > 0; j--) {
                ownSide->deck[j] = ownSide->deck[j - 1];
            }
            ownSide->deck[0] = top;
            work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
        } else if (pressed & CGP_PAD_CONFIRM) {
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
        }
        {
            s32 rotated = 1;

            if (!(held & CGP_PAD_PREVIEW)) {
                if (CGP_HAND_FLIPPED(side, work)) {
                    CGCPHandDispSub3(work->definitions, &activeSide->hand, -1, 0, 0);
                } else {
                    rotated = 0;
                    CGCPHandDispSub2(work->definitions, &activeSide->hand, -1, 0);
                }
            }
            CGCPYamaDispSub(&ownSide->hand, 0, 1, held, rotated);
        }
        break;
    case CGP_DRAW_STEP_BONUS_COUNT:
        CardCursorPassive(&work->cursor);
        D_00A57DC0 = CardPlayCntAllOpe(work, CGP_OPE_EXTRA_DRAW);
        if (D_00A57DC0 != 0) {
            CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CGP_OPE_EXTRA_DRAW);
        }
        if (CardPlayCntBtl(&ownSide->hand, CGP_CARD_DRAW_FROM_SUTE)) {
            D_00A57DC0 += 1;
        }
        D_00A57DC0 += 1;
        work->command.phaseTimer++;
        break;
    case CGP_DRAW_STEP_DRAW:
        if (!autoPlay) {
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, &activeSide->hand, D_00A57DC6, 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, &activeSide->hand, D_00A57DC6, 0);
            }
        }
        if (CardPlayCntOpe(ownSide, CGP_OPE_DRAW_FROM_SUTE) || CardPlayCntBtl(&ownSide->hand, CGP_CARD_DRAW_FROM_SUTE)) {
            CardPlayDrawSute(&ownSide->hand);
            D_00A57DC4 = CGP_DRAW_WAIT_FRAMES;
        } else {
            CardPlayDrawCard(&ownSide->hand);
            D_00A57DC4 = CGP_DRAW_WAIT_FRAMES;
        }
        work->command.phaseTimer++;
        D_00A57DC0--;
        break;
    case CGP_DRAW_STEP_DRAW_WAIT:
        if (!autoPlay) {
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, &activeSide->hand, D_00A57DC6, 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, &activeSide->hand, D_00A57DC6, 0);
            }
        }
        if (D_00A57DC4 != 0) {
            D_00A57DC4--;
        } else if (D_00A57DC0 == 0) {
            CGPNextTurnSub(work);
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        break;
    }
}

extern int CardPlayDisposeCnt(CardPlaySide *side);
extern int CardPlayBattleCnt(CardPlaySide *side);

/* Card id of the last card CGPMoveSub hovered, or -1 when the cursor is on no card. */
extern s16 D_00A57DC8;
/* Board position the held card was picked up from. */
extern CGPCursorPosition D_00A57DD0;

/* CGPMoveFaseProc steps (command.phaseTimer). */
#define CGP_MOVE_STEP_PICK 0          /* put the cursor on the first occupied slot */
#define CGP_MOVE_STEP_LIST 1          /* light up the slots a card can be picked from */
#define CGP_MOVE_STEP_SELECT 2        /* confirm the card to move */
#define CGP_MOVE_STEP_TARGET 3        /* confirm the slot to move it to */
#define CGP_MOVE_STEP_DONE 4          /* back to the first step */
#define CGP_MOVE_STEP_END 20          /* nothing left to move: wait one frame */
#define CGP_MOVE_STEP_NEXT_TURN 21

/* Cursor modes CardCursorActive takes. */
#define CGP_CURSOR_SELECTABLE 4       /* `addiu $6,$0,4`, 0x00a2b43c */
#define CGP_CURSOR_BLOCKED 3          /* a class-4 card while operation 120 is in play (`addiu $23,$0,3`, 0x00a2b430; `movn`, 0x00a2b498) */
#define CGP_CURSOR_PICKED 2           /* the card that was picked up */
#define CGP_CURSOR_TARGET 1           /* a slot it can be moved to */

/* CardSaveData.flags bits that make the side's moves automatic (0x00a2b1dc, 0x00a2b1f4). */
#define CGP_SAVE_FLAG_SIDE0_AUTO 0x08
#define CGP_SAVE_FLAG_SIDE1_AUTO 0x04

#define CGP_ERR_NO_MOVABLE_CARD 6     /* `addiu $6,$0,6`, 0x00a2b20c */
#define CGP_ERR_NO_CARD_THERE 11      /* hand area chosen, `addiu $6,$0,0xB`, 0x00a2b734 */
#define CGP_ERR_MOVE_REFUSED 15       /* `addiu $6,$0,0xF`, 0x00a2b5ec */

/* Cards and operation cards that stop a move (CardPlayCntBtl / CardPlayCntOpe arguments). */
#define CGP_CARD_BLOCKS_CLASS4 10     /* `addiu $5,$0,0xA`, 0x00a2b5b4: card on either hand's board stops a class-4 card */
#define CGP_OPE_BLOCKS_CLASS4 120     /* `addiu $5,$0,0x78`, 0x00a2b480 */
#define CGP_OPE_LOCK_MOVES 138        /* `addiu $5,$0,0x8A`, 0x00a2b7dc */

/* Refuses the move with an error message and the buzzer; the caller returns. */
static inline void CGPMoveRefuse(s32 side, CardGameWork *work, s16 reason)
{
    CGPSetErrorMessPlus(side, work, CGP_ERR_MOVE_REFUSED, reason);
    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
}

void CGPMoveFaseProc(s32 side, CardGameWork *work, CardPlaySide *ownSide)
{
    u16 pressed;
    s32 disposalArea;
    s32 battleArea;
    s32 handArea;
    s32 i;
    CGPCursorPosition position;
    CGPCursorPosition activePosition;
    CGPCursorPosition target;

    if (side == 0) {
        pressed = work->command.input[0].pressed;
        disposalArea = CGP_AREA_PLAYER_DISPOSAL;
        battleArea = CGP_AREA_PLAYER_BATTLE;
        handArea = CGP_AREA_PLAYER_HAND;
    } else {
        pressed = work->command.input[1].pressed;
        disposalArea = CGP_AREA_ENEMY_DISPOSAL;
        battleArea = CGP_AREA_ENEMY_BATTLE;
        handArea = CGP_AREA_ENEMY_HAND;
    }
    CGPCursor2Pos(&position, work);
    if ((s16) work->command.phaseTimer != 0 && handArea == position.zone && work->cursorMoveAny == 0) {
        work->cursor.selectionMode = 0;
        if (CGP_HAND_FLIPPED(side, work)) {
            CGCPHandDispSub3(work->definitions, &ownSide->hand, position.index, 0, 0);
        } else {
            CGCPHandDispSub2(work->definitions, &ownSide->hand, position.index, 0);
        }
    } else {
        work->cursor.selectionMode = 1;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_MOVE_STEP_PICK:
    {
        s32 disposeCount;
        s32 battleCount;

        D_00A57DC8 = -1;
        disposeCount = CardPlayDisposeCnt(ownSide);
        battleCount = CardPlayBattleCnt(ownSide);
        if (disposeCount == 0 && battleCount == 0) {
            work->command.phaseTimer = CGP_MOVE_STEP_END;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMess(side, work, CGP_ERR_NO_MOVABLE_CARD);
            return;
        }
        if (side == 0) {
            if (disposeCount != 0) {
                for (i = 0; i < 4; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 0;
                        work->cursorZone = 5 + i;
                        break;
                    }
                }
            } else {
                for (i = 0; i < 4; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 1 + (i & 1);
                        work->cursorZone = 5 + (i & 2);
                        break;
                    }
                }
            }
        } else {
            if (disposeCount != 0) {
                for (i = 0; i < 4; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 3;
                        work->cursorZone = 4 - i;
                        break;
                    }
                }
            } else {
                for (i = 0; i < 4; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 2 - (i & 1);
                        work->cursorZone = 4 - (i & 2);
                        break;
                    }
                }
            }
        }
        CGPRotStageSub(work, side);
        work->cursor.selectionSubMode = 1;
        work->cursor.selectionMode = 1;
        work->command.phaseTimer++;
        break;
    }
    case CGP_MOVE_STEP_LIST:
    {
        CardCursorControl *cursor = &work->cursor;

        CardCursorPassive(cursor);
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                D_00A57DD0.zone = disposalArea;
                D_00A57DD0.index = i;
                CardCursorActive(cursor, &D_00A57DD0, CGP_CURSOR_SELECTABLE);
            }
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                s32 mode;

                D_00A57DD0.zone = battleArea;
                D_00A57DD0.index = i;
                mode = CGP_CURSOR_SELECTABLE;
                if (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & CGP_CARD_FLAG_CLASS4) {
                    if (CardPlayCntOpe(&work->player, CGP_OPE_BLOCKS_CLASS4) + CardPlayCntOpe(&work->enemy, CGP_OPE_BLOCKS_CLASS4)) {
                        mode = CGP_CURSOR_BLOCKED;
                    }
                }
                CardCursorActive(cursor, &D_00A57DD0, mode);
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_MOVE_STEP_SELECT:
    {
        s16 *hovered = &D_00A57DC8;

        *hovered = CGPMoveSub(side, ownSide, work);
        if (!(pressed & CGP_PAD_CONFIRM)) {
            return;
        }
        if (work->cursorMoveAny != 0) {
            return;
        }
        if (*hovered >= 0) {
            CGPCursor2Pos(&D_00A57DD0, work);
            if (disposalArea == D_00A57DD0.zone) {
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                CardCursorPassive(&work->cursor);
                CardCursorActive(&work->cursor, &D_00A57DD0, CGP_CURSOR_PICKED);
                for (i = 0; i < 4; i++) {
                    activePosition.zone = battleArea;
                    activePosition.index = i;
                    CardCursorActive(&work->cursor, &activePosition, CGP_CURSOR_TARGET);
                }
                work->command.phaseTimer++;
            } else if (battleArea == D_00A57DD0.zone) {
                if (work->definitions[*hovered].flags & CGP_CARD_FLAG_CLASS4) {
                    if (CardPlayCntBtl(&work->player.hand, CGP_CARD_BLOCKS_CLASS4) || CardPlayCntBtl(&work->enemy.hand, CGP_CARD_BLOCKS_CLASS4)) {
                        CGPMoveRefuse(side, work, CGP_CARD_BLOCKS_CLASS4);
                        return;
                    }
                    if (work->definitions[*hovered].flags & CGP_CARD_FLAG_CLASS4) {
                        if (CardPlayCntOpe(&work->player, CGP_OPE_BLOCKS_CLASS4) || CardPlayCntOpe(&work->enemy, CGP_OPE_BLOCKS_CLASS4)) {
                            CGPMoveRefuse(side, work, CGP_OPE_BLOCKS_CLASS4);
                            return;
                        }
                    }
                }
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                CardCursorPassive(&work->cursor);
                CardCursorActive(&work->cursor, &D_00A57DD0, CGP_CURSOR_PICKED);
                for (i = 0; i < 4; i++) {
                    if (battleArea != D_00A57DD0.zone || i != D_00A57DD0.index) {
                        target.zone = battleArea;
                        target.index = i;
                        CardCursorActive(&work->cursor, &target, CGP_CURSOR_TARGET);
                    }
                }
                for (i = 0; i < 4; i++) {
                    target.zone = disposalArea;
                    target.index = i;
                    CardCursorActive(&work->cursor, &target, CGP_CURSOR_TARGET);
                }
                work->command.phaseTimer++;
            }
        } else {
            CGPCursor2Pos(&target, work);
            if (handArea == target.zone) {
                CGPSetErrorMess(side, work, CGP_ERR_NO_CARD_THERE);
            }
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            CGPCursor2Pos(&D_00A57DD0, work);
        }
        break;
    }
    case CGP_MOVE_STEP_TARGET:
        CGPMoveSub(side, ownSide, work);
        if (pressed & CGP_PAD_CANCEL) {
            work->command.phaseTimer = CGP_MOVE_STEP_LIST;
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            CardCursorPassive(&work->cursor);
        }
        if (!(pressed & CGP_PAD_CONFIRM)) {
            return;
        }
        CGPCursor2Pos(&target, work);
        if (battleArea == target.zone) {
            if (disposalArea == D_00A57DD0.zone) {
                if (target.cardId >= 0) {
                    if (CardPlayCntOpe(&work->player, CGP_OPE_LOCK_MOVES) || CardPlayCntOpe(&work->enemy, CGP_OPE_LOCK_MOVES)) {
                        CGPMoveRefuse(side, work, CGP_OPE_LOCK_MOVES);
                        return;
                    }
                }
                if (work->definitions[target.cardId].flags & CGP_CARD_FLAG_CLASS4) {
                    if (CardPlayCntBtl(&work->player.hand, CGP_CARD_BLOCKS_CLASS4) || CardPlayCntBtl(&work->enemy.hand, CGP_CARD_BLOCKS_CLASS4)) {
                        CGPMoveRefuse(side, work, CGP_CARD_BLOCKS_CLASS4);
                        return;
                    }
                    if (work->definitions[target.cardId].flags & CGP_CARD_FLAG_CLASS4) {
                        if (CardPlayCntOpe(&work->player, CGP_OPE_BLOCKS_CLASS4) || CardPlayCntOpe(&work->enemy, CGP_OPE_BLOCKS_CLASS4)) {
                            CGPMoveRefuse(side, work, CGP_OPE_BLOCKS_CLASS4);
                            return;
                        }
                    }
                }
                CGPMoveDSBATTLEwork((CardMoveWork *) &ownSide->battle[target.index].battle,
                                    (CardMoveWork *) &ownSide->disposal[D_00A57DD0.index].battle);
                work->command.phaseTimer++;
            } else {
                if (D_00A57DD0.index == target.index) {
                    return;
                }
                if (work->definitions[target.cardId].flags & CGP_CARD_FLAG_CLASS4) {
                    if (CardPlayCntBtl(&work->player.hand, CGP_CARD_BLOCKS_CLASS4) || CardPlayCntBtl(&work->enemy.hand, CGP_CARD_BLOCKS_CLASS4)) {
                        CGPMoveRefuse(side, work, CGP_CARD_BLOCKS_CLASS4);
                        return;
                    }
                    if (work->definitions[target.cardId].flags & CGP_CARD_FLAG_CLASS4) {
                        if (CardPlayCntOpe(&work->player, CGP_OPE_BLOCKS_CLASS4) || CardPlayCntOpe(&work->enemy, CGP_OPE_BLOCKS_CLASS4)) {
                            CGPMoveRefuse(side, work, CGP_OPE_BLOCKS_CLASS4);
                            return;
                        }
                    }
                }
                CGPMoveDSBATTLEwork((CardMoveWork *) &ownSide->battle[target.index].battle,
                                    (CardMoveWork *) &ownSide->battle[D_00A57DD0.index].battle);
                work->command.phaseTimer++;
            }
        } else if (disposalArea == target.zone) {
            if (CardPlayCntOpe(&work->player, CGP_OPE_LOCK_MOVES) || CardPlayCntOpe(&work->enemy, CGP_OPE_LOCK_MOVES)) {
                CGPMoveRefuse(side, work, CGP_OPE_LOCK_MOVES);
                return;
            }
            if (work->definitions[target.cardId].flags & CGP_CARD_FLAG_CLASS4) {
                if (CardPlayCntBtl(&work->player.hand, CGP_CARD_BLOCKS_CLASS4) || CardPlayCntBtl(&work->enemy.hand, CGP_CARD_BLOCKS_CLASS4)) {
                    CGPMoveRefuse(side, work, CGP_CARD_BLOCKS_CLASS4);
                    return;
                }
                if (work->definitions[target.cardId].flags & CGP_CARD_FLAG_CLASS4) {
                    if (CardPlayCntOpe(&work->player, CGP_OPE_BLOCKS_CLASS4) || CardPlayCntOpe(&work->enemy, CGP_OPE_BLOCKS_CLASS4)) {
                        CGPMoveRefuse(side, work, CGP_OPE_BLOCKS_CLASS4);
                        return;
                    }
                }
            }
            if (battleArea == D_00A57DD0.zone) {
                CGPMoveDSBATTLEwork((CardMoveWork *) &ownSide->disposal[target.index].battle,
                                    (CardMoveWork *) &ownSide->battle[D_00A57DD0.index].battle);
                work->command.phaseTimer++;
            } else {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            }
        } else {
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
        }
        break;
    case CGP_MOVE_STEP_DONE:
        xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
        CardCursorPassive(&work->cursor);
        work->command.phaseTimer = CGP_MOVE_STEP_LIST;
        break;
    case CGP_MOVE_STEP_END:
        work->command.phaseTimer++;
        break;
    case CGP_MOVE_STEP_NEXT_TURN:
        CGPNextTurnSub(work);
        break;
    }
}

/*
 * Steps of CGPSetFaseProc (command.phaseTimer): the fase in which the side puts hand cards on the
 * board. The hand is sorted first (0), then the cursor picks a card (1). A card that fits is placed by
 * CGPDisposeSub; weapon cards go through the attach steps (2, 3); a card that finds no free slot needs
 * cards discarded first (10..13) or, for the paired cards, replaces the card seven ids below it (20).
 */
#define CGP_SET_STEP_SORT_HAND 0
#define CGP_SET_STEP_PICK_CARD 1
#define CGP_SET_STEP_ATTACH_LIGHT 2   /* light the cards the picked weapon card can attach to */
#define CGP_SET_STEP_ATTACH_PICK 3
#define CGP_SET_STEP_DISCARD_BEGIN 10
#define CGP_SET_STEP_DISCARD_PICK 11  /* "Cards to be discarded: N" */
#define CGP_SET_STEP_DISCARD_NEXT 12
#define CGP_SET_STEP_PUT_DOWN 13      /* "Where do you want to put it?" */
#define CGP_SET_STEP_PAIRED_SLOT 20

/* CGPDisposeSub's result for a card that finds no free slot (`addiu $2,$0,0xF`, 0x00a2bd9c). */
#define CGP_DISPOSE_NO_FREE_SLOT 15

/* Cards the steps single out (ids from the immediates around 0x00a2bd04, 0x00a2bdac and 0x00a2bf68). */
#define CGP_CARD_FLAG_RESET 14        /* its layer flag is cleared unless the side has exactly one battle card */

#define CGP_ERR_FLAG_RESET 0x25       /* `addiu $6,$0,0x25`, 0x00a2bd1c */
#define CGP_ERR_NO_WEAPON_TARGET 2    /* `addiu $6,$0,2`, 0x00a2beb4 */

/* Cursor zones the sorted hand starts on: the zone beside the hand while the hand is empty (0x00a2bc58, 0x00a2bc78). */
#define CGP_AREA_PLAYER_NO_HAND 8
#define CGP_AREA_ENEMY_NO_HAND (-1)

/* CardCursorActive mode that lights a slot as selectable (`addiu $6,$0,1`, 0x00a2bfa0). */
#define CGP_CURSOR_LIGHT 1
/* CGCPHandDispSub2/3 mode of the hand while a card is picked (`addiu $7,$0,5`, 0x00a2bf14). */
#define CGP_HAND_DISP_PICK 5

#define CGP_SET_DISCARD_TEXT_X 143    /* `addiu $4,$0,0x8F`, 0x00a2c308 */
#define CGP_SET_DISCARD_COUNT_X 363   /* `addiu $4,$0,0x16B`, 0x00a2c350 */
#define CGP_SET_PUT_DOWN_TEXT_X 160   /* `addiu $4,$0,0xA0`, 0x00a2c5fc */
#define CGP_SET_PROMPT_Y 213          /* `addiu $5,$0,0xD5` */
#define CGP_SET_TEXT_COLOR 0xFFF8     /* `ori $6,$0,0xFFF8` */
#define CGP_ZENKAKU_GLYPH_SIZE 2      /* full-width digit tables hold two bytes per glyph */

/* Prompt panel: model and texture of the card frame, placed 0.91 in front for the discard prompt (0x00a2c35c..0x00a2c3e0). */
#define CGP_SET_PROMPT_TEXTURE 0x01DB5800
#define CGP_SET_PROMPT_MODEL 0x01DAAC00
#define CGP_SET_PROMPT_SCALE_X 1.5f
#define CGP_SET_PROMPT_Z 0.91f

/* A card put on the board starts its slide at this height and takes ten frames (0x00a2c6f8, 0x00a2c818). */
#define CGP_PLACE_START_Z 0.12f
#define CGP_PLACE_FRAMES 10
#define CGP_PLACE_SLIDE_DIVISOR 10.0f

/* CardDefinition.discardRequirements has one byte per requirement. */
#define CGP_REQUIREMENT_COUNT 4

extern void CardPlayTypeSort(CardDefinition *definitions, s16 *cards, s32 count, u8 *order);
extern float BattlePos1P[][3];
/* The hand card the cursor picked (CGPCursor2Pos fills it in step 1); every later step works on it. */
extern CGPCursorPosition D_00A57DD8;
/* The card member of D_00A57DD8 under the symbol the binary labels it with; step 1 reads it by absolute address in the join after CGPDisposeSub (0x00a2bdf4). */
extern s16 D_00A57DDC;
/* Cards still to be discarded (`lw`/`sw` 0x00a2be14, 0x00a2c2fc, 0x00a2c4f8). */
extern s32 D_00A57DE0;
extern char ZenkakuListB[];
extern void xglFontPrint(int x, int y, int color, const char *text);

/* The definition and the hand-slide record of the picked hand card; CardPlayField is the same side record seen with the slide records. */
#define CGP_HELD_DEFINITION work->definitions[ownSide->hand.cards[D_00A57DD8.index]]
#define CGP_HELD_MOVE ((CardPlayField *) ownSide)->handMove[D_00A57DD8.index]

/*
 * One call per frame for the side whose turn it is (called with the side, the work area and the side's
 * card record). zoneA, zoneB and zoneC are the cursor zones of the side: the hand, the disposal row and
 * the battle row for the pick and attach steps; the discard and put-down steps (11..13) use zoneA for
 * the disposal row and zoneB for the battle row.
 */
void CGPSetFaseProc(s32 side, CardGameWork *work, CardPlaySide *ownSide)
{
    u8 order[3];
    CGPCursorPosition position;
    CGPCursorPosition target;
    CGPCursorPosition highlight;
    char digits[3];
    float matrix[4][4];
    float place[4];
    float scale[4];
    u16 pressed;
    s32 zoneA;
    s32 zoneB;
    s32 zoneC;
    s32 handCount;
    s32 result;

    pressed = work->command.input[0].pressed;
    zoneA = CGP_AREA_PLAYER_HAND;
    zoneB = CGP_AREA_PLAYER_DISPOSAL;
    zoneC = CGP_AREA_PLAYER_BATTLE;
    if (side != 0) {
        pressed = work->command.input[1].pressed;
        zoneA = CGP_AREA_ENEMY_HAND;
        zoneB = CGP_AREA_ENEMY_DISPOSAL;
        zoneC = CGP_AREA_ENEMY_BATTLE;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_SET_STEP_SORT_HAND:
        handCount = CardPlayHandCnt(&ownSide->hand);
        if (side == 0) {
            work->cursorIndex = 0;
            work->cursorZone = CGP_AREA_PLAYER_NO_HAND;
            if (handCount != 0) {
                work->cursorZone = CGP_AREA_PLAYER_HAND;
            }
        } else {
            work->cursorIndex = 0;
            work->cursorZone = CGP_AREA_ENEMY_NO_HAND;
            if (handCount != 0) {
                work->cursorZone = CGP_AREA_ENEMY_HAND;
            }
        }
        CGPRotStageSub(work, side);
        work->cursor.selectionSubMode = 1;
        work->cursor.selectionMode = 1;
        work->command.phaseTimer++;
        /* Card kinds in the order the hand shows them. */
        order[0] = 1;
        order[1] = 4;
        order[2] = 2;
        CardPlayTypeSort(work->definitions, ownSide->hand.cards, handCount, order);
        break;
    case CGP_SET_STEP_PICK_CARD:
    {
        s32 i;

        if (CardPlayBattleCnt(ownSide) != 1) {
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId == CGP_CARD_FLAG_RESET) {
                    if (ownSide->battle[i].battle.layers[0].flag != 0) {
                        CGPSetErrorMessPlus(side, work, CGP_ERR_FLAG_RESET, CGP_CARD_FLAG_RESET);
                        ownSide->battle[i].battle.layers[0].flag = 0;
                    }
                }
            }
        }
        CardCursorPassive(&work->cursor);
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&D_00A57DD8, work);
        work->cursor.selectionMode = 1;
        if (work->cursorMoveAny != 0) {
            return;
        }
        if (zoneA == D_00A57DD8.zone) {
            work->cursor.selectionMode = 0;
            result = CGPDisposeSub(side, work, D_00A57DD8.index);
            if (result != 0) {
                if (result == CGP_DISPOSE_NO_FREE_SLOT) {
                    /* Cards 7 and 9..13 pair with the card seven ids below them (CGP_PAIRED_CARD_OFFSET). */
                    switch (D_00A57DD8.cardId) {
                    case 7:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                        work->command.phaseTimer = CGP_SET_STEP_PAIRED_SLOT;
                        break;
                    default:
                        {
                            s32 j;

                            /* One card to discard for every requirement byte of the picked card that is set. */
                            D_00A57DE0 = 0;
                            for (j = CGP_REQUIREMENT_COUNT - 1; j >= 0; j--) {
                                if (work->definitions[D_00A57DDC].discardRequirements[CGP_REQUIREMENT_COUNT - 1 - j] != 0) {
                                    D_00A57DE0++;
                                }
                            }
                        }
                        work->command.phaseTimer = CGP_SET_STEP_DISCARD_BEGIN;
                        break;
                    }
                } else if (D_00A57DD8.cardId == CGP_CARD_SIDE_WEAPON) {
                    if (CardPlayCntBtl(&ownSide->hand, CGP_CARD_SIDE_WEAPON_TARGET) &&
                        (CardPlayCntBtl(&ownSide->hand, 0) || CardPlayCntBtl(&ownSide->hand, 1) ||
                         CardPlayCntBtl(&ownSide->hand, 7))) {
                        work->command.phaseTimer++;
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_NO_WEAPON_TARGET);
                        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    }
                } else {
                    work->command.phaseTimer++;
                }
            }
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, &ownSide->hand, D_00A57DD8.index, CGP_HAND_DISP_PICK, 0);
            } else {
                CGCPHandDispSub2(work->definitions, &ownSide->hand, D_00A57DD8.index, CGP_HAND_DISP_PICK);
            }
        } else if (pressed & CGP_PAD_CONFIRM) {
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
        }
        break;
    }
    case CGP_SET_STEP_ATTACH_LIGHT:
    {
        s32 i;

        CardCursorPassive(&work->cursor);
        work->cursor.selectionMode = 1;
        if (D_00A57DD8.cardId == CGP_CARD_SIDE_WEAPON) {
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId == CGP_CARD_SIDE_WEAPON_TARGET) {
                    position.zone = zoneC;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
            for (i = 0; i < 4; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId == CGP_CARD_SIDE_WEAPON_TARGET) {
                    position.zone = zoneB;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    CGPWeaponEquipChk(work, D_00A57DD8.cardId, ownSide->battle[i].battle.layers[0].cardId)) {
                    position.zone = zoneC;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
            for (i = 0; i < 4; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    CGPWeaponEquipChk(work, D_00A57DD8.cardId, ownSide->disposal[i].battle.layers[0].cardId)) {
                    position.zone = zoneB;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_SET_STEP_ATTACH_PICK:
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&position, work);
        if (pressed & CGP_PAD_CONFIRM) {
            CGPDisposeWeaponSub(side, work, D_00A57DD8, position);
            return;
        }
        if (pressed & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            work->command.phaseTimer = CGP_SET_STEP_PICK_CARD;
        }
        break;
    case CGP_SET_STEP_DISCARD_BEGIN:
        work->cursor.selectionMode = 1;
        work->command.phaseTimer++;
        break;
    case CGP_SET_STEP_DISCARD_PICK:
    {
        s32 area;
        s32 i;

        zoneA = CGP_AREA_PLAYER_DISPOSAL;
        zoneB = CGP_AREA_PLAYER_BATTLE;
        if (side != 0) {
            zoneA = CGP_AREA_ENEMY_DISPOSAL;
            zoneB = CGP_AREA_ENEMY_BATTLE;
        }
        if (ownSide->hand.cards[D_00A57DD8.index] == CGP_CARD_DRAW_FROM_SUTE) {
            CardCursorPassive(&work->cursor);
            highlight.zone = zoneA;
            for (i = 0; i < 4; i++) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                    ownSide->disposal[i].battle.layers[0].life = 0;
                }
            }
            highlight.zone = zoneB;
            for (i = 0; i < 4; i++) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
                if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                    ownSide->battle[i].battle.layers[0].life = 0;
                }
            }
            work->command.phaseTimer = CGP_SET_STEP_PUT_DOWN;
            break;
        }
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&target, work);
        CardCursorPassive(&work->cursor);
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & 1)) {
                highlight.zone = zoneB;
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                !(ownSide->disposal[i].battle.flags & 1) &&
                (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].flags & 1)) {
                highlight.zone = zoneA;
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        digits[0] = ZenkakuListB[D_00A57DE0 * CGP_ZENKAKU_GLYPH_SIZE];
        digits[1] = ZenkakuListB[D_00A57DE0 * CGP_ZENKAKU_GLYPH_SIZE + 1];
        digits[2] = 0;
        xglFontPrint(CGP_SET_DISCARD_TEXT_X, CGP_SET_PROMPT_Y, CGP_SET_TEXT_COLOR, "\v\031\003\r\0Cards to be discarded:");
        xglFontPrint(CGP_SET_DISCARD_COUNT_X, CGP_SET_PROMPT_Y, CGP_SET_TEXT_COLOR, digits);
        scale[0] = CGP_SET_PROMPT_SCALE_X;
        scale[1] = scale[2] = scale[3] = 1.0f;
        place[0] = place[1] = 0.0f;
        place[2] = CGP_SET_PROMPT_Z;
        place[3] = 1.0f;
        xglMatrixUnit(matrix);
        xglMatrixTrans(matrix, (const float (*)[4]) matrix, place);
        xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
        nmlModelSetPlace(matrix);
        nmlModelSetTexture((void *) CGP_SET_PROMPT_TEXTURE);
        nmlModelEntryCard((void *) CGP_SET_PROMPT_MODEL);
        if (pressed & CGP_PAD_CONFIRM) {
            if (target.cardId < 0 || !(work->definitions[target.cardId].flags & 1)) {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                break;
            }
            area = target.zone;
            if (zoneA == area || zoneB == area) {
                if (zoneA == area) {
                    if (ownSide->disposal[target.index].battle.flags & 1) {
                        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                        break;
                    }
                    CardPlayDesertBattle(&ownSide->hand, &ownSide->disposal[target.index].battle);
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    work->command.phaseTimer++;
                } else {
                    CardPlayDesertBattle(&ownSide->hand, &ownSide->battle[target.index].battle);
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    work->command.phaseTimer++;
                }
            } else {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            }
        } else if (pressed & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
        }
        break;
    }
    case CGP_SET_STEP_DISCARD_NEXT:
    {
        s32 area;
        s32 i;

        D_00A57DE0--;
        if (D_00A57DE0 > 0) {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            break;
        }
        zoneA = CGP_AREA_PLAYER_DISPOSAL;
        zoneB = CGP_AREA_PLAYER_BATTLE;
        if (side != 0) {
            zoneA = CGP_AREA_ENEMY_DISPOSAL;
            zoneB = CGP_AREA_ENEMY_BATTLE;
        }
        CardCursorPassive(&work->cursor);
        highlight.zone = zoneA;
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        highlight.zone = zoneB;
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId < 0) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_SET_STEP_PUT_DOWN:
    {
        s32 area;
        s32 i;

        zoneA = CGP_AREA_PLAYER_DISPOSAL;
        zoneB = CGP_AREA_PLAYER_BATTLE;
        if (side != 0) {
            zoneA = CGP_AREA_ENEMY_DISPOSAL;
            zoneB = CGP_AREA_ENEMY_BATTLE;
        }
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&target, work);
        xglFontPrint(CGP_SET_PUT_DOWN_TEXT_X, CGP_SET_PROMPT_Y, CGP_SET_TEXT_COLOR, "\v\031\003\r\0Where do you want to put it?");
        scale[0] = CGP_SET_PROMPT_SCALE_X;
        scale[1] = scale[2] = scale[3] = 1.0f;
        xglMatrixUnit(matrix);
        xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
        nmlModelSetPlace(matrix);
        nmlModelSetTexture((void *) CGP_SET_PROMPT_TEXTURE);
        nmlModelEntryCard((void *) CGP_SET_PROMPT_MODEL);
        if (pressed & CGP_PAD_CONFIRM) {
            if (target.cardId >= 0) {
                return;
            }
            area = target.zone;
            if (zoneA != area && zoneB != area) {
                return;
            }
            if (zoneB == area) {
                ownSide->battle[target.index].battle.flags = 0;
                ownSide->battle[target.index].battle.power = CGP_HELD_DEFINITION.basePower;
                ownSide->battle[target.index].battle.powerLimit = CGP_HELD_DEFINITION.basePower;
                ownSide->battle[target.index].battle.layers[0].cardId = ownSide->hand.cards[D_00A57DD8.index];
                ownSide->battle[target.index].battle.layers[0].life = CGP_HELD_DEFINITION.operationDuration;
                ownSide->battle[target.index].battle.position[0] = CGP_HELD_MOVE.position[0];
                ownSide->battle[target.index].battle.position[1] = CGP_HELD_MOVE.position[1];
                ownSide->battle[target.index].battle.position[2] = CGP_PLACE_START_Z;
                ownSide->battle[target.index].battle.step[0] = (BattlePos1P[target.index][0] - CGP_HELD_MOVE.position[0]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->battle[target.index].battle.step[1] = (BattlePos1P[target.index][1] - CGP_HELD_MOVE.position[1]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->battle[target.index].battle.step[2] = 0;
                ownSide->battle[target.index].battle.moveFrames = CGP_PLACE_FRAMES;
            } else {
                ownSide->disposal[target.index].battle.power = CGP_HELD_DEFINITION.basePower;
                ownSide->disposal[target.index].battle.powerLimit = CGP_HELD_DEFINITION.basePower;
                ownSide->disposal[target.index].battle.layers[0].cardId = ownSide->hand.cards[D_00A57DD8.index];
                ownSide->disposal[target.index].battle.layers[0].life = CGP_HELD_DEFINITION.operationDuration;
                ownSide->disposal[target.index].battle.position[0] = CGP_HELD_MOVE.position[0];
                ownSide->disposal[target.index].battle.position[1] = CGP_HELD_MOVE.position[1];
                ownSide->disposal[target.index].battle.position[2] = CGP_PLACE_START_Z;
                ownSide->disposal[target.index].battle.step[0] = (DisposePos1P[target.index][0] - CGP_HELD_MOVE.position[0]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->disposal[target.index].battle.step[1] = (DisposePos1P[target.index][1] - CGP_HELD_MOVE.position[1]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->disposal[target.index].battle.step[2] = 0;
                ownSide->disposal[target.index].battle.moveFrames = CGP_PLACE_FRAMES;
            }
            {
                s32 cost = CGP_HELD_DEFINITION.reserveCost;

                ownSide->hand.playPoints[0] -= CGP_HELD_DEFINITION.playCost;
                ownSide->hand.cards[D_00A57DD8.index] = -1;
                for (i = 0; i < cost; i++) {
                    CardPlayCostCard((CardPlayHand *) &ownSide->hand);
                }
            }
            CardPlayCleanHand((CardPlayHand *) &ownSide->hand);
            CardCursorPassive(&work->cursor);
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer = CGP_SET_STEP_PICK_CARD;
        }
        break;
    }
    case CGP_SET_STEP_PAIRED_SLOT:
    {
        CardLayerStack *stack;
        s32 slot;
        s32 i;
        s32 life;

        slot = 0;
        stack = 0;
        for (i = 0; i < 4; i++) {
            if (D_00A57DD8.cardId - CGP_PAIRED_CARD_OFFSET == ownSide->battle[i].battle.layers[0].cardId) {
                slot = i;
                stack = &ownSide->battle[i].battle;
                break;
            }
        }
        if (stack != 0) {
            life = stack->layers[0].life - work->definitions[stack->layers[0].cardId].operationDuration;
            CardPlayDesertBattle(&ownSide->hand, stack);
            stack->power = work->definitions[D_00A57DD8.cardId].basePower;
            stack->powerLimit = work->definitions[D_00A57DD8.cardId].basePower;
            stack->layers[0].cardId = D_00A57DD8.cardId;
            stack->layers[0].life = work->definitions[D_00A57DD8.cardId].operationDuration + life;
            stack->position[0] = CGP_HELD_MOVE.position[0];
            stack->position[1] = CGP_HELD_MOVE.position[1];
            stack->position[2] = CGP_PLACE_START_Z;
            stack->step[0] = (BattlePos1P[slot][0] - CGP_HELD_MOVE.position[0]) / CGP_PLACE_SLIDE_DIVISOR;
            stack->step[1] = (BattlePos1P[slot][1] - CGP_HELD_MOVE.position[1]) / CGP_PLACE_SLIDE_DIVISOR;
            stack->step[2] = 0;
            stack->moveFrames = CGP_PLACE_FRAMES;
            ownSide->hand.playPoints[0] -= CGP_HELD_DEFINITION.playCost;
            ownSide->hand.cards[D_00A57DD8.index] = -1;
            CardPlayCleanHand((CardPlayHand *) &ownSide->hand);
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
        }
        work->command.phaseTimer = CGP_SET_STEP_PICK_CARD;
        break;
    }
    }
}

extern int CardPlayDesert2Card(CardHand *hand, s32 first, s32 second);
extern void CardPlayDesertOperation(CardPlaySide *side, s32 index);
extern void CardPlayCleanYama(CardHand *hand);
extern void CCU063ExecSub(CardGameWork *work, CardPlaySide *side);
extern void CCU064ExecSub(CardGameWork *work, CardPlaySide *side);
extern void CCU065ExecSub(CardGameWork *work, CardPlaySide *side);
extern void CGCPHandDispSub(CardHand *hand, int selected, int flipped);
extern const char D_00A4F108[];
extern const char D_00A4F128[];

/*
 * Message ids the command steps hand to CGPSetErrorMess / CGPDispErrorMess /
 * CGPDispErrorMessPlus (rows of ErrorMessTbl).
 */
#define CGP_ERR_COMMAND_FAILED 13      /* "Command Error", `addiu $6,$0,0xD`, 0x00a2ce18 */
#define CGP_MESS_SELECT_CARD 24        /* "Select Card" prompt, `addiu $5,$0,0x18`, 0x00a2d100 */
#define CGP_MESS_SELECT_DISCARD 25     /* the same prompt for the card to throw away, `addiu $5,$0,0x19`, 0x00a2d354 */

/* Sound CardSetEffect starts with an operation effect (`lui $8,2` / `ori $8,$8,8`, 0x00a2d720). */
#define CGP_SE_COMMAND_EFFECT 0x20008

/* Card kind and definition flag the discard step looks for (`lbu`/`lhu`/`andi $2,$2,4`, 0x00a2d1d4..0x00a2d1e4). */
#define CGP_CARD_FLAG_COMMAND_TARGET 0x04

/* Prompt "Cards to be discarded" of command 131 (xglFontPrint 0x00a2ce38 / 0x00a2cf90) and its card model. */
#define CGP_DISCARD_PROMPT_X 0x8F
#define CGP_DISCARD_PROMPT_Y 0xD5
#define CGP_DISCARD_PROMPT_COLOR 0xFFF8
#define CGP_DISCARD_PROMPT_TEXTURE 0x01DB5800
#define CGP_DISCARD_PROMPT_MODEL 0x01DAAC00
#define CGP_DISCARD_PROMPT_SCALE_X 1.5f   /* 0x3FC00000, 0x00a2ce50 */

/* Life command 139 takes off the card it targets (`addiu $3,$3,-3`, 0x00a2d6dc). */
#define CGP_TARGET_LIFE_LOSS 3

/* Frames command 142 pays its cost cards for, and command 143 waits before and after its effect. */
#define CGP_COST_REPEAT 4              /* `addiu $2,$0,4`, 0x00a2d960 */
#define CGP_EFFECT_DELAY 10            /* `addiu $3,$0,0xA`, 0x00a2da50 */
#define CGP_EFFECT_HOLD 15             /* `addiu $3,$0,0xF`, 0x00a2da94 */

/* Cursor modes CardCursorActive takes for the selectable cards of these commands. */
#define CGP_CURSOR_PICK 1

/*
 * A battle slot as the command code walks it: the card stack, then the shunt
 * position, 0x1BC bytes in all. Command 139 indexes the stacks of a side from the first one.
 */
typedef struct CGPStackSlot {
    CardLayerStack stack;
    float shuntPosition[3];
} CGPStackSlot;

s32 CGPCFPlayCommOperation(s16 side, CardGameWork *work, s16 command)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    u16 held;
    u16 pressed;
    u16 repeat;
    s32 result;
    float firstScale[4];
    Matrix4 firstMatrix;
    float secondScale[4];
    Matrix4 secondMatrix;
    /* The original static names are noted beside each. */
    static CGPCursorPosition cursor;        /* cursor */
    static s16 card0[1];                    /* card0: first card picked to throw away */
    static s16 card1;                       /* card1: second card */
    static CGPCursorPosition handCursor;    /* work: hand slot under the cursor */
    static s32 NowNo;                       /* NowNo: deck position */
    static s32 lp;                          /* lp: cost cards still to pay */
    static s32 timer;                       /* timer: frames to wait */
    CGPCursorPosition ownPos;
    CGPCursorPosition foePos;
    CGPCursorPosition opePos;

    result = 0;
    if (side == 0) {
        own = &work->player;
        foe = &work->enemy;
        held = work->command.input[0].held;
        pressed = work->command.input[0].pressed;
        repeat = work->command.input[0].repeat;
    } else {
        own = &work->enemy;
        foe = &work->player;
        held = work->command.input[1].held;
        pressed = work->command.input[1].pressed;
        repeat = work->command.input[1].repeat;
    }
    switch (command) {
    case 63:
        switch ((s16) work->command.step) {
        case 0:
            work->command.step++;
            break;
        case 1:
            CCU063ExecSub(work, own);
            work->command.step++;
            break;
        case 2:
            result = -1;
            break;
        }
        break;
    case 64:
        switch ((s16) work->command.step) {
        case 0:
            work->command.step++;
            break;
        case 1:
            CCU064ExecSub(work, own);
            work->command.step++;
            break;
        case 2:
            result = -1;
            break;
        }
        break;
    case 65:
        switch ((s16) work->command.step) {
        case 0:
            work->command.step++;
            break;
        case 1:
            CCU065ExecSub(work, own);
            work->command.step++;
            break;
        case 2:
            result = -1;
            break;
        }
        break;
    case 131:
    {
        s16 handCount = CardPlayHandCnt(&own->hand);

        switch ((s16) work->command.step) {
        case 0:
            card0[0] = 0;
            card1 = 0;
            if (handCount < 2) {
                CGPSetErrorMess(side, work, CGP_ERR_COMMAND_FAILED);
                result = 1;
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                break;
            }
            work->command.step++;
            break;
        case 1:
            xglFontPrint(CGP_DISCARD_PROMPT_X, CGP_DISCARD_PROMPT_Y, CGP_DISCARD_PROMPT_COLOR, D_00A4F108);
            firstScale[0] = CGP_DISCARD_PROMPT_SCALE_X;
            firstScale[3] = 1.0f;
            firstScale[2] = 1.0f;
            firstScale[1] = 1.0f;
            xglMatrixUnit(firstMatrix);
            xglMatrixScale(firstMatrix, (const float (*)[4]) firstMatrix, firstScale);
            nmlModelSetPlace(firstMatrix);
            nmlModelSetTexture((void *) CGP_DISCARD_PROMPT_TEXTURE);
            nmlModelEntryCard((void *) CGP_DISCARD_PROMPT_MODEL);
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, &own->hand, card0[0], 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, &own->hand, card0[0], 0);
            }
            if (repeat & CGP_PAD_LEFT) {
                if (card0[0] > 0) {
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    card0[0]--;
                }
            } else if (repeat & CGP_PAD_RIGHT) {
                if (card0[0] < handCount - 1) {
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    card0[0]++;
                }
            }
            if (pressed & CGP_PAD_CONFIRM) {
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->command.step++;
            }
            break;
        case 2:
            xglFontPrint(CGP_DISCARD_PROMPT_X, CGP_DISCARD_PROMPT_Y, CGP_DISCARD_PROMPT_COLOR, D_00A4F128);
            secondScale[0] = CGP_DISCARD_PROMPT_SCALE_X;
            secondScale[3] = 1.0f;
            secondScale[2] = 1.0f;
            secondScale[1] = 1.0f;
            xglMatrixUnit(secondMatrix);
            xglMatrixScale(secondMatrix, (const float (*)[4]) secondMatrix, secondScale);
            nmlModelSetPlace(secondMatrix);
            nmlModelSetTexture((void *) CGP_DISCARD_PROMPT_TEXTURE);
            nmlModelEntryCard((void *) CGP_DISCARD_PROMPT_MODEL);
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, &own->hand, card1, 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, &own->hand, card1, 0);
            }
            if (repeat & CGP_PAD_LEFT) {
                if (card1 > 0) {
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    card1--;
                }
            } else if (repeat & CGP_PAD_RIGHT) {
                if (card1 < handCount - 1) {
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    card1++;
                }
            }
            if (pressed & CGP_PAD_CONFIRM) {
                if (card0[0] == card1) {
                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    break;
                }
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->command.step++;
            }
            break;
        case 3:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & CGP_PAD_CONFIRM) {
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->command.step++;
            } else if (pressed & CGP_PAD_CANCEL) {
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
                work->command.step = 1;
            }
            break;
        case 4:
            result = 1;
            CardPlayDesert2Card(&own->hand, card0[0], card1);
            CardPlayDrawSute(&own->hand);
            break;
        }
        break;
    }
    case 133:
        switch ((s16) work->command.step) {
        case 0:
        {
            s32 count = CardPlayHandCnt(&own->hand);
            s32 found = 0;
            s32 i;

            for (i = 0; i < count; i++) {
                if (work->definitions[own->hand.cards[i]].kind == CGP_CARD_KIND_COMMAND &&
                    (work->definitions[own->hand.cards[i]].flags & CGP_CARD_FLAG_COMMAND_TARGET)) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
                CGPSetErrorMess(side, work, CGP_ERR_COMMAND_FAILED);
                result = 1;
            } else {
                handCursor.index = 0;
                work->command.step++;
            }
            break;
        }
        case 1:
        {
            s32 count;
            s32 flipped;

            if (side == 0) {
                flipped = work->rotStageId != 0;
            } else {
                flipped = work->rotStageId == 0;
            }
            CGCPHandDispSub(&own->hand, handCursor.index, flipped);
            count = CardPlayHandCnt(&own->hand);
            if (repeat & CGP_PAD_RIGHT) {
                if (handCursor.index < count - 1) {
                    xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                    handCursor.index++;
                }
            } else if (repeat & CGP_PAD_LEFT) {
                if (handCursor.index > 0) {
                    xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                    handCursor.index--;
                }
            }
            if (pressed & CGP_PAD_CONFIRM) {
                if (!(work->definitions[own->hand.cards[handCursor.index]].flags & CGP_CARD_FLAG_COMMAND_TARGET)) {
                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    break;
                }
                handCursor.zone = CGP_AREA_PLAYER_HAND;
                if (side != 0) {
                    handCursor.zone = CGP_AREA_ENEMY_HAND;
                }
                work->command.step++;
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                break;
            }
            break;
        }
        case 2:
            CGPDispErrorMessPlus(side, CGP_MESS_SELECT_DISCARD, own->hand.cards[handCursor.index]);
            if (pressed & CGP_PAD_CONFIRM) {
                NowNo = 0;
                work->command.step++;
                CardPlayDesertCard(&own->hand, handCursor.index);
            }
            if (pressed & CGP_PAD_CANCEL) {
                work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            }
            break;
        case 3:
        {
            s32 yamaCount = CardPlayYamaCnt(own);

            if (pressed & CGP_PAD_CONFIRM) {
                if (work->definitions[own->deck[NowNo]].flags & CGP_CARD_FLAG_COMMAND_TARGET) {
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    work->command.step++;
                } else {
                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                }
            }
            if (pressed & CGP_PAD_CANCEL) {
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
                work->command.step = 0;
                result = 1;
            }
            if (repeat & CGP_PAD_RIGHT) {
                NowNo++;
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                if (NowNo >= yamaCount) {
                    NowNo = yamaCount - 1;
                }
            }
            if (repeat & CGP_PAD_LEFT) {
                NowNo--;
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                if (NowNo < 0) {
                    NowNo = 0;
                }
            }
            CGCPYamaDispSub(&own->hand, NowNo, yamaCount, held, 0);
            break;
        }
        case 4:
            work->command.step++;
            break;
        case 5:
        {
            s32 count = CardPlayHandCnt(&own->hand);

            own->hand.cards[count] = own->deck[NowNo];
            own->deck[NowNo] = -1;
            CardPlayCleanYama(&own->hand);
            CardPlayShuffleYama(&own->hand);
            work->command.step++;
            break;
        }
        case 6:
            work->command.step = 10;
            break;
        case 10:
            result = 1;
            break;
        }
        break;
    case 139:
        switch ((s16) work->command.step) {
        case 0:
        {
            s32 i;

            ownPos.zone = CGP_AREA_PLAYER_BATTLE;
            foePos.zone = CGP_AREA_ENEMY_BATTLE;
            if (side != 0) {
                ownPos.zone = CGP_AREA_ENEMY_BATTLE;
                foePos.zone = CGP_AREA_PLAYER_BATTLE;
            }
            CardCursorPassive(&work->cursor);
            for (i = 0; i < 4; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0) {
                    ownPos.index = i;
                    CardCursorActive(&work->cursor, &ownPos, CGP_CURSOR_PICK);
                }
                if (foe->battle[i].battle.layers[0].cardId >= 0) {
                    foePos.index = i;
                    CardCursorActive(&work->cursor, &foePos, CGP_CURSOR_PICK);
                }
            }
            work->command.step++;
            break;
        }
        case 1:
            CGPCursorMove2(side, work);
            CGPCursor2Pos(&cursor, work);
            if (pressed & CGP_PAD_CONFIRM) {
                if (cursor.cardId < 0 ||
                    !(cursor.zone == CGP_AREA_ENEMY_BATTLE || cursor.zone == CGP_AREA_PLAYER_BATTLE)) {
                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    break;
                }
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->command.step++;
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & CGP_PAD_CONFIRM) {
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->command.step++;
            } else if (pressed & CGP_PAD_CANCEL) {
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
                work->command.step = 1;
            }
            break;
        case 3:
        {
            CGPStackSlot *slots;
            s32 targetSide;

            if (cursor.zone == CGP_AREA_ENEMY_BATTLE) {
                slots = (CGPStackSlot *) &work->enemy.battle[0].battle;
                targetSide = 1;
            } else if (cursor.zone == CGP_AREA_PLAYER_BATTLE) {
                slots = (CGPStackSlot *) &work->player.battle[0].battle;
                targetSide = 0;
            } else {
                slots = (CGPStackSlot *) &work->enemy.battle[0].battle;
                targetSide = 1;
            }
            slots[cursor.index].stack.layers[0].life -= CGP_TARGET_LIFE_LOSS;
            if (slots[cursor.index].stack.layers[0].life < 0) {
                slots[cursor.index].stack.layers[0].life = 0;
            }
            CardSetEffect(targetSide, work, 5, 0, CGP_SE_COMMAND_EFFECT,
                          slots[cursor.index].stack.position,
                          &slots[cursor.index].stack);
            CardSetEffect(targetSide, work, 13, 3, 0,
                          slots[cursor.index].stack.position,
                          &slots[cursor.index].stack);
            result = -1;
            break;
        }
        }
        break;
    case 141:
    {
        s32 opeZone = CGP_AREA_ENEMY_OPERATION;

        if (side != 0) {
            opeZone = CGP_AREA_PLAYER_OPERATION;
        }

        switch ((s16) work->command.step) {
        case 0:
        {
            s32 i;

            CardCursorPassive(&work->cursor);
            opePos.zone = opeZone;
            for (i = 0; i < 4; i++) {
                if (foe->operations[i] >= 0) {
                    opePos.index = i;
                    CardCursorActive(&work->cursor, &opePos, CGP_CURSOR_PICK);
                }
            }
            work->command.step++;
            break;
        }
        case 1:
            CGPCursorMove2(side, work);
            CGPCursor2Pos(&cursor, work);
            if (pressed & CGP_PAD_CONFIRM) {
                if (cursor.cardId < 0 || opeZone != cursor.zone) {
                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    break;
                }
                work->command.step++;
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & CGP_PAD_CONFIRM) {
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->command.step++;
            } else if (pressed & CGP_PAD_CANCEL) {
                xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
                work->command.step = 1;
            }
            break;
        case 3:
            result = -1;
            CardPlayDesertOperation(foe, cursor.index);
            break;
        }
        break;
    }
    case 142:
        switch ((s16) work->command.step) {
        case 0:
            work->command.step++;
            lp = CGP_COST_REPEAT;
            CardSetEffect(side, work, 5, 0, CGP_SE_COMMAND_EFFECT, 0, 0);
            CardSetEffect(side, work, 13, 4, 0, 0, 0);
            break;
        case 1:
            CardPlayCostCard((CardPlayHand *) &foe->hand);
            work->command.step++;
            break;
        case 2:
            if (--lp != 0) {
                work->command.step--;
            } else {
                work->command.step++;
            }
            break;
        case 3:
            result = -1;
            break;
        }
        break;
    case 143:
        switch ((s16) work->command.step) {
        case 0:
            work->command.step++;
            timer = CGP_EFFECT_DELAY;
            break;
        case 1:
            if (timer != 0) {
                timer--;
                break;
            }
            CCO143ExecSub(side, work, own, foe);
            timer = CGP_EFFECT_HOLD;
            work->command.step++;
            break;
        case 2:
            if (timer != 0) {
                timer--;
                break;
            }
            result = -1;
            break;
        }
        break;
    }
    return result;
}

extern void CardDispCard(Matrix4 matrix, s16 card, int enabled);
extern void xglMatrixRotZ(Matrix4 destination, const Matrix4 source, float angle);
extern const char D_00A4F2F0[];
extern const char D_00A4F308[];
extern const char D_00A4F330[];

/*
 * Models and textures in the card game's arena: the card backdrop and its texture
 * (CardViewProc draws the same ones) and the cursor frame model that CardMainInit
 * loads from game\\Cur_WakuW.lex, which the prompt reuses as its frame.
 */
#define CGP_CARD_BACKDROP_MODEL 0x01DA5600     /* lui/ori 0x00a2dc28 */
#define CGP_CARD_BACKDROP_TEXTURE 0x01DB5800   /* lui 0x00a2dc10, ori 0x00a2dc1c */
#define CGP_ANSWER_FRAME_MODEL 0x01D12000      /* lui/ori 0x00a2dda8 */

#define CGP_ANSWER_PROMPT_X 0x7C               /* `addiu $4,$0,0x7C`, 0x00a2ddbc */
#define CGP_ANSWER_PROMPT_Y 0x82               /* `addiu $5,$0,0x82`, 0x00a2ddc0 */
#define CGP_ANSWER_HINT_X 0x18                 /* `addiu $4,$0,0x18`, 0x00a2ddf0 */
#define CGP_ANSWER_HINT_Y 0x127                /* `addiu $5,$0,0x127`, 0x00a2ddf8 */

/*
 * Answer cards are drawn in a row: each one 0.5663 wide in x and 0.02 higher in z than
 * the one before, starting at -1.4193; the right-hand panel is shifted by half a pitch.
 */
#define CGP_ANSWER_CARD_PITCH 0.5663f          /* 0x3F10F909, 0x00a2dbc8 */
#define CGP_ANSWER_CARD_START_X (-1.4193f)     /* 0xBFB5AB9F, 0x00a2de90 */
#define CGP_ANSWER_CARD_STEP_Z 0.02f           /* 0x3CA3D70A, 0x00a2dea0 */
#define CGP_ANSWER_PANEL_SHIFT 0.28315f        /* 0x3E90F909, 0x00a2de34 */
#define CGP_ANSWER_ROW_Y 0.25f                 /* 0x3E800000, 0x00a2dfc8 */
#define CGP_ANSWER_CARD_SCALE 0.35f            /* 0x3EB33333, 0x00a2de08 */

/* Placement of the prompt's backdrop and of the card being answered (0x00a2db70..0x00a2dc88). */
#define CGP_BACKDROP_Z 0.1f                    /* 0x3DCCCCCD */
#define CGP_BACKDROP_SCALE_Y 0.75f             /* 0x3F400000 */
#define CGP_MAIN_CARD_X (-1.9785f)             /* 0xBFFD3F7D */
#define CGP_MAIN_CARD_Z 0.01f                  /* 0x3C23D70A */
#define CGP_MAIN_CARD_SCALE 0.4f               /* 0x3ECCCCCD */

/* Cards in the second row face the other way: a half turn about z (0x40490FDB, 0x00a2df08). */
#define CGP_HALF_TURN 3.1415927f

/*
 * The answer cards follow the card list at +0x148 of a side's hand: 160 halfwords
 * past hand->cards (CardPlayHand.answerCards).
 */
#define CGP_ANSWER_OFFSET 160
#define CGP_ANSWER_MAX 40

/*
 * Draws the "block" prompt for the command card cardId: the card on its backdrop,
 * the prompt text ("Block this event?" while the other side has no answer card,
 * "Event blocked. Block further?" once it has one), the hint line ("If yes, select
 * card: ... No: ...") and the answer cards of both sides in two rows. The calls
 * from CGPCommFaseSub also pass a fifth halfword (D_00A57E1C) that is not read here.
 */
void CGPCFSDispSub(s32 frameSide, s32 side, CardGameWork *work, s16 cardId, s16 unusedIndex)
{
    float scale[4];
    float position[4];
    Matrix4 base;
    Matrix4 place;
    CardHand *ownHand;
    CardHand *foeHand;
    s32 found;
    s32 i;
    s32 count;
    float xShift;
    float spacing;
    s16 *cards;

    ownHand = &work->player.hand;
    foeHand = &work->enemy.hand;
    xglMatrixUnit(base);
    position[2] = CGP_BACKDROP_Z;
    scale[1] = CGP_BACKDROP_SCALE_Y;
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[3] = 1.0f;
    scale[0] = 1.0f;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixTrans(base, (const float (*)[4]) base, position);
    spacing = CGP_ANSWER_CARD_PITCH;
    xglMatrixScale(place, (const float (*)[4]) base, scale);
    if (side == 0) {
        nmlModelSetLight(Player1Color, asDir);
    } else {
        nmlModelSetLight(Player2Color, asDir);
    }
    found = 0;
    nmlModelSetTexture((void *) CGP_CARD_BACKDROP_TEXTURE);
    nmlModelSetPlace(place);
    nmlModelEntryCard((void *) CGP_CARD_BACKDROP_MODEL);
    position[0] = CGP_MAIN_CARD_X;
    position[1] = 0.0f;
    position[2] = CGP_MAIN_CARD_Z;
    position[3] = 1.0f;
    scale[1] = CGP_MAIN_CARD_SCALE;
    scale[0] = CGP_MAIN_CARD_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixTrans(place, (const float (*)[4]) base, position);
    xglMatrixScale(place, (const float (*)[4]) place, scale);
    CardDispCard(place, cardId, 1);
    if (side == 0) {
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (foeHand->cards[CGP_ANSWER_OFFSET + i] >= 0) {
                found = 1;
                break;
            }
        }
    } else {
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (ownHand->cards[CGP_ANSWER_OFFSET + i] >= 0) {
                found = 1;
                break;
            }
        }
    }
    if (found == 0) {
        if (frameSide == 0) {
            nmlModelSetLight(CLP1Color, asDir);
        } else {
            nmlModelSetLight(CLP2Color, asDir);
        }
        nmlModelSetPlace(place);
        nmlModelSetTexture(work->cursorXtx[2]);
        nmlModelEntryCard((void *) CGP_ANSWER_FRAME_MODEL);
        xglFontPrint(CGP_ANSWER_PROMPT_X, CGP_ANSWER_PROMPT_Y, CGP_TEXT_COLOR, D_00A4F2F0);
    } else {
        xglFontPrint(CGP_ANSWER_PROMPT_X, CGP_ANSWER_PROMPT_Y, CGP_TEXT_COLOR, D_00A4F308);
    }
    xglFontPrint(CGP_ANSWER_HINT_X, CGP_ANSWER_HINT_Y, CGP_TEXT_COLOR, D_00A4F330);
    scale[1] = CGP_ANSWER_CARD_SCALE;
    scale[0] = CGP_ANSWER_CARD_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xShift = 0.0f;
    if (frameSide == 0) {
        xShift = CGP_ANSWER_PANEL_SHIFT;
    }
    count = 0;
    cards = ownHand->cards;
    for (i = 0; i < CGP_ANSWER_MAX; i++) {
        if (cards[CGP_ANSWER_OFFSET + i] >= 0) {
            count++;
        }
    }
    position[1] = -CGP_ANSWER_ROW_Y;
    if (work->rotStageId != 0) {
        position[1] = -position[1];
    }
    for (i = 0; i < count; i++) {
        position[0] = spacing * i + CGP_ANSWER_CARD_START_X + xShift;
        position[2] = i * CGP_ANSWER_CARD_STEP_Z + CGP_ANSWER_CARD_STEP_Z;
        xglMatrixTrans(place, (const float (*)[4]) base, position);
        xglMatrixScale(place, (const float (*)[4]) place, scale);
        if (work->rotStageId != 0) {
            xglMatrixRotZ(place, (const float (*)[4]) place, CGP_HALF_TURN);
        }
        CardDispCard(place, ownHand->cards[CGP_ANSWER_OFFSET + i], 1);
    }
    if (count != 0 && side != 0) {
        nmlModelSetLight(CLP1Color, asDir);
        nmlModelSetPlace(place);
        nmlModelSetTexture(work->cursorXtx[2]);
        nmlModelEntryCard((void *) CGP_ANSWER_FRAME_MODEL);
    }
    count = 0;
    cards = foeHand->cards;
    for (i = 0; i < CGP_ANSWER_MAX; i++) {
        if (cards[CGP_ANSWER_OFFSET + i] >= 0) {
            count++;
        }
    }
    xShift = 0.0f;
    if (frameSide != 0) {
        xShift = CGP_ANSWER_PANEL_SHIFT;
    }
    position[1] = CGP_ANSWER_ROW_Y;
    if (work->rotStageId != 0) {
        position[1] = -position[1];
    }
    scale[1] = CGP_ANSWER_CARD_SCALE;
    scale[3] = 1.0f;
    scale[0] = CGP_ANSWER_CARD_SCALE;
    scale[2] = 1.0f;
    for (i = 0; i < count; i++) {
        position[0] = spacing * i + CGP_ANSWER_CARD_START_X + xShift;
        position[2] = i * CGP_ANSWER_CARD_STEP_Z + CGP_ANSWER_CARD_STEP_Z;
        xglMatrixTrans(place, (const float (*)[4]) base, position);
        xglMatrixScale(place, (const float (*)[4]) place, scale);
        if (work->rotStageId == 0) {
            xglMatrixRotZ(place, (const float (*)[4]) place, CGP_HALF_TURN);
        }
        CardDispCard(place, foeHand->cards[CGP_ANSWER_OFFSET + i], 1);
    }
    if (count != 0 && side == 0) {
        nmlModelSetLight(CLP2Color, asDir);
        nmlModelSetPlace(place);
        nmlModelSetTexture(work->cursorXtx[2]);
        nmlModelEntryCard((void *) CGP_ANSWER_FRAME_MODEL);
    }
}

/*
 * Records the answered card (hand->cards[cardIndex]) in the first empty slot
 * of hand->answerCards, pays its cost by calling CardPlayCostCard that many
 * times, removes it from hand->cards and tail-calls CardPlayCleanHand.
 */
void CGPAnswerSub(CGPCardDefSource *src, CardPlayHand *hand, s32 cardIndex)
{
    s32 i;
    s32 cost;

    for (i = 0; i < 40; i++) {
        if (hand->answerCards[i] < 0) {
            break;
        }
    }
    hand->answerCards[i] = hand->cards[cardIndex];
    cost = src->defs[hand->cards[cardIndex]].cost;
    for (i = 0; i < cost; i++) {
        CardPlayCostCard(hand);
    }
    hand->cards[cardIndex] = -1;
    CardPlayCleanHand(hand);
}

/*
 * Commands one side's disposal slots play once its command phase ends. A slot
 * holding one of these cards whose stack carries flag 1 plays the command that
 * card ends the phase with: card 29 command 62, card 58 command 13, card 62
 * command 15, card 61 command 60, card 57 command 64 and card 55 command 11.
 */
void CGPCommFaseEndSub(s32 side, CardPlaySide *ownSide, CardGameWork *work)
{
    s32 i;

    if (CardPlayCntDsp(ownSide, 29) != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == 29) {
                if (ownSide->disposal[i].battle.flags & 1) {
                    CardPlayCommandPlay(side, 62, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, 58) != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == 58) {
                if (ownSide->disposal[i].battle.flags & 1) {
                    CardPlayCommandPlay(side, 13, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, 62) != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == 62) {
                if (ownSide->disposal[i].battle.flags & 1) {
                    CardPlayCommandPlay(side, 15, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, 61) != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == 61) {
                if (ownSide->disposal[i].battle.flags & 1) {
                    CardPlayCommandPlay(side, 60, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, 57) != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == 57) {
                if (ownSide->disposal[i].battle.flags & 1) {
                    CardPlayCommandPlay(side, 64, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, 55) != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == 55) {
                if (ownSide->disposal[i].battle.flags & 1) {
                    CardPlayCommandPlay(side, 11, work, 0, 0);
                }
            }
        }
    }
}

/* Steps of CGPCommFaseSub (command.phaseTimer). */
#define CGP_COMM_STEP_JUNK_CHECK 0
#define CGP_COMM_STEP_SORT_HAND 1
#define CGP_COMM_STEP_PICK 2
#define CGP_COMM_STEP_CONFIRM 3
#define CGP_COMM_STEP_OPERATION 4
#define CGP_COMM_STEP_RESTART 5
#define CGP_COMM_STEP_CONDITION 10
#define CGP_COMM_STEP_DISCARD 11
#define CGP_COMM_STEP_OFFER 12
#define CGP_COMM_STEP_SHOW 13
#define CGP_COMM_STEP_PLAY 14
#define CGP_COMM_STEP_AFTER_PLAY 15
#define CGP_COMM_STEP_ADVANCE 20
#define CGP_COMM_STEP_FOE_ANSWER 21
#define CGP_COMM_STEP_OWN_ANSWER 22
#define CGP_COMM_STEP_JUNK_ANSWERS 23
#define CGP_COMM_STEP_ANSWER_NOTE 24
#define CGP_COMM_STEP_FINISH 25
#define CGP_COMM_STEP_END_PHASE 30
#define CGP_COMM_STEP_NEXT_TURN 31
#define CGP_COMM_STEP_ENEMY_REQUEST 40
#define CGP_COMM_STEP_ENEMY_OPERATION 41


/* Messages of CGPCommFaseSub (CGPSetErrorMess / CGPDispErrorMess reasons). */
#define CGP_MESS_JUNK_COUNTED 0x24     /* card 14 takes the junk count, `addiu $6,$0,0x24`, 0x00a2e600 */
#define CGP_ERR_NO_PLAYABLE_CARD 30    /* `addiu $6,$0,0x1E`, 0x00a2e90c */
#define CGP_ERR_NOT_PLAYABLE 17        /* CGPChkPlayable refused the card, `addiu $6,$0,0x11`, 0x00a2eb0c */
#define CGP_ERR_NOT_DISCARDABLE 18     /* hand card without kind bit 1, `addiu $6,$0,0x12`, 0x00a2eb60 */
#define CGP_ERR_SLOT_LOCKED 38         /* battle slot with flag 2 set, `addiu $6,$0,0x26`, 0x00a2ebb8 */
#define CGP_ERR_WRONG_AREA 19          /* `addiu $6,$0,0x13`, 0x00a2ebd0 */
#define CGP_ERR_ANSWERS_DONE 29        /* `addiu $6,$0,0x1D`, 0x00a2f9cc */
#define CGP_MESS_CONFIRM_CARD 23       /* `addiu $5,$0,0x17`, 0x00a2ec00 and 0x00a2edf8 */
#define CGP_MESS_ANSWER_OFFER 22       /* shown to the answering side, `addiu $5,$0,0x16`, 0x00a2ee98 */
#define CGP_MESS_ANSWER_NOTE 28        /* `addiu $5,$0,0x1C`, 0x00a2f89c */

#define CGP_CARD_ANSWER_A 82           /* the two answer cards (`addiu $8,$0,0x52`, 0x00a2e7f0) */
#define CGP_CARD_ANSWER_B 116          /* `addiu $7,$0,0x74`, 0x00a2e7f4 */
#define CGP_PAD_DISMISS 0x00F0         /* any face button closes the answer note (`andi $2,$17,0xF0`, 0x00a2f8a0) */

#define CGP_COMMAND_CARD_Z 0.02f       /* 0x3CA3D70A, 0x00a2efcc */
#define CGP_COMMAND_CARD_SCALE 0.355f  /* 0x3EB5C28F, 0x00a2efbc */
#define CGP_DEGREES_HALF_TURN 180.0f   /* 0x43340000, 0x00a2f118 */
#define CGP_COMMAND_BACK_MODEL 0x01D0C000 /* lui/ori 0x00a2f1ec */

extern CGPCursorPosition D_00A57E10;
extern s16 D_00A57E18;
extern s16 D_00A57E1A;
extern s16 D_00A57E1C;
extern float CLColor[4][4];
extern int CardEnemyCommandRequest(int side, CardGameWork *work);
extern int CardEnemyOperationPlay(int side, CardGameWork *work);
extern int CardEnemy1stAnswer(int side, CardGameWork *work);
extern int CardEnemyAnswer(int side, CardGameWork *work);
extern int CardEnemyCommandPlay(int side, CardGameWork *work);
extern s32 CardPlayCommandCondition(u16 side, u16 command, CardGameWork *work);

void CGPCommFaseSub(s32 side, CardGameWork *work, u16 cpuTurn, u16 cpuFoe)
{
    u8 order[3];
    Matrix4 matrix;
    float scale[4];
    float position[4];
    Matrix4 backMatrix;
    float backScale[4];
    float backPosition[4];
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    u16 pressed;
    u16 pressedFoe;
    s32 foeIndex;
    s32 operationArea;
    s32 handArea;
    s32 battleArea;

    if (side == 0) {
        foeIndex = 1;
        ownSide = &work->player;
        foeSide = &work->enemy;
        pressedFoe = work->command.input[1].pressed;
        pressed = work->command.input[0].pressed;
        operationArea = CGP_AREA_PLAYER_OPERATION;
        battleArea = CGP_AREA_PLAYER_BATTLE;
        handArea = CGP_AREA_PLAYER_HAND;
    } else {
        foeIndex = 0;
        ownSide = &work->enemy;
        foeSide = &work->player;
        pressedFoe = work->command.input[0].pressed;
        pressed = work->command.input[1].pressed;
        operationArea = CGP_AREA_ENEMY_OPERATION;
        battleArea = CGP_AREA_ENEMY_BATTLE;
        handArea = CGP_AREA_ENEMY_HAND;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_COMM_STEP_JUNK_CHECK:
    {
        s32 i;

        if ((s16) work->command.phase == 2 || (s16) work->command.phase == 9) {
            if (CardPlayCntBtl(&ownSide->hand, CGP_CARD_FLAG_RESET) != 0 && CardPlayBattleCnt(ownSide) == 1) {
                s32 junkCount = CardPlayJunkCnt((CardPlayHand *) &ownSide->hand);

                if (junkCount != 0) {
                    CGPSetErrorMessPlus(side, work, CGP_MESS_JUNK_COUNTED, CGP_CARD_FLAG_RESET);
                    for (i = 0; i < 4; i++) {
                        if (ownSide->battle[i].battle.layers[0].cardId == CGP_CARD_FLAG_RESET) {
                            ownSide->battle[i].battle.layers[0].flag = junkCount;
                            break;
                        }
                    }
                    for (i = 0; i < junkCount; i++) {
                        ownSide->junk[i] = -1;
                    }
                }
            }
        }
        if (cpuTurn != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_ENEMY_REQUEST;
        } else {
            work->command.phaseTimer++;
        }
        break;
    }
    case CGP_COMM_STEP_SORT_HAND:
    {
        s32 playable;
        s32 value;
        s32 i;

        value = CGP_AREA_PLAYER_HAND;
        if (side != 0) {
            value = CGP_AREA_ENEMY_HAND;
        }
        CGPSetCursor(work, value, 0);
        value = CardPlayHandCnt(&ownSide->hand);
        order[0] = 2;
        order[1] = 1;
        order[2] = 4;
        CardPlayTypeSort(work->definitions, ownSide->hand.cards, value, order);
        playable = 0;
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                playable++;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                playable++;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->operations[i] >= 0) {
                playable++;
                break;
            }
        }
        if (playable != 0) {
            playable = 0;
            for (i = 0; i < value; i++) {
                s32 card = ownSide->hand.cards[i];

                if (work->definitions[card].kind == CGP_CARD_KIND_DISCARD_ONLY) {
                    if (card != CGP_CARD_ANSWER_A) {
                        if (card != CGP_CARD_ANSWER_B) {
                            playable++;
                        }
                    }
                }
            }
            for (i = 0; i < 4; i++) {
                if (ownSide->operations[i] >= 0) {
                    if (CGPChkPlayable(ownSide->operations[i])) {
                        playable++;
                    }
                }
            }
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                    if (CGPChkPlayable(ownSide->battle[i].battle.layers[0].cardId)) {
                        playable++;
                    }
                }
            }
        }
        if (playable == 0) {
            work->command.phaseTimer = CGP_COMM_STEP_END_PHASE;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMess(side, work, CGP_ERR_NO_PLAYABLE_CARD);
            break;
        }
        CGPRotStageSub(work, side);
        work->command.phaseTimer++;
        work->cursor.selectionMode = 1;
        break;
    }
    case CGP_COMM_STEP_PICK:
    {
        u8 acted;

        CGPMoveSub(side, ownSide, work);
        CGPCursor2Pos(&D_00A57E10, work);
        D_00A57E18 = D_00A57E10.cardId;
        acted = 0;
        work->cursor.selectionMode = 1;
        if (work->cursorMoveAny == 0 && handArea == D_00A57E10.zone) {
            if (CGP_HAND_FLIPPED(side, work)) {
                work->cursor.selectionMode = 0;
                CGCPHandDispSub3(work->definitions, &ownSide->hand, D_00A57E10.index, 2, 0);
            } else {
                work->cursor.selectionMode = 0;
                CGCPHandDispSub2(work->definitions, &ownSide->hand, D_00A57E10.index, 2);
            }
        }
        if (pressed & CGP_PAD_CONFIRM) {
            if (work->cursorMoveAny == 0 && D_00A57E18 >= 0) {
                CGPCursor2Pos(&D_00A57E10, work);
                if ((operationArea == D_00A57E10.zone && (ownSide->operationFlags[D_00A57E10.index] & 1) == 0) ||
                    (battleArea == D_00A57E10.zone &&
                     (ownSide->battle[D_00A57E10.index].battle.flags & CGP_BATTLE_FLAG_SLOT_LOCKED) == 0)) {
                    if (CGPChkPlayable(D_00A57E18)) {
                        s32 allowed = 1;

                        if (battleArea != D_00A57E10.zone) {
                            allowed = CGPChkOpePlayable(side, work, D_00A57E10.cardId) != 0;
                        }
                        if (allowed) {
                            CardCursorActive(&work->cursor, &D_00A57E10, 2);
                            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                            work->cursor.selectionMode = 1;
                            acted = 1;
                            work->command.phaseTimer++;
                        } else {
                            CGPSetErrorMess(side, work, CGP_ERR_COMMAND_FAILED);
                            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                        }
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_NOT_PLAYABLE);
                        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    }
                } else if (handArea == D_00A57E10.zone) {
                    if (work->definitions[D_00A57E10.cardId].kind & CGP_CARD_KIND_DISCARD_ONLY) {
                        work->cursor.selectionMode = 0;
                        work->command.phaseTimer = CGP_COMM_STEP_CONDITION;
                        acted = 1;
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_NOT_DISCARDABLE);
                        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    }
                } else {
                    if (battleArea == D_00A57E10.zone && (ownSide->battle[D_00A57E10.index].battle.flags & CGP_BATTLE_FLAG_SLOT_LOCKED)) {
                        CGPSetErrorMess(side, work, CGP_ERR_SLOT_LOCKED);
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_WRONG_AREA);
                    }
                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                }
            }
            if (acted != 0) {
                return;
            }
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
        }
        break;
    }
    case CGP_COMM_STEP_CONFIRM:
        CGPDispErrorMessPlus(side, CGP_MESS_CONFIRM_CARD, D_00A57E18);
        if (pressed & CGP_PAD_CONFIRM) {
            work->command.step = 0;
            work->command.phaseTimer++;
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            return;
        }
        if (pressed & CGP_PAD_CANCEL) {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            CardCursorPassive(&work->cursor);
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
        }
        break;
    case CGP_COMM_STEP_OPERATION:
    {
        s32 result = CGPCFPlayCommOperation(side, work, D_00A57E18);

        if (result == 0) {
            return;
        }
        if (result < 0) {
            if (work->definitions[D_00A57E18].kind == CGP_CARD_KIND_OPERATION) {
                CardPlayDesertOperation(ownSide, D_00A57E10.index);
            } else {
                ownSide->battle[D_00A57E10.index].battle.layers[0].life = 0;
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_COMM_STEP_RESTART:
        CardCursorPassive(&work->cursor);
        work->command.phaseTimer = CGP_COMM_STEP_SORT_HAND;
        break;
    case CGP_COMM_STEP_ENEMY_REQUEST:
    {
        s32 index = CardEnemyCommandRequest(side, work);

        if (index < 0) {
            work->command.phaseTimer++;
            break;
        }
        ownSide->battle[0].cmdView.playedCard = ownSide->hand.cards[index];
        ownSide->hand.cards[index] = -1;
        CardPlayCleanHand((CardPlayHand *) &ownSide->hand);
        work->command.step = 0;
        work->command.phaseTimer = CGP_COMM_STEP_OFFER;
        break;
    }
    case CGP_COMM_STEP_ENEMY_OPERATION:
        if (CardEnemyOperationPlay(side, work) != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_END_PHASE;
        }
        break;
    case CGP_COMM_STEP_CONDITION:
    {
        s32 error = CardPlayChkCondition(work->definitions, ownSide, D_00A57E10.cardId, work);

        if (error == 0) {
            error = CardPlayCommandCondition(side, D_00A57E10.cardId, work);
        }
        if (error != 0) {
            CGPSetErrorMess(side, work, error);
            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            work->command.phaseTimer = CGP_COMM_STEP_SORT_HAND;
        } else {
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.step = 0;
            work->command.phaseTimer++;
            D_00A57E1A = D_00A57E1C = 0;
        }
        break;
    }
    case CGP_COMM_STEP_DISCARD:
        CGPDispErrorMess(side, CGP_MESS_CONFIRM_CARD);
        if (pressed & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            work->command.phaseTimer = CGP_COMM_STEP_PICK;
            break;
        }
        if (pressed & CGP_PAD_CONFIRM) {
            ownSide->battle[0].cmdView.playedCard = ownSide->hand.cards[D_00A57E10.index];
            ownSide->hand.cards[D_00A57E10.index] = -1;
            CardPlayCleanHand((CardPlayHand *) &ownSide->hand);
            work->command.step = 0;
            work->command.phaseTimer++;
        }
        break;
    case CGP_COMM_STEP_OFFER:
        if (cpuFoe != 0) {
            if (CardEnemy1stAnswer(foeIndex, work) != 0) {
                work->command.phaseTimer = CGP_COMM_STEP_ADVANCE;
            } else {
                work->command.phaseTimer++;
            }
            break;
        }
        CGPDispErrorMessPlus(foeIndex, CGP_MESS_ANSWER_OFFER, ownSide->battle[0].cmdView.playedCard);
        if (pressedFoe & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            work->command.phaseTimer++;
        }
        if (pressedFoe & CGP_PAD_CONFIRM) {
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer = CGP_COMM_STEP_ADVANCE;
            work->cursor.selectionMode = 0;
        }
        break;
    case CGP_COMM_STEP_SHOW:
        switch (ownSide->battle[0].cmdView.playedCard) {
        case 83: case 84: case 85: case 86: case 87: case 88:
        case 92: case 107: case 109: case 117: case 118:
            if (side == 0) {
                CGPSetCursor(work, CGP_AREA_PLAYER_BATTLE, 0);
            } else {
                CGPSetCursor(work, CGP_AREA_ENEMY_BATTLE, 0);
            }
            break;
        case 89: case 90: case 91: case 96: case 97: case 98: case 99:
        case 103: case 111: case 112: case 114: case 119:
            if (side == 0) {
                CGPSetCursor(work, CGP_AREA_ENEMY_BATTLE, 0);
            } else {
                CGPSetCursor(work, CGP_AREA_PLAYER_BATTLE, 0);
            }
            break;
        default:
            break;
        }
        work->command.phaseTimer++;
        work->cursor.selectionMode = 1;
        work->command.step = 0;
        xglMatrixUnit(matrix);
        if (side != 0) {
            xglMatrixRotZ(matrix, (const float (*)[4]) matrix, CGP_HALF_TURN);
        }
        position[0] = position[1] = 0.0f;
        position[2] = CGP_COMMAND_CARD_Z;
        position[3] = 1.0f;
        scale[0] = scale[1] = CGP_COMMAND_CARD_SCALE;
        scale[2] = scale[3] = 1.0f;
        xglMatrixTrans(matrix, (const float (*)[4]) matrix, position);
        xglMatrixScale(matrix, (const float (*)[4]) matrix, scale);
        CardDispCard(matrix, ownSide->battle[0].cmdView.playedCard, 1);
        break;
    case CGP_COMM_STEP_PLAY:
    {
        s32 played;
        s32 i;

        if (cpuTurn != 0) {
            played = CardEnemyCommandPlay(side, work);
        } else {
            played = CardPlayCommandPlay(side, work->commandIds[ownSide->battle[0].cmdView.playedCard], work, 0, 0);
        }
        if (played != 0) {
            s32 cost = work->definitions[ownSide->battle[0].cmdView.playedCard].reserveCost;

            for (i = 0; i < cost; i++) {
                CardPlayCostCard((CardPlayHand *) &ownSide->hand);
            }
            CardCursorPassive(&work->cursor);
            CGPSetPermanentMess(work, 0);
            ownSide->junk[CardPlayJunkCnt((CardPlayHand *) &ownSide->hand)] = ownSide->battle[0].cmdView.playedCard;
            ownSide->battle[0].cmdView.playedCard = -1;
            CardPlayCleanHand((CardPlayHand *) &ownSide->hand);
            CardPlayCleanJunk(ownSide);
            work->command.phaseTimer++;
            break;
        }
        {
            float halfTurn = CGP_HALF_TURN;

            xglMatrixUnit(backMatrix);
            xglMatrixRotZ(backMatrix, (const float (*)[4]) backMatrix, -work->rotStageVector[2] / CGP_DEGREES_HALF_TURN * halfTurn);
            if (side != 0) {
                xglMatrixRotZ(backMatrix, (const float (*)[4]) backMatrix, halfTurn);
            }
            backPosition[0] = backPosition[1] = 0.0f;
            backPosition[2] = CGP_COMMAND_CARD_Z;
            backPosition[3] = 1.0f;
            backScale[0] = backScale[1] = CGP_COMMAND_CARD_SCALE;
            backScale[2] = backScale[3] = 1.0f;
            xglMatrixTrans(backMatrix, (const float (*)[4]) backMatrix, backPosition);
            xglMatrixScale(backMatrix, (const float (*)[4]) backMatrix, backScale);
            CardDispCard(backMatrix, ownSide->battle[0].cmdView.playedCard, 1);
            nmlModelSetLight(CLColor, asDir);
            nmlModelSetPlace(backMatrix);
            nmlModelSetTexture(work->cursorXtx[2]);
            nmlModelEntryCard((void *) CGP_COMMAND_BACK_MODEL);
        }
        break;
    }
    case CGP_COMM_STEP_AFTER_PLAY:
        work->command.phaseTimer = CGP_COMM_STEP_PICK;
        if (cpuTurn != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_ENEMY_REQUEST;
        }
        break;
    case CGP_COMM_STEP_ADVANCE:
        work->command.phaseTimer++;
        break;
    case CGP_COMM_STEP_FOE_ANSWER:
    {
        s32 value;
        s32 i;

        if (cpuFoe != 0) {
            if (CardEnemyAnswer(foeIndex, work) != 0) {
                work->command.phaseTimer++;
            } else {
                work->command.phaseTimer = CGP_COMM_STEP_JUNK_ANSWERS;
            }
            break;
        }
        value = CardPlayHandCnt(&foeSide->hand);
        if (work->rotStageId == 0) {
            if (pressedFoe & CGP_PAD_RIGHT) {
                if (D_00A57E1C < value - 1) {
                    D_00A57E1C++;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            } else if (pressedFoe & CGP_PAD_LEFT) {
                if (D_00A57E1C > 0) {
                    D_00A57E1C--;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            }
        } else {
            if (pressedFoe & CGP_PAD_LEFT) {
                if (D_00A57E1C < value - 1) {
                    D_00A57E1C++;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            } else if (pressedFoe & CGP_PAD_RIGHT) {
                if (D_00A57E1C > 0) {
                    D_00A57E1C--;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            }
        }
        CGPCFSDispSub(side, foeIndex, work, ownSide->battle[0].cmdView.playedCard, D_00A57E1C);
        if (CGP_HAND_FLIPPED(foeIndex, work)) {
            CGCPHandDispSub3(work->definitions, &foeSide->hand, D_00A57E1C, 2, 1);
        } else {
            CGCPHandDispSub2(work->definitions, &foeSide->hand, D_00A57E1C, 2);
        }
        if (pressedFoe & CGP_PAD_CONFIRM) {
            s32 card = foeSide->hand.cards[D_00A57E1C];

            if (card != CGP_CARD_ANSWER_A && card != CGP_CARD_ANSWER_B) {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                return;
            }
            value = CardPlayChkCondition(work->definitions, foeSide, foeSide->hand.cards[D_00A57E1C], work);
            if (value != 0) {
                CGPSetErrorMess(foeIndex, work, value);
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                return;
            }
            value = work->definitions[foeSide->hand.cards[D_00A57E1C]].reserveCost;
            for (i = 0; i < value; i++) {
                CardPlayCostCard((CardPlayHand *) &foeSide->hand);
            }
            for (i = 0; i < CGP_ANSWER_MAX; i++) {
                if (foeSide->hand.cards[CGP_ANSWER_OFFSET + i] < 0) {
                    break;
                }
            }
            foeSide->hand.cards[CGP_ANSWER_OFFSET + i] = foeSide->hand.cards[D_00A57E1C];
            foeSide->hand.cards[D_00A57E1C] = -1;
            CardPlayCleanHand((CardPlayHand *) &foeSide->hand);
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer++;
            break;
        }
        if (pressedFoe & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            work->command.phaseTimer = CGP_COMM_STEP_JUNK_ANSWERS;
        }
        break;
    }
    case CGP_COMM_STEP_OWN_ANSWER:
    {
        s32 value;
        s32 i;

        if (cpuTurn != 0) {
            if (CardEnemyAnswer(side, work) != 0) {
                work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            } else {
                work->command.phaseTimer = CGP_COMM_STEP_FINISH;
            }
            break;
        }
        value = CardPlayHandCnt(&ownSide->hand);
        if (work->rotStageId == 0) {
            if (pressed & CGP_PAD_RIGHT) {
                if (D_00A57E1A < value - 1) {
                    D_00A57E1A++;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            } else if (pressed & CGP_PAD_LEFT) {
                if (D_00A57E1A > 0) {
                    D_00A57E1A--;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            }
        } else {
            if (pressed & CGP_PAD_LEFT) {
                if (D_00A57E1A < value - 1) {
                    D_00A57E1A++;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            } else if (pressed & CGP_PAD_RIGHT) {
                if (D_00A57E1A > 0) {
                    D_00A57E1A--;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
            }
        }
        CGPCFSDispSub(side, side, work, ownSide->battle[0].cmdView.playedCard, D_00A57E1A);
        if (CGP_HAND_FLIPPED(side, work)) {
            CGCPHandDispSub3(work->definitions, &ownSide->hand, D_00A57E1A, 2, 1);
        } else {
            CGCPHandDispSub2(work->definitions, &ownSide->hand, D_00A57E1A, 2);
        }
        if (pressed & CGP_PAD_CONFIRM) {
            s32 card = ownSide->hand.cards[D_00A57E1A];

            if (card != CGP_CARD_ANSWER_A && card != CGP_CARD_ANSWER_B) {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                return;
            }
            value = CardPlayChkCondition(work->definitions, ownSide, ownSide->hand.cards[D_00A57E1A], work);
            if (value != 0) {
                CGPSetErrorMess(side, work, value);
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                return;
            }
            value = work->definitions[ownSide->hand.cards[D_00A57E1A]].reserveCost;
            for (i = 0; i < value; i++) {
                CardPlayCostCard((CardPlayHand *) &ownSide->hand);
            }
            for (i = 0; i < CGP_ANSWER_MAX; i++) {
                if (ownSide->hand.cards[CGP_ANSWER_OFFSET + i] < 0) {
                    break;
                }
            }
            ownSide->hand.cards[CGP_ANSWER_OFFSET + i] = ownSide->hand.cards[D_00A57E1A];
            ownSide->hand.cards[D_00A57E1A] = -1;
            CardPlayCleanHand((CardPlayHand *) &ownSide->hand);
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            break;
        }
        if (pressed & CGP_PAD_CANCEL) {
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
            work->command.phaseTimer = CGP_COMM_STEP_FINISH;
        }
        break;
    }
    case CGP_COMM_STEP_JUNK_ANSWERS:
    {
        s32 i;
        s32 junk = CardPlayJunkCnt((CardPlayHand *) &ownSide->hand);

        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (ownSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                ownSide->junk[junk++] = ownSide->hand.cards[CGP_ANSWER_OFFSET + i];
                ownSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        junk = CardPlayJunkCnt((CardPlayHand *) &foeSide->hand);
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (foeSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                foeSide->junk[junk++] = foeSide->hand.cards[CGP_ANSWER_OFFSET + i];
                foeSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_COMM_STEP_ANSWER_NOTE:
        if (cpuTurn != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_SHOW;
            work->cursor.selectionMode = 1;
            break;
        }
        CGPDispErrorMessPlus(side, CGP_MESS_ANSWER_NOTE, ownSide->battle[0].cmdView.playedCard);
        if (pressed & CGP_PAD_DISMISS) {
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer = CGP_COMM_STEP_SHOW;
        }
        break;
    case CGP_COMM_STEP_FINISH:
    {
        s32 i;
        s32 junk = CardPlayJunkCnt((CardPlayHand *) &ownSide->hand);

        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (ownSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                ownSide->junk[junk++] = ownSide->hand.cards[CGP_ANSWER_OFFSET + i];
                ownSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        {
            s32 cost = work->definitions[ownSide->battle[0].cmdView.playedCard].reserveCost;

            for (i = 0; i < cost; i++) {
                CardPlayCostCard((CardPlayHand *) &ownSide->hand);
            }
        }
        ownSide->junk[junk] = ownSide->battle[0].cmdView.playedCard;
        ownSide->battle[0].cmdView.playedCard = -1;
        junk = CardPlayJunkCnt((CardPlayHand *) &foeSide->hand);
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (foeSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                foeSide->junk[junk++] = foeSide->hand.cards[CGP_ANSWER_OFFSET + i];
                foeSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        work->command.phaseTimer = CGP_COMM_STEP_ENEMY_REQUEST;
        if (cpuTurn == 0) {
            CGPSetErrorMess(side, work, CGP_ERR_ANSWERS_DONE);
            work->command.phaseTimer = CGP_COMM_STEP_PICK;
        }
        work->cursor.selectionMode = 1;
        break;
    }
    case CGP_COMM_STEP_END_PHASE:
        switch ((s16) work->command.phase) {
        case 4:
            CGPCommFaseEndSub(0, &work->player, work);
            break;
        case 11:
            CGPCommFaseEndSub(1, &work->enemy, work);
            break;
        }
        work->command.phaseTimer++;
        break;
    case CGP_COMM_STEP_NEXT_TURN:
        CGPNextTurnSub(work);
        break;
    }
}

/*
 * CardLayerStack.flags bit CGPBattleAfterEffect sets on an attacking stack; the
 * end phase clears it on every slot of the side (`andi $2,$2,0xFD`, 0x00a2fadc).
 */
#define CGP_STACK_FLAG_MARKED 0x02

/* Operation cards CardPlayCntOpe counts; ids as in the other CardPlayCntOpe tests of this TU. */
#define CGP_OPE_RECOVERY 128          /* CardPlayRecavery runs once per copy (`addiu $5,$0,0x80`, 0x00a2fda0) */
#define CGP_OPE_REGEN_CLASS4 136      /* either side: +1 life on each disposal card of class 4 (0x00a2fdd0, 0x00a2fddc) */
#define CGP_OPE_REGEN_OTHER 127       /* own side: +1 life on each disposal card not of class 4 (0x00a2ffdc) */

/*
 * An operation card with a duration below this counts down once per end phase;
 * higher durations never expire (`slti $2,$2,0x40`, 0x00a2fb70).
 */
#define CGP_OPE_PERMANENT_DURATION 64

/* Cards that regain life at the end phase (`addiu $2,$0,6`, 0x00a2fed4; `0xD`, 0x00a2fedc; `0x4D`, 0x00a300d4). */
#define CGP_CARD_REGEN_SMALL 6           /* +2 life, clamped at its operationDuration (0xEA is definitions[6]+0x12) */
#define CGP_CARD_REGEN_LARGE 13          /* +3 life (0x1E6 is definitions[13]+0x12) */
#define CGP_CARD_REGEN_LARGE_BATTLE 77   /* +3 life on a battle slot only */

/* The foe's battle card that makes this side play command 77 on the same slot (`addiu $2,$0,0x36`, 0x00a30204;
 * `addiu $5,$0,0x4D`, 0x00a3021c). */
#define CGP_CARD_FOE_TRIGGER 54
#define CGP_COMMAND_FOE_TRIGGERED 77

/* CardSetEffect codes and sound of the regeneration effect (0x00a2fe5c, 0x00a2fe8c, `lui $8,2` 0x00a2fe84). */
#define CGP_EFFECT_REGEN_START 10
#define CGP_EFFECT_REGEN 12
#define CGP_SE_REGEN 0x2000D

/*
 * End of a side's turn: clears the per-turn marks of the own battle and disposal
 * slots, counts down the command timers, expires the operation cards of both
 * sides (duration exhausted: the card moves to the junk pile), applies the
 * recovery and regeneration operation cards and the regenerating cards on the
 * own slots, and answers a foe battle card 54 with command 77.
 */
void CGPEndFaseSub(s32 side, CardPlaySide *ownSide, CardPlaySide *foeSide,
                   CardGameWork *work)
{
    s32 otherSide;
    s32 found;
    s32 count;
    s32 i;
    s32 j;
    s32 foeCard;
    s32 disposalCard;
    s32 battleCard;

    otherSide = (side == 0);
    for (i = 0; i < 4; i++) {
        ownSide->battle[i].battle.flags &= ~CGP_STACK_FLAG_MARKED;
        ownSide->battle[i].battle.layers[0].state = 0;
        ownSide->battle[i].battle.layers[0].flag = 0;
        ownSide->battle[i].battle.layers[0].boostCount = 0;
    }
    for (i = 0; i < 4; i++) {
        ownSide->disposal[i].battle.flags &= ~CGP_STACK_FLAG_MARKED;
        ownSide->disposal[i].battle.layers[0].state = 0;
        ownSide->disposal[i].battle.layers[0].flag = 0;
        ownSide->disposal[i].battle.layers[0].boostCount = 0;
    }
    if (work->command.turnCountdown != 0) {
        work->command.turnCountdown--;
    }
    if (work->command.recoveryApplied != 0) {
        work->command.recoveryApplied--;
    }
    for (i = 0; i < 4; i++) {
        if (ownSide->operations[i] >= 0) {
            if (ownSide->operationDuration[i] < CGP_OPE_PERMANENT_DURATION) {
                if (ownSide->operations[i] == CGP_OPE_TIMED_CARD) {
                    found = 0;
                    for (j = 0; j < 4; j++) {
                        if (ownSide->disposal[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[ownSide->disposal[j].battle.layers[0].cardId].flags &
                             CGP_CARD_FLAG_CLASS4)) {
                            found = 1;
                            break;
                        }
                    }
                    for (j = 0; j < 4; j++) {
                        if (ownSide->battle[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[ownSide->battle[j].battle.layers[0].cardId].flags &
                             CGP_CARD_FLAG_CLASS4)) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        ownSide->operationDuration[i] = work->definitions[CGP_OPE_TIMED_CARD].operationDuration;
                    } else {
                        ownSide->operationDuration[i]--;
                    }
                } else {
                    ownSide->operationDuration[i]--;
                }
            }
            if (ownSide->operationDuration[i] <= 0) {
                ownSide->junk[CardPlayJunkCnt((CardPlayHand *) ownSide)] = ownSide->operations[i];
                ownSide->operations[i] = -1;
            }
        }
        if (foeSide->operations[i] >= 0) {
            if (foeSide->operationDuration[i] < CGP_OPE_PERMANENT_DURATION) {
                /* Tests the own side's operation card, not the foe's (0x00a2fc84). */
                if (ownSide->operations[i] == CGP_OPE_TIMED_CARD) {
                    found = 0;
                    for (j = 0; j < 4; j++) {
                        if (foeSide->disposal[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[foeSide->disposal[j].battle.layers[0].cardId].flags &
                             CGP_CARD_FLAG_CLASS4)) {
                            found = 1;
                            break;
                        }
                    }
                    for (j = 0; j < 4; j++) {
                        if (foeSide->battle[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[foeSide->battle[j].battle.layers[0].cardId].flags &
                             CGP_CARD_FLAG_CLASS4)) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        foeSide->operationDuration[i] = work->definitions[CGP_OPE_TIMED_CARD].operationDuration;
                    } else {
                        foeSide->operationDuration[i]--;
                    }
                } else {
                    foeSide->operationDuration[i]--;
                }
            }
            if (foeSide->operationDuration[i] <= 0) {
                foeSide->junk[CardPlayJunkCnt((CardPlayHand *) foeSide)] = foeSide->operations[i];
                foeSide->operations[i] = -1;
            }
        }
    }
    i = CardPlayCntOpe(ownSide, CGP_OPE_RECOVERY);
    if (i != 0) {
        while (i > 0) {
            CardPlayRecavery(&ownSide->hand);
            i--;
        }
    }
    count = CardPlayCntOpe(ownSide, CGP_OPE_REGEN_CLASS4) +
            CardPlayCntOpe(foeSide, CGP_OPE_REGEN_CLASS4);
    if (count != 0) {
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].flags &
                 CGP_CARD_FLAG_CLASS4)) {
                ownSide->disposal[i].battle.layers[0].life++;
                if (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].operationDuration <
                    ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life =
                        work->definitions[ownSide->disposal[i].battle.layers[0].cardId].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_REGEN_START, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_REGEN, 1, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
            }
        }
    }
    for (i = 0; i < 4; i++) {
        disposalCard = ownSide->disposal[i].battle.layers[0].cardId;
        if (disposalCard >= 0) {
            switch ((s16) disposalCard) {
            case CGP_CARD_REGEN_SMALL:
                ownSide->disposal[i].battle.layers[0].life += 2;
                if (work->definitions[CGP_CARD_REGEN_SMALL].operationDuration < ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life = work->definitions[CGP_CARD_REGEN_SMALL].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_REGEN_START, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_REGEN, 2, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
                break;
            case CGP_CARD_REGEN_LARGE:
                ownSide->disposal[i].battle.layers[0].life += 3;
                if (work->definitions[CGP_CARD_REGEN_LARGE].operationDuration < ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life = work->definitions[CGP_CARD_REGEN_LARGE].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_REGEN_START, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_REGEN, 3, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
                break;
            }
            if (CardPlayCntOpe(ownSide, CGP_OPE_REGEN_OTHER) &&
                !(work->definitions[disposalCard].flags & CGP_CARD_FLAG_CLASS4)) {
                ownSide->disposal[i].battle.layers[0].life++;
                if (work->definitions[disposalCard].operationDuration <
                    ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life =
                        work->definitions[disposalCard].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_REGEN_START, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_REGEN, 1, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
            }
        }
    }
    for (i = 0; i < 4; i++) {
        battleCard = ownSide->battle[i].battle.layers[0].cardId;
        if (battleCard >= 0) {
            switch ((s16) battleCard) {
            case CGP_CARD_REGEN_SMALL:
                ownSide->battle[i].battle.layers[0].life += 2;
                if (work->definitions[CGP_CARD_REGEN_SMALL].operationDuration < ownSide->battle[i].battle.layers[0].life) {
                    ownSide->battle[i].battle.layers[0].life = work->definitions[CGP_CARD_REGEN_SMALL].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_REGEN_START, 0, 0, ownSide->battle[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_REGEN, 2, CGP_SE_REGEN, ownSide->battle[i].battle.position,
                              &ownSide->battle[i].battle);
                break;
            case CGP_CARD_REGEN_LARGE:
            case CGP_CARD_REGEN_LARGE_BATTLE:
                ownSide->battle[i].battle.layers[0].life += 3;
                if (work->definitions[battleCard].operationDuration <
                    ownSide->battle[i].battle.layers[0].life) {
                    ownSide->battle[i].battle.layers[0].life =
                        work->definitions[battleCard].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_REGEN_START, 0, 0, ownSide->battle[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_REGEN, 3, CGP_SE_REGEN, ownSide->battle[i].battle.position,
                              &ownSide->battle[i].battle);
                break;
            }
        }
        foeCard = foeSide->battle[i].battle.layers[0].cardId;
        if (foeCard >= 0) {
            if (foeCard == CGP_CARD_FOE_TRIGGER) {
                CardPlayCommandPlay(otherSide, CGP_COMMAND_FOE_TRIGGERED, work, i, 0);
            }
        }
    }
}

/* Battle slot (0..3) of the side's battle row the phase is working on. */
extern s32 D_00A57E20;
/* Battle slot of the other side's row that the support attacks land on (steps 41 and 42). */
extern s32 D_00A57E24;
/* Frames the phase still waits before its next step. */
extern u8 D_00A57E28;
/* Cleared when the support step starts; no reader found. */
extern u8 D_00A57E29;

/*
 * Steps of CGPBattleFaseProc (command.phaseTimer). The values are the entries of its
 * switch jump table jtbl_00A4F4C0 (0x00a4f4c0, dispatched at 0x00a312cc..0x00a312e8 on the
 * s16 phaseTimer, 62 entries, every other entry goes to the end label 0x00a31138):
 * 0 -> 0x00a302f0, 1 -> 0x00a304d0, 5 -> 0x00a30560, 10 -> 0x00a305d8, 11 -> 0x00a30640,
 * 20 -> 0x00a30698, 21 -> 0x00a30718, 22 -> 0x00a307f0, 30 -> 0x00a30870, 31 -> 0x00a30908,
 * 32 -> 0x00a30950, 40 -> 0x00a30998, 41 -> 0x00a30b58, 42 -> 0x00a30c78, 43 -> 0x00a30e50,
 * 50 -> 0x00a30f28, 51 -> 0x00a30fb8, 52 -> 0x00a31038, 60 -> 0x00a310e0, 61 -> 0x00a31108.
 * The names describe what the code at each target does.
 */
#define CGP_BATTLE_STEP_BEGIN 0            /* check that a battle can take place */
#define CGP_BATTLE_STEP_COMMAND 1          /* queued battle command, then the front attacks */
#define CGP_BATTLE_STEP_END 5              /* nothing (more) to fight: next turn */
#define CGP_BATTLE_STEP_FRONT 10           /* CGP_ATTACK_FRONT attacks, one slot per step */
#define CGP_BATTLE_STEP_FRONT_WAIT 11
#define CGP_BATTLE_STEP_PIERCE 20          /* CGP_ATTACK_PIERCE attacks */
#define CGP_BATTLE_STEP_PIERCE_WAIT 21
#define CGP_BATTLE_STEP_PIERCE_COMMAND 22  /* after the command of a CGP_CARD_PIERCE_COMMAND card */
#define CGP_BATTLE_STEP_COUNTER 30         /* the foe's counter cards strike back */
#define CGP_BATTLE_STEP_COUNTER_ATTACK 31
#define CGP_BATTLE_STEP_COUNTER_WAIT 32
#define CGP_BATTLE_STEP_SUPPORT_CHARGE 40  /* support cards count down and show their effect */
#define CGP_BATTLE_STEP_SUPPORT 41         /* CGP_ATTACK_SUPPORT attacks on each foe slot */
#define CGP_BATTLE_STEP_SUPPORT_NEXT 42
#define CGP_BATTLE_STEP_SUPPORT_COMMAND 43 /* after the command of a CGP_CARD_SUPPORT_COMMAND card */
#define CGP_BATTLE_STEP_DIRECT 50          /* CGP_ATTACK_DIRECT attacks */
#define CGP_BATTLE_STEP_DIRECT_WAIT 51
#define CGP_BATTLE_STEP_FINISH 52
#define CGP_BATTLE_STEP_CLOSE 60
#define CGP_BATTLE_STEP_CLOSE_WAIT 61

/* Foe battle cards that strike back during this side's battle (`addiu $5,$0,0x5`, 0x00a30304; `0xC`, 0x00a30314). */
#define CGP_CARD_COUNTER_1 5
#define CGP_CARD_COUNTER_2 12
/* Operation card in play on either side that stops the battle (`addiu $5,$0,0x7D`, 0x00a3044c). */
#define CGP_OPE_NO_BATTLE 125
/* Card whose attack is followed by its queued command (`addiu $4,$0,0x4C`, 0x00a30744). */
#define CGP_CARD_PIERCE_COMMAND 76
/* Support card that plays CGP_COMMAND_SUPPORT_FOLLOWUP after its attacks (`addiu $4,$0,0x20`, 0x00a30d84;
 * `addiu $5,$0,0x49`, 0x00a30dcc). */
#define CGP_CARD_SUPPORT_COMMAND 32
#define CGP_COMMAND_SUPPORT_FOLLOWUP 73
/* A support card's second layer below this life wears down after the attacks (`slti $2,$2,0x50`, 0x00a30d24). */
#define CGP_SUPPORT_WEAR_LIMIT 80

/* Error messages of the phase (`addiu $6,$0,6`, 0x00a303ac; `0x14`, 0x00a30410; `0x15`, 0x00a310bc). */
#define CGP_ERR_NO_BATTLE 6
#define CGP_ERR_BATTLE_BLOCKED 20
#define CGP_ERR_BATTLE_DONE 21
/* Value shown with CGP_ERR_BATTLE_BLOCKED while command.recoveryApplied is set (`addiu $7,$0,0x5C`, 0x00a30418). */
#define CGP_BATTLE_BLOCK_RECOVERY 92

/* Frames the phase waits after an attack (`addiu $2,$0,0x1E`) and after a strong support effect (`0x2D`, 0x00a30ab8). */
#define CGP_BATTLE_WAIT_FRAMES 30
#define CGP_BATTLE_SUPPORT_WAIT_FRAMES 45
/* Frames of the slide of the command window and of the closing wait (`addiu $3,$0,0xA`, 0x00a30510). */
#define CGP_BATTLE_SLIDE_FRAMES 10

/* CardSetEffect codes and sound of a charged support card (`addiu $6,$0,0x21`, 0x00a30ad0; `0x17`, 0x00a30b20;
 * `lui $8,2` + `ori $8,$8,6`, 0x00a30ab0). Power 3 or more shows the strong effect. */
#define CGP_EFFECT_SUPPORT_STRONG 33
#define CGP_EFFECT_SUPPORT 23
#define CGP_SUPPORT_STRONG_POWER 3
#define CGP_SE_SUPPORT 0x20006

/*
 * Battle phase of one side, one step per call (command.phaseTimer). After a
 * queued battle command the side's battle slots attack in the order of their
 * attack type: front attacks, piercing attacks, the foe's counter cards, the
 * support cards (which first count down their power, and attack each foe slot
 * once it reaches zero) and the direct attacks. Each attack waits
 * D_00A57E28 frames before the next one.
 */
void CGPBattleFaseProc(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    s32 attackType;
    s32 phase;

    ownSide = &work->player;
    foeSide = &work->enemy;
    if (side != 0) {
        ownSide = &work->enemy;
        foeSide = &work->player;
    }
    phase = work->command.phaseTimer;
    switch ((s16) phase) {
    case CGP_BATTLE_STEP_BEGIN:
    {
        s32 ownCount;
        s32 foeCount;

        CardPlayClearEndflg(ownSide);
        ownCount = CardPlayBattleCnt(ownSide);
        foeCount = CardPlayCntBtl(&foeSide->hand, CGP_CARD_COUNTER_1);
        foeCount += CardPlayCntBtl(&foeSide->hand, CGP_CARD_COUNTER_2);
        if (ownCount == 0 && foeCount == 0) {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMess(side, work, CGP_ERR_NO_BATTLE);
            return;
        }
        if (work->command.recoveryApplied != 0) {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMessPlus(side, work, CGP_ERR_BATTLE_BLOCKED, CGP_BATTLE_BLOCK_RECOVERY);
            return;
        }
        if (work->command.turnCountdown != 0 || CardPlayCntOpe(ownSide, CGP_OPE_NO_BATTLE) != 0 ||
            CardPlayCntOpe(foeSide, CGP_OPE_NO_BATTLE) != 0) {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
            CGPSetErrorMessPlus(side, work, CGP_ERR_BATTLE_BLOCKED, CGP_OPE_NO_BATTLE);
            return;
        }
        CGPRotStageSub(work, side);
        D_00A57E20 = 0;
        work->command.phaseTimer = CGP_BATTLE_STEP_COMMAND;
        return;
    }
    case CGP_BATTLE_STEP_COMMAND:
        if (CardPlayCommandCheck(work->command.phase,
                                 side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
            work->cursor.selectionMode = 1;
            CardPlayCommandBattleExecute(side, work->command.phase, work);
            work->cursor.selectionMode = 1;
        } else {
            work->command.slideDirection = 1;
            work->cursor.selectionMode = 0;
            work->command.phaseTimer = CGP_BATTLE_STEP_FRONT;
            work->command.slideTimer = CGP_BATTLE_SLIDE_FRAMES;
            work->command.step = 0;
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_END:
        if (CardPlayCommandCheck(work->command.phase,
                                 side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
            work->cursor.selectionMode = 1;
            CardPlayCommandBattleExecute(side, work->command.phase, work);
        } else {
            CGPNextTurnSub(work);
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_FRONT:
        if (CGPBattleSub(side, CGP_ATTACK_FRONT, D_00A57E20, work, ownSide, foeSide, 0)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[D_00A57E20].battle);
        D_00A57E28 = 0;
        if (attackType == CGP_ATTACK_FRONT) {
            D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
        }
        D_00A57E20++;
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_FRONT_WAIT:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
            break;
        }
        CGPRefreshFieldHP(work);
        CardCursorPassive(&work->cursor);
        work->command.step = 0;
        if (D_00A57E20 >= 4) {
            work->command.phaseTimer = CGP_BATTLE_STEP_PIERCE;
            D_00A57E20 = 0;
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        break;
    case CGP_BATTLE_STEP_PIERCE:
        if (CGPBattleSub(side, CGP_ATTACK_PIERCE, D_00A57E20, work, ownSide, foeSide, 0)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[D_00A57E20].battle);
        D_00A57E28 = 0;
        if (attackType == CGP_ATTACK_PIERCE) {
            D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
        }
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_PIERCE_WAIT:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
            break;
        }
        CGPRefreshFieldHP(work);
        CardCursorPassive(&work->cursor);
        work->command.step = 0;
        if (ownSide->battle[D_00A57E20].battle.layers[0].cardId == CGP_CARD_PIERCE_COMMAND) {
            if (CardPlayCommandCheck(work->command.phase,
                                     side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
                CardPlayCommandBattleExecute(side, work->command.phase, work);
            }
            work->command.phaseTimer++;
            D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
            D_00A57E20++;
        } else {
            D_00A57E20++;
            if (D_00A57E20 >= 4) {
                work->command.phaseTimer = CGP_BATTLE_STEP_COUNTER;
                D_00A57E20 = 0;
            } else {
                work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            }
        }
        break;
    case CGP_BATTLE_STEP_PIERCE_COMMAND:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
        } else {
            CardCursorPassive(&work->cursor);
            work->command.step = 0;
            if (D_00A57E20 >= 4) {
                work->command.phaseTimer = CGP_BATTLE_STEP_COUNTER;
                D_00A57E20 = 0;
            } else {
                work->command.phaseTimer = CGP_BATTLE_STEP_PIERCE;
            }
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_COUNTER:
    {
        s32 found;
        s32 i;

        found = 0;
        if (CardPlayCntBtl(&foeSide->hand, CGP_CARD_COUNTER_1) != 0 ||
            CardPlayCntBtl(&foeSide->hand, CGP_CARD_COUNTER_2) != 0) {
            for (i = 0; i < 4; i++) {
                if (foeSide->battle[i].battle.layers[0].cardId == CGP_CARD_COUNTER_1 ||
                    foeSide->battle[i].battle.layers[0].cardId == CGP_CARD_COUNTER_2) {
                    if (foeSide->battle[i].battle.layers[1].cardId < 0) {
                        D_00A57E20 = i;
                        found = 1;
                        work->command.phaseTimer++;
                    }
                }
            }
        }
        if (found) {
            return;
        }
        work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
        break;
    }
    case CGP_BATTLE_STEP_COUNTER_ATTACK:
        if (CGPBattleSub(side == 0, CGP_ATTACK_PIERCE, D_00A57E20, work, foeSide, ownSide, 0)) {
            return;
        }
        work->command.phaseTimer++;
        D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
        break;
    case CGP_BATTLE_STEP_COUNTER_WAIT:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
            break;
        }
        CGPRefreshFieldHP(work);
        CardCursorPassive(&work->cursor);
        D_00A57E20 = 0;
        D_00A57E24 = 0;
        work->command.step = 0;
        work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
        break;
    case CGP_BATTLE_STEP_SUPPORT_CHARGE:
        D_00A57E29 = 0;
        if (ownSide->battle[D_00A57E20].battle.layers[0].cardId >= 0) {
            if (CardChkAttackType(work->definitions, &ownSide->battle[D_00A57E20].battle) == CGP_ATTACK_SUPPORT &&
                !(ownSide->battle[D_00A57E20].battle.flags & CGP_STACK_FLAG_FROZEN)) {
                s32 power;

                ownSide->battle[D_00A57E20].battle.power = (s8) ownSide->battle[D_00A57E20].battle.power - 1;
                power = CardPlayBattleCnt(foeSide);
                if ((s8) ownSide->battle[D_00A57E20].battle.power == 0 && power != 0) {
                    power = CGPCalcPower(work, &ownSide->battle[D_00A57E20].battle);
                    power += CGPCalcPowerPlus(work, (CardPowerCard *) &ownSide->battle[D_00A57E20].battle, power);
                    if (power >= CGP_SUPPORT_STRONG_POWER) {
                        D_00A57E28 = CGP_BATTLE_SUPPORT_WAIT_FRAMES;
                        CardSetEffect(side, work, CGP_EFFECT_SUPPORT_STRONG, 0, CGP_SE_SUPPORT,
                                      ownSide->battle[D_00A57E20].battle.position, 0);
                    } else {
                        D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
                        CardSetEffect(side, work, CGP_EFFECT_SUPPORT, 0, CGP_SE_SUPPORT,
                                      ownSide->battle[D_00A57E20].battle.position, 0);
                    }
                }
            }
        }
        phase = work->command.phaseTimer;
        work->command.phaseTimer = phase + 1;
        break;
    case CGP_BATTLE_STEP_SUPPORT:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
            break;
        }
        if (CGPBattleSub(side, CGP_ATTACK_SUPPORT, D_00A57E20, work, ownSide, foeSide,
                         &foeSide->battle[D_00A57E24].battle)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[D_00A57E20].battle);
        D_00A57E28 = 0;
        if (attackType == CGP_ATTACK_SUPPORT) {
            if (foeSide->battle[D_00A57E24].battle.layers[0].cardId >= 0 &&
                (s8) ownSide->battle[D_00A57E20].battle.power == 0) {
                D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
            }
        }
        work->command.phaseTimer++;
        D_00A57E24++;
        break;
    case CGP_BATTLE_STEP_SUPPORT_NEXT:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
            break;
        }
        work->command.step = 0;
        if (D_00A57E24 >= 4) {
            CardCursorPassive(&work->cursor);
            if (ownSide->battle[D_00A57E20].battle.layers[0].cardId >= 0) {
                if (CardChkAttackType(work->definitions, &ownSide->battle[D_00A57E20].battle) == CGP_ATTACK_SUPPORT) {
                    if (ownSide->battle[D_00A57E20].battle.layers[1].cardId >= 0) {
                        if (ownSide->battle[D_00A57E20].battle.layers[1].life < CGP_SUPPORT_WEAR_LIMIT) {
                            if (!(ownSide->battle[D_00A57E20].battle.flags & CGP_STACK_FLAG_FROZEN)) {
                                ownSide->battle[D_00A57E20].battle.layers[1].life--;
                            }
                        }
                    }
                    if ((s8) ownSide->battle[D_00A57E20].battle.power <= 0) {
                        ownSide->battle[D_00A57E20].battle.power = ownSide->battle[D_00A57E20].battle.powerLimit;
                    }
                }
            }
            D_00A57E24 = 0;
            if (ownSide->battle[D_00A57E20].battle.layers[0].cardId == CGP_CARD_SUPPORT_COMMAND &&
                !(ownSide->battle[D_00A57E20].battle.flags & CGP_STACK_FLAG_FROZEN)) {
                CardPlayCommandPlay(side, CGP_COMMAND_SUPPORT_FOLLOWUP, work, D_00A57E20, 0);
                work->command.phaseTimer++;
                D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
            } else {
                D_00A57E20++;
                if (D_00A57E20 >= 4) {
                    work->command.phaseTimer = CGP_BATTLE_STEP_DIRECT;
                    D_00A57E20 = 0;
                } else {
                    work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
                }
            }
        } else {
            work->command.phaseTimer = phase - 1;
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_SUPPORT_COMMAND:
        if (CardPlayCommandCheck(work->command.phase,
                                 side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
            CardPlayCommandBattleExecute(side, work->command.phase, work);
            return;
        }
        if (D_00A57E28 != 0) {
            D_00A57E28--;
        } else {
            D_00A57E20++;
            if (D_00A57E20 >= 4) {
                work->command.phaseTimer = CGP_BATTLE_STEP_DIRECT;
                D_00A57E20 = 0;
            } else {
                work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
            }
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_DIRECT:
        if (CGPBattleSub(side, CGP_ATTACK_DIRECT, D_00A57E20, work, ownSide, foeSide, 0)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[D_00A57E20].battle);
        D_00A57E28 = 0;
        if (attackType == CGP_ATTACK_DIRECT) {
            D_00A57E28 = CGP_BATTLE_WAIT_FRAMES;
        }
        D_00A57E20++;
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_DIRECT_WAIT:
        if (D_00A57E28 != 0) {
            D_00A57E28--;
            break;
        }
        CardCursorPassive(&work->cursor);
        work->command.step = 0;
        if (D_00A57E20 >= 4) {
            D_00A57E20 = 0;
            work->command.phaseTimer++;
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_FINISH:
        work->command.slideTimer = CGP_BATTLE_SLIDE_FRAMES;
        work->command.phaseTimer = CGP_BATTLE_STEP_CLOSE;
        work->command.slideDirection = 0;
        if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
            return;
        }
        if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
            return;
        }
        CGPSetErrorMess(side, work, CGP_ERR_BATTLE_DONE);
        return;
    case CGP_BATTLE_STEP_CLOSE:
        CGPRefreshFieldHP(work);
        D_00A57E28 = CGP_BATTLE_SLIDE_FRAMES;
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_CLOSE_WAIT:
        CGPRefreshFieldHP(work);
        if (D_00A57E28 != 0) {
            D_00A57E28--;
        } else {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
        }
        break;
    }
}

/* Interrupt message CGPEndFaseProc shows when the hand is over the limit ("Too many cards in your hand"). */
#define CGP_MESS_HAND_FULL 0x2B
/* Error message of the discard prompt ("Throw away this card?"). */
#define CGP_ERR_DISCARD_CONFIRM 25
/* Frames the hand list stays inert after it opens (`addiu $3,$0,0x19`, 0x00a312f8). */
#define CGP_DISCARD_INPUT_DELAY 25

/* Where the enlarged card of the discard selection is drawn (0x3EBCED91 and 0x3E99999A, 0x00a313d4..0x00a313e4). */
#define CGP_DISCARD_PREVIEW_Y 0.369f
#define CGP_DISCARD_PREVIEW_Z 0.3f

/* Steps of CGPEndFaseProc (command.phaseTimer). */
#define CGP_END_STEP_BEGIN 0
#define CGP_END_STEP_DRAW 1
#define CGP_END_STEP_HAND_CHECK 2
#define CGP_END_STEP_SELECT 3
#define CGP_END_STEP_CONFIRM 4
#define CGP_END_STEP_NEXT_TURN 10

extern int CardPlayCommandExecute(s16 command, CardGameWork *work);
extern void CardEnemyEnd(int side, CardGameWork *work);
extern int CardPlayDesertCard(CardHand *hand, int index);

/* Hand index the discard selection stands on (read `lw` 0x00a31354, written 0x00a312fc). */
extern s32 D_00A57E2C;
/* Frames left before the discard selection reacts (`lw` 0x00a31318, set to 25 at 0x00a31300). */
extern s32 D_00A57E30;

/*
 * Last phase of a turn for one side: runs the pending command queues, then steps
 * through the end of the turn. A hand holding too many cards opens a list window
 * (CGPCheckTeMax) from which the player picks the card to throw away: left and right
 * move the selection, triangle shows the card large and circle asks "Throw away
 * this card?"; circle there discards it and checks the hand again, cross goes back
 * to the selection. A hand within the limit ends the phase at once. The computer
 * player ends the phase itself (CardEnemyEnd).
 */
void CGPEndFaseProc(s32 side, CardGameWork *work, CardListWindow *window)
{
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    u16 held;
    u16 repeat;
    s32 count;
    float position[4];
    Matrix4 matrix;

    ownSide = &work->player;
    foeSide = &work->enemy;
    held = work->command.input[0].held;
    repeat = work->command.input[0].repeat;
    if (side != 0) {
        ownSide = &work->enemy;
        foeSide = &work->player;
        held = work->command.input[1].held;
        repeat = work->command.input[1].repeat;
    }
    if (CardPlayCommandCheck(work->command.phase, work->commandQueue[0][0]) +
        CardPlayCommandCheck(work->command.phase, work->commandQueue[1][0]) != 0) {
        CardPlayCommandExecute(work->command.phase, work);
        return;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_END_STEP_BEGIN:
        work->cursor.selectionMode = 0;
        work->command.phaseTimer++;
        /* fall through */
    case CGP_END_STEP_DRAW:
        work->command.phaseTimer++;
        CGPEndFaseSub(side, ownSide, foeSide, work);
        break;
    case CGP_END_STEP_HAND_CHECK:
        if (CGPCheckTeMax(window, &ownSide->hand) == 0) {
            work->command.phaseTimer = CGP_END_STEP_NEXT_TURN;
            break;
        }
        if (side == 0 ? (work->flags & CGP_FLAG_SIDE0_AUTO) : (work->flags & CGP_FLAG_SIDE1_AUTO)) {
            CardEnemyEnd(side, work);
            break;
        }
        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
        CGPSetInterruptMess(work, CGP_MESS_HAND_FULL);
        CGPRotStageSub(work, side);
        work->command.phaseTimer++;
        D_00A57E2C = 0;
        D_00A57E30 = CGP_DISCARD_INPUT_DELAY;
        break;
    case CGP_END_STEP_SELECT:
        if (D_00A57E30 != 0) {
            D_00A57E30--;
            break;
        }
        {
            s32 flipped = side == 0 ? work->rotStageId != 0 : work->rotStageId == 0;

            CGCPHandDispSub(&ownSide->hand, D_00A57E2C, flipped);
        }
        count = CardPlayHandCnt(&ownSide->hand);
        if (repeat & CGP_PAD_RIGHT) {
            if (D_00A57E2C < count - 1) {
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                D_00A57E2C++;
            }
        } else if (repeat & CGP_PAD_LEFT) {
            if (D_00A57E2C > 0) {
                xglSoundEffectNormalID(CGP_SE_CURSOR, 0);
                D_00A57E2C--;
            }
        }
        if (held & CGP_PAD_PREVIEW) {
            position[0] = 0.0f;
            position[1] = CGP_DISCARD_PREVIEW_Y;
            position[2] = CGP_DISCARD_PREVIEW_Z;
            position[3] = 1.0f;
            xglMatrixUnit(matrix);
            xglMatrixTrans(matrix, (const float (*)[4]) matrix, position);
            CardDisp(matrix, ownSide->hand.cards[D_00A57E2C], 1, 5);
        }
        if (repeat & CGP_PAD_CONFIRM) {
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            work->command.phaseTimer++;
        }
        break;
    case CGP_END_STEP_CONFIRM:
    {
        s32 flipped = side == 0 ? work->rotStageId != 0 : work->rotStageId == 0;

        CGPDispErrorMess2(side, CGP_ERR_DISCARD_CONFIRM);
        CGCPHandDispSub(&ownSide->hand, D_00A57E2C, flipped);
        if (repeat & CGP_PAD_CONFIRM) {
            work->command.phaseTimer = CGP_END_STEP_HAND_CHECK;
            CardPlayDesertCard(&ownSide->hand, D_00A57E2C);
            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
        } else if (repeat & CGP_PAD_CANCEL) {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
        }
        break;
    }
    case CGP_END_STEP_NEXT_TURN:
        CGPNextTurnSub(work);
        break;
    }
}

extern void CardPlayDispHand(s32 side, s32 faceUp, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispBattle(s32 side, CardGameWork *work, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispDispose(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispOperation(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispYama(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispSute(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispJunk(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispCursor(s32 mode, CardGameWork *work, Matrix4 matrix);
extern void CardEnemyMove(s32 side, CardGameWork *work);
extern void CardEnemySet(s32 side, CardGameWork *work);
extern void CaedGameWinMode(s32 outcome, CardGameWork *work, s32 round);
extern s32 NewCMPGameModeExit(s32 player, CardGameWork *work, s32 round);
extern void xglMatrixRotX(Matrix4 destination, const Matrix4 source, float angle);


/*
 * Matrix copies below are the original's constrained lq/sq pairs (the sceVu0Copy shape, the
 * same form as tu001 CardMainInit). Per copy: 8 instructions, lq $2,off(src) then
 * sq $2,off(dest) for off = 0x00, 0x10, 0x20, 0x30. The original addresses are:
 *   0x00a32158..0x00a32174  models[4]  (dest $3)
 *   0x00a3217c..0x00a32198  models[5]  (dest $4)
 *   0x00a321c4..0x00a321e0  cursorMatrix (dest $18)
 *   0x00a3230c..0x00a32328  models[6]  (dest $3)
 *   0x00a32330..0x00a3234c  models[7]  (dest $4)
 * each is one __asm__ statement; $2 is the scratch GPR the original uses; "memory" orders the copy with the
 * surrounding accesses to the matrices.
 */

/*
 * Pad-flip loop, original 0x00a315c8..0x00a31640: `addu $5,$17,$7` and `addu $6,$10,$7` add one
 * offset register to two bases, work and work + 8. The offset register $7 starts at 0x5AD0 and steps
 * by `addiu $7,$7,2` (k = 0..2), so it is the halfword index of input[0].held + k scaled by two
 * (CardGameWork viewed as u16 words); the enemy base work + 8 is the same view advanced by one
 * CardCommandInput (4 halfwords). Addressed through the structure members the compiler folds 0x5AD0
 * into each address and the shared offset register is lost.
 */
/* Halfword index of command.input[0].held in CardGameWork (+0x5AD0 / 2). */
#define CARD_PAD_INPUT0_WORD 0x2D68
#define CARD_CMD_INPUT_WORDS (sizeof(CardCommandInput) / sizeof(u16))
/* 0xC000 sign-extended: the loop keeps it in $9 (addiu $9,$0,-0x4000, 0x00a315c4) ahead of the work + 8 base in $10. */
#define CARD_PAD_UP_PAIR (-0x4000)

/*
 * CARD_PAD_FLIP(pad, mask, turned): flips the d-pad bits of the halfword `*pad` (two statements,
 * `turned` is a u16 temporary, `mask` is the 0xC000 pair as the sign-extended register constant the
 * loop keeps, see CARD_PAD_UP_PAIR). The 0xF000 d-pad nibble moves: the 0xC000 bit pair shifts left
 * by 2, the 0x3000 pair shifts right by 2, and the low 12 bits stay (PADFLIP, original
 * 0x00a315e0..0x00a31640).
 */
#define CARD_PAD_FLIP(pad, mask, turned)                                     \
    turned = ((*(pad) & 0xF000) << 2) & mask;                                 \
    *(pad) = (((*(pad) & 0xF000) >> 2) & 0x3000) | (turned | (*(pad) & 0x0FFF))

/* Operation cards CardGameProc checks on either side. 145, 146 and 147 refuse the end of a turn while the turn's
 * side still has a card to place and a disposal card with definition flag 4 (145) or 2 (147), or any disposal card
 * (146), waiting (`addiu $5,$0,0x91`, 0x00a31a68; `0x92`, 0x00a319f0; `0x93`, 0x00a31afc). 121 turns the hands
 * face down (`addiu $5,$0,0x79`, 0x00a32244 and 0x00a333b4). */
#define CGP_OPE_DISPOSE_LOCK_FLAG4 145
#define CGP_OPE_DISPOSE_LOCK_ANY 146
#define CGP_OPE_DISPOSE_LOCK_FLAG2 147
#define CGP_OPE_HANDS_FACE_DOWN 121

/* Pad and flag bits CardGameProc tests. */
/* Buttons that dismiss the error message window (`andi $2,$3,0xF0`, 0x00a3173c, on the pressed bits of the side's pad). */
#define CGP_PAD_FACE_BUTTONS 0x00F0
/* Button that asks for the phase change (`andi $2,$23,0x80`, 0x00a31ce0). */
#define CGP_PAD_SQUARE 0x0080
/* Start button of either pad, which opens the menu (`andi $2,$2,0x800`, 0x00a31d28 for pad 0 and 0x00a31d64 for pad 1). */
#define CGP_PAD_START 0x0800
/* Button that turns the stage (`andi $2,$4,0x1` on cursorPressed, 0x00a32028). */
#define CGP_PAD_ROTATE_STAGE 0x0001
/* CardGameWork.flags bit that hides the hands (`andi $2,$3,0x80` on the flags halfword, 0x00a321e8 and 0x00a32354). */
#define CGP_FLAG_HIDE_HANDS 0x80
/* Fixed EE runtime address of the model the error window of CardGameProc enters after the texture
 * (`lui $4,0x1DA8` + `ori $4,$4,0x8100`, 0x00a31714..0x00a3171c). */
#define CGP_GAME_MESS_MODEL 0x01DA8100
#define PI_F 3.1415927f

s32 CardGameProc(s32 mode, CardListWindow *window, CardGameWork *work, s32 round)
{
    CardEffectMatrix matrix;
    float offset[4];
    float vec[4];
    CGPBoardArea place;
    CardEffectMatrix cursorMatrix;
    s32 result;
    s32 side;
    u16 pressed;
    u16 cursorPressed;
    s32 faceUp;
    s32 i;
    u16 cpuTurn;
    u16 cpuFoe;

    result = 0;
    switch ((s16) work->command.phase) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
    case 11:
        side = 0;
        pressed = work->command.input[0].pressed;
        break;
    default:
        pressed = work->command.input[1].pressed;
        side = 1;
        break;
    }
    if (work->rotStageId != 0) {
        s32 mask = CARD_PAD_UP_PAIR;
        u16 *playerWords = (u16 *) work;
        u16 *enemyWords = playerWords + CARD_CMD_INPUT_WORDS;
        u16 turned;
        s32 k;
        s32 word;

        for (k = 0; k < 3; k++) {
            u16 *playerPad;
            u16 *enemyPad;

            word = CARD_PAD_INPUT0_WORD + k;
            playerPad = playerWords + word;
            enemyPad = enemyWords + word;
            CARD_PAD_FLIP(playerPad, mask, turned);
            CARD_PAD_FLIP(enemyPad, mask, turned);
        }
    }
    if (mode == 0) {
        offset[0] = 0.0f;
        offset[1] = 1.95f;
        offset[2] = 0.1f;
        offset[3] = 1.0f;
        vec[0] = 1.0f;
        vec[1] = 0.85f;
        vec[2] = 1.0f;
        vec[3] = 1.0f;
        xglMatrixUnit(matrix.m);
        xglMatrixTrans(matrix.m, (const float (*)[4]) matrix.m, offset);
        xglMatrixScale(matrix.m, (const float (*)[4]) matrix.m, vec);
        nmlModelSetPlace(matrix.m);
        if (side == 0) {
            nmlModelSetLight(Player1Color, asDir);
        } else {
            nmlModelSetLight(Player2Color, asDir);
        }
        nmlModelSetTexture((void *) CGP_ERROR_MESS_TEXTURE);
        nmlModelEntryCard((void *) CGP_GAME_MESS_MODEL);
        if (work->errorMessKind != 0) {
            u16 answer = work->command.input[0].pressed;

            if (work->errorMessSide != 0) {
                answer = work->command.input[1].pressed;
            }
            if (answer & CGP_PAD_FACE_BUTTONS) {
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                work->errorMessKind = 0;
                work->flags &= ~CGP_FLAG_ERROR_PLUS;
            }
        } else if (work->command.interruptMessId != 0) {
            if (work->command.interruptMessTimer != 0) {
                work->command.interruptMessTimer--;
            } else {
                work->command.interruptMessId = 0;
                work->command.interruptMessTimer = 0;
                work->interruptMess = 0;
            }
        } else {
            if ((u16) (work->command.flowRequest - 3) >= 3) {
                if (CardPlayYamaCnt(&work->enemy) == 0) {
                    work->command.step = 0;
                    work->command.flowRequest = 3;
                } else if (CardPlayYamaCnt(&work->player) == 0) {
                    work->command.step = 0;
                    work->command.flowRequest = 4;
                }
            }
            if ((s16) work->command.phase != 5 && (s16) work->command.phase != 12) {
                place.area = CGP_AREA_PLAYER_BATTLE;
                for (i = 0; i < 4; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->player, &work->player.battle[i].battle, &place);
                }
                place.area = CGP_AREA_PLAYER_DISPOSAL;
                for (i = 0; i < 4; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->player, &work->player.disposal[i].battle, &place);
                }
                place.area = CGP_AREA_ENEMY_BATTLE;
                for (i = 0; i < 4; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->enemy, &work->enemy.battle[i].battle, &place);
                }
                place.area = CGP_AREA_ENEMY_DISPOSAL;
                for (i = 0; i < 4; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->enemy, &work->enemy.disposal[i].battle, &place);
                }
            }
            if (work->command.flowRequest != 0) {
                switch (work->command.flowRequest) {
                case 1:
                    CGPDispErrorMess(-1, 0);
                    if (pressed & CGP_PAD_CONFIRM) {
                        CardPlaySide *turnSide = &work->player;
                        s32 blocked;
                        s32 slot;

                        xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                        switch ((s16) work->command.phase) {
                        case 8:
                            turnSide = &work->enemy;
                            /* fall through */
                        case 1:
                            blocked = 0;
                            if (CardPlayCntOpe(&work->player, CGP_OPE_DISPOSE_LOCK_ANY) != 0 || CardPlayCntOpe(&work->enemy, CGP_OPE_DISPOSE_LOCK_ANY) != 0) {
                                if (CardPlayBattleCnt(turnSide) < 4 && CardPlayDisposeCnt(turnSide) > 0) {
                                    CGPSetErrorMessPlus(1, work, 14, CGP_OPE_DISPOSE_LOCK_ANY);
                                    xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                                    blocked = 1;
                                }
                            }
                            if (CardPlayCntOpe(&work->player, CGP_OPE_DISPOSE_LOCK_FLAG4) != 0 || CardPlayCntOpe(&work->enemy, CGP_OPE_DISPOSE_LOCK_FLAG4) != 0) {
                                if (CardPlayBattleCnt(turnSide) < 4 && CardPlayDisposeCnt(turnSide) > 0) {
                                    blocked = 0;
                                    for (slot = 0; slot < 4; slot++) {
                                        if (turnSide->disposal[slot].battle.layers[0].cardId >= 0 &&
                                            (work->definitions[turnSide->disposal[slot].battle.layers[0].cardId].flags & 4)) {
                                            CGPSetErrorMessPlus(1, work, 14, CGP_OPE_DISPOSE_LOCK_FLAG4);
                                            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                                            blocked = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (CardPlayCntOpe(&work->player, CGP_OPE_DISPOSE_LOCK_FLAG2) != 0 || CardPlayCntOpe(&work->enemy, CGP_OPE_DISPOSE_LOCK_FLAG2) != 0) {
                                if (CardPlayBattleCnt(turnSide) < 4 && CardPlayDisposeCnt(turnSide) > 0) {
                                    blocked = 0;
                                    for (slot = 0; slot < 4; slot++) {
                                        if (turnSide->disposal[slot].battle.layers[0].cardId >= 0 &&
                                            (work->definitions[turnSide->disposal[slot].battle.layers[0].cardId].flags & 2)) {
                                            CGPSetErrorMessPlus(1, work, 14, CGP_OPE_DISPOSE_LOCK_FLAG2);
                                            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                                            blocked = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                            work->command.flowRequest = 0;
                            if (!blocked) {
                                CGPNextTurnSub(work);
                            }
                            break;
                        case 4:
                            CGPCommFaseEndSub(0, &work->player, work);
                            work->command.flowRequest = 0;
                            CGPNextTurnSub(work);
                            break;
                        case 11:
                            CGPCommFaseEndSub(1, &work->enemy, work);
                            work->command.flowRequest = 0;
                            CGPNextTurnSub(work);
                            break;
                        default:
                            work->command.flowRequest = 0;
                            CGPNextTurnSub(work);
                            break;
                        }
                    } else if (pressed & CGP_PAD_CANCEL) {
                        work->command.flowRequest = 0;
                        xglSoundEffectNormalID(CGP_SE_CANCEL, 0);
                    }
                    break;
                case 3:
                    if (work->fase == 9) {
                        CaedGameWinMode(1, work, round);
                    } else {
                        CaedGameWinMode(0, work, round);
                    }
                    result = 1;
                    break;
                case 4:
                    if (work->fase == 9) {
                        CaedGameWinMode(2, work, round);
                    } else {
                        CaedGameWinMode(-1, work, round);
                    }
                    result = 2;
                    break;
                case 5:
                    CaedGameWinMode(3, work, round);
                    break;
                case 6:
                {
                    s32 exitResult = NewCMPGameModeExit(work->menuPlayer, work, round);

                    if (exitResult < 0) {
                        work->command.flowRequest = 0;
                    } else if (exitResult != 0) {
                        if (CGPChkFaseLock(work)) {
                            work->command.flowRequest = 1;
                            xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                        }
                    }
                    break;
                }
                }
            } else {
                if (pressed & CGP_PAD_SQUARE) {
                    if (CGPChkFaseLock(work)) {
                        work->command.flowRequest = 1;
                        xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
                    }
                }
                if (work->command.input[0].pressed & CGP_PAD_START) {
                    work->command.flowRequest = 6;
                    work->menuPlayer = 0;
                    work->command.exitMenuSub = -1;
                    work->command.exitMenuItem = 0;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                } else if (work->command.input[1].pressed & CGP_PAD_START) {
                    work->command.flowRequest = 6;
                    work->menuPlayer = 1;
                    work->command.exitMenuSub = -1;
                    work->command.exitMenuItem = 0;
                    xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
                }
                if (work->fadeOutFlag != 0) {
                    work->fadeOutFlag = 0;
                    CGPNextTurnSub(work);
                } else {
                    switch ((s16) work->command.phase) {
                    case 0:
                        CGPDrawFaseProc(0, work, &work->player, &work->enemy);
                        break;
                    case 1:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            CardEnemyMove(0, work);
                        } else {
                            CGPMoveFaseProc(0, work, &work->player);
                        }
                        break;
                    case 3:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            CardEnemySet(0, work);
                        } else {
                            CGPSetFaseProc(0, work, &work->player);
                        }
                        break;
                    case 4:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            cpuFoe = 1;
                        } else {
                            cpuFoe = 0;
                        }
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            cpuTurn = 1;
                        } else {
                            cpuTurn = 0;
                        }
                        CGPCommFaseSub(1, work, cpuTurn, cpuFoe);
                        break;
                    case 5:
                        CGPBattleFaseProc(0, work);
                        break;
                    case 6:
                        CGPEndFaseProc(0, work, window);
                        break;
                    case 7:
                        CGPDrawFaseProc(1, work, &work->enemy, &work->player);
                        break;
                    case 8:
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            CardEnemyMove(1, work);
                        } else {
                            CGPMoveFaseProc(1, work, &work->enemy);
                        }
                        break;
                    case 10:
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            CardEnemySet(1, work);
                        } else {
                            CGPSetFaseProc(1, work, &work->enemy);
                        }
                        break;
                    case 9:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            cpuFoe = 1;
                        } else {
                            cpuFoe = 0;
                        }
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            cpuTurn = 1;
                        } else {
                            cpuTurn = 0;
                        }
                        CGPCommFaseSub(1, work, cpuTurn, cpuFoe);
                        break;
                    case 2:
                    case 11:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            cpuTurn = 1;
                        } else {
                            cpuTurn = 0;
                        }
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            cpuFoe = 1;
                        } else {
                            cpuFoe = 0;
                        }
                        CGPCommFaseSub(0, work, cpuTurn, cpuFoe);
                        break;
                    case 12:
                        CGPBattleFaseProc(1, work);
                        break;
                    case 13:
                        CGPEndFaseProc(1, work, window);
                        break;
                    }
                }
            }
        }
    }
    xglMatrixUnit(matrix.m);
    switch ((s16) work->command.phase) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
    case 6:
    case 11:
        cursorPressed = work->command.input[0].pressed;
        break;
    default:
        cursorPressed = work->command.input[1].pressed;
        break;
    }
    if (work->rotStageFlag != 0) {
        float dy = -2.0f;
        float dx = -0.09f;

        work->rotStageFlag--;
        if (work->rotStageActive == 0) {
            dy = 2.0f;
            dx = 0.09f;
        }
        work->rotStageVector[1] += dy;
        work->rotStageVector[0] += dx;
    }
    if (work->rotStageCooldown == 0) {
        if (cursorPressed & CGP_PAD_ROTATE_STAGE) {
            if ((s16) work->command.phase == 5 || (s16) work->command.phase == 12) {
                xglSoundEffectNormalID(CGP_SE_BUZZER, 0);
            } else {
                work->rotStageCooldown = 20;
                work->rotStageId = (work->rotStageId + 1) & 1;
                xglSoundEffectNormalID(CGP_SE_DECIDE, 0);
            }
        }
    } else {
        work->rotStageCooldown--;
        work->rotStageVector[2] += -9.0f;
    }
    vec[0] = vec[1] = 0.0f;
    vec[2] = work->rotStageVector[0];
    if (!(work->flags & CGP_FLAG_ERROR_PLUS) && work->command.commandIndex == 0) {
        vec[2] = work->rotStageVector[0] - work->command.slideOffset;
    }
    vec[3] = 1.0f;
    xglMatrixTrans(matrix.m, (const float (*)[4]) matrix.m, vec);
    xglMatrixRotX(matrix.m, (const float (*)[4]) matrix.m, work->rotStageVector[1] / 180.0f * PI_F);
    xglMatrixRotZ(matrix.m, (const float (*)[4]) matrix.m, work->rotStageVector[2] / 180.0f * PI_F);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[4].matrix), "r" (&matrix)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[5].matrix), "r" (&matrix)
        : "$2", "memory");
    xglMatrixRotZ(work->models[15].matrix, (const float (*)[4]) matrix.m, -work->rotStageVector[2] / 180.0f * PI_F);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" (&cursorMatrix), "r" (&matrix)
        : "$2", "memory");
    faceUp = 0;
    if (!(work->flags & CGP_FLAG_HIDE_HANDS)) {
        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
            faceUp = 1;
        } else {
            switch ((s16) work->command.phase) {
            case 4:
            case 7:
            case 8:
            case 9:
            case 10:
            case 12:
            case 13:
                faceUp = 1;
                break;
            case 2:
            case 11:
                if ((s16) work->command.phaseTimer >= 10) {
                    faceUp = 1;
                }
                break;
            }
        }
    }
    if (CardPlayCntOpe(&work->player, CGP_OPE_HANDS_FACE_DOWN) != 0 || CardPlayCntOpe(&work->enemy, CGP_OPE_HANDS_FACE_DOWN) != 0) {
        faceUp = 0;
    }
    CardPlayDispHand(0, faceUp, &work->player, matrix.m);
    CardPlayDispBattle(0, work, &work->player, matrix.m);
    CardPlayDispDispose(0, &work->player, matrix.m);
    CardPlayDispOperation(0, &work->player, matrix.m);
    CardPlayDispYama(0, &work->player, matrix.m);
    CardPlayDispSute(0, &work->player, matrix.m);
    CardPlayDispJunk(0, &work->player, matrix.m);
    CardDispEffect(0, work, &matrix);
    xglMatrixRotZ(matrix.m, (const float (*)[4]) matrix.m, PI_F);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[6].matrix), "r" (&matrix)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[7].matrix), "r" (&matrix)
        : "$2", "memory");
    faceUp = 0;
    if (!(work->flags & CGP_FLAG_HIDE_HANDS)) {
        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
            faceUp = 1;
        } else {
            switch ((s16) work->command.phase) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 5:
            case 6:
            case 11:
                faceUp = 1;
                break;
            case 4:
            case 9:
                if ((s16) work->command.phaseTimer >= 10) {
                    faceUp = 1;
                }
                break;
            }
        }
    }
    if (CardPlayCntOpe(&work->player, CGP_OPE_HANDS_FACE_DOWN) != 0 || CardPlayCntOpe(&work->enemy, CGP_OPE_HANDS_FACE_DOWN) != 0) {
        faceUp = 0;
    }
    CardPlayDispHand(1, faceUp, &work->enemy, matrix.m);
    CardPlayDispBattle(1, work, &work->enemy, matrix.m);
    CardPlayDispDispose(1, &work->enemy, matrix.m);
    CardPlayDispOperation(1, &work->enemy, matrix.m);
    CardPlayDispYama(1, &work->enemy, matrix.m);
    CardPlayDispSute(1, &work->enemy, matrix.m);
    CardPlayDispJunk(1, &work->enemy, matrix.m);
    CardDispEffect(1, work, &matrix);
    CardPlayDispCursor(mode, work, cursorMatrix.m);
    CardPlayDispInfo(work);
    return result;
}



char PermMess000Txt[8] = "\241\241\241\241";

char PermMess001Txt[40] = "\013\031\003\r\000 Please select a Lv10 Shion.";

char PermMess002Txt[56] = "\013\031\003\r\000 Please select a card to be returned to the hand.";

char PermMess003Txt[56] = "\013\031\003\r\000 Please select a card to be added to the hand.";

char PermMess004Txt[56] = "\013\031\003\r\000 Please select a card to discard from the hand.";

char PermMess005Txt[56] = "\013\031\003\r\000 Please select a card to be returned to the hand.";

char PermMess006Txt[48] = "\013\031\003\r\000 Please select a card to be replaced.";

char PermMess007Txt[48] = "\013\031\003\r\000 Please select a card to recover its HP.";

char PermMess008Txt[48] = "\013\031\003\r\000 Please select a card to double its ATK.";

char PermMess009Txt[40] = "\013\031\003\r\000 Please select a card to attack.";

char PermMess010Txt[40] = "\013\031\003\r\000 Please select a card to down.";

char PermMess011Txt[56] = "\013\031\003\r\000 Please select a card to remove down status.";

char PermMess012Txt[48] = "\013\031\003\r\000 Please select a card to be discarded.";

char PermMess013Txt[56] = "\013\031\003\r\000 Please select a weapon card to be discarded.";

char PermMess014Txt[64] = "\013\031\003\r\000 Please select a card to return to the standby area.";

static char OLMess0000Txt[8] = "\241\241\241\241";

static char OLMess0010Txt[32] = "\013\031\003\r\000 \243\261P will draw a card.";

static char OLMess0011Txt[48] = "\013\031\003\r\000 \243\261P can exchange a card in their hand.";

static char OLMess0020Txt[56] = "\013\031\003\r\000 \243\261P can move the battle cards on the field.";

static char OLMess0030Txt[48] = "\013\031\003\r\000 \243\261P can use a battle or situation card.";

static char OLMess0040Txt[56] = "\013\031\003\r\000 \243\261P can use an event card or the card text.";

static char OLMess0041Txt[40] = "\013\031\003\r\000 \243\261P can use an event card.";

static char OLMess0042Txt[40] = "\013\031\003\r\000 \243\262P can use an event card.";

static char OLMess0050Txt[32] = "\013\031\003\r\000 Engaging in battle.";

static char OLMess0060Txt[32] = "\013\031\003\r\000 \243\262P will draw a card.";

static char OLMess0061Txt[48] = "\013\031\003\r\000 \243\262P can exchange a card in their hand.";

static char OLMess0070Txt[56] = "\013\031\003\r\000 \243\262P can move the battle cards on the field.";

static char OLMess0080Txt[48] = "\013\031\003\r\000 \243\262P can use a battle or situation card.";

static char OLMess0090Txt[56] = "\013\031\003\r\000 \243\262P can use an event card or the card text.";
