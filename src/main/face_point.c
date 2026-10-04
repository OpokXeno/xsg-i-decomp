#include "common.h"
#include "shared.h"

static int s_nIgnoreCulling = 0;

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
    u8 unmodeled_030[304];
} CullingVolume;

/*
 * s_inCulling is the culling map's own state: an active/ready byte at
 * offset 0, the ten registered volumes at offset 32 (the 352-byte stride
 * xglCullingCheck, xglCullingCheckSeparate, xglCullingCheckSeparateInit,
 * xglCullingMapSet and xglCullingMapCreate all index), an evidenced count
 * at 0xDE0 (compared against 10 and incremented once per registered
 * volume) and an evidenced source value at 0xDE4 (set from a caller
 * argument by xglCullingMapSet and read back by xglCullingMapLastCheck).
 */
typedef struct CullingMap {
    u8 active;
    u8 unmodeled_001[31];
    CullingVolume volumes[10];
    s32 count;
    s32 source;
} CullingMap;

static CullingMap s_inCulling;

INCLUDE_ASM("asm/main/nonmatchings/face_point", culling_matrix);

/* Empty in this build: the original body is a bare return. */
static void culling_cell_disp(void)
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
        "sqc2 vf15, 0(%4)\n\t"
        "nop\n\t"
        :
        : "r"(normal), "r"(lineStart), "r"(lineEnd), "r"(planePoint), "r"(destination)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/face_point", _CheckLine);

INCLUDE_ASM("asm/main/nonmatchings/face_point", plane_from_points);

INCLUDE_ASM("asm/main/nonmatchings/face_point", setup_occlusion);

INCLUDE_ASM("asm/main/nonmatchings/face_point", check_occlusion);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheck);

void setup_occlusion(CullingVolume *volume, s32 camera);

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheckSeparate);

/*
 * Declared as an incomplete array, not a scalar: cc1 -G8 would otherwise
 * classify a plain scalar extern as small data and emit a single
 * gp-relative load, but the original site is two absolute hi/lo
 * instructions; an incomplete-extent declaration keeps the target's
 * unknown size ineligible for that optimization.
 */
extern s32 D_00969760[];

s32 xglCullingExist(void) {
    return D_00969760[0];
}

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapLastCheck);

void xglCullingMapInit(void)
{
    s_nIgnoreCulling = 0;
    s_inCulling.count = 0;
    s_inCulling.active = 0;
    s_inCulling.source = 0;
}

/* The map table stores pointers to separately scaffolded volume and index data. */
typedef struct CullingMapEntry {
    const char *name;
    /* The unchanged loop tests these pointer fields only for nullness. */
    const float *entryCount;
    const s32 *source;
    u8 unmodeled_00C[4];
} CullingMapEntry;

