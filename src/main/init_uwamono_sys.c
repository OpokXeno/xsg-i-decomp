#include "common.h"
#include "shared.h"

/*
 * UwamonoMapUnit is this TU's local view of the same MapUnit[] record that
 * src/main/init_drill.c also touches; only the fields SendBrokenSignal,
 * GetUwamonoSignal and CreateUwamonoCommon read or write are named, and the
 * rest is unmodeled padding recovered as its exact byte span so the offsets
 * below stay exact.
 */
typedef struct UwamonoMapUnit {
    u32 flags;                        /* +0x00 */
    unsigned char unmodeled_04[0x20];
    /*
     * MAP_updateUnitSymbol (VA 0x002bfbe0) adds the scaffold-owned .lit4
     * constant D_004D7EBC (value 0.05) to this field unconditionally
     * (`lwc1`/`add.s`/`swc1`, no branch in the original); no other role is
     * evidenced here.
     */
    float symbolPhase;                /* +0x24 */
    unsigned char unmodeled_28[0x79];
    signed char actionNo;             /* +0xA1 */
    signed char actionSub;            /* +0xA2 */
    unsigned char unmodeled_a3[5];
    short sequenceNo;                 /* +0xA8 */
    short sequenceSub;                /* +0xAA */
    unsigned char unmodeled_ac[0xf8];
    signed char signal;               /* +0x1A4 */
} UwamonoMapUnit;

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitUwamonoSys);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", Unit_CreateUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_LoadUwamonoResource);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GetPartsPos);

void SendBrokenSignal(UwamonoMapUnit *unit)
{
    unit->signal = 2;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GetPartsSize);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", ClearUwamonoEffect);

signed char GetUwamonoSignal(UwamonoMapUnit *unit)
{
    return unit->signal;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitMapParts);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitMapKoware);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitSaveSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitShopSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitEvsSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitRetSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SetHideObject);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GameStateRestoreKoware);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SetMapUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", ResetShootSys);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CreateUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CreateKowaremono);

void CreateUwamonoCommon(UwamonoMapUnit *unit)
{
    unit->actionNo = 0;
    unit->flags |= 0x10000;
    unit->sequenceNo = 0;
    unit->sequenceSub = 0;
    unit->actionSub = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitKoware);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitEnemy);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitHide);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitSaveSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitShopSymbol);

extern const float D_004D7EBC;

void MAP_updateUnitSymbol(UwamonoMapUnit *unit)
{
    unit->symbolPhase = unit->symbolPhase + D_004D7EBC;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SetUwaWind);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoBrokenSe);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawActiveCursol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitWithNyuru);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitReverse);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitPos);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitPosSize);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitPosReverse);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitAt);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitPosAt);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxCorner);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckCorner);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxTest);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckNyuru);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruCircle);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruMatrix);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruCorner);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", ShootMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossPointUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossPointUwamonoAt);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CalcNearCrossPointCircle);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CalcNearCrossPointBox);

/*
 * CheckNearPoint (VA 0x002c29d8, main/tu182) -- re-treated 2026-09-12 from the
 * accepted ee-asm record config/units/math-spark-cont01-002c29d8.json as C
 * with constrained ee-vu-cop2 inline assembly.
 * The EE has no scalar instruction for the aligned XYZ vector subtract, so each
 * of the two blocks below is the hardware sequence the original executes
 * (lqc2/lqc2/vsub.xyz/sqc2 into a reused stack scratch vector); everything
 * around it -- the parameters, the two xglVectorLength calls, the comparison
 * and the selected-vector copy -- is ordinary C. See notes.md for the
 * per-instruction correspondence with CheckNearPoint.s.
 */
