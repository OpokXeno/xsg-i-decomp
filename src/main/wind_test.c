#include "common.h"
#include "shared.h"

#define WIND_POSITION_PTR (&wpos.position.w)

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

typedef union WindVectorBlock {
    Vector4 vector;
    u64 words[2];
} WindVectorBlock;

struct Actor;

typedef struct WindTestDirectionalRecord {
    u64 payload_words[2];
    u32 zero_10;
    float y_bias_14;
    u32 zero_18;
    float scalar_1c;
    float direction_20[3];
} WindTestDirectionalRecord;

/* Actor prefix read and written by InitTest: the same layout act_2.h gives
 * struct Actor through position (+0x10), rotation (+0x50) and scale (+0x60). */
typedef struct WindActor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    WindVectorBlock position;
    unsigned char unmodeled_20[0x30];
    WindVectorBlock rotation;
    WindVectorBlock scale;
} WindActor;

extern WindPadRecord PadData[2];

extern double fptodp(float value);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void ppNextStart(WindParticle *particle);
extern void ppNextEnd(WindParticle *particle);
extern void EXM_SetPointWind(Vector4 *particle);
extern void EXM_SetDirectionalWind(Vector4 *particle);
extern void EXM_SetShakePower(float power);
extern void EXM_StepShakeWind(void);
extern void EXM_InitWind(void);
extern void xglRenderClearFrame(void);
extern void xglSleep(void);
static int InitTest(void);
static void MoveCamera(void);
static void MoveWind2(void);
extern void ACT_setMotion(struct Actor *actor, unsigned int motionId);
extern void ACT_updateMotion(struct Actor *actor);
extern void ACT_modelDraw(struct Actor *actor);
extern double dpmul(double left, double right);
extern double dpadd(double left, double right);
extern double dpsub(double left, double right);
extern float dptofp(double value);
extern float atan2f(float y, float x);
extern double cos(double angle);
extern double sin(double angle);
static void MoveWind(void);
extern void GameResourceInit(int, int);
extern void xglStudioInit(void);
extern void xglStudioChange(int studio);
extern void xglStudioMainCameraInit(void);
extern void xglStudioGetCamera(StudioCamera **camera, int index);
extern void ppInit(WindParticle *particle);
extern void ppSetPos(WindParticle *particle, float x, float y, float z);
extern void EXM_SetShakeTime(float time);
extern void ACT_init(void);
extern void ACT_resourceInit(void);
extern void *ACT_create(int, int);
extern void ACT_initMotion(struct Actor *actor);
extern void ACT_loadMotion(struct Actor *actor, int, int);
extern void ACT_loadResource(struct Actor *actor, int);
extern void ACT_allocMatrix(struct Actor *actor, int);
extern void ACT_setModelWrapper(struct Actor *actor, int);

const Vector4 windRotationAndScaleVector __attribute__((aligned(16))) = { 0.0f, 1.5707963705062866f, 0.0f, 1.0f };

const Vector4 unitScaleVector = { 1.0f, 1.0f, 1.0f, 1.0f };

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
    WindVectorBlock local;
    WindVectorBlock rotationAndScale[2];
    WindActor *actor;
    float initialPower;
    WindVectorBlock *localBlock = &local;

    GameResourceInit(0x07000000, 0x06000000);
    xglStudioInit();
    xglStudioChange(0);
    xglStudioMainCameraInit();
    xglStudioGetCamera(&pCamera, 0);
    pCamera->position.y = 2.0f;
    pCamera->position.z = 2.0f;
    pCamera->position.x = 0.0f;
    ppInit(&cpos);
    ppSetPos(&cpos, 0.0f, 1.0f, 3.0f);
    cpos.gravity = 0.0f;
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
    rotationAndScale[0] = *(const WindVectorBlock *)&windRotationAndScaleVector;
    rotationAndScale[1] = *(const WindVectorBlock *)&unitScaleVector;
    local.vector.w = 1.0f;
    actor = (WindActor *)pAct;
    actor->position.words[0] = localBlock->words[0];
    wtype = 2;
    {
        float value = wpos.gravity;

        wpos.gravity = value;
    }
    actor->position.words[1] = localBlock->words[1];
    {
        float damping = wpos.damping;

        wpos.damping = damping;
    }
    actor->rotation = rotationAndScale[0];
    actor->scale = rotationAndScale[1];
    ppInit(&wpos);
    wpos.gravity = 0.0f;
    ppSetPos(&wpos, 0.5f, 1.0f, 0.0f);
    wpos.position.w = initialPower;
    shake = initialPower;
    EXM_SetPointWind(&wpos.position);
    ppInit(&wvec);
    wvec.gravity = 0.0f;
    ppSetPos(&wvec, -0.05f, 0.0f, 0.0f);
    wpos.position.w = 1.0f;
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
        lookX = -cpos.position.x;
        lookZ = -cpos.position.z;
        lookHeight = 1.0f - cpos.position.y;
        cry = -atan2f(lookX, cpos.position.z);
        crx = atan2f(lookHeight, __builtin_sqrtf(lookX * lookX + lookZ * lookZ));
        crvx = crvy = 0.0f;
    }
    ppNextStart(&cpos);
    if (PadData[0].buttons.input.held & 1) {
        cpos.position.y -= 0.01f;
    }
    if (PadData[0].buttons.input.held & 4) {
        cpos.position.y += 0.01f;
    }
    moveX = (float)PadData[0].axis[2] * 0.0001f;
    moveZ = (float)PadData[0].axis[3] * 0.0001f;
    position = fptodp(cpos.position.x);
    horizontalMove = moveX;
    rotatedMove = horizontalMove * cos(-cry);
    verticalMove = moveZ;
    cpos.position.x = position + (rotatedMove - verticalMove * sin(-cry));
    position = cpos.position.z;
    rotatedMove = verticalMove * cos(-cry);
    cpos.position.z = position + (rotatedMove + horizontalMove * sin(-cry));
    ppNextEnd(&cpos);
    *(WindVectorBlock *)&pCamera->position = *(WindVectorBlock *)&cpos.position;
    pCamera->rotation.x = crx;
    pCamera->rotation.y = cry;
    pCamera->rotation.z = 0.0f;
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
    xglFontDebugPrintf(0x64, 0xC8, position_format, fptodp(WIND_POSITION_PTR[0]));
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
                    directional_record.payload_words[0] = directionalRecordPayloadTable[0];
                    directional_record.payload_words[1] = directionalRecordPayloadTable[1];
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