static const float s_aMapCullingKou01[18];
static const float s_aMapCullingPro02[18];
static const float s_aMapCullingVok03[18];
static const float s_aMapCullingVok04[27];
static const float s_aMapCullingVok07[27];
static const float s_aMapCullingVok10[27];
static const float s_aMapCullingVok11[27];
static const float s_aMapCullingVok12[36];
static const s32 s_aMapLastDyu04[12];
static const s32 s_aMapLastDyu07[12];
static const s32 s_aMapLastDyu10[12];
static const s32 s_aMapLastDyu12[12];
static const s32 s_aMapLastDyu13[12];
static const s32 s_aMapLastDyu15[12];
static const s32 s_aMapLastDyu16[12];
static const s32 s_aMapLastEls01[12];
static const s32 s_aMapLastEls02[12];
static const s32 s_aMapLastEls02b[12];
static const s32 s_aMapLastEls03[12];
static const s32 s_aMapLastEls04[12];
static const s32 s_aMapLastEls04b[12];
static const s32 s_aMapLastEls09[12];
static const s32 s_aMapLastEls10[12];
static const s32 s_aMapLastGnk02[12];
static const s32 s_aMapLastGnk04[12];
static const s32 s_aMapLastGnk09[12];
static const s32 s_aMapLastGnk10[12];
static const s32 s_aMapLastGnk11[12];
static const s32 s_aMapLastGnk12[12];
static const s32 s_aMapLastGnk13[12];
static const s32 s_aMapLastGnk14[12];
static const s32 s_aMapLastGnk15[12];
static const s32 s_aMapLastGnk18[12];
static const s32 s_aMapLastGnk20[12];
static const s32 s_aMapLastGnk21[12];
static const s32 s_aMapLastGnu01[12];
static const s32 s_aMapLastGnu02[12];
static const s32 s_aMapLastGnu03[12];
static const s32 s_aMapLastGnu04[12];
static const s32 s_aMapLastGnu05[12];
static const s32 s_aMapLastGnu06[12];
static const s32 s_aMapLastKas04[12];
static const s32 s_aMapLastKas12[12];
static const s32 s_aMapLastKas13[12];
static const s32 s_aMapLastKas18[12];
static const s32 s_aMapLastKas19[12];
static const s32 s_aMapLastKas20[12];
static const s32 s_aMapLastKas31[12];
static const s32 s_aMapLastKou01[12];
static const s32 s_aMapLastKou04[12];
static const s32 s_aMapLastKou05[12];
static const s32 s_aMapLastKuk02[12];
static const s32 s_aMapLastKuk11[12];
static const s32 s_aMapLastKuk12[12];
static const s32 s_aMapLastKuk16[12];
static const s32 s_aMapLastUta03[12];
static const s32 s_aMapLastUta04[12];
static const s32 s_aMapLastUta05[12];
static const s32 s_aMapLastUta06[12];
static const s32 s_aMapLastUta07[12];
static const s32 s_aMapLastUta08[12];
static const s32 s_aMapLastUta13[12];
static const s32 s_aMapLastUta17[12];
static const s32 s_aMapLastUtk04[12];
static const s32 s_aMapLastUtk05[12];
static const s32 s_aMapLastVok03[12];
static const s32 s_aMapLastVok04[12];
static const s32 s_aMapLastVok07[12];
static const s32 s_aMapLastVok10[12];
static const s32 s_aMapLastVok12[12];
static const s32 s_aMapLastVok12b[12];
static const s32 s_aMapLastVok13[12];
static const s32 s_aMapLastVok13b[12];
static const s32 s_aMapLastVok24[12];

