#include "common.h"
#include "shared.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/exm_2.h"
#include "main/pp_init.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"
#include "main/xgl_studio.h"
#include "main/xgl_thread.h"

/* A pp_init particle (ppInit/ppNextStart/ppNextEnd read and write its first
 * 0x38 bytes) inside a 0x40-byte record: the original .bss places cpos, wpos
 * and wvec 0x40 apart (section 2**3), the same 0x40-byte extent colli_test
 * keeps for pos1/pos2, while hair_test's bare PpParticle cpos is 0x38. */
typedef struct WindParticle {
    PpParticle particle;
    unsigned char unmodeled_38[8];
} WindParticle;

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

struct Actor;

typedef struct WindTestDirectionalRecord {
    u64 payload_words[2];
    u32 zero_10;
    float y_bias_14;
    u32 zero_18;
    float scalar_1c;
    float direction_20[3];
} WindTestDirectionalRecord;

extern WindPadRecord PadData[2];

extern double fptodp(float value);
static int InitTest(void);
static void MoveCamera(void);
static void MoveWind2(void);
extern double dpmul(double left, double right);
extern double dpadd(double left, double right);
extern double dpsub(double left, double right);
extern float dptofp(double value);
extern float atan2f(float y, float x);
extern double cos(double angle);
extern double sin(double angle);
static void MoveWind(void);
extern void GameResourceInit(int, int);
extern void xglStudioGetCamera(StudioCamera **camera, int index);
extern void ACT_init(void);
const AlignedVector4 windRotationAndScaleVector = { 0.0f, 1.5707963705062866f, 0.0f, 1.0f };

const AlignedVector4 unitScaleVector = { 1.0f, 1.0f, 1.0f, 1.0f };

static const u64 directionalRecordPayloadTable[2] = {
    0x000000ff000000ffULL,
    0x00000050000000ffULL,
};

static signed char wtype;

static StudioCamera *pCamera;

static float crx;

static WindParticle cpos;

static WindParticle wpos;

/* LOCAL in the original .sbss (wtype 0x004DC610, cry 0x004DC61C, pAct
 * 0x004DC62C, shake 0x004DC630); the INCLUDE_ASM functions reach cry and pAct
 * as cry_004DC61C and pAct_004DC62C. */
static float cry;

static float crvx;

static float crvy;

static float wpow;

static struct Actor *pAct;

static float shake;

static WindParticle wvec;

static int InitTest(void)
{
    DoublewordVector4 local;
    AlignedVector4 rotation;
    AlignedVector4 scale;
    Actor *actor;
    float initialPower;
    DoublewordVector4 *localBlock = &local;

    GameResourceInit(0x07000000, 0x06000000);
    xglStudioInit();
    xglStudioChange(0);
    xglStudioMainCameraInit();
    xglStudioGetCamera(&pCamera, 0);
    pCamera->position.vector.y = 2.0f;
    pCamera->position.vector.z = 2.0f;
    pCamera->position.vector.x = 0.0f;
    ppInit(&cpos.particle);
    ppSetPos(&cpos.particle, 0.0f, 1.0f, 3.0f);
    cpos.particle.gravity = 0.0f;
    crx = 0.0f;
    cry = 0.0f;
    crvx = 0.0f;
    crvy = 0.0f;
    ACT_init();
    ACT_resourceInit();
    pAct = ACT_create(-1, 2);
    initialPower = 0.1f;
    ACT_initMotion(pAct);
    ACT_loadMotion(pAct, 2, 1);
    ACT_loadResource(pAct, 2);
    ACT_allocMatrix(pAct, -1);
    ACT_setModelWrapper(pAct, 0);
    memset(localBlock, 0, 16);
    rotation = windRotationAndScaleVector;
    scale = unitScaleVector;
    local.vector.w = 1.0f;
    actor = pAct;
    ((DoublewordVector4 *)&actor->position)->words[0] = localBlock->words[0];
    wtype = 2;
    {
        float value = wpos.particle.gravity;

        wpos.particle.gravity = value;
    }
    ((DoublewordVector4 *)&actor->position)->words[1] = localBlock->words[1];
    {
        float damping = wpos.particle.damping;

        wpos.particle.damping = damping;
    }
    actor->rotation = rotation;
    actor->scale = scale;
    ppInit(&wpos.particle);
    wpos.particle.gravity = 0.0f;
    ppSetPos(&wpos.particle, 0.5f, 1.0f, 0.0f);
    wpos.particle.position.w = initialPower;
    shake = initialPower;
    EXM_SetPointWind(&wpos.particle.position);
    ppInit(&wvec.particle);
    wvec.particle.gravity = 0.0f;
    ppSetPos(&wvec.particle, -0.05f, 0.0f, 0.0f);
    wpos.particle.position.w = 1.0f;
    wpow = initialPower;
    EXM_SetShakeTime(0.5f);
    return 1;
}

