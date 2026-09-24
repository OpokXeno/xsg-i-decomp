#include "common.h"
#include "sdv.h"

extern void *memset(void *, int, unsigned int);

void sdvInitAmbient(void)
{
    _sdvAmbFrame = 0;
    _sdvAmbState = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvSaveAmbient);

static void sdvSetAmbStateSub(int state, int effect_no, int force)
{
    if (force != 0) {
        _sdvAmbState = state;
        return;
    }

    srsAnalyzeEftNo(effect_no, &charID_0, &eftCate_1);
    if (eftCate_1 == 0xE || effect_no == 0xB25) {
        if ((unsigned int)(effect_no - 0x8FC) >= 0xB6) {
            _sdvAmbState = state;
        }
    }
}

void sdvSetAmbState(int state, int effect_no)
{
    sdvSetAmbStateSub(state, effect_no, 0);
}

void sdvSetAmbState2(int state, int effect_no)
{
    sdvSetAmbStateSub(state, effect_no, 1);
}

void sdvSetAmbient(void *map_rgb, void *ambient)
{
    xglLightIntensityAmbient(xglStudioGetLight2(), ambient);
    func_A2C3D8(map_rgb);
}

void sdvRestoreAmbient(void)
{
    sdvSetAmbient(_sdvMapRgb, _sdvAmbient);
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecSeqTbl);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvProgressKey);

void sdvPlaySound(int sound_id, int unused, int flags)
{
    xglSoundEffectNormalID(sound_id, 0);
}

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

void sdvScheduleCamera(SdvCameraTask *task)
{
    float params[44];

    if (_sefBattleMode != 0 && task != 0 && task->active != 0) {
        if (task->pos.active != 0) {
            sdvProgressPrm(0, &task->pos, 0xC);
        }
        if (task->angle.active != 0) {
            sdvProgressPrm(1, &task->angle, 0xC);
        }
        if (task->scale.active != 0) {
            sdvProgressPrm(3, &task->scale, 4);
        } else {
            memset(params, 0, sizeof(params));
            func_A31920(2, params);
            params[41] = 40.0f;
            func_A31920(3, params);
        }
        if (task->offset.active != 0) {
            sdvProgressPrm(0x41, &task->offset, 6);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvTransOffset);

void sdvInitSpecialWork(void)
{
    memset(_sdvSpecialBuf, 0, sizeof(_sdvSpecialBuf));
}

void sdvClearSpecialWork(void)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (_sdvSpecialBuf[i] != 0) {
            GameDefocusSet(i, 0, 0);
            _sdvSpecialBuf[i] = 0;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvAllocSpecialWork);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecSpecial);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawSpecial);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvScheduleAlter);

extern void sefMemZero(void *data, unsigned int size);

void sdvInitAlter(SdvAlter *alter)
{
    sefMemZero(alter, sizeof(*alter));
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvCreateAlter);

void sdvDestroyAlter(SdvAlter *alter)
{
    sefMemZero(alter, sizeof(*alter));
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvKillAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvExecAlter);

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawAlter);

void sdvInitAlters(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        sdvInitAlter(&_sdvAlter[i]);
    }
}

void sdvDestroyAlters(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        sdvDestroyAlter(&_sdvAlter[i]);
    }
}

unsigned short sdvExecAlters(void)
{
    int i;
    unsigned short status;

    for (i = 0; i < 16; i++) {
        status = _sdvAlter[i].active;
        if (status != 0) {
            status = sdvExecAlter(&_sdvAlter[i]);
        }
    }
    return status;
}

INCLUDE_ASM("asm/main/nonmatchings/sdv", sdvDrawAlters);