static const CullingMapEntry s_aCullingMap[70] = {
    { ((const char *)0x004D49D8), s_aMapCullingVok03, s_aMapLastVok03, {0, 0, 0, 0} },
    { ((const char *)0x004D49C8), s_aMapCullingVok04, s_aMapLastVok04, {0, 0, 0, 0} },
    { ((const char *)0x004D49B8), s_aMapCullingVok07, s_aMapLastVok07, {0, 0, 0, 0} },
    { ((const char *)0x004D49A8), s_aMapCullingVok10, s_aMapLastVok10, {0, 0, 0, 0} },
    { ((const char *)0x004D4998), s_aMapCullingVok11, 0, {0, 0, 0, 0} },
    { ((const char *)0x004D4988), s_aMapCullingVok12, s_aMapLastVok12, {0, 0, 0, 0} },
    { ((const char *)0x004D4978), s_aMapCullingVok12, s_aMapLastVok12b, {0, 0, 0, 0} },
    { ((const char *)0x004D4968), s_aMapCullingPro02, 0, {0, 0, 0, 0} },
    { ((const char *)0x004D4958), 0, s_aMapLastEls10, {0, 0, 0, 0} },
    { ((const char *)0x004D4948), 0, s_aMapLastEls09, {0, 0, 0, 0} },
    { ((const char *)0x004D4938), 0, s_aMapLastEls02, {0, 0, 0, 0} },
    { ((const char *)0x004D4928), 0, s_aMapLastEls02b, {0, 0, 0, 0} },
    { ((const char *)0x004D4918), 0, s_aMapLastEls03, {0, 0, 0, 0} },
    { ((const char *)0x004D4908), 0, s_aMapLastEls04, {0, 0, 0, 0} },
    { ((const char *)0x004D48F8), 0, s_aMapLastEls04b, {0, 0, 0, 0} },
    { ((const char *)0x004D48E8), 0, s_aMapLastEls01, {0, 0, 0, 0} },
    { ((const char *)0x004D48D8), 0, s_aMapLastVok24, {0, 0, 0, 0} },
    { ((const char *)0x004D48C8), s_aMapCullingVok11, 0, {0, 0, 0, 0} },
    { ((const char *)0x004D48B8), s_aMapCullingKou01, s_aMapLastKou01, {0, 0, 0, 0} },
    { ((const char *)0x004D48A8), 0, s_aMapLastKou04, {0, 0, 0, 0} },
    { ((const char *)0x004D4898), 0, s_aMapLastKou05, {0, 0, 0, 0} },
    { ((const char *)0x004D4888), 0, s_aMapLastDyu16, {0, 0, 0, 0} },
    { ((const char *)0x004D4878), 0, s_aMapLastDyu15, {0, 0, 0, 0} },
    { ((const char *)0x004D4868), 0, s_aMapLastDyu13, {0, 0, 0, 0} },
    { ((const char *)0x004D4858), 0, s_aMapLastDyu12, {0, 0, 0, 0} },
    { ((const char *)0x004D4848), 0, s_aMapLastDyu10, {0, 0, 0, 0} },
    { ((const char *)0x004D4838), 0, s_aMapLastDyu07, {0, 0, 0, 0} },
    { ((const char *)0x004D4828), 0, s_aMapLastDyu04, {0, 0, 0, 0} },
    { ((const char *)0x004D4818), 0, s_aMapLastKuk02, {0, 0, 0, 0} },
    { ((const char *)0x004D4808), 0, s_aMapLastKuk11, {0, 0, 0, 0} },
    { ((const char *)0x004D47F8), 0, s_aMapLastKuk12, {0, 0, 0, 0} },
    { ((const char *)0x004D47E8), 0, s_aMapLastKuk16, {0, 0, 0, 0} },
    { ((const char *)0x004D47D8), 0, s_aMapLastGnu06, {0, 0, 0, 0} },
    { ((const char *)0x004D47C8), 0, s_aMapLastGnu05, {0, 0, 0, 0} },
    { ((const char *)0x004D47B8), 0, s_aMapLastGnu04, {0, 0, 0, 0} },
    { ((const char *)0x004D47A8), 0, s_aMapLastGnu03, {0, 0, 0, 0} },
    { ((const char *)0x004D4798), 0, s_aMapLastGnu02, {0, 0, 0, 0} },
    { ((const char *)0x004D4788), 0, s_aMapLastGnu01, {0, 0, 0, 0} },
    { ((const char *)0x004D4778), 0, s_aMapLastVok13b, {0, 0, 0, 0} },
    { ((const char *)0x004D4768), 0, s_aMapLastVok13, {0, 0, 0, 0} },
    { ((const char *)0x004D4758), 0, s_aMapLastGnk02, {0, 0, 0, 0} },
    { ((const char *)0x004D4748), 0, s_aMapLastGnk21, {0, 0, 0, 0} },
    { ((const char *)0x004D4738), 0, s_aMapLastGnk20, {0, 0, 0, 0} },
    { ((const char *)0x004D4728), 0, s_aMapLastGnk18, {0, 0, 0, 0} },
    { ((const char *)0x004D4718), 0, s_aMapLastGnk15, {0, 0, 0, 0} },
    { ((const char *)0x004D4708), 0, s_aMapLastGnk14, {0, 0, 0, 0} },
    { ((const char *)0x004D46F8), 0, s_aMapLastGnk13, {0, 0, 0, 0} },
    { ((const char *)0x004D46E8), 0, s_aMapLastGnk12, {0, 0, 0, 0} },
    { ((const char *)0x004D46D8), 0, s_aMapLastGnk11, {0, 0, 0, 0} },
    { ((const char *)0x004D46C8), 0, s_aMapLastGnk10, {0, 0, 0, 0} },
    { ((const char *)0x004D46B8), 0, s_aMapLastGnk09, {0, 0, 0, 0} },
    { ((const char *)0x004D46A8), 0, s_aMapLastGnk04, {0, 0, 0, 0} },
    { ((const char *)0x004D4698), 0, s_aMapLastUta17, {0, 0, 0, 0} },
    { ((const char *)0x004D4688), 0, s_aMapLastUta13, {0, 0, 0, 0} },
    { ((const char *)0x004D4678), 0, s_aMapLastUta08, {0, 0, 0, 0} },
    { ((const char *)0x004D4668), 0, s_aMapLastUta07, {0, 0, 0, 0} },
    { ((const char *)0x004D4658), 0, s_aMapLastUta06, {0, 0, 0, 0} },
    { ((const char *)0x004D4648), 0, s_aMapLastUta05, {0, 0, 0, 0} },
    { ((const char *)0x004D4638), 0, s_aMapLastUta04, {0, 0, 0, 0} },
    { ((const char *)0x004D4628), 0, s_aMapLastUta03, {0, 0, 0, 0} },
    { ((const char *)0x004D4618), 0, s_aMapLastKas20, {0, 0, 0, 0} },
    { ((const char *)0x004D4608), 0, s_aMapLastKas19, {0, 0, 0, 0} },
    { ((const char *)0x004D45F8), 0, s_aMapLastKas18, {0, 0, 0, 0} },
    { ((const char *)0x004D45E8), 0, s_aMapLastKas13, {0, 0, 0, 0} },
    { ((const char *)0x004D45D8), 0, s_aMapLastKas12, {0, 0, 0, 0} },
    { ((const char *)0x004D45C8), 0, s_aMapLastKas04, {0, 0, 0, 0} },
    { ((const char *)0x004D45B8), 0, s_aMapLastKas31, {0, 0, 0, 0} },
    { ((const char *)0x004D45A8), 0, s_aMapLastUtk04, {0, 0, 0, 0} },
    { ((const char *)0x004D4598), 0, s_aMapLastUtk05, {0, 0, 0, 0} },
    { ((const char *)0x004DC4D0), 0, 0, {0, 0, 0, 0} },
};

