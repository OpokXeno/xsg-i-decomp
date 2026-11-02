#include "common.h"
#include "main/xgl_render.h"
#include "main/xgl_2.h"
#include "shared.h"
#include "nml_model_set.h"
#include "main/xgl_studio.h"
#include "main/get.h"
#include "main/nml_model_set.h"
#include "main/face_point.h"
#include "main/nml_packet_add.h"
#include "main/nml_filter_set.h"
#include "main/xgl_light.h"
#include "quadword.h"
#include "vu0_macro.h"

#define NML_RENDER_MAP_ENTRY 0x10000u

/* ACT_modelDrawSub supplies alpha as a signed word from its actor at +0x9a8. */
#define NML_RENDER_STENCIL 0x4u

#define NML_RENDER_ZWRITE 0x8u
#define NML_RENDER_TOUMEI 0x20u

/*
 * Bounds and alignment a model texture pointer must satisfy: the pointer
 * lands inside main RAM at or above the executable's own load base
 * (config/tu/main/tu000.json "start": "0x00200000") and at or below the top
 * of the 32 MiB EE RAM, and it is qword (16-byte) aligned as GIF/DMA
 * texture transfers require.
 */
#define NML_TEXTURE_RAM_BASE 0x00200000u

#define NML_TEXTURE_RAM_MAX_OFFSET 0x01dfffffu
#define NML_TEXTURE_ALIGN_MASK 0xfu
#define NML_RENDER_FOG 0x2u
#define NML_NAME_MATCH_PREFIX 1
#define NML_NAME_MATCH_EXACT 2
#define NML_NAME_MATCH_SUBSTRING 3

#define LAYOUT_W(off) (s_inLayout.slots[(off) / 4].f)

#define LAYOUT_S32(off) (s_inLayout.slots[(off) / 4].i)

/* Measurement proxy only: stands in for the StudioCamera scissor members
 * (float at +0x60 x0, +0x64 y0, +0x68 x1, +0x6c y1) until the owner adds them. */
#define SCISSOR_F(c, i) (*(float *)((char *)(c) + 0x60 + 4 * (i)))

/*
 * The renderer passes this 0x1e0-byte block to nmlPacketSetAttributeData16N
 * with count 0x1e, so the copied extent is established. nmlModelClear clears
 * projection_entry at +0x1c0. No C body names the earlier float slots; their
 * meanings stay unknown, so that region is explicit unmodeled storage.
 */
typedef struct NmlProRealFields {
    Matrix4 matrix_000;
    Matrix4 inverseTransform;
    /* The original Circle/Map bodies use the +0x80/+0xC0 ranges only as
     * whole-qword copy destinations. Their base is 0x0095B940 and every
     * access is at a 16-byte slot in the corresponding function.s; model
     * each owned qword directly, without an overlay. */
    NmlVector4Overlay unmodeled_080[4];
    NmlVector4Overlay unmodeled_0c0[4];
    int unmodeled_100;
    int unmodeled_104;
    int unmodeled_108;
    int unmodeled_10c;
    short unmodeled_110;
    short unmodeled_112;
    short unmodeled_114;
    short unmodeled_116;
    int unmodeled_118;
    int unmodeled_11c;
    float unmodeled_120;
    float unmodeled_124;
    unsigned char unmodeled_128[8];
    u64 textureWord;
    u64 clampWord;
    float textureUOffset;
    float textureUScale;
    float textureVOffset;
    float textureVScale;
    Vector4 transformOrigin;
    Vector4 unmodeled_160;
    NmlVector4Overlay unmodeled_170[2];
    NmlVector4Overlay unmodeled_190[3];
} NmlProRealFields;

typedef struct NmlProRealState {
    NmlProRealFields fields;
    int projection_entry;
    unsigned char unmodeled_1c4[0x1c8 - 0x1c4];
    u64 unmodeled_1c8;
    u64 unmodeled_1d0;
    unsigned char unmodeled_1d8[0x1e0 - 0x1d8];
} NmlProRealState;

typedef struct {
    float color[3];
    float duration;
} NmlFadeValues;

/*
 * TU-local partial view of GameLoopState (main 0x00338680, size 0x2a030;
 * TU-local by canon, config/header-canon.json). Only the two fields this
 * setter reads are modeled: the fade level at +0x58 and the three fade
 * color components at +0x90, both forwarded to fade_set unchanged.
 */
typedef struct {
    unsigned char unmodeled_000[0xc - 0x0];
    unsigned short stealthMode; /* +0xc */
    unsigned char unmodeled_00e[0x10 - 0xe];
    unsigned int flags; /* +0x10 */
    unsigned char unmodeled_014[0x58 - 0x14];
    int level;                 /* +0x58 */
    int previousFadeMode; /* +0x5c */
    unsigned char unmodeled_060[0x30];
    union {
        float color[3]; /* +0x90 */
        NmlFadeValues fade;
    };
    NmlFadeValues previousFade; /* +0xa0 */
} NmlGameLoopState;

typedef struct NmlMaterialName {
    char text[32];
} NmlMaterialName;

struct NmlShadowTexture {
    unsigned int uploadHeader[8];
    unsigned char pixels[64][64];
};

typedef struct NmlDirectRecord {
    void *data;
    u16 count;
    u16 attribute;
} NmlDirectRecord;

typedef struct {
    unsigned char unmodeled_00[8];
    int entryCount;
    int entriesOffset;
} NmlDirectXtxBlock;

typedef struct {
    unsigned int packed;
    unsigned int word4;
    unsigned int word8;
    int size;
    int dataOffset;
} NmlDirectXtxEntry;



typedef int NmlProjectedPoint[4];


typedef struct NmlDirectEntry {
    const void *data;
    unsigned short size;
    unsigned short type;
} NmlDirectEntry;

typedef union GifCommandData {
    u64 value;
    struct {
        u32 low;
        u32 high;
    } words;
} GifCommandData;

/* A block whose material flag 0x10 is set keeps its bounding-box corner
 * offsets at +0x70/+0x80, in the bytes a weighted block uses for its matrix
 * index list. */
typedef union NmlModelBlockBounds {
    NmlModelBlock block;
    struct {
        unsigned char unmodeled_00[0x70];
        Vector4 boxMinOffset; /* +0x70 */
        Vector4 boxMaxOffset; /* +0x80 */
    } bounds;
} NmlModelBlockBounds;

/*
 * fadeOutLockedFlag is main:0x0095DB90, the same byte the FadeControl comment above
 * ties to s_inFadeOut's "locked" field at +0x30 (0x0095DB60 + 0x30): cited
 * there only as evidence, not as claimed source. Addressing it through
 * &s_inFadeOut+0x30 rather than its own symbol changes the emitted %lo
 * immediate (sw v0,48(v1) for the original's sw v0,0(v1)), so this
 * function keeps the flat scaffold symbol, as an unsized array so -G8
 * small-data gp-relative addressing does not apply to it either (matching
 * nmlModelBackBufferRequestFlags/nmlModelMpeg2CrossFadeTime above).
 */
extern int fadeOutLockedFlag[];

/* activeFadeInCancelFrames is s_inActiveFadeIn's "cancelFrames" byte (+0x28); see the
 * fadeOutLockedFlag comment above for why it stays a flat scaffold symbol. */
extern int activeFadeInCancelFrames[];

/* fadeInCancelFrames is s_inFadeIn's "cancelFrames" byte (+0x28). */
extern int fadeInCancelFrames[];

/* activeFadeOutCancelFrames is s_inActiveFadeOut's "cancelFrames" byte (+0x28). */
extern int activeFadeOutCancelFrames[];

/* fadeOutCancelFrames is s_inFadeOut's "cancelFrames" byte (+0x28). */
extern int fadeOutCancelFrames[];

/* fadeOutDisposeFlag is s_inFadeOut's "dispose" byte (+0x24). */
extern int fadeOutDisposeFlag[];

/* fadeInDisposeFlag is s_inFadeIn's "dispose" byte (+0x24). */
extern int fadeInDisposeFlag[];

extern NmlGameLoopState GameLoopState;
extern float nonLinearCameraMinAngleY;
extern float nonLinearCameraMaxAngleY;
extern f32 stealthDepthScale;
extern f32 stealthSortThreshold;
extern f32 stealthDistanceTolerance;
extern f32 shapeWeightEpsilon;
extern f32 entryClipWidthScale;
extern f32 entryClipPoolHead;
extern f32 entryClipPoolBody;
extern f32 entryClipPoolTail;
extern f32 entryClipPoolInner;
extern f32 entryClipPoolOuter;
extern f32 entryClipPoolUpper;
extern f32 entryClipPoolLower;
extern f32 entryClipPoolMid;
extern f32 entryClipPoolRear;
extern char faceNameSuffix[];
extern float calcSampleThreshold;
extern int g_nPacketOver;
extern const DoublewordAlignedVector4 proRealOrigin;
extern float circleViewLimitMin;
extern const char magicCarpetComeName[];
extern const char eyeMaterialName[];
extern float entryPartsClipScale;

static void CONSTRUCT_MODELSYSTEM(void);

/* INIT_BACK_BUFFER (above) is still INCLUDE_ASM; declare it so its caller
 * does not see an implicit declaration. */
static void INIT_BACK_BUFFER(void);

static void fade_render(XglPacket *packet, FadeControl *control);

/* fade_set (near the top of this TU) is still INCLUDE_ASM; declare it so
 * this caller does not see an implicit declaration. */
static void fade_set(FadeControl *, float *, int, int, int, int, int);

/* nmlModelFogPara (above) is still INCLUDE_ASM; declare it so its caller
 * does not see an implicit declaration. */
static void nmlModelFogPara(Vector4 *fog, float fogNear, float fogFar, float fogMin, float fogMax);

extern unsigned int strlen(const char *string);

extern int memcmp(const void *left, const void *right, unsigned int count);
extern int strcmp(const char *left, const char *right);
extern char *strstr(const char *string, const char *pattern);

/* nmlModelLexDataCheck remains assembly-owned below. */
int nmlModelLexDataCheck(NmlModel *model);

/* nmlModelDirectXtxSub (above) is still INCLUDE_ASM; declare it so this
 * caller does not see an implicit declaration. */
static void nmlModelDirectXtxSub(void *data, int modelId, int nextModelId);

/*
 * CONSTRUCT_CIRCLR_SHADOW (near the top of this TU) is still INCLUDE_ASM
 * and nmlModelClear is defined below (this TU); declare them, and the
 * s_inMapHandle array nmlModelClear also declares nearer its own use, so
 * this caller does not see an implicit declaration.
 */
static void CONSTRUCT_CIRCLR_SHADOW(void);
/* CLEAR_LAYOUT_MODEL and CLEAR_MODEL_ENTRY (near the top of this TU) are
 * still INCLUDE_ASM; declare them so this caller does not see an implicit
 * declaration. */
static void CLEAR_LAYOUT_MODEL(LayoutStore *arg0);
static void CLEAR_MODEL_ENTRY(void);

/* FLUSH_MODELSYSTEM (above) is still INCLUDE_ASM; declare it so this
 * caller does not see an implicit declaration. */
static void FLUSH_MODELSYSTEM(void);

extern float xglPointLength(Vector4 *first, Vector4 *second);
extern void nmlFilterStealthMake(int, f32, void *, LayoutSlot *);
extern f32 tanf(f32);
extern int nmlModelCalcClipStudio(Vector4 *, int);
static float deapth_for_studio(int studio, Vector4 *position, float scale);
extern void nmlPacketAddGsZbuf(int);
extern void nmlPacketAddGsFBA(int);
extern void nmlPacketAddGsPixeltest(int);
extern void nmlPacketAddGsAlpha(u64 alpha);
extern void nmlPacketAddGifTagStandard(int, int);
extern void nmlPacketAddScreen(const StudioCamera *);

extern void nmlPacketAddTransMicrocode(const NmlModelBlock *,
                                       const LayoutStore *);

extern void nmlPacketAddExecProg(NmlModelBlock *,
                                 LayoutStore *, int, int);

extern int nmlPacketAddParts(NmlModelBlock *,
                                       LayoutStore *, StudioCamera *);

extern void nmlPacketAddBlockMaterial(NmlModelBlock *,
                                      LayoutStore *, int, int);

extern void nmlFilterSetFrameAlphaClear(void);
extern void nmlPacketAddGsFBA1(unsigned int);
extern void nmlFilterSetFrameToBuffer(int);
static void AlphaGroupSort(void);
extern void nmlPacketAddTransMicrocodeInit(void);
extern void nmlPacketAddGsZbuf1(unsigned int);
extern void nmlPacketAddGsPixeltest1(unsigned int);
extern void nmlPacketAddGsAlpha1(unsigned long long);
extern void nmlPacketAddGsTexture(void);
extern void nmlPacketAddGsClamp(unsigned int);
extern void nmlFilterSetBufferRender(float);
extern void sefDrawEffect(void);

static int nmlModelRenderProreal(LayoutStore *model, int frame);
void nmlPacketSendCircleTexture(void *shadow, void *clut);
static void set_circle_shadow_ratio(int scale);
static void _WeightToGlobalVec(Vector4 *destination, Matrix4 matrix, Matrix4 weights);
void nmlFadePacketWrite(XglPacket *packet);
static void AlphaGroupLastEntry(void);
static int nmlModelRenderTexture(NmlBlockGroup *, int, int);
static int nmlModelRenderCircle(const NmlBlockGroup *group, int pitch, int frame);
static int nmlModelRenderDropCircle(const NmlBlockGroup *, int, StudioCamera *);

static int nmlModelRenderDrop(NmlBlockGroup *range,
                              int step, StudioCamera *screen, int mode);

extern void nmlPacketClrModelCache(void);
extern void nmlPacketAddGsFogCol(const float color[3]);
extern void nmlPacketAddGifTag(NmlModelBlock *, LayoutStore *, int);
extern void nmlPacketAddTextureParam(NmlModelBlock *, LayoutStore *);
extern void nmlPacketAddPixelControl(NmlModelBlock *, LayoutStore *, int);
extern int nmlPacketAddTexture(LayoutStore *);
extern void nmlPacketAddReflectParam(NmlModelBlock *, LayoutStore *);
extern void nmlPacketAddFog(LayoutStore *, int);
extern void nmlPacketAddReflRot(NmlModelBlock *, LayoutStore *);
extern void nmlFilterBackClear(void);
extern void nmlFilterSetFlatRender(LayoutStore *);
extern void nmlFilterSetPacket(LayoutStore *, int);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

extern void nmlPacketMakeCircleTexture(LayoutStore *model,
                                       StudioCamera *camera);

extern int nmlModelCalcClipNoCulling(Vector4 *);
extern void _ApplyMatrix(Vector4 *, Matrix4, const Vector4 *);
void xglCullingMapLastCheck(void);
void xglMemCopy2(void *destination, const void *source, int count);
void xglMemCopy4(void *destination, const void *source, int count);
void *nmlPacketSetAttributeAlloc16N(int count);
void *nmlPacketSetAttributeData64N(const void *data, int count);
unsigned char nmlModelCalcClipMat2AllCam(const void *a, const void *b, void *c);
extern int nmlFilterSetVolumeCubeRender(Vector4 *, Vector4 *);

int g_aSubWindow[4] = { 0 };

LayoutStore s_inLayout = { 0 };

static void *s_pMapLast = 0;

/*
 * Allocation counter CONSTRUCT_PARENT_BUF/FLUSH_PARENT_BUF reset to zero:
 * an index into the s_aParentBuf entries (main 0x0095bb50), also still
 * INCLUDE_ASM in this TU.
 */
static int s_nParentBuf = 0;

static int s_nToumeiNum = 0;

static unsigned int s_nShadowVec = 0;

static int s_nMapShadowParts = 0;

static float s_fSortOffsetEntry = 0.0f;

static void *s_nParent = 0;

static int s_nClip = 0;

static int s_nShapeNum = 0;

static int s_nMapClip = 0;

static int s_nPacketSignal = 0;

static int s_nUseZwrite = 0;

static int s_nUseBackBuffer = 0;

static int s_nEffectWrite = 1;

static int s_nMapLast = 0;

static int s_nFadeDoit = 0;

static int s_nPause = 0;

static int s_nMenu = 0;

static int s_nFrameLockOff = 0;

static int s_nRenderCancelOld = 0;

int g_nGsEntry = 0;

static int s_nDispVisible;

static int s_nCircleShadow;

static u64 TestEnv_0_004A9170[12];

static int s_nCount;

static int s_aShapeMathPara[10];

static int s_nStencilVal;

static int s_nRenderCancel;

static int s_aShapeMathPara[10] = { 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

float nonLinearCameraMinAngleY = 1.0471975803375244f;

float nonLinearCameraMaxAngleY = 5.235987663269043f;

f32 shapeWeightEpsilon = 0.0010000000474974513f;

f32 stealthDepthScale = 0.018999999389052391f;

f32 stealthSortThreshold = 0.0099999997764825821f;

f32 stealthDistanceTolerance = 0.10000000149011612f;

float calcSampleThreshold = 0.0099999997764825821f;

f32 entryClipWidthScale = 1.2999999523162842f;

float entryPartsClipScale = 1.2000000476837158f;

float textureTransparencyMin = 0.0010000000474974513f;

float circleViewLimitMin = 0.0010000000474974513f;

static int s_nCircleShadow = 0;

static float s_fSortOffset = 0.0f;

static int s_nDispVisible = -1;

static int s_nStencilVal = -2147483648;

static int s_nCount = 0;

static int s_nModel = 0;

static int s_nBlocks = 1;

static int s_nDirect = 0;

static int s_nBlocksAlpha = 0;

static int s_nBlocksAlphaLast = 0;

static int s_nAnotherStudio = 0;

static int s_nUseStealth = 0;

static int s_nUseGnosys = 0;

static int s_nMapAlphaEntry = 0;

static int s_nMainCameraWarp = 0;

static int s_nRenderCancel = 0;

char faceNameSuffix[8] = "_face";

const char eyeMaterialName[8] = "eye";

int g_nPacketOver = 0;

static u64 TestEnv_0_004A9170[12] = { 0x0000000000000000ULL, 0x5100000500000000ULL, 0x4000000000008001ULL, 0x000000000000eeeeULL, 0x0000000000000000ULL, 0x0000000000000050ULL, 0x0000000000000000ULL, 0x0000000000000051ULL, 0x0000000000000000ULL, 0x0000000000000052ULL, 0x0000000000000000ULL, 0x0000000000000053ULL };

static unsigned char s_aDispVisible[1024];

static int s_nAlphaGroup;

static int s_nNonAlphaGroup;

static int s_aToumeiId[16];

static LayoutSlot s_aToumei[16];

static int s_aShapeId[32];

static float s_aShapeWeight[32];

static int s_aMapLast[12];

static Vector4 s_inShadowVec;

static Vector4 s_inGblPos;

/*
 * Circle rendering copies this 0x400-byte table after the 0x20-byte prefix
 * at 0x00956920: nmlPacketSetAttributeData16N consumes 66 16-byte qwords.
 * Keep its definition between s_inGblPos and s_inProReal, matching the
 * original local-storage order. set_circle_shadow_ratio writes entries 0-7
 * and 16-23. The original storage is 0x1000 bytes (no other label until
 * s_aAlphaGroup at 0x00957940); only the first 0x400 are read.
 */
static int circleShadowRatioTable[1024];

/*
 * The renderer passes this 0x1e0-byte block to nmlPacketSetAttributeData16N
 * with count 0x1e, so the copied extent is established.  ProjectMap and
 * ProjectCircle access the listed float slots; nmlModelClear clears the word
 * at +0x1c0.  Other bytes remain unnamed because their roles are not shown
 * by this TU's C callers.
 */
static NmlProRealState s_inProReal[1];

static int s_inBackBuffer[10];

static int s_inMapHandle[4];

static FadeControl s_inFadeOut;

static FadeControl s_inFadeIn;

static FadeControl s_inActiveFadeOut;

static FadeControl s_inActiveFadeIn;

static char s_aTexName[32];

static NmlMaterialName s_aMatName;

static Matrix4 s_inGblPlace;

static struct NmlShadowTexture s_aCircleShadow;

static unsigned int s_aCircleShadowClut[8];

static NmlDirectRecord s_aDirect[5500];

static void * s_apBlock[4096];

static unsigned short s_aBlockLayout[4096];

static void * s_apBlockLast[32];

static unsigned short s_aBlockLayoutLast[32];

static LayoutStore * s_apModelLayout[512];

static Vector4 s_inMulColor;

static NmlBlockGroup s_aNonAlphaGroup[512];

static unsigned short s_aBLsave[256];

static unsigned int s_aBPsave[256];

static float s_aTexOffset[2];

static Vector4 s_inScale;

static NmlParentBuffer s_aParentBuf[512];

static Vector4 s_inGblPointC[4];

static Vector4 s_inGblFogPara;

static Vector4 s_inMainCameraPos;

static Vector4 s_inMainCameraAng;

static NmlBlockGroup s_aAlphaGroup[512];

static NmlPointQuadword s_inGblPointP[4];

static NmlPointQuadword s_inGblFogCol[1];

static inline GifCommandData *fade_packet_begin(GifCommandData *gif,
    u64 tagWord, u64 registerList)
{
    gif[1].value = registerList;
    gif[0].value = tagWord;
    return gif + 2;
}

static inline GifCommandData *fade_packet_add_ad(GifCommandData *write,
    u64 value, u64 address)
{
    write->value = value;
    write++;
    write->value = address;
    return write + 1;
}

static inline int fade_packet_close(GifCommandData *gif,
    GifCommandData *write)
{
    int qwordCount = (write - gif) / 2;

    gif[0].value += qwordCount - 1;
    return qwordCount;
}

static float _VectorLengthSQ(const Vector4 *first, const Vector4 *second)
{
    float lengthSq;

    Vu0DistanceSquaredXYZ(lengthSq, first, second);
    return lengthSq;
}

static void _CurSetMatrix(float matrix[4][4])
{
    Vu0SetCurrentMatrix(matrix);
}

static void _CurSetViewScaleTrans(const float viewScale[4], const float viewTrans[4])
{
    Vu0SetViewScaleTranslation(viewScale, viewTrans);
}

static int _CurRotTransPersClip(Vector4 *destination, const Vector4 *vector)
{
    int clipFlags;

    Vu0CurrentRotTransPersClip(clipFlags, destination, vector);
    return clipFlags;
}

static void _CurRotTransPersFog(Vector4 *destination, const Vector4 *vector, const Vector4 *fog) {
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf4, 0(%2)\n\t"
        "vmulax.xyzw ACC, vf27, vf31x\n\t"
        "vmadday.xyzw ACC, vf28, vf31y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf31z\n\t"
        "vmaddw.xyzw vf31, vf30, vf0w\n\t"
        "vdiv Q, vf0w, vf31w\n\t"
        "vmulz.w vf1, vf0, vf4z\n\t"
        "vwaitq\n\t"
        "vmula.w ACC, vf1, vf0\n\t"
        "vmaddq.w vf31, vf4, Q\n\t"
        "vmaxx.w vf31, vf31, vf4x\n\t"
        "vminiy.w vf31, vf31, vf4y\n\t"
        "sqc2 vf31, 0(%0)"
        :
        : "r"(destination), "r"(vector), "r"(fog)
        : "memory"
    );
}

