/*
 * TU-local declarations of ov01/tu004 (src/ov01/calc.c).
 */

#ifndef SRC_OV01_CALC_H
#define SRC_OV01_CALC_H

#include "shared.h"

enum CalcTechniqueCount { CALC_NORMAL_TECHNIQUE_COUNT = 6 };

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
 * - weaponId (+0x5E/+0x60/+0x62): the per-slot weapon ids. calcDblActChk
 *   (this TU, 0x00a16898) indexes them with calcSameWpnChk's slot and passes
 *   the id to dataWpnGet and calcAttWpnChk; CalcCharParaData keeps its own
 *   weaponId at the same offset. unitLoad hands all three to sefSetupPlayer
 *   as its second through fourth arguments (lh 0x5e/0x60/0x62 at
 *   0x00a018d8..0x00a018e8), which stores them next to the id in its
 *   _battlePrm slot (sh at +0x10/+0x12/+0x14, 0x002e6d84..0x002e6d8c).
 * - dblActKind (+0x3A): a signed byte calcDblActChk loads with
 *   `lb $2,0x3A($17)` and requires to be CALC_DBL_ACT_KIND.
 * - wpnSlotValue (+0x70/+0x72/+0x74): a halfword per weapon slot that
 *   calcDblActChk (`lh $3,0x70($3)` at slot * 2) requires to be positive
 *   when calcAttWpnChk holds for that slot's weapon.
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
    short maxHp;                /* +0x00: calcIdol2Chk's threshold base */
    short maxEp;                /* +0x02: calcEp's clamp ceiling */
    short baseAtk;                /* +0x04: calcWpnAtkGet override term */
    /*
     * +0x06/+0x08/+0x0A: three halfwords calcSkillInitSub (this TU) raises
     * and clamps to 999 (`lhu $2,0x6($6)`, `lhu $2,0x8($6)`, `lhu $2,0xA($6)`,
     * 0x00a11dd8/0x00a11e74/0x00a11ea8), under the SkillParam bits 0x4000,
     * 0x800 and 0x400. +0x06/+0x0A are the phyDefense/magDefense pair that
     * src/main/menu_para_pt_rate_get.c names on the same record.
     */
    short phyDefense;
    short magAttack;
    short magDefense;
    unsigned char hitStat;      /* +0x0C: calcHitParaGet's own signed view */
    /*
     * +0x0D: calcHitRate (this TU, 0x00a13010) loads the target's byte with
     * `lbu $3,0xD($2)` and reinterprets it as signed (`sll $18,$3,24`/
     * `sra $16,$18,24`) as the evasion term it subtracts from the hit rate.
     */
    unsigned char evadeStat;
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
    unsigned char unmodeled_14[0x15 - 0x14];
    signed char sizeClass;          /* +0x15: cmdPartySizeGet size byte */
    signed char counterRate;    /* +0x16: calcCounter's base counter chance */
    unsigned char unmodeled_17[0x18 - 0x17];
    u16 conditions;                /* +0x18: status conditions word */
    unsigned char unmodeled_1a[0x1C - 0x1A];
    u16 flags;               /* +0x1C */
    unsigned char unmodeled_1e[0x24 - 0x1E];
    u16 statProtect[1];      /* +0x24: calcStatProtectGet/Set's group word */
    unsigned char unmodeled_26[0x34 - 0x26];
    u16 currentHp;           /* +0x34: calcPara2OrgSub's copy source */
    u16 currentEp;           /* +0x36: calcPara2OrgSub's copy source */
    short charaId;           /* +0x38 */
    signed char dblActKind;     /* +0x3A: calcDblActChk */
    signed char bcLimit;        /* +0x3B: calcBc's own clamp ceiling */
    signed char bcValue;        /* +0x3C: calcBc's own running counter */
    signed char bpGauge;        /* +0x3D: calcBp's `lbu $2,0x3D($16)`/`sb`,
                                 * the promoted sum is compared as signed against 50 (`slti`) and reduced with a signed modulo, which is what the field's signed type reproduces */
    short turnBias;             /* +0x3E: calcWct/calcTurnEnd */
    short waitCount;            /* +0x40: calcTurnEnd/calcWctDec */
    short waitCountBase;        /* +0x42: calcTurnEnd */
    short posId;                /* +0x44: calcLineChk/calcMoveChk/calcA2Chk's
                                 * `lh $x,0x44($2)`, fed to dataPosLineGet
                                 * (src/ov01/data_unit_org_get.c). */
    unsigned char unmodeled_46[0x50 - 0x46];
    CalcUnitParam *linkedUnit;   /* +0x50: menuStatChrIdGet reads its charaId */
    short baseCharaId;             /* +0x54: unit character id when flags&0x40 clear */
    unsigned char unmodeled_56[0x5E - 0x56];
    short weaponId[3];       /* +0x5E: per-slot weapon ids */
    short accessoryId[3];    /* +0x64: calcMagDefGet's per-slot item ids */
    short attachmentId[3];   /* +0x6A: calcWpnEnableChk's `lh $3,0x6A($3)`,
                              * mirroring CalcCharParaData's attachmentId */
    short wpnSlotValue[3];   /* +0x70: calcDblActChk, per weapon slot */
    short normalTechniqueId[CALC_NORMAL_TECHNIQUE_COUNT]; /* +0x76 */
    short slotTechniqueId[2];   /* +0x82: calcSlotAChk's own pair, right
                                 * after normalTechniqueId. */
    unsigned char unmodeled_86[0x8E - 0x86];
    short etherId[12];           /* +0x8E: calcEthEnableChk's `lh $3,0xE($2)`
                                  * from calcUPGet(unit) + 0x80 + 2 * slot,
                                  * mirroring CalcCharParaData's etherId */
    short skillId[3];            /* +0xA6: calcSkillEquChk's `lh $3,0x6($2)`,
                                  * mirroring CalcCharParaData's skillId at
                                  * the same offset. */
    u16 statActiveMask[8];   /* +0xAC: calcStatTurn's per-category bit set */
    /*
     * +0xBC: calcStatReset (this TU, 0x00a17918) clears this per-category
     * word (AND ~mask) instead of statActiveMask when its fourth argument
     * is nonzero; every known caller passes 0 there, so only the clear
     * itself is evidenced.
     */
    u16 statAltMask[8];      /* +0xBC */
    unsigned char statTurnCount[8][16]; /* +0xCC: calcStatTurn's countdown grid */
    u16 etherDisableFlags;   /* +0x14C: calcEtherDisableGet/Set's bit set */
    unsigned char unmodeled_14e[0x158 - 0x14E];
    struct UnitWork *effectUnit;   /* +0x158: effect object, same kind as statEffUnits */
    struct UnitWork *statEffUnits[8]; /* +0x15C: statEffOn/statEffOff */
};

