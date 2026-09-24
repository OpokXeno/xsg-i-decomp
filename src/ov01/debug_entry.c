/*
 * OV01 original TU 14: 0x00a2e4b0..0x00a31340 (64 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/battle_init.h"
#include "ov01/calc.h"

/* Debug menu: enters the battle debug interface; page 0/1/2 selects the
 * status, parameter or spec display (debugBattle tail-calls one of them). */
extern void debugBattle(int page);

void debugEntry(int page)
{
    debugBattle(page);
}

extern char *err; /* fallback string returned for an out-of-range debug table index */
extern char *nameTbl[0xC3]; /* debug menu's character name table */

char *debugNameGet(int charaId) {
    if (charaId < 0xC3) {
        return nameTbl[charaId];
    }
    return err;
}

extern char *wpnTbl[0x74]; /* debug menu's weapon name table */

char *debugWpnGet(int wpnId) {
    if (wpnId < 0x74) {
        return wpnTbl[wpnId];
    }
    return err;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugCmdPrint);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugThinkRegName);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugThinkPrint);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugBattle);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugStatDisp);

extern int unitTblGet(int side, BattleUnit ***table);
extern int unitNoGet(ObjectTask *unit);
extern char *debugNameGet(int charaId);

/*
 * calcUPGet's per-unit parameter record, evidenced here by debugParaDisp's
 * own printf labels (include/ov01/calc.h's CalcUnitParam is this TU's own
 * independent, differently-scoped view of the same object: it already
 * evidences agility/moveKindId +0x0E-0x0F, flags +0x1C, charaId +0x38 and
 * turnBias/waitCount/waitCountBase +0x3E-0x42 from other functions).
 */
typedef struct DebugUnitPara {
    short maxHp;             /* +0x00: "HP=%d/%d" 2nd */
    short maxEp;             /* +0x02: "EP=%d/%d" 2nd */
    short str;               /* +0x04: "STR=%3d" */
    short vit;               /* +0x06: "VIT=%3d" */
    short eatk;              /* +0x08: "EATK=%3d" */
    short edef;              /* +0x0A: "EDEF=%3d" */
    signed char hit;         /* +0x0C: "HIT=%3d" */
    signed char eva;         /* +0x0D: "EVA=%3d" */
    signed char agility;     /* +0x0E: "AGL=%3d" */
    signed char moveKindId;  /* +0x0F: "MVTYPE=%1d" */
    unsigned char unmodeled_10[0x14 - 0x10];
    signed char level;       /* +0x14: "LV=%d" */
    unsigned char unmodeled_15[0x16 - 0x15];
    signed char cRate;       /* +0x16: "C_RATE=%3d" */
    unsigned char unmodeled_17[0x18 - 0x17];
    unsigned short weak;     /* +0x18: "WEAK=%04X" */
    unsigned short spec;     /* +0x1A: "SPEC=%04X" */
    unsigned short flags;    /* +0x1C: "INFO=%04X" */
    unsigned char unmodeled_1e[0x34 - 0x1E];
    short hp;                /* +0x34: "HP=%d/%d" 1st */
    short ep;                /* +0x36: "EP=%d/%d" 1st */
    short charaId;           /* +0x38: debugNameGet's argument */
    unsigned char unmodeled_3a[0x3C - 0x3A];
    signed char bc;          /* +0x3C: "BC =%3d" */
    signed char bp;          /* +0x3D: "BP = %3d" */
    short turnBias;          /* +0x3E: "AGLPLUS=%3d" */
    short waitCount;         /* +0x40: "WCT=%3d/%3d" 1st */
    short waitCountBase;     /* +0x42: "WCT=%3d/%3d" 2nd */
    unsigned char unmodeled_44[0x14C - 0x44];
    unsigned short etherDis; /* +0x14C: "ETHERDIS=%04X" */
    unsigned char unmodeled_14e[0x17C - 0x14E];
    int wpnAtkAdj;           /* +0x17C: "WPNATKADJ=%3d" */
} DebugUnitPara;

extern DebugUnitPara *calcUPGet(ObjectTask *unit);

extern const char D_00A506C0[]; /* "** UNIT PARA **\n" */
extern const char D_00A50668[]; /* "** %d) %s LV=%d HP=%d/%d EP=%d/%d\n" */
extern const char D_00A506D8[]; /* " STR=%3d VIT=%3d EATK=%3d EDEF=%3d HIT=%3d EVA=%3d\n" */
extern const char D_00A50710[]; /* " AGL=%3d BC =%3d BP = %3d MVTYPE=%1d C_RATE=%3d\n" */
extern const char D_00A50748[]; /* " WEAK=%04X SPEC=%04X INFO=%04X OBJSTAT=%04X\n" */
extern const char D_00A50778[]; /* " WCT=%3d/%3d AGLPLUS=%3d ETHERDIS=%04X WPNATKADJ=%3d\n" */
extern int printf(const char *format, ...);