int strcmp(const char *, const char *);

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapSet);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapCreate);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapDisp);

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
 * access. The final statement appends a bare nop after the eighth lqc2, in
 * the same asm block, as the delay-slot filler this route's pinned assembler
 * needs: cc1 leaves a leaf function's bare `j $31` delay slot to the
 * assembler, and in reorder mode the assembler fills it by moving the
 * previous instruction there unless a real one already occupies it -- and
 * among the instruction kinds this route emits, only lqc2/sqc2 get moved.
 * The original toolchain did not do this (74 of 79 comparable sites in
 * SLUS_204.69 keep `jr $31; nop`), so the nop restores the original extent
 * and instruction order
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
    __asm__ __volatile__("lqc2 vf19, 112(%0)\n\t"
                         "nop"                 :: "r"(clip) : "memory");
}

INCLUDE_ASM("asm/main/nonmatchings/face_point", _ModelCalcClipMat2);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _ModelCalcClipMat1);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _ModelCalcClip);

s32 xglStudioGetActiveCamera(void);
s32 _ModelCalcClip(s32 model);
s32 xglCullingCheck(s32 camera, s32 model);

/*
 * Tests model bounds against the active camera's combined view volume and
 * returns its clipping mask: the frustum clip mask _ModelCalcClip computes,
 * combined with xglCullingCheck's registered-volume mask for the same
 * camera/model pair.
 */
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

s32 xglStudioGetActiveCamera(void);
s32 _ModelCalcClip(s32 model);

/*
 * Tests model against the active camera's clip state directly, without
 * consulting the registered culling-map volumes xglCullingCheck reads.
 */
s32 nmlModelCalcClipNoCulling(s32 model) {
    s32 camera = xglStudioGetActiveCamera();

    if (camera != 0) {
        _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
        return _ModelCalcClip(model);
    }
    return camera;
}

/*
 * Transforms model's bounds by matrix (the same row-by-row point transform
 * as _ApplyMatrix) into a stack-local Vector4, then passes its address in
 * place of a model handle to _ModelCalcClip and xglCullingCheck: model is
 * itself the address of a per-model bounding Vector4 (dereferenced here via
 * lqc2, the same handle-as-address convention nmlModelCalcClipCam's
 * camera + 0x4F0 cast already evidences), and the transformed copy is
 * tested against the active camera's combined view volume the same way
 * nmlModelCalcClip tests the untransformed one.
 */
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

s32 xglCullingCheck(s32 camera, s32 model);

/*
 * Transforms model's bounds through matrix1 and then matrix2 (the same
 * two-pass point transform as _ApplyMatrix2Mat) into a stack-local Vector4,
 * then tests that transformed copy against the active camera's combined
 * view volume the way nmlModelCalcClip tests the untransformed one: model
 * is itself the address of a per-model bounding Vector4.
 */
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

s32 _ModelCalcClip(s32 model);

/*
 * camera + 0x4F0 is the pair of 4x4 clip matrices _ModelCalcClipInit reads
 * (see its own comment): each nmlModelCalcClip*Cam wrapper loads camera's
 * clip state, then tail-calls the matching _ModelCalcClip* worker on model.
 */
