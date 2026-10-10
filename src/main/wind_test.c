#include "common.h"

#include "shared.h"

static signed char wtype;

/* A pp_init particle (ppInit/ppNextStart/ppNextEnd read and write it up to
 * +0x34).  The original wind particles lie 0x40 apart (wpos 0x00585180, wvec
 * 0x005851C0, after cpos 0x00585140), the same 0x40-byte extent colli_test's
 * ParticleState records for pos1/pos2. */
typedef struct WindParticle {
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    float gravity;
    float damping;
    unsigned char unmodeled_38[8];
} WindParticle;

static WindParticle wpos;

#define D_0058518C (&wpos.position.w)

extern double fptodp(float value);

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

/* CPU-decoded controller state: xglPadRead stores held/pressed/repeat at
 * +0x28/+0x2a/+0x2c and the signed analog axes at +0x64. Records are 0x68 bytes. */
typedef struct WindPadRecord {
    u8 unmodeled_00[0x28];
    union {
        u64 packed;
        struct {
            u16 held;
            u16 pressed;
            u16 repeat;
            u16 released;
        } input;
    } buttons;
    u8 unmodeled_30[0x64 - 0x30];
    signed char axis[4];
} WindPadRecord;

extern WindPadRecord PadData[2];

extern void ppNextStart(WindParticle *particle);

extern void ppNextEnd(WindParticle *particle);

extern void EXM_SetPointWind(Vector4 *particle);

extern void EXM_SetDirectionalWind(Vector4 *particle);

extern void EXM_SetShakePower(float power);

extern void EXM_StepShakeWind(void);

extern void EXM_InitWind(void);

extern void xglRenderClearFrame(void);

extern void xglSleep(void);

extern int InitTest(void);

extern void MoveCamera(void);

static void MoveWind2(void);

struct Actor;

extern void ACT_setMotion(struct Actor *actor, unsigned int motionId);

extern void ACT_updateMotion(struct Actor *actor);

extern void ACT_modelDraw(struct Actor *actor);

extern double dpmul(double left, double right);

extern double dpadd(double left, double right);

extern double dpsub(double left, double right);

extern float dptofp(double value);

extern double cos(double angle);

extern double sin(double angle);

/* LOCAL in the original .sbss (wtype 0x004DC610, cry 0x004DC61C, pAct
 * 0x004DC62C, shake 0x004DC630); the INCLUDE_ASM functions reach cry and pAct
 * as cry_004DC61C and pAct_004DC62C. */
static float cry;

static struct Actor *pAct;

static float shake;

static WindParticle wvec;

static void MoveWind(void);

static const u64 D_004CBBD0[2] = {
    0x000000ff000000ffULL,
    0x00000050000000ffULL,
};

typedef struct WindTestDirectionalRecord {
    u64 payload_words[2];
    u32 zero_10;
    float y_bias_14;
    u32 zero_18;
    float scalar_1c;
    float direction_20[3];
} WindTestDirectionalRecord;

INCLUDE_ASM("asm/main/nonmatchings/wind_test", InitTest_002D9430);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveCamera);

static void MoveWind(void)
{
    float movementX;
    float movementZ;
    float movementScale;
    unsigned short buttons;
    double positionX;
    double forward;
    double side;
    double forwardOffset;
    double sideOffset;
    double positionZ;

    ppNextStart(&wpos);
    movementScale = 0.00005f;
    buttons = PadData[0].buttons.input.held;
    if (buttons & 1) {
        wpos.position.y -= 0.05f;
    }
    if (buttons & 4) {
        wpos.position.y += 0.05f;
    }
    movementX = (float)PadData[0].axis[2] * movementScale;
    movementZ = (float)PadData[0].axis[3] * movementScale;
    positionX = fptodp(wpos.position.x);
    forward = fptodp(movementX);
    forwardOffset = dpmul(forward, cos(fptodp(-cry)));
    side = fptodp(movementZ);
    sideOffset = dpmul(side, sin(fptodp(-cry)));
    wpos.position.x = dptofp(dpadd(positionX, dpsub(forwardOffset, sideOffset)));
    positionZ = fptodp(wpos.position.z);
    sideOffset = dpmul(side, cos(fptodp(-cry)));
    forwardOffset = dpmul(forward, sin(fptodp(-cry)));
    wpos.position.z = dptofp(dpadd(positionZ, dpadd(sideOffset, forwardOffset)));
    ppNextEnd(&wpos);
}

