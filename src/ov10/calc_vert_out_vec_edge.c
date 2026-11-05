/*
 * OV10 original TU 0: 0x00a00000..0x00a00410 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "main/xgl_studio.h"

static int s_nDebMode = 0;
static int s_nMode = 0;
static CardGameWork proc_0;
const char cardMinicardEffectGroup[] = "Minicard";
const char cardInitErrorFormat[32] = "Card Inisialize ERROR!!!\n";
extern void xglRenderClearFrame(void);
extern void xglRenderGlobalFadeInit(void);
extern void xglSleep(void);
extern void xglSoundLoadEffect(const char *bank_name, void *buffer, int mode);
extern int CardMainInit(CardGameWork *work);
extern int CardMainProc(CardGameWork *work);
extern int printf(const char *format, ...);

typedef struct EdgeVertex {
    Vector4 position;
    Vector4 outVec;
} EdgeVertex;

/*
 * Each vertex record in edgeList is a neighbour count followed by that many
 * vertex indices. The outward vector of a vertex is the sum of the unit
 * vectors from each neighbour towards it, normalised at the end.
 */
void CalcVertOutVecEdge(EdgeVertex *vertices, int vertexCount, const unsigned short *edgeList)
{
    int i;
    int j;
    int edgeCount;
    EdgeVertex *vertex;
    int index;
    Vector4 diff;
    float length;

    vertex = vertices;
    for (i = 0; i < vertexCount; i++) {
        /* The chained store is byte-required: it keeps 0.0f in a callee-saved FP register across the sqrt calls. */
        vertex->outVec.x = vertex->outVec.y = vertex->outVec.z = 0.0f;
        edgeCount = *edgeList++;
        for (j = 0; j < edgeCount; j++) {
            index = *edgeList++;
            diff.x = vertex->position.x - vertices[index].position.x;
            diff.y = vertex->position.y - vertices[index].position.y;
            diff.z = vertex->position.z - vertices[index].position.z;
            length = __builtin_sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
            vertex->outVec.x += diff.x / length;
            vertex->outVec.y += diff.y / length;
            vertex->outVec.z += diff.z / length;
        }
        length = __builtin_sqrtf(vertex->outVec.x * vertex->outVec.x
                                 + vertex->outVec.y * vertex->outVec.y
                                 + vertex->outVec.z * vertex->outVec.z);
        vertex->outVec.x /= length;
        vertex->outVec.y /= length;
        vertex->outVec.z /= length;
        vertex++;
    }
}

/*
 * The clear-environment block at 0x004a9080: yNewRenderInit writes the four
 * colour words at +0x20..+0x2c, the rest of the 0x60-byte block is untouched
 * here.
 */
typedef struct CardClearEnv {
    u8 unmodeled_00[0x20];
    u32 color_r;
    u32 color_g;
    u32 color_b;
    u32 color_a;
    u8 unmodeled_30[0x30];
} CardClearEnv;

/* One entry of the model table at 0x00a56bf0: two words, the transform at +0x10. */
typedef struct ModelRecord {
    u32 state;
    u32 mode;
    u8 unmodeled_08[0x08];
    Matrix4 matrix;
} ModelRecord;

extern CardClearEnv ClearEnv;
static int s_nPause_00A42038;
static float s_inMulCol[4];
static float s_inMulColAdd[4];
static float s_fvInt[4];
static float s_fvPos[4];
static ModelRecord s_aModel[10];
extern void xglStudioInit(void);
extern void xglStudioChange(int studio_index);
extern void xglStudioMainCameraInit(void);
extern void xglLightSetDefault(StudioLight *light);

void yNewRenderInit(void)
{
    StudioLight *light;
    int i;

    ClearEnv.color_r = 0;
    ClearEnv.color_g = 0;
    ClearEnv.color_b = 0;
    ClearEnv.color_a = 0x80;
    xglStudioInit();
    xglStudioChange(0);
    xglStudioGetLight(&light);
    xglLightSetDefault(light);
    s_nPause_00A42038 = 0;
    s_nDebMode = 0;
    s_inMulCol[0] = 1.0f;
    s_inMulCol[1] = 1.0f;
    s_inMulCol[2] = 1.0f;
    s_inMulCol[3] = 1.0f;
    s_inMulColAdd[0] = 0.01f;
    s_inMulColAdd[1] = 0.015f;
    s_inMulColAdd[2] = 0.02f;
    s_inMulColAdd[3] = 0.0f;
    s_fvPos[0] = 0.0f;
    s_fvPos[1] = 0.0f;
    s_fvPos[2] = 5.8f;
    s_fvPos[3] = 1.0f;
    s_fvInt[0] = 0.0f;
    s_fvInt[1] = 0.0f;
    s_fvInt[2] = 0.0f;
    s_fvInt[3] = 1.0f;
    xglStudioMainCameraInit();
    for (i = 0; i < 10; i++) {
        /* Byte-required: the pointer for the word stores and the index for the matrix call keep one induction register. */
        ModelRecord *model = &s_aModel[i];

        model->state = 0;
        model->mode = 0;
        xglMatrixUnit(s_aModel[i].matrix);
    }
}

extern void yNewRenderInit(void);

void CardGameRoot(void)
{
    xglRenderClearFrame();
    s_nMode = 1;
    yNewRenderInit();
    s_nMode = 1;
    s_nDebMode = 0;
    xglRenderGlobalFadeInit();
    xglSoundLoadEffect(cardMinicardEffectGroup, (void *)0x01B00000, 2);
    if (CardMainInit(&proc_0) < 0)
        printf(cardInitErrorFormat);

    while (CardMainProc(&proc_0))
        xglSleep();

    xglSoundLoadEffect(0, 0, 1);
    xglSoundLoadEffect(0, 0, 2);
    xglRenderGlobalFadeInit();
}

void CardGameRoot(void);

void YmineTest(void) {
    CardGameRoot();
}



static int s_nPause_00A42038 = 0;

static float s_inMulCol[4];

static float s_inMulColAdd[4];

static float s_fvInt[4];

static float s_fvPos[4];

static ModelRecord s_aModel[10];
