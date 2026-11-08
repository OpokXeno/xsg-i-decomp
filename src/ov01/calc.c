/*
 * OV01 original TU 4: 0x00a10bb8..0x00a191c0 (116 functions)
 */
#include "common.h"
#include "ov01/actor.h"
#include "quadword.h"
#include "main/xgl_2.h"
#include "ov01/battle_init.h"
#include "calc.h"

/* calcCfEncount's own callees: calcPartyStat is defined below in this TU. */
extern void calcPartyStat(int, int);
extern void menuTimeSet(int);

/* actStockGet (src/ov01/battle_init.c): the action-stock record calcBoost uses. */
extern int *actStockGet(void);

/*
 * The queued-command word calcTurnStart (this TU) reads through
 * unitCmdPtrGet's return, and the words it writes through unitCmdSet's
 * second argument (both defined in src/ov01/unit_cmd.c).
 */
#include "ov01/unit_cmd.h"
typedef CmdEntry CalcCmdEntry;

extern int calcAp(BattleUnit *unit, int ap);
extern CalcCmdEntry *unitCmdPtrGet(BattleUnit *unit);

/* calcDeadUnitNum: calcSpecChk's own callee, defined below in this TU. */
extern int calcDeadUnitNum(BattleUnit *unit, int flag);

/* calcBattleChk: calcWctDec's own callee, defined below in this TU. */
extern int calcBattleChk(BattleUnit *unit);

/* calcTrg2PE (this TU, defined below): calcDeadUnitNum's own callee. */
extern int calcTrg2PE(BattleUnit *unit, int trgFlags);

/*
 * calcWct/calcStatTurn/rnd: calcTurnEnd's own callees, all defined later
 * in this TU.
 */
extern int calcWct(BattleUnit *unit);
extern void calcStatTurn(BattleUnit *unit);
extern int rnd(int max);

/*
 * dataWpnGet's return type, as far as calcAttWpnChk reads it: a byte at
 * +0x10 that it tests for zero/nonzero (data_unit_org_get.c, dataWpnGet's
 * own TU, owns the rest of the layout).
 */
typedef struct WpnData WpnData;

struct WpnData {
    short attack;                      /* +0x00: base term calcWpnAtkGet reads */
    unsigned char unmodeled_02[0x0F - 0x02];
    signed char apCost;               /* +0x0F: AP cost calcWpnEnableChk compares
                                       * against the AP count calcAp keeps */
    u8 equipped;                      /* +0x10: calcAttWpnChk's own check */
    unsigned char unmodeled_11[0x14 - 0x11];
    short tecId;                      /* +0x14: tech id calcWpnEnableChk passes to
                                       * dataTecGet/calcTakeEpGet */
    unsigned char unmodeled_16[0x18 - 0x16];
    short dblAct;                     /* +0x18: calcDblActChk requires nonzero */
    unsigned char unmodeled_1a[0x1C - 0x1A];
    short phyDefense;                 /* +0x1C: calcPhyDefGet's accumulated term */
};

extern WpnData *dataWpnGet(int weaponId);

/*
 * The curve descriptor calcSpline (this TU) advances every frame and hands
 * to calcLinerSub/calcBezierSub/calcSplineSub/calcHermiteSub; calcSplineInit
 * sets it up with the control points, their count, the duration in frames
 * as a float and a fixed 0.5 tension, the frame counter starting at zero.
 * The types are the readers' own: calcSpline loads +0x00 as the point
 * array it indexes by 16 bytes per entry and +0x04 as the point count;
 * calcHermiteSub scales its tangents by +0x0C.
 */
typedef struct CalcSpline {
    Vector4 *points;   /* +0x00 */
    int count;         /* +0x04 */
    float duration;    /* +0x08 */
    float tension;     /* +0x0C */
    int frame;         /* +0x10 */
} CalcSpline;

/*
 * The 0x180-byte parameter table calcCopyParaMake fills from a unit's
 * origin record and calcTotalParaMenuSub hands back. calcCopyParaMake
 * copies the whole origin into it first, so the offsets below are the
 * origin's: agwsId is the halfword at +0x38 (tested against 0x10 before any
 * AGWS adjustment), engineId/frameId are +0x56/+0x58.
 */
typedef struct CalcParaTable {
    short maxHp;            /* +0x00 */
    short maxEp;            /* +0x02 */
    short val4;             /* +0x04 */
    short val6;             /* +0x06 */
    short eatk;             /* +0x08 */
    short edef;             /* +0x0A */
    signed char hit;        /* +0x0C */
    signed char eva;        /* +0x0D */
    unsigned char valE;     /* +0x0E */
    unsigned char unmodeled_0f[0x36 - 0x0F];
    short ep;               /* +0x36 */
    short agwsId;           /* +0x38 */
    unsigned char unmodeled_3a[0x56 - 0x3A];
    short engineId;         /* +0x56 */
    short frameId;          /* +0x58 */
    unsigned char unmodeled_5a[0x180 - 0x5A];
} CalcParaTable;

/*
 * The part adjustment record dataFrmGet (frame) and dataEngGet (engine)
 * return; calcCopyParaMake adds its +0x08/+0x0A/+0x0C/+0x0E fields into
 * the table.
 */
typedef struct CalcPartAdj {
    unsigned char unmodeled_00[8];
    short a8;               /* +0x08 */
    short aA;               /* +0x0A */
    short aC;               /* +0x0C */
    unsigned char aE;       /* +0x0E */
} CalcPartAdj;

extern int unitAgwsPilotGet(int charaId);
extern unsigned char *dataFrmGet(int index);
extern unsigned char *dataEngGet(int index);
extern const char calcAGWSParaEPEPMAXText[]; /* "** AGWS para = EP=%d EPMAX=%d\n" */
extern const char calcAGWSParaEatkEdefHITEVAText[]; /* "** AGWS para = Eatk=%d Edef=%d HIT=%d EVA=%d\n" */
extern CalcCharParaData *calcTotalParaMenuSub(int charaId, int *attack, int *defense, unsigned char *table);

/*
 * Builds the parameter table for a unit from its origin record. For an AGWS
 * unit, the pilot's stats and the frame and engine adjustments are applied
 * on top.
 */
void calcCopyParaMake(CalcCharParaData *origin, unsigned char *table)
{
    unsigned char buf[0x180];
    CalcParaTable *t;
    CalcParaTable *r;
    CalcPartAdj *frm;
    CalcPartAdj *eng;
    int pilot;

    frm = 0;
    eng = 0;
    t = (CalcParaTable *) table;
    *t = *(CalcParaTable *) origin;
    if (t->agwsId > 0x10) {
        pilot = unitAgwsPilotGet(t->agwsId);
        if (t->frameId != 0) {
            frm = (CalcPartAdj *) dataFrmGet(t->frameId);
        }
        if (t->engineId != 0) {
            eng = (CalcPartAdj *) dataEngGet(t->engineId);
        }
        if (pilot != 0) {
            r = (CalcParaTable *) calcTotalParaMenuSub(pilot, 0, 0, buf);
            t->eatk = r->eatk;
            t->edef = r->edef;
            t->hit = r->hit;
            t->eva = r->eva;
            t->ep = r->ep;
            t->maxEp = r->maxEp;
            printf(calcAGWSParaEPEPMAXText, t->ep, t->maxEp);
            printf(calcAGWSParaEatkEdefHITEVAText, t->eatk, t->edef, t->hit, t->eva);
        }
        if (frm != 0) {
            t->maxHp += frm->a8;
            t->val6 += frm->aA;
            t->valE += frm->aE;
            t->edef += frm->aC;
        }
        if (eng != 0) {
            t->val4 += eng->a8;
        }
    }
}

/*
 * Stack-local stand-in for a unit that has no BattleUnit. calcTotalParaMenuSub
 * points the `work` chain of a BattleUnit/Actor pair at a temporary
 * CalcUnitParam-compatible record, so calcSkillInit, calcWpnAtkGet,
 * calcPhyDefGet and calcMagDefGet can run on it. The 0x110-byte Actor extent
 * keeps the stack frame at its original size.
 */
typedef struct CalcScratchActor {
    u32 flags;                                    /* +0x00 */
    unsigned char unmodeled_04[0x84 - 0x04];
    struct CalcUnitParam *up;                     /* +0x84 */
    unsigned char unmodeled_88[0x110 - 0x88];
} CalcScratchActor;

typedef struct CalcScratchUnit {
    BattleUnit unit;            /* +0x00 */
    unsigned char unmodeled_30[0x80 - 0x30];
} CalcScratchUnit;

/* unitTblGet (src/ov01/unit_cmd.c): writes the active unit table's base
 * address through its second argument and returns its count. */
extern int unitTblGet(int type, BattleUnit ***table);

/* calcPara2OrgSub (this TU, below): writes one unit's HP/EP back. */
extern void calcPara2OrgSub(BattleUnit *unit);

/*
 * Writes every unit's HP/EP back to its persistent origin record, and the
 * linked unit's through calcTotalParaMenuSub's fake-unit pattern; a unit
 * carrying CalcUnitParam flag 0x40 hands its own currentEp to the linked
 * unit first.
 */
void calcPara2Org(void)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    BattleUnit **table;
    int count;
    int i;

    count = unitTblGet(0, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            calcPara2OrgSub(table[i]);
            if (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_IS_AGWS) {
                fakeUnit.unit.work = (Actor *) &fakeActor;
                fakeActor.up = calcUPGet(table[i])->linkedUnit;
                fakeActor.flags = 0;
                fakeActor.up->currentEp = calcUPGet(table[i])->currentEp;
                calcPara2OrgSub(&fakeUnit.unit);
            } else if (calcUPGet(table[i])->linkedUnit != 0) {
                fakeUnit.unit.work = (Actor *) &fakeActor;
                fakeActor.up = calcUPGet(table[i])->linkedUnit;
                fakeActor.flags = 0;
                calcPara2OrgSub(&fakeUnit.unit);
            }
        }
    }
}

/* dataUnitOrgGet (src/ov01/data_unit_org_get.c), this TU's own
 * private view (see CalcCharParaData, src/ov01/calc.h). */
extern CalcCharParaData *dataUnitOrgGet(int charaId);

void calcPara2OrgSub(BattleUnit *unit) {
    CalcUnitParam *param;
    CalcCharParaData *origin;

    param = calcUPGet(unit);
    origin = dataUnitOrgGet(param->charaId);
    origin->hp = param->currentHp;
    origin->ep = param->currentEp;
}

/*
 * Sets the HP of every unit's origin record to 1 when the unit's table slot,
 * re-read after the two lookups, is gone or dead (ACTOR_FLAG_DEAD), or when
 * the origin record's HP is already zero. A slot that is empty at the first
 * read is skipped: it has no origin record to update.
 *
 * The original keeps the unit table pointer in a register across the loop and
 * refreshes that register after the two calls through a second load, then
 * indexes the element through the refreshed copy. Only this spelling, the
 * re-read into its own local and the cursor assigned from it, reproduces
 * that copy: reading `table` again directly merges into one load.
 */
void calcDeadReset(void)
{
    BattleUnit **table;
    BattleUnit **units;
    BattleUnit **refreshed;
    BattleUnit *unit;
    CalcCharParaData *origin;
    int count;
    int i;

    i = 0;
    count = unitTblGet(0, &table);
    if (count > 0) {
        units = table;
        do {
            unit = units[i];
            if (unit != 0) {
                origin = dataUnitOrgGet(calcUPGet(unit)->charaId);
                refreshed = table;
                units = refreshed;
                if (refreshed[i] == 0
                    || (refreshed[i]->work->flags & ACTOR_FLAG_DEAD)
                    || origin->hp == 0) {
                    origin->hp = 1;
                }
            }
        } while (++i < count);
    }
}

CalcUnitParam *calcUPGet(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    return actor->up;
}

int calcEp(BattleUnit *unit, int delta);

/*
 * calcHpDmg: applies one hit-table mode to dst (and, for the drain modes
 * 7-10, gives the same amount back to src). Modes 1/3 subtract from dst,
 * 2/4 add to it, and each odd/even pair chooses HP (calcHp) or EP
 * (calcEp). Other modes change nothing and report success.
 */
int calcHpDmg(BattleUnit *src, BattleUnit *dst, int packedHit, int amount)
{
    unsigned char mode = packedHit;
    int ret;

    switch (mode) {
    case 1:
        ret = calcHp(dst, -amount);
        break;
    case 3:
        ret = calcEp(dst, -amount);
        break;
    case 2:
        ret = calcHp(dst, amount);
        break;
    case 4:
        ret = calcEp(dst, amount);
        break;
    case 7:
    case 8:
        ret = calcHp(dst, -amount);
        calcHp(src, amount);
        break;
    case 9:
    case 10:
        ret = calcEp(dst, -amount);
        calcEp(src, amount);
        break;
    default:
        ret = 1;
        break;
    }
    return ret;
}

/*
 * calcTotalParaMenuSub (this TU, defined below): the worker calcTotalParaMenu
 * forwards to, with the scratch table below.
 */
extern CalcCharParaData *calcTotalParaMenuSub(int charaId, int *attack, int *defense, unsigned char *table);

/* The 0x180-byte scratch table calcTotalParaMenu passes to
 * calcTotalParaMenuSub; calcCopyParaMake fills it, and the CalcCharParaData
 * and CalcUnitParam views read the same storage. */
static unsigned char unitPara_0[0x180];

CalcCharParaData *calcTotalParaMenu(int charaId, int *attack, int *defense)
{
    return calcTotalParaMenuSub(charaId, attack, defense, unitPara_0);
}

/* calcCopyParaMake/calcSkillInit/calcWpnAtkGet/calcPhyDefGet/calcMagDefGet:
 * calcTotalParaMenuSub's own callees, all defined in this TU below; the
 * latter four each read their argument
 * through calcUPGet, so calcTotalParaMenuSub hands them its own fake unit
 * above. */
extern void calcCopyParaMake(CalcCharParaData *origin, unsigned char *table);
extern int calcSkillInit(BattleUnit *unit);
extern int calcWpnAtkGet(BattleUnit *unit, int slot);
extern int calcPhyDefGet(BattleUnit *unit);
extern int calcMagDefGet(BattleUnit *unit);

/*
 * Builds the derived parameters of charaId into table and returns it as a
 * CalcCharParaData. attack and defense (when non-null) receive the three
 * weapon attack values and the physical and magic defense values.
 */
CalcCharParaData *calcTotalParaMenuSub(int charaId, int *attack, int *defense, unsigned char *table)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    CalcCharParaData *scratch;
    CalcCharParaData *origin;
    int i;

    scratch = (CalcCharParaData *) table;
    fakeUnit.unit.work = (Actor *) &fakeActor;
    calcCopyParaMake(dataUnitOrgGet(charaId), table);
    fakeActor.up = (CalcUnitParam *) table;
    calcSkillInit(&fakeUnit.unit);

    origin = dataUnitOrgGet(charaId);
    if (scratch->maxHp < origin->hp) {
        origin->hp = scratch->maxHp;
    }
    if (origin->ep > scratch->maxEp) {
        origin->ep = scratch->maxEp;
    }
    if (attack != 0) {
        for (i = 0; i < 3; i++) {
            attack[i] = calcWpnAtkGet(&fakeUnit.unit, i);
        }
    }
    if (defense != 0) {
        defense[0] = calcPhyDefGet(&fakeUnit.unit);
        defense[1] = calcMagDefGet(&fakeUnit.unit);
    }
    return scratch;
}

/*
 * The record calcTakeEpGetMenu builds calcTotalParaMenu's recalculated
 * record into; nothing else in this TU fills the rest of it.
 */
typedef struct CalcTakeEpPara {
    unsigned char unmodeled_00[0x84];
    CalcCharParaData *totalPara; /* +0x84: calcTotalParaMenu's own return value */
    unsigned char unmodeled_88[0x110 - 0x88];
} CalcTakeEpPara;

/*
 * The context block calcTakeEpGetMenu builds for its only call to
 * calcTakeEpGet (this TU); nothing else in this TU fills the rest of it.
 */
typedef struct CalcTakeEpArg {
    BattleUnit unit;           /* +0x00: unit.work (+0x10) is the record above */
    unsigned char unmodeled_30[0x80 - 0x30];
} CalcTakeEpArg;

/* calcTakeEpGet takes a unit: calcWpnEnableChk (this TU) passes its own
 * unit argument. */
extern int calcTakeEpGet(BattleUnit *unit, int tecId);

/* dataEthGet (src/ov01/data_unit_org_get.c), this TU's own
 * private view (see CalcEtherStatus, src/ov01/calc.h). */
extern CalcEtherStatus *dataEthGet(int etherId);

int calcTakeEpGetMenu(int charaId, int etherId) {
    CalcTakeEpArg arg;
    CalcTakeEpPara para;

    arg.unit.work = (Actor *) &para;
    para.totalPara = calcTotalParaMenu(charaId, 0, 0);
    return calcTakeEpGet(&arg.unit, dataEthGet(etherId)->tecId);
}

/*
 * dataTecGet's return, as far as calcBp2Tid reads it: a byte at +0x11
 * selecting the BP timeline (data_unit_org_get.c, dataTecGet's own TU,
 * owns the rest of the layout).
 */
typedef struct TecKindView TecKindView;

struct TecKindView {
    /*
     * +0x00: attribute bits: calcAtkChk's range class (TEC_ATTR_RANGE, defined
     * beside calcAtkChk) and calcUseEtherMenu's CALC_TRG_ALL_PARTY.
     */
    u8 attr;
    signed char epCost;     /* +0x01: base term calcTakeEpGet reads */
    u16 flags;              /* +0x02: bit 0x800 rules out a counter */
    int effectType : 8; /* +0x04: effect type, 4 clears statuses (calcStatClr) */
    /*
     * +0x05: 1 makes the hit double *damage* while the target carries status
     * (0, 0x20), which calcStat then resets.
     */
    int doubleDamageKind : 8;
    /*
     * +0x06: base term calcPowLevGet reads (unsigned byte). TecRecord
     * (src/ov01/data_unit_org_get.c) owns
     * the rest of this layout and does not yet name this byte or the next
     * one either (its unmodeled_00[0x11] covers both).
     */
    u8 powLevBase;
    signed char agility;    /* +0x07: base term calcPlusAglGet reads */
    /*
     * +0x08/+0x09/+0x0A: calcStatAtk's status-test bytes, tested against 1
     * under TEC flags 0x4000, 0x2000 and 0x1000 respectively.
     */
    unsigned char statFlagKind[3];
    unsigned char unmodeled_0b[0x0C - 0x0B];
    signed char statCategory; /* +0x0C: status category calcStatClr reads */
    signed char statRate;     /* +0x0D: status rate calcStatAtk reads */
    u16 statMask;           /* +0x0E: status mask calcStatClr reads */
    unsigned char unmodeled_10[0x11 - 0x10];
    /*
     * +0x11: calcBp2Tid's timeline selector, read as a signed byte, hence
     * `signed char`.
     */
    signed char kind;
    /* +0x12: calcCrit's attribute, passed to calcAtrChg. */
    u16 attribute;
};

/*
 * Bits of TecKindView.flags (+0x02), each named by the effect its one test
 * has; the field stays u16.
 */
enum TecFlag {
    /* futtobiChk, on its technique argument: clear, no knockback (result 0). */
    TEC_FLAG_KNOCKBACK = 0x100,
    /* calcHitRate: set, the hit roll is skipped (result 0). */
    TEC_FLAG_NO_HIT_ROLL = 0x200,
    /* calcHitRate: set, the first roll's result 2 is blocked. */
    TEC_FLAG_NO_HIT_RESULT_2 = 0x400
};

extern TecKindView *dataTecGet(int tecId);

/*
 * The battle command tgtFindFirst (below) passes on to
 * tgtFindChk, as far as either reads it: tgtFindFirst resolves +0x00
 * through dataTecGet and +0x08 through dataItmGet, takes the target from
 * +0x04, and branches on mode 3 (an item, as in sndConvSpid's mode 3).
 * calcHitRate (below) reads the same +0x00 and +0x04.
 */
typedef struct TgtCmd {
    int tecId;          /* +0x00 */
    BattleUnit *target; /* +0x04 */
    int itmId;          /* +0x08: an item's id, or an ether's for mode 2 */
    int mode;           /* +0x0C: 2 = ether, 3 = item */
} TgtCmd;

/*
 * The hit record calcHitDmg (this TU) fills and hands to
 * calcDmg, which passes it on to calcLinePos; calcLinePos reads only the
 * byte at +0x00 and goes on when it is 1; calcStatClr records the status
 * category and bits the technique clears.
 */