static void _CurApplyMatrix(Vector4 *destination, const Vector4 *vector) {
    Vu0ApplyCurrentMatrix(destination, vector);
}

static void _WeightToGlobalVec(Vector4 *destination, Matrix4 matrix, Matrix4 weights)
{
    __asm__ __volatile__(
        "lq $2, 0(%2)\n\t"
        "lq $3, 16(%2)\n\t"
        "lq $4, 32(%2)\n\t"
        "lqc2 vf3, 48(%2)\n\t"
        "qmfc2 $5, vf0\n\t"
        "pextlw $6, $3, $2\n\t"
        "pextuw $7, $3, $2\n\t"
        "pextlw $8, $5, $4\n\t"
        "pextuw $9, $5, $4\n\t"
        "pcpyld $2, $8, $6\n\t"
        "pcpyud $3, $6, $8\n\t"
        "pcpyld $4, $9, $7\n\t"
        "qmtc2 $2, vf5\n\t"
        "qmtc2 $3, vf6\n\t"
        "qmtc2 $4, vf7\n\t"
        "vmulax.xyz ACC, vf5, vf3x\n\t"
        "vmadday.xyz ACC, vf6, vf3y\n\t"
        "vmaddz.xyz vf2, vf7, vf3z\n\t"
        "vmove.w vf2, vf0\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vsub.xyz vf2, vf0, vf2\n\t"
        "vmulax.xyzw ACC, vf27, vf5x\n\t"
        "vmadday.xyzw ACC, vf28, vf5y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf5z\n\t"
        "vmaddw.xyzw vf5, vf30, vf5w\n\t"
        "vmulax.xyzw ACC, vf27, vf6x\n\t"
        "vmadday.xyzw ACC, vf28, vf6y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf6z\n\t"
        "vmaddw.xyzw vf3, vf30, vf6w\n\t"
        "vmulax.xyzw ACC, vf27, vf7x\n\t"
        "vmadday.xyzw ACC, vf28, vf7y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf7z\n\t"
        "vmaddw.xyzw vf4, vf30, vf7w\n\t"
        "vmulax.xyzw ACC, vf27, vf2x\n\t"
        "vmadday.xyzw ACC, vf28, vf2y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf2z\n\t"
        "vmaddw.xyzw vf30, vf30, vf2w\n\t"
        "sqc2 vf30, 0(%0)"
        :
        : "r"(destination), "r"(matrix), "r"(weights)
        : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "memory");
}

static void _MinMaxSort(Vector4 *min, Vector4 *max) {
    __asm__ __volatile__(
        "lqc2 vf20, 0(%0)\n\t"
        "lqc2 vf21, 0(%1)\n\t"
        "vmini.xyz vf22, vf20, vf21\n\t"
        "vmax.xyz vf23, vf20, vf21\n\t"
        "sqc2 vf22, 0(%0)\n\t"
        "sqc2 vf23, 0(%1)"
        :
        : "r"(min), "r"(max)
        : "memory"
    );
}

void CLEAR_LAYOUT_MODEL(LayoutStore *arg0)
{
    StudioLight *light;

    xglStudioChange(0);
    xglStudioGetLight(&light);
    xglLightCalcMatrix(light);
    QuadwordCopyMatrix(arg0->fields.lightDirection, &light->color_modifier_matrix);
    QuadwordCopyMatrix(arg0->fields.lightColor, light->normal_matrix);
    QuadwordCopyMatrix(s_inGblPlace, arg0->fields.place);
    QuadwordCopy(&s_inGblPos, &arg0->slots[0x1F0 / 4]);
    arg0->slots[0x88].f = 1.0f;
    arg0->slots[0x89].f = 2.0f;
    arg0->slots[0xB0].f = 1.0f;
    arg0->slots[0x90].f = 0.0f;
    xglMatrixUnit(arg0->fields.place);
    xglMatrixUnit(arg0->fields.inversePlace);
    arg0->slots[0x8F].f = 0.0f;
    arg0->slots[0x94].i &= 0xFF000000;
    arg0->fields.alpha = 0x44;
    arg0->slots[0x8A].f = 1.0f;
    arg0->slots[0xC0].f = 1.0f;
    arg0->slots[0xC1].f = 1.0f;
    arg0->slots[0xC2].f = 1.0f;
    arg0->slots[0xC3].f = 1.0f;
    arg0->slots[0xC4].f = 0.0f;
    arg0->slots[0xC5].f = 0.0f;
    arg0->slots[0xC6].f = 0.0f;
    arg0->slots[0xC7].f = 0.0f;
    s_inScale.z = 1.0f;
    s_inScale.y = 1.0f;
    s_inScale.x = 1.0f;
    arg0->slots[0x8E].i = 117;
    arg0->slots[0x8B].f = 1.0f;
    arg0->slots[0x83].f = 1.0f;
    arg0->slots[0x82].f = 1.0f;
    arg0->slots[0x81].f = 1.0f;
    arg0->slots[0x80].f = 1.0f;
    arg0->slots[0x8D].f = 0.0f;
    arg0->slots[0xA0].f = 0.0f;
    arg0->slots[0xB2].f = 0.0f;
    arg0->slots[0x8C].f = 0.0f;
    arg0->slots[0x9D].f = 1.0f;
    arg0->slots[0x9F].f = 0.0f;
    arg0->slots[0x9A].f = 0.0f;
    arg0->slots[0xAA].f = 0.0f;
    arg0->slots[0xAC].f = 0.0f;
    arg0->slots[0xAD].f = 0.0f;
    arg0->slots[0xAE].f = 0.0f;
    arg0->slots[0xAF].f = 1.0f;
    arg0->slots[0xBC].f = 1.0f;
    arg0->slots[0xBD].f = 1.0f;
    arg0->slots[0xBE].f = 1.0f;
    arg0->slots[0xBF].f = 1.0f;
    arg0->slots[0xB1].f = 0.0f;
    QuadwordZeroMatrix(&arg0->fields.pointColor[0]);
    QuadwordZero(&arg0->slots[0x1F0 / 4]);
    QuadwordZero(&arg0->slots[0x210 / 4]);
}

static void CONSTRUCT_CIRCLR_SHADOW(void)
{
    int row;
    int column;
    struct NmlShadowTexture *shadow;
    float pixelScale = 14.0f;

    if (s_nCircleShadow != 0) {
        return;
    }
    row = 0;
    shadow = &s_aCircleShadow;

    for (; row < 64; row++) {
        float y = (float)row - 32.0f;
        for (column = 0; column < 64; column++) {
            float x = (float)column - 32.0f;
            float distance = __builtin_sqrtf(x * x + y * y);
            if (distance <= 1.0f) {
                distance = 0.0f;
            } else if (distance > 32.0f) {
                distance = 1.0f;
            } else {
                distance = (distance - 1.0f) / 31.0f;
            }
            shadow->pixels[row][column] =
                (unsigned char)((1.0f - distance) * pixelScale);
        }
    }

    {
        unsigned int *uploadHeader = s_aCircleShadowClut;

        uploadHeader[3] = 0x50000041;
        uploadHeader[4] = 0x8040;
        uploadHeader[0] = 0;
        uploadHeader[1] = 0;
        uploadHeader[2] = 0;
        uploadHeader[5] = 0x08000000;
        uploadHeader[6] = 0;
        uploadHeader[7] = 0;
        uploadHeader = s_aCircleShadow.uploadHeader;
        uploadHeader[4] = 0x8100;
        uploadHeader[3] = 0x50000101;
        uploadHeader[5] = 0x08000000;
        uploadHeader[0] = 0;
        uploadHeader[1] = 0;
        uploadHeader[2] = 0;
        uploadHeader[6] = 0;
        uploadHeader[7] = 0;
    }
    set_circle_shadow_ratio(0x1e);
    s_nCircleShadow = 1;
}

static void CONSTRUCT_ALPHA_GROUP(void)
{
    s_nAlphaGroup = 0;
    s_nNonAlphaGroup = 0;
}

static void INIT_ALPHA_GROUP(void)
{
    CONSTRUCT_ALPHA_GROUP();
}

static void FLUSH_ALPHA_GROUP(void)
{
    s_nAlphaGroup = 0;
    s_nNonAlphaGroup = 0;
}

static void CLEAR_PROREAL(NmlProRealState *proReal)
{
    proReal->projection_entry = 0;
}

static void CONSTRUCT_BACK_BUFFER(void)
{
    s_inBackBuffer[0x10 / 4] = -1;
    s_inBackBuffer[0x14 / 4] = 0;
    s_inBackBuffer[0x00 / 4] = 0;
    s_inBackBuffer[0x04 / 4] = 0;
    s_inBackBuffer[0x08 / 4] = 0;
    s_inBackBuffer[0x0c / 4] = 0;
    s_inBackBuffer[0x18 / 4] = 0;
    s_inBackBuffer[0x1c / 4] = 0;
    s_inBackBuffer[0x20 / 4] = 0;
    s_inBackBuffer[0x24 / 4] = 0;
}

static void INIT_BACK_BUFFER(void)
{
    s_inBackBuffer[0x10 / 4] = -1;
    s_inBackBuffer[0x14 / 4] = 0;
    s_inBackBuffer[0x00 / 4] = 0;
    s_inBackBuffer[0x04 / 4] = 0;
    s_inBackBuffer[0x08 / 4] = 0;
    s_inBackBuffer[0x0c / 4] = 0;
    s_inBackBuffer[0x18 / 4] = 0;
    s_inBackBuffer[0x1c / 4] = 0;
    s_inBackBuffer[0x20 / 4] = 0;
}

static void FLUSH_BACK_BUFFER(void)
{
    s_inBackBuffer[0x00 / 4] = 0;
}

static void CONSTRUCT_PARENT_BUF(void)
{
    s_nParentBuf = 0;
}

static void INIT_PARENT_BUF(void)
{
    CONSTRUCT_PARENT_BUF();
}

static void FLUSH_PARENT_BUF(void)
{
    s_nParentBuf = 0;
}

static void CONSTRUCT_MAP_HANDLE(int *mapHandle)
{
    /*
     * The original writes word 0 and puts the word-1 store in the return's
     * delay slot; the two stores are independent, and in declaration order
     * cc1 schedules them the other way round (form 01, build/form-01,
     * first difference at 0x6e8: sw zero,4(a0) for sw zero,0(a0)).
     */
    mapHandle[1] = 0;
    mapHandle[0] = 0;
}

static void INIT_MAP_HANDLE(int *mapHandle)
{
    CONSTRUCT_MAP_HANDLE(mapHandle);
}

static void CLEAR_MAP_HANDLE(int *mapHandle)
{
    mapHandle[0] = 0;
}

static void FLUSH_MAP_HANDLE(int *mapHandle)
{
    mapHandle[1] = 0;
}

static void CONSTRUCT_FADE_CONTROL(FadeControl *control)
{
    control->frame = -1;
    control->duration = 1;
    control->skipRender = 0;
    control->dispose = 0;
    control->cancelFrames = 0;
    control->startDelay = 0;
    control->locked = 0;
}

static void INIT_FADE_CONTROL(FadeControl *control)
{
    CONSTRUCT_FADE_CONTROL(control);
}

static void CLEAR_MODEL_ENTRY(void)
{
    s_nDispVisible = -1;
    s_nToumeiNum = s_nShadowVec = s_nMapShadowParts = s_aTexName[0] = s_aMatName.text[0] = 0;
    s_fSortOffsetEntry = 0.0f;
    s_nParent = 0;
    s_nClip = 0;
    s_nShapeNum = 0;
    s_nMapClip = 0;
}

static void CONSTRUCT_MODELSYSTEM(void)
{
    int primaryWindow = 2;
    int inactiveWindow = 0;
    unsigned int initialBlockCount = 1;

    g_aSubWindow[0] = primaryWindow;
    s_nModel = 0;
    s_nBlocks = initialBlockCount;
    s_nDirect = 0;
    s_nBlocksAlpha = 0;
    s_nBlocksAlphaLast = 0;
    s_nAnotherStudio = 0;
    s_nUseStealth = 0;
    s_nUseGnosys = 0;
    s_nUseZwrite = 0;
    s_nMapAlphaEntry = 0;
    s_nMapLast = 0;
    s_nMainCameraWarp = 0;
    s_nRenderCancel = 0;
    s_nRenderCancelOld = 0;
    s_nFrameLockOff = 0;
    s_nPause = 0;
    s_nMenu = 0;
    g_aSubWindow[3] = g_aSubWindow[2] = g_aSubWindow[1] = inactiveWindow;
    s_nEffectWrite = 1;
    QuadwordZeroMatrix(s_inGblPointC);
    QuadwordZero(s_inGblFogCol);
    QuadwordZero(&s_inGblFogPara);
}

static void INIT_MODELSYSTEM(void)
{
    CONSTRUCT_MODELSYSTEM();
}

static void FLUSH_MODELSYSTEM(void)
{
    StudioCamera *camera;

    s_nPacketSignal = s_nPacketSignal + 1;
    s_nBlocks = 1;
    s_nModel = 0;
    s_nDirect = 0;
    s_nBlocksAlpha = 0;
    s_nBlocksAlphaLast = 0;
    s_nAnotherStudio = 0;
    s_nUseStealth = 0;
    s_nUseGnosys = 0;
    s_nUseZwrite = 0;
    s_nUseBackBuffer = 0;
    s_nMapAlphaEntry = 0;
    s_nFrameLockOff = 0;
    camera = xglStudioSelectGetActiveCamera(0);
    if (camera != 0) {
        QuadwordCopy(&s_inMainCameraPos, &camera->position);
        QuadwordCopy(&s_inMainCameraAng, &camera->rotation);
        s_nMainCameraWarp = 0;
    }
}

void nmlModelSetBackBufferClear(void)
{
    INIT_BACK_BUFFER();
}

void nmlModelSetFaceModel(int enabled)
{
    if (enabled != 0) {
        s_inLayout.slots[0x250 / 4].i |= 0x100000;
    }
}

void nmlModelSetHumanModel(int enabled)
{
    if (enabled != 0) {
        s_inLayout.slots[0x250 / 4].i |= 0x40;
    }
}

void nmlModelSetMapClip(int enabled)
{
    if (enabled != 0) {
        s_nMapClip = 1;
    }
}

void nmlModelSendRenderCancel(void)
{
}

int nmlModelGetRenderCancel(void)
{
    return s_nRenderCancelOld;
}

void nmlModelSendBackBufferSignal(void)
{
    s_nUseBackBuffer = 1;
}

int nmlModelIsBackBufferRequest(void)
{
    return nmlModelBackBufferRequestFlags[0];
}

void nmlModelSendPauseSignal(int pause)
{
    s_nPause = pause;
}

void nmlModelSendMenuStart(void)
{
    s_nMenu = 1;
}

void nmlModelSendMenuEnd(void)
{
    s_nMenu = 0;
}

int nmlModelGetMenuStatus(void)
{
    return s_nMenu;
}

void nmlModelSendSignalStartBattle(void)
{
    s_nFrameLockOff = 1;
}

void nmlModelSendSignalEventFinish(void)
{
    s_nFrameLockOff = 1;
}

void nmlModelSendPacketChangeSignal(void)
{
    s_nPacketSignal = 0;
    nmlModelBackBufferRequestFlags[0] = 0;
}

void nmlModelSendSignalMovieStart(void)
{
    INIT_BACK_BUFFER();
    INIT_FADE_CONTROL(&s_inFadeIn);
    INIT_FADE_CONTROL(&s_inFadeOut);
    INIT_FADE_CONTROL(&s_inActiveFadeIn);
    INIT_FADE_CONTROL(&s_inActiveFadeOut);
}

void nmlModelSetMpeg2CrossFadeTime(int time)
{
    nmlModelMpeg2CrossFadeTime[0] = time;
}

static inline void NmlMovieRestoreSavedBuffer(void)
{
    nmlModelSetBackBuffer(nmlModelMpeg2CrossFadeTime[0], 1, sRender.draw_buffer_base, 1);
    nmlModelSetFadeInCancel(30);
}

/*
 * TU-local partial view of the shared render-state global sRender (main
 * 0x004A90E0, size 0x5C; see src/main/game_over.c's DrawImageRenderState,
 * src/main/map_1.c's MapRenderState, src/main/window_tex_load.c's
 * UmnRenderSize and src/main/game_defocus.c's DefocusRenderState for other
 * TUs' views of the same object). Only the halfword at +0x22 is evidenced
 * here: nmlModelSendSignalMovieFinish forwards it unchanged as the
 * "ownerIndex" argument of nmlModelSetBackBuffer and
 * nmlModelSetBackBufferToBattle.
 */
void nmlModelSendSignalMovieFinish(int mode)
{
    switch (mode - 1) {
    case 0:
        nmlModelSetBackBufferToBattle(sRender.draw_buffer_base);
        nmlModelSetFadeInCancel(30);
        nmlModelSetFadeOutCancel(30);
        break;
    case 1:
        nmlModelSetBackBuffer(0x26, 1, sRender.draw_buffer_base, 0);
        nmlModelSetFadeInCancel(30);
        break;
    case 4:
        nmlModelSetBackBuffer(0x26, 1, sRender.draw_buffer_base, 0);
        nmlModelSetFadeOutCancel(30);
        nmlModelSetFadeInCancel(30);
        break;
    case 2:
        nmlModelSetBackBuffer(3, 1, sRender.draw_buffer_base, 0);
        nmlModelSetFadeInCancel(30);
        break;
    case 3:
        NmlMovieRestoreSavedBuffer();
        break;
    }
}

int nmlModelSetRenderLevel(int level)
{
    int renderLevel = s_inLayout.slots[0x2b0 / 4].i | level;
    s_inLayout.slots[0x2b0 / 4].i = renderLevel;
    return renderLevel;
}

static void fade_render(XglPacket *packet, FadeControl *control)
{
    int mode = control->unclassified_1c;
    unsigned int zeroModeDepthAndFog = 0x00fffffc;
    unsigned int depthAndFog = 0x02ffffff;
    int frame;

    if (mode == 0) {
        depthAndFog = zeroModeDepthAndFog;
    }
    if (control->startDelay != 0) {
        control->startDelay--;
        return;
    }

    frame = control->frame;
    if (frame >= 0) {
        if (control->dispose != 0) {
            control->frame = 0;
            frame = 0;
        }
        if (frame >= 0) {
            if (control->cancelFrames == 0) {
                if (mode == 0) {
                    control->level = (int)(((float)(frame - 5) /
                        (float)control->duration) * 128.0f);
                } else {
                    control->level = (int)(((float)frame /
                        (float)control->duration) * 128.0f);
                }
                if (mode == 0) {
                    control->level = 128 - control->level;
                }
                if (control->level < 0) {
                    control->level = 0;
                }
                if (control->level > 128) {
                    control->level = 128;
                }
                if (control->skipRender == 0) {
                    GifCommandData gifData[32];
                    GifCommandData *write = fade_packet_begin(gifData,
                        0x1000000000008000ULL, 14);
                    u64 xyzControl = (u64)depthAndFog << 32;
                    u64 rgbaq;
                    int qwordCount;

                    write = fade_packet_add_ad(write, 0x01bf000001ff0000ULL, 64);
                    write = fade_packet_add_ad(write, 68, 66);
                    write = fade_packet_add_ad(write, 0x30000, 71);
                    write = fade_packet_add_ad(write, 70, 0);
                    rgbaq = (u64)control->color[0] | 0x3f80000000000000ULL;
                    rgbaq |= ((u64)control->color[2] << 16) |
                        ((u64)control->color[1] << 8);
                    rgbaq |= (u64)control->level << 24;
                    write = fade_packet_add_ad(write, rgbaq, 1);
                    write = fade_packet_add_ad(write, xyzControl | 0x72007000, 4);
                    write = fade_packet_add_ad(write, xyzControl | 0x8e009000, 4);
                    qwordCount = fade_packet_close(gifData, write);

                    sceVif1PkCnt(packet, 0);
                    sceVif1PkOpenDirectHLCode(packet, 0);
                    sceVif1PkAddDirectDataN(packet, &gifData, qwordCount);
                    sceVif1PkCloseDirectHLCode(packet);
                    frame = control->frame;
                }
            }
            if (frame >= 0) {
                int nextFrame = frame - 1;

                control->frame = nextFrame;
                if (nextFrame < 0) {
                    if (control->locked != 0) {
                        control->frame = 0;
                    } else {
                        control->skipRender = 0;
                    }
                }
            }
        }
    }
    if (control->dispose != 0 || control->cancelFrames > 0) {
        control->frame = -1;
        control->duration = 1;
        control->skipRender = 0;
        control->dispose = 0;
        control->locked = 0;
    }
}

void nmlFadePacketWrite(XglPacket *packet)
{
    fade_render(packet, &s_inActiveFadeIn);
    fade_render(packet, &s_inActiveFadeOut);
    fade_render(packet, &s_inFadeIn);
    fade_render(packet, &s_inFadeOut);
    s_nFadeDoit = 0;
}

static void fade_set(FadeControl *control, float *color, int skipRender,
                     int duration, int direction, int mode, int startDelay)
{
    float red, green, blue;

    control->frame = duration;
    control->duration = duration;
    __builtin_memcpy(&red, &color[0], sizeof(red));
    control->color[0] = (int)(red * 255.0f);
    __builtin_memcpy(&green, &color[1], sizeof(green));
    control->color[1] = (int)(green * 255.0f);
    __builtin_memcpy(&blue, &color[2], sizeof(blue));
    control->color[2] = (int)(blue * 255.0f);
    control->skipRender = skipRender;
    control->unclassified_1c = direction;
    control->unclassified_20 = mode;
    control->startDelay = startDelay;
    control->level = 0;
    control->dispose = 0;
    if (duration < 0) {
        control->frame = -1;
    }
    sRender.fade_callback = nmlFadePacketWrite;
}

void nmlModelSetFadeOutLock(void) {
    fadeOutLockedFlag[0] = 1;
}

void nmlModelSetFadeOutLockOff(void) {
    fadeOutLockedFlag[0] = 0;
}

void nmlModelSetActiveFadeInCancel(int frames) {
    activeFadeInCancelFrames[0] = (frames < 0) ? 0 : frames;
}

void nmlModelSetFadeInCancel(int frames) {
    fadeInCancelFrames[0] = (frames < 0) ? 0 : frames;
}

void nmlModelSetActiveFadeOutCancel(int frames) {
    activeFadeOutCancelFrames[0] = (frames < 0) ? 0 : frames;
}

void nmlModelSetFadeOutCancel(int frames) {
    fadeOutCancelFrames[0] = (frames < 0) ? 0 : frames;
}

void nmlModelFadeDoit(void)
{
    if (s_inFadeIn.cancelFrames != 0) {
        s_inFadeIn.cancelFrames--;
    }
    if (s_inFadeOut.cancelFrames != 0) {
        s_inFadeOut.cancelFrames--;
    }
    if (s_inActiveFadeIn.cancelFrames != 0) {
        s_inActiveFadeIn.cancelFrames--;
    }
    if (s_inActiveFadeOut.cancelFrames != 0) {
        s_inActiveFadeOut.cancelFrames--;
    }
}