static void MoveWind2(void)
{
    float movementX;
    float movementZ;
    float movementScale;
    unsigned short buttons;
    double positionX;
    double forward;
    double side;
    double forwardOffset;
    double sideOffset;
    double positionZ;

    ppNextStart(&wvec);
    movementScale = 0.00001f;
    buttons = PadData[0].buttons.input.held;
    if (buttons & 1) {
        wvec.position.y -= 0.001f;
    }
    if (buttons & 4) {
        wvec.position.y += 0.001f;
    }
    movementX = (float)PadData[0].axis[2] * movementScale;
    movementZ = (float)PadData[0].axis[3] * movementScale;
    positionX = fptodp(wvec.position.x);
    forward = fptodp(movementX);
    forwardOffset = dpmul(forward, cos(fptodp(-cry)));
    side = fptodp(movementZ);
    sideOffset = dpmul(side, sin(fptodp(-cry)));
    wvec.position.x = dptofp(dpadd(positionX, dpsub(forwardOffset, sideOffset)));
    positionZ = fptodp(wvec.position.z);
    sideOffset = dpmul(side, cos(fptodp(-cry)));
    forwardOffset = dpmul(forward, sin(fptodp(-cry)));
    wvec.position.z = dptofp(dpadd(positionZ, dpadd(sideOffset, forwardOffset)));
    ppNextEnd(&wvec);
}

static void PrintDisp(void)
{
    const char *position_format;

    xglFontDebugPrintf(0, 0, "\013WindTest");
    if (wtype == 2) {
        xglFontDebugPrintf(8, 8, "\013point");
    } else {
        xglFontDebugPrintf(8, 8, "\013directional");
    }
    position_format = "\013\033\030\033\036\033\037WindPow:%f";
    xglFontDebugPrintf(0x64, 0xC8, position_format, fptodp(D_0058518C[0]));
    xglFontDebugPrintf(0x64, 0xD0,
                       "\013\033\030\033\035\033\034RandPow:%f",
                       fptodp(shake));
}

void WindTest(void)
{
    const float minimum = 0.0f;
    const float step = 0.005f;
    const float wind_scale = 10.0f;
    const float directional_y_bias = 1.8f;

    xglRenderClearFrame();
    xglSleep();

    if (InitTest() != 0) {
        if ((PadData[0].buttons.packed & 0x08000100ULL) != 0x08000100ULL) {
            do {
                WindTestDirectionalRecord directional_record;
                if ((PadData[0].buttons.input.pressed & 0x0100) != 0) {
                    if (wtype == 2) {
                        wtype = 1;
                    } else {
                        wtype = 2;
                    }
                }

                EXM_StepShakeWind();

                if ((PadData[0].buttons.input.held & 0x0080) != 0) {
                    if (wtype == 2) {
                        MoveWind();
                    } else {
                        MoveWind2();
                    }
                } else {
                    MoveCamera();
                }

                if ((PadData[0].buttons.input.repeat & 0x1000) != 0) {
                    wpos.position.w += step;
                } else if ((PadData[0].buttons.input.repeat & 0x4000) != 0) {
                    wpos.position.w -= step;
                    if (wpos.position.w < minimum) {
                        wpos.position.w = minimum;
                    }
                }

                if ((PadData[0].buttons.input.repeat & 0x2000) != 0) {
                    shake += step;
                } else if ((PadData[0].buttons.input.repeat & 0x8000) != 0) {
                    shake -= step;
                    if (shake < minimum) {
                        shake = minimum;
                    }
                }

                if (wtype == 2) {
                    EXM_SetPointWind(&wpos.position);
                } else {
                    EXM_SetDirectionalWind(&wvec.position);
                    directional_record.payload_words[0] = D_004CBBD0[0];
                    directional_record.payload_words[1] = D_004CBBD0[1];
                    directional_record.zero_10 = 0;
                    directional_record.y_bias_14 = directional_y_bias;
                    directional_record.zero_18 = 0;
                    directional_record.scalar_1c = 0.1f;
                    directional_record.direction_20[0] =
                        wvec.position.x * wind_scale + minimum;
                    directional_record.direction_20[1] =
                        wvec.position.y * wind_scale + directional_y_bias;
                    directional_record.direction_20[2] =
                        wvec.position.z * wind_scale + minimum;
                }

                EXM_SetShakePower(shake);
                ACT_setMotion(pAct, 0);
                ACT_updateMotion(pAct);
                ACT_modelDraw(pAct);
                PrintDisp();
                xglSleep();
            } while ((PadData[0].buttons.packed & 0x08000100ULL) != 0x08000100ULL);
        }

        EXM_InitWind();
    }
}