typedef struct CalcHitInfo {
    /*
     * +0x00: calcRevHit's packed word, the hit mode in the low byte and
     * the side above it. A word: calcHitDmg stores it, and calcStat hands it
     * whole to calcRevHit and stores the answer back; calcLinePos and calcStat
     * read the mode alone.
     */
    int result;
    /*
     * +0x04: the damage amount. calcUseEtherMenu (this TU) hands the record's
     * +0x00 and +0x04 words to calcHpDmg, which applies the word as the
     * calcHp/calcEp delta for the mode in result's low byte.
     */
    int damage;
    /* +0x08: 1 when calcHitDmg's PL ICHIGEKI path fires. */
    int ichigekiHit;
    int setCategory;   /* +0x0C: category calcHitItem sets, 8 = none */
    int setMask;       /* +0x10: mask calcHitItem sets to zero */
    /*
     * +0x14/+0x18: the category and mask calcStatClr records. Read
     * together: category 8 with a zero mask records no clearing; category 8
     * with mask 0xFFFF clears every bit of every category; any other category
     * clears that category's statMask bits.
     */
    int clearCategory;
    int clearMask;
} CalcHitInfo;

/* Mode value of TgtCmd.mode for an ether. */
#define TGT_CMD_MODE_ETHER 2

/*
 * calcHitDmg (this TU): calcUseEtherMenu's own callee. It resolves
 * cmd against the user's unit into hit; calcHpDmg (above) applies hit's mode
 * (its +0x00 word, low byte) and amount (+0x04) to the target.
 */
extern int calcHitDmg(BattleUnit *user, TgtCmd *cmd, CalcHitInfo *hit);

/*
 * Uses ether etherId from the menu: charaId's character casts it on
 * targetCharaId, or on every party member when the technique's attr
 * carries CALC_TRG_ALL_PARTY.
 * Both the user and each target are fake units whose actor points at the
 * character's origin record; the target's maxHp/maxEp and statAltMask are
 * calcTotalParaMenu's recalculated values while calcHitDmg/calcHpDmg run,
 * as in calcUseItemMenu, and the user's maxEp while calcEp takes the cost.
 */
int calcUseEtherMenu(int charaId, int targetCharaId, int etherId)
{
    CalcScratchUnit user;
    CalcScratchUnit target;
    CalcScratchActor userActor;
    CalcScratchActor targetActor;
    TgtCmd cmd;
    CalcHitInfo hit;
    int partyIds[8];
    u16 savedStatAlt[8];
    int *targetIds;
    CalcCharParaData *origin;
    CalcCharParaData *total;
    short maxHp;
    short maxEp;
    int count;
    int member;
    int i;

    user.unit.work = (Actor *) &userActor;
    userActor.up = (CalcUnitParam *) dataUnitOrgGet(charaId);
    target.unit.work = (Actor *) &targetActor;
    cmd.tecId = dataEthGet(etherId)->tecId;
    cmd.target = &target.unit;
    cmd.itmId = etherId;
    cmd.mode = TGT_CMD_MODE_ETHER;
    if (dataTecGet(dataEthGet(etherId)->tecId)->attr & CALC_TRG_ALL_PARTY) {
        targetIds = partyIds;
        count = PartyAllPartyGet(targetIds, 0);
    } else {
        targetIds = partyIds;
        count = 1;
        targetIds[0] = targetCharaId;
    }
    for (member = 0; member < count; member++) {
        origin = dataUnitOrgGet(targetIds[member]);
        total = calcTotalParaMenu(targetIds[member], 0, 0);
        maxHp = origin->maxHp;
        maxEp = origin->maxEp;
        for (i = 0; i < 8; i++) {
            savedStatAlt[i] = origin->statAltMask[i];
        }
        origin->maxHp = total->maxHp;
        origin->maxEp = total->maxEp;
        for (i = 0; i < 8; i++) {
            origin->statAltMask[i] = total->statAltMask[i];
        }
        targetActor.up = (CalcUnitParam *) origin;
        calcHitDmg(&user.unit, &cmd, &hit);
        calcHpDmg(&user.unit, &target.unit, hit.result, hit.damage);
        origin->maxHp = maxHp;
        origin->maxEp = maxEp;
        for (i = 0; i < 8; i++) {
            origin->statAltMask[i] = savedStatAlt[i];
        }
    }
    origin = dataUnitOrgGet(charaId);
    total = calcTotalParaMenu(charaId, 0, 0);
    maxEp = origin->maxEp;
    origin->maxEp = total->maxEp;
    calcEp(&user.unit, -calcTakeEpGetMenu(charaId, etherId));
    origin->maxEp = maxEp;
    return 1;
}

/*
 * dataItmGet's return, as far as calcUseItemMenu, calcItmEnableChk,
 * calcDItemChk, calcHitItem and calcDmgItem read it: the item's
 * target-selection flags at +0x08, handed on to calcTgtUnitTbl, and a flag
 * byte at +0x09 whose bit 0 calcDItemChk tests and whose bit 1 calcDmgItem
 * tests. It is a plain byte, not bit-fields: with the u16 bit-fields at +0x0C
 * the record is 2-aligned and the compiler would read +0x09 bit-fields as
 * halfword loads.
 */
typedef struct ItmTrgView {
    unsigned char unmodeled_00[0x08];
    u8 trgFlags;        /* +0x08 */
    u8 useFlags;        /* +0x09 */
    u8 power;           /* +0x0A: amount used by calcHitItem/calcDmgItem */
    unsigned char unmodeled_0b[0x0C - 0x0B];
    /*
     * +0x0C: the effect bits calcHitItem and calcDmgItem test, read as one
     * halfword. Each name is the hit mode calcHitItem returns for the bit,
     * as calcHpDmg applies it: mode 1 calcHp(-amount), 2 calcHp(+amount), 3/4 the same
     * through calcEp, 5 and 6 neither; the amount is calcDmgItem's.
     */
    u16 recoverHp : 1;      /* bit 0: mode 2, amount power * 50 */
    u16 recoverEp : 1;      /* bit 1: mode 4, amount power * 10 */
    u16 resultMode6 : 1;    /* bit 2: calcHitItem's mode 6 only */
    u16 recoverHpRate : 1;  /* bit 3: mode 0x802 (low byte 2), amount
                             * maxHp * power / 20 */
    u16 resultMode5 : 1;    /* bit 4: calcHitItem's mode 5 only */
    u16 resultMode5Alt : 1; /* bit 5: calcHitItem's mode 5 only */
    u16 : 2;
    u16 hpToOne : 1;        /* bit 8: with power 1, mode 1 and amount
                             * currentHp - 1; otherwise mode 6, amount 0 */
    u16 : 4;
    /*
     * bit 13: calcDmgItem gives it bit 3's amount (maxHp * power / 20);
     * calcHitItem does not test it, so such an item falls to mode 1
     * (calcHp(-amount)): the name follows that pairing.
     */
    u16 damageHpRate : 1;
} ItmTrgView;

extern ItmTrgView *dataItmGet(int itmId);

/* calcUseItem (this TU, below): the item effect of itmId used by user
 * on target; calcUseItemMenu passes no user. */

/*
 * Uses item itmId from the menu on charaId, or on every party member when
 * the item's trgFlags carry CALC_TRG_ALL_PARTY.
 * Each target's origin record stands in as the unit parameter block of a
 * fake unit, with maxHp/maxEp and statAltMask temporarily replaced by
 * calcTotalParaMenu's recalculated values and restored after calcUseItem.
 * targetIds is assigned in each branch rather than once before the if: the
 * single-assignment form changes the order the original loads the array
 * address.
 */
int calcUseItemMenu(int charaId, int itmId)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    int partyIds[8];
    u16 savedStatAlt[8];
    ItmTrgView *item;
    int *targetIds;
    CalcCharParaData *origin;
    CalcCharParaData *total;
    short maxHp;
    short maxEp;
    int count;
    int member;
    int i;

    fakeUnit.unit.work = (Actor *) &fakeActor;
    item = dataItmGet(itmId);
    if (item->trgFlags & CALC_TRG_ALL_PARTY) {
        targetIds = partyIds;
        count = PartyAllPartyGet(targetIds, 0);
    } else {
        targetIds = partyIds;
        count = 1;
        targetIds[0] = charaId;
    }
    for (member = 0; member < count; member++) {
        origin = dataUnitOrgGet(targetIds[member]);
        total = calcTotalParaMenu(targetIds[member], 0, 0);
        maxHp = origin->maxHp;
        maxEp = origin->maxEp;
        for (i = 0; i < 8; i++) {
            savedStatAlt[i] = origin->statAltMask[i];
        }
        origin->maxHp = total->maxHp;
        origin->maxEp = total->maxEp;
        for (i = 0; i < 8; i++) {
            origin->statAltMask[i] = total->statAltMask[i];
        }
        fakeActor.up = (CalcUnitParam *) origin;
        calcUseItem(0, &fakeUnit.unit, itmId);
        origin->maxHp = maxHp;
        origin->maxEp = maxEp;
        for (i = 0; i < 8; i++) {
            origin->statAltMask[i] = savedStatAlt[i];
        }
    }
    return 1;
}

/*
 * dataPlChaGet's return, as far as calcPowLevGet reads it: a short at +0x9A
 * that tecId 0x262/0x263 divides by 5 and adds to the power level. It is the
 * +0x0A halfword of PlCharacter.special[6] (0x48 + 6*0xC + 0xA = 0x9A) in
 * data_unit_org_get.h, which this TU cannot include (that header includes
 * this one), so the plain offset is kept here. CalcSpecSlot views the same
 * 0xC-byte record above and names only +0x04 and +0x06.
 */
typedef struct CalcPlCharaView {
    /*
     * +0x00/+0x0C/+0x10/+0x14: running totals calcResultSub (this TU) adds a
     * battle's share to, clamped to 9999999 and 9999 (inside PlCharacter's
     * unmodeled_0).
     */
    int exp;
    int expToNext;    /* +0x04: calcLvUp's exp left before the next level */
    u16 waitOffset;   /* +0x08: changed by ITEM_EFFECT_WAIT_OFFSET in calcUseItem */
    unsigned char unmodeled_0a[0x0C - 0x0A];
    int techPoint;
    int etherPoint;
    int skillPoint;
    unsigned char unmodeled_18[0x9A - 0x18];
    short special6PowLevTerm; /* +0x9A: PlCharacter.special[6]'s own +0x0A */
} CalcPlCharaView;

extern CalcPlCharaView *dataPlChaGet(int cid);

/*
 * The item record calcUseItem reads through dataItmGet, as far as it goes:
 * the per-stat group selector and power at +0x0A (an index into the
 * four-entry group table, and the amount for the point and growth effects),
 * a target byte at +0x0B that picks the stat
 * reset, the effect word at +0x0C and a halfword value at +0x0E. The value
 * is read three ways: as a signed halfword (`lh $6,0xE`, the stat mask), as
 * an unsigned halfword (`lhu $4,0xE`, the protect bits and the growth
 * tests) and as its low byte (`lbu $4,0xE`, the amount).
 */
typedef struct CalcUseItemRecord {
    unsigned char unmodeled_00[0x0A];
    unsigned char power;        /* +0x0A */
    unsigned char targetSel;    /* +0x0B */
    u16 effectBits;             /* +0x0C */
    union {
        short s;
        u16 u;
        unsigned char b;
    } value;                    /* +0x0E */
} CalcUseItemRecord;

/*
 * The effect bits of CalcUseItemRecord.effectBits that calcUseItem tests,
 * in the order it applies them. The three HP entries all go through
 * calcDmgItem and calcHp; the names say where in the sequence each sits.
 */
enum {
    ITEM_EFFECT_HP_FIRST = 0x0001,
    ITEM_EFFECT_EP = 0x0002,
    ITEM_EFFECT_STATUS_CLEAR = 0x0004,
    ITEM_EFFECT_HP_SECOND = 0x0008,
    ITEM_EFFECT_STAT_SET = 0x0010,
    ITEM_EFFECT_STAT_PROTECT = 0x0020,
    ITEM_EFFECT_WAIT_OFFSET = 0x0040,
    ITEM_EFFECT_GROWTH = 0x0080,
    ITEM_EFFECT_POWER_SWITCH = 0x0100,
    ITEM_EFFECT_SKILL_POINTS = 0x0200,
    ITEM_EFFECT_ETHER_POINTS = 0x0400,
    ITEM_EFFECT_TECH_POINTS = 0x0800,
    ITEM_EFFECT_PARTY_BYTE = 0x1000,
    ITEM_EFFECT_HP_THIRD = 0x2000
};

/* The per-character byte of PartyDataGet's block that ITEM_EFFECT_PARTY_BYTE
 * adds to, capped at 5 (a stored 6 or more is set back to 5). */
#define CALC_PARTY_CHARA_BYTE 0x13F

extern void calcStatResetCha(BattleUnit *unit);
extern void specEscapeProc(void);
extern int calcDmgItem(BattleUnit *unit, TgtCmd *cmd);

/*
 * calcUseItem: applies item itmId's effects to target, the way the item's
 * effect word (+0x0C) selects them, with user as the source for the damage
 * calcDmgItem computes. Returns the last HP result, 1 when no HP effect ran.
 */
int calcUseItem(BattleUnit *user, BattleUnit *target, int itmId)
{
    TgtCmd hit;
    int statGroup[4] = {0, 1, 2, 3};
    CalcUseItemRecord *item;
    CalcUnitParam *up;
    int ret;

    item = (CalcUseItemRecord *) dataItmGet(itmId);
    up = calcUPGet(target);
    hit.tecId = 0;
    hit.target = target;
    hit.itmId = itmId;
    ret = 1;
    /* The effects run in ITEM_EFFECT_* order; the HP and EP entries use calcDmgItem's amount. */
    if (item->effectBits & ITEM_EFFECT_HP_FIRST) {
        ret = calcHp(target, calcDmgItem(user, &hit));
    }
    if (item->effectBits & ITEM_EFFECT_EP) {
        calcEp(target, calcDmgItem(user, &hit));
    }
    if (item->effectBits & ITEM_EFFECT_STATUS_CLEAR) {
        if (calcBattleChk(target) == 0) {
            Actor *actor;

            actor = target->work;
            actor->flags &= ~0x10;
        }
        if (item->targetSel == 0) {
            calcStatResetCha(target);
        } else if (item->targetSel == 1) {
            calcStatReset(target, 0, 0x7FFF, 0);
        } else if (item->targetSel == 2) {
            calcStatReset(target, 1, 0x7FFF, 0);
        }
    }
    if (item->effectBits & ITEM_EFFECT_HP_SECOND) {
        ret = calcHp(target, calcDmgItem(user, &hit));
    }
    if (item->effectBits & ITEM_EFFECT_STAT_SET) {
        calcStatSet(target, statGroup[item->power], item->value.s, 0);
    }
    if (item->effectBits & ITEM_EFFECT_STAT_PROTECT) {
        up->statProtect[statGroup[item->power]] |= item->value.u;
    }
    if (item->effectBits & ITEM_EFFECT_WAIT_OFFSET) {
        if (item->value.s & 0x8000) {
            dataPlChaGet(up->charaId)->waitOffset += item->value.b;
        } else {
            dataPlChaGet(up->charaId)->waitOffset -= item->value.b;
        }
    }
    /* Growth: each value bit adds item->power to one base stat, capped below its limit. */
    if (item->effectBits & ITEM_EFFECT_GROWTH) {
        if (item->value.s & 0x8000) {
            int add;
            int sum;
            unsigned short cur;

            add = item->power;
            cur = up->baseAtk;
            sum = cur + add;
            up->baseAtk = sum;
            if ((short) sum >= 0x3E8) {
                up->baseAtk = 0x3E7;
            }
        }
        if (item->value.s & 0x4000) {
            int add;
            int sum;
            unsigned short cur;

            add = item->power;
            cur = up->phyDefense;
            sum = cur + add;
            up->phyDefense = sum;
            if ((short) sum >= 0x3E8) {
                up->phyDefense = 0x3E7;
            }
        }
        if (item->value.s & 0x2000) {
            int add;
            int sum;
            unsigned short cur;

            add = item->power;
            cur = up->magAttack;
            sum = cur + add;
            up->magAttack = sum;
            if ((short) sum >= 0x3E8) {
                up->magAttack = 0x3E7;
            }
        }
        if (item->value.s & 0x1000) {
            int add;
            int sum;
            unsigned short cur;

            add = item->power;
            cur = up->magDefense;
            sum = cur + add;
            up->magDefense = sum;
            if ((short) sum >= 0x3E8) {
                up->magDefense = 0x3E7;
            }
        }
        if (item->value.s & 0x800) {
            int add;
            int sum;
            unsigned short cur;

            add = item->power;
            cur = up->maxHp;
            sum = cur + add;
            up->maxHp = sum;
            if ((short) sum >= 0x2710) {
                up->maxHp = 0x270F;
            }
        }
        if (item->value.s & 0x400) {
            int add;
            int sum;
            unsigned short cur;

            add = item->power;
            cur = up->maxEp;
            sum = cur + add;
            up->maxEp = sum;
            if ((short) sum >= 0x64) {
                up->maxEp = 0x63;
            }
        }
    }
    if (item->effectBits & ITEM_EFFECT_SKILL_POINTS) {
        dataPlChaGet(up->charaId)->skillPoint += item->power;
        if (dataPlChaGet(up->charaId)->skillPoint >= 0x2710) {
            dataPlChaGet(up->charaId)->skillPoint = 0x270F;
        }
    }
    if (item->effectBits & ITEM_EFFECT_ETHER_POINTS) {
        dataPlChaGet(up->charaId)->etherPoint += item->power;
        if (dataPlChaGet(up->charaId)->etherPoint >= 0x2710) {
            dataPlChaGet(up->charaId)->etherPoint = 0x270F;
        }
    }
    if (item->effectBits & ITEM_EFFECT_TECH_POINTS) {
        dataPlChaGet(up->charaId)->techPoint += item->power;
        if (dataPlChaGet(up->charaId)->techPoint >= 0x2710) {
            dataPlChaGet(up->charaId)->techPoint = 0x270F;
        }
    }
    if (item->effectBits & ITEM_EFFECT_PARTY_BYTE) {
        unsigned char *party;
        int slot;

        party = (unsigned char *) PartyDataGet();
        slot = up->charaId;
        slot += CALC_PARTY_CHARA_BYTE;
        party[slot] += item->power;
        if (*(unsigned char *) ((int) up->charaId + (int) PartyDataGet() + CALC_PARTY_CHARA_BYTE) >= 6) {
            *(unsigned char *) ((int) up->charaId + (int) PartyDataGet() + CALC_PARTY_CHARA_BYTE) = 5;
        }
    }
    if (item->effectBits & ITEM_EFFECT_HP_THIRD) {
        ret = calcHp(target, calcDmgItem(user, &hit));
    }
    if (item->effectBits & ITEM_EFFECT_POWER_SWITCH) {
        switch (item->power) {
        case 1:
            ret = calcHp(target, -calcDmgItem(user, &hit));
            break;
        case 2:
            calcBc(target, 1);
            break;
        case 3:
            specEscapeProc();
            break;
        case 4:
            if (*(int *) ((char *) calcUPGet(target) + 0x17C) < 0x19) {
                *(int *) ((char *) calcUPGet(target) + 0x17C) = 0x19;
            }
            break;
        case 5:
            if (*(int *) ((char *) calcUPGet(target) + 0x17C) < 0x32) {
                *(int *) ((char *) calcUPGet(target) + 0x17C) = 0x32;
            }
            break;
        }
    }
    return ret;
}

/* calcSkillInitSub (this TU, defined below): one equipped skill id of unit. */
extern void calcSkillInitSub(BattleUnit *unit, int skillId);
extern AccessoryData *dataAccGet(int accessoryId);

/*
 * Hands calcSkillInitSub every skill the unit carries: the skill of each
 * of its three accessories, then its three own skill ids. A zero accessory
 * id is skipped, as is a zero skill id from the accessory or from the unit.
 * Always returns 1.
 */
int calcSkillInit(BattleUnit *unit)
{
    CalcUnitParam *up;
    int i;
    int accId;
    int skillId;

    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        accId = up->accessoryId[i];
        if (accId != 0) {
            skillId = dataAccGet(accId)->skillId;
            if (skillId != 0) {
                calcSkillInitSub(unit, skillId);
            }
        }
    }
    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        skillId = up->skillId[i];
        if (skillId != 0) {
            calcSkillInitSub(unit, skillId);
        }
    }
    return 1;
}

/*
 * dataSklGet's return record (src/ov01/data_unit_org_get.c), as
 * far as calcSkillInitSub and calcSkillSub read it: two halfword bit sets.
 */
typedef struct SkillData SkillData;

