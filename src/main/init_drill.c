#include "common.h"
#include "shared.h"

/*
 * MapUnit[] entries are game-wide unit records; only the fields this TU
 * touches are named. The rest of the struct is unmodeled padding recovered
 * as its exact byte span so the struct size and member offsets stay exact.
 */
typedef struct DrillMapUnit {
    u32 flags;
    void *update;
    unsigned char unmodeled_08[0x9c];
    u16 serial;
    unsigned char unmodeled_a6[0xfc];
    /* DrillResetFlag tests this value against its cleanup range; other uses
     * of this field have not been identified. */
    u16 drill_reset_value;
    unsigned char unmodeled_1a4[0x15c];
} DrillMapUnit;

extern DrillMapUnit MapUnit[64];

/* Map input supplies the same pad record read by the drill camera. */
typedef struct DrillPadLayout {
    unsigned char unmodeled_00[0x28];
    u16 half_28;
    u16 half_2a;
    unsigned char unmodeled_2c[0x36];
    unsigned char pitchDeadZone;
    unsigned char yawDeadZone;
    unsigned char unmodeled_64[2];
    signed char pitchStick;
    signed char yawStick;
} DrillPadLayout;
extern DrillPadLayout PadData;

typedef union DrillVector {
    Vector4 vector;
    u64 words[2];
} DrillVector;

typedef struct Drill Drill;
typedef struct DrillEffectRef {
    unsigned char unmodeled_000[0x80];
    DrillVector offset;
    unsigned char unmodeled_090[0x634];
    Drill *owner;
    unsigned char unmodeled_6c8[0x3c4];
    u32 flags;
    unsigned char unmodeled_a90[0x0a];
    u8 hidden;
} DrillEffectRef;

typedef struct DrillCameraAngles {
    unsigned char unmodeled_00[0x10];
    float yaw;
    float pitch;
    float roll;
    unsigned char unmodeled_1c[4];
    float shakeOffsetX;
    float shakeOffsetY;
    float shakeOffsetZ;
    unsigned char unmodeled_2c[4];
    float shakeOffsetStepX;
    float shakeOffsetStepY;
    float shakeOffsetStepZ;
    unsigned char unmodeled_3c[4];
    float positionX;
    float positionY;
    float positionZ;
    unsigned char unmodeled_4c[4];
    float shakeVelocityX;
    float shakeVelocityY;
    float shakeVelocityZ;
    unsigned char unmodeled_5c[4];
    float shakeVelocityStepX;
    float shakeVelocityStepY;
    float shakeVelocityStepZ;
} DrillCameraAngles;

typedef struct DrillCamera {
    unsigned char unmodeled_000[4];
    u32 state;
    unsigned char unmodeled_008[0x88];
    DrillCameraAngles angles;
} DrillCamera;

typedef struct DrillSave {
    unsigned char unmodeled_00[4];
    signed char hit;
    unsigned char unmodeled_05[3];
    s16 cameraPreset;
    unsigned char unmodeled_0a[2];
    float xSpeed;
    float fallSpeed;
    float zSpeed;
    float xReturnSpeed;
    float riseSpeed;
    float zReturnSpeed;
    unsigned char unmodeled_24[8];
    signed char returnLimit;
    unsigned char unmodeled_2d;
    signed char kind;
    unsigned char unmodeled_2f[4];
    signed char hitCount;
    unsigned char unmodeled_34[8];
    int returnCount;
    unsigned char unmodeled_40[0x0c];
    DrillEffectRef *effect1;
    unsigned char unmodeled_50[8];
    u32 flags;
} DrillSave;