void nmlModelSetFadeDoit(void)
{
    s_nFadeDoit = 1;
}

void nmlModelSetFadeOutDispose(void) {
    fadeOutDisposeFlag[0] = 1;
}

void nmlModelSetFadeInDispose(void) {
    fadeInDisposeFlag[0] = 1;
}

void nmlModelSetFadeOut(int duration, int mode) {
    fade_set(&s_inFadeOut, GameLoopState.color, GameLoopState.level, duration + 2, 0, mode, 0);
}

void nmlModelSetFadeIn(int duration, int mode)
{
    float zeroValue;

    fade_set(&s_inFadeIn, GameLoopState.previousFade.color,
             GameLoopState.previousFadeMode, duration, 1, mode, 0);
    zeroValue = 0.0f;
    GameLoopState.level = 0;
    GameLoopState.previousFade.color[2] = zeroValue;
    GameLoopState.previousFade.color[1] = zeroValue;
    GameLoopState.previousFade.color[0] = zeroValue;
    GameLoopState.previousFade.duration = 30.0f;
    GameLoopState.previousFadeMode = 0;
    GameLoopState.fade.color[2] = zeroValue;
    GameLoopState.fade.color[1] = zeroValue;
    GameLoopState.fade.duration = 30.0f;
    GameLoopState.fade.color[0] = zeroValue;
}

void nmlModelSetFadeInInterrupt(int frames, float red, float green, float blue)
{
    s_inFadeIn.level = 0;
    s_inFadeIn.duration = frames;
    s_inFadeIn.frame = frames;
    s_inFadeIn.color[0] = (int) (red * 255.0f);
    s_inFadeIn.color[1] = (int) (green * 255.0f);
    s_inFadeIn.color[2] = (int) (blue * 255.0f);
}

void nmlModelSetActiveFadeOut(int duration, int packedColor, int startDelay)
{
    float color[3];
    u32 rgb = packedColor;

    if (s_inActiveFadeOut.frame < 0) {
        color[0] = (float)(rgb & 0xff) * 0.0078125f;
        color[1] = (float)((rgb >> 8) & 0xff) * 0.0078125f;
        color[2] = (float)((rgb >> 16) & 0xff) * 0.0078125f;
        fade_set(&s_inActiveFadeOut, color, 0, duration + 2, 0, 1, startDelay);
    }
}

void nmlModelSetActiveFadeIn(int duration, int packedColor, int startDelay)
{
    float color[3];
    u32 rgb = packedColor;

    color[0] = (float)(rgb & 0xff) * 0.0078125f;
    color[1] = (float)((rgb >> 8) & 0xff) * 0.0078125f;
    color[2] = (float)((rgb >> 16) & 0xff) * 0.0078125f;
    fade_set(&s_inActiveFadeIn, color, 0, duration, 1, 1, startDelay);
}

int nmlModelGetFadeLevel(void)
{
    int active = 1;

    if (s_inFadeIn.frame < 0) {
        active = 0;
    }
    if (s_inFadeOut.frame >= 0) {
        active = 1;
    }
    if (s_inActiveFadeIn.frame >= 0) {
        active = 1;
    }
    if (s_inActiveFadeOut.frame >= 0) {
        active = 1;
    }
    return active;
}

int nmlModelAskNonLinearCamera(void)
{
    StudioCamera *camera;
    Vector4 angleDelta;
    float length;
    float angleY;

    if (s_nMainCameraWarp == 0) {
        camera = xglStudioSelectGetActiveCamera(0);
        if (camera != 0) {
            s_nMainCameraWarp = 1;
            length = xglPointLength(&s_inMainCameraPos, &camera->position.vector);
            if (length >= 1.0f) {
                s_nMainCameraWarp = 2;
            } else {
                Vu0SubVectorXYZ(&angleDelta, &s_inMainCameraAng, &camera->rotation);
                angleY = __builtin_fabsf(angleDelta.y);
                if (nonLinearCameraMinAngleY < angleY && angleY < nonLinearCameraMaxAngleY) {
                    s_nMainCameraWarp = 2;
                }
            }
        }
    }
    return s_nMainCameraWarp == 2;
}

void nmlModelGetGblPosition(Vector4 *position)
{
    QuadwordCopy(position, &s_inGblPos);
    position->w = 1.0f;
}

/* Battle and event-skip configure the same special back-buffer request:
 * a 0x28-frame time, the control state and the owner, written as one group.
 * The do/while (0) group is what keeps the original store order: without it
 * cc1 schedules the +0x20 clear ahead of the owner and +0x04 stores. */
void nmlModelSetBackBufferToBattle(int ownerIndex)
{
    do {
        s_inBackBuffer[6] = 0x28;
        s_inBackBuffer[5] = 1;
        s_inBackBuffer[0] = 1;
        s_inBackBuffer[4] = ownerIndex;
        s_inBackBuffer[1] = 0;
    } while (0);
    s_inBackBuffer[8] = 0;
}

void nmlModelSetBackBufferToEventSkip(int ownerIndex)
{
    do {
        s_inBackBuffer[6] = 0x28;
        s_inBackBuffer[5] = 1;
        s_inBackBuffer[0] = 1;
        s_inBackBuffer[4] = ownerIndex;
        s_inBackBuffer[1] = 0;
    } while (0);
    s_inBackBuffer[8] = 0;
}

void nmlModelSetBackBuffer(int modelId, int duration, int ownerIndex, int value)
{
    s_inBackBuffer[8 / 4] = modelId;
    s_inBackBuffer[0x10 / 4] = ownerIndex;
    s_inBackBuffer[0x20 / 4] = value;
    s_inBackBuffer[0xC / 4] = duration;
    s_inBackBuffer[0x14 / 4] = 0;
    s_inBackBuffer[0x00 / 4] = 0;
    s_inBackBuffer[0x04 / 4] = 0;
    if (duration <= 0) {
        s_inBackBuffer[0xC / 4] = 1;
    }
}

void nmlModelSetEffectWrite(int enabled)
{
    s_nEffectWrite = enabled;
}

void nmlModelUseSubWindow(unsigned int index, int enabled) {
    if (index < 4U) {
        g_aSubWindow[index] = enabled;
    }
}

void nmlModelSetWindow(u32 window) {
    if (window < 4U) {
        s_inLayout.slots[0x27c / 4].i = window;
    }
}

void nmlModelSetSortOffset(float offset)
{
    s_fSortOffsetEntry = offset;
}

void nmlModelSetGlobalPointLight(int index, const Vector4 *position, const Vector4 *color)
{
    if (index < 3U) {
        NmlPointQuadword *point = &s_inGblPointP[index];

        QuadwordCopy(point, position);
        xglVectorScaleXYZ(&s_inGblPointC[index], color, 128.0f);
        s_inLayout.fields.render_status |= 0x02000000;
    }
}

void nmlModelSetGlobalPointLightCol(int index, const Vector4 *color)
{
    if (index < 3U) {
        xglVectorScaleXYZ(&s_inGblPointC[index], color, 128.0f);
        s_inLayout.fields.render_status |= 0x02000000;
    }
}

void nmlModelSetGlobalPointLightPos(unsigned int index, const Vector4 *position)
{
    if (index < 3) {
        NmlPointQuadword *destination = &s_inGblPointP[index];
        QuadwordCopy(destination, position);
        s_inLayout.fields.render_status |= 0x02000000;
    }
}

void nmlModelSetGlobalPointLightReset(void)
{
    s_inLayout.fields.render_status &= 0xfdffffffu;
    QuadwordZero(s_inGblPointC);
    __asm__ __volatile__("sq $0, 16(%0)" : : "r"(s_inGblPointC) : "memory");
    __asm__ __volatile__("sq $0, 32(%0)" : : "r"(s_inGblPointC) : "memory");
    __asm__ __volatile__("sq $0, 48(%0)" : : "r"(s_inGblPointC) : "memory");
}

void nmlModelSetPointLight(unsigned int index, const Vector4 *position, const Vector4 *color)
{
    if (index < 3) {
        QuadwordCopy(&s_inLayout.fields.pointPosition[index], position);
        xglVectorScaleXYZ(&s_inLayout.fields.pointColor[index], color, 128.0f);
        s_inLayout.fields.render_status |= 0x1000;
    }
}

void nmlModelSetSpecularOff(int disableSpecular) {
    if (disableSpecular != 0) {
        s_inLayout.slots[0x2b0 / 4].i |= 2;
    }
}

void nmlModelSetShadowMapId(void)
{
}

void nmlModelSetShadowHeight(float height)
{
    s_inLayout.slots[0x2ac / 4].f = height;
    s_inLayout.slots[0x2a8 / 4].i = 1;
}

void nmlModelSetShadowVec(const Vector4 *shadowVector)
{
    QuadwordCopy(&s_inShadowVec, shadowVector);
    s_nShadowVec = 1;
}

void nmlModelSetTexMap(int mode, int size, float weight)
{
    LayoutStore *layout = &s_inLayout;
    unsigned long long texMapState;

    layout->slots[0x268 / 4].i = mode;
    layout->slots[0x270 / 4].i = size;
    layout->slots[0x274 / 4].f = weight;

    if (layout->slots[0x270 / 4].i < 0) {
        layout->slots[0x270 / 4].i = 0;
    }
    if (layout->slots[0x270 / 4].i >= 0x81) {
        layout->slots[0x270 / 4].i = 0x80;
    }

    texMapState = layout->qwords[0x268 / 8];
    if ((texMapState & 0x10001) == 0x10001) {
        layout->slots[0x250 / 4].i |= 0x40000;
    } else if ((texMapState & 0x20001) == 0x20001) {
        layout->slots[0x250 / 4].i |= 0x200000;
    } else if ((texMapState & 0x30000) == 0x30000) {
        layout->slots[0x250 / 4].i |= 0x400000;
    } else {
        layout->slots[0x250 / 4].i |= 0x4000;
    }
}

void nmlModelSetProreal(NmlProRealState *input)
{
    DoublewordAlignedVector4 transformOrigin;
    Vector4 scale;
    Matrix4 scaledTransform;
    int textureStartX;
    int textureEndX;
    int textureStartY;
    int textureEndY;
    u64 textureXDirection;
    u64 textureYDirection;
    int screenWidth;
    int screenHeight;
    int textureMode;

    screenWidth = 0x400;
    screenHeight = 0x200;
    scale.x = (float)input->fields.unmodeled_108 * 0.015625f * input->fields.unmodeled_120;
    scale.y = (float)input->fields.unmodeled_10c * 0.015625f * input->fields.unmodeled_124;
    transformOrigin = proRealOrigin;
    scale.w = 1.0f;
    scale.z = 1.0f;
    xglMatrixScale(scaledTransform, input->fields.matrix_000, &scale.x);
    xglVectorMulMat(&input->fields.transformOrigin, scaledTransform,
                    &transformOrigin);

    Vu0SubVectorXYZ(&input->fields.unmodeled_160, &input->fields.transformOrigin, &scaledTransform[3]);

    xglMatrixInverse(input->fields.inverseTransform, scaledTransform);
    xglVectorNormal(&input->fields.unmodeled_160, &input->fields.unmodeled_160);

    textureMode = input->fields.unmodeled_114;
    switch (textureMode) {
    case 20: {
        int textureBase = input->fields.unmodeled_116 << 5;
        input->fields.textureWord =
            (u64)(textureBase | 0x29440000) |
            ((u64)(textureBase + input->fields.unmodeled_118) << 37) |
            0x2000000640000000ULL;
        break;
    }
    case 19: {
        int textureBase = input->fields.unmodeled_116 << 5;
        input->fields.textureWord =
            (u64)(textureBase | 0x29340000) |
            ((u64)(textureBase + input->fields.unmodeled_118) << 37) |
            0x2000000640000000ULL;
        break;
    }
    case 0:
        screenWidth = 0x200;
        input->fields.textureWord = (u64)(input->fields.unmodeled_116 << 5) |
                               0x24020000ULL | 0x2000000640000000ULL;
        break;
    }

    textureXDirection = input->fields.unmodeled_110 != 0 ? 3 : 2;
    textureYDirection = input->fields.unmodeled_112 != 0 ? 3 : 2;
    textureStartX = input->fields.unmodeled_110 != 0 ? input->fields.unmodeled_108 - 1 : input->fields.unmodeled_100;
    textureEndX = input->fields.unmodeled_110 != 0 ? input->fields.unmodeled_100 :
                  input->fields.unmodeled_100 + input->fields.unmodeled_108 - 1;
    textureStartY = input->fields.unmodeled_112 != 0 ? input->fields.unmodeled_10c - 1 : input->fields.unmodeled_104;
    textureEndY = input->fields.unmodeled_112 != 0 ? input->fields.unmodeled_104 :
                  input->fields.unmodeled_104 + input->fields.unmodeled_10c - 1;

    input->fields.clampWord = textureXDirection |
        (textureYDirection << 2) |
        ((u64)textureStartX << 4) |
        ((u64)textureEndX << 14) |
        ((u64)textureStartY << 24) |
        ((u64)textureEndY << 34);
    input->fields.textureUScale = (float)input->fields.unmodeled_108 / (float)screenWidth;
    input->fields.textureUOffset = (float)input->fields.unmodeled_100 / (float)screenWidth +
                            input->fields.textureUScale * 0.5f;
    input->fields.textureVScale = -(float)input->fields.unmodeled_10c / (float)screenHeight;
    input->fields.textureVOffset = (float)input->fields.unmodeled_104 / (float)screenHeight -
                            input->fields.textureVScale * 0.5f;

    QuadwordCopy(&input->fields.unmodeled_0c0[0], &s_inProReal[0].fields.unmodeled_170[0]);
    QuadwordZero(&input->fields.unmodeled_0c0[1]);
    QuadwordZero(&input->fields.unmodeled_0c0[2]);
    QuadwordCopy(&input->fields.unmodeled_0c0[3], &s_inProReal[0].fields.unmodeled_170[1]);
    QuadwordCopy(&input->fields.unmodeled_080[0], &s_inProReal[0].fields.unmodeled_160);
    input->fields.unmodeled_11c = 0;
    s_inLayout.fields.render_status |= 0x800;
    s_inProReal[0] = *input;
}

int nmlModelSetTexProreal(void) {
    int renderStatus = s_inLayout.fields.render_status | 0x800;
    s_inLayout.fields.render_status = renderStatus;
    return renderStatus;
}

int nmlModelSetAxisSymmetry(float symmetry) {
    int flags;

    s_inLayout.slots[0x230 / 4].f = symmetry * 0.0f;
    flags = s_inLayout.slots[0x230 / 4].i | 1;
    s_inLayout.slots[0x230 / 4].i = flags;
    return flags;
}

void nmlModelSetParent(void *parent)
{
    s_nParent = parent;
}

void nmlModelSetRenderStatus(int status)
{
    if (status == 0x100) {
        s_inLayout.fields.render_status |= 0x100;
    } else if (status == 0x200) {
        s_inLayout.fields.render_status |= 0x200;
    }
}

void nmlModelSetShapeId(int shapeId, float weight)
{
    extern int s_aShapeId[];
    extern float s_aShapeWeight[];

    if (s_nShapeNum < 32) {
        s_aShapeId[s_nShapeNum] = shapeId;
        s_aShapeWeight[s_nShapeNum++] = weight;
    }
}

void nmlModelSetToumeiParts(int partId, float alpha)
{
    extern int s_aToumeiId[];
    extern LayoutSlot s_aToumei[];

    if (s_nToumeiNum < 16) {
        s_aToumeiId[s_nToumeiNum] = partId;
        s_aToumei[s_nToumeiNum++].f = alpha;
    }
}

void nmlModelSetFilter(int filter, float start, float end)
{
    s_inLayout.slots[0x234 / 4].i = filter;
    if (start < 0.0f) {
        start = 0.0f;
    }
    if (start > 128.0f) {
        start = 128.0f;
    }
    s_inLayout.slots[0x1e0 / 4].f = start;
    s_inLayout.slots[0x1e4 / 4].f = end;
    if ((filter & 2) != 0) {
        s_nUseStealth = 1;
        if (GameLoopState.stealthMode == 2 && SCRIPT_getCfTime() < 2) {
            s_nUseStealth = 0;
        }
    }
}

void nmlModelSetPixelAlpha(int alpha)
{
    if (alpha <= 0) {
        alpha = 1;
    }
    if (alpha >= 128) {
        alpha = 127;
    }
    s_inLayout.fields.pixelAlpha = alpha;
}

void nmlModelSetPartsPixelAlpha(int partId, int alpha)
{
    if (alpha <= 0) {
        alpha = 1;
    }
    if (alpha >= 128) {
        alpha = 127;
    }
    if (s_inLayout.fields.partPixelAlphaCount < 4) {
        s_inLayout.fields.partPixelAlpha[s_inLayout.fields.partPixelAlphaCount].alpha = alpha;
        s_inLayout.fields.partPixelAlpha[s_inLayout.fields.partPixelAlphaCount].partId = partId;
        ++s_inLayout.fields.partPixelAlphaCount;
    }
}

void nmlModelSetScale(const Vector4 *scale)
{
    QuadwordCopy(&s_inScale, scale);
    if (s_inScale.x < s_inScale.y) {
        s_inScale.x = s_inScale.y;
    }
    if (s_inScale.x < s_inScale.z) {
        s_inScale.x = s_inScale.z;
    }
    if (s_inScale.x < 1.0f) {
        s_inScale.x = 1.0f;
    }
    s_inLayout.slots[0x22c / 4].f = s_inScale.x;
}

void nmlModelSetClip(int clip)
{
    s_nClip = clip;
}

void nmlModelSetAlpha(u64 alpha)
{
    s_inLayout.fields.alpha = alpha;
}

void nmlModelSetStencil(int enabled)
{
    if (enabled) {
        s_inLayout.fields.render_status |= NML_RENDER_STENCIL;
    }
}

void nmlModelSetZwrite(int enabled)
{
    if (enabled) {
        s_inLayout.fields.render_status |= NML_RENDER_ZWRITE;
    }
}

void nmlModelSetTransparency(float transparency)
{
    if (transparency < 0.0f) {
        transparency = 0.0f;
    }
    if (1.0f < transparency) {
        transparency = 1.0f;
    }
    s_inLayout.slots[0x220 / 4].f = transparency;
    s_inLayout.slots[0x2c0 / 4].f = transparency;
}

void nmlModelSetReflTransparency(float transparency)
{
    if (transparency < 0.0f) {
        transparency = 0.0f;
    }
    if (1.0f < transparency) {
        transparency = 1.0f;
    }
    s_inLayout.slots[0x224 / 4].f = transparency;
}

void nmlModelSetToumei(int enabled)
{
    if (enabled) {
        s_inLayout.fields.render_status |= NML_RENDER_TOUMEI;
    }
}

void nmlModelSetLight(Matrix4 *direction, Matrix4 *color)
{
    QuadwordCopyMatrix(&s_inLayout.fields.lightDirection, direction);
    QuadwordCopyMatrix(&s_inLayout.fields.lightColor, color);
    s_inLayout.fields.render_status |= 0x10;
}

void nmlModelSetTexture(const char *texture)
{
    /* Accept only a non-null, in-range, qword-aligned pointer whose header
     * carries the "XTX" texture-resource signature: texture[0..2] ==
     * 'X','T','X' (texture[2] is compared against texture[0], not a literal,
     * per the original disassembly). */
    if (texture == 0 ||
        (u32)texture - NML_TEXTURE_RAM_BASE > NML_TEXTURE_RAM_MAX_OFFSET ||
        ((u32)texture & NML_TEXTURE_ALIGN_MASK) != 0 ||
        texture[0] != 'X' || texture[1] != 'T' || texture[2] != texture[0]) {
        return;
    }
    s_inLayout.fields.texture = texture;
}

void nmlModelSetTexfunc(int texfunc)
{
    s_inLayout.fields.texfunc = texfunc;
}

/* Stores the current model matrix pointer in the layout store at +0x23c. */
void nmlModelSetMatrix(void *matrix)
{
    s_inLayout.fields.currentMatrix = matrix;
}

void nmlModelSetPlace(Matrix4 *place)
{
    QuadwordCopyMatrix(&s_inLayout.fields.place, place);
    xglMatrixInverse(s_inLayout.fields.inversePlace, *place);
    s_inLayout.fields.render_status |= 1;
}

void nmlModelSetTexOffset(const char *name, float offsetU, float offsetV)
{
    __builtin_memcpy(s_aTexName, name, sizeof(s_aTexName));
    s_aTexName[31] = 0;
    s_aTexOffset[0] = offsetU;
    s_aTexOffset[1] = offsetV;
}

void nmlModelSetMatOffset(const NmlMaterialName *name, const Vector4 *color)
{
    s_aMatName = *name;
    s_aMatName.text[31] = 0;
    QuadwordCopy(&s_inMulColor, color);
}

void nmlModelSetMulColor(const Vector4 *color)
{
    QuadwordCopy(&s_inLayout.fields.multiplyColor, color);
}

void nmlModelSetFogCol(const Vector4 *color)
{
    QuadwordCopy(&s_inLayout.fields.fogColor, color);
    s_inLayout.fields.render_status |= NML_RENDER_FOG;
}

/* The four fog distances of RgFog.dist (defaults 50, 250, 0, 1). */
static void nmlModelFogPara(Vector4 *fog, float fogNear, float fogFar, float fogMin, float fogMax)
{
    float shadeMax;
    float shadeMin;

    shadeMax = (1.0f - fogMax) * 255.0f;
    shadeMin = (1.0f - fogMin) * 255.0f;
    if (shadeMax < 0.0f) {
        shadeMax = 0.0f;
    }
    if (shadeMax > 255.0f) {
        shadeMax = 255.0f;
    }
    if (shadeMin < 0.0f) {
        shadeMin = 0.0f;
    }
    if (shadeMin > 255.0f) {
        shadeMin = 255.0f;
    }
    fog->x = shadeMax;
    fog->y = shadeMin;
    fog->w = (fogFar * fogNear * (shadeMin - shadeMax)) / (fogFar - fogNear);
    fog->z = ((((shadeMax - shadeMin) * (fogFar + fogNear)) / (fogFar - fogNear)) + (shadeMax + shadeMin)) * 0.5f;
}

void nmlModelSetFogDist(float fogNear, float fogFar, float fogMin, float fogMax)
{
    nmlModelFogPara(&s_inLayout.fields.fogPara, fogNear, fogFar, fogMin, fogMax);
    s_inLayout.fields.render_status |= NML_RENDER_FOG;
}

void nmlModelSetGlobalFogCol(const NmlPointQuadword *color)
{
    QuadwordCopy(s_inGblFogCol, color);
    s_inLayout.fields.render_status |= 0x01000000;
}

void nmlModelSetGlobalFogDist(float fogNear, float fogFar, float fogMin, float fogMax)
{
    nmlModelFogPara(&s_inGblFogPara, fogNear, fogFar, fogMin, fogMax);
    s_inLayout.fields.render_status |= 0x01000000;
}