struct SkillData {
    u16 typeFlags;   /* +0x00: calcSkillSub's own mask test; calcSkillInitSub's
                      * SkillType bits, the low byte a status category */
    u16 levelFlags;  /* +0x02: calcSkillSub returns bits 0-6 of this field;
                      * calcSkillInitSub's SkillParam bits */
};

extern SkillData *dataSklGet(int skillId);

/*
 * SkillData.typeFlags bits calcSkillInitSub tests, in its own order.
 */
enum SkillType {
    SKILL_TYPE_STAT_SET = 0x400,     /* calls calcStatSet(..., 1) */
    SKILL_TYPE_STAT_PROTECT = 0x800, /* calls calcStatProtectSet */
    SKILL_TYPE_PARAM_UP = 0x8000     /* raises the SkillParam members below */
};

/*
 * SkillData.levelFlags bits of a SKILL_TYPE_PARAM_UP skill: each selects the
 * CalcUnitParam member calcSkillInitSub raises by the skill's level (bits
 * 0-6), tested in this order from 0x8000 down to 0x80.
 */
enum SkillParam {
    SKILL_PARAM_MAX_EP = 0x80,        /* maxEp, by level percent, max 99 */
    SKILL_PARAM_MAX_HP = 0x100,       /* maxHp, by level percent, max 9999 */
    SKILL_PARAM_AGILITY = 0x200,      /* agility, max 99 */
    SKILL_PARAM_MAG_DEFENSE = 0x400,  /* magDefense, max 999 */
    SKILL_PARAM_MAG_ATTACK = 0x800,   /* magAttack, max 999 */
    SKILL_PARAM_EVADE = 0x1000,       /* evadeStat, max 99 */
    SKILL_PARAM_HIT = 0x2000,         /* hitStat, max 99 */
    SKILL_PARAM_PHY_DEFENSE = 0x4000, /* phyDefense, max 999 */
    SKILL_PARAM_ATTACK = 0x8000       /* baseAtk, max 999 */
};

/* calcStatSet (this TU, below): calcStatSetChk's own arguments;
 * calcStatProtectSet is defined later in this TU. */
extern void calcStatProtectSet(BattleUnit *unit, int group, int mask);
extern int calcStatSetChk(BattleUnit *unit, int category, int flag, int force);

/*
 * Applies one equipped skill to the unit: a parameter skill raises the
 * members its levelFlags select by its level, each clamped to its maximum;
 * a protect or status skill calls calcStatProtectSet or calcStatSet with the
 * category in the low byte of typeFlags.
 */
void calcSkillInitSub(BattleUnit *unit, int skillId)
{
    SkillData *skill;
    CalcUnitParam *up;
    int level;

    skill = dataSklGet(skillId);
    up = calcUPGet(unit);
    if (skill->typeFlags & SKILL_TYPE_PARAM_UP) {
        level = skill->levelFlags & 0x7F;
        if (skill->levelFlags & SKILL_PARAM_ATTACK) {
            up->baseAtk += level;
            if (up->baseAtk >= 1000) {
                up->baseAtk = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_PHY_DEFENSE) {
            up->phyDefense += level;
            if (up->phyDefense >= 1000) {
                up->phyDefense = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_HIT) {
            up->hitStat += level;
            if (up->hitStat >= 100) {
                up->hitStat = 99;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_EVADE) {
            up->evadeStat += level;
            if (up->evadeStat >= 100) {
                up->evadeStat = 99;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAG_ATTACK) {
            up->magAttack += level;
            if (up->magAttack >= 1000) {
                up->magAttack = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAG_DEFENSE) {
            up->magDefense += level;
            if (up->magDefense >= 1000) {
                up->magDefense = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_AGILITY) {
            up->agility += level;
            if (up->agility >= 100) {
                up->agility = 99;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAX_HP) {
            up->maxHp += up->maxHp * level / 100;
            if (up->maxHp >= 10000) {
                up->maxHp = 9999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAX_EP) {
            up->maxEp += up->maxEp * level / 100;
            if (up->maxEp >= 100) {
                up->maxEp = 99;
            }
        }
    } else if (skill->typeFlags & SKILL_TYPE_STAT_PROTECT) {
        calcStatProtectSet(unit, skill->typeFlags & 0xFF, skill->levelFlags);
    } else if (skill->typeFlags & SKILL_TYPE_STAT_SET) {
        calcStatSet(unit, skill->typeFlags & 0xFF, skill->levelFlags, 1);
    }
}

/* calcSkillSub/AccessoryData.skillId (both this TU, defined below): the
 * per-skill type/level mask test and the accessory's granted skill id. */
extern int calcSkillSub(BattleUnit *unit, int skillId, int typeMask, int levelMask);
extern AccessoryData *dataAccGet(int accessoryId);

/*
 * The sum of calcSkillSub over the same skills calcSkillInit walks: a zero
 * accessory id is skipped, as is a zero skill id from the accessory or from
 * the unit; such slots add nothing.
 */
int calcSkill(BattleUnit *unit, int kind, int mask)
{
    CalcUnitParam *up;
    int result;
    int i;
    int accId;
    int skillId;

    result = 0;
    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        accId = up->accessoryId[i];
        if (accId != 0) {
            skillId = dataAccGet(accId)->skillId;
            if (skillId != 0) {
                result += calcSkillSub(unit, skillId, kind, mask);
            }
        }
    }
    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        skillId = up->skillId[i];
        if (skillId != 0) {
            result += calcSkillSub(unit, skillId, kind, mask);
        }
    }
    return result;
}

int calcSkillSub(BattleUnit *unit, int skillId, int typeMask, int levelMask) {
    SkillData *skill;
    unsigned short levelFlags;
    int result;

    skill = dataSklGet(skillId);
    calcUPGet(unit);
    result = 0;
    if (skill->typeFlags & typeMask) {
        levelFlags = skill->levelFlags;
        result = (levelFlags & levelMask) != 0 ? (levelFlags & 0x7F) : 0;
    }
    return result;
}

int calcAccEquChk(BattleUnit *unit, int accId)
{
    CalcUnitParam *param;
    int i;

    for (i = 0; i < 3; i++)
    {
        param = calcUPGet(unit);
        if (accId == param->accessoryId[i])
        {
            return 1;
        }
    }
    return 0;
}

/* dataAccGet (src/ov01/data_unit_org_get.c), this TU's own
 * private view (see AccessoryData, src/ov01/calc.h). */
extern AccessoryData *dataAccGet(int accessoryId);

int calcSkillEquChk(BattleUnit *unit, int skillId)
{
    CalcUnitParam *param;
    int i;
    short accessoryId;

    for (i = 0; i < 3; i++)
    {
        param = calcUPGet(unit);
        accessoryId = param->accessoryId[i];
        if (accessoryId != 0)
        {
            if (skillId == dataAccGet(accessoryId)->skillId)
            {
                return 1;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        param = calcUPGet(unit);
        if (skillId == param->skillId[i])
        {
            return 1;
        }
    }

    return 0;
}

/* calcSkillEquChk: calcSkillEquChkAll's own callee (this TU); its second call
 * in that function passes the fake unit. */
extern int calcSkillEquChk(BattleUnit *unit, int skillId);

/*
 * Returns 1 when some battle unit has skillId equipped, checking the linked
 * unit of an AGWS unit as well; otherwise returns 0.
 */
int calcSkillEquChkAll(int skillId)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    BattleUnit **table;
    BattleUnit *unit;
    int count;
    int i;
    int found;

    i = 0;
    count = unitTblGet(0, &table);
    found = 0;
    if (count > 0) {
        for (; i < count; i++) {
            unit = table[i];
            if (unit != 0) {
                found = 1;
                if (calcSkillEquChk(unit, skillId) == 0) {
                    if (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_IS_AGWS) {
                        fakeUnit.unit.work = (Actor *) &fakeActor;
                        fakeActor.up = calcUPGet(table[i])->linkedUnit;
                        if (calcSkillEquChk(&fakeUnit.unit, skillId) != 0) {
                            return 1;
                        }
                    }
                } else {
                    return found;
                }
            }
        }
        return 0;
    }
    return found;
}

int calcAp(BattleUnit *unit, int ap)
{
    int current = calcUPGet(unit)->dblActKind;

    if (ap < 0 && current < -ap) {
        return 0;
    }
    current += ap;
    if (current > 6) {
        current = 6;
    }
    calcUPGet(unit)->dblActKind = current;
    return 1;
}

extern int plMutekiGet(void);
extern int enMutekiGet(void);
extern const char calcPLMUTEKIText[]; /* "** PL MUTEKI\n" */
extern const char calcENMUTEKIText[]; /* "** EN MUTEKI\n" */

int calcHp(BattleUnit *unit, int delta)
{
    int hp;
    Actor *actor;

    actor = unit->work;
    if (!(actor->flags & ACTOR_FLAG_ENEMY)) {
        if (plMutekiGet() != 0) {
            printf(calcPLMUTEKIText);
            return 1;
        }
    } else if (enMutekiGet() != 0) {
        printf(calcENMUTEKIText);
        return 1;
    }
    hp = calcUPGet(unit)->currentHp;
    hp += delta;
    if (hp < 0) {
        hp = 0;
    } else if (calcUPGet(unit)->maxHp < hp) {
        hp = calcUPGet(unit)->maxHp;
    }
    calcUPGet(unit)->currentHp = hp;
    return hp != 0;
}

int calcEp(BattleUnit *unit, int delta)
{
    int ep;

    ep = calcUPGet(unit)->currentEp;
    ep += delta;
    if (ep < 0)
    {
        ep = 0;
    }
    else if (calcUPGet(unit)->maxEp < ep)
    {
        ep = calcUPGet(unit)->maxEp;
    }
    calcUPGet(unit)->currentEp = ep;
    return 1;
}

/*
 * Adds bp to the unit's boost-point gauge while its boost count is below
 * the limit; every 50 points become one boost through calcBc, and a full
 * count empties the gauge. Returns 1 on every path; calcBp2Tid ignores the
 * result.
 */
int calcBp(BattleUnit *unit, int bp)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);
    if (up->bcValue < up->bcLimit) {
        up->bpGauge += bp;
        if (up->bpGauge >= 50) {
            up->bpGauge %= 50;
            calcBc(unit, 1);
        }
    }
    if (up->bcValue >= up->bcLimit) {
        up->bpGauge = 0;
    }
    return 1;
}

extern int menuTimeGet(void);
extern const char calcBPUpEventTimerGText[]; /* "** BP up: EventTimer G (%d)\n" */

int calcBp2Tid(BattleUnit *unit, int tecId)
{
    Actor *actor;
    int kind;
    int tid;

    actor = unit->work;
    if (actor->flags & ACTOR_FLAG_ENEMY) {
        return 0;
    }

    kind = dataTecGet(tecId)->kind;
    if (kind == 0) {
        tid = 4;
    } else if (kind == 1) {
        tid = 6;
    } else if (kind == 2) {
        tid = 10;
    } else {
        tid = 0;
    }

    if (menuTimeGet() == 2) {
        tid *= 3;
        printf(calcBPUpEventTimerGText, tid);
    }

    calcBp(unit, tid);
    return 1;
}

/*
 * Adds delta to the unit's boost count, clamped to 0..bcLimit. Typed int, not
 * void: the original returns 1 on every path, and the callers ignore the value.
 */
int calcBc(BattleUnit *unit, int delta)
{
    int bc;

    bc = calcUPGet(unit)->bcValue;
    bc += delta;
    if (bc < 0)
    {
        bc = 0;
    }
    else if (calcUPGet(unit)->bcLimit < bc)
    {
        bc = calcUPGet(unit)->bcLimit;
    }
    calcUPGet(unit)->bcValue = bc;
    return 1;
}

int calcCounter(BattleUnit *unit, int tecId)
{
    int rate;

    if (dataTecGet(tecId)->flags & 0x800) {
        return 0;
    }
    rate = calcUPGet(unit)->counterRate;
    rate += calcSkill(unit, 0x4000, 0x4000);
    if (rate > 100) {
        rate = 100;
    }
    return rnd(100) < rate;
}

extern BattleUnit *actUnitGet(void);

/*
 * calcBoostChk: whether unit may take a boost in the category mode selects
 * (mode 1 is the boost that a battle unit's own bcValue gates).
 */
int calcBoostChk(BattleUnit *unit, int mode)
{
    if (unit != 0 && !(*(u64 *) unit->work & 0x480) && actUnitGet() != unit
        && ((CalcActStock *) actStockGet())->slot[mode] == 0 && calcUPGet(unit)->currentHp != 0) {
        if ((calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) && calcAccEquChk(unit, 0x83) == 0) {
            return 0;
        }
        if (mode == 1 && calcUPGet(unit)->bcValue <= 0) {
            return 0;
        }
        if (calcStatGet(unit, 1, 4) == 0) {
            goto ok;
        }
    }
    return 0;
ok:
    if (mode == 1) {
        Actor *actor;

        actor = unit->work;
        if (actor->flags & 0x10) {
            return 0;
        }
        return 1;
    }
    return 1;
}

int sndSysSePlay(int soundId);

int calcBoost(BattleUnit *unit, int mode) {
    Actor *work;

    if (mode == 1) {
        calcBc(unit, -1);
    }
    work = unit->work;
    work->flags |= 0x400;
    ((CalcActStock *) actStockGet())->slot[mode] = unit;
    decoEffCall(unit, 0x64);
    return sndSysSePlay(9);
}

int calcIdol2Chk(BattleUnit *unit)
{
    int hp = calcUPGet(unit)->currentHp;

    Actor *actor;

    actor = unit->work;
    if ((actor->flags & ACTOR_FLAG_ENEMY) == 0) {
        if (hp < calcUPGet(unit)->maxHp / 5) {
            return 1;
        }
    }
    return 0;
}

int calcTuigekiChk(BattleUnit *unit)
{
    return calcStatGet(unit, 7, 0x80) != 0;
}

int calcSpecChk(BattleUnit *unit, int specId)
{
    int result;

    if (specId == 0x34 && calcDeadUnitNum(unit, 0x40) <= 0)
    {
        return 0;
    }

    if (specId != 0x5A || (result = calcStatGet(unit, 7, 2)) != 0)
    {
        if (specId != 0x59 || (result = calcStatGet(unit, 7, 1)) != 0)
        {
            result = 1;
        }
    }

    return result;
}

extern int calcTgtUnitTbl(BattleUnit *unit, int trgFlags, BattleUnit **table);
extern int cfEventGet(void);
extern int dataItmBoxChk(int itmId);

int calcDItemChk(BattleUnit *unit, int itmId)
{
    if (calcUPGet(unit)->dblActKind >= 6
        && (dataItmGet(itmId)->trgFlags & CALC_TRG_ALL_PARTY) == 0
        && dataItmBoxChk(itmId) >= 2
        && (dataItmGet(itmId)->useFlags & 1)) {
        return 1;
    }
    return 0;
}

void calcTurnStart(BattleUnit *unit)
{
    int ap = 4;
    int apForSelect;
    Actor *work = unit->work;
    Actor *actor;

    if (calcStatGet(unit, 2, 0x10) != 0 || calcStatGet(unit, 6, 0x1000) != 0)
    {
        ap = 6;
    }

    if (calcStatGet(unit, 0, 0x40) != 0 || calcStatGet(unit, 4, 0x20) != 0)
    {
        ap /= 2;
    }

    apForSelect = ap;
    calcAp(unit, calcStatGet(unit, 7, 0x20) ? apForSelect + 1 : ap);

    actor = unit->work;
    if (actor->flags & 0x800)
    {
        CalcCmdEntry *cmd = unitCmdPtrGet(unit);

        if (cmd->selector == 0)
        {
            CalcCmdEntry newCmd;

            newCmd.selector = 0xD;
            newCmd.argument = 0x1C;
            newCmd.extra = 0;
            unitCmdSet(unit, &newCmd);
        }

        work->flags &= ~0x800;
    }
}

void calcTurnEnd(BattleUnit *unit)
{
    CalcUnitParam *up;
    short wct;
    Actor *actor;

    actor = unit->work;
    actor->flags &= ~0x6418;

    up = calcUPGet(unit);
    calcUPGet(unit)->waitCount = wct = calcWct(unit);
    up->waitCountBase = wct;

    calcUPGet(unit)->turnBias = 0;

    calcStatTurn(unit);
}

int calcWct(BattleUnit *unit)
{
    int base;
    int accum = 0;

    base = (calcUPGet(unit)->turnBias - calcUPGet(unit)->agility) * 9 + 0xB4;

    if (calcStatGet(unit, 0, 0x1000) != 0 || calcStatGet(unit, 4, 0x800) != 0)
    {
        accum = base / 2;
    }

    if (calcStatGet(unit, 2, 0x100) != 0 || calcStatGet(unit, 6, 0x4000) != 0)
    {
        accum -= base / 4;
    }

    if (calcStatGet(unit, 2, 0x80) != 0 || calcStatGet(unit, 6, 0x2000) != 0)
    {
        accum -= base / 2;
    }

    return base + accum;
}

int calcWctDec(BattleUnit *unit)
{
    int wait = calcUPGet(unit)->waitCount;

    if (calcBattleChk(unit) != 0)
    {
        int decremented = wait - ((rnd(100) < 30) ? 1 : 0);

        wait = decremented - 1;
        if (wait < 0)
        {
            wait = 0;
        }
    }

    calcUPGet(unit)->waitCount = wait;
    return wait;
}

extern int dataMtdRead(BattleUnit *unit, int n);

int calcHitMotChk(BattleUnit *unit)
{
    int motion;

    if ((unsigned int) (unitCmdPtrGet(unit)->selector - 1) >= 3) {
        return 0;
    }
    motion = dataMtdRead(unit, 1);
    switch (motion) {
    case 0:
        return 0;
    case 1:
        return motion;
    }
    return 0;
}

extern int calcHit(BattleUnit *attacker, TgtCmd *cmd);
extern int calcDmg(BattleUnit *attacker, TgtCmd *cmd, CalcHitInfo *hit);
extern int plIchigekiGet(void);
extern int cfEventGet(void);
extern const char calcDMGEVAText[]; /* "** DMG: EVA \n" */
extern const char calcPLICHIGEKIText[]; /* "** PL ICHIGEKI\n" */

/*
 * Resolves cmd against the user into hit: the hit class (calcHit) and, on a
 * hit, the damage (calcDmg). A non-enemy user against an enemy target zeroes
 * the damage while cfEventGet carries bit 0. A user that is not an enemy and
 * has PL ICHIGEKI active turns a hit class below 2 into a hit of class 1 whose
 * damage is the target's maxHp. Always returns 1.
 */
int calcHitDmg(BattleUnit *user, TgtCmd *cmd, CalcHitInfo *hit)
{
    int r;
    unsigned char mode;

    hit->setCategory = 8;
    hit->clearCategory = 8;
    hit->setMask = 0;
    hit->clearMask = 0;
    r = calcHit(user, cmd);
    hit->result = r;
    if ((unsigned char) r == 0) {
        hit->damage = 0;
        printf(calcDMGEVAText);
    } else {
        hit->damage = calcDmg(user, cmd, hit);
    }
    if (!(*(unsigned *) user->work & ACTOR_FLAG_ENEMY)) {
        if ((cfEventGet() & 1) && (*(unsigned *) cmd->target->work & ACTOR_FLAG_ENEMY)) {
            hit->damage = 0;
            return 1;
        }
    }
    mode = hit->result;
    if (!(*(unsigned *) user->work & ACTOR_FLAG_ENEMY) && plIchigekiGet() != 0) {
        if (mode < 2) {
            CalcUnitParam *up;

            hit->result = 1;
            up = calcUPGet(cmd->target);
            hit->ichigekiHit = 1;
            hit->damage = up->maxHp;
            printf(calcPLICHIGEKIText);
        }
    }
    return 1;
}

/* calcHitParaGet (defined below): the attacker's hit rate. */
int calcHitParaGet(BattleUnit *unit);

/*
 * The attacker's hit rate (calcHitParaGet) less the target's evadeStat gives
 * margin; the evade is halved when the target carries the statuses tested
 * first, or raised by a quarter when the attacker carries the third one.
 * Early returns: 0 when the target's HP is 0; 1 when its actor carries
 * flag 0x800 (Actor.flags); 0 when the technique carries TEC_FLAG_NO_HIT_ROLL.
 * Then a first roll, rnd(100) - margin, is compared with 90: at 90 or
 * more it returns 2 unless the target carries CALC_UNIT_FLAG_NO_HIT_RESULT_2,
 * the technique carries TEC_FLAG_NO_HIT_RESULT_2, or calcBattleChk of the
 * target is 0. Failing that, a second, independent roll is compared with 85:
 * at 85 or more it returns 1 unless the target carries
 * CALC_UNIT_FLAG_NO_HIT_RESULT_1 or calcBattleChk is 0; otherwise 0.
 */
int calcHitRate(BattleUnit *unit, TgtCmd *cmd)
{
    int hitRate;
    int evade;
    int margin;
    Actor *targetActor;

    hitRate = calcHitParaGet(unit);
    evade = calcUPGet(cmd->target)->evadeStat;
    if (calcStatGet(cmd->target, 0, 0x200) != 0 || calcStatGet(cmd->target, 4, 0x100) != 0) {
        evade /= 2;
    } else if (calcStatGet(unit, 2, 0x20) != 0) {
        evade += evade / 4;
    }

    if (calcUPGet(cmd->target)->currentHp == 0) {
        return 0;
    }
    targetActor = cmd->target->work;
    if (targetActor->flags & 0x800) {
        return 1;
    }
    if (dataTecGet(cmd->tecId)->flags & TEC_FLAG_NO_HIT_ROLL) {
        return 0;
    }

    margin = hitRate - evade;
    if (rnd(100) - margin >= 90) {
        if (!(calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_NO_HIT_RESULT_2)
            && !(dataTecGet(cmd->tecId)->flags & TEC_FLAG_NO_HIT_RESULT_2)
            && calcBattleChk(cmd->target)) {
            return 2;
        }
    }
    if (rnd(100) - margin >= 85
        && !(calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_NO_HIT_RESULT_1)
        && calcBattleChk(cmd->target)) {
        return 1;
    }
    return 0;
}

int calcHitParaGet(BattleUnit *unit) {
    int hitRate;

    hitRate = calcUPGet(unit)->hitStat;
    if (calcStatGet(unit, 0, 0x400) != 0 || calcStatGet(unit, 4, 0x200) != 0) {
        hitRate /= 2;
    } else if (calcStatGet(unit, 2, 0x40) != 0) {
        hitRate += hitRate / 4;
    }
    return hitRate;
}

/*
 * The hit class of a technique against its target, from the technique's
 * effect type (+0x04). Class 0 rolls for a hit unless the target's unit flags
 * carry 0x80 (the hit roll is skipped); a calcHitRate result of 1 adds the
 * 0x200 bit and 2 means a miss. Classes 5 and 6 add the 0x200 bit on a
 * first-roll hit. Unknown classes print and fall back to 1.
 */
extern const char calcJobTypeErrText[]; /* "** jobType err -> %d\n" */

int calcHit(BattleUnit *attacker, TgtCmd *cmd)
{
    TecKindView *tec;
    unsigned short attr;
    int r;
    int hr;

    tec = dataTecGet(cmd->tecId);
    attr = tec->flags;
    switch (tec->effectType) {
    case 0:
        r = 0;
        if (!(calcUPGet(cmd->target)->flags & 0x80)) {
            hr = calcHitRate(attacker, cmd);
            r = (attr & 0x40) ? 3 : 1;
            if (hr == 1) {
                r |= 0x200;
            } else if (hr == 2) {
                r = 0;
            }
        }
        break;
    case 1:
        r = (attr & 0x40) ? 4 : 2;
        break;
    case 2:
        r = 0x802;
        break;
    case 3:
        r = 5;
        break;
    case 4:
        r = 6;
        break;
    case 5:
        r = (attr & 0x40) ? 9 : 7;
        if (calcHitRate(attacker, cmd) == 1) {
            r |= 0x200;
        }
        break;
    case 6:
        r = (attr & 0x40) ? 10 : 8;
        if (calcHitRate(attacker, cmd) == 1) {
            r |= 0x200;
        }
        break;
    default:
        printf(calcJobTypeErrText, cmd->tecId);
        r = 1;
        break;
    }
    return r;
}

/*
 * dataAttGet's return (src/ov01/data_unit_org_get.c, where AttRecord is still
 * opaque), this TU's own bounded view: calcWpnAtkGet's attachment attack term.
 * AttRecord does not name this field, so this is a second definition of the
 * same record; a same-size split of AttRecord's opaque bytes would name it on
 * the owner side instead.
 */
typedef struct CalcAttData {
    short attack;                 /* +0x00 */
    signed char statCategory;     /* +0x02: calcStatAtk's stat category */
    signed char statRate;         /* +0x03: calcStatAtk's rate */
    unsigned short statMask;      /* +0x04: calcStatAtk's stat mask */
} CalcAttData;

extern CalcAttData *dataAttGet(int attachmentId);
extern const char calcWpnAtkGetAttEquipErrText[]; /* "** calcWpnAtkGet: Att equip err! %d\n" */
extern const char calcWpnAtkGetWpnAtkAdjText[]; /* "** calcWpnAtkGet: wpnAtkAdj=%d\n" */

/*
 * calcWpnAtkGet's attack-boost percentage, the word at +0x17C of the unit's
 * CalcUnitParam: a nonzero value gates the whole bonus block and is divided
 * into atk*pct/100. CalcUnitParam (above) ends at 0x17C (effectUnit at +0x158
 * plus statEffUnits[8] reaches exactly +0x17C), so a trailing member would
 * change the struct's size; the term is read through this named offset
 * instead.
 */
#define CALC_PARAM_ATK_BOOST_PCT_OFF 0x17C

int calcWpnAtkGet(BattleUnit *unit, int slot)
{
    Actor *actor;
    short weaponId;
    short attachmentId;
    int atk;

    atk = 0;
    if (slot != -1)
    {
        weaponId = calcUPGet(unit)->weaponId[slot];
        if (weaponId != 0)
        {
            if (calcAttWpnChk(weaponId))
            {
                attachmentId = calcUPGet(unit)->attachmentId[slot];
                if (attachmentId != 0)
                {
                    atk = dataAttGet(attachmentId)->attack;
                }
                else
                {
                    printf(calcWpnAtkGetAttEquipErrText, weaponId);
                }
            }
            else
            {
                atk = dataWpnGet(weaponId)->attack;
            }
        }

        if (*(int *)((char *)calcUPGet(unit) + CALC_PARAM_ATK_BOOST_PCT_OFF) != 0)
        {
            /* An extra calcUPGet(unit) call the original makes and
             * discards, right before reading unit->work below. */
            calcUPGet(unit);
            actor = unit->work;
            if (!(actor->flags & ACTOR_FLAG_ENEMY))
            {
                if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS))
                {
                    /* The 0xBB..0xC2 charaId range that futtobiChk and
                     * calcPlusAglGet exclude the same way. */
                    if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                        calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)
                    {
                        atk = calcUPGet(unit)->baseAtk;
                    }
                }
            }

            atk += atk * (*(int *)((char *)calcUPGet(unit) + CALC_PARAM_ATK_BOOST_PCT_OFF)) / 100;
            printf(calcWpnAtkGetWpnAtkAdjText, atk);
        }
    }
    return atk;
}

int calcPhyDefGet(BattleUnit *unit)
{
    int def;
    int i;
    short accessoryId;
    short weaponId;

    def = 0;
    for (i = 0; i < 3; i++)
    {
        accessoryId = calcUPGet(unit)->accessoryId[i];
        if (accessoryId != 0)
        {
            def += dataAccGet(accessoryId)->phyDefense;
        }
    }

    for (i = 0; i < 3; i++)
    {
        weaponId = calcUPGet(unit)->weaponId[i];
        if (weaponId != 0)
        {
            def += dataWpnGet(weaponId)->phyDefense;
        }
    }

    return def;
}

/* dataAccGet (src/ov01/data_unit_org_get.c), this TU's own
 * private view (see AccessoryData, src/ov01/calc.h). */
extern AccessoryData *dataAccGet(int accessoryId);

int calcMagDefGet(BattleUnit *unit)
{
    int magicDef;
    int i;
    short accessoryId;

    magicDef = 0;
    for (i = 0; i < 3; i++) {
        accessoryId = calcUPGet(unit)->accessoryId[i];
        if (accessoryId != 0) {
            magicDef += dataAccGet(accessoryId)->magicDefense;
        }
    }
    return magicDef;
}

/*
 * dataSpecDataGet's return (src/ov01/data_unit_org_get.c, this TU's own
 * bounded view): calcPowLevGet adds the halfword at +0x04 to the technique's
 * power level; calcPlusAglGet adds the halfword at +0x06 to the technique's
 * agility. dataSpecDataGet's own TU already names this record
 * (src/ov01/data_unit_org_get.h's `PlSpecialSlot`, an 0xC-byte id/unmodeled
 * pair) but not these two fields, and that header includes this one; a
 * same-size split of `PlSpecialSlot.unmodeled_2[10]` would name them on the
 * owner side.
 */
typedef struct CalcSpecSlot {
    short id;                      /* +0x00 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short powLevBonus;              /* +0x04: calcPowLevGet's bonus term */
    short agility;                  /* +0x06: calcPlusAglGet's bonus term */
    unsigned char unmodeled_08[0x0C - 0x08];
} CalcSpecSlot;

extern CalcSpecSlot *dataSpecDataGet(int charaId, int specialId);


/*
 * Returns the power level of tecId for unit: the technique's base, plus the
 * special-character bonus for kind 2 techniques and the PL special term for
 * tecIds 0x262 and 0x263.
 */
int calcPowLevGet(BattleUnit *unit, int tecId)
{
    Actor *actor;
    CalcSpecSlot *spec;
    int powLev;

    powLev = dataTecGet(tecId)->powLevBase;
    if (dataTecGet(tecId)->kind == 2) {
        actor = unit->work;
        if (!(actor->flags & ACTOR_FLAG_ENEMY)) {
            if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                /* The same 0xBB..0xC2 charaId exclusion as calcWpnAtkGet. */
                if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                    calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                    spec = dataSpecDataGet(calcUPGet(unit)->charaId, tecId);
                    if (spec != 0) {
                        powLev += spec->powLevBonus;
                    }
                }
            }
        }
    }
    /* Technique ids 0x262 and 0x263; what they are is not established. */
    if ((unsigned int) (tecId - 0x262) < 2) {
        powLev += dataPlChaGet(calcUPGet(unit)->charaId)->special6PowLevTerm / 5;
    }
    return powLev;
}

