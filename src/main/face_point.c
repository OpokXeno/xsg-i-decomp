#include "common.h"
#include "shared.h"
#include "main/nml_packet_add.h"
#include "main/xgl_2.h"

typedef int s32;

/*
 * One registered culling volume: the three vectors xglCullingMapSet and
 * xglCullingMapCreate fill in from a 9-float box record, followed by the
 * derived state culling_matrix builds from them and setup_occlusion and
 * check_occlusion read back. position.w and scale.w are set to 1.0f by both
 * writers; rotation.w is never written.
 */
typedef struct CullingVolume {
    Vector4 position;
    Vector4 rotation;
    Vector4 scale;
    Vector4 normalizedDirection;
    Matrix4 transform;
    Matrix4 inverseTransform;
    Vector4 cullingCorners[4];
    Vector4 occlusionPlanes[5];
    float occlusionDepth;
    u8 unmodeled_154[12];
} CullingVolume;

/*
 * s_inCulling is the culling map's own state: an active/ready byte at
 * offset 0, the ten registered volumes at offset 32 (the 352-byte stride
 * xglCullingCheck, xglCullingCheckSeparate, xglCullingCheckSeparateInit,
 * xglCullingMapSet and xglCullingMapCreate all index), an evidenced count
 * at 0xDE0 (compared against 10 and incremented once per registered
 * volume) and an evidenced source value at 0xDE4 (set from a caller
 * argument by xglCullingMapSet and read back by xglCullingMapLastCheck).
 * The map argument itself is retained at 0xDE8 and passed back to
 * nmlModelSetMapLastEntry when the last-entry indices are consumed.
 */
typedef struct CullingMap {
    u8 active;
    u8 unmodeled_001[31];
    CullingVolume volumes[10];
    s32 count;
    s32 source;
    s32 cullingMap;
} CullingMap;

/* The map table stores pointers to separately scaffolded volume and index data. */
typedef struct CullingMapEntry {
    const char *name;
    /* The unchanged loop tests these pointer fields only for nullness. */
    const float *entryCount;
    const s32 *source;
    u8 unmodeled_00C[4];
} CullingMapEntry;

typedef struct CullingMapNameSource {
    u8 unmodeled_000[0x10];
    char name_with_prefix[32];
} CullingMapNameSource;

typedef struct CullingCameraView {
    u8 unmodeled_000[0x1B0];
    Matrix4 occlusionTransform;
} CullingCameraView;

/*
 * Declared as an incomplete array, not a scalar: cc1 -G8 would otherwise
 * classify a plain scalar extern as small data and emit a single
 * gp-relative load, but the original site is two absolute hi/lo
 * instructions; an incomplete-extent declaration keeps the target's
 * unknown size ineligible for that optimization.
 */
extern s32 cullingExistFlag[];

extern int g_aSubWindow[4];
extern const char cullingMapVok03Name[16];
extern const char cullingMapVok04Name[16];
extern const char cullingMapVok07Name[16];
extern const char cullingMapVok10Name[16];
extern const char cullingMapVok11Name[16];
extern const char cullingMapVok12Name[16];
extern const char cullingMapVok12bName[16];
extern const char cullingMapPro02Name[16];
extern const char cullingMapEls10Name[16];
extern const char cullingMapEls09Name[16];
extern const char cullingMapEls02Name[16];
extern const char cullingMapEls02bName[16];
extern const char cullingMapEls03Name[16];
extern const char cullingMapEls04Name[16];
extern const char cullingMapEls04bName[16];
extern const char cullingMapEls01Name[16];
extern const char cullingMapVok24Name[16];
extern const char cullingMapDyu01Name[16];
extern const char cullingMapKou01Name[16];
extern const char cullingMapKou04Name[16];
extern const char cullingMapKou05Name[16];
extern const char cullingMapDyu16Name[16];
extern const char cullingMapDyu15Name[16];
extern const char cullingMapDyu13Name[16];
extern const char cullingMapDyu12Name[16];
extern const char cullingMapDyu10Name[16];
extern const char cullingMapDyu07Name[16];
extern const char cullingMapDyu04Name[16];
extern const char cullingMapKuk02Name[16];
extern const char cullingMapKuk11Name[16];
extern const char cullingMapKuk12Name[16];
extern const char cullingMapKuk16Name[16];
extern const char cullingMapGnu06Name[16];
extern const char cullingMapGnu05Name[16];
extern const char cullingMapGnu04Name[16];
extern const char cullingMapGnu03Name[16];
extern const char cullingMapGnu02Name[16];
extern const char cullingMapGnu01Name[16];
extern const char cullingMapVok13bName[16];
extern const char cullingMapVok13Name[16];
extern const char cullingMapGnk02Name[16];
extern const char cullingMapGnk21Name[16];
extern const char cullingMapGnk20Name[16];
extern const char cullingMapGnk18Name[16];
extern const char cullingMapGnk15Name[16];
extern const char cullingMapGnk14Name[16];
extern const char cullingMapGnk13Name[16];
extern const char cullingMapGnk12Name[16];
extern const char cullingMapGnk11Name[16];
extern const char cullingMapGnk10Name[16];
extern const char cullingMapGnk09Name[16];
extern const char cullingMapGnk04Name[16];
extern const char cullingMapUta17Name[16];
extern const char cullingMapUta13Name[16];
extern const char cullingMapUta08Name[16];
extern const char cullingMapUta07Name[16];
extern const char cullingMapUta06Name[16];
extern const char cullingMapUta05Name[16];
extern const char cullingMapUta04Name[16];
extern const char cullingMapUta03Name[16];
extern const char cullingMapKas20Name[16];
extern const char cullingMapKas19Name[16];
extern const char cullingMapKas18Name[16];
extern const char cullingMapKas13Name[16];
extern const char cullingMapKas12Name[16];
extern const char cullingMapKas04Name[16];
extern const char cullingMapKas31Name[16];
extern const char cullingMapUtk04Name[16];
extern const char cullingMapUtk05Name[16];
extern const u64 cullingForwardAxisVector[2];
extern const u64 cullingCornerLowerLeftVector[2];
extern const u64 cullingCornerLowerRightVector[2];
extern const u64 cullingCornerUpperRightVector[2];
extern const u64 cullingCornerUpperLeftVector[2];

extern unsigned int strlen(const char *string);

void setup_occlusion(CullingVolume *volume, s32 camera);

int strcmp(const char *, const char *);

s32 xglStudioGetActiveCamera(void);

static s32 _ModelCalcClip(s32 model);
s32 xglCullingCheck(s32 camera, s32 model);
static s32 _ModelCalcClipMat1(s32 model, s32 matrix);
static s32 _ModelCalcClipMat2(s32 model, s32 matrix1, s32 matrix2);
s32 xglStudioSelectGetActiveCamera(s32 cameraIndex);
void xglVectorOuter(Vector4 *out, const Vector4 *a, const Vector4 *b);
extern void xglMatrixStackInverse(void);
extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);

extern void xglVectorScaleXYZ(Vector4 *destination, const Vector4 *source,
                              float scale);

static const float s_aCullingBase[9];

static int s_nIgnoreCulling = 0;