unsigned int nmlModelGetFogPara(const Vector4 *position)
{
    const Vector4 *fog;
    StudioCamera *camera;
    Vector4 projected;
    unsigned int result;

    result = 0xff;
    fog = &s_inGblFogPara;
    if (s_inLayout.fields.render_status & NML_RENDER_FOG) {
        fog = &s_inLayout.fields.fogPara;
    }
    camera = xglStudioSelectGetActiveCamera(LAYOUT_S32(0x27c));
    if (camera != 0) {
        _CurSetMatrix(camera->viewMatrix);
        _CurRotTransPersFog(&projected, position, fog);
        result = (unsigned int) projected.w;
    }
    return result;
}

unsigned int nmlModelGetFogParaStudio(const Vector4 *position, int studio)
{
    const Vector4 *fog;
    StudioCamera *camera;
    Vector4 projected;
    unsigned int result;

    result = 0xff;
    fog = &s_inGblFogPara;
    if ((s_inLayout.fields.render_status & 2) != 0) {
        fog = &s_inLayout.fields.fogPara;
    }
    camera = xglStudioSelectGetActiveCamera(studio);
    if (camera != 0) {
        _CurSetMatrix(camera->viewMatrix);
        _CurRotTransPersFog(&projected, position, fog);
        result = (unsigned int) projected.w;
    }
    return result;
}

int nmlModelSetGlobalFogReset(void) {
    int renderStatus = s_inLayout.fields.render_status & 0xfeffffff;
    s_inLayout.fields.render_status = renderStatus;
    return renderStatus;
}

void nmlModelSetFogCancel(void) {
    int renderStatus = s_inLayout.fields.render_status | 0x2000;
    s_inLayout.fields.render_status = renderStatus;
}

void nmlModelInitPartsVisible(NmlModel *model, int visible)
{
    int *blockOffsets;
    int blockIndex;
    NmlModelBlock *block;

    if (nmlModelLexDataCheck(model) != 0) {
        return;
    }
    blockOffsets = model->blockOffsets;
    block = (NmlModelBlock *)((char *)model + blockOffsets[0]);
    for (blockIndex = 0; blockIndex < model->blockCount; blockIndex++) {
        block = (NmlModelBlock *)((char *)model + blockOffsets[blockIndex]);
        if (visible != 0) {
            block->flags = block->flags & ~NML_BLOCK_HIDDEN;
        } else {
            block->flags |= NML_BLOCK_HIDDEN;
        }
    }
}

void nmlModelSetMapEntry(void)
{
    int *mapHandle = s_inMapHandle;

    s_inLayout.fields.render_status |= NML_RENDER_MAP_ENTRY;
    mapHandle[0] = 1;
}

void nmlModelSetShadowMapEntry(void) {
    s_inLayout.fields.render_status |= 0x10000;
}

/* The map callers pass halfword part IDs; the layout has eight shadow slots. */
void nmlModelSetMapShadowParts(int partId)
{
    if (s_nMapShadowParts < 8) {
        s_inLayout.fields.mapShadowParts[s_nMapShadowParts++] = partId;
    }
}

void nmlModelSetMapLastEntry(void *map, int blockId)
{
    extern void *s_pMapLast;
    extern int s_aMapLast[];

    if (s_nMapLast < 12) {
        s_pMapLast = map;
        s_aMapLast[s_nMapLast++] = blockId;
    }
}

void nmlModelSetMapLastInit(void)
{
    s_nMapLast = 0;
}

void nmlModelSetPartsVisible(NmlModel *model, int partId, int visible)
{
    int *blockOffsets;
    int blockIndex;
    NmlModelBlock *block;
    u32 hideBits;
    u32 keepBits;

    if (nmlModelLexDataCheck(model) != 0 || partId >= model->partCount) {
        return;
    }
    blockOffsets = model->blockOffsets;
    if ((((NmlModelBlock *)((char *)model + blockOffsets[0]))->materialFlags & NML_BLOCK_MATERIAL_NAMED_PARTS) != 0) {
        hideBits = NML_BLOCK_HIDDEN;
        keepBits = 0xffffffffu;
        if (visible != 0) {
            hideBits = 0;
            keepBits = ~NML_BLOCK_HIDDEN;
        }
        for (blockIndex = 0; blockIndex < model->blockCount; blockIndex++) {
            block = (NmlModelBlock *)((char *)model + blockOffsets[blockIndex]);
            if (block->partId == partId) {
                block->flags = (block->flags | hideBits) & keepBits;
            }
        }
    } else if (visible == 0 && s_nDispVisible < 1023) {
        s_aDispVisible[++s_nDispVisible] = (unsigned char) partId;
    }
}

void nmlModelSetNameVisible(NmlModel *model, const char *name, int visible, int match)
{
    int *block_offsets;
    NmlModelBlock *first_block;
    NmlModelBlock *block;
    u32 set_flags;
    u32 keep_flags;
    int block_index;

    if (nmlModelLexDataCheck(model) != 0) {
        return;
    }

    block_offsets = model->blockOffsets;
    first_block = (NmlModelBlock *)((char *)model + block_offsets[0]);
    if (!(first_block->materialFlags & NML_BLOCK_MATERIAL_NAMED_PARTS)) {
        return;
    }

    set_flags = NML_BLOCK_HIDDEN;
    keep_flags = ~0u;
    if (visible) {
        set_flags = 0;
        keep_flags = ~NML_BLOCK_HIDDEN;
    }

    switch (match) {
    case NML_NAME_MATCH_PREFIX:
        for (block_index = 0; block_index < model->blockCount; block_index++) {
            block = (NmlModelBlock *)((char *)model + block_offsets[block_index]);
            if (memcmp(block->name, name, strlen(name)) == 0) {
                block->flags = (block->flags | set_flags) & keep_flags;
            }
        }
        break;
    case NML_NAME_MATCH_EXACT:
        for (block_index = 0; block_index < model->blockCount; block_index++) {
            block = (NmlModelBlock *)((char *)model + block_offsets[block_index]);
            if (strcmp(block->name, name) == 0) {
                block->flags = (block->flags | set_flags) & keep_flags;
            }
        }
        break;
    case NML_NAME_MATCH_SUBSTRING:
        for (block_index = 0; block_index < model->blockCount; block_index++) {
            block = (NmlModelBlock *)((char *)model + block_offsets[block_index]);
            if (strstr(block->name, name) != 0) {
                block->flags = (block->flags | set_flags) & keep_flags;
            }
        }
        break;
    }
}

void nmlModelDirectSend(int mode, const void *data, int count)
{
    NmlDirectRecord *entry;

    if (s_nPacketSignal != 0) {
        return;
    }

    if (s_nDirect >= 5500) {
        return;
    }

    entry = &s_aDirect[s_nDirect];
    entry->count = (s16)count;
    entry->attribute = 1;
    switch (mode) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 11:
            entry->data = nmlPacketSetAttributeData16N(data, count);
            break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
        case 12:
            entry->data = (void *)data;
            break;
    }
    switch (mode) {
        case 1:
        case 2:
            entry->attribute = 1;
            break;
        case 3:
        case 4:
            entry->attribute = 2;
            break;
        case 5:
        case 6:
            entry->attribute = 3;
            break;
        case 7:
        case 8:
            entry->attribute = 4;
            break;
        case 9:
        case 10:
            entry->attribute = 5;
            break;
        case 11:
        case 12:
            entry->attribute = 6;
            break;
    }
    s_nDirect++;
}

static void nmlModelDirectXtxSub(void *data, int modelId, int nextModelId)
{
    NmlDirectXtxBlock *block;
    NmlDirectXtxEntry *entry;
    unsigned int low;
    unsigned int high;
    int index;
    u64 env4;
    u64 env8;
    u64 *env;

    block = data;
    entry = (NmlDirectXtxEntry *) ((char *) data + block->entriesOffset);
    for (index = 0; index < block->entryCount; index++) {
        env = TestEnv_0_004A9170;
        low = entry->packed & 0xffffu;
        high = entry->packed >> 16;
        if (entry->packed == low) {
            high = (unsigned int) (((int) (low + 0x3f)) >> 6);
        }
        env4 = ((u64) ((entry->word8 >> 6) + 0x3800) << 32) | ((u64) high << 48);
        __builtin_memcpy(&env[4], &env4, sizeof(env4));
        env8 = low | ((u64) entry->word4 << 32);
        __builtin_memcpy(&env[8], &env8, sizeof(env8));
        nmlModelDirectSend(modelId, env, 6);
        nmlModelDirectSend(nextModelId, (char *)data + entry->dataOffset, entry->size);
        entry++;
    }
}

/*
 * Callers (GameRadarDraw main 0x00252fc8, unit6003_draw main 0x003211a0;
 * both still INCLUDE_ASM) never inspect the result -- the tail call below
 * matches the compiled form, so this is void.
 */
void nmlModelDirectSendXtx(int modelId, void *data)
{
    const signed char *bytes = data;

    if (s_nPacketSignal == 0 &&
        ((u32)data - NML_TEXTURE_RAM_BASE) <= NML_TEXTURE_RAM_MAX_OFFSET &&
        ((u32)data & 0xF) == 0 &&
        bytes[0] == 'X' && bytes[1] == 'T' && bytes[2] == bytes[0]) {
        nmlModelDirectXtxSub(data, modelId, modelId + 1);
    }
}

static void set_group_status(LayoutStore *layout, u32 *status)
{
    *status = 0;
    if (layout->fields.render_status & NML_RENDER_FOG) {
        *status = 0x4;
    }
    if (layout->fields.render_status & NML_RENDER_STENCIL) {
        *status |= 0x2;
    }
    if (layout->fields.render_status & 0x4000) {
        *status |= 0x1;
    }
    if (layout->fields.render_status & 0x800) {
        *status |= 0x8;
    }
    if (layout->fields.render_status & 0x10000) {
        *status |= 0x10;
    }
    if (layout->fields.render_status & NML_RENDER_TOUMEI) {
        *status |= 0x20;
    }
    if (layout->fields.render_status & 0x20000) {
        *status |= 0x40;
    }
    if (layout->fields.render_status & 0x40000) {
        *status |= 0x80;
    }
    if (layout->fields.render_status & 0x200000) {
        *status |= 0x100;
    }
    if (layout->fields.render_status & 0x400000) {
        *status |= 0x200;
    }
    if (layout->fields.render_status & 0x800000) {
        *status |= 0x800;
    }
    if (layout->fields.render_status & NML_RENDER_ZWRITE) {
        *status |= 0x400;
        s_nUseZwrite = 1;
    }
}

static void AlphaGroupSortEntry(const Vector4 *position)
{
    StudioCamera *camera = xglStudioSelectGetActiveCamera(s_inLayout.fields.window);
    float distance = 0.0f;
    if (camera != 0) {
        distance = _VectorLengthSQ(&camera->position.vector, position);
    }
    s_aAlphaGroup[s_nAlphaGroup].sortDistance = distance + s_fSortOffset;
}

static void AlphaGroupLastEntry(void)
{
    int index = 0;

    if (s_nBlocksAlphaLast > 0) {
        for (index = 0; index < s_nBlocksAlphaLast; ++index) {
            int capacityCount = s_nBlocksAlpha;
            if (capacityCount + s_nBlocks < 0xFFF) {
                int groupIndex = s_nAlphaGroup;
                if (groupIndex < 0x1FF) {
                    int *alphaBlockCount = &s_nBlocksAlpha;
                    LayoutStore *modelLayout;
                    int appendedBlockCount;

                    __builtin_memcpy(&s_apBlock[0xFFF - capacityCount],
                                     &s_apBlockLast[index], sizeof(s_apBlockLast[index]));
                    s_aBlockLayout[0xFFF - *alphaBlockCount] = s_aBlockLayoutLast[index];
                    __builtin_memcpy(&modelLayout,
                                     &s_apModelLayout[s_aBlockLayoutLast[index]],
                                     sizeof(modelLayout));
                    s_aAlphaGroup[groupIndex].firstBlock = (short)(0xFFF - *alphaBlockCount);
                    s_aAlphaGroup[groupIndex].blockCount = 1;
                    appendedBlockCount = *alphaBlockCount + 1;
                    __builtin_memcpy(alphaBlockCount, &appendedBlockCount,
                                     sizeof(appendedBlockCount));
                    __builtin_memset(&s_aAlphaGroup[groupIndex].sortDistance, 0,
                                     sizeof(s_aAlphaGroup[groupIndex].sortDistance));
                    set_group_status(modelLayout, &s_aAlphaGroup[groupIndex].flags);
                    ++s_nAlphaGroup;
                }
            }
        }
    }
}

/* Sorts the s_nAlphaGroup alpha groups by descending sortDistance. The
 * original is hand-written assembly: a bubble sort that swaps neighbouring
 * 16-byte records with one lq/sq pair and repeats the pass until it makes no
 * swap. */