struct Drill {
    unsigned char unmodeled_000[0x10];
    DrillVector position;
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_02c[0x54];
    float (*matrix)[4];
    unsigned char unmodeled_084[0x1e];
    u8 phase;
    unsigned char unmodeled_0a3[5];
    s16 crashTimer;
    s16 bounceCount;
    unsigned char unmodeled_0ac[0x3c];
    DrillEffectRef *effect3;
    unsigned char unmodeled_0ec[0xb4];
    DrillSave save;
    unsigned char unmodeled_1fc[0x24];
    float cameraYaw;
    float cameraPitch;
    unsigned char unmodeled_228[8];
    DrillEffectRef *effect2;
};

DrillCamera *xglStudioGetActiveCamera(void);
DrillEffectRef *sefCreateEffectCf(int effect_id, DrillVector *position, int flags);
void sefRewindEffectCf(DrillEffectRef *effect);
void xglFontDebugPrintf(int x, int y, const char *fmt, ...);
void xglMatrixStackUnit(void);
void xglMatrixStackRotY(float angle);
void xglMatrixStackSave(float matrix[4][4]);
void xglSoundEffectStopID(int id, int channel);
void xglSoundEffectNormalID(int id, int channel);
void Vibration_Set_Strong(int left, int right, int duration);
void DrillHitCheck(Drill *drill);
void DrillCalcCameraPos(DrillCamera *camera);
void DrillResetCameraPos(Drill *drill, DrillCamera *camera);
void DrillCameraControlFunc(Drill *drill, DrillCamera *camera);
float xglSin(float angle);
float xglCos(float angle);
void *Unit_CreateUwamono(int identifier, int unit_id);
void InitItemSymbol(void *item);
void SetHideObject(void *unit);
int xglFlagsSet1(int bit_offset, int value);

typedef struct DrillItemInfo {
    u16 state;
    u16 value;
} DrillItemInfo;
typedef struct DrillItemUnit {
    unsigned char unmodeled_000[0xa0];
    u8 itemKind;
    unsigned char unmodeled_0a1[0xff];
    DrillItemInfo info;
    unsigned char unmodeled_1a4;
    u8 spawnedItemKind;
    unsigned char unmodeled_1a6[0x15a];
} DrillItemUnit;

const char D_004CA960[] = "SetContainer\n";
const char D_004CAA40[] = "DRILL ZMOVE";
const char D_004CAA50[] = "DRILL ZSTANDBY";
const char D_004CAA60[] = "DRILL XMOVE";
const char D_004CAA70[] = "DRILL CRASH %2d";
const char D_004CAA80[] = "DRILL RETURN";
const char D_004CAA90[] = "drill_stanby";
const char D_004CAAA0[] = "drill_end";
const char D_004CAAB0[] = "CAMERA CONTROL";

INCLUDE_ASM("asm/main/nonmatchings/init_drill", InitDrill);

int CreateContainer(int, int);
int printf(const char *, ...);
void DrillClearContainer(void);

/*
 * Clears every existing drill container, then creates `count` new ones at
 * random serials in 0x7014..0x7027. A creation that fails (a full container
 * slot table) retries once with a fixed serial (0x7014..0x7017) chosen by
 * xglSRand() & 3.
 */
