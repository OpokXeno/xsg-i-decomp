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
    {0.800000011920929f, 0.800000011920929f, 1.2000000476837158f, 1.0f}
};

float Player2Color[4][4] = {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.2000000476837158f, 0.800000011920929f, 0.800000011920929f, 1.0f}
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
    u8 unmodeled_00[3];
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
} CardPlayField;

extern float HandPos1P[];
extern float HandPos2P[];

extern void CGPCursor2Pos(CGPCursorPosition *position, CardGameWork *work);

extern int CardPlayIdentityCheck(CardGameWork *work, CardHand *hand, u16 identity);
extern int CardPlayRecavery(CardHand *hand);
extern s16 *CPCSearchCommWk(u16 side, CardGameWork *work);
extern s32 CardPlayCommandPlay(u16 side, u16 command, CardGameWork *work,
                               s16 operand, s16 target);
extern int CardPlayCntOpe(CardPlaySide *side, int opeId);
extern float DisposePos1P[][3];
extern float OperationPos1P[][3];

extern void CGPDispErrorMessPlus(s8 side, u8 reason, s16 value);
extern s32 CGPCursor2Area(CardGameWork *work);
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
extern const char D_00A4E4F0[];
extern const char D_00A4E500[];
extern const char D_00A4E508[];
extern const char D_00A4E510[];

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

extern int printf(const char *format, ...);
extern const char D_00A4E500[];
extern const char D_00A4E508[];
extern const char D_00A4E520[];

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

void CGPSetErrorMess(s8 code, CardGameWork *work, u8 reason)
{
    work->errorMessCode = code;
    work->errorMessKind = 1;
    work->errorMessReason = reason;
    work->errorMessValue = -1;
}

void CGPSetErrorMessPlus(s8 code, CardGameWork *work, u8 reason, s16 value)
{
    work->errorMessCode = code;
    work->flags |= CGP_FLAG_ERROR_PLUS;
    work->errorMessKind = 3;
    work->errorMessReason = reason;
    work->errorMessValue = value;
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMessCore);

/*
 * Screen position and scale CGPDispErrorMessCore (still assembler in this TU)
 * places the rendered message with; declared locally next to the callers
 * that build one on their own stack.
 */
typedef struct CGPErrorLayout {
    float x;
    float y;
    float scaleX;
    float scaleY;
} CGPErrorLayout;

extern void CGPDispErrorMessCore(CardGameWork *work, u16 reason, s32 x, s32 y,
                                  CGPErrorLayout *layout);

void CGPDispErrorMess3(CardGameWork *work, s32 reason)
{
    CGPErrorLayout layout;

    layout.x = 1.1f;
    layout.y = 0.0f;
    layout.scaleX = 0.5f;
    layout.scaleY = 1.0f;
    CGPDispErrorMessCore(work, reason, 230, 164, &layout);
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMessPlus);

void CGPDispErrorMess(CardGameWork *work, s32 reason)
{
    CGPErrorLayout layout;

    layout.x = 0.0f;
    layout.y = 0.0f;
    layout.scaleX = 0.5f;
    layout.scaleY = 1.0f;
    CGPDispErrorMessCore(work, reason, 124, 164, &layout);
}

void CGPDispErrorMess2(CardGameWork *work, s32 reason)
{
    CGPErrorLayout layout;

    layout.x = 0.0f;
    layout.y = 0.48f;
    layout.scaleX = 0.5f;
    layout.scaleY = 1.0f;
    CGPDispErrorMessCore(work, reason, 124, 116, &layout);
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
        if (work->errorMessCode == 0) {
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
                CGPDispErrorMessPlus(work->errorMessCode, work->errorMessReason,
                                     work->errorMessValue);
            } else {
                CGPDispErrorMess((CardGameWork *) (s32) work->errorMessCode,
                                 work->errorMessReason);
            }
        } else if (work->errorMessKind & CGP_FLAG_ERROR_PLUS) {
            CGPDispErrorMessPlus(-1, work->errorMessReason, work->errorMessValue);
        } else {
            CGPDispErrorMess((CardGameWork *) -1, work->errorMessReason);
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
        xglFontPrint(2, 2, 0xFFF0, D_00A4E530);
        xglFontPrint(0, 0, 0, D_00A4E538);
        xglFontPrint(2, 2, 0xFFF0, text);
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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkFaseLock);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardDispEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCheckTeMax);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardGameInit);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPNextTurnSub);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPequipSub);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDisposeWeaponSub);

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

extern float HandPos1P[];
extern float HandPos2P[];

extern void CGPCursor2Pos(CGPCursorPosition *position, CardGameWork *work);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursorMove2);

/* CGPCursorMove2, which does the actual cursor movement, is still
 * assembler-scaffolded in this TU. */
extern void CGPCursorMove2(s32 flag, CardGameWork *work);

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
        work->cursorPos[0] = HandPos1P[0];
        work->cursorPos[1] = HandPos1P[1];
        work->cursorPos[2] = HandPos1P[2];
    } else {
        work->cursorPos[0] = HandPos2P[0];
        work->cursorPos[1] = HandPos2P[1];
        work->cursorPos[2] = HandPos2P[2];
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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkOpePlayable);

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

extern s32 CardPlayCommandPlay(u16 side, u16 command, CardGameWork *work,
                               s16 operand, s16 target);
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
    if (work->definitions[ownSide->hand.cards[index]].kind == 2) {
        error = 11;
    } else {
        if (work->definitions[ownSide->hand.cards[index]].kind == 1) {
            if (ownSide->hand.playPoints[0] < work->definitions[ownSide->hand.cards[index]].playCost) {
                error = 12;
            }
        } else if (work->definitions[ownSide->hand.cards[index]].kind == 4) {
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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDisposeSub);

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

extern int CardPlayCntOpe(CardPlaySide *side, int opeId);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleDiffenceEffect);

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
                    if (work->definitions[card].kind == 1) {
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

extern s32 CGPBattleAttackEffect(s32 side, CardGameWork *work, s32 power,
                                 CardLayerStack *attacker, CardLayerStack *defender);
extern s32 CGPBattleDiffenceEffect(s32 side, CardGameWork *work, s32 power,
                                   CardLayerStack *defender, CardLayerStack *attacker);

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
        if (CardChkAttackType(work->definitions, card) == 4) {
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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPHPCheckSub);

typedef struct CGPBoardArea {
    s16 area;
    s16 slot;
} CGPBoardArea;
extern void CGPHPCheckSub(CardDefinition *definitions, CardPlaySide *side,
                          CardLayerStack *stack, CGPBoardArea *place);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleSub);

void CGPRotStageSub(CardGameWork *work, u32 stage)
{
    if ((work->save->flags & CARD_SAVE_ROTATE_STAGE) && work->fase == 9 &&
        work->rotStageId != stage && work->rotStageCooldown == 0)
    {
        work->rotStageId = stage;
        work->rotStageCooldown = 20;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDrawFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPMoveFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPSetFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCFPlayCommOperation);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCFSDispSub);

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

extern s32 CardPlayCntDsp(CardPlaySide *side, s16 cardId);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCommFaseSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPEndFaseSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPEndFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardGameProc);



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
