#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitUwamonoSys);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", Unit_CreateUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_LoadUwamonoResource);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GetPartsPos);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SendBrokenSignal);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GetPartsSize);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", ClearUwamonoEffect);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GetUwamonoSignal);

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

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CreateUwamonoCommon);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitKoware);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitEnemy);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitHide);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitSaveSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitShopSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitSymbol);

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

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoCommonFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoBgmFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoBgmFadeOut);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckUwamonoHeight);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckUwamonoAt);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawRadar);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawSprite);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SortLine);
