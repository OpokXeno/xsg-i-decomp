#include "common.h"
#include "shared.h"

extern int s_nIgnoreCulling;

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheckSeparateInit);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheckSeparate);

typedef int s32;

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

/*
 * s_inCulling is the culling map's own state: an active/ready byte at
 * offset 0, an evidenced count at 0xDE0 (compared and incremented by
 * xglCullingCheck/xglCullingMapSet/xglCullingMapCreate) and an evidenced
 * source value at 0xDE4 (set from a caller argument by xglCullingMapSet and
 * read back by xglCullingMapLastCheck). The bytes between 0 and 0xDE0 are
 * accessed only by the still-untranslated culling functions of this TU.
 */
typedef struct CullingMap {
    u8 active;
    u8 unmodeled_001[0xDDF];
    s32 count;
    s32 source;
} CullingMap;

extern CullingMap s_inCulling;

void xglCullingMapInit(void)
{
    s_nIgnoreCulling = 0;
    s_inCulling.count = 0;
    s_inCulling.active = 0;
    s_inCulling.source = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/face_point", check_culling_map);

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClip);

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat1);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat2);

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat1AllCam);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat2AllCam);

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