void SetContainer(int count)
{
    printf((char *) D_004CA960);
    DrillClearContainer();
    if (count > 0) {
        do {
            if (CreateContainer(-1, (xglSRand() % 20U) + 0x7014) == 0) {
                switch (xglSRand() & 3) {
                case 0:
                    CreateContainer(-1, 0x7014);
                    break;
                case 1:
                    CreateContainer(-1, 0x7015);
                    break;
                case 2:
                    CreateContainer(-1, 0x7016);
                    break;
                case 3:
                    CreateContainer(-1, 0x7017);
                    break;
                }
            }
        } while (--count != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_drill", CreateContainer);

/*
 * Drill container serials run 0x7014..0x7027 (0x14 consecutive values); any
 * serial in that range, or any unit with the 0x100000 drill-container flag
 * bit set, is a drill container and gets cleared.
 */
void DrillClearContainer(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        DrillMapUnit *unit = &MapUnit[i];

        if ((u32)unit->serial - 0x7014 < 0x14) {
            unit->serial = (u16)-1;
        } else if ((unit->flags & 0x100000) != 0) {
            unit->serial = (u16)-1;
        } else {
            continue;
        }
        unit->update = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_drill", SetContainerParam);

/*
 * Spawns an item unit (template 0x7000), gives it a "pending pickup" state
 * (5) and the caller's value, initializes its item symbol, copies its kind
 * onto the container, and hides the container.
 */
void SetContainerItem(DrillItemUnit *container, int value)
{
    DrillItemUnit *item;

    item = Unit_CreateUwamono(-1, 0x7000);
    if (item != 0) {
        DrillItemInfo *info = &item->info;

        info->value = value;
        info->state = 5;
        InitItemSymbol(item);
        container->spawnedItemKind = item->itemKind;
        SetHideObject(container);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_drill", CalcPosContainer);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", MAP_updateUnitDrill);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillPowerOffFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillStandbyFunc);

/*
 * Advances the drill along Z while the pad's trigger (byte 0x28 bit 0x80)
 * is held, switching state and audio when released or on reaching the
 * -23.3 limit, then rewinds the drill's own effect and its item-slot
 * effect to the new Z position.
 */
void DrillZMoveFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillEffectRef *effect;

    xglFontDebugPrintf(8, 16, (char *) D_004CAA40);
    Vibration_Set_Strong(128, 128, 2);
    save->cameraPreset = 1;

    if ((PadData.half_28 & 0x80) != 0) {
        drill->position.vector.z -= save->zSpeed;
    } else {
        drill->phase = 3;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectNormalID(0x30097, 0);
    }

    if (drill->position.vector.z < -23.3f) {
        drill->position.vector.z = -23.3f;
        drill->phase = 3;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectNormalID(0x30097, 0);
    }

    effect = drill->effect2;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.z = drill->position.vector.z;

    effect = save->effect1;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.x = -4.0f;
    effect->offset.vector.y = 1.5f;
    effect->offset.vector.z = drill->position.vector.z + 0.5f;
}

/*
 * The drill is stopped along Z: the pad's L1 bit (0x2 of the halfword at
 * PadData+0x2a) cycles the camera preset through 0..2, and its trigger bit
 * (0x80) stops the camera shake, restarts the running sound and hands the
 * drill to phase 4.
 */
void DrillZStopFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    DrillEffectRef *effect;

    xglFontDebugPrintf(8, 16, (char *) D_004CAA50);
    camera = xglStudioGetActiveCamera();
    DrillCameraControlFunc(drill, camera);

    if ((PadData.half_2a & 0x2) != 0) {
        DrillResetCameraPos(drill, camera);
        save->cameraPreset++;
        save->cameraPreset %= 3;
    }

    if ((PadData.half_2a & 0x80) != 0) {
        camera->angles.shakeVelocityX = 0.0f;
        camera->angles.shakeVelocityY = 0.0f;
        camera->angles.shakeVelocityZ = 0.0f;
        camera->angles.shakeOffsetX = 0.0f;
        camera->angles.shakeOffsetY = 0.0f;
        camera->angles.shakeOffsetZ = 0.0f;
        camera->state = 0;
        drill->phase = 4;
        xglSoundEffectNormalID(0x30098, 0);
    }

    effect = drill->effect2;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
}

/*
 * Slides the drill sideways while the pad's trigger (byte 0x28 bit 0x80) is
 * held. Releasing it inside the track, or reaching the +4.3 edge, ends the
 * run: the drill switches to phase 5, spawns the two hand-over effects,
 * remembers the camera angles and swaps the running sound. Either way the
 * drill's effect and its item-slot effect are rewound to the new position.
 */
void DrillXMoveFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    DrillEffectRef *effect;

    camera = xglStudioGetActiveCamera();
    xglFontDebugPrintf(8, 16, (char *) D_004CAA60);
    Vibration_Set_Strong(128, 128, 2);

    if ((PadData.half_28 & 0x80) != 0) {
        drill->position.vector.x += save->xSpeed;
    } else if (drill->position.vector.x < -1.9000001f) {
        drill->position.vector.x += save->xSpeed;
    } else {
        drill->phase = 5;

        effect = sefCreateEffectCf(1594, 0, 0);
        effect->offset = drill->position;
        effect->flags &= ~0x10;
        effect->offset.vector.y = 0.0f;

        effect = sefCreateEffectCf(1657, 0, 0);
        effect->flags &= ~0x400;
        effect->owner = drill;

        drill->cameraYaw = camera->angles.yaw;
        drill->cameraPitch = camera->angles.pitch;

        xglSoundEffectStopID(0x30098, 0);
        xglSoundEffectNormalID(0x30099, 0);
        return;
    }

    if (drill->position.vector.x > 4.3f) {
        drill->position.vector.x = 4.3f;
        drill->phase = 5;

        effect = sefCreateEffectCf(1594, 0, 0);
        effect->offset = drill->position;
        effect->flags &= ~0x10;
        effect->offset.vector.y = 0.0f;

        effect = sefCreateEffectCf(1657, 0, 0);
        effect->flags &= ~0x400;
        effect->owner = drill;

        drill->cameraYaw = camera->angles.yaw;
        drill->cameraPitch = camera->angles.pitch;

        xglSoundEffectStopID(0x30098, 0);
        xglSoundEffectNormalID(0x30099, 0);
    }

    effect = drill->effect3;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.x = drill->position.vector.x;
    effect->offset.vector.z = drill->position.vector.z;

    effect = save->effect1;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.x = drill->position.vector.x;
    effect->offset.vector.y = 6.0f;
    effect->offset.vector.z = drill->position.vector.z;
}

/*
 * The drill has crashed into the ground: it spins about Y while it falls,
 * and each time it reaches ground level it bounces, spawning the impact
 * effect on the first bounce and handing the drill to phase 6 after nine.
 */
void DrillCrashFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    DrillEffectRef *effect;

    effect = drill->effect3;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
    camera = xglStudioGetActiveCamera();
    xglFontDebugPrintf(8, 16, (char *) D_004CAA70, save->hitCount);

    drill->crashTimer++;
    drill->rotationY += drill->crashTimer * 0.1f;
    drill->rotationX = 0.0f;
    drill->rotationZ = 0.0f;
    xglMatrixStackUnit();
    xglMatrixStackRotY(drill->rotationY);
    xglMatrixStackSave(drill->matrix);

    camera->angles.positionX = drill->position.vector.x + 4.0f;
    camera->angles.positionY = drill->position.vector.y + 5.0f;
    camera->angles.positionZ = drill->position.vector.z - 4.0f;
    camera->angles.yaw = -0.78539819f;
    camera->angles.pitch = 2.3561945f;
    camera->angles.roll = 0.0f;

    if (drill->crashTimer >= 16) {
        drill->crashTimer = 16;
        drill->position.vector.y -= save->fallSpeed;
        DrillHitCheck(drill);
        if (drill->position.vector.y < 0.0f) {
            if (drill->bounceCount == 0) {
                sefCreateEffectCf(1721, &drill->position, 0);
            }
            drill->bounceCount++;
            drill->position.vector.y = 0.0f;
            if (drill->bounceCount >= 9) {
                camera->angles.yaw = drill->cameraYaw;
                camera->angles.pitch = drill->cameraPitch;
                DrillCalcCameraPos(camera);
                drill->phase = 6;
            }
        }
    }
}

