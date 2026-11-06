/*
 * TU-local declarations of ov01/tu004 (src/ov01/calc.c). The shared
 * records (CalcUnitParam, Actor, the flag enums) live in
 * include/ov01/calc.h and include/ov01/battle_init.h.
 */

#ifndef SRC_OV01_CALC_H
#define SRC_OV01_CALC_H

#include "ov01/calc.h"

/*
 * calcUPGet (this TU, 0x00a11010): `lw $3,16($4)` loads
 * unit->work and `lw $2,132($3)` returns the CalcUnitParam pointer at
 * Actor+0x84 (include/ov01/battle_init.h).
 */

/*
 * A player character's persistent 0x180-byte-strided origin record;
 * dataUnitOrgGet (src/ov01/data_unit_org_get.c)
 * returns the one for charaId, and calcTotalParaMenu (this TU)
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
    /*
     * +0x04..+0x0D: calcLvUp adds each level's gain to these, clamped (4
     * halfwords at 999, 2 bytes at 99); the names are guesses from that use.
     * +0x14: the level, the calcLvUp loop index into dataExpTblGet's table
     * and its cap of 99.
     */
    u16 statSum[4];             /* +0x04 */
    signed char gainSum[2];     /* +0x0C */
    unsigned char unmodeled_0e[0x14 - 0x0E];
    signed char level;          /* +0x14 */
    unsigned char unmodeled_15[0x1C - 0x15];
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
 * dataEthGet's (src/ov01/data_unit_org_get.c) return record, as far as
 * calcTakeEpGetMenu (this TU) reads it. src/main/menu_ether.c evidences the
 * same tecId member under its own bounded view of the same record.
 */
typedef struct CalcEtherStatus {
    unsigned char unmodeled_00[6];
    short tecId; /* +6: the technique dataTecGet resolves for this ether */
} CalcEtherStatus;

/*
 * dataAccGet's (src/ov01/data_unit_org_get.c) return record, as far as
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
    BattleUnit *slot[2]; /* +0x10: the boosting unit for mode 0/1 */
} CalcActStock;

#endif /* SRC_OV01_CALC_H */