static void AlphaGroupSort(void)
{
    __asm__ __volatile__(
        ".set noreorder\n\t"
        "lw $8,%0\n\t"
        "slti $2,$8,2\n\t"
        "bne $2,$0,3f\n\t"
        "lui $7,%%hi(%1)\n\t"
        "addiu $7,$7,%%lo(%1)\n"
        "1:\n\t"
        "move $5,$0\n\t"
        "move $4,$8\n\t"
        "addi $4,$4,-1\n\t"
        "move $6,$7\n"
        "2:\n\t"
        "lwc1 $f1,4($6)\n\t"
        "lwc1 $f0,20($6)\n\t"
        "c.le.s $f0,$f1\n\t"
        "nop\n\t"
        "bc1t 4f\n\t"
        "nop\n\t"
        "lq $2,0($6)\n\t"
        "lq $3,16($6)\n\t"
        "sq $3,0($6)\n\t"
        "sq $2,16($6)\n\t"
        "addi $5,$5,1\n"
        "4:\n\t"
        "addi $6,$6,16\n\t"
        "addi $4,$4,-1\n\t"
        "bne $0,$4,2b\n\t"
        "nop\n\t"
        "bne $0,$5,1b\n\t"
        "nop\n"
        "3:\n\t"
        ".set reorder"
        :
        : "m"(s_nAlphaGroup), "i"(s_aAlphaGroup)
        : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$f0", "$f1", "memory");
}

static int is_shadow_map_parts(const LayoutStore *layout, int partId)
{
    int found = 0;
    int count = layout->fields.mapShadowPartCount;
    if (count != 0) {
        int index;
        for (index = 0; index < count; ++index) {
            if (layout->fields.mapShadowParts[index] == partId) {
                found = 1;
                break;
            }
        }
    }
    return found;
}

static int is_parts_transparency(const NmlModelBlock *block)
{
    int index;
    int found = 0;
    for (index = 0; index < s_nToumeiNum; ++index) {
        if (s_aToumeiId[index] == block->partId) {
            found = 1;
            break;
        }
    }
    return found;
}

static int is_block_last_entry(const NmlModelBlock *block)
{
    int index;
    int found = 0;
    for (index = 0; index < s_nMapLast; ++index) {
        if (block->partId == s_aMapLast[index]) {
            found = 1;
            break;
        }
    }
    return found;
}

NmlParentBuffer *parent_buf_entry(LayoutStore *inLayout)
{
    NmlParentBuffer *entry = 0;
    if (s_nParentBuf < 512) {
        LayoutStore **layout;
        int index;
        entry = &s_aParentBuf[s_nParentBuf];
        layout = &entry->layout;
        *layout = 0;
        index = s_nParentBuf;
        s_aParentBuf[index].nonAlphaCount = 0;
        s_nParentBuf = index + 1;
        s_aParentBuf[index].alphaCount = 0;
    }
    return entry;
}

static NmlParentBuf *parent_buf_search(const NmlModel *parent)
{
    NmlParentBuf *found;
    int i;

    found = 0;
    for (i = s_nParentBuf - 1; i >= 0; i--) {
        NmlParentBuf *record = &s_aParentBuf[i];

        if (record->layout != 0 && record->layout->fields.model == parent && parent != 0) {
            found = record;
            break;
        }
    }
    return found;
}

static void set_circle_shadow_ratio(int scale)
{
    float ratio = (float)scale * 0.0625f;
    int i;

    for (i = 0; i < 16; i++) {
        int index = (i < 8) ? i : i + 8;

        circleShadowRatioTable[index] = (int)(128.0f - (float)i * ratio) << 24;
    }
}

int nmlModelLexDataCheck(NmlModel *model)
{
    int result = 1;
    NmlModelBlock *lastBlock;
    char *name;
    int *offsets;

    if ((unsigned int) model - 0x00200000u <= 0x01dfffffu
        && ((unsigned int) model & 0xf) == 0
        && (*(u64 *) model & 0x00ffffffULL) == 0x78656cULL) {
        if (model->unclassified_3f == 0
            || model->textureSetOffset != 0
            || (offsets = model->blockOffsets,
                lastBlock = (NmlModelBlock *) ((char *) model
                    + offsets[model->blockCount - 1]),
                name = (char *) lastBlock + lastBlock->dataOffset,
                name += model->unclassified_a0 != 0 ? model->blockCount << 4 : 0,
                strstr(name + lastBlock->dataSize, magicCarpetComeName) != 0)) {
            long long count = model->blockCount;

            if (count > 0) {
                result = 0;
            }
        }
    }
    return result;
}

static void nmlModelLexBlockDoit(NmlModel *model)
{
    int blockIndex;
    int *blockOffsets;

    blockIndex = 0;
    if (model->blockCount > 0) {
      blockOffsets = model->blockOffsets;
      do {
        NmlModelBlock *block;
        block = (NmlModelBlock *)((char *)model + blockOffsets[blockIndex]);

        if (s_nPause == 0 &&
            (block->translationStepX != 0.0f || block->translationStepY != 0.0f)) {
            float wrapX = 0.5f;
            float wrapY = 1.0f;
            block->translationX += block->translationStepX;
            block->translationY += block->translationStepY;

            if (block->translationX >= wrapX) {
                block->translationX -= wrapX;
            } else if (block->translationX <= -wrapX) {
                block->translationX += wrapX;
            }
            if (block->translationY >= wrapY) {
                block->translationY -= wrapY;
            } else if (block->translationY <= -wrapY) {
                block->translationY += wrapY;
            }
        }

        if (s_aMatName.text[0] != '\0') {
            if (strcmp(s_aMatName.text, block->materialName) == 0) {
                QuadwordCopy(&block->multiplyColor, &s_inMulColor);
            }
        } else {
            QuadwordCopy(&block->multiplyColor, &s_inLayout.fields.multiplyColor);
        }

        if ((u32)(block->shapeFlags - 2) < 2) {
            int partId = block->partId;
            int index;
            int divisor;
            int phase;

            for (index = 0; index < s_nShapeNum; ++index) {
                if (s_aShapeId[index] == partId) {
                    partId = -1;
                    break;
                }
            }

            if (partId >= 0 && s_nShapeNum < 32) {
                divisor = s_aShapeMathPara[(int)block->shapeMathParameter];
                phase = s_nCount & (divisor - 1);
                switch (block->shapeMode) {
                case 0: {
                    int halfDivisor = divisor >> 1;
                    float *weightEntry;

                    s_aShapeId[s_nShapeNum] = partId;
                    weightEntry = &s_aShapeWeight[s_nShapeNum];
                    *weightEntry = (float)(phase >= halfDivisor ?
                        divisor - phase : phase) / (float)halfDivisor *
                        (float)block->shapeWeight;
                    break;
                }
                case 1: {
                    float phaseFraction = (float)phase / (float)divisor;

                    s_aShapeId[s_nShapeNum] = partId;
                    s_aShapeWeight[s_nShapeNum] =
                        phaseFraction * (float)block->shapeWeight;
                    break;
                }
                }
                if ((float)block->shapeWeight <= s_aShapeWeight[s_nShapeNum]) {
                    s_aShapeWeight[s_nShapeNum] =
                        (float)block->shapeWeight - shapeWeightEpsilon;
                }
                ++s_nShapeNum;
            }
        }
      } while (++blockIndex < model->blockCount);
    }
}

static void nmlModelStealthEntry(void *arg0, void *arg1)
{
    NmlModel *model = arg0;
    StudioCamera *camera = arg1;
    const struct StudioCamera *fovView = arg1;
    const Vector4 *samplePosition;
    NmlProjectedPoint projected;
    f32 depth;
    int index;
    int swapped;

    _CurSetMatrix(camera->viewMatrix);
    _CurSetViewScaleTrans((const float *)&camera->screenScale,
                         (const float *)&camera->screenOffset);
    samplePosition = &s_inLayout.fields.center.v;
    _CurRotTransPersClip(projected, samplePosition);
    depth = ((float)projected[2] * stealthDepthScale) / tanf(fovView->fov * 0.5f);

    if (model->sampleCount != 0) {
        LayoutStore *const layout = &s_inLayout;
        for (index = 0; index < 10; ++index) {
            nmlFilterStealthMake(index, depth, camera, layout->slots);
            ++layout->fields.stealthEntryCount;
        }
        {
            float sortThreshold = stealthSortThreshold;
            LayoutStore *const sortLayout = layout;
            float distanceTolerance = stealthDistanceTolerance;
            swapped = 1;
            while (swapped != 0) {
                swapped = 0;
                for (index = 1; index <= 9; ++index) {
                    struct NmlStealthGeometry *left = sortLayout->fields.stealthParts[index - 1];
                    struct NmlStealthGeometry *right;
                    if (sortThreshold < left->sortClass) {
                        right = sortLayout->fields.stealthParts[index];
                        if (left->endpoints[0].w + distanceTolerance < right->endpoints[0].w) {
                            sortLayout->fields.stealthParts[index - 1] = right;
                            swapped = 1;
                            sortLayout->fields.stealthParts[index] = left;
                        }
                    }
                }
            }
        }
    } else {
        nmlFilterStealthMake(0, depth, camera, s_inLayout.slots);
        ++s_inLayout.fields.stealthEntryCount;
    }
}

static void nmlModelCalcSample(void *arg0, NmlParentBuf *parent)
{
    unsigned char *modelBytes = (unsigned char *)arg0;
    NmlModel *model = (NmlModel *)arg0;
    Vector4 *weightedPosition;
    Vector4 *currentPosition;

    if (model->sampleCount != 0) {
        weightedPosition = &s_inLayout.fields.center.v;
        _WeightToGlobalVec(weightedPosition,
                           (float (*)[4])(modelBytes + model->weightOffset),
                           (float (*)[4])(modelBytes + model->transformOffset));
        if ((s_inLayout.slots[0x230 / 4].i & 1) != 0) {
            s_inLayout.slots[0x1F4 / 4].f =
                s_inLayout.slots[0x230 / 4].f - s_inLayout.slots[0x1F4 / 4].f;
        }
        currentPosition = &s_inLayout.fields.currentPosition;
        QuadwordCopy(currentPosition, weightedPosition);
        if (parent != 0) {
            Vector4 *parentPosition = &parent->layout->fields.currentPosition;
            QuadwordCopy(currentPosition, parentPosition);
        }
    } else {
        Vu0AddVectorXYZ(&s_inLayout.fields.center, &s_inLayout.fields.center, &model->entryTranslation);
    }

    _CurSetMatrix((f32 (*)[4])&s_inLayout.fields.place);
    _CurApplyMatrix(&s_inLayout.fields.center.v,
                    &s_inLayout.fields.center.v);
    s_inLayout.slots[0x1FC / 4].f = model->entryTranslation.w;
    s_inLayout.slots[0x2C4 / 4].f = model->entryTranslation.w;
    if (parent != 0) {
        NmlModel *parentModel = parent->layout->fields.model;
        s_inLayout.slots[0x2C4 / 4].f = parentModel->entryTranslation.w;
    }

    if (strstr((char *)modelBytes + 0x11, "boss_030") != 0 && model->sampleCount >= 30) {
        Vector4 sample;
        int sampleOffset = 0x7C0;

        _WeightToGlobalVec(&sample,
                           (float (*)[4])(modelBytes + model->weightOffset + sampleOffset),
                           (float (*)[4])(modelBytes + model->transformOffset + sampleOffset));
        _CurSetMatrix((f32 (*)[4])&s_inLayout.fields.place);
        _CurApplyMatrix(&sample, &sample);
        if (sample.y < 0.0f) {
            s_inLayout.slots[0x2C4 / 4].f = model->entryTranslation.w * 0.25f;
        }
    }

    if (s_inLayout.slots[0x7F].f < calcSampleThreshold) {
        s_inLayout.slots[0x7F].f = 1.0f;
    }
    {
        float length = __builtin_sqrtf(
            s_inLayout.fields.place[0][0] * s_inLayout.fields.place[0][0] +
            s_inLayout.fields.place[0][1] * s_inLayout.fields.place[0][1] +
            s_inLayout.fields.place[0][2] * s_inLayout.fields.place[0][2]);

        if (length < 1.0f) {
            length = 1.0f;
        }
        s_inLayout.slots[0x7F].f =
            s_inLayout.slots[0x7F].f * (s_inLayout.slots[0x8B].f * length);
    }
}

static int nmlModelCalcEntryClip(NmlModel *model, StudioCamera *camera)
{
    Vector4 clip;
    int anySubwindow;
    int index;

    int clipDisabled = 1;
    do {
    } while (0);
    if (s_nClip != clipDisabled) {
        QuadwordCopy(&clip, &s_inLayout.slots[0x7C]);
        clip.w *= entryClipWidthScale;

        if (model->sampleCount != 0) {
            anySubwindow = 0;
            for (index = 1; index < 4; ++index) {
                if (g_aSubWindow[index] != 0) {
                    anySubwindow = 1;
                }
            }
            if (anySubwindow) {
                int passed = 0;
                int active = 0;
                for (index = 0; index < 4; ++index) {
                    if (g_aSubWindow[index] != 0) {
                        int result = nmlModelCalcClipStudio(&clip, index);
                        ++active;
                        s_inLayout.slots[0xC4 + index].i = result;
                        passed += result;
                    }
                }
                if (passed == active) {
                    s_inLayout.slots[0x94].i |= 0x20000;
                }
            } else if (nmlModelCalcClipStudio(&clip, 0) != 0) {
                s_inLayout.slots[0xC4].i = 1;
                s_inLayout.slots[0x94].i |= 0x20000;
            }
        }
    }
    return 0;
}

static float deapth_for_studio(int studio, Vector4 *position, float scale)
{
    StudioCamera *camera;
    NmlProjectedPoint projected;
    float depth;
    float factor;

    depth = 0.0f;
    factor = scale;
    camera = xglStudioSelectGetActiveCamera(studio);
    if (camera == 0) {
        return depth;
    }
    _CurSetMatrix(camera->viewMatrix);
    _CurSetViewScaleTrans((const float *)&camera->screenScale,
                          (const float *)&camera->screenOffset);
    _CurRotTransPersClip(projected, position);
    depth = ((float)projected[2] * factor) /
            tanf(((struct StudioCamera *)camera)->fov * 0.5f);
    return depth;
}

static void nmlModelCalcSpecularClip(void *model, int useParentDepth)
{
    f32 distanceScale = s_inLayout.slots[0x7F].f;
    f32 depth;
    f32 subwindowEnd;
    f32 subwindowRange;
    int flags;
    int index;

    if (strstr((char *)model + 0x10, faceNameSuffix) == 0 && s_nParent != 0) {
        NmlModel *parent = (NmlModel *)s_nParent;
        distanceScale = parent->entryTranslation.w;
    }
    if (useParentDepth != 0) {
        NmlModel *parent = (NmlModel *)s_nParent;
        distanceScale = parent->entryTranslation.w;
    }

    depth = deapth_for_studio(s_inLayout.fields.window, &s_inLayout.fields.center.v,distanceScale);
    flags = s_inLayout.slots[0xAC].i;
    if ((flags & 2) == 0) {
        if ((flags & 0x20) == 0) {
            if (__builtin_fabsf(depth) < 62582.0f) {
                s_inLayout.slots[0xAC].i = flags | 2;
            } else if (__builtin_fabsf(depth) <= 147523.0f) {
                s_inLayout.slots[0x8A].f =
                    (s_inLayout.slots[0x8A].f *
                     (__builtin_fabsf(depth) - 62582.0f)) /
                    84941.0f;
            }
            if (*(f32 *)((char *)model + 0x9C) > 10.0f) {
                s_inLayout.slots[0x8A].f = 1.0f;
                s_inLayout.slots[0xAC].i &= ~2;
            }
        }
        if (__builtin_fabsf(depth) < 10000.0f) {
            s_inLayout.slots[0x9B].f = 0.0f;
        } else if (__builtin_fabsf(depth) <= 20000.0f) {
            s_inLayout.slots[0x9B].f =
                (s_inLayout.slots[0x9B].f *
                 (__builtin_fabsf(depth) - 10000.0f)) /
                10000.0f;
        }
        if (s_inLayout.slots[0x89].f <= 0.0f) {
            s_inLayout.slots[0xAC].i |= 0x10;
        }
        flags = s_inLayout.slots[0xAC].i;
        if ((flags & 0x10) != 0) {
            s_inLayout.slots[0x89].f = 0.0f;
        }
        if ((flags & 0x102) != 0) {
            s_inLayout.slots[0x8A].f = 0.0f;
            s_inLayout.slots[0xAC].i = flags | 2;
        }

        s_inLayout.slots[s_inLayout.slots[0x9F].i + 0xC0].f =
            s_inLayout.slots[0x8A].f;
        subwindowEnd = 147523.0f;
        subwindowRange = 84941.0f;
        for (index = 0; index < 4; ++index) {
            if (s_inLayout.slots[0x9F].i != index &&
                g_aSubWindow[index] != 0) {
                f32 subwindowAbsoluteDepth;
                depth = deapth_for_studio(index, &s_inLayout.fields.center.v,distanceScale);
                subwindowAbsoluteDepth = __builtin_fabsf(depth);

                if (subwindowAbsoluteDepth < 62582.0f) {
                    s_inLayout.slots[index + 0xC0].i = 0;
                } else if (subwindowAbsoluteDepth <= subwindowEnd) {
                    s_inLayout.slots[index + 0xC0].f =
                        (s_inLayout.slots[index + 0xC0].f *
                         (subwindowAbsoluteDepth - 62582.0f)) /
                        subwindowRange;
                }
            }
        }
    } else {
        s_inLayout.slots[0xC0].f = 0.0f;
        s_inLayout.slots[0xC1].f = 0.0f;
        s_inLayout.slots[0xC2].f = 0.0f;
        s_inLayout.slots[0xC3].f = 0.0f;
        s_inLayout.slots[0x8A].f = 0.0f;
    }
    if ((s_inLayout.slots[0x94].i & 0x100040) != 0) {
        float clipDistance = __builtin_fabsf(depth);
        float clipLimit = 32582.0f;
        if (clipDistance < clipLimit) {
            s_inLayout.slots[0xAC].i |= 0x800;
        }
    }
}

static void nmlModelCalcDropShadow(NmlModel *model)
{
    xglMatrixUnit((float (*)[4])&s_inLayout.slots[0x180 / 4].f);

    {
        Vector4 shadow;

        shadow.x = LAYOUT_W(0x40);
        shadow.y = 1.5f;
        shadow.z = LAYOUT_W(0x60);

        if (s_nShadowVec != 0) {
            shadow.x = s_inShadowVec.x;
            shadow.z = s_inShadowVec.z;
        }

        if (shadow.y != 0.0f) {
            LAYOUT_W(0x190) = -shadow.x / shadow.y;
            LAYOUT_W(0x198) = -shadow.z / shadow.y;
        } else {
            LAYOUT_W(0x190) = 0.0f;
            LAYOUT_W(0x198) = 0.0f;
        }

        LAYOUT_W(0x194) = 0.0f;
        LAYOUT_W(0x1b4) = LAYOUT_W(0xb4) + 0.005f;
        LAYOUT_W(0x1b0) = -LAYOUT_W(0xb4) * LAYOUT_W(0x190);
        LAYOUT_W(0x1b8) = -LAYOUT_W(0xb4) * LAYOUT_W(0x198);

        if (LAYOUT_S32(0x2a8) != 0) {
            LAYOUT_W(0x1b4) = LAYOUT_W(0x2ac) + 0.005f;
            LAYOUT_W(0x1b0) = -LAYOUT_W(0x2ac) * LAYOUT_W(0x190);
            LAYOUT_W(0x1b8) = -LAYOUT_W(0x2ac) * LAYOUT_W(0x198);
        }
    }
}

static void nmlModelCalcEntryProjectCircle(NmlModel *model)
{
    extern void nmlModelSetProreal(NmlProRealState *);
    extern void xglMatrixStackInverse(void);
    extern void xglVectorNormal(Vector4 *, const Vector4 *);
    extern void xglVectorScaleXYZ(Vector4 *, const Vector4 *, float);
    extern DoublewordAlignedVector4 circleScaleVector;
    extern DoublewordAlignedVector4 circleCenterVector;
    extern DoublewordAlignedVector4 circleOffsetVector;
    Vector3 projectionAngles;
    Matrix savedMatrix;
    DoublewordAlignedVector4 screenScaleStorage = circleScaleVector;
    DoublewordAlignedVector4 screenCenter = circleCenterVector;
    DoublewordAlignedVector4 screenOffsetStorage = circleOffsetVector;
    Vector4 cameraTranslation;
    Vector4 normal;
    Vector4 spare;
    float scale;
    float inverseScale;
    float modelScale;
    float offset;
    float value;
    float one;

    s_inProReal[0].fields.unmodeled_190[0].q = s_inLayout.fields.center.q;
    inverseScale = 0.9f;
    modelScale = model->entryTranslation.w * inverseScale;
    scale = 1.0f;
    one = 1.0f;
    scale /= modelScale;

    screenScaleStorage.x *= scale;
    screenScaleStorage.y *= scale;
    projectionAngles.x = 1.5707964f;
    cameraTranslation.x = 0.0f;
    cameraTranslation.y = one;
    cameraTranslation.z = 0.0f;
    cameraTranslation.w = one;
    projectionAngles.z = 0.0f;
    projectionAngles.y = 0.0f;
    Vu0AddVectorXYZ(&cameraTranslation, &cameraTranslation, &s_inProReal[0].fields.unmodeled_190[0].v);

    xglMatrixStackUnit();
    xglMatrixStackTrans(&cameraTranslation.x);
    xglMatrixStackRotY(projectionAngles.y);
    xglMatrixStackRotX(projectionAngles.x);
    xglMatrixStackSave(s_inProReal[0].fields.matrix_000);
    xglMatrixStackInverse();
    xglMatrixStackSave((float (*)[4])savedMatrix.elements);
    xglMatrixStackUnit();

    screenOffsetStorage.x -= ((float)sRender.reso.frame_width - screenCenter.x) * 0.5f;
    screenOffsetStorage.y -= ((float)sRender.reso.frame_height - screenCenter.y) * 0.5f;
    xglMatrixStackTrans(&screenOffsetStorage.x);
    xglMatrixStackScale(&screenScaleStorage.x);
    xglMatrixStackMul(savedMatrix.rows);
    xglMatrixStackSave(s_inLayout.fields.projectionMatrix);

    s_inProReal[0].unmodeled_1c8 = 0x89;
    s_inProReal[0].unmodeled_1d0 = 0x3200000062LL;
    s_inProReal[0].fields.unmodeled_100 = 0;
    s_inProReal[0].fields.unmodeled_104 = 0;
    s_inProReal[0].fields.unmodeled_108 = 0x40;
    s_inProReal[0].fields.unmodeled_10c = 0x40;
    s_inProReal[0].fields.unmodeled_116 = sRender.texture_buffer_base;
    s_inProReal[0].fields.unmodeled_118 = 0x80;
    s_inProReal[0].fields.unmodeled_114 = 0x13;
    s_inProReal[0].fields.unmodeled_110 = 0;
    s_inProReal[0].fields.unmodeled_112 = 0;
    s_inProReal[0].fields.unmodeled_120 = 64.0f / (scale * 64.0f);
    s_inProReal[0].fields.unmodeled_124 = 64.0f / (scale * 64.0f);
    s_inProReal[0].fields.unmodeled_170[0].v.x =
        s_inProReal[0].fields.unmodeled_170[0].v.y =
        s_inProReal[0].fields.unmodeled_170[0].v.z = 0.0f;
    s_inProReal[0].fields.unmodeled_170[0].v.w = one;
    s_inProReal[0].fields.unmodeled_170[1].v.x =
        s_inProReal[0].fields.unmodeled_170[1].v.y =
        s_inProReal[0].fields.unmodeled_170[1].v.z = one;
    s_inProReal[0].fields.unmodeled_170[1].v.w = one;
    s_inProReal[0].projection_entry = 0;
    nmlModelSetProreal(s_inProReal);
    s_inProReal[0].fields.unmodeled_11c = 1;

    QuadwordZeroMatrix(&s_inProReal[0].fields.unmodeled_0c0[0]);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_0c0[0], &s_inProReal[0].fields.unmodeled_170[0]);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_0c0[3], &s_inProReal[0].fields.unmodeled_170[1]);
    xglVectorScaleXYZ(&s_inProReal[0].fields.unmodeled_160,
                      &s_inProReal[0].fields.unmodeled_160, -1.0f);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_080[0], &s_inProReal[0].fields.unmodeled_160);

    s_inProReal[0].fields.unmodeled_190[1].q = s_inProReal[0].fields.unmodeled_190[0].q;
    s_inProReal[0].fields.unmodeled_190[2].q = s_inProReal[0].fields.unmodeled_190[0].q;
    s_inLayout.fields.render_status &= ~0x800;
    s_inProReal[0].fields.unmodeled_080[0].v.x = 0.0f;
    s_inProReal[0].fields.unmodeled_080[1].v.x = one;
    s_inProReal[0].fields.unmodeled_080[2].v.x = 0.0f;
    offset = (s_inLayout.slots[0x9d].f * 0.8f) / scale;
    s_inProReal[0].fields.unmodeled_190[2].v.x += offset;
    s_inProReal[0].fields.unmodeled_190[2].v.z += offset;
    s_inProReal[0].fields.unmodeled_190[1].v.x -= offset;
    s_inProReal[0].fields.unmodeled_190[1].v.z -= offset;
    value = s_inProReal[0].fields.unmodeled_190[1].v.y -
            (model->entryTranslation.w * s_inLayout.slots[0x9d].f + 0.5f);
    s_inProReal[0].fields.unmodeled_190[1].v.y = value;
    if (s_inLayout.slots[0x2d].f < value) {
        s_inProReal[0].fields.unmodeled_190[1].v.y = s_inLayout.slots[0x2d].f - 0.5f;
    }

    normal.x = 0.0f;
    normal.y = one;
    normal.z = 0.0f;
    xglVectorNormal(&normal, &normal);
    inverseScale = one / scale;
    xglVectorScaleXYZ(&normal, &normal, inverseScale);
    if (normal.x < 0.0f) {
        s_inProReal[0].fields.unmodeled_190[1].v.x += normal.x;
    } else {
        s_inProReal[0].fields.unmodeled_190[2].v.x += normal.x;
    }
    if (normal.z < 0.0f) {
        s_inProReal[0].fields.unmodeled_190[1].v.z += normal.z;
    } else {
        s_inProReal[0].fields.unmodeled_190[2].v.z += normal.z;
    }
}

static void nmlModelCalcEntryProjectMap(NmlModel *model, StudioCamera *camera)
{
    extern void nmlModelSetProreal(NmlProRealState *);
    extern void xglMatrixStackInverse(void);
    extern void xglVectorNormal(Vector4 *, const Vector4 *);
    extern void xglVectorScale(Vector4 *, const Vector4 *, float);
    extern void xglVectorScaleXYZ(Vector4 *, const Vector4 *, float);
    extern float atan2f(float, float);
    extern const NmlVector4Overlay mapScaleVector;
    extern const NmlVector4Overlay mapCenterVector;
    extern const NmlVector4Overlay mapOffsetVector;
    Vector3 angles;
    Matrix savedMatrix;
    NmlVector4Overlay projectionScale;
    NmlVector4Overlay screenCenter;
    NmlVector4Overlay screenOffset;
    Vector4 surfaceDir;
    Vector4 normal;
    float distance;
    float denom;
    float scale;
    float offset;
    /* Never accessed: the original frame keeps this 16-byte slot (frame 272). */
    Vector4 unusedFrameSlot;

    /* Empty loop kept from the original: its loop notes order the prologue
     * (callee saves before the projection constant loads). */
    do { } while (0);
    projectionScale = mapScaleVector;
    screenCenter = mapCenterVector;
    screenOffset = mapOffsetVector;
    denom = model->entryTranslation.w;
    if (5.0f + camera->position.vector.y < s_inLayout.slots[0x7d].f) {
        if (5.0f + camera->position.vector.y < s_inLayout.slots[0x2d].f) {
            s_inLayout.fields.render_status &= ~0x4000;
            return;
        }
    }

    QuadwordCopy(&s_inProReal[0].fields.unmodeled_190[0], &s_inLayout.fields.center);
    s_inProReal[0].fields.unmodeled_190[0].v.y -= 0.2f;
    distance = __builtin_fabsf(s_inProReal[0].fields.unmodeled_190[0].v.y - s_inLayout.slots[0x2d].f);
    if (distance > model->entryTranslation.w) {
        denom = distance;
    }
    if (denom < 0.968f) {
        denom = 0.968f;
    }
    scale = 0.968f / denom;
    projectionScale.v.x *= scale;
    projectionScale.v.y *= scale;
    surfaceDir.x = s_inLayout.slots[0x10].f;
    surfaceDir.y = 1.5f;
    surfaceDir.z = s_inLayout.slots[0x18].f;
    surfaceDir.w = 1.0f;
    if (s_nShadowVec != 0) {
        surfaceDir.x = s_inShadowVec.x;
        surfaceDir.z = s_inShadowVec.z;
    }
    angles.z = __builtin_sqrtf(surfaceDir.x * surfaceDir.x + surfaceDir.z * surfaceDir.z);
    angles.y = -atan2f(surfaceDir.x, -surfaceDir.z);
    angles.x = atan2f(surfaceDir.y, angles.z);

    xglVectorNormal(&surfaceDir, &surfaceDir);
    xglVectorScale(&surfaceDir, &surfaceDir, 3.0f);
    Vu0AddVectorXYZ(&surfaceDir, &surfaceDir, &s_inProReal[0].fields.unmodeled_190[0].v);

    xglMatrixStackUnit();
    xglMatrixStackTrans(&surfaceDir.x);
    xglMatrixStackRotY(angles.y);
    xglMatrixStackRotX(angles.x);
    xglMatrixStackRotZ(1.5707964f);
    xglMatrixStackSave(s_inProReal[0].fields.matrix_000);
    xglMatrixStackInverse();
    xglMatrixStackSave((float (*)[4])savedMatrix.elements);
    xglMatrixStackUnit();

    screenOffset.v.x -= ((float)sRender.reso.frame_width - screenCenter.v.x) * 0.5f;
    screenOffset.v.y -= ((float)sRender.reso.frame_height - screenCenter.v.y) * 0.5f;
    xglMatrixStackTrans(&screenOffset.v.x);
    xglMatrixStackScale(&projectionScale.v.x);
    xglMatrixStackMul(savedMatrix.rows);
    xglMatrixStackSave(s_inLayout.fields.projectionMatrix);

    s_inProReal[0].unmodeled_1c8 = 0x89;
    s_inProReal[0].unmodeled_1d0 = 0x3200000062LL;
    s_inProReal[0].fields.unmodeled_100 = 0;
    s_inProReal[0].fields.unmodeled_104 = 0;
    s_inProReal[0].fields.unmodeled_108 = 0x100;
    s_inProReal[0].fields.unmodeled_10c = 0xe0;
    s_inProReal[0].fields.unmodeled_116 = sRender.texture_buffer_base;
    s_inProReal[0].fields.unmodeled_110 = 0;
    s_inProReal[0].fields.unmodeled_112 = 0;
    s_inProReal[0].fields.unmodeled_120 = 64.0f / (scale * 128.0f);
    s_inProReal[0].fields.unmodeled_124 = s_inProReal[0].fields.unmodeled_120;
    s_inProReal[0].fields.unmodeled_170[0].v.x = s_inProReal[0].fields.unmodeled_170[0].v.y =
        s_inProReal[0].fields.unmodeled_170[0].v.z = 0.0f;
    s_inProReal[0].fields.unmodeled_170[0].v.w = 1.0f;
    s_inProReal[0].fields.unmodeled_170[1].v.x = s_inProReal[0].fields.unmodeled_170[1].v.y =
        s_inProReal[0].fields.unmodeled_170[1].v.z = 1.0f;
    s_inProReal[0].fields.unmodeled_170[1].v.w = 1.0f;
    s_inProReal[0].projection_entry = 0;
    s_inProReal[0].fields.unmodeled_118 = 0;
    s_inProReal[0].fields.unmodeled_114 = 0;
    nmlModelSetProreal(s_inProReal);
    s_inProReal[0].fields.unmodeled_11c = 1;

    QuadwordZeroMatrix(&s_inProReal[0].fields.unmodeled_0c0[0]);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_0c0[0], &s_inProReal[0].fields.unmodeled_170[0]);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_0c0[3], &s_inProReal[0].fields.unmodeled_170[1]);
    xglVectorScaleXYZ(&s_inProReal[0].fields.unmodeled_160,
                      &s_inProReal[0].fields.unmodeled_160, -1.0f);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_080[0], &s_inProReal[0].fields.unmodeled_160);

    s_inLayout.fields.render_status &= ~0x800;
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_190[1], &s_inProReal[0].fields.unmodeled_190[0]);
    QuadwordCopy(&s_inProReal[0].fields.unmodeled_190[2], &s_inProReal[0].fields.unmodeled_190[0]);

    offset = s_inLayout.slots[0x9d].f * 0.8f;
    s_inProReal[0].fields.unmodeled_190[2].v.x += offset;
    s_inProReal[0].fields.unmodeled_190[2].v.z += offset;
    s_inProReal[0].fields.unmodeled_190[1].v.x -= offset;
    s_inProReal[0].fields.unmodeled_190[1].v.z -= offset;
    s_inProReal[0].fields.unmodeled_190[2].v.y += model->entryTranslation.w * s_inLayout.slots[0x9d].f;
    s_inProReal[0].fields.unmodeled_190[1].v.y -= model->entryTranslation.w * s_inLayout.slots[0x9d].f;

    normal.x = -s_inLayout.slots[0x10].f;
    normal.y = 0.0f;
    normal.z = -s_inLayout.slots[0x18].f;
    if (s_nShadowVec != 0) {
        normal.x = -s_inShadowVec.x;
        normal.z = -s_inShadowVec.z;
    }
    xglVectorNormal(&normal, &normal);
    xglVectorScaleXYZ(&normal, &normal, 1.0f / scale);
    if (normal.x < 0.0f) {
        s_inProReal[0].fields.unmodeled_190[1].v.x += normal.x;
    } else {
        s_inProReal[0].fields.unmodeled_190[2].v.x += normal.x;
    }
    if (normal.z < 0.0f) {
        s_inProReal[0].fields.unmodeled_190[1].v.z += normal.z;
    } else {
        s_inProReal[0].fields.unmodeled_190[2].v.z += normal.z;
    }
    if (s_inProReal[0].fields.unmodeled_190[1].v.y > s_inLayout.slots[0x2d].f) {
        s_inProReal[0].fields.unmodeled_190[1].v.y = s_inLayout.slots[0x2d].f - 0.5f;
    }
    if ((s_inLayout.fields.texMapMode & 0x40000) != 0) {
        if (0.0f < s_inProReal[0].fields.unmodeled_190[1].v.y) {
            s_inProReal[0].fields.unmodeled_190[1].v.y = -0.2f;
        }
    }
}

