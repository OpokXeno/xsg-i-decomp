#include "common.h"
#include "shared.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/exm_2.h"
#include "main/pp_init.h"
#include "main/res_get_path.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"
#include "main/xgl_studio.h"
#include "main/xgl_thread.h"

/* A pp_init particle (ppInit, ppNextStart and ppNextEnd use its first 0x38 bytes)
 * inside a 0x40-byte record: the original places cpos, wpos and wvec 0x40 bytes apart. */
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

/* wtype: which wind the test places and edits. */
enum {
    WIND_TYPE_DIRECTIONAL = 1,
    WIND_TYPE_POINT = 2
};

/* PadData[0].buttons.packed: SELECT held with START pressed ends the test. */
#define WIND_TEST_EXIT_PADS 0x08000100ULL

static signed char wtype;

static StudioCamera *pCamera;

static float crx;

static WindParticle cpos;

static WindParticle wpos;

/* File-local data from the original .sbss section. */
static float cry;

static float crvx;

static float crvy;

static float wpow;

static struct Actor *pAct;

static float shake;

static WindParticle wvec;

/* Sets up the test scene: the camera, the particles, the test actor and the point wind. Returns 1. */
static int InitTest(void)
{
    DoublewordVector4 local;
    AlignedVector4 rotation;
    AlignedVector4 scale;
    Actor *actor;
    float initialPower;
    DoublewordVector4 *localBlock = &local;

    GameResourceInit(GAME_RESOURCE_BASE_ADDRESS, GAME_RESOURCE_BUFFER_SIZE);
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
    wtype = WIND_TYPE_POINT;
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

/* Moves the camera from the pad: R1/R2 and the analog axes change the view angles, and the camera particle follows the sticks. */
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

    if (PadData[0].buttons.input.held & PAD_R2) {
        crvx -= 0.01f;
    }
    if (PadData[0].buttons.input.held & PAD_R1) {
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
    if ((PadData[0].buttons.input.pressed & PAD_R3) != 0 ||
        ((PadData[0].buttons.input.held & PAD_L2) != 0 &&
         (PadData[0].buttons.input.held & PAD_R2) != 0)) {
        crx = crvx = 0.0f;
    }
    /* CROSS aims the camera at the point (0, 1, 0). */
    if ((PadData[0].buttons.input.held & PAD_CROSS) != 0) {
        lookX = -cpos.particle.position.x;
        lookZ = -cpos.particle.position.z;
        lookHeight = 1.0f - cpos.particle.position.y;
        cry = -atan2f(lookX, cpos.particle.position.z);
        crx = atan2f(lookHeight, __builtin_sqrtf(lookX * lookX + lookZ * lookZ));
        crvx = crvy = 0.0f;
    }
    /* The camera particle moves with the analog sticks, rotated by the yaw cry. */
    ppNextStart(&cpos.particle);
    if (PadData[0].buttons.input.held & PAD_L2) {
        cpos.particle.position.y -= 0.01f;
    }
    if (PadData[0].buttons.input.held & PAD_L1) {
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

/* Moves the point wind wpos: L1/L2 change its height and the analog sticks move it, rotated by the camera yaw. */
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
    if (buttons & PAD_L2) {
        wpos.particle.position.y -= 0.05f;
    }
    if (buttons & PAD_L1) {
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

/* Moves the directional wind's position wvec the same way as MoveWind, with smaller steps. */
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
    if (buttons & PAD_L2) {
        wvec.particle.position.y -= 0.001f;
    }
    if (buttons & PAD_L1) {
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

/* Draws the test labels: the wind type, the wind power and the shake power. */
static void PrintDisp(void)
{
    const char *position_format;

    /* Text: {reset}WindTest */
    xglFontDebugPrintf(0, 0, "\013WindTest");
    if (wtype == WIND_TYPE_POINT) {
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

/* Runs the wind test until SELECT is held with START pressed, then resets the wind. */
void WindTest(void)
{
    const float minimum = 0.0f;
    const float step = 0.005f;
    const float wind_scale = 10.0f;
    const float directional_y_bias = 1.8f;

    xglRenderClearFrame();
    xglSleep();

    if (InitTest() != 0) {
        if ((PadData[0].buttons.packed & WIND_TEST_EXIT_PADS) != WIND_TEST_EXIT_PADS) {
            do {
                WindTestDirectionalRecord directional_record;
                /* SELECT switches between the point wind and the directional wind. */
                if ((PadData[0].buttons.input.pressed & PAD_SELECT) != 0) {
                    if (wtype == WIND_TYPE_POINT) {
                        wtype = WIND_TYPE_DIRECTIONAL;
                    } else {
                        wtype = WIND_TYPE_POINT;
                    }
                }

                EXM_StepShakeWind();

                /* SQUARE moves the wind; without it the pad moves the camera. */
                if ((PadData[0].buttons.input.held & PAD_SQUARE) != 0) {
                    if (wtype == WIND_TYPE_POINT) {
                        MoveWind();
                    } else {
                        MoveWind2();
                    }
                } else {
                    MoveCamera();
                }

                /* UP/DOWN set the wind power; LEFT/RIGHT set the shake power. */
                if ((PadData[0].buttons.input.repeat & PAD_UP) != 0) {
                    wpos.particle.position.w += step;
                } else if ((PadData[0].buttons.input.repeat & PAD_DOWN) != 0) {
                    wpos.particle.position.w -= step;
                    if (wpos.particle.position.w < minimum) {
                        wpos.particle.position.w = minimum;
                    }
                }

                if ((PadData[0].buttons.input.repeat & PAD_RIGHT) != 0) {
                    shake += step;
                } else if ((PadData[0].buttons.input.repeat & PAD_LEFT) != 0) {
                    shake -= step;
                    if (shake < minimum) {
                        shake = minimum;
                    }
                }

                /* Apply the current wind: the point wind, or a directional record built from wvec. */
                if (wtype == WIND_TYPE_POINT) {
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
            } while ((PadData[0].buttons.packed & WIND_TEST_EXIT_PADS) != WIND_TEST_EXIT_PADS);
        }

        EXM_InitWind();
    }
}