/*
 * Bits of CalcUnitParam.flags (+0x1C) that recovered code tests, every one
 * an `lhu $3,0x1C($2)`/`andi $3,$3,<bit>` on calcUPGet's result. Enum
 * constants, not macros: header_harvest.py can never publish a macro, and it
 * publishes an enum only when another TU's declaration needs it (as
 * CALC_NORMAL_TECHNIQUE_COUNT above is carried by the array bound in
 * CalcUnitParam). Nothing declares these, so today they are visible only to
 * the TUs that include this header (calc.c); the other TUs read the bit as a
 * literal beside a comment that names the enumerator. The field itself stays
 * u16. Each name states the effect the test has in the function cited, no
 * more; a bit whose effect is not established is named by its position.
 */
enum CalcUnitFlag {
    /* calcHitRate alone (0x00a130f0, on the target): set, it blocks the
     * result 2 of the first roll. */
    CALC_UNIT_FLAG_NO_HIT_RESULT_2 = 0x1,
    /* calcHitRate alone (0x00a1314c, on the target): set, it blocks the
     * result 1 of the second roll. */
    CALC_UNIT_FLAG_NO_HIT_RESULT_1 = 0x2,
    /* calcStatSetChk alone (`andi $2,$4,0x8`, 0x00a17394): with BIT12 set,
     * it lets status category 5 through. Role not established. */
    CALC_UNIT_FLAG_BIT3 = 0x8,
    /* The unit is an AGWS. src/ov01/data_unit_org_get.h already spells this
     * bit CALC_UNIT_FLAG_AGWS, as a macro; this enumerator is spelled
     * differently because that header is not visible to calc.c and a macro of
     * the same name would rewrite an enumerator once it is published. Unify the
     * two when the declaration is published. The evidence, by the original
     * names of what the bit selects:
     * keyBuffSet (ov01/battle_init, 0x00a0b1c0) reads keyItemTblAgws instead
     * of keyItemTblCha when it is set, and unitCmdAgws (ov01/unit_cmd,
     * 0x00a027fc..0x00a02804) takes the character id from linkedUnit instead
     * of baseCharaId; calcPara2Org and calcSkillEquChkAll (this TU) also
     * redirect to linkedUnit on it. Tested the same way by calcWpnAtkGet
     * (0x00a13478), calcPowLevGet (0x00a136b8), calcTgtUnitTbl
     * (0x00a16d50, 0x00a16dfc), calcPlusAglGet, futtobiChk, scenarioCam
     * (0x00a0ba9c) and sndConvSe. */
    CALC_UNIT_FLAG_IS_AGWS = 0x40,
    /* calcAtkChk alone (`lhu $3,0x1C($2)`/`andi $3,$3,0x100`,
     * 0x00a1603c..0x00a16040, on the target): set, a range-class-0
     * technique cannot attack it. */
    CALC_UNIT_FLAG_NO_RANGE0_ATTACK = 0x100,
    /* calcBackLineChk alone (0x00a16370, selector 0, on a unit standing
     * further back): set, the check fails. */
    CALC_UNIT_FLAG_BLOCKS_BACK_LINE = 0x800,
    /* Tested on the target by calcDmgItem (`andi $3,$3,0x200`, 0x00a159ac):
     * set, a recoverHp item's amount changes sign; and by calcHitItem
     * (`andi $3,$3,0x200`, 0x00a15830): set, the recoverHp item's hit
     * result 2 is reversed through calcRevHit. */
    CALC_UNIT_FLAG_ITEM_REVERSED = 0x200,
    /* Two readers, both on the target: calcStatSetChk (`andi $2,$4,0x1000`,
     * 0x00a17380): set, status categories 0..3 are refused, clear, 4..6 are;
     * calcDmgItem: set, a recoverHp item whose useFlags bit 1 is set has its
     * amount zeroed. The flag's full role is not established. */
    CALC_UNIT_FLAG_BIT12 = 0x1000
};