static CullingMap s_inCulling;

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
static const float s_aMapCullingPro02[18] = { -28.9f, -0.0f, 1.0f, 0.0f, 1.5707964f, 0.0f, 1.0f, 3.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok03[18] = { 0.0f, -0.2f, 25.5f, -1.5184364f, 0.0f, 0.0f, 18.9f, 11.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok04[27] = { -10.6f, 2.1f, 25.5f, 0.0f, 0.0f, 0.0f, 17.9f, 12.1f, 1.0f, -10.3f, 2.4f, 15.0f, 0.0f, 0.0f, 0.0f, 17.2f, 10.4f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok14[27] = { 0.0f, 0.0f, 3.5f, 1.5707963705062866f, 0.0f, 0.0f, 3.0f, 10.899999618530273f, 1.0f, 0.0f, 0.0f, 18.299999237060547f, 0.0f, 0.0f, 0.0f, 5.9000000953674316f, 4.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok07[27] = { -18.2f, 3.0f, 22.0f, 0.0f, 0.0f, 0.0f, 7.2f, 10.0f, 1.0f, 0.0f, 2.9f, 21.8f, 0.0f, 0.0f, 0.0f, 6.9f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok10[27] = { 9.4f, 0.4f, -9.1f, 0.0f, 0.13962635f, 0.0f, 20.6f, 10.7f, 1.0f, -26.0f, 0.0f, -9.0f, 0.0f, -0.296706f, 0.0f, 9.8f, 9.9f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok11[27] = { 0.0f, 0.4f, 9.5f, -1.3613569f, 0.0f, 0.0f, 2.1f, 4.6f, 1.0f, 0.0f, 0.0f, 6.6f, -0.45378563f, 0.0f, 0.0f, 1.0f, 2.1f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok12[36] = { -2.2f, 2.4f, 7.2f, 0.0f, 0.0f, 0.0f, 19.16f, 10.0f, 1.0f, 16.6f, 2.7f, 0.2f, 0.0f, 1.5707964f, 0.0f, 11.3f, 10.0f, 1.0f, -3.7f, 2.6f, -8.3f, 0.0f, 0.0f, 0.0f, 20.73f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingKou01[18] = { 10.2f, -1.6f, -2.6f, 0.0f, -0.6632251f, 0.0f, 5.6f, 4.3f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const s32 s_aMapLastVok13[12] = { 136, 127, 91, 122, 113, 104, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok13b[12] = { 104, 113, 122, 91, 127, 136, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKou01[12] = { 214, 215, 216, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKou04[12] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKou05[12] = { 61, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu01[12] = { 50, 49, 48, 47, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu02[12] = { 26, 24, 25, 22, 21, 20, 19, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu03[12] = { 32, 33, 51, 52, 53, 54, 155, 61, 60, 63, 116, 129 };

static const s32 s_aMapLastGnu04[12] = { 32, 34, 26, 31, 33, 48, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu05[12] = { 36, 37, 42, 43, 44, 45, 46, 47, 55, 56, -1, -1 };

static const s32 s_aMapLastGnu06[12] = { 236, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk16[12] = { 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk12[12] = { 70, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk11[12] = { 158, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk02[12] = { 113, 114, 115, 116, 117, 118, 119, 93, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu04[12] = { 274, 273, 272, 255, 252, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu07[12] = { 58, 59, 62, 63, 68, 69, 66, 67, 26, -1, -1, -1 };

static const s32 s_aMapLastDyu10[12] = { 98, 78, 17, 325, 67, 66, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu12[12] = { 18, 195, 196, 74, 75, 66, 65, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu13[12] = { 22, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu16[12] = { 32, 30, 62, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu15[12] = { 156, 157, 155, 158, 136, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok24[12] = { 59, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok10[12] = { 172, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok12[12] = { 164, 165, 163, 162, 167, 138, 147, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok12b[12] = { 163, 87, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok03[12] = { 118, 119, 114, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok04[12] = { 124, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok07[12] = { 102, 105, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls03[12] = { 102, 105, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls02[12] = { 57, 58, 84, 85, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls02b[12] = { 56, 57, 59, 60, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls09[12] = { 117, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls10[12] = { 93, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls04[12] = { 70, 71, 74, 75, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls04b[12] = { 75, 74, 70, 71, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastEls01[12] = { 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk02[12] = { 33, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk21[12] = { 80, 81, 64, 65, 79, 78, 77, 76, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk20[12] = { 90, 103, 104, 49, 88, 81, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk18[12] = { 26, 30, 29, 43, 35, 28, 27, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk15[12] = { 76, 67, 68, 66, 65, 69, 61, 56, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk14[12] = { 36, 33, 32, 35, 34, 65, 68, 26, 27, 30, -1, -1 };

static const s32 s_aMapLastGnk13[12] = { 83, 84, 85, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk12[12] = { 70, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk11[12] = { 79, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk10[12] = { 80, 81, 82, 83, 51, 96, 97, 95, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk09[12] = { 24, 23, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk04[12] = { 19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta17[12] = { 36, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta13[12] = { 115, 116, 61, 111, 84, 85, 113, 112, 17, -1, -1, -1 };

static const s32 s_aMapLastUta08[12] = { 57, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta07[12] = { 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta06[12] = { 57, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta05[12] = { 57, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta04[12] = { 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUta03[12] = { 57, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas20[12] = { 101, 44, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas19[12] = { 103, 100, 104, 101, 102, 99, 106, 17, 112, 113, -1, -1 };

static const s32 s_aMapLastKas18[12] = { 38, 37, 29, 30, 35, 34, 33, 31, -1, -1, -1, -1 };

static const s32 s_aMapLastKas13[12] = { 38, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas12[12] = { 71, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas04[12] = { 125, 123, 120, 122, 121, 134, 132, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas31[12] = { 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUtk05[12] = { 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUtk04[12] = { 54, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const CullingMapEntry s_aCullingMap[70] = {
    { cullingMapVok03Name, s_aMapCullingVok03, s_aMapLastVok03, {0, 0, 0, 0} },
    { cullingMapVok04Name, s_aMapCullingVok04, s_aMapLastVok04, {0, 0, 0, 0} },
    { cullingMapVok07Name, s_aMapCullingVok07, s_aMapLastVok07, {0, 0, 0, 0} },
    { cullingMapVok10Name, s_aMapCullingVok10, s_aMapLastVok10, {0, 0, 0, 0} },
    { cullingMapVok11Name, s_aMapCullingVok11, 0, {0, 0, 0, 0} },
    { cullingMapVok12Name, s_aMapCullingVok12, s_aMapLastVok12, {0, 0, 0, 0} },
    { cullingMapVok12bName, s_aMapCullingVok12, s_aMapLastVok12b, {0, 0, 0, 0} },
    { cullingMapPro02Name, s_aMapCullingPro02, 0, {0, 0, 0, 0} },
    { cullingMapEls10Name, 0, s_aMapLastEls10, {0, 0, 0, 0} },
    { cullingMapEls09Name, 0, s_aMapLastEls09, {0, 0, 0, 0} },
    { cullingMapEls02Name, 0, s_aMapLastEls02, {0, 0, 0, 0} },
    { cullingMapEls02bName, 0, s_aMapLastEls02b, {0, 0, 0, 0} },
    { cullingMapEls03Name, 0, s_aMapLastEls03, {0, 0, 0, 0} },
    { cullingMapEls04Name, 0, s_aMapLastEls04, {0, 0, 0, 0} },
    { cullingMapEls04bName, 0, s_aMapLastEls04b, {0, 0, 0, 0} },
    { cullingMapEls01Name, 0, s_aMapLastEls01, {0, 0, 0, 0} },
    { cullingMapVok24Name, 0, s_aMapLastVok24, {0, 0, 0, 0} },
    { cullingMapDyu01Name, s_aMapCullingVok11, 0, {0, 0, 0, 0} },
    { cullingMapKou01Name, s_aMapCullingKou01, s_aMapLastKou01, {0, 0, 0, 0} },
    { cullingMapKou04Name, 0, s_aMapLastKou04, {0, 0, 0, 0} },
    { cullingMapKou05Name, 0, s_aMapLastKou05, {0, 0, 0, 0} },
    { cullingMapDyu16Name, 0, s_aMapLastDyu16, {0, 0, 0, 0} },
    { cullingMapDyu15Name, 0, s_aMapLastDyu15, {0, 0, 0, 0} },
    { cullingMapDyu13Name, 0, s_aMapLastDyu13, {0, 0, 0, 0} },
    { cullingMapDyu12Name, 0, s_aMapLastDyu12, {0, 0, 0, 0} },
    { cullingMapDyu10Name, 0, s_aMapLastDyu10, {0, 0, 0, 0} },
    { cullingMapDyu07Name, 0, s_aMapLastDyu07, {0, 0, 0, 0} },
    { cullingMapDyu04Name, 0, s_aMapLastDyu04, {0, 0, 0, 0} },
    { cullingMapKuk02Name, 0, s_aMapLastKuk02, {0, 0, 0, 0} },
    { cullingMapKuk11Name, 0, s_aMapLastKuk11, {0, 0, 0, 0} },
    { cullingMapKuk12Name, 0, s_aMapLastKuk12, {0, 0, 0, 0} },
    { cullingMapKuk16Name, 0, s_aMapLastKuk16, {0, 0, 0, 0} },
    { cullingMapGnu06Name, 0, s_aMapLastGnu06, {0, 0, 0, 0} },
    { cullingMapGnu05Name, 0, s_aMapLastGnu05, {0, 0, 0, 0} },
    { cullingMapGnu04Name, 0, s_aMapLastGnu04, {0, 0, 0, 0} },
    { cullingMapGnu03Name, 0, s_aMapLastGnu03, {0, 0, 0, 0} },
    { cullingMapGnu02Name, 0, s_aMapLastGnu02, {0, 0, 0, 0} },
    { cullingMapGnu01Name, 0, s_aMapLastGnu01, {0, 0, 0, 0} },
    { cullingMapVok13bName, 0, s_aMapLastVok13b, {0, 0, 0, 0} },
    { cullingMapVok13Name, 0, s_aMapLastVok13, {0, 0, 0, 0} },
    { cullingMapGnk02Name, 0, s_aMapLastGnk02, {0, 0, 0, 0} },
    { cullingMapGnk21Name, 0, s_aMapLastGnk21, {0, 0, 0, 0} },
    { cullingMapGnk20Name, 0, s_aMapLastGnk20, {0, 0, 0, 0} },
    { cullingMapGnk18Name, 0, s_aMapLastGnk18, {0, 0, 0, 0} },
    { cullingMapGnk15Name, 0, s_aMapLastGnk15, {0, 0, 0, 0} },
    { cullingMapGnk14Name, 0, s_aMapLastGnk14, {0, 0, 0, 0} },
    { cullingMapGnk13Name, 0, s_aMapLastGnk13, {0, 0, 0, 0} },
    { cullingMapGnk12Name, 0, s_aMapLastGnk12, {0, 0, 0, 0} },
    { cullingMapGnk11Name, 0, s_aMapLastGnk11, {0, 0, 0, 0} },
    { cullingMapGnk10Name, 0, s_aMapLastGnk10, {0, 0, 0, 0} },
    { cullingMapGnk09Name, 0, s_aMapLastGnk09, {0, 0, 0, 0} },
    { cullingMapGnk04Name, 0, s_aMapLastGnk04, {0, 0, 0, 0} },
    { cullingMapUta17Name, 0, s_aMapLastUta17, {0, 0, 0, 0} },
    { cullingMapUta13Name, 0, s_aMapLastUta13, {0, 0, 0, 0} },
    { cullingMapUta08Name, 0, s_aMapLastUta08, {0, 0, 0, 0} },
    { cullingMapUta07Name, 0, s_aMapLastUta07, {0, 0, 0, 0} },
    { cullingMapUta06Name, 0, s_aMapLastUta06, {0, 0, 0, 0} },
    { cullingMapUta05Name, 0, s_aMapLastUta05, {0, 0, 0, 0} },
    { cullingMapUta04Name, 0, s_aMapLastUta04, {0, 0, 0, 0} },
    { cullingMapUta03Name, 0, s_aMapLastUta03, {0, 0, 0, 0} },
    { cullingMapKas20Name, 0, s_aMapLastKas20, {0, 0, 0, 0} },
    { cullingMapKas19Name, 0, s_aMapLastKas19, {0, 0, 0, 0} },
    { cullingMapKas18Name, 0, s_aMapLastKas18, {0, 0, 0, 0} },
    { cullingMapKas13Name, 0, s_aMapLastKas13, {0, 0, 0, 0} },
    { cullingMapKas12Name, 0, s_aMapLastKas12, {0, 0, 0, 0} },
    { cullingMapKas04Name, 0, s_aMapLastKas04, {0, 0, 0, 0} },
    { cullingMapKas31Name, 0, s_aMapLastKas31, {0, 0, 0, 0} },
    { cullingMapUtk04Name, 0, s_aMapLastUtk04, {0, 0, 0, 0} },
    { cullingMapUtk05Name, 0, s_aMapLastUtk05, {0, 0, 0, 0} },
    { nmlEmptyName, 0, 0, {0, 0, 0, 0} },
};

const char cullingMapUtk05Name[16] = "MC_UTK05";

const char cullingMapUtk04Name[16] = "MC_UTK04";

const char cullingMapKas31Name[16] = "MC_KAS31";

const char cullingMapKas04Name[16] = "MC_KAS04";

const char cullingMapKas12Name[16] = "MC_KAS12";

const char cullingMapKas13Name[16] = "MC_KAS13";

const char cullingMapKas18Name[16] = "MC_KAS18";

const char cullingMapKas19Name[16] = "MC_KAS19";

const char cullingMapKas20Name[16] = "MC_KAS20";

const char cullingMapUta03Name[16] = "MC_UTA03";

const char cullingMapUta04Name[16] = "MC_UTA04";

const char cullingMapUta05Name[16] = "MC_UTA05";

const char cullingMapUta06Name[16] = "MC_UTA06";

const char cullingMapUta07Name[16] = "MC_UTA07";

const char cullingMapUta08Name[16] = "MC_UTA08";

const char cullingMapUta13Name[16] = "MC_UTA13";

const char cullingMapUta17Name[16] = "MC_UTA17";

const char cullingMapGnk04Name[16] = "MC_GNK04";

const char cullingMapGnk09Name[16] = "MC_GNK09";

const char cullingMapGnk10Name[16] = "MC_GNK10";

const char cullingMapGnk11Name[16] = "MC_GNK11";

const char cullingMapGnk12Name[16] = "MC_GNK12";

const char cullingMapGnk13Name[16] = "MC_GNK13";

const char cullingMapGnk14Name[16] = "MC_GNK14";

const char cullingMapGnk15Name[16] = "MC_GNK15";

const char cullingMapGnk18Name[16] = "MC_GNK18";

const char cullingMapGnk20Name[16] = "MC_GNK20";

const char cullingMapGnk21Name[16] = "MC_GNK21";

const char cullingMapGnk02Name[16] = "MC_GNK02";

const char cullingMapVok13Name[16] = "MC_VOK13";

const char cullingMapVok13bName[16] = "MC_VOK13B";

const char cullingMapGnu01Name[16] = "MC_GNU01";

const char cullingMapGnu02Name[16] = "MC_GNU02";

const char cullingMapGnu03Name[16] = "MC_GNU03";

const char cullingMapGnu04Name[16] = "MC_GNU04";

const char cullingMapGnu05Name[16] = "MC_GNU05";

const char cullingMapGnu06Name[16] = "MC_GNU06";

const char cullingMapKuk16Name[16] = "MC_KUK16";

const char cullingMapKuk12Name[16] = "MC_KUK12";

const char cullingMapKuk11Name[16] = "MC_KUK11";

const char cullingMapKuk02Name[16] = "MC_KUK02";

const char cullingMapDyu04Name[16] = "Mc_dyu04";

const char cullingMapDyu07Name[16] = "MC_DYU07";

const char cullingMapDyu10Name[16] = "MC_DYU10";

const char cullingMapDyu12Name[16] = "MC_DYU12";

const char cullingMapDyu13Name[16] = "MC_DYU13";

const char cullingMapDyu15Name[16] = "MC_DYU15";

const char cullingMapDyu16Name[16] = "MC_DYU16";

const char cullingMapKou05Name[16] = "MC_KOU05";

const char cullingMapKou04Name[16] = "MC_KOU04";

const char cullingMapKou01Name[16] = "MC_KOU01";

const char cullingMapDyu01Name[16] = "MC_DYU01";

const char cullingMapVok24Name[16] = "MC_VOK24";

const char cullingMapEls01Name[16] = "MC_ELS01";

const char cullingMapEls04bName[16] = "MC_ELS04B";

const char cullingMapEls04Name[16] = "MC_ELS04";

const char cullingMapEls03Name[16] = "MC_ELS03";

const char cullingMapEls02bName[16] = "MC_ELS02B";

const char cullingMapEls02Name[16] = "mc_els02";

const char cullingMapEls09Name[16] = "MC_ELS09";

const char cullingMapEls10Name[16] = "MC_ELS10";

const char cullingMapPro02Name[16] = "MC_PRO02";

const char cullingMapVok12bName[16] = "MC_VOK12b";

const char cullingMapVok12Name[16] = "mc_vok12";

const char cullingMapVok11Name[16] = "MC_VOK11";

const char cullingMapVok10Name[16] = "MC_VOK10";

const char cullingMapVok07Name[16] = "MC_VOK07";

const char cullingMapVok04Name[16] = "mc_vok04";

static void culling_matrix(CullingVolume *volume)
{
    u64 axis[2];
    u64 corners[4][2];

    xglMatrixStackUnit();
    xglMatrixStackTrans(&volume->position.x);
    xglMatrixStackRotY(volume->rotation.y);
    xglMatrixStackRotX(volume->rotation.x);
    xglMatrixStackRotZ(volume->rotation.z);
    xglMatrixStackScale(&volume->scale.x);
    xglMatrixStackSave(volume->transform);
    xglMatrixStackInverse();
    xglMatrixStackSave(volume->inverseTransform);

    axis[0] = cullingForwardAxisVector[0];
    axis[1] = cullingForwardAxisVector[1];
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddz.xyz vf31, vf29, vf31z\n\t"
        "sqc2 vf31, 0(%2)\n\t"
        :
        : "r"(&axis[0]), "r"(volume->transform), "r"(&volume->normalizedDirection)
        : "memory");
    xglVectorNormal(&volume->normalizedDirection, &volume->normalizedDirection);

    corners[0][0] = cullingCornerLowerLeftVector[0];
    corners[0][1] = cullingCornerLowerLeftVector[1];
    corners[1][0] = cullingCornerLowerRightVector[0];
    corners[1][1] = cullingCornerLowerRightVector[1];
    corners[2][0] = cullingCornerUpperRightVector[0];
    corners[2][1] = cullingCornerUpperRightVector[1];
    corners[3][0] = cullingCornerUpperLeftVector[0];
    corners[3][1] = cullingCornerUpperLeftVector[1];

    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        :
        : "r"(&volume->cullingCorners[0]), "r"(&corners[0][0]),
          "r"(volume->transform)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        :
        : "r"(&volume->cullingCorners[1]), "r"(&corners[1][0]),
          "r"(volume->transform)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        :
        : "r"(&volume->cullingCorners[2]), "r"(&corners[2][0]),
          "r"(volume->transform)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        :
        : "r"(&volume->cullingCorners[3]), "r"(&corners[3][0]),
          "r"(volume->transform)
        : "memory");
}

/* An empty per-volume debug display hook; the caller still passes the volume. */
static void culling_cell_disp(CullingVolume *volume)
{
}

/*
 * Line-plane intersection. lineStart/lineEnd are the two points of the
 * directed line; normal.xyz is the plane normal and normal.w scales
 * lineStart.w to offset planePoint along the normal before use. The
 * intersection point is destination = lineStart + t * (lineEnd - lineStart),
 * with t = (dot(normal, C) - dot(normal, lineStart)) / dot(normal, lineEnd -
 * lineStart) and C = planePoint + normal.xyz * (normal.w * lineStart.w).
 * Only destination's xyz lanes are computed by the vsub.xyz that produces
 * vf15; destination->w keeps whatever is in vf15's w lane going into that
 * sqc2, unrelated to lineStart/lineEnd/planePoint's own w.
 */
void _FacePoint(Vector4 *destination, const Vector4 *lineStart, const Vector4 *lineEnd,
                const Vector4 *normal, const Vector4 *planePoint) {
    __asm__ __volatile__(
        "lqc2 vf22, 0(%0)\n\t"
        "lqc2 vf20, 0(%1)\n\t"
        "lqc2 vf21, 0(%2)\n\t"
        "lqc2 vf23, 0(%3)\n\t"
        "vmulw.xyz vf14, vf22, vf22w\n\t"
        "vmulw.xyz vf14, vf14, vf20w\n\t"
        "vadd.xyz vf23, vf23, vf14\n\t"
        "vaddx.x vf10, vf0, vf22x\n\t"
        "vaddy.x vf11, vf0, vf22y\n\t"
        "vaddz.x vf12, vf0, vf22z\n\t"
        "vsub.xyz vf19, vf21, vf20\n\t"
        "vmulax.x ACC, vf10, vf21x\n\t"
        "vmadday.x ACC, vf11, vf21y\n\t"
        "vmaddz.x vf17, vf12, vf21z\n\t"
        "vmulax.x ACC, vf10, vf19x\n\t"
        "vmadday.x ACC, vf11, vf19y\n\t"
        "vmaddz.x vf18, vf12, vf19z\n\t"
        "vmulax.x ACC, vf10, vf23x\n\t"
        "vmadday.x ACC, vf11, vf23y\n\t"
        "vmaddz.x vf16, vf12, vf23z\n\t"
        "vdiv Q, vf0w, vf18x\n\t"
        "vsub.x vf16, vf17, vf16\n\t"
        "vwaitq\n\t"
        "vmulq.x vf16, vf16, Q\n\t"
        "vmulx.xyz vf15, vf19, vf16x\n\t"
        "vsub.xyz vf15, vf21, vf15\n\t"
        "sqc2 vf15, 0(%4)"
        :
        : "r"(normal), "r"(lineStart), "r"(lineEnd), "r"(planePoint), "r"(destination)
        : "memory"
    );
}

float _CheckLine(const Vector4 *offsetStart, const Vector4 *offsetEnd,
                 const Vector4 *endpointStart, const Vector4 *endpointEnd,
                 const Vector4 *planePoint, const Vector4 *planeNormal)
{
    Vector4 adjustedStart;
    Vector4 adjustedEnd;
    Vector4 endpointDelta;
    Vector4 pointFromStart;
    Vector4 normalOffset;

    xglVectorScaleXYZ(&normalOffset, planeNormal,
                      planeNormal->w * planePoint->w * 0.5f);
    xglVectorScaleXYZ(&adjustedStart, offsetStart, planePoint->w * 0.5f);
    xglVectorScaleXYZ(&adjustedEnd, offsetEnd, planePoint->w * 0.5f);

    __asm__ __volatile__(
        "lqc2 vf3, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&adjustedStart), "r"(endpointStart)
        : "memory");

    __asm__ __volatile__(
        "lqc2 vf3, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&adjustedEnd), "r"(endpointEnd)
        : "memory");

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%0)\n\t"
        "vadd.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&adjustedStart), "r"(&normalOffset)
        : "memory");

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%0)\n\t"
        "vadd.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&adjustedEnd), "r"(&normalOffset)
        : "memory");

    __asm__ __volatile__(
        "lqc2 vf3, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%2)\n\t"
        :
        : "r"(&adjustedStart), "r"(&adjustedEnd),
          "r"(&endpointDelta)
        : "memory");

    __asm__ __volatile__(
        "lqc2 vf3, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%2)\n\t"
        :
        : "r"(&adjustedStart), "r"(planePoint),
          "r"(&pointFromStart)
        : "memory");

    return endpointDelta.x * pointFromStart.x +
           endpointDelta.y * pointFromStart.y +
           endpointDelta.z * pointFromStart.z;
}

static void plane_from_points(const Vector4 *point0, const Vector4 *point1,
                              const Vector4 *point2, Vector4 *plane)
{
    Vector4 edge0;
    Vector4 edge1;
    Vector4 normal;

    edge0.x = point1->x - point0->x;
    edge0.y = point1->y - point0->y;
    edge0.z = point1->z - point0->z;
    edge1.x = point2->x - point0->x;
    edge1.y = point2->y - point0->y;
    edge1.z = point2->z - point0->z;
    xglVectorOuter(&normal, &edge0, &edge1);
    xglVectorNormal(&normal, &normal);
    plane->x = normal.x;
    plane->y = normal.y;
    plane->z = normal.z;
    plane->w = -(normal.x * point0->x + normal.y * point0->y + normal.z * point0->z);
}

void setup_occlusion(CullingVolume *volume, s32 camera)
{
    const CullingCameraView *cameraView = (const CullingCameraView *) camera;
    const Matrix4 *cameraTransform = &cameraView->occlusionTransform;
    Vector4 corners[4];
    Vector4 origin;

    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)"
        :
        : "r"(&corners[0]), "r"(&volume->cullingCorners[0]), "r"(cameraTransform)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)"
        :
        : "r"(&corners[1]), "r"(&volume->cullingCorners[1]), "r"(cameraTransform)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)"
        :
        : "r"(&corners[2]), "r"(&volume->cullingCorners[2]), "r"(cameraTransform)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%0)"
        :
        : "r"(&corners[3]), "r"(&volume->cullingCorners[3]), "r"(cameraTransform)
        : "memory");

    corners[0].z = -corners[0].z;
    corners[1].z = -corners[1].z;
    corners[2].z = -corners[2].z;
    corners[3].z = -corners[3].z;
    volume->occlusionDepth = corners[0].z;
    if (corners[1].z < volume->occlusionDepth) {
        volume->occlusionDepth = corners[1].z;
    }
    if (corners[2].z < volume->occlusionDepth) {
        volume->occlusionDepth = corners[2].z;
    }
    if (corners[3].z < volume->occlusionDepth) {
        volume->occlusionDepth = corners[3].z;
    }
    __asm__ __volatile__("sq $0, 0(%0)" : : "r"(&origin) : "memory");
    plane_from_points(&corners[0], &corners[1], &corners[2], &volume->occlusionPlanes[0]);
    if (0.0f < volume->occlusionPlanes[0].w) {
        plane_from_points(&origin, &corners[0], &corners[1], &volume->occlusionPlanes[1]);
        plane_from_points(&origin, &corners[1], &corners[2], &volume->occlusionPlanes[2]);
        plane_from_points(&origin, &corners[2], &corners[3], &volume->occlusionPlanes[3]);
        plane_from_points(&origin, &corners[3], &corners[0], &volume->occlusionPlanes[4]);
    } else {
        plane_from_points(&corners[2], &corners[1], &corners[0], &volume->occlusionPlanes[0]);
        plane_from_points(&origin, &corners[1], &corners[0], &volume->occlusionPlanes[1]);
        plane_from_points(&origin, &corners[2], &corners[1], &volume->occlusionPlanes[2]);
        plane_from_points(&origin, &corners[3], &corners[2], &volume->occlusionPlanes[3]);
        plane_from_points(&origin, &corners[0], &corners[3], &volume->occlusionPlanes[4]);
    }
}

s32 check_occlusion(CullingVolume *volume, s32 camera, s32 model)
{
    const CullingCameraView *cameraView = (const CullingCameraView *) camera;
    const Matrix4 *cameraTransform = &cameraView->occlusionTransform;
    const Vector4 *modelBounds = (const Vector4 *) model;
    Vector4 viewPoint;
    float negativeZ;
    float negativeW;
    s32 result;

    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%2)"
        :
        : "r"(modelBounds), "r"(cameraTransform), "r"(&viewPoint)
        : "memory");

    result = 0;
    negativeZ = -viewPoint.z;
    viewPoint.z = negativeZ;
    if (!((negativeZ - viewPoint.w) < volume->occlusionDepth)) {
        negativeW = -viewPoint.w;
        if (!(negativeW < viewPoint.x * volume->occlusionPlanes[0].x +
                          viewPoint.y * volume->occlusionPlanes[0].y +
                          viewPoint.z * volume->occlusionPlanes[0].z +
                          volume->occlusionPlanes[0].w) &&
            !(negativeW < viewPoint.x * volume->occlusionPlanes[1].x +
                          viewPoint.y * volume->occlusionPlanes[1].y +
                          viewPoint.z * volume->occlusionPlanes[1].z +
                          volume->occlusionPlanes[1].w) &&
            !(negativeW < viewPoint.x * volume->occlusionPlanes[2].x +
                          viewPoint.y * volume->occlusionPlanes[2].y +
                          viewPoint.z * volume->occlusionPlanes[2].z +
                          volume->occlusionPlanes[2].w) &&
            !(negativeW < viewPoint.x * volume->occlusionPlanes[3].x +
                          viewPoint.y * volume->occlusionPlanes[3].y +
                          viewPoint.z * volume->occlusionPlanes[3].z +
                          volume->occlusionPlanes[3].w) &&
            !(negativeW < viewPoint.x * volume->occlusionPlanes[4].x +
                          viewPoint.y * volume->occlusionPlanes[4].y +
                          viewPoint.z * volume->occlusionPlanes[4].z +
                          volume->occlusionPlanes[4].w)) {
            result = 1;
        }
    }
    return result;
}

s32 xglCullingCheck(s32 camera, s32 model)
{
    s32 i = 0;
    s32 result = 0;

    if (s_nIgnoreCulling == 0) {
        for (i = result; i < s_inCulling.count; i++) {
            setup_occlusion(&s_inCulling.volumes[i], camera);
            result = check_occlusion(&s_inCulling.volumes[i], camera, model);
            if (result != 0) {
                break;
            }
        }
    }
    return result;
}

/*
 * Rebuilds the occlusion planes of every registered volume for camera, so
 * that the xglCullingCheckSeparate calls that follow only have to test
 * model against them. xglCullingCheck does both steps per volume instead.
 */
void xglCullingCheckSeparateInit(s32 camera)
{
    s32 i;

    for (i = 0; i < s_inCulling.count; i++) {
        setup_occlusion(&s_inCulling.volumes[i], camera);
    }
}

s32 xglCullingCheckSeparate(s32 camera, s32 model)
{
    s32 i = 0;
    s32 result = 0;

    for (i = 0; i < s_inCulling.count; i++) {
        result = check_occlusion(&s_inCulling.volumes[i], camera, model);
        if (result != 0) {
            break;
        }
    }
    return result;
}

s32 xglCullingExist(void) {
    return cullingExistFlag[0];
}

void xglCullingMapLastCheck(void)
{
    s32 entry;
    s32 i;

    if (s_inCulling.source != 0) {
        for (i = 0; i < 12; i++) {
            entry = ((const s32 *) s_inCulling.source)[i];
            if (entry >= 0) {
                nmlModelSetMapLastEntry(s_inCulling.cullingMap, entry);
            } else {
                break;
            }
        }
    }
}

void xglCullingMapInit(void)
{
    s_nIgnoreCulling = 0;
    s_inCulling.count = 0;
    s_inCulling.active = 0;
    s_inCulling.source = 0;
}

/*
 * Finds a named culling-map entry in the current map table and returns its
 * index or minus one. The table ends at the first row with entryCount and
 * source both zero.
 */
s32 check_culling_map(const char *name) {
    s32 result = -1;
    s32 i;

    for (i = 0; s_aCullingMap[i].entryCount != 0 || s_aCullingMap[i].source != 0; i++) {
        if (strcmp(s_aCullingMap[i].name, name) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

void xglCullingMapSet(s32 cullingMap)
{
    const CullingMapNameSource *nameSource = (const CullingMapNameSource *) cullingMap;
    const float *record;
    CullingVolume *volume;
    s32 index;
    s32 recordIndex;

    s_inCulling.count = 0;
    s_inCulling.source = 0;
    if (strlen(nameSource->name_with_prefix) < 32) {
        strcpy((char *) &s_inCulling, &nameSource->name_with_prefix[1]);
    }
    index = check_culling_map((const char *) &s_inCulling);
    if (index >= 0) {
        s_inCulling.cullingMap = cullingMap;
        s_inCulling.source = (s32) s_aCullingMap[index].source;
        if (s_aCullingMap[index].entryCount != 0) {
            recordIndex = 0;
            while (1) {
                record = &s_aCullingMap[index].entryCount[recordIndex++ * 9];
                volume = &s_inCulling.volumes[s_inCulling.count];
                if (s_inCulling.count >= 10) {
                    break;
                }
                if (record[6] < 0.0f) {
                    break;
                }
                volume->position.x = record[0];
                volume->position.y = record[1];
                volume->position.z = record[2];
                volume->position.w = 1.0f;
                volume->rotation.x = record[3];
                volume->rotation.y = record[4];
                volume->rotation.z = record[5];
                volume->scale.x = record[6];
                volume->scale.y = record[7];
                volume->scale.z = record[8];
                volume->scale.w = 1.0f;
                culling_matrix(volume);
                s_inCulling.count++;
            }
        }
    }
}

void xglCullingMapCreate(void)
{
    CullingVolume *volume = &s_inCulling.volumes[s_inCulling.count];
    const float *record = s_aCullingBase;

    if (s_inCulling.count < 10) {
        volume->position.x = record[0];
        volume->position.y = record[1];
        volume->position.z = record[2];
        volume->position.w = 1.0f;
        volume->rotation.x = record[3];
        volume->rotation.y = record[4];
        volume->rotation.z = record[5];
        volume->scale.x = record[6];
        volume->scale.y = record[7];
        volume->scale.z = record[8];
        volume->scale.w = 1.0f;
        culling_matrix(volume);
        s_inCulling.count++;
    }
}

void xglCullingMapDisp(void)
{
    s32 remaining;
    CullingVolume *volume;

    if (s_inCulling.count > 0) {
        volume = s_inCulling.volumes;
        remaining = s_inCulling.count;
        do {
            remaining--;
            culling_cell_disp(volume);
            volume++;
        } while (remaining != 0);
    }
}

void xglCullingIgnore(void)
{
    s_nIgnoreCulling = 1;
}

void xglCullingIgnoreOff(void)
{
    s_nIgnoreCulling = 0;
}

/* Empty in this build: the original body is a bare return. */
void xglCullingMapDebug(void)
{
}

/*
 * _ModelCalcClipInit: load the pair of 4x4 clip matrices into VU0 macro-mode
 * registers for the callees that read them without reloading.
 *
 * Eight lqc2 loads at decimal offsets 0,16,...,112 off `clip` establish the
 * parameter as two consecutive 16-byte-aligned 4x4 matrices (128 bytes):
 * clip[0]'s four rows land in vf12..vf15 and clip[1]'s four rows land in
 * vf16..vf19, in address order.
 *
 * The registers persist in VU0 state for _ModelCalcClip, _ModelCalcClipMat1
 * and _ModelCalcClipMat2 -- the only three callees that read vf12..vf19
 * without reloading them. This function's own callers are 8 of the 10
 * nmlModelCalcClip* wrappers, which tail-call _ModelCalcClip right after
 * calling this function (nmlModelCalcClip, nmlModelCalcClipNoCulling,
 * nmlModelCalcClipMat1, nmlModelCalcClipMat2, nmlModelCalcClipCam,
 * nmlModelCalcClipStudio, nmlModelCalcClipMat1AllCam,
 * nmlModelCalcClipMat2AllCam); only nmlModelCalcClipMat1Cam and
 * nmlModelCalcClipMat2Cam instead tail-call _ModelCalcClipMat1 and
 * _ModelCalcClipMat2 respectively. All three consumers read vf16..vf19
 * before vf12..vf15 (_ModelCalcClipMat1 at 0x0023f154 then 0x0023f1c0;
 * _ModelCalcClipMat2 at 0x0023f01c then 0x0023f088; _ModelCalcClip at
 * 0x0023f260 then 0x0023f2cc): the bank order does not follow either
 * callee's own "Mat1"/"Mat2" name, so there is no mat1/mat2 register
 * nickname to derive from the consumers -- clip[0] and clip[1] are just
 * the two matrices in address order. nmlModelCalcClipMat1 corroborates the
 * 128-byte extent by passing camera+0x4F0 as this function's argument
 * (0x0023f468).
 *
 * Eight lqc2 loads, nothing else: no branch, no stack frame, no other memory
 * access. cc1 leaves the bare `j $31` delay slot to the assembler; this object
 * is assembled with -g, which disables delay-slot filling, so the return keeps
 * the original `jr $31; nop`.
 */
static void _ModelCalcClipInit(const Matrix4 clip[2])
{
    __asm__ __volatile__("lqc2 vf12, 0(%0)"   :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf13, 16(%0)"  :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf14, 32(%0)"  :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf15, 48(%0)"  :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf16, 64(%0)"  :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf17, 80(%0)"  :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf18, 96(%0)"  :: "r"(clip) : "memory");
    __asm__ __volatile__("lqc2 vf19, 112(%0)" :: "r"(clip) : "memory");
}

/*
 * _ModelCalcClipMat2: transform the bounding sphere at `model` (xyz centre,
 * w radius) by `matrix1` and then `matrix2` (each with its w row cleared) and
 * test it against the clip matrices _ModelCalcClipInit left in vf12..vf19.
 * Returns 1 as soon as one plane distance is not below the radius, else 0.
 * Original hand-written VU0 assembly (labels _$mcc2_*).
 */
static s32 _ModelCalcClipMat2(s32 model, s32 matrix1, s32 matrix2)
{
    s32 clipped;

    __asm__ __volatile__(
        "lqc2 vf10,0(%1)\n\t"
        "lqc2 vf27,0(%2)\n\t"
        "lqc2 vf28,16(%2)\n\t"
        "lqc2 vf29,32(%2)\n\t"
        "lqc2 vf30,48(%2)\n\t"
        "vsubw.w vf27w,vf27w,vf27w\n\t"
        "vsubw.w vf28w,vf28w,vf28w\n\t"
        "vsubw.w vf29w,vf29w,vf29w\n\t"
        "lqc2 vf20,0(%3)\n\t"
        "lqc2 vf21,16(%3)\n\t"
        "lqc2 vf22,32(%3)\n\t"
        "lqc2 vf23,48(%3)\n\t"
        "vsubw.w vf20w,vf20w,vf20w\n\t"
        "vsubw.w vf21w,vf21w,vf21w\n\t"
        "vsubw.w vf22w,vf22w,vf22w\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf20x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf20y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf20z\n\t"
        "vmaddw.xyzw vf20xyzw,vf30xyzw,vf20w\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf21x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf21y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf21z\n\t"
        "vmaddw.xyzw vf21xyzw,vf30xyzw,vf21w\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf22x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf22y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf22z\n\t"
        "vmaddw.xyzw vf22xyzw,vf30xyzw,vf22w\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf23x\n\t"
        "vmove.xyzw vf27xyzw,vf20xyzw\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf23y\n\t"
        "vmove.xyzw vf28xyzw,vf21xyzw\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf23z\n\t"
        "vmove.xyzw vf29xyzw,vf22xyzw\n\t"
        "vmaddw.xyzw vf30xyzw,vf30xyzw,vf23w\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf10x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf10y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf10z\n\t"
        "vmaddw.xyzw vf20xyzw,vf30xyzw,vf0w\n\t"
        "vaddw.x vf2x,vf0x,vf10w\n\t"
        "qmfc2 $2,vf2\n\t"
        "mtc1 $2,$f1\n\t"
        "vmulax.xyzw ACC,vf16xyzw,vf20x\n\t"
        "vmadday.xyzw ACC,vf17xyzw,vf20y\n\t"
        "vmaddaz.xyzw ACC,vf18xyzw,vf20z\n\t"
        "vmaddw.xyzw vf9xyzw,vf19xyzw,vf0w\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmulax.xyzw ACC,vf12xyzw,vf20x\n\t"
        "vmadday.xyzw ACC,vf13xyzw,vf20y\n\t"
        "vmaddaz.xyzw ACC,vf14xyzw,vf20z\n\t"
        "vmaddw.xyzw vf9xyzw,vf15xyzw,vf0w\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "sub $2,$2,$2\n\t"
        "j 2f\n\t"
        "1:\n\t"
        "sub $2,$2,$2\n\t"
        "addi $2,$2,1\n\t"
        "2:\n\t"
        "move %0,$2"
        : "=r"(clipped)
        : "r"(model), "r"(matrix1), "r"(matrix2)
        : "$2", "$f0", "$f1", "memory");
    return clipped;
}

/*
 * _ModelCalcClipMat1: the same test after one transform by `matrix`
 * (labels _$mcc1_*).
 */
static s32 _ModelCalcClipMat1(s32 model, s32 matrix)
{
    s32 clipped;

    __asm__ __volatile__(
        "lqc2 vf10,0(%1)\n\t"
        "lqc2 vf27,0(%2)\n\t"
        "lqc2 vf28,16(%2)\n\t"
        "lqc2 vf29,32(%2)\n\t"
        "lqc2 vf30,48(%2)\n\t"
        "vsubw.w vf27w,vf27w,vf27w\n\t"
        "vsubw.w vf28w,vf28w,vf28w\n\t"
        "vsubw.w vf29w,vf29w,vf29w\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf10x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf10y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf10z\n\t"
        "vmaddw.xyzw vf20xyzw,vf30xyzw,vf0w\n\t"
        "vaddw.x vf2x,vf0x,vf10w\n\t"
        "qmfc2 $2,vf2\n\t"
        "mtc1 $2,$f1\n\t"
        "vmulax.xyzw ACC,vf16xyzw,vf20x\n\t"
        "vmadday.xyzw ACC,vf17xyzw,vf20y\n\t"
        "vmaddaz.xyzw ACC,vf18xyzw,vf20z\n\t"
        "vmaddw.xyzw vf9xyzw,vf19xyzw,vf0w\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmulax.xyzw ACC,vf12xyzw,vf20x\n\t"
        "vmadday.xyzw ACC,vf13xyzw,vf20y\n\t"
        "vmaddaz.xyzw ACC,vf14xyzw,vf20z\n\t"
        "vmaddw.xyzw vf9xyzw,vf15xyzw,vf0w\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "sub $2,$2,$2\n\t"
        "j 2f\n\t"
        "1:\n\t"
        "sub $2,$2,$2\n\t"
        "addi $2,$2,1\n\t"
        "2:\n\t"
        "move %0,$2"
        : "=r"(clipped)
        : "r"(model), "r"(matrix)
        : "$2", "$f0", "$f1", "memory");
    return clipped;
}

/*
 * _ModelCalcClip: test the untransformed bounding sphere at `model` against
 * the six planes of the clip matrices in vf16..vf19 and vf12..vf15
 * (labels _$mcc_*).
 */
static s32 _ModelCalcClip(s32 model)
{
    s32 clipped;

    __asm__ __volatile__(
        "lqc2 vf10,0(%1)\n\t"
        "vaddw.x vf2x,vf0x,vf10w\n\t"
        "qmfc2 $2,vf2\n\t"
        "mtc1 $2,$f1\n\t"
        "vmulax.xyzw ACC,vf16xyzw,vf10x\n\t"
        "vmadday.xyzw ACC,vf17xyzw,vf10y\n\t"
        "vmaddaz.xyzw ACC,vf18xyzw,vf10z\n\t"
        "vmaddw.xyzw vf9xyzw,vf19xyzw,vf0w\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmulax.xyzw ACC,vf12xyzw,vf10x\n\t"
        "vmadday.xyzw ACC,vf13xyzw,vf10y\n\t"
        "vmaddaz.xyzw ACC,vf14xyzw,vf10z\n\t"
        "vmaddw.xyzw vf9xyzw,vf15xyzw,vf0w\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "vmr32.xyzw vf9xyzw,vf9xyzw\n\t"
        "qmfc2 $2,vf9\n\t"
        "mtc1 $2,$f0\n\t"
        "c.lt.s $f0,$f1\n\t"
        "bc1f 1f\n\t"
        "sub $2,$2,$2\n\t"
        "j 2f\n\t"
        "1:\n\t"
        "sub $2,$2,$2\n\t"
        "addi $2,$2,1\n\t"
        "2:\n\t"
        "move %0,$2"
        : "=r"(clipped)
        : "r"(model)
        : "$2", "$f0", "$f1", "memory");
    return clipped;
}

s32 nmlModelCalcClip(s32 model) {
    s32 clip;
    s32 camera = xglStudioGetActiveCamera();

    if (camera != 0) {
        _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
        clip = _ModelCalcClip(model);
        clip |= xglCullingCheck(camera, model);
        return clip;
    }
    return camera;
}

s32 nmlModelCalcClipNoCulling(s32 model) {
    s32 camera = xglStudioGetActiveCamera();

    if (camera != 0) {
        _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
        return _ModelCalcClip(model);
    }
    return camera;
}

s32 nmlModelCalcClipMat1(s32 model, s32 matrix) {
    Vector4 bounds;
    s32 clip;
    s32 camera = xglStudioGetActiveCamera();

    if (camera != 0) {
        __asm__ __volatile__(
            "lqc2 vf31, 0(%0)\n\t"
            "lqc2 vf27, 0(%1)\n\t"
            "lqc2 vf28, 16(%1)\n\t"
            "lqc2 vf29, 32(%1)\n\t"
            "lqc2 vf30, 48(%1)\n\t"
            "vmulax.xyz ACC, vf27, vf31x\n\t"
            "vmadday.xyz ACC, vf28, vf31y\n\t"
            "vmaddaz.xyz ACC, vf29, vf31z\n\t"
            "vmaddw.xyz vf31, vf30, vf0w\n\t"
            "sqc2 vf31, 0(%2)\n\t"
            :
            : "r"(model), "r"(matrix), "r"(&bounds)
            : "memory"
        );
        _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
        clip = _ModelCalcClip((s32) &bounds);
        clip |= xglCullingCheck(camera, (s32) &bounds);
        return clip;
    }
    return camera;
}

s32 nmlModelCalcClipMat2(s32 model, s32 matrix1, s32 matrix2) {
    Vector4 bounds;
    s32 clip;
    s32 camera = xglStudioGetActiveCamera();

    if (camera != 0) {
        __asm__ __volatile__(
            "lqc2 vf31, 0(%0)\n\t"
            "lqc2 vf27, 0(%1)\n\t"
            "lqc2 vf28, 16(%1)\n\t"
            "lqc2 vf29, 32(%1)\n\t"
            "lqc2 vf30, 48(%1)\n\t"
            "vmulax.xyz ACC, vf27, vf31x\n\t"
            "vmadday.xyz ACC, vf28, vf31y\n\t"
            "vmaddaz.xyz ACC, vf29, vf31z\n\t"
            "vmaddw.xyz vf31, vf30, vf0w\n\t"
            "lqc2 vf27, 0(%2)\n\t"
            "lqc2 vf28, 16(%2)\n\t"
            "lqc2 vf29, 32(%2)\n\t"
            "lqc2 vf30, 48(%2)\n\t"
            "vmulax.xyz ACC, vf27, vf31x\n\t"
            "vmadday.xyz ACC, vf28, vf31y\n\t"
            "vmaddaz.xyz ACC, vf29, vf31z\n\t"
            "vmaddw.xyz vf31, vf30, vf0w\n\t"
            "sqc2 vf31, 0(%3)\n\t"
            :
            : "r"(model), "r"(matrix1), "r"(matrix2), "r"(&bounds)
            : "memory"
        );
        _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
        clip = _ModelCalcClip((s32) &bounds);
        clip |= xglCullingCheck(camera, (s32) &bounds);
        return clip;
    }
    return camera;
}

void nmlModelCalcClipCam(s32 model, s32 camera) {
    _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
    _ModelCalcClip(model);
}

void nmlModelCalcClipMat1Cam(s32 model, s32 matrix, s32 camera) {
    _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
    _ModelCalcClipMat1(model, matrix);
}

void nmlModelCalcClipMat2Cam(s32 model, s32 matrix1, s32 matrix2, s32 camera) {
    _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
    _ModelCalcClipMat2(model, matrix1, matrix2);
}

s32 nmlModelCalcClipStudio(s32 model, s32 cameraIndex) {
    s32 clip = 0;
    s32 camera = xglStudioSelectGetActiveCamera(cameraIndex);

    if (camera != 0) {
        _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
        clip = _ModelCalcClip(model);
        clip |= xglCullingCheck(camera, model);
    }
    return clip;
}

s32 nmlModelCalcClipMat1AllCam(s32 model, s32 matrix)
{
    Vector4 bounds;
    s32 clipped = 1;
    s32 camera;
    s32 clip;
    s32 i;

    for (i = 0; i < 4; i++) {
        camera = xglStudioSelectGetActiveCamera(i);
        if (camera == 0) {
            return camera;
        }
        if (g_aSubWindow[i] != 0) {
            __asm__ __volatile__(
                "lqc2 vf31, 0(%0)\n\t"
                "lqc2 vf27, 0(%1)\n\t"
                "lqc2 vf28, 16(%1)\n\t"
                "lqc2 vf29, 32(%1)\n\t"
                "lqc2 vf30, 48(%1)\n\t"
                "vmulax.xyz ACC, vf27, vf31x\n\t"
                "vmadday.xyz ACC, vf28, vf31y\n\t"
                "vmaddaz.xyz ACC, vf29, vf31z\n\t"
                "vmaddw.xyz vf31, vf30, vf0w\n\t"
                "sqc2 vf31, 0(%2)\n\t"
                :
                : "r"(model), "r"(matrix), "r"(&bounds)
                : "memory"
            );
            _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
            clip = _ModelCalcClip((s32) &bounds);
            clip |= xglCullingCheck(camera, (s32) &bounds);
            if (clip == 0) {
                clipped = 0;
                break;
            }
        }
    }
    return clipped;
}

s32 nmlModelCalcClipMat2AllCam(s32 model, s32 matrix1, s32 matrix2)
{
    Vector4 bounds;
    s32 clipMask = 0;
    s32 tested = 0;
    s32 total = 0;
    s32 camera;
    s32 clip;
    s32 i;

    for (i = 0; i < 3; i++) {
        if (g_aSubWindow[i] != 0) {
            camera = xglStudioSelectGetActiveCamera(i);
            if (camera == 0) {
                return camera;
            }
            _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
            __asm__ __volatile__(
                "lqc2 vf31, 0(%0)\n\t"
                "lqc2 vf27, 0(%1)\n\t"
                "lqc2 vf28, 16(%1)\n\t"
                "lqc2 vf29, 32(%1)\n\t"
                "lqc2 vf30, 48(%1)\n\t"
                "vmulax.xyz ACC, vf27, vf31x\n\t"
                "vmadday.xyz ACC, vf28, vf31y\n\t"
                "vmaddaz.xyz ACC, vf29, vf31z\n\t"
                "vmaddw.xyz vf31, vf30, vf0w\n\t"
                "lqc2 vf27, 0(%2)\n\t"
                "lqc2 vf28, 16(%2)\n\t"
                "lqc2 vf29, 32(%2)\n\t"
                "lqc2 vf30, 48(%2)\n\t"
                "vmulax.xyz ACC, vf27, vf31x\n\t"
                "vmadday.xyz ACC, vf28, vf31y\n\t"
                "vmaddaz.xyz ACC, vf29, vf31z\n\t"
                "vmaddw.xyz vf31, vf30, vf0w\n\t"
                "sqc2 vf31, 0(%3)\n\t"
                :
                : "r"(model), "r"(matrix1), "r"(matrix2), "r"(&bounds)
                : "memory"
            );
            clip = _ModelCalcClip((s32) &bounds);
            tested++;
            clipMask |= clip << i;
            total += clip;
        }
    }
    if (tested != 0) {
        if (total == tested) {
            clipMask |= 0x80;
        }
    }
    return clipMask;
}

void _ApplyMatrix33(Vector4 *destination, const Matrix4 matrix, const Vector4 *vector) {
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddz.xyz vf31, vf29, vf31z\n\t"
        "sqc2 vf31, 0(%2)"
        :
        : "r"(vector), "r"(matrix), "r"(destination)
        : "memory"
    );
}

void _ApplyMatrix(Vector4 *destination, const Matrix4 matrix, const Vector4 *vector) {
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%2)"
        :
        : "r"(vector), "r"(matrix), "r"(destination)
        : "memory"
    );
}

void _ApplyMatrix2Mat(Vector4 *destination, const Matrix4 matrix1, const Matrix4 matrix2, const Vector4 *vector) {
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "lqc2 vf27, 0(%2)\n\t"
        "lqc2 vf28, 16(%2)\n\t"
        "lqc2 vf29, 32(%2)\n\t"
        "lqc2 vf30, 48(%2)\n\t"
        "vmulax.xyz ACC, vf27, vf31x\n\t"
        "vmadday.xyz ACC, vf28, vf31y\n\t"
        "vmaddaz.xyz ACC, vf29, vf31z\n\t"
        "vmaddw.xyz vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%3)"
        :
        : "r"(vector), "r"(matrix1), "r"(matrix2), "r"(destination)
        : "memory"
    );
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char cullingMapVok03Name[16] = "MC_VOK03";

static const float s_aCullingBase[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

const u64 cullingForwardAxisVector[2] = { 0ULL, 4575657222473777152ULL };

const u64 cullingCornerLowerLeftVector[2] = { 13799029261476036608ULL, 4575657221408423936ULL };

const u64 cullingCornerLowerRightVector[2] = { 13799029259328552960ULL, 4575657221408423936ULL };

const u64 cullingCornerUpperRightVector[2] = { 4575657222473777152ULL, 4575657221408423936ULL };

const u64 cullingCornerUpperLeftVector[2] = { 4575657224621260800ULL, 4575657221408423936ULL };