int debugParaDisp(void)
{
    BattleUnit **units;
    DebugUnitPara *up;
    int *objStat;
    int count;
    int i;

    count = unitTblGet(2, &units);
    printf(D_00A506C0);
    for (i = 0; i < count; i++) {
        if (units[i] != 0) {
            up = calcUPGet((ObjectTask *)units[i]);
            printf(D_00A50668, unitNoGet((ObjectTask *)units[i]), debugNameGet(up->charaId),
                   up->level, up->hp, up->maxHp, up->ep, up->maxEp);
            printf(D_00A506D8, up->str, up->vit, up->eatk, up->edef, up->hit, up->eva);
            printf(D_00A50710, up->agility, up->bc, up->bp, up->moveKindId, up->cRate);
            objStat = ((ObjectTask *)units[i])->work;
            printf(D_00A50748, up->weak, up->spec, up->flags, *objStat);
            printf(D_00A50778, up->waitCount, up->waitCountBase, up->turnBias,
                   up->etherDis, up->wpnAtkAdj);
        }
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugSpecDisp);

extern int plMuteki; /* debug menu: player invincibility flag */

int plMutekiGet(void)
{
    return plMuteki;
}

extern int enMuteki; /* debug menu: enemy invincibility flag */

int enMutekiGet(void)
{
    return enMuteki;
}

extern int plIchigeki; /* debug menu: player one-hit-kill flag */

int plIchigekiGet(void)
{
    return plIchigeki;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugItemSet);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugEtherSet);

void debugSkillSet(void) {

}

extern int calcAgwsEquipOrg(int agwsId, int charaId);
extern int calcAttEquipOrg(int charaId, int slot, int attId);
extern int calcWpnEquipOrg(int charaId, int slot, int wpnId, int variant);