static void MoveCamera(void)
{
    float lookX;
    float lookZ;
    float lookHeight;
    float moveX;
    float moveZ;
    double position;
    double horizontalMove;
    double verticalMove;
    double rotatedMove;

    if (PadData[0].buttons.input.held & 2) {
        crvx -= 0.01f;
    }
    if (PadData[0].buttons.input.held & 8) {
        crvx += 0.01f;
    }
    crvy -= (float)PadData[0].axis[0] * 0.0001f;
    crvx += (float)PadData[0].axis[1] * 0.0001f;
    cry += crvy;
    if (cry < 0.0f) {
        cry += 6.2831855f;
    } else if (cry > 6.2831855f) {
        cry -= 6.2831855f;
    }
    crx += crvx;
    if (crx < 0.0f) {
        crx += 6.2831855f;
    } else if (crx > 6.2831855f) {
        crx -= 6.2831855f;
    }
    crvy *= 0.8f;
    crvx *= 0.8f;
    if ((PadData[0].buttons.input.pressed & 0x400) != 0 ||
        ((PadData[0].buttons.input.held & 1) != 0 &&
         (PadData[0].buttons.input.held & 2) != 0)) {
        crx = crvx = 0.0f;
    }
    if ((PadData[0].buttons.input.held & 0x40) != 0) {
        lookX = -cpos.particle.position.x;
        lookZ = -cpos.particle.position.z;
        lookHeight = 1.0f - cpos.particle.position.y;
        cry = -atan2f(lookX, cpos.particle.position.z);
        crx = atan2f(lookHeight, __builtin_sqrtf(lookX * lookX + lookZ * lookZ));
        crvx = crvy = 0.0f;
    }
    ppNextStart(&cpos.particle);
    if (PadData[0].buttons.input.held & 1) {
        cpos.particle.position.y -= 0.01f;
    }
    if (PadData[0].buttons.input.held & 4) {
        cpos.particle.position.y += 0.01f;
    }
    moveX = (float)PadData[0].axis[2] * 0.0001f;
    moveZ = (float)PadData[0].axis[3] * 0.0001f;
    position = fptodp(cpos.particle.position.x);
    horizontalMove = moveX;
    rotatedMove = horizontalMove * cos(-cry);
    verticalMove = moveZ;
    cpos.particle.position.x = position + (rotatedMove - verticalMove * sin(-cry));
    position = cpos.particle.position.z;
    rotatedMove = verticalMove * cos(-cry);
    cpos.particle.position.z = position + (rotatedMove + horizontalMove * sin(-cry));
    ppNextEnd(&cpos.particle);
    pCamera->position.vector = cpos.particle.position;
    pCamera->rotation.vector.x = crx;
    pCamera->rotation.vector.y = cry;
    pCamera->rotation.vector.z = 0.0f;
}

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

    ppNextStart(&wpos.particle);
    movementScale = 0.00005f;
    buttons = PadData[0].buttons.input.held;
    if (buttons & 1) {
        wpos.particle.position.y -= 0.05f;
    }
    if (buttons & 4) {
        wpos.particle.position.y += 0.05f;
    }
    movementX = (float)PadData[0].axis[2] * movementScale;
    movementZ = (float)PadData[0].axis[3] * movementScale;
    positionX = fptodp(wpos.particle.position.x);
    forward = fptodp(movementX);
    forwardOffset = dpmul(forward, cos(fptodp(-cry)));
    side = fptodp(movementZ);
    sideOffset = dpmul(side, sin(fptodp(-cry)));
    wpos.particle.position.x = dptofp(dpadd(positionX, dpsub(forwardOffset, sideOffset)));
    positionZ = fptodp(wpos.particle.position.z);
    sideOffset = dpmul(side, cos(fptodp(-cry)));
    forwardOffset = dpmul(forward, sin(fptodp(-cry)));
    wpos.particle.position.z = dptofp(dpadd(positionZ, dpadd(sideOffset, forwardOffset)));
    ppNextEnd(&wpos.particle);
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

    ppNextStart(&wvec.particle);
    movementScale = 0.00001f;
    buttons = PadData[0].buttons.input.held;
    if (buttons & 1) {
        wvec.particle.position.y -= 0.001f;
    }
    if (buttons & 4) {
        wvec.particle.position.y += 0.001f;
    }
    movementX = (float)PadData[0].axis[2] * movementScale;
    movementZ = (float)PadData[0].axis[3] * movementScale;
    positionX = fptodp(wvec.particle.position.x);
    forward = fptodp(movementX);
    forwardOffset = dpmul(forward, cos(fptodp(-cry)));
    side = fptodp(movementZ);
    sideOffset = dpmul(side, sin(fptodp(-cry)));
    wvec.particle.position.x = dptofp(dpadd(positionX, dpsub(forwardOffset, sideOffset)));
    positionZ = fptodp(wvec.particle.position.z);
    sideOffset = dpmul(side, cos(fptodp(-cry)));
    forwardOffset = dpmul(forward, sin(fptodp(-cry)));
    wvec.particle.position.z = dptofp(dpadd(positionZ, dpadd(sideOffset, forwardOffset)));
    ppNextEnd(&wvec.particle);
}

