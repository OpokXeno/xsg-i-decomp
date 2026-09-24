#ifndef INCLUDE_OV01_BATTLE_INIT_H
#define INCLUDE_OV01_BATTLE_INIT_H

#include "shared.h"

typedef struct Actor Actor;

/*
 * The unit record scenarioMtd, transWepIn/transWepOut and spWepDispOn
 * (0x00a0e550, still INCLUDE_ASM here) all share: spWepDispOn receives the
 * same pointer scenarioMtd receives, dereferences it to reach this record,
 * then indexes its +0x1C table by (weapon-slot & 3) and reads +0x14, exactly
 * as transWepIn/transWepOut do on their own argument and scenarioMtd does on
 * *unitHandle. Bytes between the evidenced members are untouched here.
 */
typedef struct BattleUnit BattleUnit;

struct BattleUnit {
    unsigned char unmodeled_00[0x10];
    Actor *work;       /* +0x10 */
    Actor *actor;      /* +0x14 */
    unsigned char unmodeled_18[0x1C - 0x18];
    Actor *weapon[4];  /* +0x1C */
};

/*
 * Show (transWepIn) or hide (transWepOut) the weapon actor in the unit's
 * slot `slot & 3` by installing its fade draw callback.
 */
void transWepIn(BattleUnit *unit, int slot);

void transWepOut(BattleUnit *unit, int slot);

/*
 * manWorkGet's return type. Only the two fields menuKeyGuideObjDraw (src/
 * ov01/menu.c, ov01/tu009) consumes are evidenced there: unitTask at +0x10
 * and controlPhase at +0xc. This TU's own code only takes the record's
 * address (data still asm-owned; original ELF symbol name).
 */
typedef struct ManWork {
    unsigned char unmodeled_00[0xc];
    int controlPhase;     /* +0xc */
    ObjectTask *unitTask; /* +0x10 */
} ManWork;

/*
 * Each battle unit's own large parameter block, reached only through the
 * per-unit table below; nothing else in this TU dereferences it.
 */
typedef struct UnitWork {
    unsigned char unmodeled_00[0xA8C];
    unsigned int flags; /* +0xA8C: UNIT_WORK_STATUS_BLOCKED suppresses
                          * status-effect ticks for this unit */
} UnitWork;

ManWork *manWorkGet(void);

#endif /* INCLUDE_OV01_BATTLE_INIT_H */
