/*
 * OV12 original TU 23: 0x00a15600..0x00a17c90 (63 functions)
 */
#include "common.h"
#include "rg_shot.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CommonGetGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GetShotPos2);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GetShotPosDir);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTimeGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTimeLife);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTimeEffect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonDestruct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonHit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonIsReleased);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitShotCommonSub);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitShotCommon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitRgShotEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CreateRgNormalShot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgNormalShotEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _HomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitHoming);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CreateHoming);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgHomingShotEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeEntryHistory);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeBomPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeShotZeroLife);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHomingHit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeBomHit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHitRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHitBg);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeSetBom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitGrenade);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CreateGrenade);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgGrenadeEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FireKill);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FirePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FireIsReleased);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FireRelease);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FireHit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FireHitRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitFire);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CreateFire);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgFireEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _BeamPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _BeamIsReleased);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _BeamHit);

/*
 * Partial view of a beam object.  The callback's only evidenced state is the
 * lifetime value at byte offset 0x98; the surrounding object layout remains
 * outside this allocation.
 */

static void _BeamRelease(void *beam)
{
    BeamLifetimeField *lifetime_field =
        (BeamLifetimeField *)((unsigned char *)beam + 0x98);

    lifetime_field->lifetime = -0.5f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitBeam);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CreateBeam);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgBeamEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", CreateRgShotFromEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotSetSoundDriver);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotSetHitSound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotSetBgHitSound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotSetNoLifeSound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotSetHideSound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotGetDamage);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotIsReleased);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotGetHitRobNum);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotRelease);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotHitRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotHitBg);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonHitRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonHitBg);