typedef struct DrillReturnEffectRef {
    unsigned char unmodeled_000[0x80];
    Vector4 offset;                     /* +0x080: effect offset vector */
    unsigned char unmodeled_090[0x634];
    void *owner;                         /* +0x6c4: owning Drill */
    unsigned char unmodeled_6c8[0x3c4];
    u32 flags;                            /* +0xa8c */
    unsigned char unmodeled_a90[0x0a];
    u8 hidden;                            /* +0xa9a: visibility flag */
} DrillReturnEffectRef;

typedef struct DrillReturnSave {
    unsigned char unmodeled_00[4];
    signed char hit;                     /* +0x004 */
    unsigned char unmodeled_05[7];
    float xSpeed;                         /* +0x00c */
    float fallSpeed;                      /* +0x010 */
    float zSpeed;                         /* +0x014 */
    float xReturnSpeed;                   /* +0x018 */
    float riseSpeed;                      /* +0x01c */
    float zReturnSpeed;                   /* +0x020 */
    unsigned char unmodeled_24[8];
    signed char returnLimit;              /* +0x02c */
    unsigned char unmodeled_2d;
    signed char kind;                     /* +0x02e */
    unsigned char unmodeled_2f[4];
    signed char hitCount;                 /* +0x033 */
    unsigned char unmodeled_34[8];
    int returnCount;                      /* +0x03c */
    unsigned char unmodeled_40[0x0c];
    DrillReturnEffectRef *effect1;        /* +0x04c */
    unsigned char unmodeled_50[8];
    u32 flags;                            /* +0x058 */
} DrillReturnSave;

