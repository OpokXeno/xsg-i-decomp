/*
 * OV10 original TU 8: 0x00a21768..0x00a324b8 (68 functions)
 */
#include "common.h"
#include "cgp.h"

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPRecalcPosSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CCO143ExecSub);

extern int printf(const char *format, ...);
extern char D_00A4E4F0[]; /* "Yama[%2d]:" */
extern char D_00A4E500[]; /* "%3d," */
extern char D_00A4E508[]; /* "\n" */
extern char D_00A4E510[]; /* "Junk[%2d]:" */

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
extern char D_00A4E500[]; /* "%3d," */
extern char D_00A4E508[]; /* "\n" */
extern char D_00A4E520[]; /* "Sute[%2d]:" */

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardPlayDispInfo);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkFaseLock);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardSetEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CDEMatrixSub);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPMoveDSBATTLEwork);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPequipSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPWeaponEquipChk);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDisposeWeaponSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursor2Area);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursor2Pos);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCMSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursorMove2);

/* CGPCursorMove2, which does the actual cursor movement, is still
 * assembler-scaffolded in this TU. */
extern void CGPCursorMove2(s32 flag, CardGameWork *work);

void CGPCursorMove(s32 flag, CardGameWork *work)
{
    CGPCursorMove2(flag, work);
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPSetCursor);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkPlayable);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkOpePlayable);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardPlayClearEndflg);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkCardPlayCost);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardPlayDisployment);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispoSub0);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDisposeSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPMoveSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleBeforeEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleAttackEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleDiffenceEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBAEsub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleAfterEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleCalcuration);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCalcPower);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCalcPowerEnv);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPHPCheckSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPRefreshFieldHP);

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

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPAnswerSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCommFaseEndSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCommFaseSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPEndFaseSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPEndFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardGameProc);
