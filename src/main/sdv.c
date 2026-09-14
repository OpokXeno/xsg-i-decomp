#include "common.h"
#include "sdv.h"

extern void *memset(void *, int, unsigned int);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvInitAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSaveAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSetAmbStateSub);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSetAmbState);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSetAmbState2);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSetAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvRestoreAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecSeqTbl);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvProgressKey);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvPlaySound);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvScheduleSound);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSelectCamera);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSetCameraParam);

static void sdvSetCameraOffset(SdvCamOffset *cam, int kind,
                               const int16_t *vec)
{
    int cam_kind = kind;
    const int16_t *cam_vec = vec;

    memset(cam, 0, sizeof(*cam));
    cam->kind = cam_kind;

    if (cam_kind == 1) {
        cam->pos[0] = (float)cam_vec[0] * 0.01f;
        cam->pos[1] = (float)cam_vec[1] * 0.01f;
        cam->pos[2] = (float)cam_vec[2] * 0.01f;
    } else if (cam_kind == 2) {
        cam->ang[0] = (float)cam_vec[0];
        cam->ang[1] = (float)cam_vec[1];
        cam->ang[2] = (float)cam_vec[2];
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvProgressPrm);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvScheduleCamera);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvTransOffset);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvInitSpecialWork);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvClearSpecialWork);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvAllocSpecialWork);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecSpecial);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawSpecial);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvScheduleAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvInitAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvCreateAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDestroyAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvKillAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvInitAlters);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDestroyAlters);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecAlters);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawAlters);