void nmlModelCalcClipCam(s32 model, s32 camera) {
    _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
    _ModelCalcClip(model);
}

s32 _ModelCalcClipMat1(s32 model, s32 matrix);

void nmlModelCalcClipMat1Cam(s32 model, s32 matrix, s32 camera) {
    _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
    _ModelCalcClipMat1(model, matrix);
}

s32 _ModelCalcClipMat2(s32 model, s32 matrix1, s32 matrix2);

void nmlModelCalcClipMat2Cam(s32 model, s32 matrix1, s32 matrix2, s32 camera) {
    _ModelCalcClipInit((const Matrix4 *) (camera + 0x4F0));
    _ModelCalcClipMat2(model, matrix1, matrix2);
}

s32 xglCullingCheck(s32 camera, s32 model);
s32 xglStudioSelectGetActiveCamera(s32 cameraIndex);

/*
 * Selects the studio camera identified by cameraIndex, then combines
 * _ModelCalcClip's frustum clip mask for model with xglCullingCheck's
 * registered-volume mask for the same camera/model pair.
 */
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

extern int g_aSubWindow[4];

/*
 * Transforms model's bounds by matrix and tests them in every enabled
 * sub-window's camera: the model is clipped away (1) only when every
 * enabled sub-window clips it, and the first sub-window that keeps it
 * answers 0. A sub-window without a selectable camera ends the sweep.
 */
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

/*
 * Transforms model's bounds through matrix1 and matrix2 and collects the
 * frustum clip mask of every enabled sub-window: each tested sub-window
 * contributes its mask shifted to its own bit, and bit 7 is added when
 * every tested sub-window reported the same single clip bit. Only the
 * first three sub-windows are swept, and a sub-window without a selectable
 * camera answers 0.
 */
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

/*
 * ACC accumulates matrix * vector row by row: row 0 times x, row 1 times y,
 * and vmaddz folds row 2 times z directly into vf31 without a separate ACC
 * step. Row 3 (the translation row) is loaded into vf30 but never used --
 * this applies only the 3x3 rotation part of matrix. vmaddz writes only
 * vf31's xyz lanes, so destination->w keeps whatever the first lqc2 loaded
 * from vector->w.
 */
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
        "sqc2 vf31, 0(%2)\n\t"
        "nop\n\t"
        :
        : "r"(vector), "r"(matrix), "r"(destination)
        : "memory"
    );
}

/*
 * Same row-by-row accumulation as _ApplyMatrix33, extended with the
 * translation row: vmaddw folds row 3 (times vf0's constant w lane, 1.0)
 * into vf31's xyz lanes, so vector is transformed as a point. Only xyz
 * lanes are ever written to vf31, so destination->w again keeps whatever
 * the first lqc2 loaded from vector->w.
 */
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
        "sqc2 vf31, 0(%2)\n\t"
        "nop\n\t"
        :
        : "r"(vector), "r"(matrix), "r"(destination)
        : "memory"
    );
}

/*
 * Applies matrix1 then matrix2 to vector in sequence (matrix2 * (matrix1 *
 * vector)) instead of pre-combining the two matrices first: each pass is
 * the same row-by-row accumulation as _ApplyMatrix, translation row
 * included (vmaddw times vf0's constant w lane, 1.0). Only xyz lanes are
 * ever written to vf31, so destination->w keeps whatever the first lqc2
 * loaded from vector->w.
 */
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
        "sqc2 vf31, 0(%3)\n\t"
        "nop\n\t"
        :
        : "r"(vector), "r"(matrix1), "r"(matrix2), "r"(destination)
        : "memory"
    );
}



