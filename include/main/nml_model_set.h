#ifndef INCLUDE_MAIN_NML_MODEL_SET_H
#define INCLUDE_MAIN_NML_MODEL_SET_H

#include "shared.h"
#include "main/xgl_2.h"
#include "main/nml_packet_add.h"

#define NML_MAP_SHADOW_PARTS_MAX 8

/* Slot of a render layout store: one 32-bit word read as float or int. */
typedef union {
    float f;
    int i;
} LayoutSlot;

/*
 * Request bits of the layout store's render-status word (LayoutStore +0x250).
 * Each nmlModelSet* setter ORs its own bit; the render entries set the
 * microcode-family bit around nmlPacketAddTransMicrocode.
 */
enum NmlRenderStatus {
    NML_RENDER_PLACE = 0x1,           /* nmlModelSetPlace */
    NML_RENDER_FOG = 0x2,             /* nmlModelSetFogCol / SetFogDist */
    NML_RENDER_STENCIL = 0x4,
    NML_RENDER_ZWRITE = 0x8,
    NML_RENDER_LIGHT = 0x10,          /* nmlModelSetLight */
    NML_RENDER_TOUMEI = 0x20,         /* see-through (toumei) parts */
    NML_RENDER_HUMAN_MODEL = 0x40,
    NML_RENDER_PROREAL = 0x800,       /* nmlModelSetTexProreal / RenderProreal */
    NML_RENDER_POINT_LIGHT = 0x1000,  /* nmlModelSetPointLight */
    NML_RENDER_FOG_CANCEL = 0x2000,
    NML_RENDER_TEXMAP_TEXTURE = 0x4000, /* nmlModelSetTexMap, plain texture kind */
    NML_RENDER_TEXTURE_PASS = 0x8000, /* set while nmlModelRenderTexture runs */
    NML_RENDER_MAP_ENTRY = 0x10000,   /* nmlModelSetMapEntry / SetShadowMapEntry */
    NML_RENDER_CULLED = 0x20000,      /* nmlModelCalcSample: clipped out of every active window */
    NML_RENDER_TEXMAP_DROP = 0x40000, /* nmlModelSetTexMap, drop kind */
    NML_RENDER_DROP_PASS = 0x80000,   /* set while nmlModelRenderDrop runs */
    NML_RENDER_FACE_MODEL = 0x100000, /* nmlModelSetFaceModel */
    NML_RENDER_TEXMAP_CIRCLE = 0x200000,
    NML_RENDER_TEXMAP_DROP_CIRCLE = 0x400000,
    NML_RENDER_BACK_CLEAR = 0x800000, /* group becomes NML_GROUP_BACK_CLEAR */
    NML_RENDER_GLOBAL_FOG = 0x1000000, /* nmlModelSetGlobalFogCol / Dist */
    NML_RENDER_GLOBAL_POINT_LIGHT = 0x2000000
};

/*
 * Kind flags of one NmlBlockGroup (alpha and non-alpha groups alike), built
 * by set_group_status from the render-status bits above.
 */
enum NmlGroupFlag {
    NML_GROUP_TEXTURE = 0x1,
    NML_GROUP_STENCIL = 0x2,
    NML_GROUP_FOG = 0x4,
    NML_GROUP_PROREAL = 0x8,
    NML_GROUP_MAP = 0x10,
    NML_GROUP_TOUMEI = 0x20,
    NML_GROUP_CULLED = 0x40,
    NML_GROUP_DROP = 0x80,
    NML_GROUP_CIRCLE = 0x100,
    NML_GROUP_DROP_CIRCLE = 0x200,
    NML_GROUP_ZWRITE = 0x400,
    NML_GROUP_BACK_CLEAR = 0x800
};

/*
 * Block material flags (NmlModelBlock.materialFlags).  The blend bits pick the
 * GS ALPHA register the alpha-group pass loads (additive 0x48, subtractive
 * 0x42); the reflect bit gates nmlPacketAddReflRot / nmlPacketAddReflectParam.
 */
#define NML_BLOCK_MATERIAL_NAMED_PARTS 0x10u
enum NmlMaterialFlag {
    NML_MATERIAL_BLEND_ADD = 0x20,
    NML_MATERIAL_BLEND_SUB = 0x40,
    NML_MATERIAL_REFLECT = 0x1000,
    NML_MATERIAL_LIGHT_PARA = 0x20000 /* lit by the single light parameter set */
};