/*
 * Bits of the target-selection byte that dataItmGet's record carries at
 * +0x08 (an unsigned `lbu`; calc.c's ItmTrgView.trgFlags, battle_init.c's
 * ItmTrgView), which calcTgtUnitTbl and calcDeadUnitNum take as an int, and
 * that cmdAtktbl, calcTgtUnitTbl and calcDeadUnitNum hand to calcTrg2PE.
 * calcTrg2PE's own side selections (0x40 and 0x80, compared as a pair)
 * are spelled in its body.
 */
enum CalcTrgFlag {
    /* calcTgtUnitTbl alone (`andi $2,$16,0x4` at 0x00a16d38, `andi
     * $23,$16,0x4` at 0x00a16d90): set, units carrying CALC_UNIT_FLAG_IS_AGWS
     * are left out of the table (the unit itself included). */
    CALC_TRG_EXCLUDE_AGWS = 0x4,
    /* The unit itself: calcTgtUnitTbl (0x00a16d04) makes the table that one
     * unit, and calcTrg2PE answers with the unit's own side for any word
     * carrying it. */
    CALC_TRG_SELF = 0x8,
    /* calcDItemChk (this TU) needs it clear; scenarioCam (ov01/battle_init,
     * `ori $2,$3,0x10` at 0x00a0baf0) adds it to the item's word in the
     * camera event when the exec record's tecId is nonzero. Role not
     * established. */
    CALC_TRG_BIT4 = 0x10,
    /* calcTgtUnitTbl alone (`andi $22,$16,0x20` at 0x00a16d94): set, the
     * table takes the units carrying CALC_ACTOR_FLAG_DEAD instead of the
     * ones without it. */
    CALC_TRG_DEAD_UNITS = 0x20
};