typedef struct DrillReturnView {
    unsigned char unmodeled_000[0x10];
    Vector4 position;                    /* +0x010: direct xyz view used by DrillReturnFunc */
    float rotationX;                      /* +0x020 */
    float rotationY;                      /* +0x024 */
    float rotationZ;                      /* +0x028 */
    unsigned char unmodeled_02c[0x54];
    float (*matrix)[4];                   /* +0x080 */
    unsigned char unmodeled_084[0x1e];
    u8 phase;                             /* +0x0a2 */
    unsigned char unmodeled_0a3[5];
    s16 crashTimer;                       /* +0x0a8 */
    unsigned char unmodeled_0aa[0x3e];
    DrillReturnEffectRef *effect3;        /* +0x0e8 */
    unsigned char unmodeled_0ec[0xb4];
    DrillReturnSave save;                 /* +0x1a0 */
    unsigned char unmodeled_1fc[0x24];
    float cameraYaw;                      /* +0x220 */
    float cameraPitch;                    /* +0x224 */
    unsigned char unmodeled_228[8];
    DrillReturnEffectRef *effect2;        /* +0x230 */
} DrillReturnView;

typedef struct DrillGameLoopState {
    unsigned char unmodeled_000[0x10];
    u32 flags;
} DrillGameLoopState;

extern DrillGameLoopState GameLoopState;
extern const char D_004CAA80[];
extern const char D_004CAA90[];
extern const char D_004CAAA0[];
extern void CallMethod(const char *method);

/* DrillReturnFunc's submitted body uses flat Vector4 expressions and a partial effect view.
 * Both views point into the same proven object; callees retain their published
 * canonical prototypes, so the local call aliases cast only at those pointer
 * boundaries without changing the EE pointer ABI or pointed-to bytes. */
typedef Drill DrillReturnCanonical;
typedef DrillEffectRef DrillReturnEffectCanonical;
#define Drill DrillReturnView
#define DrillSave DrillReturnSave
#define DrillEffectRef DrillReturnEffectRef
#define DrillHitCheck(view) DrillHitCheck((DrillReturnCanonical *)(view))
#define sefRewindEffectCf(view) sefRewindEffectCf((DrillReturnEffectCanonical *)(view))

void DrillReturnFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillEffectRef *effect;
    DrillCamera *camera;

    xglFontDebugPrintf(8, 16, (const char *)D_004CAA80);
    if (drill->crashTimer > 0) {
        drill->crashTimer--;
        drill->rotationY += drill->crashTimer * 0.1f;
        drill->rotationX = 0.0f;
        drill->rotationZ = 0.0f;
        xglMatrixStackUnit();
        xglMatrixStackRotY(drill->rotationY);
        xglMatrixStackSave(drill->matrix);
        DrillHitCheck(drill);
        return;
    }

    if (drill->position.y < 4.5f) {
        drill->position.y += save->riseSpeed;
        if (drill->position.y >= 4.5f) {
            drill->position.y = 4.5f;
            xglSoundEffectNormalID(0x30098, 0);
        }
        return;
    }

    drill->position.y = 4.5f;
    if (drill->position.x > -2.9000001f) {
        drill->position.x -= save->xReturnSpeed;
        if (drill->position.x <= -2.9000001f) {
            drill->position.x = -2.9000001f;
            xglSoundEffectStopID(0x30098, 0);
            xglSoundEffectNormalID(0x30096, 0);
        }
        effect = drill->effect3;
        sefRewindEffectCf(effect);
        effect->hidden = 1;
        effect->offset.x = drill->position.x;
        effect->offset.z = drill->position.z;
        effect = save->effect1;
        sefRewindEffectCf(effect);
        effect->hidden = 1;
        effect->offset.x = drill->position.x;
        effect->offset.y = 6.0f;
        effect->offset.z = drill->position.z;
        return;
    }

    drill->position.x = -2.9000001f;
    effect = drill->effect3;
    effect->hidden = 0;
    if (drill->position.z < -12.5f) {
        drill->position.z += save->zReturnSpeed;
        if (drill->position.z >= -12.5f) {
            drill->position.z = -12.5f;
            xglSoundEffectStopID(0x30096, 0);
            xglSoundEffectNormalID(0x30097, 0);
        }
        effect = drill->effect2;
        sefRewindEffectCf(effect);
        effect->hidden = 1;
        effect->offset.z = drill->position.z;
        effect = save->effect1;
        effect->offset.x = -4.0f;
        effect->offset.y = 1.5f;
        effect->offset.z = drill->position.z - 0.5f;
        return;
    }

    drill->phase = 1;
    drill->position.z = -12.5f;
    effect = drill->effect2;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
    save->returnCount++;
    if (save->returnLimit == -1) {
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod((const char *)D_004CAA90);
        return;
    }
    if (save->returnLimit < save->returnCount) {
        camera = xglStudioGetActiveCamera();
        camera->state = 4;
        GameLoopState.flags &= ~0x10000;
        drill->phase = 0;
        GameLoopState.flags |= 0x80000;
        save->hit = 0;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod((const char *)D_004CAAA0);
    } else {
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod((const char *)D_004CAA90);
    }
}
#undef sefRewindEffectCf
#undef DrillHitCheck
#undef DrillEffectRef
#undef DrillSave
#undef Drill


/*
 * Steers the drill camera from the right analog stick: each axis moves yaw
 * or pitch by its distance past the pad's dead zone, yaw stays inside
 * -pi/2 .. -pi/12 and, while the drill is not riding a container slot,
 * pitch stays inside 0 .. pi/2.
 */