/* Render-level word (LayoutStore +0x2b0) bits. */
enum NmlRenderLevel {
    NML_LEVEL_NO_SPECULAR = 0x2
};

/*
 * A loaded model resource.  Its block count is +0x44 and the table at +0xb0
 * holds byte offsets from this record to individual NmlModelBlock records.
 */
typedef struct NmlModel {
    unsigned char unmodeled_000[0x3f];
    signed char unclassified_3f;
    int partCount; /* +0x40 */
    int blockCount;
    int sampleCount; /* +0x48 */
    unsigned char unmodeled_04c[4];
    int weightedMatrixOffset; /* +0x50: byte offset from the model to its weighted matrices */
    unsigned char unmodeled_054[4];
    int weightOffset;    /* +0x58: byte offset from the model to the sample weights */
    int transformOffset; /* +0x5c: byte offset from the model to the sample transforms */
    u32 geometryTableOffset; /* +0x60: byte offset to the geometry block offset table */
    unsigned char unmodeled_064[8];
    u32 textureSetOffset; /* +0x6c: byte offset to the model's texture set; 0 = none */
    Vector4 boxMinOffset; /* +0x70: bounding box corners relative to the placement */
    Vector4 boxMaxOffset; /* +0x80 */
    Vector4 entryTranslation; /* +0x90; w at +0x9c */
    int unclassified_a0;
    unsigned char unmodeled_0a4[0x0c];
    int blockOffsets[1];
} NmlModel;

/*
 * The block material parameter parcel at +0xd0 of a model block: the five
 * quadwords nmlPacketAddBlockMaterial sends.  Its bytes come from the model
 * resource, so the words are evidenced by their readers only.
 */
typedef union NmlBlockMaterialParameter {
    u32 words[20];
    float lanes[20];
    XglQuadword quadwords[5];
} NmlBlockMaterialParameter;

/*
 * One named mesh block (material) of a loaded model, as nmlModelEntry lists
 * it in s_apBlock and the nml packet builders render it.
 */
typedef struct NmlModelBlock {
    /*
     * +0x00: the block (material) name.  The name matcher compares it, and
     * add_exec_prog searches it with strstr for lenz_name and eye_name (main
     * 0x0023877c and 0x00238794).
     */
    char name[0x20];
    /*
     * +0x20: the block's visibility and render flags.  Bit 0 picks the
     * environment microcode family in nmlPacketAddTransMicrocode (main
     * 0x00236d24 and 0x00236d68); bits 1 and 2 select the face and back
     * program variants the s_aUcode*FaceAdr / s_aUcode*BackAdr rows of
     * s_aUcodeTbl hold; bits 7, 8 and 9 choose the program index in
     * nmlPacketAddExecProg and bit 10 suppresses its extended pair.
     */
    u32 flags;
    /* +0x24: the span nmlPacketAddTransData sends: dataOffset is added to
     * the block's own address, dataSize is its byte length. */
    int dataOffset;
    int dataSize;
    int partId; /* +0x2c; also the geometry block the material is drawn with */
    unsigned short matrixIndices[48]; /* +0x30; transform then weighted entries */
    Vector4 clipBounds; /* +0x90; w is the weighted clip radius */
    u32 geometryIndex; /* +0xa0 */
    u32 vertexIndexOffset;
    u32 vertexIndexCount;
    u16 vertexIndexBase;
    unsigned char unmodeled_0ae[2];
    XglQuadword gifTag; /* +0xb0 */
    /*
     * +0xc0: the material's blend and lighting flags.  add_exec_prog reads it
     * with lw (main 0x002387e8) and so does nmlPacketAddReflRot (main
     * 0x00239a40); nmlPacketAddTransMicrocode reads the same address with ld
     * (main 0x00236d40), spanning +0xc0..+0xc7.  AddBlockMaterial reads the
     * alpha scale at +0xc4 as a float.
     */
    union {
        u64 materialFlagsQword;
        struct {
            u32 materialFlags;
            float materialAlphaScale;
        };
    };
    unsigned char unmodeled_0c8[4];
    u32 alphaTag; /* +0xcc */
    union {
        NmlBlockMaterialParameter materialParameters; /* +0xd0 */
        struct {
            unsigned char unmodeled_0d0[0x2c];
            float reflectAlpha; /* +0xfc */
            Vector4 multiplyColor; /* +0x100 */
            float translationX, translationY;
            float translationStepX, translationStepY;
        };
    };
    struct {
        u32 buffer;
        u32 width;
    } textureParameters[6]; /* +0x120 */
    unsigned char unmodeled_150[0x1c];
    signed char shapeMathParameter;
    signed char shapeMode, shapeWeight;
    /* +0x16f: the shape blender reads it unsigned (lbu); the material tag
     * nmlPacketAddBlockMaterial sends carries it sign-extended (lb). */
    union {
        unsigned char shapeFlags;
        signed char alphaMode;
    };
    char materialName[32]; /* +0x170 */
} NmlModelBlock;