/*
 * The charaId range src/ov01/data_unit_org_get.h names
 * SPECIAL_MODEL_CHARACTER_ID_BEGIN/END (macros, not visible to calc.c):
 * futtobiChk (`slti $3,$3,0xBB` at 0x00a14648, `slti $3,$3,0xC3` at
 * 0x00a14660) and calcLinePos (the same pair at 0x00a154e0/0x00a154f8), both
 * this TU, compare CalcUnitParam.charaId against it, as do the still-asm
 * calcWpnAtkGet (0x00a13490/0x00a134a8), calcPowLevGet (0x00a136d0/
 * 0x00a136e8) and calcPlusAglGet.
 * The CALC_ spelling avoids the macro collision, as for the flag enums above.
 */
enum CalcSpecialModelCharacterId {
    CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN = 0xBB,
    CALC_SPECIAL_MODEL_CHARACTER_ID_END = 0xC3
};

/* calcUPGet follows ObjectTask.work at +0x10 to the battle actor's up
 * member at +0x84 (0x00A11010..0x00A1101C). calcTurnStart/End use its
 * flags at +0x00. The shared head is also the base of rendered actors,
 * while calc's scratch actor retains only this bounded 0x88-byte prefix. */
/* ACT_create's actor pool supplies both ObjectTask.work and the unit's
 * separate motion actor. calcUPGet reaches up through work at +0x84;
 * dataUnitFileLoadMdl reaches the model pointers through motionActor.
 * Only fields used by these consumers are named. */
typedef struct BattleActor {
    int flags;
    void (*update)(struct BattleActor *self);
    void (*draw)(struct BattleActor *self);
    unsigned char unmodeled_0c[0x84 - 0x0C];
    struct CalcUnitParam *up; /* +0x84 */
} BattleActor;

typedef BattleActor CalcActorRecord;

/*
 * Bits of BattleActor.flags (+0x00 of ObjectTask.work) that this TU's
 * recovered code tests (an `lw` of the word, then an `andi` with the bit;
 * calcWpnAtkGet: `lw $2,0x0($3)`/`andi $2,$2,0x40`). The spelling
 * is CALC_ACTOR_* and not the ACTOR_FLAG_ENEMY of src/ov01/unit_cmd.h or
 * the BATTLE_ACTOR_FLAG_ENEMY of src/ov01/data_unit_org_get.h: those are
 * macros, and data_unit_org_get.c includes its own header before
 * ov01/calc.h, so an enumerator of the same name would be rewritten by the
 * macro once this enum reaches the generated header.
 */