static const float s_aMapCullingVok03[18] = { 0.0f, -0.20000000298023224f, 25.5f, -1.5184364318847656f, 0.0f, 0.0f, 18.899999618530273f, 11.600000381469727f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok04[27] = { -10.600000381469727f, 2.0999999046325684f, 25.5f, 0.0f, 0.0f, 0.0f, 17.899999618530273f, 12.100000381469727f, 1.0f, -10.300000190734863f, 2.4000000953674316f, 15.0f, 0.0f, 0.0f, 0.0f, 17.200000762939453f, 10.399999618530273f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok07[27] = { -18.200000762939453f, 3.0f, 22.0f, 0.0f, 0.0f, 0.0f, 7.1999998092651367f, 10.0f, 1.0f, 0.0f, 2.9000000953674316f, 21.799999237060547f, 0.0f, 0.0f, 0.0f, 6.9000000953674316f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok10[27] = { 9.3999996185302734f, 0.40000000596046448f, -9.1000003814697266f, 0.0f, 0.13962635397911072f, 0.0f, 20.600000381469727f, 10.699999809265137f, 1.0f, -26.0f, 0.0f, -9.0f, 0.0f, -0.29670599102973938f, 0.0f, 9.8000001907348633f, 9.8999996185302734f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok11[27] = { 0.0f, 0.40000000596046448f, 9.5f, -1.3613568544387817f, 0.0f, 0.0f, 2.0999999046325684f, 4.5999999046325684f, 1.0f, 0.0f, 0.0f, 6.5999999046325684f, -0.45378562808036804f, 0.0f, 0.0f, 1.0f, 2.0999999046325684f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingVok12[36] = { -2.2000000476837158f, 2.4000000953674316f, 7.1999998092651367f, 0.0f, 0.0f, 0.0f, 19.159999847412109f, 10.0f, 1.0f, 16.600000381469727f, 2.7000000476837158f, 0.20000000298023224f, 0.0f, 1.5707963705062866f, 0.0f, 11.300000190734863f, 10.0f, 1.0f, -3.7000000476837158f, 2.5999999046325684f, -8.3000001907348633f, 0.0f, 0.0f, 0.0f, 20.729999542236328f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const float s_aMapCullingKou01[18] = { 10.199999809265137f, -1.6000000238418579f, -2.5999999046325684f, 0.0f, -0.66322511434555054f, 0.0f, 5.5999999046325684f, 4.3000001907348633f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const s32 s_aMapLastVok13[12] = { 136, 127, 91, 122, 113, 104, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKou01[12] = { 214, 215, 216, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKou04[12] = { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu06[12] = { 236, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk11[12] = { 158, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu04[12] = { 274, 273, 272, 255, 252, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu12[12] = { 18, 195, 196, 74, 75, 66, 65, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu13[12] = { 22, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu15[12] = { 156, 157, 155, 158, 136, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok10[12] = { 172, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok12[12] = { 164, 165, 163, 162, 167, 138, 147, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok12b[12] = { 163, 87, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk09[12] = { 24, 23, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk04[12] = { 19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUtk05[12] = { 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

const char D_004D4598[16] = "MC_UTK05";

const char D_004D45A8[16] = "MC_UTK04";

const char D_004D45B8[16] = "MC_KAS31";

const char D_004D45C8[16] = "MC_KAS04";

const char D_004D45D8[16] = "MC_KAS12";

const char D_004D45E8[16] = "MC_KAS13";

const char D_004D45F8[16] = "MC_KAS18";

const char D_004D4608[16] = "MC_KAS19";

const char D_004D4618[16] = "MC_KAS20";

const char D_004D4628[16] = "MC_UTA03";

const char D_004D4638[16] = "MC_UTA04";

const char D_004D4648[16] = "MC_UTA05";

const char D_004D4658[16] = "MC_UTA06";

const char D_004D4668[16] = "MC_UTA07";

const char D_004D4678[16] = "MC_UTA08";

const char D_004D4688[16] = "MC_UTA13";

const char D_004D4698[16] = "MC_UTA17";

const char D_004D46A8[16] = "MC_GNK04";

const char D_004D46B8[16] = "MC_GNK09";

const char D_004D46C8[16] = "MC_GNK10";

const char D_004D46D8[16] = "MC_GNK11";

const char D_004D46E8[16] = "MC_GNK12";

const char D_004D46F8[16] = "MC_GNK13";

const char D_004D4708[16] = "MC_GNK14";

const char D_004D4718[16] = "MC_GNK15";

const char D_004D4728[16] = "MC_GNK18";

const char D_004D4738[16] = "MC_GNK20";

const char D_004D4748[16] = "MC_GNK21";

const char D_004D4758[16] = "MC_GNK02";

const char D_004D4768[16] = "MC_VOK13";

const char D_004D4778[16] = "MC_VOK13B";

const char D_004D4788[16] = "MC_GNU01";

const char D_004D4798[16] = "MC_GNU02";

const char D_004D47A8[16] = "MC_GNU03";

const char D_004D47B8[16] = "MC_GNU04";

const char D_004D47C8[16] = "MC_GNU05";

const char D_004D47D8[16] = "MC_GNU06";

const char D_004D47E8[16] = "MC_KUK16";

const char D_004D47F8[16] = "MC_KUK12";

const char D_004D4808[16] = "MC_KUK11";

const char D_004D4818[16] = "MC_KUK02";

const char D_004D4828[16] = "Mc_dyu04";

const char D_004D4838[16] = "MC_DYU07";

const char D_004D4848[16] = "MC_DYU10";

const char D_004D4858[16] = "MC_DYU12";

const char D_004D4868[16] = "MC_DYU13";

const char D_004D4878[16] = "MC_DYU15";

const char D_004D4888[16] = "MC_DYU16";

const char D_004D4898[16] = "MC_KOU05";

const char D_004D48A8[16] = "MC_KOU04";

const char D_004D48B8[16] = "MC_KOU01";

const char D_004D48C8[16] = "MC_DYU01";

const char D_004D48D8[16] = "MC_VOK24";

const char D_004D48E8[16] = "MC_ELS01";

const char D_004D48F8[16] = "MC_ELS04B";

const char D_004D4908[16] = "MC_ELS04";

const char D_004D4918[16] = "MC_ELS03";

const char D_004D4928[16] = "MC_ELS02B";

const char D_004D4938[16] = "mc_els02";

const char D_004D4948[16] = "MC_ELS09";

const char D_004D4958[16] = "MC_ELS10";

const char D_004D4968[16] = "MC_PRO02";

const char D_004D4978[16] = "MC_VOK12b";

const char D_004D4988[16] = "mc_vok12";

const char D_004D4998[16] = "MC_VOK11";

const char D_004D49A8[16] = "MC_VOK10";

const char D_004D49B8[16] = "MC_VOK07";

const char D_004D49C8[16] = "mc_vok04";

const char D_004D49D8[16] = "MC_VOK03";



static const s32 s_aMapLastVok13b[12] = { 104, 113, 122, 91, 127, 136, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKou05[12] = { 61, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu01[12] = { 50, 49, 48, 47, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu02[12] = { 26, 24, 25, 22, 21, 20, 19, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnu05[12] = { 36, 37, 42, 43, 44, 45, 46, 47, 55, 56, -1, -1 };

static const s32 s_aMapLastKuk16[12] = { 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk12[12] = { 70, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKuk02[12] = { 113, 114, 115, 116, 117, 118, 119, 93, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu07[12] = { 58, 59, 62, 63, 68, 69, 66, 67, 26, -1, -1, -1 };

static const s32 s_aMapLastDyu10[12] = { 98, 78, 17, 325, 67, 66, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastVok24[12] = { 59, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

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

static const s32 s_aMapLastGnk21[12] = { 80, 81, 64, 65, 79, 78, 77, 76, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk20[12] = { 90, 103, 104, 49, 88, 81, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk15[12] = { 76, 67, 68, 66, 65, 69, 61, 56, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk14[12] = { 36, 33, 32, 35, 34, 65, 68, 26, 27, 30, -1, -1 };

static const s32 s_aMapLastGnk13[12] = { 83, 84, 85, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk12[12] = { 70, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk11[12] = { 79, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk10[12] = { 80, 81, 82, 83, 51, 96, 97, 95, -1, -1, -1, -1 };

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

static const s32 s_aMapLastKas13[12] = { 38, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas12[12] = { 71, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas04[12] = { 125, 123, 120, 122, 121, 134, 132, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas31[12] = { 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastUtk04[12] = { 54, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };



static const float s_aMapCullingPro02[18] = { -28.899999618530273f, -0.0f, 1.0f, 0.0f, 1.5707963705062866f, 0.0f, 1.0f, 3.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static const s32 s_aMapLastGnu03[12] = { 32, 33, 51, 52, 53, 54, 155, 61, 60, 63, 116, 129 };

static const s32 s_aMapLastGnu04[12] = { 32, 34, 26, 31, 33, 48, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastDyu16[12] = { 32, 30, 62, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk02[12] = { 33, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastGnk18[12] = { 26, 30, 29, 43, 35, 28, 27, -1, -1, -1, -1, -1 };

static const s32 s_aMapLastKas18[12] = { 38, 37, 29, 30, 35, 34, 33, 31, -1, -1, -1, -1 };