/* The same 16 bytes as two doublewords, for the plain ld/sd copies. */
typedef struct NmlQword {
    unsigned long long lo;
    unsigned long long hi;
} NmlQword;

/* A vector on a doubleword boundary: the original copies it with ld/sd. */
typedef union NmlVector4Overlay {
    Vector4 v;
    NmlQword q;
} NmlVector4Overlay;

typedef struct NmlPartPixelAlpha {
    short alpha;
    short partId;
} NmlPartPixelAlpha;

/* One 0x40-byte weighted record: a transform matrix, or the part flags word
 * at +0x1c (matrix[1][3]).  The packet builders send it as four vectors. */
typedef struct NmlPartFlagData {
    unsigned char unmodeled_00[0x1c];
    unsigned int flags; /* +0x1c */
    unsigned char unmodeled_20[0x40 - 0x20];
} NmlPartFlagData;

typedef union NmlWeightedMatrix {
    Matrix4 matrix;
    Vector4 rows[4];
    NmlPartFlagData part;
} NmlWeightedMatrix;

/* +0x1c0 of a layout: the fog colour parcel; the fourth word is the integer
 * fog intensity nmlModelEntry and nmlPacketAddFog store. */
typedef struct NmlFogColor {
    float color[3];
    int intensity;
} NmlFogColor;

typedef union NmlPartsReferenceScratch {
    u64 halves[2];
    Vector4 vector;
    float components[4];
} NmlPartsReferenceScratch;

/*
 * The "proreal" parameter block: the 0x1e0 bytes nmlModelSetProreal builds in
 * s_inProReal (main 0x0095b940) and nmlModelEntry copies into the packet with
 * nmlPacketSetAttributeData16N(s_inProReal, 0x1e), storing the packet address
 * it returns in the layout's proreal (main 0x00233028-0x00233030).
 */
typedef struct NmlProRealParam {
    unsigned char unmodeled_000[0x40];
    Matrix projection_transform;
    Matrix4 normal_transform;
    u32 block_material_data[16];
    unsigned char unmodeled_100[0x1c];
    /*
     * +0x11c: nmlModelSetProreal clears it (main 0x0022f9dc) and the two
     * projection entries set it to 1 immediately after calling that function,
     * nmlModelCalcEntryProjectCircle (main 0x002320ec) and
     * nmlModelCalcEntryProjectMap (main 0x00232680), so it marks a block built
     * for a projected entry.  add_exec_prog reads it to take the projected
     * slot of the "Pro" microcode family instead of the plain one.
     */
    u32 projection_entry;
    unsigned char unmodeled_120[0x10];
    GsTag texture_gs_parameters[2];
    unsigned char unmodeled_140[0x20];
    /* +0x160: the last normal row; the map projection's volume test reads
     * its x and z as the clipping plane normal. */
    union {
        XglQuadword final_normal_row;
        struct {
            float plane_normal_x;
            unsigned char unmodeled_164[4];
            float plane_normal_z;
        };
    };
    unsigned char unmodeled_170[0x20];
    NmlPartsReferenceScratch reference_point; /* +0x190 */
    Vector4 bounds_min; /* +0x1a0 */
    Vector4 bounds_max; /* +0x1b0 */
    void *texture; /* +0x1c0 */
    unsigned char unmodeled_1c4[4];
    u64 alpha; /* +0x1c8 */
} NmlProRealParam;

struct NmlStealthGeometry;

/*
 * The model layout the engine renders from: the single s_inLayout (main
 * 0x004a91e0, 0x320 bytes) that the nmlModelSet* API fills, plus the
 * per-entry copies nmlModelEntry stores in s_apModelLayout.  The packet
 * builders and the filter renderers read the same record.
 */