void CheckNearPoint(const Vector4 *origin, const Vector4 *candA, const Vector4 *candB, Vector4 *out)
{
    Vector4 delta;
    float lenA;
    float lenB;

    __asm__ __volatile__(
        "lqc2 vf3,0(%1)\n\t"
        "lqc2 vf2,0(%0)\n\t"
        "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
        "sqc2 vf2,0(%2)"
        :
        : "r"(origin), "r"(candA), "r"(&delta)
        : "memory"
    );
    xglVectorLength(&lenA, &delta);

    __asm__ __volatile__(
        "lqc2 vf3,0(%1)\n\t"
        "lqc2 vf2,0(%0)\n\t"
        "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
        "sqc2 vf2,0(%2)"
        :
        : "r"(origin), "r"(candB), "r"(&delta)
        : "memory"
    );
    xglVectorLength(&lenB, &delta);

    if (lenA < lenB)
        *out = *candA;
    else
        *out = *candB;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoAreaCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AreaCheckBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitAim);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckActor);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitAt);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitCircle);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UnlockMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckNearMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckBrokenMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AimHeightCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AimMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AimMapUnitLookCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckCornerDist);

/*
 * *(UwamonoLinkedUnit.position below) is a separate record UwamonoCommonFunc
 * keeps synchronized with this unit's position; only the field it writes is
 * named.
 */
typedef struct UwamonoLinkedUnit {
    unsigned char unmodeled_00[0x30];
    Vector4 position; /* +0x30, set from the owning unit's position */
} UwamonoLinkedUnit;

/*
 * UwamonoCommonFunc's own additive view of the same MapUnit[] record
 * UwamonoMapUnit above already partially names (src/main/init_drill.c keeps
 * a third view, DrillMapUnit); only the fields UwamonoCommonFunc reads or
 * writes that UwamonoMapUnit does not already name are named here: position
 * (+0x10, read to sync linkedUnit's position); linkedUnit (+0x80, the record
 * kept in sync); serial (+0xA4, an lh compared against 0x1000 like
 * DrillMapUnit's own +0xA4 serial); and the timers block (+0x1A0), whose
 * heightCheckFlag (+0x1CE) and bgmTimer (+0x1E8) fields the original reaches
 * through a second pointer computed once, ahead of the serial branch, and
 * kept live (register evidence) across the CheckUwamonoHeight call.
 */
typedef struct UwamonoTimers {
    unsigned char unmodeled_00[0x2e];
    signed char heightCheckFlag; /* +0x2E (record +0x1CE) */
    unsigned char unmodeled_2f[0x19];
    int bgmTimer;                /* +0x48 (record +0x1E8) */
} UwamonoTimers;

typedef struct UwamonoCommonUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;                 /* +0x10 */
    unsigned char unmodeled_20[0x60];
    UwamonoLinkedUnit *linkedUnit;     /* +0x80 */
    unsigned char unmodeled_84[0x20];
    short serial;                     /* +0xA4 */
    unsigned char unmodeled_a6[0xfa];
    UwamonoTimers timers;              /* +0x1A0 */
} UwamonoCommonUnit;

/* Defined in src/main/map_create_unit_peer.c (main/tu270), still in asm. */
extern void MAP_updateUnitPartsSequence(void *unit);
extern void MAP_updateUnitSequence(void *unit);

/* Defined later in this TU (a local sibling still in asm). */
void CheckUwamonoHeight(UwamonoCommonUnit *unit);
/* Defined later in this TU (a local sibling still in asm). */
void UwamonoBgmFunc(UwamonoCommonUnit *unit);

void UwamonoCommonFunc(UwamonoCommonUnit *unit)
{
    UwamonoTimers *timers = &unit->timers;

    if (unit->serial < 0x1000) {
        MAP_updateUnitPartsSequence(unit);
    } else {
        MAP_updateUnitSequence(unit);
    }

    if (timers->heightCheckFlag != 0) {
        CheckUwamonoHeight(unit);
    }

    unit->linkedUnit->position.x = unit->position.x;
    unit->linkedUnit->position.y = unit->position.y;
    unit->linkedUnit->position.z = unit->position.z;

    if (timers->bgmTimer > 0) {
        UwamonoBgmFunc(unit);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoBgmFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoBgmFadeOut);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckUwamonoHeight);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckUwamonoAt);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawRadar);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawSprite);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SortLine);