static inline void nmlModelWeightAttributeVector(
    Vector4 *destination,
    Matrix4 transform,
    Matrix4 weights)
{
    _WeightToGlobalVec(destination, transform, weights);
}

static void nmlModelCalcEntryPartsClip(NmlModel *model, LayoutStore *layout)
{
    /* Four original initialized words have no observed subsequent reader. */
    unsigned int initializedWords[4];
    int *partOffsets = model->blockOffsets;
    int vertexIndex;
    int partIndex;

    initializedWords[3] = 255;
    initializedWords[2] = 255;
    initializedWords[1] = 255;
    initializedWords[0] = 255;
    if (layout->fields.clipMasks != 0) {
        for (vertexIndex = 0; vertexIndex < model->sampleCount; ++vertexIndex) {
            Vector4 *weightedVector = &((Vector4 *)0x70000000)[vertexIndex];
            int firstMatrix = model->partCount * 2 + vertexIndex;
            int secondMatrix = model->partCount * 2 + model->sampleCount
                             + vertexIndex;
            NmlWeightedMatrix *matrixRecords = layout->fields.weightedMatrices;

            nmlModelWeightAttributeVector(weightedVector, matrixRecords[firstMatrix].matrix,
                                          matrixRecords[secondMatrix].matrix);
            if ((s_inLayout.slots[0x230 / 4].i & 1) != 0) {
                weightedVector->y = s_inLayout.slots[0x230 / 4].f
                                  - weightedVector->y;
            }
            _ApplyMatrix(weightedVector, layout->fields.place, weightedVector);
        }
        for (partIndex = 0; partIndex < model->blockCount; ++partIndex) {
            NmlModelBlock *block = (NmlModelBlock *)((char *)model + partOffsets[partIndex]);

            if ((block->flags & 1) == 0) {
                Vector4 partClipBounds;

                _ApplyMatrix(&partClipBounds,
                             (layout->fields.weightedMatrices + block->matrixIndices[0] - 1)->matrix,
                             &block->clipBounds);
                _ApplyMatrix(&partClipBounds, layout->fields.place, &partClipBounds);
                layout->fields.clipMasks[partIndex] =
                    (unsigned char)nmlModelCalcClip(&partClipBounds);
            } else {
                int weightedIndex = 0;
                unsigned char passingCount = 0;

                for (;; ++weightedIndex) {
                    int partCount;
                    unsigned int firstWeightedMatrix;
                    unsigned int clipIndex;
                    Vector4 *clipBounds;

                    if (block->matrixIndices[weightedIndex + 1] == 0) {
                        break;
                    }
                    __builtin_memcpy(&partCount, &model->partCount, sizeof partCount);
                    firstWeightedMatrix = partCount * 2u;
                    clipIndex = block->matrixIndices[weightedIndex + 1]
                              - firstWeightedMatrix - 1;
                    clipBounds = &((Vector4 *)0x70000000)[clipIndex];
                    clipBounds->w = block->clipBounds.w * entryPartsClipScale;
                    if (nmlModelCalcClipNoCulling(clipBounds) != 0) {
                        ++passingCount;
                    }
                }
                if (passingCount != 0 && passingCount == weightedIndex) {
                    layout->fields.clipMasks[partIndex] = 1;
                }
            }
        }
    }
}

void nmlModelEntry(NmlModel *model)
{
    NmlParentBuffer *parentEntry;
    NmlParentBuf *parent;
    StudioCamera *camera;
    LayoutStore *layout;
    NmlModelBlock *block;
    int nonAlphaParentAdded;
    int alphaParentAdded;
    LayoutSlot toumeiSlot;
    int quadCount;
    int index;
    int groupScan;
    int quadIndex;
    int shapeIndex;
    int toumeiIndex;
    int alphaAdded;
    int nonAlphaAdded;
    int parentAlphaCount;
    int parentNonAlphaCount;
    int alphaBlockCount;
    unsigned int fogValue;
    int *fogColorWord;
    int groupIndex;
    int copyCount;
    int displayed;
    int listIndex;
    int copyIndex;
    int alphaGroupDelta;
    int nonAlphaGroupDelta;
    unsigned int clipBits;
    NmlModelBlock *first;
    Vector4 sortPos;
    NmlBlockGroup *alphaGroup;
    int *blockOffsets;

    nmlPacketSetCurrent();
    while (nmlModelLexDataCheck(model) == 0 && s_nModel < 0x200 &&
           s_nBlocks + s_nBlocksAlpha + model->blockCount < 0xfff) {
        parentEntry = 0;
        parent = parent_buf_search(s_nParent);
        nonAlphaParentAdded = 0;
        alphaParentAdded = 0;
        camera = xglStudioSelectGetActiveCamera(LAYOUT_S32(0x27c));
        if (camera != 0) {
            /* Stored through the field's address, as a plain word: the
             * original orders it against the scalar globals read next.
             * The parent entry counters below are written the same way. */
            (&s_inLayout.fields.model)[0] = model;
            LAYOUT_S32(0x25c) = s_nModel;
            s_inLayout.fields.mapShadowPartCount = s_nMapShadowParts;
            if ((s_inLayout.qwords[0x250 / 8] & 0x1000002) == 0x1000000) {
                QuadwordCopy(&s_inLayout.fields.fogPara, &s_inGblFogPara);
                QuadwordCopy(&s_inLayout.fields.fogColor, s_inGblFogCol);
            }
            if ((s_inLayout.qwords[0x250 / 8] & 0x2001000) == 0x2000000) {
                QuadwordCopyMatrix(&s_inLayout.fields.pointPosition[0], &s_inGblPointP[0]);
                QuadwordCopyMatrix(&s_inLayout.fields.pointColor[0], &s_inGblPointC[0]);
            }
            if (parent != 0) {
                s_inLayout.fields.render_status |= parent->layout->fields.render_status & 0x204000;
            } else {
                s_fSortOffset = s_fSortOffsetEntry;
            }
            nmlModelAskNonLinearCamera();
            if (s_inLayout.fields.render_status & 0x800000) {
                LAYOUT_W(0x220) = 1.0f;
            }
            if (strstr((char *)model + 0x11, faceNameSuffix) != 0) {
                s_inLayout.fields.render_status |= 0x100000;
            }
            xglCullingMapLastCheck();
            nmlModelLexBlockDoit(model);
            if (s_nParent == 0) {
                parentEntry = parent_buf_entry(&s_inLayout);
            }
            nmlModelCalcSample(model, parent);
            if (parent == 0 && (s_inLayout.fields.packetFilterFlags & 2)) {
                nmlModelStealthEntry(model, camera);
            } else {
                s_inLayout.fields.packetFilterFlags &= 0xfffffffd;
            }
            if (nmlModelCalcEntryClip(model, camera) == 0) {
                if ((s_inLayout.fields.renderLevel & 0x400) != 0 &&
                    (s_inLayout.fields.packetFilterFlags & 1) != 0) {
                    s_nUseGnosys = 1;
                    s_inLayout.fields.packetFilterFlags |= 4;
                }
                if (s_inLayout.fields.render_status & 0x1000002) {
                    fogColorWord = &s_inLayout.fields.fogColor.intensity;
                    fogValue = nmlModelGetFogParaStudio(&s_inLayout.fields.center.v,
                                                        LAYOUT_S32(0x27c));
                    LAYOUT_W(0x2bc) = (float)fogValue / 255.0f;
                    LAYOUT_W(0x2f0 + LAYOUT_S32(0x27c) * 4) = LAYOUT_W(0x2bc);
                    LAYOUT_W(0x228) = LAYOUT_W(0x228) * LAYOUT_W(0x2bc);
                    LAYOUT_W(0x300 + LAYOUT_S32(0x27c) * 4) =
                        LAYOUT_W(0x300 + LAYOUT_S32(0x27c) * 4) * LAYOUT_W(0x2bc);
                    *fogColorWord = (int)fogValue << 4;
                    for (groupIndex = 1; groupIndex < 4; groupIndex++) {
                        if (g_aSubWindow[groupIndex] != 0) {
                            fogValue = nmlModelGetFogParaStudio(&s_inLayout.fields.center.v,
                                                                groupIndex);
                            LAYOUT_W(0x2f0 + groupIndex * 4) = (float)fogValue / 255.0f;
                        }
                    }
                }
                LAYOUT_W(0x26c) = 1.0f;
                nmlModelCalcSpecularClip(model, (int)parent);
                if ((s_inLayout.fields.render_status & 0x40000) != 0 && s_nParent == 0) {
                    nmlModelCalcDropShadow(model);
                }
                if ((s_inLayout.fields.render_status & 0x200000) != 0 && s_nParent == 0) {
                    nmlModelCalcEntryProjectCircle(model);
                }
                if ((s_inLayout.fields.render_status & 0x4000) != 0 && s_nParent == 0) {
                    nmlModelCalcEntryProjectMap(model, camera);
                }
                xglMatrixReverse(s_inLayout.fields.lightColor, s_inLayout.fields.lightColor);
                if (s_inMapHandle[0] != 0) {
                    s_inLayout.fields.weightedMatrices = (NmlWeightedMatrix *)((char *)model + model->weightedMatrixOffset);
                } else {
                    s_inLayout.fields.weightedMatrices = nmlPacketSetAttributeData64N(
                        (char *)model + model->weightedMatrixOffset,
                        (model->partCount + model->sampleCount) * 2);
                }
                quadCount = model->blockCount;
                if ((quadCount & 0xf) != 0) {
                    quadCount = quadCount - (quadCount & 0xf) + 16;
                }
                s_inLayout.fields.clipMasks = nmlPacketSetAttributeAlloc16N(quadCount / 16);
                for (quadIndex = 0; quadIndex < quadCount / 16; quadIndex++) {
                    QuadwordZero(quadIndex * 16 + (int)s_inLayout.fields.clipMasks);
                }
                if ((s_inLayout.fields.render_status & 0x204800) != 0) {
                    LAYOUT_S32(0x260) = (int)nmlPacketSetAttributeData16N(s_inProReal, 0x1e);
                }
                LAYOUT_S32(0x264) = 0;
                layout = nmlPacketSetAttributeData16N(&s_inLayout, 0x32);
                s_apModelLayout[s_nModel] = layout;
                if (s_inMapHandle[0] != 0) {
                    s_inMapHandle[1] = (int)layout;
                }
                if (parentEntry != 0) {
                    parentEntry->layout = layout;
                }
                if (s_nShapeNum != 0) {
                    for (shapeIndex = 0; shapeIndex < s_nShapeNum; shapeIndex++) {
                        float (*shapeMatrix)[4] = layout->fields.weightedMatrices[s_aShapeId[shapeIndex]].matrix;
                        if (s_aShapeWeight[shapeIndex] < 0.0f) {
                            s_aShapeWeight[shapeIndex] = 0.0f;
                        }
                        shapeMatrix[2][3] = s_aShapeWeight[shapeIndex];
                    }
                }
                if (s_nToumeiNum != 0) {
                    toumeiSlot.i = 0;
                    for (toumeiIndex = 0; toumeiIndex < s_nToumeiNum; toumeiIndex++) {
                        float (*toumeiMatrix)[4] = layout->fields.weightedMatrices[s_aToumeiId[toumeiIndex]].matrix;
                        toumeiSlot.i = s_aToumei[toumeiIndex].i | 1;
                        toumeiMatrix[1][3] = toumeiSlot.f;
                    }
                }
                if (s_nClip != 1 && model->sampleCount != 0 &&
                    (layout->fields.render_status & 0x100000) == 0 &&
                    (layout->fields.renderLevel & 0x200) != 0) {
                    nmlModelCalcEntryPartsClip(model, layout);
                }
                alphaGroup = &s_aAlphaGroup[s_nAlphaGroup];
                alphaAdded = 0;
                nonAlphaAdded = 0;
                alphaBlockCount = 0;
                parentAlphaCount = 0;
                parentNonAlphaCount = 0;
                blockOffsets = model->blockOffsets;
                if (s_nAlphaGroup < 0x1ff) {
                    alphaGroup->firstBlock = (short)(0xfff - s_nBlocksAlpha);
                    alphaGroup->blockCount = 0;
                    __builtin_memset(&s_aAlphaGroup[s_nAlphaGroup].sortDistance, 0,
                                     sizeof(s_aAlphaGroup[s_nAlphaGroup].sortDistance));
                    s_aAlphaGroup[s_nAlphaGroup].owner.layout = layout;
                    set_group_status(layout, &s_aAlphaGroup[s_nAlphaGroup].flags);
                }
                if (s_nNonAlphaGroup < 0x1ff) {
                    s_aNonAlphaGroup[s_nNonAlphaGroup].firstBlock = (short)s_nBlocks;
                    s_aNonAlphaGroup[s_nNonAlphaGroup].blockCount = 0;
                    s_aNonAlphaGroup[s_nNonAlphaGroup].owner.layout = layout;
                    set_group_status(layout, &s_aNonAlphaGroup[s_nNonAlphaGroup].flags);
                }
                if (s_nParent != 0 && parent != 0) {
                    parentAlphaCount = parent->alphaCount;
                    parentNonAlphaCount = parent->nonAlphaCount;
                }
                for (index = 0; index < model->blockCount; index++) {
                    block = (NmlModelBlock *)((char *)model + blockOffsets[index]);
                    if (s_nClip != 1) {
                        if ((block->materialFlags & 0x10) != 0 || s_nMapClip != 0) {
                            clipBits = nmlModelCalcClipMat2AllCam(&block->clipBounds, &layout->fields.place,
                                                                  layout->fields.weightedMatrices + block->matrixIndices[0] - 1);
                            layout->fields.clipMasks[index] |= clipBits;
                            if ((layout->fields.clipMasks[index] & 0x80) != 0) {
                                continue;
                            }
                        } else if (layout->fields.clipMasks[index] != 0) {
                            continue;
                        }
                    }
                    if (s_nDispVisible >= 0) {
                        displayed = 0;
                        for (listIndex = 0; listIndex < s_nDispVisible + 1; listIndex++) {
                            if (block->partId == s_aDispVisible[listIndex]) {
                                displayed = 1;
                                break;
                            }
                        }
                        if (displayed) {
                            continue;
                        }
                    }
                    if ((block->flags & 0x48) == 0 && s_nBlocksAlpha + s_nBlocks < 0xfff) {
                        if (((layout->fields.renderLevel & 0x800) == 0 || (s_inLayout.fields.render_status & 0x20) != 0) &&
                            !((block->materialFlags & 6) == 0 &&
                                             (layout->fields.render_status & 0x20) == 0 &&
                                             is_parts_transparency(block) == 0)) {
                            if (s_pMapLast == model && is_block_last_entry(block) != 0) {
                                s_apBlockLast[s_nBlocksAlphaLast] = block;
                                s_aBlockLayoutLast[s_nBlocksAlphaLast++] = s_nModel;
                            } else {
                                alphaGroupDelta = (s_nParent != 0 && parent != 0 && parentAlphaCount != 0) ? -1 : 0;
                                alphaAdded = 1;
                                s_aAlphaGroup[s_nAlphaGroup + alphaGroupDelta].blockCount++;
                                s_apBlock[0xfff - s_nBlocksAlpha] = block;
                                s_aBlockLayout[0xfff - s_nBlocksAlpha++] = s_nModel;
                                alphaBlockCount++;
                                if (parentEntry != 0) {
                                    (&parentEntry->alphaCount)[0]++;
                                }
                                if (s_nParent != 0 && parent != 0 && parentAlphaCount == 0) {
                                    alphaParentAdded = 1;
                                    parent->alphaCount++;
                                }
                            }
                        }
                        if ((block->materialFlags & 2) == 0 && (layout->fields.render_status & 0x20) == 0 &&
                            (block->materialFlagsQword & 0x40004) != 0x40004 && is_parts_transparency(block) == 0) {
                            nonAlphaGroupDelta = (s_nParent != 0 && parent != 0 && parentNonAlphaCount != 0) ? -1 : 0;
                            nonAlphaAdded = 1;
                            s_aNonAlphaGroup[s_nNonAlphaGroup + nonAlphaGroupDelta].blockCount++;
                            s_apBlock[s_nBlocks] = block;
                            s_aBlockLayout[s_nBlocks++] = s_nModel;
                            if (parentEntry != 0) {
                                (&parentEntry->nonAlphaCount)[0]++;
                            }
                            if (s_nParent != 0 && parent != 0 && parentNonAlphaCount == 0) {
                                nonAlphaParentAdded = 1;
                                parent->nonAlphaCount++;
                            }
                        }
                    }
                }
                s_nModel = s_nModel + 1;
                if (s_nAlphaGroup > 0 && s_aAlphaGroup[s_nAlphaGroup - 1].blockCount > 0 &&
                    alphaBlockCount > 0 &&
                    alphaBlockCount < s_aAlphaGroup[s_nAlphaGroup - 1].blockCount &&
                    s_nParent != 0 && parent != 0 && alphaParentAdded == 0 &&
                    (layout->fields.render_status & 0x100000) != 0 && s_nAlphaGroup < 0x1ff) {
                    xglMemCopy2(s_aBLsave, &s_aBlockLayout[0x1000 - s_nBlocksAlpha], alphaBlockCount);
                    xglMemCopy4(s_aBPsave, &s_apBlock[0x1000 - s_nBlocksAlpha], alphaBlockCount);
                    copyCount = s_aAlphaGroup[s_nAlphaGroup - 1].blockCount - alphaBlockCount;
                    if (alphaParentAdded < copyCount) {
                        for (copyIndex = 0; copyIndex < copyCount; copyIndex++) {
                            s_aBlockLayout[0x1000 - s_nBlocksAlpha + copyIndex] =
                                s_aBlockLayout[0x1000 + alphaBlockCount - s_nBlocksAlpha + copyIndex];
                            s_apBlock[0x1000 - s_nBlocksAlpha + copyIndex] =
                                s_apBlock[0x1000 + alphaBlockCount - s_nBlocksAlpha + copyIndex];
                        }
                    }
                    xglMemCopy2(&s_aBlockLayout[1 + s_aAlphaGroup[s_nAlphaGroup - 1].firstBlock - alphaBlockCount],
                                s_aBLsave, alphaBlockCount);
                    xglMemCopy4(&s_apBlock[1 + s_aAlphaGroup[s_nAlphaGroup - 1].firstBlock - alphaBlockCount],
                                s_aBPsave, alphaBlockCount);
                }
                if (alphaAdded != 0 && (s_nParent == 0 || alphaParentAdded != 0) &&
                    s_nAlphaGroup < 0x1ff) {
                    QuadwordCopy(&sortPos, &layout->fields.center);
                    first = (NmlModelBlock *)((char *)model + blockOffsets[0]);
                    if (first->materialFlags & 0x10) {
                        sortPos.z = 10000.0f;
                        sortPos.y = 10000.0f;
                        sortPos.x = 10000.0f;
                    }
                    AlphaGroupSortEntry(&sortPos);
                    s_nAlphaGroup = s_nAlphaGroup + 1;
                }
                if (nonAlphaAdded != 0 && (s_nParent == 0 || nonAlphaParentAdded != 0)) {
                    s_nNonAlphaGroup = s_nNonAlphaGroup + 1;
                }
            }
        }
        break;
    }
    for (groupScan = 0; groupScan < s_nAlphaGroup; groupScan++) {
        if (s_aAlphaGroup[groupScan].flags & 0x10) {
            s_nMapAlphaEntry = 1;
        }
    }
    nmlModelClear();
}

