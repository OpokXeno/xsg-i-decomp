/*
 * TU-local declarations of ov01/tu004 (src/ov01/calc.c).
 */

#ifndef SRC_OV01_CALC_H
#define SRC_OV01_CALC_H

#include "shared.h"

/*
 * The per-unit parameter block calcUPGet (this TU, still asm) returns for a
 * battle unit's ObjectTask. Only the members other TUs' code reads are
 * modelled; everything between them stays an unmodeled span.
 *
 * - flags (+0x1C): a halfword bit set; sndConvSe (ov01/tu013) and
 *   unitCmdMove (ov01/tu002) both read it with `lhu $3,0x1C($2)` right after
 *   `jal calcUPGet` (0x00a2dfc0, 0x00a02634); sndConvSe tests bit 0x40.
 * - charaId (+0x38): the character/type id. unitCidGet feeds it to
 *   dataCidGet, and unitLoad range-tests it (0xBB..0xC2) to decide whether
 *   the unit still needs its sound-effect setup call.
 * - sefSetupParams (+0x5E/+0x60/+0x62): the three shorts unitLoad hands to
 *   sefSetupPlayer as its second through fourth arguments (lh 0x5e/0x60/0x62
 *   at 0x00a018d8..0x00a018e8). sefSetupPlayer stores them, next to the id,
 *   in its _battlePrm slot (sh at +0x10/+0x12/+0x14,
 *   0x002e6d84..0x002e6d8c); nothing says more about what they select.
 * - linkedUnit (+0x50): another unit's parameter block. menuStatChrIdGet
 *   (ov01/tu009) loads it with `lw $2,0x50($18)` from its saved calcUPGet
 *   result and returns that block's charaId (`lh $17,0x38($2)`, 0x00a29cac)
 *   for an enemy unit or one in the 0xBB..0xC2 range.
 * - moveKindId (+0x0F): a signed byte unitCmdMove (ov01/tu002) loads with
 *   `lb $3,0xF($2)` right after `jal calcUPGet` (0x00a02640..0x00a02648) and
 *   compares with 4 to choose the warp move over the jump move.
 * - maxEp (+0x02): a short calcEp (this TU, 0x00a1254c) loads with
 *   `lh $3,2($2)` as the ceiling it clamps currentEp against, mirroring
 *   CalcCharParaData's own maxEp at the same relative offset.
 * - hitStat (+0x0C): an unsigned byte calcHitParaGet (this TU, 0x00a131b4)
 *   loads with `lbu $3,12($2)` and immediately reinterprets as signed
 *   (`sll $18,$3,0x18`/`sra $16,$18,0x18`) before the rest of its formula.
 * - agility (+0x0E): a signed byte calcWct (this TU, 0x00a12ce8) loads with
 *   `lb $7,0xE($2)` and subtracts from turnBias before scaling the result by
 *   9 and adding 0xB4 to seed the next wait count.
 * - turnBias (+0x3E): a short calcWct (this TU, 0x00a12ce4) loads with
 *   `lh $3,0x3E($2)` as the wait-count formula's base term; calcTurnEnd
 *   (this TU, 0x00a12cac) clears it with `sh $0,0x3E($2)` after computing
 *   the next turn's wait count.
 * - waitCount (+0x40): a short calcTurnEnd (this TU, 0x00a12c8c) stores
 *   calcWct's return into with `sh $2,0x40($17)`; calcWctDec (this TU,
 *   0x00a12e00/0x00a12e30) reads and rewrites the same short every time it
 *   ticks the count down.
 * - waitCountBase (+0x42): a short calcTurnEnd (this TU, 0x00a12c94) stores
 *   the same calcWct return into with `sh $2,0x42($18)`, a separate copy
 *   from waitCount that only calcTurnEnd (this TU) writes.
 * - currentHp/currentEp (+0x34/+0x36): two halfwords calcPara2OrgSub (this
 *   TU, 0x00a10f00) reads with `lhu $3,0x34($16)`/`lhu $4,0x36($16)` right
 *   after `jal calcUPGet` and copies into the unit's persistent origin
 *   record (dataUnitOrgGet's return) at the same offsets.
 * - accessoryId (+0x64): three halfword item ids calcMagDefGet (this TU,
 *   0x00a135d8) reads with `lh $3,4($2)` from calcUPGet(unit) + 0x60 + 2*i,
 *   i = 2..0, and feeds each non-zero one to dataAccGet for its magic
 *   defense byte.
 * - statEffUnits (+0x15C): eight object pointers. statEffOn/statEffOff
 *   (ov01/tu003, 0x00a0f620/0x00a0f6a8) walk them with `lw $3,0xC($2)` from
 *   calcUPGet(unit) + 0x150 + 4 * i, i = 0..7, skip null entries and set or
 *   clear bit 0x100 of the word at +0xA8C of each pointee. The pointee type
 *   is named only by its tag here; its first user completes it.
 * - statProtect (+0x24): a halfword calcStatProtectGet/calcStatProtectSet
 *   (this TU, 0x00a170c0/0x00a17100) index by a runtime group argument
 *   (`sll $16,$16,0x1`/`addu $16,$16,$2` before `lhu $2,36($16)`); every
 *   evidenced caller (calcDmg, calcStatSetChk, this TU) passes group 1, so
 *   only that element is named here and the array's real extent stays
 *   unresolved.
 * - etherDisableFlags (+0x14C): a halfword bit set calcEtherDisableGet/
 *   calcEtherDisableSet (this TU, 0x00a18740/0x00a18778) test and update
 *   with `lhu $2,332($2)`/`sh $16,332($2)` against a caller-selected bit.
 * - statActiveMask (+0xAC): eight halfword bit sets, one per status category;
 *   calcStatTurn (this TU, 0x00a16fd8) tests bit `calcStatIdx2Bit(statIdx)`
 *   of statActiveMask[category] (`lhu`, offset unchanged through the 16-index
 *   inner loop) before touching statTurnCount[category][statIdx].
 * - statTurnCount (+0xCC): an 8x16 byte grid of per-status remaining-turn
 *   counters; calcStatTurn (this TU) decrements the active ones each turn
 *   (`lbu`/`sb`, +0x10 per category) and, on reaching zero, calls
 *   calcStatReset(unit, category, calcStatIdx2Bit(statIdx), 0). 0xFF marks a
 *   counter as not running.
 */