int calcPlusAglGet(BattleUnit *unit, int tecId)
{
    Actor *actor;
    CalcSpecSlot *spec;
    int agl;

    agl = dataTecGet(tecId)->agility;
    if (dataTecGet(tecId)->kind == 2) {
        actor = unit->work;
        if (!(actor->flags & ACTOR_FLAG_ENEMY)) {
            if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                    calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                    spec = dataSpecDataGet(calcUPGet(unit)->charaId, tecId);
                    if (spec != 0) {
                        agl += spec->agility;
                    }
                }
            }
        }
    }
    return agl;
}

/* calcDmg's callees defined below in this TU. */
typedef struct CalcStatAtkArg CalcStatAtkArg;
typedef struct CalcStatAtkOut CalcStatAtkOut;
typedef struct CalcCritArg CalcCritArg;
/*
 * calcDmg's damage value once its address has been passed out. A read of the
 * plain variable has alias set 0 after the first `&`, so it would not unify
 * with the stores before it, and the codegen for the last printf changes.
 */
typedef int CalcDamage;
/*
 * calcCrit's damage word, scaled in place when the critical hits. The void *
 * member is never read here; it is the second view of the same word, and
 * without it the access in calcCrit compiles to different code.
 */
typedef union CalcCritDmg {
    int value;
    void *p;
} CalcCritDmg;

extern int calcStat(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit, int *damage);
extern int calcStatAtk(BattleUnit *self, CalcStatAtkArg *arg, CalcStatAtkOut *out, int *val);
extern int calcStatClr(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit, int *dmg);
extern int calcCrit(BattleUnit *self, CalcCritArg *arg, unsigned int *out, CalcCritDmg *dmg);
extern int calcSpec(BattleUnit *self, TgtCmd *arg, int *mode, int *dmg);
extern int calcLinePos(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit, int *damage);
extern int calcBackLineChk(BattleUnit *unit, int selector);
extern int calcStatProtectGet(BattleUnit *unit, int group, int mask);
extern int futtobiChk(BattleUnit *unit, TecKindView *tec);
extern const char calcPhyArmMagArmText[]; /* "** phyArm=%d magArm=%d\n" */
extern const char calcTGTINFOJTYPECTYPEFCText[]; /* "** TGTINFO=%04X JTYPE=%d CTYPE=%d FC=%X\n" */
extern const char calcDMG0HitAtrHitFlagDmgText[]; /* "** DMG0:hitAtr=%d hitFlag=%X dmg=%d\n" */
extern const char calcStatDEATHText[]; /* "** Stat: DEATH\n" */
extern const char calcStatGRAVText[]; /* "** Stat: GRAV\n" */
extern const char calcDMG1HitAtrHitFlagDmgText[]; /* "** DMG1:hitAtr=%d hitFlag=%X dmg=%d\n" */

/*
 * Computes the damage of cmd's technique from attacker to the target, from
 * both units' stats and the hit record, applies the status and critical
 * adjustments, and returns it (clamped to 0..9999).
 */
int calcDmg(BattleUnit *attacker, TgtCmd *cmd, CalcHitInfo *hit)
{
    TecKindView *tec;
    int dmg;
    int phyArm;
    int magArm;
    int atk;
    int def;
    int magAtk;
    int magDef;
    unsigned char mode;
    short maxPoint;
    short curPoint;
    int wpn;
    int a;
    int d;
    int adj;
    int rate;
    int statOk;
    int category;
    int first;

    tec = dataTecGet(cmd->tecId);
    atk = calcUPGet(attacker)->baseAtk;
    def = calcUPGet(cmd->target)->phyDefense;
    magAtk = calcUPGet(attacker)->magAttack;
    magDef = calcUPGet(cmd->target)->magDefense;
    /* Attack and defense values adjusted by the statuses of both units. */
    if (calcStatGet(attacker, 0, 0x100) != 0 || calcStatGet(attacker, 4, 0x1000) != 0) {
        atk -= atk / 4;
    }
    if (calcStatGet(cmd->target, 0, 0x80) != 0 || calcStatGet(cmd->target, 4, 0x400) != 0) {
        def -= def / 4;
    }
    if (calcStatGet(attacker, 1, 0x800) != 0) {
        magAtk -= magAtk / 4;
    }
    if (calcStatGet(attacker, 3, 0x20) != 0) {
        magAtk += magAtk / 4;
        magDef -= magDef / 4;
    } else if (calcStatGet(attacker, 3, 0x10) != 0) {
        magAtk -= magAtk / 4;
        magDef += magDef / 4;
    }
    if (calcStatGet(cmd->target, 7, 0x4000) != 0) {
        magDef += magDef / 4 * calcDeadUnitNum(cmd->target, 0x40);
    }
    if (calcStatGet(cmd->target, 7, 0x2000) != 0) {
        def += def / 4 * calcDeadUnitNum(cmd->target, 0x40);
    }
    if (calcStatGet(attacker, 7, 0x400) != 0) {
        atk = atk * (14 - calcUPGet(attacker)->currentHp / (calcUPGet(attacker)->maxHp / 10)) / 4;
    }
    mode = hit->result;
    {
        int v = hit->setMask;

        hit->setMask = v;
    }
    /*
     * Base damage by effect type: 0 physical or magic hit, 1 magic hit, 2 max HP
     * scaled, 5 and 6 point drain from the target's max or current points.
     */
    switch (tec->effectType) {
    case 0:
        phyArm = calcPhyDefGet(cmd->target);
        magArm = calcMagDefGet(cmd->target);
        printf(calcPhyArmMagArmText, phyArm, magArm);
        if (mode == 3) {
            dmg = calcUPGet(cmd->target)->maxEp * tec->powLevBase / 20;
        } else {
            switch (tec->doubleDamageKind) {
            case 0:
                wpn = calcWpnAtkGet(attacker, cmd->itmId);
                if (tec->flags & 0x80) {
                    a = calcHitParaGet(attacker) + wpn;
                } else {
                    a = atk + wpn;
                }
                if (calcStatGet(attacker, 2, 0x1000) != 0) {
                    a += a / 5;
                } else if (calcStatGet(attacker, 2, 0x400) != 0) {
                    a += a / 4;
                } else if (calcStatGet(attacker, 2, 0x200) != 0) {
                    a -= a / 4;
                }
                d = def + phyArm;
                if (calcStatGet(cmd->target, 2, 0x800) != 0) {
                    d += d / 5;
                } else if (calcStatGet(cmd->target, 2, 0x400) != 0) {
                    d -= d / 4;
                } else if (calcStatGet(cmd->target, 2, 0x200) != 0) {
                    d += d / 4;
                }
                if (calcStatGet(attacker, 3, 0x400) != 0 && calcBackLineChk(attacker, 1) == 0) {
                    a += a / 4;
                }
                if (calcStatGet(cmd->target, 3, 0x400) != 0 && calcBackLineChk(cmd->target, 1) == 0) {
                    d += d / 4;
                }
                d *= 3;
                a *= 4;
                dmg = a * calcPowLevGet(attacker, cmd->tecId) / 20 - d;
                if (dmg < 0) {
                    dmg = 0;
                } else {
                    dmg += rnd(dmg / 10 + 2);
                }
                if (((hit->result >> 8) & 0xFF) == 2) {
                    rate = (calcAtrChg(attacker, tec->attribute) & 2) ? 20 : 50;
                    dmg -= dmg * rate / 100;
                }
                break;
            case 1:
                magAtk *= 5;
                magArm = (magDef + magArm) * 4;
                dmg = magAtk * calcPowLevGet(attacker, cmd->tecId) / 20 - magArm;
                if (dmg < 0) {
                    dmg = 0;
                } else {
                    dmg += rnd(dmg / 10 + 2);
                }
                if (calcStatGet(attacker, 3, 0x40) != 0) {
                    dmg *= 2;
                }
                if (!(tec->flags & 0x1000)) {
                    adj = 0;
                    if (calcStatGet(cmd->target, 3, 8) != 0) {
                        adj = -(dmg / 4);
                    }
                    if (calcStatGet(cmd->target, 3, 4) != 0) {
                        adj += dmg / 2;
                    }
                    dmg += adj;
                    if (calcStatGet(cmd->target, 6, 0x800) != 0) {
                        dmg -= dmg / 4;
                    }
                }
                if (((hit->result >> 8) & 0xFF) == 2) {
                    dmg -= dmg / 5;
                }
                break;
            default:
                dmg = 0;
                break;
            }
        }
        break;
    case 1:
        if (mode == 4) {
            dmg = calcUPGet(cmd->target)->maxEp * tec->powLevBase / 20;
        } else {
            dmg = calcUPGet(attacker)->magAttack * calcPowLevGet(attacker, cmd->tecId);
            if (calcStatGet(cmd->target, 3, 0x80) != 0) {
                dmg *= 2;
                calcStatReset(cmd->target, 3, 0x80, 0);
            }
            if (!(tec->flags & 0x1000)) {
                adj = 0;
                if (calcStatGet(cmd->target, 3, 8) != 0) {
                    adj = -(dmg / 4);
                }
                if (calcStatGet(cmd->target, 3, 4) != 0) {
                    adj += dmg / 4;
                }
                dmg += adj;
                if (calcStatGet(cmd->target, 6, 0x800) != 0) {
                    dmg -= dmg / 4;
                }
            }
            if (tec->flags & 0x10) {
                if (calcUPGet(cmd->target)->flags & 0x1000) {
                    dmg = 0;
                }
            }
        }
        break;
    case 2:
        dmg = calcUPGet(cmd->target)->maxHp * calcPowLevGet(attacker, cmd->tecId) / 10;
        break;
    case 5:
        if (mode == 7) {
            maxPoint = calcUPGet(cmd->target)->maxHp;
            curPoint = calcUPGet(cmd->target)->currentHp;
        } else {
            maxPoint = calcUPGet(cmd->target)->maxEp;
            curPoint = calcUPGet(cmd->target)->currentEp;
        }
        if (mode == 9 && calcStatGet(cmd->target, 3, 2) != 0) {
            dmg = 0;
        } else {
            dmg = maxPoint * tec->powLevBase / 20;
        }
        if (curPoint < dmg) {
            dmg = curPoint;
        }
        if (((hit->result >> 8) & 0xFF) == 2) {
            dmg -= dmg / 2;
        }
        break;
    case 6: {
        if (mode == 8) {
            dmg = calcUPGet(cmd->target)->currentHp - 1;
        } else {
            dmg = calcUPGet(cmd->target)->currentEp - 1;
        }
        if (mode == 10 && calcStatGet(cmd->target, 3, 2) != 0) {
            dmg = 0;
        }
        if (((hit->result >> 8) & 0xFF) == 2) {
            dmg -= dmg / 2;
        }
        break;
    }
    default:
        dmg = 0;
        break;
    }
    if (dmg < 0) {
        dmg = 0;
    }
    printf(calcTGTINFOJTYPECTYPEFCText, tec->attr, tec->effectType, tec->doubleDamageKind, tec->flags);
    printf(calcDMG0HitAtrHitFlagDmgText, (unsigned char) hit->result, (hit->result >> 8) & 0xFF, dmg);
    calcStat(attacker, cmd, hit, &dmg);
    statOk = calcStatAtk(attacker, (CalcStatAtkArg *) cmd, (CalcStatAtkOut *) hit, &dmg);
    calcStatClr(attacker, cmd, hit, &dmg);
    calcCrit(attacker, (CalcCritArg *) cmd, (unsigned int *) hit, (CalcCritDmg *) &dmg);
    calcSpec(attacker, cmd, &hit->result, &dmg);
    calcLinePos(attacker, cmd, hit, &dmg);
    if (statOk == 0 && tec->effectType == 3) {
        hit->result &= 0xFF00;
    }
    /* Category 1 hits with mask 0x100 (death) or 2 (GRAV) set the outcome. */
    category = hit->setCategory;
    if (cmd->tecId == 0x23E && (category != 1 || hit->setMask != 0x100)) {
        hit->setCategory = 1;
        hit->setMask = 2;
        category = 1;
    }
    first = category;
    if (category == 1) {
        if (hit->setMask == 0x100) {
            calcStatSet(cmd->target, 1, 0x100, 0);
            *(CalcDamage *) &dmg = calcUPGet(cmd->target)->currentHp;
            hit->result = category;
            printf(calcStatDEATHText);
            category = hit->setCategory;
        }
        if (category == first && hit->setMask == 2 && calcStatProtectGet(cmd->target, 1, 2) == 0) {
            *(CalcDamage *) &dmg = calcUPGet(cmd->target)->currentHp / 4;
            hit->result = category;
            printf(calcStatGRAVText);
        }
    }
    /* Hit class 1: the target's and attacker's status multipliers, then the ichigeki flag. */
    category = hit->result & 0xFF;
    if (category != 1) {
        hit->ichigekiHit = 0;
    } else {
        if (calcStatGet(cmd->target, 7, 2) != 0) {
            *(CalcDamage *) &dmg /= 2;
        }
        if (calcStatGet(attacker, 7, 1) != 0) {
            *(CalcDamage *) &dmg += *(CalcDamage *) &dmg / 2;
        }
        if ((hit->result & 0xFF) == category) {
            if (futtobiChk(cmd->target, tec) != 0) {
                hit->ichigekiHit = 2;
            } else {
                hit->ichigekiHit = (hit->result >> 8) & 1;
            }
        } else {
            hit->ichigekiHit = 0;
        }
    }
    /* Flag 8 forces 9999, and any damage of 10000 or more is capped at 9999. */
    if (tec->flags & 8) {
        *(CalcDamage *) &dmg = 9999;
    }
    if (*(CalcDamage *) &dmg >= 10000) {
        *(CalcDamage *) &dmg = 9999;
    }
    {
        int value = *(CalcDamage *) &dmg;

        printf(calcDMG1HitAtrHitFlagDmgText, hit->result & 0xFF, (hit->result >> 8) & 0xFF, value);
    }
    return *(CalcDamage *) &dmg;
}

