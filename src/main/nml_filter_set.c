#include "common.h"
#include "shared.h"

void nmlModelSetFilterGunosys(int *model, int flags, int count);
void nmlModelSetFilterStealth(int *model, int flags);

/*
 * Loads the supplied four-row matrix into the resident VU0 macro-mode
 * registers vf27-vf30 (ee-vu-cop2), the same resident-register family
 * _CurRotTransPersClip below reads back, matching src/main/nml_model_set.c's
 * own _CurSetMatrix.
 */
static void _CurSetMatrix(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf27,0(%0)\n\t"
        "lqc2 $vf28,16(%0)\n\t"
        "lqc2 $vf29,32(%0)\n\t"
        "lqc2 $vf30,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

/*
 * Loads the view-scale vector into VU register vf25 and the view-translation
 * vector into vf26, matching src/main/nml_model_set.c's own
 * _CurSetViewScaleTrans.
 */
static void _CurSetViewScaleTrans(const float viewScale[4], const float viewTrans[4])
{
    __asm__ __volatile__(
        "lqc2 $vf25,0(%0)\n\t"
        "lqc2 $vf26,0(%1)\n\t"
        "nop"
        :
        : "r"(viewScale), "r"(viewTrans)
        : "memory"
    );
}

/*
 * Transforms *vector by the resident matrix in vf27-vf30, perspective-divides
 * it, applies the resident view scale (vf25) and translation (vf26) and stores
 * the fixed-point result at *destination (ee-vu-cop2).  The return value is the
 * VU0 clipping-flag register vi18, cleared on entry and accumulated by the
 * vclipw test, masked to its six near/far X/Y/Z bits.
 *
 * The original object masks the flags in $2 and then hands them to the caller
 * through $4 (daddu a0,v0,zero / jr ra / daddu v0,a0,zero), so the returned
 * value is a second local pinned to that register; the instruction-free barrier
 * below keeps both copies alive across the hand-off, which is the only thing
 * that separates them.
 */
static int _CurRotTransPersClip(Vector4 *destination, const Vector4 *vector)
{
    register int clipFlags asm("$2");
    register int returnedFlags asm("$4");

    __asm__ __volatile__(
        "ctc2 $0,$vi18\n\t"
        "lqc2 $vf31,0(%2)\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf31x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf31y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf31z\n\t"
        "vmaddw.xyzw vf31xyzw,vf30xyzw,vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vclipw.xyz vf31xyz,vf31w\n\t"
        "vdiv Q,vf0w,vf31w\n\t"
        "vwaitq\n\t"
        "vmulq.xyzw vf31xyzw,vf31xyzw,Q\n\t"
        "vmulaw.xyzw ACC,vf26xyzw,vf0w\n\t"
        "vmadd.xyzw vf31xyzw,vf31xyzw,vf25xyzw\n\t"
        "vftoi4.xyw vf23xyw,vf31xyw\n\t"
        "vftoi0.z vf23z,vf31z\n\t"
        "vsub.w vf23w,vf23w,vf23w\n\t"
        "sqc2 $vf23,0(%1)\n\t"
        "cfc2 %0,$vi18\n\t"
        "nop"
        : "=r"(clipFlags)
        : "r"(destination), "r"(vector)
        : "memory"
    );
    clipFlags &= 0x3f;
    returnedFlags = clipFlags;
    __asm__ __volatile__("" : : "r"(returnedFlags), "r"(clipFlags));
    return returnedFlags;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlModelSetFilterGunosys);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlModelSetFilterStealth);

/*
 * Loads the supplied four-row placement matrix into the resident VU0
 * macro-mode registers vf10-vf13, the second resident matrix family
 * _WeightToGlobalPlaceVec below combines with vf27-30 (_CurSetMatrix).
 */
static void _WeightToGlobalPlaceInit(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf10,0(%0)\n\t"
        "lqc2 $vf11,16(%0)\n\t"
        "lqc2 $vf12,32(%0)\n\t"
        "lqc2 $vf13,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", _WeightToGlobalPlaceVec);

/*
 * Squared distance between *first and *second: (second - first) dot
 * (second - first), returned as an ordinary float (the "=r" qmfc2 output
 * copied into $f0 by cc1, the same shape as src/main/xgl_2.c's
 * xglPointLength family).
 */
static float _VectorLengthSQ(const Vector4 *first, const Vector4 *second)
{
    register float lengthSq asm("$f0");

    __asm__ __volatile__(
        "lqc2 $vf10,0(%1)\n\t"
        "lqc2 $vf11,0(%2)\n\t"
        "vsub.xyz vf10xyz,vf11xyz,vf10xyz\n\t"
        "vmul.xyz vf10xyz,vf10xyz,vf10xyz\n\t"
        "vaddz.x vf10x,vf10x,vf10z\n\t"
        "vaddy.x vf10x,vf10x,vf10y\n\t"
        "qmfc2 $2,$vf10\n\t"
        "mtc1 $2,$f2\n\t"
        "mtc1 $2,%0"
        : "=f"(lengthSq)
        : "r"(first), "r"(second)
        : "$2", "memory"
    );
    return lengthSq;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterStealthMake);

void nmlFilterSetPacket(int *model, int count)
{
    int flags;
    int i;

    flags = model[0x234 / 4];
    for (i = 0; i < 31; i++) {
        switch (flags & (1 << i)) {
        case 1:
            flags &= ~1;
            nmlModelSetFilterGunosys(model, flags, count);
            break;
        case 2:
            flags &= ~2;
            nmlModelSetFilterStealth(model, flags);
            break;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetFrameToBuffer);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetBufferToFrame);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetBufferRender);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetFrameAlphaClear);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetTexClear);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterBackClear);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetFlatRender);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetTexFillDraw);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetVolumeCubeRender);