typedef struct CalcUnitParam CalcUnitParam;

struct CalcUnitParam {
    unsigned char unmodeled_00[0x02];
    short maxEp;                /* +0x02: calcEp's clamp ceiling */
    unsigned char unmodeled_04[0x0C - 0x04];
    unsigned char hitStat;      /* +0x0C: calcHitParaGet's own signed view */
    unsigned char unmodeled_0d[0x0E - 0x0D];
    signed char agility;       /* +0x0E: calcWct's wait-count formula */
    signed char moveKindId;    /* +0x0F: unitCmdMove compares it with 4 */
    /*
    * +0x10/+0x12: the two halfwords unitColiGet (ov01/tu002, 0x00a07488)
    * adds into the gap unitCmdDstSet keeps between a moving unit and the
    * unit it walks up to. It reads `lh $3,16($2)` (0x00a074a0) from the
    * moving unit's own block and `lh $3,18($2)` (0x00a074b8) from the
    * approached unit's block, adds them and divides the sum by 100.0f;
    * unitCmdDstSet subtracts that distance from the target position.
    */
    short coliMove;             /* +0x10: term used when this unit moves */
    short coliTarget;           /* +0x12: term used when it is approached */
    unsigned char unmodeled_14[0x1C - 0x14];
    u16 flags;               /* +0x1C */
    unsigned char unmodeled_1e[0x24 - 0x1E];
    u16 statProtect[1];      /* +0x24: calcStatProtectGet/Set's group word */
    unsigned char unmodeled_26[0x34 - 0x26];
    u16 currentHp;           /* +0x34: calcPara2OrgSub's copy source */
    u16 currentEp;           /* +0x36: calcPara2OrgSub's copy source */
    short charaId;           /* +0x38 */
    unsigned char unmodeled_3a[0x3E - 0x3A];
    short turnBias;             /* +0x3E: calcWct/calcTurnEnd */
    short waitCount;            /* +0x40: calcTurnEnd/calcWctDec */
    short waitCountBase;        /* +0x42: calcTurnEnd */
    unsigned char unmodeled_44[0x50 - 0x44];
    CalcUnitParam *linkedUnit;   /* +0x50: menuStatChrIdGet reads its charaId */
    unsigned char unmodeled_54[0x5E - 0x54];
    short sefSetupParams[3]; /* +0x5E: sefSetupPlayer's extra arguments */
    short accessoryId[3];    /* +0x64: calcMagDefGet's per-slot item ids */
    unsigned char unmodeled_6a[0xAC - 0x6A];
    u16 statActiveMask[8];   /* +0xAC: calcStatTurn's per-category bit set */
    unsigned char unmodeled_bc[0xCC - 0xBC];
    unsigned char statTurnCount[8][16]; /* +0xCC: calcStatTurn's countdown grid */
    u16 etherDisableFlags;   /* +0x14C: calcEtherDisableGet/Set's bit set */
    unsigned char unmodeled_14e[0x15C - 0x14E];
    struct UnitWork *statEffUnits[8]; /* +0x15C: statEffOn/statEffOff */
};