void nmlModelEntryEffect(NmlModel *model)
{
    LayoutStore *layout;
    NmlModelBlock *block;
    int *blockTable;
    int *blockOffsets;
    int paddedBlockCount;
    int quadIndex;
    int index;
    int alphaAdded;
    int nonAlphaAdded;
    int nonAlphaGroupDelta;
    int nextModel;
    Vector4 sortPos;

    nmlPacketSetCurrent();
    if (s_nModel < 0x200 && s_nBlocks + s_nBlocksAlpha + model->blockCount < 0xfff) {
        index = 0;
        blockTable = model->blockOffsets;
        LAYOUT_S32(0x25c) = s_nModel;
        s_fSortOffset = s_fSortOffsetEntry;
        s_inLayout.fields.model = model;
        Vu0AddVectorXYZ(&s_inLayout.fields.center, &s_inLayout.fields.center, &model->entryTranslation);
        _CurSetMatrix(s_inLayout.fields.place);
        _CurApplyMatrix(&s_inLayout.fields.center.v, &s_inLayout.fields.center.v);
        s_inLayout.fields.center.v.w = model->entryTranslation.w;
        s_inLayout.fields.textureTransparency = 1.0f;
        LAYOUT_S32(0x228) = 0;
        s_inLayout.fields.viewportAlpha[s_inLayout.fields.window] = 0.0f;
        s_inLayout.fields.weightedMatrices = nmlPacketSetAttributeData64N(
            (char *)model + model->weightedMatrixOffset,
            (model->partCount + model->sampleCount) * 2);
        LAYOUT_S32(0x264) = 0;
        paddedBlockCount = model->blockCount; /* one clip-mask byte per block, in whole quadwords */
        if ((paddedBlockCount & 0xf) != 0) {
            paddedBlockCount = paddedBlockCount - (paddedBlockCount & 0xf) + 16;
        }
        s_inLayout.fields.clipMasks = nmlPacketSetAttributeAlloc16N(paddedBlockCount / 16);
        for (quadIndex = 0; quadIndex < paddedBlockCount / 16; quadIndex++) {
            QuadwordZero(quadIndex * 16 + (int)s_inLayout.fields.clipMasks);
        }
        layout = nmlPacketSetAttributeData16N(&s_inLayout, 0x32);
        alphaAdded = 0;
        nonAlphaAdded = 0;
        s_apModelLayout[s_nModel] = layout;
        if (s_nAlphaGroup < 0x1ff) {
            s_aAlphaGroup[s_nAlphaGroup].firstBlock = (short)(0xfff - s_nBlocksAlpha);
            s_aAlphaGroup[s_nAlphaGroup].blockCount = 0;
            s_aAlphaGroup[s_nAlphaGroup].sortDistance = 0.0f;
            s_aAlphaGroup[s_nAlphaGroup].owner.layout = layout;
            set_group_status(layout, &s_aAlphaGroup[s_nAlphaGroup].flags);
        }
        if (s_nNonAlphaGroup < 0x1ff) {
            s_aNonAlphaGroup[s_nNonAlphaGroup].firstBlock = (short)s_nBlocks;
            s_aNonAlphaGroup[s_nNonAlphaGroup].blockCount = 0;
            s_aNonAlphaGroup[s_nNonAlphaGroup].owner.layout = layout;
            set_group_status(layout, &s_aNonAlphaGroup[s_nNonAlphaGroup].flags);
        }
        index = 0;
        if (model->blockCount > 0) {
        blockOffsets = blockTable;
        do {
            block = (NmlModelBlock *)((char *)model + *blockOffsets);
            if (s_nBlocksAlpha + s_nBlocks < 0xfff) {
                if ((block->materialFlags & 6) != 0 ||
                    (layout->fields.render_status & 0x20) != 0 ||
                    is_parts_transparency(block) != 0) {
                    s_aAlphaGroup[s_nAlphaGroup].blockCount++;
                    alphaAdded = 1;
                    __builtin_memcpy(&s_apBlock[0xfff - s_nBlocksAlpha], &block, sizeof(block));
                    s_aBlockLayout[0xfff - s_nBlocksAlpha++] = s_nModel;
                }
                if ((block->materialFlags & 2) == 0 &&
                    (layout->fields.render_status & 0x20) == 0 &&
                    (block->materialFlagsQword & 0x40004) != 0x40004 &&
                    is_parts_transparency(block) == 0) {
                    nonAlphaGroupDelta = (s_nParent != 0) ? -1 : 0;
                    nonAlphaAdded = 1;
                    s_aNonAlphaGroup[s_nNonAlphaGroup + nonAlphaGroupDelta].blockCount++;
                    s_apBlock[s_nBlocks] = block;
                    s_aBlockLayout[s_nBlocks++] = s_nModel;
                }
            }
            index++;
            blockOffsets++;
        } while (index < model->blockCount);
        }
        nextModel = s_nModel + 1;
        __builtin_memcpy(&s_nModel, &nextModel, sizeof(nextModel));
        if (alphaAdded != 0 && s_nAlphaGroup < 0x1ff) {
            QuadwordCopy(&sortPos, &layout->fields.center);
            AlphaGroupSortEntry(&sortPos);
            s_nAlphaGroup = s_nAlphaGroup + 1;
        }
        if (nonAlphaAdded != 0) {
            s_nNonAlphaGroup = s_nNonAlphaGroup + 1;
        }
    }
    nmlModelClear();
}

void nmlModelEntryCard(NmlModel *model)
{
    LayoutStore *layout;
    NmlModelBlock *block;
    int *blockTable;
    int *blockOffsets;
    int paddedBlockCount;
    int quadIndex;
    int index;
    int alphaAdded;
    int nonAlphaAdded;
    int nextModel;
    Vector4 sortPos;

    nmlPacketSetCurrent();
    if (s_nModel < 0x200 && s_nBlocks + s_nBlocksAlpha + model->blockCount < 0xfff) {
        __builtin_memset(&index, 0, sizeof(index));
        blockTable = model->blockOffsets;
        LAYOUT_S32(0x25c) = s_nModel;
        s_fSortOffset = s_fSortOffsetEntry;
        s_inLayout.fields.model = model;
        Vu0AddVectorXYZ(&s_inLayout.fields.center, &s_inLayout.fields.center, &model->entryTranslation);
        _CurSetMatrix(s_inLayout.fields.place);
        _CurApplyMatrix(&s_inLayout.fields.center.v, &s_inLayout.fields.center.v);
        s_inLayout.fields.center.v.w = model->entryTranslation.w;
        s_inLayout.fields.textureTransparency = 1.0f;
        LAYOUT_S32(0x228) = 0;
        s_inLayout.fields.viewportAlpha[s_inLayout.fields.window] = 0.0f;
        s_inLayout.fields.renderLevel |= 2;
        xglMatrixReverse(s_inLayout.fields.lightColor, s_inLayout.fields.lightColor);
        s_inLayout.fields.weightedMatrices = nmlPacketSetAttributeData64N(
            (char *)model + model->weightedMatrixOffset,
            (model->partCount + model->sampleCount) * 2);
        LAYOUT_S32(0x264) = 0;
        paddedBlockCount = model->blockCount; /* one clip-mask byte per block, in whole quadwords */
        if ((paddedBlockCount & 0xf) != 0) {
            paddedBlockCount = paddedBlockCount - (paddedBlockCount & 0xf) + 16;
        }
        s_inLayout.fields.clipMasks = nmlPacketSetAttributeAlloc16N(paddedBlockCount / 16);
        for (quadIndex = 0; quadIndex < paddedBlockCount / 16; quadIndex++) {
            QuadwordZero(quadIndex * 16 + (int)s_inLayout.fields.clipMasks);
        }
        layout = nmlPacketSetAttributeData16N(&s_inLayout, 0x32);
        __builtin_memset(&alphaAdded, 0, sizeof(alphaAdded));
        __builtin_memset(&nonAlphaAdded, 0, sizeof(nonAlphaAdded));
        s_apModelLayout[s_nModel] = layout;
        if (s_nAlphaGroup < 0x1ff) {
            s_aAlphaGroup[s_nAlphaGroup].firstBlock = (short)(0xfff - s_nBlocksAlpha);
            s_aAlphaGroup[s_nAlphaGroup].blockCount = 0;
            s_aAlphaGroup[s_nAlphaGroup].sortDistance = 0.0f;
            s_aAlphaGroup[s_nAlphaGroup].owner.layout = layout;
            set_group_status(layout, &s_aAlphaGroup[s_nAlphaGroup].flags);
        }
        if (s_nNonAlphaGroup < 0x1ff) {
            s_aNonAlphaGroup[s_nNonAlphaGroup].firstBlock = (short)s_nBlocks;
            s_aNonAlphaGroup[s_nNonAlphaGroup].blockCount = 0;
            s_aNonAlphaGroup[s_nNonAlphaGroup].owner.layout = layout;
            set_group_status(layout, &s_aNonAlphaGroup[s_nNonAlphaGroup].flags);
        }
        index = 0;
        if (model->blockCount > 0) {
        blockOffsets = blockTable;
        do {
            block = (NmlModelBlock *)((char *)model + *blockOffsets);
            if (s_nBlocksAlpha + s_nBlocks < 0xfff) {
                if ((block->materialFlags & 6) != 0 ||
                    (layout->fields.render_status & 0x20) != 0 ||
                    is_parts_transparency(block) != 0) {
                    s_aAlphaGroup[s_nAlphaGroup].blockCount++;
                    alphaAdded = 1;
                    __builtin_memcpy(&s_apBlock[0xfff - s_nBlocksAlpha], &block, sizeof(block));
                    s_aBlockLayout[0xfff - s_nBlocksAlpha++] = s_nModel;
                }
                if ((block->materialFlags & 2) == 0 &&
                    (layout->fields.render_status & 0x20) == 0 &&
                    (block->materialFlagsQword & 0x40004) != 0x40004 &&
                    is_parts_transparency(block) == 0) {
                    s_aNonAlphaGroup[s_nNonAlphaGroup].blockCount++;
                    nonAlphaAdded = 1;
                    s_apBlock[s_nBlocks] = block;
                    s_aBlockLayout[s_nBlocks++] = s_nModel;
                }
            }
            index++;
            blockOffsets++;
        } while (index < model->blockCount);
        }
        nextModel = s_nModel + 1;
        __builtin_memcpy(&s_nModel, &nextModel, sizeof(nextModel));
        if (alphaAdded != 0 && s_nAlphaGroup < 0x1ff) {
            QuadwordCopy(&sortPos, &layout->fields.center);
            AlphaGroupSortEntry(&sortPos);
            s_nAlphaGroup = s_nAlphaGroup + 1;
        }
        if (nonAlphaAdded != 0) {
            s_nNonAlphaGroup = s_nNonAlphaGroup + 1;
        }
    }
    nmlModelClear();
}

static int nmlModelRenderProreal(LayoutStore *model, int frame)
{
    int groupIndex;
    int index;
    int result;
    Vector4 pointA;
    Vector4 pointB;
    Vector4 pointC;
    Vector4 pointD;
    Vector4 *worldMax;
    Vector4 *worldMin;
    StudioCamera *camera;

    groupIndex = 0;
    result = groupIndex;
    camera = xglStudioSelectGetActiveCamera(model->fields.window);
    if (camera == 0) {
        return 0;
    }
    nmlPacketClrModelCache();
    for (groupIndex = 0; groupIndex < s_nNonAlphaGroup; groupIndex++) {
        int cube;
        int pass;
        int visible;

        visible = (s_aNonAlphaGroup[groupIndex].flags & 0x10) ? (model->fields.texMapMode & 1) : (model->fields.texMapMode & 2);
        if (visible == 0) {
            continue;
        }
        cube = nmlFilterSetVolumeCubeRender(&model->fields.proreal->bounds_min, &model->fields.proreal->bounds_max);
        pass = (model->fields.texMapMode & 0x40000) ? 1 : cube;
        if (model == s_aNonAlphaGroup[groupIndex].owner.layout) {
            continue;
        }
        if ((s_aNonAlphaGroup[groupIndex].flags & 0x10) == 0) {
            Vector4 *bound = &s_aNonAlphaGroup[groupIndex].owner.layout->fields.center.v;
            if (model->fields.proreal->bounds_max.x < bound->x || bound->z > model->fields.proreal->bounds_max.z ||
                bound->x < model->fields.proreal->bounds_min.x || bound->z < model->fields.proreal->bounds_min.z) {
                continue;
            }
            Vu0SubVectorXYZ(&pointA, &model->fields.proreal->reference_point, bound);
            if (pointA.x * model->fields.proreal->plane_normal_x + pointA.z * model->fields.proreal->plane_normal_z < 0.0f) {
                continue;
            }
        }

        nmlPacketGsInit();
        nmlPacketAddGsTexture();
        nmlPacketAddGsZbuf(1);
        nmlPacketAddGsFrame(sRender.display_buffer_base, 0);
        if (pass != 0) {
            nmlPacketAddGsPixeltest(0x51001);
        } else {
            nmlPacketAddGsPixeltest(0x5d001);
        }
        if (s_aNonAlphaGroup[groupIndex].flags & 0x10) {
            nmlPacketAddGsAlpha(model->fields.proreal->alpha);
        } else {
            nmlPacketAddGsAlpha(model->fields.proreal->alpha);
        }
        nmlPacketAddGsFlush();
        nmlPacketAddGifTagStandard(1, 1);

        for (index = s_aNonAlphaGroup[groupIndex].firstBlock;
             index < s_aNonAlphaGroup[groupIndex].firstBlock + s_aNonAlphaGroup[groupIndex].blockCount;
             index++) {
            NmlModelBlockBounds *entry = s_apBlock[index];
            LayoutStore *layout = s_apModelLayout[s_aBlockLayout[index]];

            worldMax = &pointC;
            worldMin = &pointD;
            if (layout->fields.render_status & 0x100000) {
                continue;
            }
            if (s_aNonAlphaGroup[groupIndex].flags & 0x10) {
                if ((int)layout == s_inMapHandle[1] && model->fields.mapShadowPartCount != 0) {
                    if (!is_shadow_map_parts(model, entry->block.partId)) {
                        continue;
                    }
                } else if (entry->block.materialFlags & 0x10) {
                    Vector4 *minCorner = &pointB;
                    float *matrixRow = layout->fields.weightedMatrices[entry->block.matrixIndices[0] - 1].matrix[3];

                    Vu0AddVectorXYZ(&pointA, &entry->bounds.boxMaxOffset, matrixRow);
                    __asm__ __volatile__(
                        "lqc2 vf3, 0(%2)\n\t"
                        "lqc2 vf2, 0(%3)\n\t"
                        "vadd.xyz vf2, vf2, vf3\n\t"
                        "sqc2 vf2, 0(%1)"
                        : "=m"(*minCorner) : "r"(minCorner),
                            "r"(matrixRow),
                            "r"(&entry->bounds.boxMinOffset) : "memory");
                    if (model->fields.proreal->bounds_max.x < pointA.x || pointA.y > model->fields.proreal->bounds_max.y ||
                        pointA.z > model->fields.proreal->bounds_max.z || pointB.x < model->fields.proreal->bounds_min.x ||
                        pointB.y < model->fields.proreal->bounds_min.y || pointB.z < model->fields.proreal->bounds_min.z) {
                        continue;
                    }
                } else {
                    Vu0AddVectorXYZ(worldMax, &layout->fields.model->boxMaxOffset, &layout->fields.place[3]);
                    __asm__ __volatile__(
                        "lqc2 vf3, 0(%2)\n\t"
                        "lqc2 vf2, 0(%3)\n\t"
                        "vadd.xyz vf2, vf2, vf3\n\t"
                        "sqc2 vf2, 0(%1)"
                        : "=m"(*worldMin) : "r"(worldMin),"r"(&layout->fields.place[3]),
                            "r"(&layout->fields.model->boxMinOffset) : "memory");
                    if (model->fields.proreal->bounds_max.x < pointC.x || pointC.y > model->fields.proreal->bounds_max.y ||
                        pointC.z > model->fields.proreal->bounds_max.z || pointD.x < model->fields.proreal->bounds_min.x ||
                        pointD.y < model->fields.proreal->bounds_min.y || pointD.z < model->fields.proreal->bounds_min.z) {
                        continue;
                    }
                }
            }

            layout->fields.render_status |= 0x800;
            layout->fields.prorealOverride = model->fields.proreal;
            nmlPacketAddTransMicrocode(&entry->block,
                                       layout);
            nmlPacketAddTextureParam(&entry->block,
                                     layout);
            nmlPacketAddBlockMaterial(&entry->block,
                                      layout, 0, frame);
            result = nmlPacketAddParts(&entry->block,
                                            layout, camera);
            if (result != 0) {
                break;
            }
            nmlPacketAddGsFlush();
            nmlPacketAddExecProg(&entry->block,
                                 layout, 1, 0);
            layout->fields.render_status &= ~0x800u;
            layout->fields.prorealOverride = 0;
        }
        if (result != 0) {
            return result;
        }
    }
    return result;
}

static int nmlModelRenderTexture(NmlBlockGroup *range, int step,
                                 int frame)
{
    extern float textureTransparencyMin;
    extern void nmlFilterSetTexClear(int);
    extern void nmlFilterSetTexFillDraw(int, LayoutStore *);
    extern void nmlPacketAddGsScissor(u64);
    LayoutStore *firstLayout;
    unsigned int capturedBase;
    int frameBase;
    int result = 0;
    StudioCamera *camera;
    LayoutStore *layout;
    NmlModelBlock *material;
    int index;

    capturedBase = sRender.texture_buffer_base;
    camera = xglStudioSelectGetActiveCamera(frame);
    if (camera != 0) {
        goto test_fov;
    }
    return 0;

render_body:
    nmlPacketGsInit();
    frameBase = capturedBase & 0xffff;
    nmlPacketAddGsZbuf(1);
    nmlPacketAddGsFBA(0);
    nmlPacketAddGsFrame(frameBase, 0);
    nmlPacketAddGsFlush();
    nmlFilterSetTexClear(frameBase);
    nmlPacketGsInit();
    nmlPacketAddGsPixeltest(0x31001);
    nmlPacketAddGsScissor(0x00df000001ff0000ULL);
    nmlPacketAddGsFlush();
    nmlPacketAddGifTagStandard(0, 0);
    if (s_nAnotherStudio != 0) {
        nmlPacketAddScreen(camera);
    }

    index = range->firstBlock - step;
    while (index != range->firstBlock + range->blockCount * step - step) {
        index += step;
        material = s_apBlock[index];
        layout = s_apModelLayout[s_aBlockLayout[index]];
        if ((layout->fields.render_status & 0x100000) == 0) {
            layout->fields.render_status |= 0x8000;
            if (firstLayout == 0) {
                firstLayout = layout;
            }
            nmlPacketAddTransMicrocode(material,
                                       layout);
            result = nmlPacketAddParts(material,
                                       layout,
                                       camera);
            if (result != 0) {
                break;
            }
            nmlPacketAddExecProg(material,
                                 layout, 0, 0);
            layout->fields.render_status &= ~0x8000;
        }
    }

    if (result != 0) {
        goto render_done;
    }
    if (firstLayout != 0) {
        nmlFilterSetTexFillDraw(frameBase, firstLayout);
    }
    nmlPacketGsInit();
    nmlPacketAddGsScissor(0x01bf000001ff0000ULL);
    nmlPacketAddGsFlush();
    if (firstLayout != 0) {
        result = nmlModelRenderProreal(firstLayout, frame);
    }
    goto render_done;

test_fov:
    if (textureTransparencyMin >= range->owner.layout->fields.textureTransparency) {
        goto render_done;
    }
    firstLayout = 0;
    goto render_body;

render_done:
    return result;
}

static int nmlModelRenderCircle(const NmlBlockGroup *group, int pitch, int frame)
{
    extern unsigned short s_aBlockLayout[4096];
    extern float circleViewLimitMin;
    LayoutStore *owner;
    LayoutStore *model;
    int result;
    int index;
    int end;
    int ratio;

    model = 0;
    result = 0;
    owner = group->owner.layout;
    if (owner->fields.textureTransparency <= circleViewLimitMin) {
        result = 0;
    } else {
        index = group->firstBlock - pitch;
        for (;;) {
            end = group->firstBlock + group->blockCount * pitch - pitch;
            if (index == end) {
                break;
            }
            index += pitch;
            model = s_apModelLayout[s_aBlockLayout[index]];
            if ((model->fields.render_status & 0x100000u) != 0) {
                model = 0;
            } else {
                break;
            }
        }
        if (model != 0) {
            ratio = (int) ((float) model->fields.texMapSize * model->fields.circleShadowScale);
            set_circle_shadow_ratio(ratio);
            nmlPacketSendCircleTexture(&s_aCircleShadow, s_aCircleShadowClut);
            result = nmlModelRenderProreal(model, frame);
        }
    }
    return result;
}

static int nmlModelRenderDropCircle(const NmlBlockGroup *group, int pitch, StudioCamera *camera)
{
    int index;
    int end;
    LayoutStore *model = 0;

    do {
        index = group->firstBlock - pitch;
        end = group->firstBlock + group->blockCount * pitch - pitch;
        while (index != end) {
            index += pitch;
            model = s_apModelLayout[s_aBlockLayout[index]];
            if ((model->fields.render_status & 0x100000u) == 0)
                break;
        }
    } while (0);

    set_circle_shadow_ratio((int)((float)model->fields.texMapSize * model->fields.circleShadowScale));
    nmlPacketSendCircleTexture(&s_aCircleShadow, s_aCircleShadowClut);
    nmlPacketMakeCircleTexture(model, camera);
    return 0;
}

static int nmlModelRenderDrop(NmlBlockGroup *range,
                              int step, StudioCamera *screen, int mode)
{
    int partsResult = 0;
    unsigned int index;
    void *block;
    LayoutStore *layout;

    nmlFilterSetFrameAlphaClear();
    nmlPacketGsInit();
    nmlPacketAddGsFBA(1);
    nmlPacketAddGsZbuf(1);
    nmlPacketAddGsAlpha(0x89);
    nmlPacketAddGsPixeltest(0x54000);
    nmlPacketAddGifTagStandard(0, 1);
    nmlPacketAddGsFlush();
    if (s_nAnotherStudio) {
        nmlPacketAddScreen(screen);
    }
    do {
        index = range->firstBlock - step;
        while (index != range->firstBlock + range->blockCount * step - step) {
            index += step;
            block = s_apBlock[index];
            layout = s_apModelLayout[s_aBlockLayout[index]];
            if ((layout->slots[0x94].i & 0x100000) == 0) {
                layout->slots[0x94].i |= 0x80000;
                nmlPacketAddTransMicrocode(block, layout);
                nmlPacketAddBlockMaterial(block, layout, 0, mode);
                partsResult = nmlPacketAddParts(block, layout, screen);
                if (partsResult != 0) {
                    break;
                }
                nmlPacketAddExecProg(block, layout, 0, 0);
                layout->slots[0x94].i &= ~0x80000u;
            }
        }
    } while (0);
    return partsResult;
}

static int nmlModelFlushSubNonAlpha(int viewport, StudioCamera *camera)
{
    int result;
    int group_index;
    int previous_tag;
    NmlModelBlock *material;
    int tag;

    nmlPacketClrModelCache();
    nmlPacketGsInit();
    nmlPacketAddGsPixeltest(0x50000);
    nmlPacketAddGsZbuf(0);
    nmlPacketAddGsFlush();

    group_index = 0;
    result = group_index;
    for (group_index = 0; group_index < s_nNonAlphaGroup; group_index++) {
        LayoutStore *layout;
        int block_index;

        previous_tag = -1;

        if (s_aNonAlphaGroup[group_index].flags & 0x48) {
            continue;
        }

        nmlPacketAddGsFrame(sRender.display_buffer_base,
                            ((s_aNonAlphaGroup[group_index].flags & 2) == 0) ? s_nStencilVal : 0);
        nmlPacketAddGsFBA(s_aNonAlphaGroup[group_index].flags & 2);

        layout = s_apModelLayout[s_aBlockLayout[s_aNonAlphaGroup[group_index].firstBlock]];
        {
            int subwindow = g_aSubWindow[viewport];
            if (subwindow == 1) {
                if (viewport != layout->fields.window) {
                    continue;
                }
            } else if (subwindow == 2) {
                if (layout->fields.window != 0) {
                    continue;
                }
            }
        }

        nmlPacketAddFog(layout, viewport);
        nmlPacketAddGsFogCol(layout->fields.fogColor.color);
        {
            float viewportAlpha = layout->fields.viewportAlpha[viewport];
            nmlPacketAddGsAlpha(((unsigned long long)(int)(viewportAlpha * 128.0f) << 32) | 104);
        }
        nmlPacketAddGsFlush();

        for (block_index = s_aNonAlphaGroup[group_index].firstBlock;
             block_index < s_aNonAlphaGroup[group_index].firstBlock +
                               s_aNonAlphaGroup[group_index].blockCount;
             block_index++) {
                material = s_apBlock[block_index];
                layout = s_apModelLayout[s_aBlockLayout[block_index]];

                if (s_nAnotherStudio != 0) {
                    if (layout->fields.viewportFlags[viewport] != 0 ||
                        ((layout->fields.clipMasks[viewport] >> viewport) & 1) != 0) {
                        continue;
                    }
                }

                tag = (material->materialFlags & 1) |
                      (layout->fields.render_status & 0x01004002);
                if (previous_tag != tag) {
                    previous_tag = tag;
                    nmlPacketAddGifTag(material, layout, 0);
                }
                nmlPacketAddTransMicrocode(material, layout);
                if ((result = nmlPacketAddTexture(layout)) != 0) {
                    break;
                }

                nmlPacketAddTextureParam(material, layout);
                nmlPacketAddPixelControl(material, layout, viewport);
                nmlPacketAddGsFlush();

                if ((material->materialFlags & 0x1000) &&
                    (!(layout->fields.renderLevel & 0x10) ||
                     ((layout->fields.render_status & 0x100000) &&
                      strstr(material->name, eyeMaterialName) != 0))) {
                    nmlPacketAddReflRot(material, layout);
                    nmlPacketAddReflectParam(material, layout);
                }

                nmlPacketAddBlockMaterial(material, layout, 0,
                                          viewport);
                if ((result = nmlPacketAddParts(material, layout, camera)) != 0) {
                    break;
                }
                nmlPacketAddExecProg(material, layout, 0, 0);
        }
    }
    return result;
}