union LayoutStore {
    LayoutSlot slots[200];
    unsigned long long qwords[100];
    struct {
        /* +0x0; row 3 w (+0x3c) carries the block material alpha the drop
         * shadow path computes. */
        Matrix4 lightDirection;
        Matrix4 lightColor; /* +0x40; rows 0-2 are the normal light vectors */
        Matrix4 place; /* +0x80 */
        Matrix4 inversePlace; /* +0xc0 */
        Vector4 pointPosition[4]; /* +0x100 */
        Vector4 pointColor[4]; /* +0x140 */
        Matrix4 projectionMatrix; /* +0x180; saved by the circle and map projection paths */
        NmlFogColor fogColor; /* +0x1c0 */
        Vector4 fogPara; /* +0x1d0 */
        float filterAlpha; /* +0x1e0 */
        float filterAmplitude; /* +0x1e4 */
        unsigned char unmodeled_1e8[8];
        /* +0x1f0: the model centre (w = radius): copied as a projection input,
         * passed to AlphaGroupSortEntry, the circle shadow centre and the
         * stealth filter's fallback sphere. */
        NmlVector4Overlay center;
        Vector4 multiplyColor; /* +0x200 */
        Vector4 currentPosition;            /* +0x210; copied from the weighted position by nmlModelCalcSample */
        float transparency; /* +0x220; nmlModelSetTransparency (also +0x2c0) */
        float reflTransparency; /* +0x224; nmlModelSetReflTransparency */
        float defaultBlockAlpha; /* +0x228 */
        float alphaLimit; /* +0x22c */
        union {
            u32 alphaOptions;
            float alphaOptionsValue;
        }; /* +0x230 */
        unsigned int packetFilterFlags;
        int pixelAlpha;                     /* +0x238, full-width pixel-alpha word */
        void *currentMatrix;                /* +0x23c; the model matrix nmlModelSetMatrix stores */
        const void *texture;                /* +0x240; the texture set nmlModelSetTexture stores */
        int texfunc;                        /* +0x244 */
        u64 alpha;                          /* +0x248 */
        /*
         * +0x250: the render-status word.  nmlModelSetRenderStatus is the
         * setter that names it, and it ORs the value it is given (main
         * 0x0022fab0 sets 0x100, 0x0022fad0 sets 0x200); the rest of the
         * nmlModelSet* API ORs one request bit each into the same word -
         * nmlModelSetPlace 0x1 (main 0x0022ff24), nmlModelSetFogCol 0x2
         * (0x00230030), nmlModelSetStencil 0x4 (0x0022fd28), nmlModelSetZwrite
         * 0x8 (0x0022fd48), nmlModelSetLight 0x10 (0x0022fe50),
         * nmlModelSetToumei 0x20 (0x0022fdf0), nmlModelSetHumanModel 0x40
         * (0x0022e760), nmlModelSetTexProreal 0x800 (0x0022fa5c),
         * nmlModelSetPointLight 0x1000 (0x0022f55c), nmlModelSetFogCancel
         * 0x2000 (0x0023034c), nmlModelSetTexMap 0x4000 (0x0022f670) and
         * nmlModelSetFaceModel 0x100000 (0x0022e73c).  The three render
         * entries set the bit that picks their microcode family just before
         * calling nmlPacketAddTransMicrocode: nmlModelRenderProreal 0x800
         * (main 0x002346c4), nmlModelRenderTexture 0x8000 (0x00234938) and
         * nmlModelRenderDrop 0x80000 (0x00234d58).
         */
        u32 render_status;
        NmlModel *model; /* +0x254; also how a child entry finds its parent layout */
        NmlWeightedMatrix *weightedMatrices; /* +0x258 */
        unsigned char unmodeled_25c[4];
        /*
         * +0x260: this layout's own proreal block, the packet address
         * nmlModelEntry stores after copying s_inProReal (main 0x00233030).
         */
        NmlProRealParam *proreal;
        /*
         * +0x264: another layout's proreal block, borrowed for the length of
         * one render.  nmlModelRenderProreal stores the entry's own +0x260
         * here (main 0x002346dc) and clears it again afterwards (main
         * 0x0023473c); nmlModelEntry clears it as well (main 0x00233034).
         * add_exec_prog reads it in preference to proreal whenever it is set.
         */
        NmlProRealParam *prorealOverride;
        int texMapMode; /* +0x268; supplied by map and actor shadow setup */
        float textureTransparency;
        int texMapSize; /* +0x270 */
        float texMapWeight; /* +0x274 */
        int mapShadowPartCount; /* +0x278 */
        int window; /* +0x27c; nmlModelSetWindow */
        struct NmlStealthGeometry *stealthParts[10]; /* +0x280 */
        int shadowHeightEnabled; /* +0x2a8 */
        float shadowHeight; /* +0x2ac */
        /*
         * +0x2b0: the render-level word nmlModelSetRenderLevel ORs its whole
         * argument into (main 0x0022e97c).  nmlModelSetSpecularOff sets bit 1
         * (main 0x0022f588), and nmlModelCalcSpecularClip sets and clears that
         * same bit (main 0x00231ae0, 0x00231bf4) and sets bit 4 (0x00231bd4).
         */
        unsigned int renderLevel;
        int partPixelAlphaCount; /* +0x2b4 */
        unsigned char *clipMasks; /* +0x2b8 */
        float specialBlockAlpha; /* +0x2bc */
        float circleShadowScale; /* +0x2c0 */
        float circleScale; /* +0x2c4 */
        int stealthEntryCount; /* +0x2c8 */
        unsigned char unmodeled_2cc[4];
        NmlPartPixelAlpha partPixelAlpha[4]; /* +0x2d0 */
        unsigned short mapShadowParts[NML_MAP_SHADOW_PARTS_MAX]; /* +0x2e0 */
        float fogIntensity[4]; /* +0x2f0; one fog intensity multiplier per viewport */
        float viewportAlpha[4];
        int viewportFlags[4];
    } fields;
};