static void PrintDisp(void)
{
    const char *position_format;

    /* Text: {reset}WindTest */
    xglFontDebugPrintf(0, 0, "\013WindTest");
    if (wtype == 2) {
        /* Text: {reset}point */
        xglFontDebugPrintf(8, 8, "\013point");
    } else {
        /* Text: {reset}directional */
        xglFontDebugPrintf(8, 8, "\013directional");
    }
    /* Text: {reset}{glyph 24}{glyph 30}{glyph 31}WindPow:%f */
    position_format = "\013\033\030\033\036\033\037WindPow:%f";
    xglFontDebugPrintf(0x64, 0xC8, position_format, fptodp(wpos.particle.position.w));
    xglFontDebugPrintf(0x64, 0xD0,
                       /* Text: {reset}{glyph 24}{glyph 29}{glyph 28}RandPow:%f */
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
                    wpos.particle.position.w += step;
                } else if ((PadData[0].buttons.input.repeat & 0x4000) != 0) {
                    wpos.particle.position.w -= step;
                    if (wpos.particle.position.w < minimum) {
                        wpos.particle.position.w = minimum;
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
                    EXM_SetPointWind(&wpos.particle.position);
                } else {
                    EXM_SetDirectionalWind(&wvec.particle.position);
                    directional_record.payload_words[0] = directionalRecordPayloadTable[0];
                    directional_record.payload_words[1] = directionalRecordPayloadTable[1];
                    directional_record.zero_10 = 0;
                    directional_record.y_bias_14 = directional_y_bias;
                    directional_record.zero_18 = 0;
                    directional_record.scalar_1c = 0.1f;
                    directional_record.direction_20[0] =
                        wvec.particle.position.x * wind_scale + minimum;
                    directional_record.direction_20[1] =
                        wvec.particle.position.y * wind_scale + directional_y_bias;
                    directional_record.direction_20[2] =
                        wvec.particle.position.z * wind_scale + minimum;
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