/*
 * futtobiChk: whether a hit with tec knocks unit back ("futtobi") one
 * row. Only a technique carrying TEC_FLAG_KNOCKBACK, an ordinary charaId outside
 * CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN..END on the ACTOR_FLAG_ENEMY-clear
 * side without CALC_UNIT_FLAG_IS_AGWS, standing in a front position
 * (posId < 4) whose row behind (posId + 3) no unit of the table occupies.
 */
int futtobiChk(BattleUnit *unit, TecKindView *tec)
{
    Actor *actor;
    BattleUnit **table;
    int count;
    int posId;
    int i;

    if (!(tec->flags & TEC_FLAG_KNOCKBACK)) {
        return 0;
    }
    actor = unit->work;
    if ((actor->flags & ACTOR_FLAG_ENEMY) || (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
        return 0;
    }
    if (calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN &&
        calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
        return 0;
    }
    posId = calcUPGet(unit)->posId;
    if (posId >= 4) {
        return 0;
    }
    count = unitTblGet(0, &table);
    posId += 3;
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            if (calcUPGet(table[i])->posId == posId) {
                return 0;
            }
        }
    }
    return 1;
}

/*
 * Applies cmd->target's statuses to a hit of mode hit->result & 0xFF and
 * its *damage: a technique with doubleDamageKind 1 doubles the damage
 * and uses up status (0, 0x20); status (0, 0x10) reverses modes 1 and 2 (calcRevHit);
 * (0, 8) halves mode 2, (1, 0x40) mode 4; (7, 0x40) moves mode 1's damage
 * by 20% per two units of its last decimal digit, down for odd damage, up
 * for even; (3, 2) cancels mode 3. A mode-1 hit also clears
 * flag 0x10 from a target calcBattleChk refuses, ends statuses
 * (1, 0x2000) and (5, 0x2000), and ends (5, 0x4000) when rnd(100) < 50.
 */
/* calcRevHit (this TU, defined below): calcStat's own callee. */
extern int calcRevHit(int hitMode);

int calcStat(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit, int *damage)
{
    TecKindView *tec;
    int mode;
    int rate;

    tec = dataTecGet(cmd->tecId);
    mode = hit->result & 0xFF;
    if (tec->doubleDamageKind == 1 && calcStatGet(cmd->target, 0, 0x20)) {
        calcStatReset(cmd->target, 0, 0x20, 0);
        *damage *= 2;
    }
    if ((mode == 1 || mode == 2) && calcStatGet(cmd->target, 0, 0x10)) {
        hit->result = calcRevHit(hit->result);
    }
    if (mode == 2 && calcStatGet(cmd->target, 0, 8)) {
        *damage /= 2;
    }
    if (mode == 4 && calcStatGet(cmd->target, 1, 0x40)) {
        *damage /= 2;
    }
    if (mode == 1 && calcStatGet(cmd->target, 7, 0x40)) {
        rate = *damage % 10 / 2 * 20;
        if (*damage & 1) {
            *damage -= *damage * rate / 100;
        } else {
            *damage += *damage * rate / 100;
        }
    }
    if (mode == 3 && calcStatGet(cmd->target, 3, 2)) {
        *damage = 0;
    }
    if (mode == 1) {
        if (!calcBattleChk(cmd->target)) {
            Actor *actor;

            actor = cmd->target->work;
            actor->flags &= ~0x10;
        }
        calcStatReset(cmd->target, 1, 0x2000, 0);
        calcStatReset(cmd->target, 5, 0x2000, 0);
        if (calcStatGet(cmd->target, 5, 0x4000) && rnd(100) < 50) {
            calcStatReset(cmd->target, 5, 0x4000, 0);
        }
    }
    return 1;
}

extern const char calcStatNoEffectText[]; /* "** Stat: no effect\n" */
extern const char calcStatAtkAttStatText[]; /* "** calcStatAtk: Att stat\n" */
extern const char calcStatAtkAttEquipErrText[]; /* "** calcStatAtk: Att equip err!\n" */
extern const char calcStatAtkWpnStatText[]; /* "** calcStatAtk: Wpn stat\n" */
extern const char calcStatChkNGText[]; /* "** Stat: Chk NG\n" */
extern const char calcStatAtkFormat[]; /* "** Stat: Atk (%d%%) %d %04X\n" */

typedef struct CalcStatAtkArg {
    int tecId;
    BattleUnit *unit;
    int slot;
    int kind;
} CalcStatAtkArg;

/* calcStatAtk's output: the state byte it tests first, and the stat it sets. */
typedef struct CalcStatAtkOut {
    unsigned char state; /* +0x00: 0 or 6 refuse the attack */
    unsigned char unmodeled_01[0x0C - 0x01];
    int category;        /* +0x0C */
    int mask;            /* +0x10 */
} CalcStatAtkOut;

/*
 * Rolls one status effect of the technique against arg->unit. Returns 1 when
 * the status applies and records its category and mask in out; 0 when it is
 * refused or the roll fails.
 */
int calcStatAtk(BattleUnit *self, CalcStatAtkArg *arg, CalcStatAtkOut *out, int *val)
{
    TecKindView *tec;
    TecKindView *wtec;
    CalcAttData *att;
    unsigned short flags;
    int rate;
    int group;
    unsigned short mask;

    tec = dataTecGet(arg->tecId);
    flags = calcUPGet(arg->unit)->flags;
    if (out->state == 0 || out->state == 6) {
        return 0;
    }
    if (((flags & 0x4000) && tec->statFlagKind[0] == 1) || ((flags & 0x2000) && tec->statFlagKind[1] == 1)
        || ((flags & 0x1000) && tec->statFlagKind[2] == 1)) {
        printf(calcStatNoEffectText);
        return 0;
    }
    rate = tec->statRate;
    group = tec->statCategory;
    mask = tec->statMask;
    if (arg->kind == 1 && arg->slot != -1) {
        if ((*(unsigned *) self->work & ACTOR_FLAG_ENEMY) == 0
            && (calcUPGet(self)->flags & CALC_UNIT_FLAG_IS_AGWS) == 0
            && (calcUPGet(self)->charaId < 0xBB || calcUPGet(self)->charaId >= 0xC3)) {
            if (tec->flags & 0x4000) {
                short id = calcUPGet(self)->weaponId[arg->slot];
                if (!(tec->flags & 0x2000)) {
                    att = dataAttGet(calcUPGet(self)->attachmentId[arg->slot]);
                    if (att != 0) {
                        rate = att->statRate;
                        group = att->statCategory;
                        mask = att->statMask;
                        printf(calcStatAtkAttStatText);
                    } else {
                        printf(calcStatAtkAttEquipErrText);
                    }
                } else {
                    wtec = dataTecGet(dataWpnGet(id)->tecId);
                    rate = wtec->statRate;
                    group = wtec->statCategory;
                    mask = wtec->statMask;
                    printf(calcStatAtkWpnStatText);
                }
            }
        } else if (calcAttWpnChk(calcUPGet(self)->weaponId[arg->slot]) != 0) {
            att = dataAttGet(calcUPGet(self)->attachmentId[arg->slot]);
            if (att != 0) {
                rate = att->statRate;
                group = att->statCategory;
                mask = att->statMask;
                printf(calcStatAtkAttStatText);
            } else {
                printf(calcStatAtkAttEquipErrText);
            }
        }
    }
    if (mask == 0) {
        return 0;
    }
    if (calcStatSetChk(arg->unit, group, mask, 0) == 0) {
        printf(calcStatChkNGText);
        return 0;
    }
    if (calcStatGet(arg->unit, 1, 8) != 0) {
        rate *= 2;
        if (rate > 100) {
            rate = 100;
        }
    }
    if (rnd(99) + 1 <= rate) {
        out->category = group;
        out->mask = mask;
        printf(calcStatAtkFormat, rate, group, mask);
        return 1;
    }
    out->mask = 0;
    out->category = 8;
    return 0;
}

/*
 * A status-clearing technique (effectType 4) clears the 0x10 bit from a
 * target calcBattleChk refuses, and records the category and bits it clears
 * in the hit record; category 8 records the full mask (0xFFFF). Any other
 * technique records category 8, no bits, and returns 0.
 */
int calcStatClr(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit, int *dmg)
{
    TecKindView *tec;
    int category;

    tec = dataTecGet(cmd->tecId);
    if (tec->effectType == 4) {
        if (!calcBattleChk(cmd->target)) {
            Actor *actor;

            actor = cmd->target->work;
            actor->flags &= ~0x10;
        }
        category = tec->statCategory;
        if (category == 8) {
            hit->clearCategory = category;
            hit->clearMask = 0xFFFF;
        } else {
            hit->clearCategory = category;
            hit->clearMask = tec->statMask;
        }
        return 1;
    }
    hit->clearMask = 0;
    hit->clearCategory = 8;
    return 0;
}

extern int calcAtrChg(BattleUnit *unit, int attribute);
extern int calcSlotAChk(BattleUnit *unit, int techId);
extern const char calcCritNoEffectText[]; /* "** Crit: no effect\n" */
extern const char calcCritSlotAText[]; /* "** Crit: slot A\n" */
extern const char calcCritEventTimerRText[]; /* "** Crit: EventTimer R\n" */
extern const char calcCritOBJCRITText[]; /* "** Crit: OBJ_CRIT\n" */
extern const char calcCritWeakXText[]; /* "** Crit: weak (%04X) x%f\n" */
extern const char calcCritMonKindText[]; /* "** Crit: monKind %04X\n" */
extern const char calcCritAtkFormat[]; /* "** Crit: Atk (%d%%) %d\n" */

typedef struct CalcCritArg {
    int tecId;
    BattleUnit *unit;
} CalcCritArg;

/*
 * Rolls for a critical hit on arg->unit. The rate rises with weakness, slot
 * and monster-kind conditions; on a hit the damage is scaled by mul/2 and
 * the critical bit 0x100 is set in out. Returns 1 on a hit.
 */
int calcCrit(BattleUnit *self, CalcCritArg *arg, unsigned int *out, CalcCritDmg *dmg)
{
    int rate;
    int mul;
    unsigned short flags;
    TecKindView *tec;
    int weak;

    rate = 10;
    mul = 3;
    flags = calcUPGet(arg->unit)->flags;
    tec = dataTecGet(arg->tecId);
    if ((((int) *out >> 8) & 2) || (*out & 0xFF) != 1 || calcStatGet(arg->unit, 2, 8) != 0) {
        printf(calcCritNoEffectText);
        return 0;
    }
    if (calcStatGet(self, 7, 0x200) != 0 && calcSlotAChk(self, arg->tecId) != 0) {
        rate = 50;
        printf(calcCritSlotAText);
    }
    if (menuTimeGet() == 1) {
        rate = 50;
        printf(calcCritEventTimerRText);
    }
    if (*(unsigned int *) self->work & 0x2000) {
        rate = 80;
        printf(calcCritOBJCRITText);
    }
    weak = calcUPGet(arg->unit)->conditions & calcAtrChg(self, tec->attribute);
    if (weak != 0) {
        rate = 100;

        if (calcUPGet(arg->unit)->conditions & 0x4000) {
            mul = 4;
        }
        if (weak & 0x300) {
            mul++;
        }
        if (weak & 0xE0) {
            mul++;
        }
        if (weak & 0x1F) {
            mul++;
        }
        printf(calcCritWeakXText, weak, (double) ((float) mul * 0.5f));
    }
    if (arg->tecId == 0xF && (flags & 0xC00) == 0x400) {
        rate += 50;
    }
    if (((flags & 0x4000) && tec->statFlagKind[0] == 4) || ((flags & 0x2000) && tec->statFlagKind[1] == 4)
        || ((flags & 0x1000) && tec->statFlagKind[2] == 4)) {
        rate += 50;
        printf(calcCritMonKindText, flags);
    }
    if (calcStatGet(arg->unit, 4, 0x10) != 0) {
        rate += 70;
        calcStatReset(arg->unit, 4, 0x10, 0);
    }
    rate += calcSkill(self, 0x4000, 0x8000);
    if (rate > 100) {
        rate = 100;
    }
    if (rnd(99) + 1 <= rate) {
        dmg->value = dmg->value * mul / 2;
        *(unsigned int *) self->work |= 0x2000;
        *out |= 0x100;
        printf(calcCritAtkFormat, rate, dmg->value);
        return 1;
    }
    return 0;
}

extern const char calcSpecEffectInfoText[]; /* "** Spec: effect(info)\n" */
extern const char calcSpecAtkDefText[]; /* "** Spec: atk=%X def=%X\n" */
extern const char calcSpecEffectABSBText[]; /* "** Spec: effect(ABSB)\n" */
extern const char calcSpecErrSPECABSBText[]; /* "** calcSpec err SPEC_ABSB\n" */
extern const char calcSpecEffectDEFText[]; /* "** Spec: effect(DEF)\n" */
extern const char calcSpecErrSPECDEFText[]; /* "** calcSpec err SPEC_DEF\n" */
extern const char calcSpecSkillALLText[]; /* "** Spec: skill(ALL)\n" */
extern const char calcSpecSkillText[]; /* "** Spec: skill(%X)\n" */

/*
 * Applies the technique's special effects (ABSB and DEF, which change the
 * hit mode and damage) and the target's skill reductions to *dmg. Returns 1
 * when an effect or reduction applied.
 */
int calcSpec(BattleUnit *self, TgtCmd *arg, int *mode, int *dmg)
{
    int kind;
    int sub;
    int ret;
    TecKindView *tec;
    unsigned short flags;
    unsigned short up1a;
    int def;
    int atk;
    int pct;

    ret = 0;
    tec = dataTecGet(arg->tecId);
    flags = calcUPGet(arg->target)->flags;
    up1a = calcUPGet(arg->target)->specDef;
    kind = *mode & 0xFF;
    sub = (*mode >> 8) & 0xFF;
    if (flags & 0x200) {
        *mode = calcRevHit(*mode);
    }
    if (kind != 1 && kind != 3) {
        return 0;
    }
    /* A target with a status of kind 2 takes half the damage. */
    if ((flags & 0x4000) && tec->statFlagKind[0] == 2 || (flags & 0x2000) && tec->statFlagKind[1] == 2
        || (flags & 0x1000) && tec->statFlagKind[2] == 2) {
        printf(calcSpecEffectInfoText);
        *dmg = *dmg / 2;
    }
    def = up1a;
    atk = calcAtrChg(self, tec->attribute) & 0x1F;
    if (calcStatGet(arg->target, 6, 0x1F) != 0) {
        def = def | calcStatGet(arg->target, 6, 0x1F) | 0x2100;
    }
    if (def & atk) {
        printf(calcSpecAtkDefText, atk, def);
        if (def & 0x4000) {
            printf(calcSpecEffectABSBText);
            if (!(def & 0x800)) {
                if (def & 0x400) {
                    *dmg = *dmg / 4;
                } else if (def & 0x200) {
                    *dmg = *dmg / 2;
                } else if (def & 0x100) {
                    *dmg = *dmg / 4 * 3;
                } else {
                    printf(calcSpecErrSPECABSBText);
                }
            }
            sub |= 0x10;
            if (kind == 1) {
                *mode = (*mode & 0xFF00) | 2;
                goto done;
            }
            ret = 1;
            if (kind == 3) {
                *mode = (*mode & 0xFF00) | 4;
            }
        } else if (def & 0x2000) {
            printf(calcSpecEffectDEFText);
            if (def & 0x800) {
                *dmg = 0;
            } else if (def & 0x400) {
                *dmg = *dmg / 4;
            } else if (def & 0x200) {
                *dmg = *dmg / 2;
            } else if (def & 0x100) {
                *dmg = *dmg / 4 * 3;
            } else {
                printf(calcSpecErrSPECDEFText);
            }
            sub |= 0x20;
done:
            ret = 1;
        } else {
            goto done;
        }
    }
    *mode |= sub << 8;
    pct = calcSkill(arg->target, 0x2000, 0x8000);
    if (pct != 0) {
        ret = 1;
        *dmg = *dmg - *dmg * pct / 100;
        printf(calcSpecSkillALLText);
    }
    pct = calcSkill(arg->target, 0x2000, atk << 8);
    if (pct != 0) {
        ret = 1;
        *dmg = *dmg - *dmg * pct / 100;
        printf(calcSpecSkillText, atk);
    }
    if (*dmg < 0) {
        *dmg = 0;
    }
    return ret;
}

/* dataPosLineGet (src/ov01/data_unit_org_get.c, this TU's own private view
 * of its PosTblEntry.line member): the position's battle line index. */
extern short dataPosLineGet(int posId);

extern const char calcLinePosAdjustText[]; /* "** LinePos: adjust\n" */

/*
 * A target on battle line 2 (dataPosLineGet of its posId) loses a fifth of
 * the damage when it is hit (result 1), unless it is an enemy
 * (ACTOR_FLAG_ENEMY), an AGWS unit (CALC_UNIT_FLAG_IS_AGWS) or a charaId in
 * 0xBB..0xC2.
 */