/* Debug menu: forces a fixed test loadout onto character 0x1A. */
int debugEquSet(void)
{
    calcAgwsEquipOrg(6, 0x1A);
    calcWpnEquipOrg(0x1A, 0, 0x4E, 0x19);
    calcWpnEquipOrg(0x1A, 1, 0x4E, 0x21);
    calcAttEquipOrg(0x1A, 0, 1);
    return calcAttEquipOrg(0x1A, 1, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", debugBattleConfig);

int configStart(void) {
    return 1;
}

/* Debug menu: cycles the character assigned to party slot `slot`. */
extern int configPlayer(int slot);

int configPlayer1(void) {
    return configPlayer(0);
}

int configPlayer2(void) {
    return configPlayer(1);
}

int configPlayer3(void) {
    return configPlayer(2);
}

/*
 * The party roster record plUnitTblGet (ov01 VA 0x00a0b630, still asm in its
 * own TU battle_init.c) returns the base of: configPlayer cycles player
 * (+0x00) 0..0x1F, configPlPos cycles position (+0x02) 1..9. plUnitTblSet
 * writes both fields back for one slot.
 */
typedef struct DebugPartyUnit {
    short player;   /* +0x00 */
    short position; /* +0x02 */
    unsigned char unmodeled_04[4];
} DebugPartyUnit;

extern DebugPartyUnit *plUnitTblGet(void);
extern void plUnitTblSet(int slot, int player, int position);
extern int valLR(int *value, int min, int max, int step);

int configPlayer(int slot)
{
    DebugPartyUnit *units;
    int player;

    units = plUnitTblGet();
    player = units[slot].player;
    valLR(&player, 0, 31, 1);
    plUnitTblSet(slot, player, units[slot].position);
    return 0;
}

/* Debug menu: cycles the formation position of party slot `slot`. */
extern int configPlPos(int slot);

int configPlPos1(void) {
    return configPlPos(0);
}

int configPlPos2(void) {
    return configPlPos(1);
}

int configPlPos3(void) {
    return configPlPos(2);
}

int configPlPos(int slot)
{
    DebugPartyUnit *units;
    int position;

    units = plUnitTblGet();
    position = units[slot].position;
    valLR(&position, 1, 9, 1);
    plUnitTblSet(slot, units[slot].player, position);
    return 0;
}

/* Debug menu left/right adjuster: steps *value by step (d-pad) or by
   10 * step (shoulder buttons), wrapping from past max to min and from below
   min to max. Returns nonzero when *value changed. */
extern int valLR(int *value, int min, int max, int step);

extern void monsSetNoSet(int monsSetNo);
extern int monsSet; /* debug menu's selected monster-set number */

int configMonsSet(void) {
    valLR(&monsSet, 0, 0x63, 1);
    monsSetNoSet(monsSet);
    return 0;
}

extern void mapNoSet(int mapNo);
extern int mapNo; /* debug menu's selected map number */

int configMap(void) {
    valLR(&mapNo, 0, 0x63, 1);
    mapNoSet(mapNo);
    return 0;
}

extern int cameraFlagGet(void);
extern void cameraFlagSet(int flag);

int configCamera(void) {
    int flag;

    flag = cameraFlagGet();
    valLR(&flag, 0, 1, 1);
    cameraFlagSet(flag);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configStat);

extern int equipId; /* debug equip menu's selected character index, 1..0x20 */
extern PadPrefix PadData;
extern int configSeq; /* debug menu's current top-level page */
extern int equipPosX; /* debug menu's cursor column on the equip screen */

int configEqu(void) {
    valLR(&equipId, 1, 0x20, 1);
    if (PadData.half_2a & 0x20) {
        configSeq = 2;
        equipPosX = 0;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", configGain);

int configPlMuteki(void) {
    int flag;

    flag = plMutekiGet();
    valLR(&flag, 0, 1, 1);
    plMuteki = flag;
    return 0;
}

int configEnMuteki(void) {
    int flag;

    flag = enMutekiGet();
    valLR(&flag, 0, 1, 1);
    enMuteki = flag;
    return 0;
}

int configPlIchigeki(void) {
    int flag;

    flag = plIchigekiGet();
    valLR(&flag, 0, 1, 1);
    plIchigeki = flag;
    return 0;
}

extern void cfEncountSet(int cfEncount);
extern int cfEncount; /* debug menu's CF encounter value */

int configCFEncount(void) {
    valLR(&cfEncount, 0, 0xFFFF, 1);
    cfEncountSet(cfEncount);
    return 0;
}

extern void cfEventSet(int cfEvent);
extern int cfEvent; /* debug menu's CF event value */

int configCFEvent(void) {
    valLR(&cfEvent, 0, 0xFFFF, 1);
    cfEventSet(cfEvent);
    return 0;
}

extern void thinkNoSet(int thinkNo);
extern int D_00A5AA60; /* debug menu's selected think (AI) number */

int configThink(void) {
    valLR(&D_00A5AA60, 0, 0x63, 1);
    thinkNoSet(D_00A5AA60);
    return 0;
}

/*
 * dataUnitOrgGet (ov01 VA 0x00a191c0, still asm in its own TU
 * data_unit_org_get.c) returns character chrNo's original stat record;
 * calcTotalParaMenu (ov01 VA 0x00a11108, still asm in ov01/calc.c) returns a
 * recalculated copy of the same record (its own maxHp/maxEp lead the same
 * way src/main/menu_para_pt_rate_get.c's CharParaData already evidences).
 * The equip screens below index its agws/engine/frame/accessory/attachment/
 * special/ether/skill ids by the character calcXxxEquipOrg is told to
 * refit.
 */
typedef struct UnitOrgData {
    short maxHp;               /* +0x00 */
    short maxEp;                /* +0x02 */
    unsigned char unmodeled_04[0x34 - 0x04];
    short hp;                   /* +0x34 */
    short ep;                   /* +0x36 */
    short charaId;               /* +0x38 */
    unsigned char unmodeled_3a[0x54 - 0x3A];
    short agwsId;                /* +0x54 */
    short engineId;               /* +0x56 */
    short frameId;                /* +0x58 */
    unsigned char unmodeled_5a[0x64 - 0x5A];
    short accessory[3];           /* +0x64 */
    short attachment[3];          /* +0x6A */
    unsigned char unmodeled_70[0x82 - 0x70];
    short special[6];             /* +0x82 */
    short ether[12];              /* +0x8E */
    short skill[3];                /* +0xA6 */
} UnitOrgData;

extern UnitOrgData *calcTotalParaMenu(int chrNo, int *attack, int *defense);
extern UnitOrgData *dataUnitOrgGet(int chrNo);

int configYadoya(void) {
    int chrNo;
    UnitOrgData *total;
    UnitOrgData *rec;

    if (PadData.half_2a & 0x20) {
        for (chrNo = 1; chrNo <= 0x20; chrNo++) {
            total = calcTotalParaMenu(chrNo, 0, 0);
            rec = dataUnitOrgGet(chrNo);
            rec->hp = total->maxHp;
            rec->ep = total->maxEp;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", valUD);

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", valLR);

extern int equipId;

int equipChar(void) {
    valLR(&equipId, 1, 0x20, 1);
    return 0;
}

int equipAgws(void) {
    UnitOrgData *unit;
    int agwsId;

    unit = dataUnitOrgGet(equipId);
    agwsId = unit->agwsId;
    valLR(&agwsId, 0x11, 0x20, 1);
    calcAgwsEquipOrg(unit->charaId, agwsId);
    return 0;
}

extern int calcEngineEquipOrg(int charaId, int engineId);

int equipEngine(void) {
    UnitOrgData *unit;
    int engineId;

    unit = dataUnitOrgGet(equipId);
    engineId = unit->engineId;
    valLR(&engineId, 0, 0x28, 1);
    calcEngineEquipOrg(unit->charaId, engineId);
    return 0;
}

extern int calcFrameEquipOrg(int charaId, int frameId);

int equipFrame(void) {
    UnitOrgData *unit;
    int frameId;

    unit = dataUnitOrgGet(equipId);
    frameId = unit->frameId;
    valLR(&frameId, 0, 0x29, 1);
    calcFrameEquipOrg(unit->charaId, frameId);
    return 0;
}

/* Debug equip menu: cycles the weapon in equipment slot `slot` of the
   character selected by equipId. */
extern int equipWpn(int slot);

int equipWpn0(void) {
    return equipWpn(0);
}

int equipWpn1(void) {
    return equipWpn(1);
}

int equipWpn2(void) {
    return equipWpn(2);
}

INCLUDE_ASM("asm/nonmatchings/ov01/debug_entry", equipWpn);

/* Debug equip menu: cycles the attachment in equipment slot `slot` of the
   character selected by equipId. */
extern int equipAtt(int slot);

int equipAtt0(void) {
    return equipAtt(0);
}

int equipAtt1(void) {
    return equipAtt(1);
}

int equipAtt2(void) {
    return equipAtt(2);
}

int equipAtt(int slot) {
    UnitOrgData *unit;
    int attId;

    unit = dataUnitOrgGet(equipId);
    attId = unit->attachment[slot];
    valLR(&attId, 0, 0xB2, 1);
    calcAttEquipOrg(unit->charaId, slot, attId);
    return 0;
}

/* Debug equip menu: cycles the accessory in equipment slot `slot` of the
   character selected by equipId. */
extern int equipAcc(int slot);

int equipAcc0(void) {
    return equipAcc(0);
}

int equipAcc1(void) {
    return equipAcc(1);
}

int equipAcc2(void) {
    return equipAcc(2);
}

extern int calcAccEquipOrg(int charaId, int slot, int accId);

int equipAcc(int slot) {
    UnitOrgData *unit;
    int accId;

    unit = dataUnitOrgGet(equipId);
    accId = unit->accessory[slot];
    valLR(&accId, 0, 0xB4, 1);
    calcAccEquipOrg(unit->charaId, slot, accId);
    return 0;
}

extern int calcSpecEquipOrg(int charaId, int slot, int specId);
extern int slot; /* shared debug cursor for the spec/ether/skill equip screens */
extern int specPos[6]; /* screen cursor column per spec slot */

int equipSpec(void) {
    UnitOrgData *unit;
    int specId;

    unit = dataUnitOrgGet(equipId);
    specId = unit->special[slot];
    valLR(&specId, 0, 0x74, 1);
    calcSpecEquipOrg(unit->charaId, slot, specId);
    if (PadData.half_2a & 0x20) {
        slot += 1;
        if (slot >= 6) {
            slot = 0;
        }
        equipPosX = specPos[slot];
    }
    return 0;
}

extern int equipEther(int firstSlot, int lastSlot);

int equipEther0(void) {
    return equipEther(0, 3);
}

int equipEther1(void) {
    return equipEther(4, 7);
}

int equipEther2(void) {
    return equipEther(8, 0xB);
}

extern int calcEtherEquipOrg(int charaId, int slot, int etherId);
extern int etherPos[4]; /* screen cursor column per ether sub-slot */

int equipEther(int firstSlot, int lastSlot) {
    UnitOrgData *unit;
    int etherId;

    unit = dataUnitOrgGet(equipId);
    etherId = unit->ether[slot + firstSlot];
    valLR(&etherId, 0, 0xB3, 1);
    calcEtherEquipOrg(unit->charaId, slot + firstSlot, etherId);
    if (PadData.half_2a & 0x20) {
        slot += 1;
        if ((lastSlot - firstSlot) < slot) {
            slot = 0;
        }
        equipPosX = etherPos[slot];
    }
    return 0;
}

extern int calcSkillEquipOrg(int charaId, int slot, int skillId);
extern int skillPos[3]; /* screen cursor column per skill slot */

int equipSkill(void) {
    UnitOrgData *unit;
    int skillId;

    unit = dataUnitOrgGet(equipId);
    skillId = unit->skill[slot];
    valLR(&skillId, 0, 0x5B, 1);
    calcSkillEquipOrg(unit->charaId, slot, skillId);
    if (PadData.half_2a & 0x20) {
        slot += 1;
        if (slot >= 3) {
            slot = 0;
        }
        equipPosX = skillPos[slot];
    }
    return 0;
}