typedef union LayoutStore LayoutStore;


void nmlModelDirectSend(int mode, const void *data, int count);
void nmlModelSetSortOffset(float offset);
void nmlModelSetGlobalPointLightCol(int index, const Vector4 *color);
void nmlModelSetGlobalPointLightPos(unsigned int index, const Vector4 *position);
void nmlModelSetSpecularOff(int disableSpecular);
void nmlModelSetRenderStatus(int status);
void nmlModelSetClip(int clip);
int nmlModelSetGlobalFogReset(void);
void nmlModelSetFogCancel(void);
void nmlModelSetMapLastEntry(void *map, int blockId);
void nmlModelFlush(void);

/* Functions and data used by other translation units. */
void nmlModelSetZwrite(int enabled);
void nmlModelSetStencil(int enabled);
void nmlModelSetToumei(int enabled);
void nmlModelSetAlpha(u64 alpha);
void nmlModelSetTransparency(float transparency);
void nmlModelSetLight(Matrix4 *direction, Matrix4 *color);
void nmlModelClear(void);
void nmlModelSetWindow(u32 window);
int nmlModelSetRenderLevel(int level);
void nmlModelSetTexture(const char *texture);
void nmlModelSetPlace(Matrix4 *place);
void nmlModelSetFogCol(const Vector4 *color);
void nmlModelSetFogDist(float fogNear, float fogFar, float fogMin, float fogMax);
void nmlModelSetMapClip(int enabled);
void nmlModelUseSubWindow(unsigned int index, int enabled);
void nmlModelGetGblPosition(Vector4 *position);
void nmlModelSetMapShadowParts(int partId);
void nmlModelSetTexMap(int mode, int size, float weight);
/* Marks model fades for processing during the next render pass. */
void nmlModelSetFadeDoit(void);
/*
 * nmlModelSetFadeInCancel/nmlModelSetFadeOutCancel/nmlModelSetBackBuffer
 * (below, this TU) and nmlModelSetBackBufferToBattle (still INCLUDE_ASM)
 * are used before their definitions; declare them so these calls do not
 * see an implicit declaration.
 */
void nmlModelSetFadeInCancel(int frames);
void nmlModelSetEffectWrite(int enabled);
void nmlModelConstruct(void);
void nmlModelSetGlobalFogDist(float fogNear, float fogFar, float fogMin, float fogMax);
void nmlModelDirectSendXtx(int modelId, void *data);
void nmlModelSetGlobalPointLightReset(void);
void nmlModelSetFadeOutCancel(int frames);
void nmlModelSetMapLastInit(void);
void nmlModelSetFilter(int filter, float start, float end);
void nmlModelSetReflTransparency(float transparency);
void nmlModelSetBackBuffer(int modelId, int duration, int ownerIndex, int value);
void nmlModelSetBackBufferToBattle(int ownerIndex);
void nmlModelFlushClear(void);
void nmlModelSetFadeIn(int duration, int mode);

#endif /* INCLUDE_MAIN_NML_MODEL_SET_H */