static int nmlModelFlushSubAlpha(int viewport, StudioCamera *camera)
{
    int result;
    int group_index;
    int previous_gif_flags;

    group_index = 0;
    result = group_index;
    nmlPacketClrModelCache();
    for (group_index = 0; group_index < s_nAlphaGroup; ++group_index) {
        LayoutStore *filter_model;
        int display_base;
        int block_index;

        filter_model = 0;
        if (s_aAlphaGroup[group_index].flags & 0x48) {
            continue;
        }
        if (result != 0) {
            break;
        }
        nmlPacketAddGsZbuf(0);
        nmlPacketAddGsAlpha(0x68);
        nmlPacketAddGsFrame(sRender.display_buffer_base,
                            (s_aAlphaGroup[group_index].flags & 2) ? 0xffffff : s_nStencilVal);
        nmlPacketAddGsFrame1(sRender.display_buffer_base,
                             (s_aAlphaGroup[group_index].flags & 2) ? 0xffffff : s_nStencilVal);
        nmlPacketAddGsFBA(s_aAlphaGroup[group_index].flags & 2);
        nmlPacketAddGsFBA1(s_aAlphaGroup[group_index].flags & 2);
        nmlPacketAddGifTagStandard(1, 1);
        nmlPacketAddGsFlush();
        nmlPacketClrModelCache();

        for (block_index = s_aAlphaGroup[group_index].firstBlock;
             block_index > s_aAlphaGroup[group_index].firstBlock -
                               s_aAlphaGroup[group_index].blockCount;
             --block_index) {
            LayoutStore *layout = s_apModelLayout[s_aBlockLayout[block_index]];
            NmlModelBlock *material = s_apBlock[block_index];

            if (s_nAnotherStudio != 0) {
                if (layout->fields.viewportFlags[viewport] != 0) {
                    continue;
                }
                if (((layout->fields.clipMasks[viewport] >> viewport) & 1) != 0) {
                    continue;
                }
            }
            if (layout->fields.packetFilterFlags != 0) {
                filter_model = layout;
            }
            if (g_aSubWindow[viewport] == 1) {
                if (viewport != layout->fields.window) {
                    continue;
                }
            } else if (g_aSubWindow[viewport] == 2) {
                if (layout->fields.window != 0) {
                    continue;
                }
            }
            if ((layout->fields.weightedMatrices[material->partId].part.flags & 1) == 0 &&
                (material->materialFlags & 0x8000) == 0 &&
                (layout->fields.render_status & 8) == 0) {
                continue;
            }
            if (material->materialFlags & 2) {
                continue;
            }
            nmlPacketAddTransMicrocode(material, layout);
            if (material->materialFlags & 4) {
                result = nmlPacketAddTexture(layout);
                if (result != 0) {
                    break;
                }
                nmlPacketAddTextureParam(material, layout);
            }
            nmlPacketAddBlockMaterial(material, layout, 1, viewport);
            result = nmlPacketAddParts(material, layout, camera);
            if (result != 0) {
                break;
            }
            nmlPacketAddPixelControl(material, layout, viewport | 0x30000);
            nmlPacketAddGsFlush();
            nmlPacketAddExecProg(material, layout, 1, 1);
        }

        if (filter_model != 0 && (filter_model->fields.packetFilterFlags & 2) != 0) {
            unsigned int filter_flags = filter_model->fields.packetFilterFlags;
            int *count_ptr = &s_nCount;
            filter_model->fields.packetFilterFlags = 2;
            nmlFilterSetPacket(filter_model, *count_ptr);
            filter_model->fields.packetFilterFlags = filter_flags & ~2u;
        }
        if (result != 0) {
            break;
        }

        {
            if (s_aAlphaGroup[group_index].flags & 0x800) {
                nmlFilterBackClear();
            }
            nmlPacketGsInit();
            nmlPacketAddGsZbuf(1);
            nmlPacketAddGsPixeltest(0x50000);
            display_base = sRender.display_buffer_base;
            if (s_aAlphaGroup[group_index].flags & 0x800) {
                display_base = sRender.flip_base;
            }
            previous_gif_flags = -1;
            nmlPacketAddGsFrame((unsigned short)display_base,
                                (s_aAlphaGroup[group_index].flags & 2) ? 0 : s_nStencilVal);
            nmlPacketAddGsFrame1((unsigned short)display_base,
                                 (s_aAlphaGroup[group_index].flags & 2) ? 0 : s_nStencilVal);
            nmlPacketAddGsFBA(s_aAlphaGroup[group_index].flags & 2);
            nmlPacketAddGsFBA1(s_aAlphaGroup[group_index].flags & 2);
            nmlPacketAddGsFlush();

            for (block_index = s_aAlphaGroup[group_index].firstBlock;
                 block_index > s_aAlphaGroup[group_index].firstBlock -
                                   s_aAlphaGroup[group_index].blockCount;
                 --block_index) {
                LayoutStore *layout = s_apModelLayout[s_aBlockLayout[block_index]];
                NmlModelBlock *material = s_apBlock[block_index];
                unsigned int flags;
                unsigned int render_status;

                if (s_nAnotherStudio != 0) {
                    if (layout->fields.viewportFlags[viewport] != 0) {
                        continue;
                    }
                    if (((layout->fields.clipMasks[viewport] >> viewport) & 1) != 0) {
                        continue;
                    }
                }
                if (g_aSubWindow[viewport] == 1) {
                    if (viewport != layout->fields.window) {
                        continue;
                    }
                } else if (g_aSubWindow[viewport] == 2) {
                    if (layout->fields.window != 0) {
                        continue;
                    }
                }

                if (layout->fields.packetFilterFlags != 0) {
                    filter_model = layout;
                }
                if (filter_model == 0 &&
                    (s_aAlphaGroup[group_index].flags & 0x800) != 0) {
                    filter_model = layout;
                }

                render_status = layout->fields.render_status;
                flags = (material->materialFlags & 1) | (render_status & 0x01004002);
                if (render_status & 0x2000) {
                    flags &= ~(render_status & 0x01000002);
                }
                nmlPacketAddTransMicrocode(material, layout);
                if (previous_gif_flags != flags) {
                    previous_gif_flags = flags;
                    nmlPacketAddGifTag(material, layout, 1);
                }
                result = nmlPacketAddTexture(layout);
                if (result != 0) {
                    break;
                }
                nmlPacketAddTextureParam(material, layout);
                if (flags & 0x01000002) {
                    nmlPacketAddFog(layout, viewport);
                    nmlPacketAddGsFogCol(layout->fields.fogColor.color);
                }
                if ((material->materialFlags & 0x1000) != 0 &&
                    ((layout->fields.renderLevel & 0x10) == 0 ||
                     ((layout->fields.render_status & 0x100000) != 0 &&
                      strstr(material->name, eyeMaterialName) != 0))) {
                    nmlPacketAddReflRot(material, layout);
                }
                nmlPacketAddBlockMaterial(material, layout, 0, viewport);
                result = nmlPacketAddParts(material, layout, camera);
                if (result != 0) {
                    break;
                }
                nmlPacketAddPixelControl(material, layout, viewport | 0x10000);
                if (material->materialFlags & 0x20) {
                    nmlPacketAddGsAlpha(0x48);
                } else if (material->materialFlags & 0x40) {
                    nmlPacketAddGsAlpha(0x42);
                } else {
                    nmlPacketAddGsAlpha(layout->fields.alpha);
                }
                nmlPacketAddGsFlush();
                if ((material->materialFlags & 0x1000) != 0 &&
                    ((layout->fields.renderLevel & 0x10) == 0 ||
                     ((layout->fields.render_status & 0x100000) != 0 &&
                      strstr(material->name, eyeMaterialName) != 0))) {
                    nmlPacketAddReflectParam(material, layout);
                }
                nmlPacketAddExecProg(material, layout, 1, 0);
            }

        }

        if (s_aAlphaGroup[group_index].flags & 0x800) {
            nmlFilterSetFlatRender(filter_model);
        }
        if (s_nUseStealth != 0) {
            nmlFilterSetFrameAlphaClear();
        }
        if (filter_model != 0) {
            nmlFilterSetPacket(filter_model, s_nCount);
        }
        if (result != 0) {
            break;
        }
        if ((s_aAlphaGroup[group_index].flags & 0x10) != 0 && viewport == 0 &&
            s_nUseStealth != 0 && s_nMapAlphaEntry != 0) {
            nmlFilterSetFrameToBuffer(sRender.display_buffer_base);
        }
        if (result != 0) {
            break;
        }
    }
    return result;
}

static int nmlModelFlushSub(int viewport)
{
    int result;
    StudioCamera *camera;
    int index;
    int result_clear;

    camera = xglStudioSelectGetActiveCamera(viewport);
    if (camera == 0) {
        return 0;
    }
    nmlPacketAddScreen(camera);
    nmlPacketGsInit();
    nmlPacketAddGsScissor(
        ((u64)(int)SCISSOR_F(camera, 0) |
         ((u64)(int)SCISSOR_F(camera, 1) << 32)) |
        (((u64)((int)SCISSOR_F(camera, 3) - 1) << 48) |
         ((u64)((int)SCISSOR_F(camera, 2) - 1) << 16)));
    nmlPacketAddGsScissor1(
        ((u64)(int)SCISSOR_F(camera, 0) |
         ((u64)(int)SCISSOR_F(camera, 1) << 32)) |
        (((u64)((int)SCISSOR_F(camera, 3) - 1) << 48) |
         ((u64)((int)SCISSOR_F(camera, 2) - 1) << 16)));
    nmlPacketAddGsFlush();
    if (viewport != 0) {
        nmlPacketAddScreenClear();
        if (s_nAnotherStudio == 0) {
            return 0;
        }
    }
    result = nmlModelFlushSubNonAlpha(viewport, camera);
    if (result == 0 && viewport == 0) {
        for (index = 0; index < s_nDirect; index++) {
            NmlDirectRecord *record = &s_aDirect[index];
            if (record->attribute == 1) {
                result = nmlPacketDirectData(record->data, record->count);
                if (result != 0) {
                    break;
                }
            }
        }
    }
    nmlPacketClrModelCache();
    result_clear = (result == 0);
    if (result_clear) {
        nmlFilterSetFrameAlphaClear();
        index = result;
        for (; index < s_nNonAlphaGroup; index++) {
            unsigned int flags = s_aNonAlphaGroup[index].flags;
            if (flags & 1) {
                result = nmlModelRenderTexture(&s_aNonAlphaGroup[index], 1, viewport);
                if (result != 0) {
                    break;
                }
                continue;
            }
            if (flags & 0x100) {
                result = nmlModelRenderCircle(&s_aNonAlphaGroup[index], 1, viewport);
                if (result != 0) {
                    break;
                }
                continue;
            }
            if (flags & 0x200) {
                result = nmlModelRenderDropCircle(&s_aNonAlphaGroup[index], 1, camera);
                if (result != 0) {
                    break;
                }
                continue;
            }
            if (flags & 0x80) {
                result = nmlModelRenderDrop(&s_aNonAlphaGroup[index], 1, camera, viewport);
                if (result != 0) {
                    break;
                }
            }
            if (result != 0) {
                break;
            }
        }
        {
            int callback_mode = -1;
            for (index = 0; index < s_nAlphaGroup && result == 0; index++) {
                if (s_aAlphaGroup[index].flags & 0x20) {
                    if (s_aAlphaGroup[index].flags & 1) {
                        result = nmlModelRenderTexture(&s_aAlphaGroup[index], callback_mode, viewport);
                        if (result != 0) {
                            break;
                        }
                        continue;
                    } else if (s_aAlphaGroup[index].flags & 0x100) {
                        result = nmlModelRenderCircle(&s_aAlphaGroup[index], callback_mode, viewport);
                        if (result != 0) {
                            break;
                        }
                        continue;
                    } else if (s_aAlphaGroup[index].flags & 0x200) {
                        result = nmlModelRenderDropCircle(&s_aAlphaGroup[index], callback_mode, camera);
                        if (result != 0) {
                            break;
                        }
                        continue;
                    } else if ((s_aAlphaGroup[index].flags & 0x80) == 0) {
                        continue;
                    } else {
                        result = nmlModelRenderDrop(&s_aAlphaGroup[index], callback_mode, camera, viewport);
                        if (result != 0) {
                            break;
                        }
                        continue;
                    }
                }
            }
        }
        if (result == 0) {
            if (viewport == 0 && s_nEffectWrite == 0) {
                nmlPacketGsInit();
                nmlPacketAddGsZbuf(0);
                nmlPacketAddGsPixeltest(0x50000);
                nmlPacketAddGsFrame(sRender.display_buffer_base, 0);
                nmlPacketAddGsFlush();
                sefDrawEffect();
            }
        }
        if (result == 0) {
            if (viewport == 0 && s_nUseStealth != 0 && s_nMapAlphaEntry == 0) {
                nmlFilterSetFrameToBuffer(sRender.display_buffer_base);
            }
        }
        if (result == 0) {
            nmlPacketGsInit();
            nmlPacketAddGsPixeltest(0x50000);
            nmlPacketAddGsFlush();
            nmlFilterSetFrameAlphaClear();
            nmlPacketClrTextureCache();
            result = nmlModelFlushSubAlpha(viewport, camera);
            if (result == 0) {
                for (index = 0; index < s_nDirect; index++) {
                    NmlDirectRecord *record = &s_aDirect[index];
                    if ((record->attribute == 2 && viewport == 0) ||
                        record->attribute - 3 == viewport) {
                        result = nmlPacketDirectData(record->data, record->count);
                        if (result != 0) {
                            break;
                        }
                    }
                }
            }
        }
    }
    nmlPacketGsInit();
    nmlPacketAddGsScissor(((u64)0x01BF << 48) | ((u64)0x01FF << 16));
    nmlPacketAddGsScissor1(((u64)0x01BF << 48) | ((u64)0x01FF << 16));
    nmlPacketAddGsFlush();
    nmlPacketClrTextureCache();
    return result;
}

void nmlModelConstruct(void)
{
    CONSTRUCT_MODELSYSTEM();
    CONSTRUCT_CIRCLR_SHADOW();
    CONSTRUCT_BACK_BUFFER();
    CONSTRUCT_ALPHA_GROUP();
    CONSTRUCT_PARENT_BUF();
    CONSTRUCT_MAP_HANDLE(s_inMapHandle);
    CONSTRUCT_FADE_CONTROL(&s_inFadeIn);
    CONSTRUCT_FADE_CONTROL(&s_inFadeOut);
    CONSTRUCT_FADE_CONTROL(&s_inActiveFadeIn);
    CONSTRUCT_FADE_CONTROL(&s_inActiveFadeOut);
    nmlModelClear();
    s_inLayout.fields.render_status = 0;
}

void nmlModelInit(void)
{
    INIT_MODELSYSTEM();
    INIT_BACK_BUFFER();
    INIT_ALPHA_GROUP();
    INIT_PARENT_BUF();
    INIT_MAP_HANDLE(s_inMapHandle);
    nmlModelClear();
    s_inLayout.fields.render_status = 0;
    nmlModelMpeg2CrossFadeTime[0] = 30;
}

void nmlModelClear(void) {
    CLEAR_LAYOUT_MODEL(&s_inLayout);
    CLEAR_PROREAL(s_inProReal);
    CLEAR_MAP_HANDLE(s_inMapHandle);
    CLEAR_MODEL_ENTRY();
}

void nmlModelFlush(void)
{
    int result = 0;
    int group_index;
    int subwindow_index;
    unsigned int game_flags;

    if (s_nRenderCancel == 0) {
        int back_buffer_mode = s_inBackBuffer[5];
        switch (back_buffer_mode) {
        case 0:
            if (s_inBackBuffer[1] > 0 && s_inBackBuffer[8] == 0) {
                s_nRenderCancel = 1;
            }
            break;
        case 1:
            s_nRenderCancel = back_buffer_mode;
            break;
        }
    }

    AlphaGroupSort();
    AlphaGroupLastEntry();
    nmlPacketAddTransMicrocodeInit();
    nmlPacketGsInit();
    nmlPacketAddGsZbuf(0);
    nmlPacketAddGsZbuf1(1);
    nmlPacketAddGsFBA(0);
    nmlPacketAddGsFBA1(0);
    nmlPacketAddGsPixeltest(0x50000);
    nmlPacketAddGsPixeltest1(0x50000);
    nmlPacketAddGsFrame(sRender.display_buffer_base, 0);
    nmlPacketAddGsFrame1(sRender.display_buffer_base, 0);
    nmlPacketAddGsAlpha1(0x0000008000000068ULL);
    nmlPacketAddGsFlush();

    if (s_nPause == 0) {
        ++s_nCount;
    }
    game_flags = GameLoopState.flags;
    if (game_flags & 0x01000000) {
        s_nStencilVal = 0;
    } else {
        s_nStencilVal = (int)0x80000000u;
    }
    for (subwindow_index = 1; subwindow_index < 4; ++subwindow_index) {
        if (g_aSubWindow[subwindow_index] != 0) {
            s_nAnotherStudio = 1;
        }
    }

    if (s_nRenderCancel == 0) {
        result = nmlModelFlushSub(0);
    }
    if (result == 0 && s_nEffectWrite == 1 && s_nRenderCancel == 0) {
        nmlPacketGsInit();
        nmlPacketAddGsZbuf(0);
        nmlPacketAddGsPixeltest(0x50000);
        nmlPacketAddGsFrame(sRender.display_buffer_base, 0);
        nmlPacketAddGsFlush();
        sefDrawEffect();
    }
    if (result == 0 && s_nUseGnosys != 0) {
        nmlFilterSetFrameToBuffer(sRender.display_buffer_base);
    }
    if (result == 0 && s_nRenderCancel == 0) {
        for (group_index = 1; group_index < 4; ++group_index) {
            if (g_aSubWindow[group_index] != 0) {
                result = nmlModelFlushSub(group_index);
                if (result != 0) {
                    break;
                }
            }
        }
    }

    if (s_inBackBuffer[3] != 0 && --s_inBackBuffer[3] == 0) {
        s_inBackBuffer[0] = 1;
        s_inBackBuffer[1] = s_inBackBuffer[2];
    }
    if (s_inBackBuffer[0] != 0) {
        int display_buffer = (short)sRender.display_buffer_base;
        s_inBackBuffer[0] = 0;
        if (s_inBackBuffer[4] >= 0) {
            display_buffer = (short)s_inBackBuffer[4];
        }
        nmlFilterSetFrameToBuffer((unsigned short)display_buffer);
    }

    {
        int back_buffer_mode = s_inBackBuffer[5];
        switch (back_buffer_mode) {
        case 0: {
            float blend = 1.0f;
            if (s_inBackBuffer[8] != 0) {
                blend = (float)s_inBackBuffer[1] / (float)s_inBackBuffer[2];
            }
            if (s_inBackBuffer[1] > 0) {
                --s_inBackBuffer[1];
                nmlFilterSetBufferRender(blend);
                s_inBackBuffer[7] = 1;
            }
            break;
        }
        case 1:
            nmlFilterSetBufferRender(1.0f);
            s_inBackBuffer[7] = back_buffer_mode;
            if (s_inBackBuffer[6] > 0 && --s_inBackBuffer[6] == 0) {
                INIT_BACK_BUFFER();
            }
            if (s_nFrameLockOff != 0) {
                INIT_BACK_BUFFER();
            }
            break;
        }
    }

    nmlModelFadeDoit();
    nmlPacketGsInit();
    nmlPacketAddGsTexture();
    nmlPacketAddGsClamp(0);
    nmlPacketAddGsZbuf(0);
    nmlPacketAddGsPixeltest(0x50000);
    nmlPacketAddGsAlpha(0x0000008000000044ULL);
    nmlPacketAddGsFrame(sRender.display_buffer_base, 0);
    nmlPacketAddGsFlush();
    if (result != 0 || g_nPacketOver != 0) {
        sRender.render_status = 1;
    }
    g_nPacketOver = 0;
    s_nRenderCancelOld = s_nRenderCancel;
    if (s_nRenderCancel > 0) {
        --s_nRenderCancel;
    } else {
        s_nRenderCancel = 0;
    }
    nmlModelFlushClear();
}

void nmlModelFlushClear(void) {
    FLUSH_MODELSYSTEM();
    FLUSH_ALPHA_GROUP();
    FLUSH_MAP_HANDLE(s_inMapHandle);
    FLUSH_PARENT_BUF();
    FLUSH_BACK_BUFFER();
    nmlModelClear();
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const DoublewordAlignedVector4 proRealOrigin = { 0.0f, 0.0f, 1000.0f, 1.0f };

const char magicCarpetComeName[16] = "MagicCarpetCome";

const DoublewordAlignedVector4 circleScaleVector = { .x = 64.0f, .y = -64.0f, .z = 8000.0f, .w = 1.0f };

const DoublewordAlignedVector4 circleCenterVector = { .x = 64.0f, .y = 64.0f, .z = 1.0f, .w = 1.0f };

const DoublewordAlignedVector4 circleOffsetVector = { .x = 2048.0f, .y = 2048.0f, .z = 0.0f, .w = 1.0f };

const NmlVector4Overlay mapScaleVector = { .v = { .x = 128.0f, .y = -128.0f, .z = 8000.0f, .w = 1.0f } };

const NmlVector4Overlay mapCenterVector = { .v = { .x = 256.0f, .y = 224.0f, .z = 1.0f, .w = 1.0f } };

const NmlVector4Overlay mapOffsetVector = { .v = { .x = 2045.5f, .y = 2046.0f, .z = 0.0f, .w = 1.0f } };