/*
 * The battle actor object behind ObjectTask.work, as far as calcUPGet reads
 * it (0x00a11010..0x00a1101c): `lw $3,16($4)` loads unit->work and
 * `lw $2,132($3)` returns the CalcUnitParam pointer at +0x84. Several TUs
 * (e.g. src/ov01/unit_cmd.h, src/main/chr.h) already model the same real
 * object under the tag `Actor` with more members evidenced up to +0x70 or
 * further, but none of them reaches +0x84 yet, and that tag is theirs to
 * complete; this TU cannot redefine it, so it names its own bounded view of
 * the same object instead. Shared-header need: once an owning TU's Actor
 * header reaches +0x84, this member belongs there and this local view can be
 * dropped.
 *
 * calcTurnStart/calcTurnEnd (this TU, 0x00a12b50..0x00a12c28) read and
 * write the flags word at +0x00 directly through unit->work, the same
 * quantity src/ov01/unit_cmd.h and src/main/chr.h evidence further members
 * of under the tag `Actor`; this TU names only the one word it touches.
 */
typedef struct CalcActorRecord CalcActorRecord;

struct CalcActorRecord {
    int flags;                      /* +0x00: calcTurnStart/calcTurnEnd */
    unsigned char unmodeled_04[0x84 - 0x04];
    CalcUnitParam *up; /* +0x84: calcUPGet's return value. */
};

/*
 * calcUPGet (this TU, still asm, 0x00a11010): `lw $3,16($4)` loads
 * unit->work and `lw $2,132($3)` returns the CalcUnitParam pointer at
 * CalcActorRecord+0x84 above.
 */
CalcUnitParam *calcUPGet(ObjectTask *unit);