enum CalcActorFlag {
    /* Cleared on the target when calcBattleChk refuses it, by calcStatClr
     * (`addiu $3,$0,-0x11`/`and` at 0x00a14d00..0x00a14d08) and by calcStat
     * for a mode-1 hit (`addiu $4,$0,-0x11`/`and` at 0x00a148f8..0x00a14904).
     * Both uses only clear it, so its role is not established. */
    CALC_ACTOR_FLAG_BIT4 = 0x10,
    /* The enemy bit src/ov01/unit_cmd.h evidences (unitLoad picks
     * sefSetupEnemy on it); tested here by calcWpnAtkGet (0x00a13460),
     * calcPowLevGet (0x00a136a0), calcBackLineChk (0x00a162d8) and the
     * older functions that spell 0x40. */
    CALC_ACTOR_FLAG_ENEMY = 0x40,
    /* Set by unitCmdDead (ov01/unit_cmd, `ori $2,$3,0x80`/`sw $2,0x0($17)`
     * at 0x00a0307c..0x00a03080 and `ori $2,$2,0x80`/`sw $2,0x0($17)` at
     * 0x00a03270..0x00a03274, $17 the work record) and by unitCmdEscape (0x00a06cfc..0x00a06d00); counted by
     * calcDeadUnitNum (0x00a16eac), skipped by calcBackLineChk
     * (0x00a1631c) and selected or skipped by calcTgtUnitTbl
     * (0x00a16dc0, 0x00a16ddc). */
    CALC_ACTOR_FLAG_DEAD = 0x80,
    /* calcHitRate (0x00a130a4, on the target: result 1 outright),
     * calcTurnStart (queues selector 0xD when no command is pending) and
     * unitMotStandSet (ov01/unit_cmd, motion 0x1D) test it; what it marks
     * is not established. */
    CALC_ACTOR_FLAG_BIT11 = 0x800
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
 * - flags (+0x1C): a halfword bit set; calcEngineEquipOrg/calcFrameEquipOrg
 *   (this TU, 0x00a16818/0x00a16858) both test bit 0x40 with
 *   `lhu $3,28($4)`/`andi $3,$3,0x40` and refuse to write engineId/frameId
 *   when it is clear.
 * - hp/ep (+0x34/+0x36): calcPara2OrgSub's copy destination for
 *   CalcUnitParam's currentHp/currentEp.
 * - agwsId (+0x54): calcAgwsEquipOrg's single equipped-AGWS slot.
 * - engineId (+0x56): calcEngineEquipOrg's equipped engine slot.
 * - frameId (+0x58): calcFrameEquipOrg's equipped frame slot.
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
    unsigned char unmodeled_04[0x1C - 0x04];
    u16 flags;                /* +0x1C: calcEngineEquipOrg/calcFrameEquipOrg's gate bit */
    unsigned char unmodeled_1e[0x34 - 0x1E];
    short hp;                  /* +0x34 */
    short ep;                  /* +0x36 */
    unsigned char unmodeled_38[0x54 - 0x38];
    short agwsId;               /* +0x54 */
    short engineId;              /* +0x56: calcEngineEquipOrg's equipped slot */
    short frameId;               /* +0x58: calcFrameEquipOrg's equipped slot */
    signed char hand[3];         /* +0x5A */
    unsigned char unmodeled_5d[0x5E - 0x5D];
    short weaponId[3];          /* +0x5E */
    short accessoryId[3];       /* +0x64 */
    short attachmentId[3];      /* +0x6A */
    unsigned char unmodeled_70[0x82 - 0x70];
    short specialId[6];         /* +0x82 */
    short etherId[12];          /* +0x8E */
    short skillId[3];            /* +0xA6 */
    unsigned char unmodeled_ac[0xBC - 0xAC];
    /*
     * +0xBC: CalcUnitParam.statAltMask's counterpart. calcUseItemMenu (this
     * TU) saves and restores eight halfwords from +0xBC of the origin record
     * it hands to calcUseItem as a unit parameter block (`addiu $5,$16,0xBC`/
     * `lhu $3,0x0($5)`, 0x00a11574..0x00a11580), and copies calcTotalParaMenu's
     * recalculated eight over them in between.
     */
    u16 statAltMask[8];
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
 * calcMagDefGet, calcSkillEquChk and calcPhyDefGet (this TU) read it.
 */
typedef struct AccessoryData {
    unsigned char unmodeled_00[0xC];
    unsigned char phyDefense;   /* +0xC: calcPhyDefGet's accumulated term */
    unsigned char magicDefense; /* +0xD: calcMagDefGet's accumulated term */
    u16 skillId;                 /* +0xE: calcSkillEquChk's `lhu $3,0xE($2)` */
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