void DrillCameraControlFunc(Drill *drill, DrillCamera *camera)
{
    DrillSave *save;
    DrillCameraAngles *angles = &camera->angles;

    xglFontDebugPrintf(8, 32, (char *) D_004CAAB0);
    save = &drill->save;

    camera->state = 0;
    angles->shakeOffsetStepX = 0.0f;
    angles->shakeOffsetStepY = 0.0f;
    angles->shakeOffsetStepZ = 0.0f;
    angles->shakeVelocityStepX = 0.0f;
    angles->shakeVelocityStepY = 0.0f;
    angles->shakeVelocityStepZ = 0.0f;
    angles->shakeOffsetX = 0.0f;
    angles->shakeOffsetY = 0.0f;
    angles->shakeOffsetZ = 0.0f;
    angles->shakeVelocityX = 0.0f;
    angles->shakeVelocityY = 0.0f;
    angles->shakeVelocityZ = 0.0f;

    if (PadData.pitchStick < 0) {
        angles->pitch += (PadData.pitchStick + PadData.pitchDeadZone) * 0.00052359881f;
    }
    if (PadData.pitchStick > 0) {
        angles->pitch += (PadData.pitchStick - PadData.pitchDeadZone) * 0.00052359881f;
    }

    if (PadData.yawStick < 0) {
        angles->yaw += (PadData.yawStick + PadData.yawDeadZone) * 0.00030000001f;
    }
    if (PadData.yawStick > 0) {
        angles->yaw += (PadData.yawStick - PadData.yawDeadZone) * 0.00030000001f;
    }

    if (angles->yaw > -0.20943953f) {
        angles->yaw = -0.20943953f;
    }
    if (angles->yaw < -1.5707964f) {
        angles->yaw = -1.5707964f;
    }

    if (save->kind == 0) {
        if (angles->pitch < 0.0f) {
            angles->pitch = 0.0f;
        }
        if (angles->pitch > 1.5707964f) {
            angles->pitch = 1.5707964f;
        }
    }

    DrillCalcCameraPos(camera);
}

/*
 * Computes the drill camera's fixed-radius orbit position from its yaw
 * and pitch: a radius-15 orbit around a fixed base offset, plus a
 * pitch-scaled vertical amplitude of 20.
 */
void DrillCalcCameraPos(DrillCamera *camera)
{
    DrillCameraAngles *angles = &camera->angles;
    float radius;

    /*
     * 0.7f alone folds to 0x3f333333 under this compiler; the original
     * .lit4 constant at 0x004d7fe8 is 0x3f333334, so the literal is
     * spelled with the extra digit that rounds to that exact bit pattern.
     */
    camera->angles.positionX = 0.70000005f;
    camera->angles.positionY = 0.5f;
    camera->angles.positionZ = -17.9f;

    camera->angles.positionY -= xglSin(angles->yaw) * 20.0f;
    radius = xglCos(angles->yaw) * 15.0f;
    camera->angles.positionX += radius * xglSin(angles->pitch);
    camera->angles.positionZ += radius * xglCos(angles->pitch);
    camera->angles.positionX -= 1.2f;
}

/*
 * Resets the active camera to one of three fixed yaw/pitch presets chosen
 * by the drill's cameraPreset field, zeroing roll and camera state.
 */
void DrillResetCameraPos(Drill *drill, DrillCamera *camera)
{
    camera->state = 0;
    if (drill->save.cameraPreset == 0) {
        camera->angles.yaw = -0.20943953f;
        camera->angles.pitch = 1.5707964f;
        camera->angles.roll = 0.0f;
        DrillCalcCameraPos(camera);
    } else {
        if (drill->save.cameraPreset == 1) {
            camera->angles.pitch = 0.0f;
            camera->angles.yaw = -0.20943953f;
            camera->angles.roll = 0.0f;
            DrillCalcCameraPos(camera);
        } else {
            if (drill->save.cameraPreset == 2) {
                camera->angles.pitch = 0.0f;
                camera->angles.yaw = -1.5707964f;
                camera->angles.roll = 0.0f;
                DrillCalcCameraPos(camera);
            }
        }
    }
}

void DrillResetFlag(Drill *drill)
{
    DrillSave *save = &drill->save;
    int i;

    save->flags &= ~0x1F0;

    for (i = 501; i < 541; i++) {
        xglFlagsSet1(0x79ec7 + i, 0);
    }

    for (i = 0; i < 64; i++) {
        DrillMapUnit *unit = &MapUnit[i];

        if ((s16)unit->serial == 0x7000 || (s16)unit->serial == 0x7009) {
            if ((u32)(unit->drill_reset_value - 501) < 50) {
                unit->serial = (u16)-1;
                unit->update = 0;
            }
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillHitCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", HitCheckContainerPosSize);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", HitCheckEnemy);