/*
 * A player character's persistent 0x180-byte-strided origin record;
 * dataUnitOrgGet (src/ov01/data_unit_org_get.c, still asm)
 * returns the one for charaId, and calcTotalParaMenu (this TU, still asm)
 * returns a recalculated copy of the same record. Only the members this
 * TU's own calls touch are modelled; src/main/menu_para_pt_rate_get.c
 * evidences further members (maxHp/maxEp/attack/phyDefense/magDefense at
 * +0x00/+0x02/+0x04/+0x06/+0x0A) under its own bounded view of the same
 * record.
 *
 * - hp/ep (+0x34/+0x36): calcPara2OrgSub's copy destination for
 *   CalcUnitParam's currentHp/currentEp.
 * - agwsId (+0x54): calcAgwsEquipOrg's single equipped-AGWS slot.
 * - hand (+0x5A): calcWpnEquipOrg's per-weapon-slot hand byte.
 * - weaponId (+0x5E): calcWpnEquipOrg's per-slot equipped weapon id.
 * - accessoryId (+0x64): calcAccEquipOrg's per-slot equipped accessory id.
 * - attachmentId (+0x6A): calcAttEquipOrg's per-slot equipped attachment id.
 * - specialId (+0x82): calcSpecEquipOrg's per-slot equipped special id.
 * - etherId (+0x8E): calcEtherEquipOrg's per-slot equipped ether id.
 * - skillId (+0xA6): calcSkillEquipOrg's per-slot equipped skill id.
 * - maxHp/maxEp (+0x00/+0x02): two halfwords calcTotalParaMenuSub (this TU,
 *   0x00a11128) reads with `lh $2,0($18)`/`lh $2,2($18)` from its own
 *   recalculated-record argument and clamps hp/ep down to when they are
 *   smaller, mirroring the same pair src/main/menu_para_pt_rate_get.c
 *   evidences under its own bounded view of this record.
 */
typedef struct CalcCharParaData CalcCharParaData;

struct CalcCharParaData {
    short maxHp;                /* +0x00: calcTotalParaMenuSub's clamp ceiling */
    short maxEp;                 /* +0x02: calcTotalParaMenuSub's clamp ceiling */
    unsigned char unmodeled_04[0x34 - 0x04];
    short hp;                  /* +0x34 */
    short ep;                  /* +0x36 */
    unsigned char unmodeled_38[0x54 - 0x38];
    short agwsId;               /* +0x54 */
    unsigned char unmodeled_56[0x5A - 0x56];
    signed char hand[3];         /* +0x5A */
    unsigned char unmodeled_5d[0x5E - 0x5D];
    short weaponId[3];          /* +0x5E */
    short accessoryId[3];       /* +0x64 */
    short attachmentId[3];      /* +0x6A */
    unsigned char unmodeled_70[0x82 - 0x70];
    short specialId[6];         /* +0x82 */
    short etherId[12];          /* +0x8E */
    short skillId[3];            /* +0xA6 */
};

CalcCharParaData *calcTotalParaMenu(int charaId, int *attack, int *defense);

/*
 * dataEthGet's (src/ov01/data_unit_org_get.c, still asm) return record, as far as
 * calcTakeEpGetMenu (this TU) reads it. src/main/menu_ether.c evidences the
 * same tecId member under its own bounded view of the same record.
 */
typedef struct CalcEtherStatus {
    unsigned char unmodeled_00[6];
    short tecId; /* +6: the technique dataTecGet resolves for this ether */
} CalcEtherStatus;

/*
 * dataAccGet's (src/ov01/data_unit_org_get.c, still asm) return record, as far as
 * calcMagDefGet (this TU) reads it.
 */
typedef struct AccessoryData {
    unsigned char unmodeled_00[0xD];
    unsigned char magicDefense; /* +0xD: calcMagDefGet's accumulated term */
} AccessoryData;

/*
 * calcBoost's (this TU, 0x00a128f8) own bounded view of the action-stock
 * record actStockGet (src/ov01/battle_init.c, declared there as `int *`)
 * returns: `sw <unit>,0x10(<actStockGet()+mode*4>)` stores the boosting
 * unit at this per-mode slot. battle_init.c owns the record and its
 * leading counter.
 */
typedef struct CalcActStock {
    unsigned char unmodeled_00[0x10];
    ObjectTask *slot[2]; /* +0x10: the boosting actor for mode 0/1 */
} CalcActStock;

#endif /* SRC_OV01_CALC_H */