int calcLinePos(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit, int *damage)
{
    Actor *actor;
    int result;
    int hitResult;
    int line;

    result = 0;
    hitResult = hit->result & 0xFF;
    line = dataPosLineGet(calcUPGet(cmd->target)->posId);
    if (hitResult == 1) {
        actor = cmd->target->work;
        if (!(actor->flags & ACTOR_FLAG_ENEMY)) {
            if (!(calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                if (calcUPGet(cmd->target)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                    calcUPGet(cmd->target)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                    if (line == 2) {
                        *damage -= *damage / 5;
                        result = 1;
                        printf(calcLinePosAdjustText);
                    }
                }
            }
        }
    }
    return result;
}

int calcRevHit(int hitMode)
{
    int mode = hitMode & 0xFF;
    int side = (hitMode >> 8) & 0xFF;

    switch (mode)
    {
        case 1:
            mode = 2;
            break;

        case 2:
            mode = 1;
            break;

        case 3:
            mode = 4;
            break;

        case 4:
            mode = 3;
            break;
    }

    return mode | (side << 8);
}

int calcAtrChg(BattleUnit *unit, int attribute) {
    int result;

    result = attribute;
    if (calcStatGet(unit, 3, 0x4000) != 0) {
        result = (result & ~0x1F) | 0x10;
    } else if (calcStatGet(unit, 3, 0x2000) != 0) {
        result = (result & ~0x1F) | 8;
    } else if (calcStatGet(unit, 3, 0x1000) != 0) {
        result = (result & ~0x1F) | 4;
    }
    return result;
}

/* calcSlotAChk (below, this TU): unit and the technique id. */
extern int calcSlotAChk(BattleUnit *unit, int techId);

/* tecId is `int`, not `short`: a short parameter makes the entry block
 * sign-extend it, which the original does not do. */
int calcTakeEpGet(BattleUnit *unit, int tecId)
{
    int ep;

    ep = dataTecGet(tecId)->epCost;
    if (calcStatGet(unit, 1, 0x10)) {
        ep *= 2;
    }
    if (calcStatGet(unit, 3, 0x200)) {
        ep /= 2;
    }
    if (calcStatGet(unit, 3, 0x40)) {
        ep *= 2;
    }
    if (calcStatGet(unit, 7, 0x100) && calcSlotAChk(unit, tecId)) {
        ep = 0;
    }

    return (ep < 100) ? ep : 99;
}

int calcSlotAChk(BattleUnit *unit, int techId)
{
    CalcUnitParam *param;
    int i;

    for (i = 0; i < 2; i++)
    {
        param = calcUPGet(unit);
        if (param->slotTechniqueId[i] == techId)
        {
            return 1;
        }
    }
    return 0;
}

int calcAttWpnChk(int weaponId)
{
    return dataWpnGet(weaponId)->equipped != 0;
}

/*
 * The hit result item cmd->itmId has on cmd->target, by its effect bits;
 * the hit record carries no status to set or clear (category 8, no bits).
 * Effect bit 0's result 2 is reversed (calcRevHit) on a target carrying
 * CALC_UNIT_FLAG_ITEM_REVERSED and again under status (0, 0x10).
 */
int calcHitItem(BattleUnit *unit, TgtCmd *cmd, CalcHitInfo *hit)
{
    ItmTrgView *item;
    int result;

    item = dataItmGet(cmd->itmId);
    hit->setCategory = 8;
    hit->setMask = 0;
    hit->clearCategory = 8;
    hit->clearMask = 0;
    if (item->recoverHp) {
        result = 2;
        if (calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_ITEM_REVERSED) {
            result = calcRevHit(2);
        }
        if (calcStatGet(cmd->target, 0, 0x10)) {
            result = calcRevHit(result);
        }
    } else if (item->recoverEp) {
        result = 4;
    } else if (item->resultMode6) {
        result = 6;
    } else if (item->resultMode5) {
        result = 5;
    } else if (item->resultMode5Alt) {
        result = 5;
    } else if (item->recoverHpRate) {
        result = 0x802;
    } else if (item->hpToOne) {
        result = (item->power == 1) ? 1 : 6;
    } else {
        result = 1;
    }
    return result;
}

/*
 * The effect amount of item cmd->itmId on cmd->target, by its effect bits:
 * a recovery for recoverHp/recoverEp/recoverHpRate, a damage amount for
 * damageHpRate, and negative when CALC_UNIT_FLAG_ITEM_REVERSED on the target
 * flips a recoverHp item.
 */
int calcDmgItem(BattleUnit *unit, TgtCmd *cmd)
{
    ItmTrgView *item;
    int amount;

    amount = 0;
    item = dataItmGet(cmd->itmId);
    if (item->recoverHp) {
        int base = item->power * 25;

        amount = base * 2;
        if (calcStatGet(cmd->target, 3, 0x80)) {
            amount = base * 4;
            calcStatReset(cmd->target, 3, 0x80, 0);
        }
        if (calcStatGet(cmd->target, 0, 8)) {
            amount /= 2;
        }
        if (calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_ITEM_REVERSED) {
            amount = -amount;
        }
        if (calcStatGet(cmd->target, 0, 0x10)) {
            amount = -amount;
        }
        if (item->useFlags & 2) {
            if (calcUPGet(cmd->target)->flags & 0x1000) {
                amount = 0;
            }
        }
    } else if (item->recoverEp) {
        int base = item->power * 5;

        amount = base * 2;
        if (calcStatGet(cmd->target, 3, 0x80)) {
            amount = base * 4;
            calcStatReset(cmd->target, 3, 0x80, 0);
        }
        if (calcStatGet(cmd->target, 1, 0x40)) {
            amount /= 2;
        }
    } else if (item->recoverHpRate) {
        int power = item->power;

        amount = calcUPGet(cmd->target)->maxHp * power / 20;
    } else if (item->damageHpRate) {
        int power = item->power;

        amount = calcUPGet(cmd->target)->maxHp * power / 20;
    } else if (item->hpToOne) {
        if (item->power == 1) {
            amount = calcUPGet(cmd->target)->currentHp - 1;
        }
    }
    return amount;
}


/*
 * calcAtkChk (this TU, below): unit, target and technique id. Its
 * return type is narrower than int: tgtFindChk returns its result through its
 * own epilogue, which a same-width int return would turn into a sibling call.
 * Every narrow width gives the same bytes here.
 */

/*
 * Candidate table of tgtFindFirst: tgtNum entries of tgtTbl are filled in
 * target-search order, and tgtIdx walks them out one at a time through
 * tgtFindNext. The 0x20-byte gap between tgtTbl and tgtIdx is the only
 * evidence of the array's size.
 */
static BattleUnit *tgtTbl[8];
static int tgtIdx;
static int tgtNum;


extern const char calcReTGTIDText[]; /* "** reTGT! (ID=%d) ->" */
extern const char calcTgtIDText[]; /* "** tgt ID=" */
extern const char calcJpText[]; /* "%d " */
extern const char calcJpText2[]; /* "\n" */

static inline int tgtIsEnemy(BattleUnit *u)
{
    Actor *actor;

    actor = u->work;
    return actor->flags & ACTOR_FLAG_ENEMY;
}

static inline int tgtIsDead(BattleUnit *u)
{
    Actor *actor;

    actor = u->work;
    return actor->flags & ACTOR_FLAG_DEAD;
}

/*
 * Starts a target search for cmd from unit and returns the first target; the
 * rest of the candidate table is walked with tgtFindNext. Returns 0 when no
 * target qualifies.
 */
BattleUnit *tgtFindFirst(BattleUnit *unit, TgtCmd *cmd)
{
    BattleUnit *single;
    BattleUnit **candidates;
    int targetFlags;
    BattleUnit *target;
    int allTargets;
    int found;
    int count;
    int i;
    int tblType;

    target = cmd->target;
    tgtNum = 0;
    tgtIdx = 0;
    if (cmd->mode != 3) {
        targetFlags = *(unsigned char *)dataTecGet(cmd->tecId);
    } else {
        targetFlags = ((unsigned char *)dataItmGet(cmd->itmId))[8];
        if (cmd->tecId != 0) {
            targetFlags &= 0x7F;
            targetFlags |= 0x50;
        }
    }
    /* Single target: the given target, when it qualifies for the flags. */
    allTargets = targetFlags & 0x10;
    if (allTargets == 0) {
        found = 0;
        if (tgtFindChk(unit, target, cmd) != 0) {
            if ((targetFlags & CALC_TRG_EXCLUDE_AGWS) && (calcUPGet(target)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
            } else if (targetFlags & 8) {
                if (unit == target) {
                    found = 1;
                }
            } else if (targetFlags & 0x20) {
                if (target == 0 || tgtIsDead(target)) {
                    found = 1;
                }
            } else if (target != 0) {
                found = tgtIsDead(target) == 0;
            }
        }
        if (found == 1) {
            tgtTbl[0] = target;
            tgtNum = found;
            return tgtTbl[tgtIdx++];
        }
        printf(calcReTGTIDText, calcUPGet(target)->charaId);
    }
    /* Otherwise collect the candidates from the side the flags select. */
    count = 1;
    if (targetFlags & 8) {
        single = unit;
        candidates = &single;
    } else {
        tblType = 2;
        if ((targetFlags & 0xC0) != 0xC0) {
            if (targetFlags & 0x80) {
                tblType = tgtIsEnemy(unit) == 0;
            } else if (targetFlags & 0x40) {
                tblType = tgtIsEnemy(unit) != 0;
            } else {
                tblType = tgtIsEnemy(unit) == 0;
            }
        }
        count = unitTblGet(tblType, &candidates);
    }
    for (i = 0; i < count; i++) {
        if (candidates[i] != 0 && tgtFindChk(unit, candidates[i], cmd) != 0) {
            if ((targetFlags & CALC_TRG_EXCLUDE_AGWS) && (calcUPGet(candidates[i])->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                continue;
            }
            if (targetFlags & 0x20) {
                if (candidates[i] != 0 && !tgtIsDead(candidates[i])) {
                    continue;
                }
            } else {
                if (candidates[i] == 0 || tgtIsDead(candidates[i])) {
                    continue;
                }
            }
            tgtTbl[tgtNum++] = candidates[i];
        }
    }
    if (tgtNum == 0) {
        return 0;
    }
    if (allTargets == 0) {
        tgtNum = 1;
    }
    printf(calcTgtIDText);
    for (i = 0; i < tgtNum; i++) {
        printf(calcJpText, calcUPGet(tgtTbl[i])->charaId);
    }
    printf(calcJpText2);
    return tgtTbl[tgtIdx++];
}

int tgtFindChk(BattleUnit *unit, BattleUnit *target, TgtCmd *cmd)
{
    if (cmd->mode != 3) {
        return calcAtkChk(unit, target, cmd->tecId);
    }
    return 1;
}

BattleUnit *tgtFindNext(void)
{
    if (tgtIdx >= tgtNum) {
        return 0;
    }
    return tgtTbl[tgtIdx++];
}

/*
 * moveTbl (ov01/battle_init data, 0x78 bytes): five reachable positions per
 * position, -1 for an empty entry. calcMoveChk indexes it by posId - 1, so
 * its six rows are posIds 1..6.
 */
extern int moveTbl[6][5];

/*
 * Whether unit may move to toPosId: enemies and AGWS units always may, a
 * player character only to a position its own row of moveTbl lists.
 */
int calcMoveChk(BattleUnit *unit, int toPosId)
{
    int posId;
    int i;
    Actor *actor;

    posId = calcUPGet(unit)->posId;
    actor = unit->work;
    if (!(actor->flags & ACTOR_FLAG_ENEMY)) {
        if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
            for (i = 0; i < 5; i++) {
                if (moveTbl[posId - 1][i] != -1 && moveTbl[posId - 1][i] == toPosId) {
                    return 1;
                }
            }
            return 0;
        }
    }
    return 1;
}

/* TecKindView.attr's range class (calcAtkChk below). */
enum { TEC_ATTR_RANGE = 0x03 };

/* calcLineChk and calcA2Chk (both this TU, defined below): calcAtkChk's
 * per-unit battle-line tests. */
extern int calcLineChk(BattleUnit *unit);
extern int calcA2Chk(BattleUnit *unit);

short calcAtkChk(BattleUnit *unit, BattleUnit *target, int tecId)
{
    Actor *actor;
    int line;
    int range;

    line = dataPosLineGet(calcUPGet(unit)->posId);
    range = dataTecGet(tecId)->attr & TEC_ATTR_RANGE;
    if (range == 0) {
        if (calcUPGet(target)->flags & CALC_UNIT_FLAG_NO_RANGE0_ATTACK) {
            return 0;
        }
    }
    if (calcStatGet(target, 1, 0x200)) {
        return 0;
    }
    actor = unit->work;
    if (!(actor->flags & ACTOR_FLAG_ENEMY)) {
        if (line != 0 && line != 1 && range != 2) {
            return 0;
        }
    } else if (range != 2) {
        if (!calcLineChk(unit)) {
            return 0;
        }
        if (!calcA2Chk(target)) {
            return 0;
        }
    }
    if (!calcLineChk(target)) {
        return range == 2;
    }
    return 1;
}

/*
 * A unit standing at posId 8 passes only when no live unit of its own side
 * stands at a posId in 1..6.
 */
int calcA2Chk(BattleUnit *unit)
{
    BattleUnit **table;
    short posId;
    int side;
    int count;
    int i;
    Actor *actor;

    if (calcUPGet(unit)->posId != 8) {
        return 1;
    }
    actor = unit->work;
    if (actor->flags & ACTOR_FLAG_ENEMY) {
        side = 1;
    } else {
        side = 0;
    }
    i = 0;
    count = unitTblGet(side, &table);
    for (; i < count; i++) {
        if (table[i] != 0 && !(table[i]->work->flags & ACTOR_FLAG_DEAD)) {
            posId = calcUPGet(table[i])->posId;
            if (posId >= 1 && posId <= 6) {
                return 0;
            }
        }
    }
    return 1;
}

/*
 * Returns 0 when a live unit on the same side stands on a lower battle line
 * number than unit; 1 otherwise, including when unit has no line.
 */
int calcLineChk(BattleUnit *unit)
{
    BattleUnit **table;
    short myLine;
    short otherLine;
    int side;
    int count;
    int i;
    Actor *actor;

    myLine = dataPosLineGet(calcUPGet(unit)->posId);
    if (myLine == 0)
    {
        return 1;
    }

    actor = unit->work;
    if (actor->flags & ACTOR_FLAG_ENEMY)
    {
        side = 1;
    }
    else
    {
        side = 0;
    }
    i = 0;
    count = unitTblGet(side, &table);
    if (count > 0)
    {
        for (; i < count; i++)
        {
            if (table[i] != 0)
            {
                Actor *tableActor;

                tableActor = table[i]->work;
                if ((tableActor->flags & ACTOR_FLAG_DEAD) == 0)
                {
                    otherLine = dataPosLineGet(calcUPGet(table[i])->posId);
                    if (otherLine != 0 && otherLine < myLine)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/*
 * calcBackLineChk's extra-blocker selector (calcDmg passes 1, stat2Proc
 * passes 0): selector 0 tests CALC_UNIT_FLAG_BLOCKS_BACK_LINE of a unit standing
 * further back; selector 1 tests that unit's charaId against 6 (which
 * character 6 is, is not established).
 */
int calcBackLineChk(BattleUnit *unit, int selector)
{
    BattleUnit **table;
    short myLine;
    short otherLine;
    int side;
    int count;
    int i;
    Actor *actor;

    myLine = dataPosLineGet(calcUPGet(unit)->posId);
    if (myLine == 0)
    {
        return 1;
    }

    actor = unit->work;
    if (actor->flags & ACTOR_FLAG_ENEMY)
    {
        side = 1;
    }
    else
    {
        side = 0;
    }
    i = 0;
    count = unitTblGet(side, &table);
    if (count > 0)
    {
        for (; i < count; i++)
        {
            if (table[i] != 0)
            {
                Actor *tableActor;

                tableActor = table[i]->work;
                if ((tableActor->flags & ACTOR_FLAG_DEAD) == 0)
                {
                    otherLine = dataPosLineGet(calcUPGet(table[i])->posId);
                    if (otherLine != 0 && myLine < otherLine)
                    {
                        switch (selector)
                        {
                        case 0:
                            if (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_BLOCKS_BACK_LINE)
                            {
                                return 0;
                            }
                            break;
                        case 1:
                            if (calcUPGet(table[i])->charaId == 6)
                            {
                                return 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

int calcWpnEnableChk(BattleUnit *unit, int slot)
{
    WpnData *wpn;
    CalcUnitParam *up;
    int weaponId;

    up = calcUPGet(unit);
    weaponId = up->weaponId[slot];
    if (weaponId == 0) {
        return 0;
    }
    wpn = dataWpnGet(weaponId);
    /* The original calls dataTecGet(wpn->tecId) here and uses nothing of
     * its result; the call is kept so the bytes match. */
    dataTecGet(wpn->tecId);
    if (calcAttWpnChk(weaponId) != 0) {
        if (up->attachmentId[slot] == 0) {
            return 0;
        }
        if (up->wpnSlotValue[slot] <= 0) {
            return 0;
        }
    }
    if (wpn->apCost > up->dblActKind) {
        return 0;
    }
    if (calcTakeEpGet(unit, wpn->tecId) > up->currentEp) {
        return 0;
    }
    return 1;
}

int calcWpnUse(BattleUnit *unit, int slot)
{
    CalcUnitParam *up;

    if (slot != -1) {
        if (slot >= 4) {
            return 1;
        }
        up = calcUPGet(unit);
        if (calcAttWpnChk(up->weaponId[slot]) != 0) {
            if (up->wpnSlotValue[slot] != 0xFF) {
                up->wpnSlotValue[slot]--;
            }
            if (up->wpnSlotValue[slot] < 0) {
                up->wpnSlotValue[slot] = 0;
            }
        }
    }
    return 1;
}

int calcWpnEquipOrg(int charaId, int slot, int weaponId, int hand) {
    CalcCharParaData *origin;

    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    origin = dataUnitOrgGet(charaId);
    origin->weaponId[slot] = (short) weaponId;
    origin->hand[slot] = (signed char) hand;
    return 1;
}

int calcAccEquipOrg(int charaId, int slot, int accessoryId) {
    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->accessoryId[slot] = (short) accessoryId;
    return 1;
}

int calcAttEquipOrg(int charaId, int slot, int attachmentId) {
    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->attachmentId[slot] = (short) attachmentId;
    return 1;
}

int calcSpecEquipOrg(int charaId, int slot, int specialId) {
    if (slot >= 6 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->specialId[slot] = (short) specialId;
    return 1;
}

int calcEtherEquipOrg(int charaId, int slot, int etherId) {
    if (slot >= 0xC || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->etherId[slot] = (short) etherId;
    return 1;
}

int calcSkillEquipOrg(int charaId, int slot, int skillId) {
    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->skillId[slot] = (short) skillId;
    return 1;
}

int calcAgwsEquipOrg(int charaId, int agwsId) {
    if ((unsigned int) (agwsId - 0x11) >= 0x10 || charaId >= 0x11) {
        return 0;
    }
    dataUnitOrgGet(charaId)->agwsId = (short) agwsId;
    return 1;
}

int calcEngineEquipOrg(int charaId, int engineId)
{
    CalcCharParaData *origin = dataUnitOrgGet(charaId);

    if ((origin->flags & 0x40) == 0) {
        return 0;
    }
    origin->engineId = (short) engineId;
    return 1;
}

int calcFrameEquipOrg(int charaId, int frameId)
{
    CalcCharParaData *origin = dataUnitOrgGet(charaId);

    if ((origin->flags & 0x40) == 0) {
        return 0;
    }
    origin->frameId = (short) frameId;
    return 1;
}

/* The CalcUnitParam.dblActKind value calcDblActChk requires; nothing else in
 * this TU names it. */
#define CALC_DBL_ACT_KIND 6

/* calcSameWpnChk (this TU, defined below): a weapon slot of unit, or -1. */
extern int calcSameWpnChk(BattleUnit *unit);

int calcDblActChk(BattleUnit *unit)
{
    CalcUnitParam *up;
    int slot;

    up = calcUPGet(unit);
    slot = calcSameWpnChk(unit);
    if (slot != -1 && up->dblActKind == CALC_DBL_ACT_KIND
        && dataWpnGet(up->weaponId[slot])->dblAct != 0) {
        if (calcAttWpnChk(up->weaponId[slot]) == 0 || up->wpnSlotValue[slot] > 0) {
            return 1;
        }
        return 0;
    }
    return 0;
}

/*
 * The first weapon slot (0 or 1) whose weapon a later slot also holds, or
 * -1. An empty slot (weapon id 0) never counts as a match, so two empty
 * slots give -1. Weapon ids 0x5B and 0x70 count as the same weapon, as do
 * 0x5C and 0x71.
 */
int calcSameWpnChk(BattleUnit *unit)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);

    if (up->weaponId[0] != 0) {
        if (up->weaponId[0] == up->weaponId[1]) {
            return 0;
        }
        if (up->weaponId[0] == up->weaponId[2]) {
            return 0;
        }
        if (up->weaponId[0] == 0x5B || up->weaponId[0] == 0x70) {
            if (up->weaponId[1] == 0x5B || up->weaponId[1] == 0x70) {
                return 0;
            }
        }
        if (up->weaponId[0] == 0x5B || up->weaponId[0] == 0x70) {
            if (up->weaponId[2] == 0x5B || up->weaponId[2] == 0x70) {
                return 0;
            }
        }
        if (up->weaponId[0] == 0x5C || up->weaponId[0] == 0x71) {
            if (up->weaponId[1] == 0x5C || up->weaponId[1] == 0x71) {
                return 0;
            }
        }
        if (up->weaponId[0] == 0x5C || up->weaponId[0] == 0x71) {
            if (up->weaponId[2] == 0x5C || up->weaponId[2] == 0x71) {
                return 0;
            }
        }
    }
    if (up->weaponId[1] != 0) {
        if (up->weaponId[1] == up->weaponId[2]) {
            return 1;
        }
        if (up->weaponId[1] == 0x5B || up->weaponId[1] == 0x70) {
            if (up->weaponId[2] == 0x5B || up->weaponId[2] == 0x70) {
                return 1;
            }
        }
        if (up->weaponId[1] == 0x5C || up->weaponId[1] == 0x71) {
            if (up->weaponId[2] == 0x5C || up->weaponId[2] == 0x71) {
                return 1;
            }
        }
    }
    return -1;
}

/* calcEtherDisableGet (this TU, defined below): calcEthEnableChk's own
 * callee, the unit's disable bit for ether slot `bit`. */
extern int calcEtherDisableGet(BattleUnit *unit, int bit);

/*
 * Whether unit can use ether etherId now: its technique must find a
 * target, the unit must have the EP calcTakeEpGet asks, ether 3 is refused
 * while cfEventGet() carries bit 8, status (7, 2) refuses ether 0x2B and
 * its absence 0x2C/0x2D, status (7, 1) refuses 0x2E and its absence
 * 0x2F/0x30, and an equipped slot holding etherId must not be disabled.
 */
int calcEthEnableChk(BattleUnit *unit, int etherId)
{
    int i;

    if (!calcTgtUnitTbl(unit, dataTecGet(dataEthGet(etherId)->tecId)->attr, 0)) {
        return 0;
    }
    if (calcUPGet(unit)->currentEp < calcTakeEpGet(unit, dataEthGet(etherId)->tecId)) {
        return 0;
    }
    if (etherId == 3 && (cfEventGet() & 8)) {
        return 0;
    }
    if ((calcStatGet(unit, 7, 2) && etherId == 0x2B)
        || (!calcStatGet(unit, 7, 2) && (etherId == 0x2C || etherId == 0x2D))
        || (calcStatGet(unit, 7, 1) && etherId == 0x2E)
        || (!calcStatGet(unit, 7, 1) && (etherId == 0x2F || etherId == 0x30))) {
        return 0;
    }
    for (i = 0; i < 12; i++) {
        if (calcUPGet(unit)->etherId[i] == etherId && calcEtherDisableGet(unit, i)) {
            return 0;
        }
    }
    return 1;
}

int calcItmEnableChk(BattleUnit *unit, int itmId)
{
    if (calcTgtUnitTbl(unit, dataItmGet(itmId)->trgFlags, 0) == 0) {
        return 0;
    }
    if (itmId == 0x10 && (cfEventGet() & 8) != 0) {
        return 0;
    }
    return 1;
}

int calcTgtUnitTbl(BattleUnit *unit, int trgFlags, BattleUnit **table)
{
    BattleUnit **srcTable;
    int side;
    int total;
    int count;
    int i;

    count = 0;
    if (trgFlags & CALC_TRG_SELF)
    {
        if (!((trgFlags & CALC_TRG_EXCLUDE_AGWS) && (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)))
        {
            if (table != 0)
            {
                table[0] = unit;
            }
            count = 1;
        }
    }
    else
    {
        i = 0;
        side = calcTrg2PE(unit, trgFlags);
        total = unitTblGet(side, &srcTable);
        if (total > 0)
        {
            for (; i < total; i++)
            {
                if (srcTable[i] == 0)
                {
                    continue;
                }
                if (!(trgFlags & CALC_TRG_DEAD_UNITS))
                {
                    if (srcTable[i]->work->flags & ACTOR_FLAG_DEAD)
                    {
                        continue;
                    }
                }
                else
                {
                    if (!(srcTable[i]->work->flags & ACTOR_FLAG_DEAD))
                    {
                        continue;
                    }
                }
                if (trgFlags & CALC_TRG_EXCLUDE_AGWS)
                {
                    if (calcUPGet(srcTable[i])->flags & CALC_UNIT_FLAG_IS_AGWS)
                    {
                        continue;
                    }
                }
                if (table != 0)
                {
                    table[count] = srcTable[i];
                }
                count++;
            }
        }
    }
    return count;
}

int calcDeadUnitNum(BattleUnit *unit, int flag)
{
    BattleUnit **table;
    int count;
    int i;
    int result;

    count = unitTblGet(calcTrg2PE(unit, flag), &table);
    result = 0;
    if (count > 0)
    {
        for (i = 0; i < count; i++)
        {
            if (table[i] != 0)
            {
                Actor *tableActor;

                tableActor = table[i]->work;
                if ((tableActor->flags & ACTOR_FLAG_DEAD) != 0)
                {
                    result++;
                }
            }
        }
    }
    return result;
}

/*
 * 0 for a missing or dead (ACTOR_FLAG_DEAD) unit or one carrying any
 * of the five statuses tested; otherwise 1.
 */
int calcBattleChk(BattleUnit *unit)
{
    if (unit == 0
        || (unit->work->flags & ACTOR_FLAG_DEAD)
        || calcStatGet(unit, 0, 0x2000)
        || calcStatGet(unit, 1, 0x2000)
        || calcStatGet(unit, 4, 0x2000)
        || calcStatGet(unit, 5, 0x4000)
        || calcStatGet(unit, 5, 0x2000)) {
        return 0;
    }
    return 1;
}

/*
 * Answers a target-selection flag word (cmdAtktbl, calcTgtUnitTbl and
 * calcDeadUnitNum supply it) against the unit's own side bit, bit 0x40 of
 * Actor's flags: a selection of 0x40, and any word carrying 0x08,
 * asks whether the unit is on that side; a selection of 0x80 asks whether it
 * is not; every other selection answers 2.
 */
int calcTrg2PE(BattleUnit *unit, int trgFlags)
{
    if ((trgFlags & 0xC0) == 0x40 || (trgFlags & 8) != 0) {
        Actor *actor;

        actor = unit->work;
        if ((actor->flags & ACTOR_FLAG_ENEMY) != 0) {
            return 1;
        }
        return 0;
    }
    if ((trgFlags & 0xC0) == 0x80) {
        Actor *actor;

        actor = unit->work;
        if ((actor->flags & ACTOR_FLAG_ENEMY) == 0) {
            return 1;
        }
        return 0;
    }
    return 2;
}

/* calcStatIdx2Bit: calcStatTurn's own callee, defined later in this TU
 * (calcStatReset, its other callee, is declared above calcStat and also
 * defined below). */
extern int calcStatIdx2Bit(int statIdx);

void calcStatTurn(BattleUnit *unit)
{
    CalcUnitParam *param;
    int category;
    int statIdx;
    unsigned char remaining;

    param = calcUPGet(unit);
    for (category = 0; category < 8; category++) {
        for (statIdx = 0; statIdx < 16; statIdx++) {
            if (param->statActiveMask[category] & calcStatIdx2Bit(statIdx)) {
                remaining = param->statTurnCount[category][statIdx];
                if (remaining != 0xFF) {
                    remaining--;
                    if (remaining == 0) {
                        calcStatReset(unit, category, calcStatIdx2Bit(statIdx), 0);
                    }
                    param->statTurnCount[category][statIdx] = remaining;
                }
            }
        }
    }
}

int calcStatProtectGet(BattleUnit *unit, int group, int mask)
{
    return calcUPGet(unit)->statProtect[group] & mask;
}

void calcStatProtectSet(BattleUnit *unit, int group, int mask)
{
    calcUPGet(unit)->statProtect[group] |= mask;
}

/*
 * calcStatGetNorm's lookup, widened: outside the two party-wide cases the
 * answer also includes statAltMask[category] & flag of the same unit.
 */
int calcStatGet(BattleUnit *unit, int category, int flag)
{
    BattleUnit **table;
    int count;
    int result;
    int i;

    if ((category == 3 && (flag & 1)) || (category == 6 && (flag & 0x800))) {
        count = unitTblGet(0, &table);
        result = 0;
        for (i = 0; i < count; i++) {
            if (table[i] != 0) {
                result = calcUPGet(table[i])->statActiveMask[category] & flag;
                if (result != 0) {
                    break;
                }
            }
        }
    } else {
        result = calcUPGet(unit)->statActiveMask[category] & flag;
        result |= calcUPGet(unit)->statAltMask[category] & flag;
    }
    return result;
}

/*
 * calcStatGet's plain lookup: statActiveMask[category] & flag of unit. When
 * category is 3 and flag has bit 0x1, or category is 6 and flag has bit 0x800,
 * the answer is instead the same statActiveMask[category] & flag of the first
 * unit of unitTblGet(0)'s table for which it is nonzero, or 0 when no unit's
 * is.
 */
int calcStatGetNorm(BattleUnit *unit, int category, int flag)
{
    BattleUnit **table;
    int count;
    int result;
    int i;

    if ((category == 3 && (flag & 1)) || (category == 6 && (flag & 0x800))) {
        count = unitTblGet(0, &table);
        result = 0;
        for (i = 0; i < count; i++) {
            if (table[i] != 0) {
                result = calcUPGet(table[i])->statActiveMask[category] & flag;
                if (result != 0) {
                    break;
                }
            }
        }
    } else {
        result = calcUPGet(unit)->statActiveMask[category] & flag;
    }
    return result;
}

/*
 * Whether status category may be set on unit: with CalcUnitParam flag 0x1000
 * (not named: its role is not established) categories 0..3 are refused and
 * category 5 too unless flag 0x8 is set; without it categories 4..6 are
 * refused. Past that, force accepts outright; otherwise calcStatProtectGet's
 * protection and status 2/0x2000 (which still admits category 7) refuse.
 */
int calcStatSetChk(BattleUnit *unit, int category, int flag, int force)
{
    u16 flags;

    flags = calcUPGet(unit)->flags;
    if (flags & 0x1000) {
        if (category >= 0 && category <= 3) {
            return 0;
        }
        if (!(flags & 0x8) && category == 5) {
            return 0;
        }
    } else if (category >= 4 && category <= 6) {
        return 0;
    }
    if (force != 0) {
        return 1;
    }
    if (calcStatProtectGet(unit, category, flag) != 0) {
        return 0;
    }
    if (calcStatGet(unit, 2, 0x2000) != 0 && category != 7) {
        return 0;
    }
    return 1;
}

extern int calcStatDefTurnGet(BattleUnit *unit, int category, int statBit);

int calcStatSet(BattleUnit *unit, int category, int mask, int force)
{
    CalcUnitParam *up;
    int statIdx;
    int defTurn;

    up = calcUPGet(unit);
    if (calcStatSetChk(unit, category, mask, force) == 0) {
        return 0;
    }
    if (force == 0) {
        up->statActiveMask[category] |= mask;
        statIdx = calcStatBit2Idx(mask);
        defTurn = calcStatDefTurnGet(unit, category, mask);
        if (defTurn != 0xFF && calcStatGet(unit, 3, 0x100) != 0) {
            defTurn *= 2;
            calcStatReset(unit, 3, 0x100, 0);
        }
        up->statTurnCount[category][statIdx] = defTurn;
    } else {
        if (category == 2 && (mask & 0x100)) {
            if (calcStatGet(unit, 2, 0x100) != 0) {
                mask = (mask & ~0x100) | 0x80;
            }
        }
        up->statAltMask[category] |= mask;
    }
    switch (category) {
    case 0:
        if (mask & 0x1000) {
            calcStatReset(unit, 2, 0x100, force);
            calcStatReset(unit, 2, 0x80, force);
        }
        if (mask & 0x400) {
            calcStatReset(unit, 2, 0x40, force);
        }
        if (mask & 0x200) {
            calcStatReset(unit, 2, 0x20, force);
        }
        if (mask & 0x40) {
            calcStatReset(unit, 2, 0x10, force);
        }
        break;
    case 2:
        if (mask & 0x100) {
            calcStatReset(unit, 0, 0x1000, force);
            calcStatReset(unit, 2, 0x80, force);
        }
        if (mask & 0x80) {
            calcStatReset(unit, 0, 0x1000, force);
            calcStatReset(unit, 2, 0x100, force);
        }
        if (mask & 0x40) {
            calcStatReset(unit, 0, 0x400, force);
        }
        if (mask & 0x20) {
            calcStatReset(unit, 0, 0x200, force);
        }
        if (mask & 0x1000) {
            calcStatReset(unit, 2, 0x400, force);
        }
        if (mask & 0x800) {
            calcStatReset(unit, 2, 0x200, force);
        }
        if (mask & 0x400) {
            calcStatReset(unit, 2, 0x1000, force);
            calcStatReset(unit, 2, 0x200, force);
        }
        if (mask & 0x200) {
            calcStatReset(unit, 2, 0x800, force);
            calcStatReset(unit, 2, 0x400, force);
        }
        if (mask & 0x10) {
            calcStatReset(unit, 0, 0x40, force);
        }
        break;
    case 1:
        if (mask & 0x10) {
            calcStatReset(unit, 3, 0x200, force);
        }
        break;
    case 3:
        if (mask & 0x4000) {
            calcStatReset(unit, 3, 0x2000, force);
            calcStatReset(unit, 3, 0x1000, force);
        }
        if (mask & 0x2000) {
            calcStatReset(unit, 3, 0x4000, force);
            calcStatReset(unit, 3, 0x1000, force);
        }
        if (mask & 0x1000) {
            calcStatReset(unit, 3, 0x4000, force);
            calcStatReset(unit, 3, 0x2000, force);
        }
        if (mask & 0x200) {
            calcStatReset(unit, 1, 0x10, force);
        }
        if (mask & 0x20) {
            calcStatReset(unit, 3, 0x10, force);
        }
        if (mask & 0x10) {
            calcStatReset(unit, 3, 0x20, force);
        }
        if (mask & 0x8) {
            calcStatReset(unit, 3, 0x4, force);
        }
        if (mask & 0x4) {
            calcStatReset(unit, 3, 0x8, force);
        }
        break;
    case 7:
        if (mask & 0x2) {
            calcStatReset(unit, 7, 0x1, force);
        }
        if (mask & 0x1) {
            calcStatReset(unit, 7, 0x2, force);
        }
        break;
    }
    return 1;
}

/* calcStatBit2Idx (this TU, defined below): shared with calcStatTurn. */
extern int calcStatBit2Idx(int statBit);

void calcStatReset(BattleUnit *unit, int category, int mask, int useAltMask)
{
    CalcUnitParam *up;
    int idx;

    up = calcUPGet(unit);
    if (useAltMask == 0) {
        up->statActiveMask[category] &= ~mask;
        idx = calcStatBit2Idx(mask);
        up->statTurnCount[category][idx] = 0;
    } else {
        up->statAltMask[category] &= ~mask;
    }
}

/* turn_1: an 8x16-byte table of
 * per-category/per-status-index wait-count-end turn bonuses. */
static signed char turn_1[8][16];

/*
 * Base bonus from turn_1[category][statIdx], returned as is unless category
 * is 7 (agility, per calcWct) and statBit has one of its two low bits set.
 * Then one more turn when the unit's current HP is strictly below half of its
 * maxHp (signed halfword, division rounded toward zero) and one more again
 * when it is strictly below a quarter of maxHp: at most two extra turns. Each
 * comparison re-reads calcUPGet(unit) rather than caching it.
 */
int calcStatDefTurnGet(BattleUnit *unit, int category, int statBit)
{
    int idx;
    int bonus;
    CalcUnitParam *up;

    idx = calcStatBit2Idx(statBit);
    bonus = turn_1[category][idx];
    if (category == 7 && (statBit & 3) != 0) {
        up = calcUPGet(unit);
        if (up->currentHp < (short) calcUPGet(unit)->maxHp / 2) {
            bonus++;
        }
        up = calcUPGet(unit);
        if (up->currentHp < (short) calcUPGet(unit)->maxHp / 4) {
            bonus++;
        }
    }
    return bonus;
}

/*
 * The inverse of calcStatIdx2Bit below: the index of statBit's lowest set bit
 * among the 16 status bits calcStatSet, calcStatReset and calcStatDefTurnGet
 * index with it, or 16 when statBit holds none of them.
 */
int calcStatBit2Idx(int statBit)
{
    int statIdx;
    int mask;

    mask = 1;
    for (statIdx = 0; statIdx < 16; statIdx++) {
        if (statBit & mask) {
            break;
        }
        mask <<= 1;
    }
    return statIdx;
}

int calcStatIdx2Bit(int statIdx)
{
    return 1 << statIdx;
}

void calcStatResetCha(BattleUnit *unit) {
    calcStatReset(unit, 0, 0x7FFF, 0);
    calcStatReset(unit, 1, 0x7FFF, 0);
    calcStatReset(unit, 2, 0x7FFF, 0);
    calcStatReset(unit, 3, 0x7FFF, 0);
}

void calcStatResetAll(BattleUnit *unit) {
    calcStatReset(unit, 0, 0x7FFF, 0);
    calcStatReset(unit, 1, 0x7FFF, 0);
    calcStatReset(unit, 2, 0x7FFF, 0);
    calcStatReset(unit, 3, 0x7FFF, 0);
    calcStatReset(unit, 4, 0x7FFF, 0);
    calcStatReset(unit, 5, 0x7FFF, 0);
    calcStatReset(unit, 6, 0x7FFF, 0);
}

/* One battle's four rewards, in calcResultSub's order (+0x0..+0xC of its
 * second argument, +0x4..+0x10 of its fourth). */
typedef struct CalcReward {
    int exp;
    int techPoint;
    int etherPoint;
    int skillPoint;
} CalcReward;

/*
 * The per-character result record calcResult fills and hands to
 * calcResultSub, as far as calcResultSub writes it.
 */
typedef struct CalcResultEntry {
    unsigned char unmodeled_00[0x04];
    CalcReward gained; /* +0x04: the rewards calcResultSub writes */
} CalcResultEntry;

extern const char calcLvUPIDText[]; /* "Lv UP! (ID=%d) %d\n" */
extern const char calcAltLvUPIDText[]; /* "Alt Lv UP! (ID=%d) %d\n" */

/* The battle's gain record monsGainPtrGet returns: the rewards, money and
 * the box items (grp/item pairs, num of them) to hand out. */
typedef struct CalcMonsGain {
    int exp;
    int tp;
    int ep;
    int sp;
    int money;
    int grp[8];
    int item[8];
    int num;
} CalcMonsGain;

extern CalcMonsGain *monsGainPtrGet(void);
extern int calcLvUp(int cid, int exp, char *res);
extern int dataBoxInc(int category, int id);
extern int dataMoneyBoxInc(int amount);
void calcResultSub(BattleUnit *unit, CalcReward *total, int memberCount, CalcResultEntry *entry);

/*
 * Gives the battle's rewards: box items and money first, then each living
 * unit's share of the experience and any level-up, recorded as 15 words per
 * unit in results. Party members who sat out get three quarters of the
 * experience and no other points. Returns the number of living units.
 */
int calcResult(int *results)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    int partyIds[8];
    int totalGain[24];
    int resultBuf[15];
    CalcScratchActor fakeActorPl;
    CalcScratchUnit fakeUnitPl;
    BattleUnit **units;
    int aliveCount;
    CalcMonsGain *monsGain;
    int unitCount;
    int unitIdx;
    int inBattle;
    int partyCount;
    int partyCharaId;
    int partyIdx;
    BattleUnit *unit;
    CalcUnitParam *up;
    CalcCharParaData *org;
    short charaId;
    signed char lvBefore;
    short hpBefore;
    short epBefore;
    monsGain = monsGainPtrGet();
    unitCount = unitTblGet(0, &units);
    aliveCount = 0;
    for (unitIdx = 0; unitIdx < unitCount; unitIdx++) {
        if ((units[unitIdx] != 0) && (!(units[unitIdx]->work->flags & ACTOR_FLAG_DEAD))) {
            aliveCount++;
        }
    }

    for (unitIdx = 0; unitIdx < monsGain->num; unitIdx++) {
        dataBoxInc(monsGain->grp[unitIdx], monsGain->item[unitIdx]);
    }

    dataMoneyBoxInc(monsGain->money);
    /* Living units gain the rewards; an AGWS unit gains through its linked unit. */
    for (unitIdx = 0; unitIdx < unitCount; unitIdx++) {
        unit = units[unitIdx];
        if ((unit != 0) && (!(unit->work->flags & ACTOR_FLAG_DEAD))) {
            if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
                up = calcUPGet(units[unitIdx])->linkedUnit;
                fakeUnit.unit.work = (Actor *) &fakeActor;
                fakeActor.up = up;
                unit = &fakeUnit.unit;
            } else {
                up = calcUPGet(units[unitIdx]);
                unit = units[unitIdx];
            }
            charaId = up->charaId;
            calcResultSub(unit, (CalcReward *) monsGain, aliveCount, (CalcResultEntry *) results);
            org = dataUnitOrgGet(charaId);
            lvBefore = ((signed char *) org)[0x14];
            results[0] = charaId;
            hpBefore = org->maxHp;
            epBefore = org->maxEp;
            if (calcLvUp(charaId, results[1], (char *) results)) {
                results[5] = ((signed char *) org)[0x14] - lvBefore;
                results[6] = org->maxHp - hpBefore;
                results[7] = org->maxEp - epBefore;
                printf(calcLvUPIDText, charaId, ((signed char *) org)[0x14]);
            }
            results += 15;
        }
    }

    __builtin_memcpy(totalGain, monsGain, 0x58);
    totalGain[1] = 0;
    totalGain[2] = 0;
    totalGain[3] = 0;
    totalGain[0] = (totalGain[0] / 4) * 3;
    partyCount = PartyAllPartyGet(partyIds, 2);
    for (partyIdx = 0; partyIdx < partyCount; partyIdx++) {
        partyCharaId = partyIds[partyIdx];
        inBattle = 0;
        for (unitIdx = 0; unitIdx < unitCount; unitIdx++) {
            if (units[unitIdx] != 0) {
                if ((partyCharaId == calcUPGet(units[unitIdx])->charaId) || ((calcUPGet(units[unitIdx])->flags & CALC_UNIT_FLAG_IS_AGWS) && (partyCharaId == calcUPGet(units[unitIdx])->linkedUnit->charaId))) {
                    inBattle = 1;
                    break;
                }
            }
        }

        if (inBattle != 1) {
            fakeActorPl.up = (CalcUnitParam *) dataUnitOrgGet(partyCharaId);
            fakeUnitPl.unit.work = (Actor *) &fakeActorPl;
            memset(resultBuf, 0, 0x3C);
            calcResultSub(&fakeUnitPl.unit, (CalcReward *) totalGain, aliveCount, (CalcResultEntry *) resultBuf);
            if (calcLvUp(partyCharaId, resultBuf[1], (char *) resultBuf)) {
                printf(calcAltLvUPIDText, partyCharaId, ((signed char *) calcUPGet(&fakeUnitPl.unit))[0x14]);
            }
        }
    }

    return aliveCount;
}

/*
 * calcSkill's mask bits of the reward skills (kind 0x1000) calcResultSub
 * asks for, each a percentage bonus on one reward.
 */
enum SkillRewardBit {
    SKILL_REWARD_SKILL_POINT = 0x800,
    SKILL_REWARD_ETHER_POINT = 0x1000,
    SKILL_REWARD_TECH_POINT = 0x2000,
    SKILL_REWARD_EXP = 0x4000
};

/*
 * Gives unit's character its share of total (each reward divided by
 * memberCount, then raised by the matching reward skill's percentage) and
 * records the shares in entry.
 */
void calcResultSub(BattleUnit *unit, CalcReward *total, int memberCount, CalcResultEntry *entry)
{
    CalcPlCharaView *pl;
    int gain;
    int bonus;

    pl = dataPlChaGet(calcUPGet(unit)->charaId);

    gain = total->exp / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_EXP);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->exp += gain;
    if (pl->exp > 9999999) {
        pl->exp = 9999999;
    }
    entry->gained.exp = gain;

    gain = total->techPoint / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_TECH_POINT);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->techPoint += gain;
    if (pl->techPoint > 9999) {
        pl->techPoint = 9999;
    }
    entry->gained.techPoint = gain;

    gain = total->etherPoint / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_ETHER_POINT);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->etherPoint += gain;
    if (pl->etherPoint > 9999) {
        pl->etherPoint = 9999;
    }
    entry->gained.etherPoint = gain;

    gain = total->skillPoint / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_SKILL_POINT);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->skillPoint += gain;
    if (pl->skillPoint > 9999) {
        pl->skillPoint = 9999;
    }
    entry->gained.skillPoint = gain;
}

extern unsigned char *dataExpTblGet();
extern void *dataSpecTblGet(int cid);
extern void *dataParaTblGet(int cid);
extern short dataSpecLearnGet(int cid, int id);
extern void dataSpecLearnSet(int cid, int id);

/*
 * Subtracts exp from the character's exp to the next level and gains levels
 * while that count is spent, learning any specs reached and recalculating the
 * stats per level. Learned specs are appended to res. Returns 1 when a level
 * was gained, 0 otherwise.
 */
int calcLvUp(int cid, int exp, char *res)
{
    CalcPlCharaView *pl;
    CalcCharParaData *org;
    int i;
    unsigned short *spec;
    char *para;
    char *base;
    unsigned short id;
    short *p;
    int old;
    int v;
    int d;
    int off;
    int *cnt;
    int n;
    int nv;
    int table;
    int expEntry;
    int expGain;

    pl = dataPlChaGet(cid);
    org = dataUnitOrgGet(cid);
    if (org->level >= 99) return 0;
    if ((pl->expToNext -= exp) > 0) goto ret0;
    while (org->level < 99) {
        org->level++;
        table = (int)dataExpTblGet(cid);
        expEntry = org->level * 4;
        expEntry += table;
        LoadWordUnaligned(expGain, expEntry);
        pl->expToNext += expGain;
        spec = (unsigned short *)dataSpecTblGet(cid) + 1;
        for (i = 0; i < 8; i++, spec += 2) {
            id = spec[-1];
            if (id != 0 && dataSpecLearnGet(cid, id) == 0 && spec[0] != 0
                && org->level >= spec[0]) {
                dataSpecLearnSet(cid, id);
                cnt = (int *)(res + 0x38);
                n = *cnt;
                ((int *)(res + 0x20))[n] = id;
                *cnt = n + 1;
                printf("** Spec Learn: (%d) %d\n", cid, id);
            }
        }
        /* Moves the eight parameters toward their level-table values; the running sums are capped. */
        para = dataParaTblGet(cid);
        base = (char *) pl + 8;
        for (i = 0; i < 8; i++) {
            off = i * 2 + 0x10;
            nv = *(unsigned short *)(para + off);
            old = *(short *)(base + off);
            v = ((nv - old) * 100 / (100 - org->level) + 25) / 50 + old;
            *(short *)(base + off) = v;
            d = v - old;
            switch (i) {
            case 0: if ((short)(org->statSum[0] += d) >= 1000) org->statSum[0] = 999; break;
            case 1: if ((short)(org->statSum[1] += d) >= 1000) org->statSum[1] = 999; break;
            case 2: if ((short)(org->statSum[2] += d) >= 1000) org->statSum[2] = 999; break;
            case 3: if ((short)(org->statSum[3] += d) >= 1000) org->statSum[3] = 999; break;
            case 4: if ((signed char)(org->gainSum[0] += d) >= 100) org->gainSum[0] = 99; break;
            case 5: if ((signed char)(org->gainSum[1] += d) >= 100) org->gainSum[1] = 99; break;
            case 6: if ((org->maxHp += d) >= 10000) org->maxHp = 9999; break;
            case 7: if ((org->maxEp += d) >= 100) org->maxEp = 99; break;
            }
        }
        if (pl->expToNext > 0) return 1;
    }
    return 1;
ret0:
    return 0;
}

/*
 * entryPhase40 (src/ov01/entry_first_init.c) calls this as a bare statement
 * and never reads a result; the int return type, left unset, is kept to
 * reproduce the original's epilogue.
 */
int calcCfEncount(int flags)
{
    if (flags & 1)
    {
        calcPartyStat(0, 0);
        menuTimeSet(1);
    }
    if (flags & 2)
    {
        calcPartyStat(0, 0);
        calcPartyStat(0, 1);
    }
}

void calcPartyStat(int side, int mode)
{
    BattleUnit **table;
    BattleUnit *unit;
    int count;
    int i;

    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        unit = table[i];
        if (unit != 0) {
            switch (mode) {
            case 0:
                calcUPGet(unit)->waitCount = 0;
                break;
            case 1:
                calcUPGet(unit)->bcValue++;
                break;
            }
        }
    }
}

int calcEtherDisableGet(BattleUnit *unit, int bit)
{
    u16 mask;

    mask = 1 << bit;
    return calcUPGet(unit)->etherDisableFlags & mask;
}

void calcEtherDisableSet(BattleUnit *unit, int bit)
{
    CalcUnitParam *param;
    u16 mask;

    mask = 1 << bit;
    param = calcUPGet(unit);
    param->etherDisableFlags |= mask;
}

int rnd(int max)
{
    return xglSRand() % (max + 1);
}

/* The pointer type changes no byte here (two `sw` of the argument). */
void calcSplineInit(CalcSpline *spline, Vector4 *points, int count, int duration)
{
    spline->points = points;
    spline->count = count;
    spline->duration = (float) duration;
    spline->tension = 0.5f;
    spline->frame = 0;
}

/* calcSpline's own callees, all defined below in this TU. */
void calcLinerSub(CalcSpline *spline, Vector4 *out, int idx, float ratio);
void calcBezierSub(CalcSpline *spline, Vector4 *out, int idx, float t);
void calcSplineSub(CalcSpline *spline, Vector4 *out, int idx, float t);
void calcHermiteSub(CalcSpline *spline, Vector4 *out, int idx, float t);

/*
 * Evaluates the curve at the current frame into out with the interpolation
 * mode selects (1 B-spline, 2 Bezier over the whole curve, 3 Hermite,
 * anything else linear). The segmented modes split the duration less one
 * frame evenly over count - 3 (B-spline) or count - 1 segments and pass the
 * segment index and the position inside it. Once the frame reaches the
 * duration out takes the last point and 1 is returned; until then the
 * frame advances and 0 is returned.
 */
int calcSpline(CalcSpline *spline, Vector4 *out, int mode)
{
    float frame;
    float step;
    int idx;

    switch (mode) {
    case 2:
        frame = spline->frame;
        calcBezierSub(spline, out, 0, 1.0f / (spline->duration - 1.0f) * frame);
        break;
    case 1:
        frame = spline->frame;
        step = (spline->duration - 1.0f) / (spline->count - 3);
        idx = frame / step;
        calcSplineSub(spline, out, idx, 1.0f / step * (frame - idx * step));
        break;
    case 3:
        frame = spline->frame;
        step = (spline->duration - 1.0f) / (spline->count - 1);
        idx = frame / step;
        calcHermiteSub(spline, out, idx, 1.0f / step * (frame - idx * step));
        break;
    case 0:
    default:
        frame = spline->frame;
        step = (spline->duration - 1.0f) / (spline->count - 1);
        idx = frame / step;
        calcLinerSub(spline, out, idx, 1.0f / step * (frame - idx * step));
        break;
    }
    if (spline->duration <= frame) {
        idx = spline->count - 1;
        out->x = spline->points[idx].x;
        out->y = spline->points[idx].y;
        out->z = spline->points[idx].z;
        return 1;
    }
    frame += 1.0f;
    spline->frame = frame;
    return 0;
}

/*
 * Linear interpolation between points[idx] and points[idx + 1] by ratio;
 * only x, y and z are written to out.
 */
void calcLinerSub(CalcSpline *spline, Vector4 *out, int idx, float ratio)
{
    Vector4 a, b;

    a = spline->points[idx];
    b = spline->points[idx + 1];
    out->x = a.x + (b.x - a.x) * ratio;
    out->y = a.y + (b.y - a.y) * ratio;
    out->z = a.z + (b.z - a.z) * ratio;
}

/*
 * The cubic Bezier through points[idx..idx + 3] at t, written to out. The
 * weights are (1-t)^3, 3t(1-t)^2, 3t^2(1-t) and t^3.
 */
void calcBezierSub(CalcSpline *spline, Vector4 *out, int idx, float t)
{
    Vector4 p0, p1, p2, p3;
    float u;
    float uu;
    float b0, b1, b2, b3;

    u = 1.0f - t;
    p0 = spline->points[idx];
    p1 = spline->points[idx + 1];
    p2 = spline->points[idx + 2];
    p3 = spline->points[idx + 3];
    uu = u * u;
    b0 = uu * u;
    b1 = 3 * t * uu;
    b2 = 3 * t * t * u;
    b3 = t * t * t;
    out->x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
    out->y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
    out->z = b0 * p0.z + b1 * p1.z + b2 * p2.z + b3 * p3.z;
}

/*
 * The uniform cubic B-spline segment over points[idx..idx + 3] at t,
 * written to out.
 */
void calcSplineSub(CalcSpline *spline, Vector4 *out, int idx, float t)
{
    Vector4 p0, p1, p2, p3;
    float t2;
    float t3;
    float b0, b1, b2, b3;

    t2 = t * t;
    t3 = t2 * t;
    p0 = spline->points[idx];
    p1 = spline->points[idx + 1];
    p2 = spline->points[idx + 2];
    p3 = spline->points[idx + 3];
    b0 = (-t3 + 3 * t2 - 3 * t + 1) / 6.0;
    b1 = (3 * t3 - 6 * t2 + 4) / 6.0;
    b2 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0;
    b3 = t3 / 6.0;
    out->x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
    out->y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
    out->z = b0 * p0.z + b1 * p1.z + b2 * p2.z + b3 * p3.z;
}

/*
 * The cubic Hermite segment from points[idx] to points[idx + 1] at t, written
 * to out. Each tangent is tension times the difference of the neighbouring
 * points; at either end of the array the difference is one-sided.
 */
void calcHermiteSub(CalcSpline *spline, Vector4 *out, int idx, float t)
{
    Vector4 p0, p1;
    Vector4 startTangent, endTangent;
    Vector4 *tangent;
    float t2;
    float t3;
    float h00, h01, h10, h11;
    int prev;
    int next;
    int i;

    t2 = t * t;
    t3 = t2 * t;
    h00 = 2 * t3 - 3 * t2 + 1;
    h01 = -2 * t3 + 3 * t2;
    h10 = t3 - 2 * t2 + t;
    h11 = t3 - t2;
    p0 = spline->points[idx];
    p1 = spline->points[idx + 1];
    tangent = &startTangent;
    for (i = 0; i < 2; i++) {
        if (idx == 0) {
            prev = 0;
            next = 1;
        } else if (idx >= spline->count - 1) {
            prev = idx - 1;
            next = idx;
        } else {
            prev = idx - 1;
            next = idx + 1;
        }
        tangent->x = spline->tension * (spline->points[next].x - spline->points[prev].x);
        tangent->y = spline->tension * (spline->points[next].y - spline->points[prev].y);
        tangent->z = spline->tension * (spline->points[next].z - spline->points[prev].z);
        tangent = &endTangent;
        idx++;
    }
    out->x = h00 * p0.x + h01 * p1.x + h10 * startTangent.x + h11 * endTangent.x;
    out->y = h00 * p0.y + h01 * p1.y + h10 * startTangent.y + h11 * endTangent.y;
    out->z = h00 * p0.z + h01 * p1.z + h10 * startTangent.z + h11 * endTangent.z;
}

/*
 * Scales vec to unit length in place. The scale is 1.0f / sqrt(x*x + y*y +
 * z*z); when that scale equals 0.0f, all three components are set to 0.0f
 * instead. The input length itself is never compared with zero.
 */
void calcNormal(Vector4 *vec)
{
    float scale;

    scale = 1.0f / __builtin_sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    if (scale == 0.0f) {
        vec->x = 0.0f;
        vec->y = 0.0f;
        vec->z = 0.0f;
    } else {
        vec->x *= scale;
        vec->y *= scale;
        vec->z *= scale;
    }
}



static signed char turn_1[8][16] = { { 5, 5, 5, -1, 5, -1, 5, -1, -1, -1, -1, -1, 5, 5, 0, 0 }, { 0, 1, -1, -1, -1, -1, -1, 5, 1, 3, -1, -1, -1, -1, 5, 0 }, { -1, 0, 0, -1, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5, -1, 0 }, { 0, -1, 5, 5, 5, 5, 3, -1, -1, 5, 5, -1, 5, 5, 5, 0 }, { 5, 5, 5, 0, -1, 5, 5, -1, 5, 5, -1, 5, -1, 5, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, 0 }, { -1, -1, -1, -1, -1, 0, 0, 0, -1, -1, -1, 3, 2, 3, 3, 0 }, { 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

const char calcAGWSParaEPEPMAXText[32] = "** AGWS para = EP=%d EPMAX=%d\n";

const char calcAGWSParaEatkEdefHITEVAText[48] = "** AGWS para = Eatk=%d Edef=%d HIT=%d EVA=%d\n";

const char calcPLMUTEKIText[16] = "** PL MUTEKI\n";

const char calcENMUTEKIText[16] = "** EN MUTEKI\n";

const char calcBPUpEventTimerGText[32] = "** BP up: EventTimer G (%d)\n";

const char calcDMGEVAText[16] = "** DMG: EVA \n";

const char calcPLICHIGEKIText[16] = "** PL ICHIGEKI\n";

const char calcJobTypeErrText[24] = "** jobType err -> %d\n";

const char calcWpnAtkGetAttEquipErrText[40] = "** calcWpnAtkGet: Att equip err! %d\n";

const char calcWpnAtkGetWpnAtkAdjText[32] = "** calcWpnAtkGet: wpnAtkAdj=%d\n";

const char calcPhyArmMagArmText[24] = "** phyArm=%d magArm=%d\n";

const char calcTGTINFOJTYPECTYPEFCText[48] = "** TGTINFO=%04X JTYPE=%d CTYPE=%d FC=%X\n";

const char calcDMG0HitAtrHitFlagDmgText[40] = "** DMG0:hitAtr=%d hitFlag=%X dmg=%d\n";

const char calcStatDEATHText[16] = "** Stat: DEATH\n";

const char calcStatGRAVText[16] = "** Stat: GRAV\n";

const char calcDMG1HitAtrHitFlagDmgText[40] = "** DMG1:hitAtr=%d hitFlag=%X dmg=%d\n";

const char calcStatNoEffectText[24] = "** Stat: no effect\n";

const char calcStatAtkAttStatText[32] = "** calcStatAtk: Att stat\n";

const char calcStatAtkAttEquipErrText[32] = "** calcStatAtk: Att equip err!\n";

const char calcStatAtkWpnStatText[32] = "** calcStatAtk: Wpn stat\n";

const char calcStatChkNGText[24] = "** Stat: Chk NG\n";

const char calcStatAtkFormat[32] = "** Stat: Atk (%d%%) %d %04X\n";

const char calcCritNoEffectText[24] = "** Crit: no effect\n";

const char calcCritSlotAText[24] = "** Crit: slot A\n";

const char calcCritEventTimerRText[24] = "** Crit: EventTimer R\n";

const char calcCritOBJCRITText[24] = "** Crit: OBJ_CRIT\n";

const char calcCritWeakXText[32] = "** Crit: weak (%04X) x%f\n";

const char calcCritMonKindText[24] = "** Crit: monKind %04X\n";

const char calcCritAtkFormat[24] = "** Crit: Atk (%d%%) %d\n";

const char calcSpecEffectInfoText[24] = "** Spec: effect(info)\n";

const char calcSpecAtkDefText[24] = "** Spec: atk=%X def=%X\n";

const char calcSpecEffectABSBText[24] = "** Spec: effect(ABSB)\n";

const char calcSpecErrSPECABSBText[32] = "** calcSpec err SPEC_ABSB\n";

const char calcSpecEffectDEFText[24] = "** Spec: effect(DEF)\n";

const char calcSpecErrSPECDEFText[32] = "** calcSpec err SPEC_DEF\n";

const char calcSpecSkillALLText[24] = "** Spec: skill(ALL)\n";

const char calcSpecSkillText[24] = "** Spec: skill(%X)\n";

const char calcLinePosAdjustText[24] = "** LinePos: adjust\n";

const char calcReTGTIDText[24] = "** reTGT! (ID=%d) ->";

const char calcTgtIDText[16] = "** tgt ID=";

const char calcJpText[8] = "%d ";

const char calcJpText2[8] = "\n";

const char calcLvUPIDText[24] = "Lv UP! (ID=%d) %d\n";

const char calcAltLvUPIDText[24] = "Alt Lv UP! (ID=%d) %d\n";
