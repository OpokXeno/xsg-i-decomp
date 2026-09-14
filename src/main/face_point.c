#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/face_point", culling_matrix);

INCLUDE_ASM("asm/main/nonmatchings/face_point", culling_cell_disp);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _FacePoint);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _CheckLine);

INCLUDE_ASM("asm/main/nonmatchings/face_point", plane_from_points);

INCLUDE_ASM("asm/main/nonmatchings/face_point", setup_occlusion);

INCLUDE_ASM("asm/main/nonmatchings/face_point", check_occlusion);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheck);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheckSeparateInit);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingCheckSeparate);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingExist);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapLastCheck);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapInit);

INCLUDE_ASM("asm/main/nonmatchings/face_point", check_culling_map);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapSet);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapCreate);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapDisp);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingIgnore);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingIgnoreOff);

INCLUDE_ASM("asm/main/nonmatchings/face_point", xglCullingMapDebug);

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

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipNoCulling);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat1);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat2);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipCam);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat1Cam);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat2Cam);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipStudio);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat1AllCam);

INCLUDE_ASM("asm/main/nonmatchings/face_point", nmlModelCalcClipMat2AllCam);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _ApplyMatrix33);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _ApplyMatrix);

INCLUDE_ASM("asm/main/nonmatchings/face_point", _ApplyMatrix2Mat);
