#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "m_math.h"

float MMathMakeRandom(void)
{
    return (float)xglSRand() * D_004D8374;
}

float MMathMakeRandom2PI(void)
{
    return (float)xglSRand() * random_two_pi_factor;
}

float MMathCalcRotNear(float first, float second)
{
    float difference = fmodf(second, near_two_pi) - fmodf(first, near_two_pi);

    if (__builtin_fabsf(difference) > near_pi) {
        if (difference < 0.0f)
            difference += near_two_pi;
        else
            difference -= near_two_pi;
    }
    return difference;
}

float MMathCalcRotFar(float first, float second)
{
    float difference = fmodf(second, far_two_pi) - fmodf(first, far_two_pi);

    if (__builtin_fabsf(difference) < far_pi) {
        if (difference < 0.0f)
            difference += far_two_pi;
        else
            difference -= far_two_pi;
    }
    return difference;
}

/*
 * MMathCalcHermite: four-component cubic Hermite/Catmull-Rom evaluation at
 * parameter t. The VU0 macro-mode block builds the four blend weights from t
 * (packed two-per-register across vf2/vf3's x/y lanes), then accumulates
 * point_start*h1 + point_end*h2 + tangent_start*h3 + tangent_end*h4 into vf1
 * and stores it; vf1.w is forced to VF00's w (1.0) by the trailing vmove.w.
 * The prototype was corrected from the stale `void` shared.h declared for it
 * (no caller anywhere reads the return value, so the change is source-visible
 * only) to the `float *` the original object's `daddu v0,a0,zero` lead-in
 * requires, matching every other pointer-returning function of this TU.
 *
 * The original fixes the parameter's COP1->GPR transfer in $10/t2 (the
 * qmtc2 immediately after reads it from there), so `scratch` is declared on
 * that hard register and used as this block's own read/write operand: a
 * genuine "=r" output at %0 receives the mfc1, and the same variable is
 * re-supplied as an "r" input at %2 for the qmtc2 that consumes it. Nothing
 * in the template names a register directly; the compiler places the mfc1
 * and qmtc2 using ordinary operand substitution, and $10 is fixed only by
 * the explicit-register C declaration, the one register value the original
 * object pins.
 */
float *MMathCalcHermite(float *destination, float parameter,
                         HermiteVector *tangent_first, HermiteVector *tangent_second,
                         HermiteVector *endpoint_first, HermiteVector *endpoint_second)
{
    register int scratch asm("$10");

    __asm__ __volatile__(
        "mfc1 %0,%1\n\t"
        "qmtc2 %2,$vf1\n\t"
        "vaddx.xyz $vf2xyz,$vf0xyz,$vf1x\n\t"
        "vmulx.xyz $vf2xyz,$vf2xyz,$vf1x\n\t"
        "vmulx.y $vf2y,$vf2y,$vf1x\n\t"
        "vaddw.xyzw $vf3xyzw,$vf0xyzw,$vf0w\n\t"
        "vaddw.xyzw $vf3xyzw,$vf3xyzw,$vf0w\n\t"
        "vmr32.xyzw $vf3xyzw,$vf3xyzw\n\t"
        "vmul.xyz $vf3xyz,$vf3xyz,$vf2xyz\n\t"
        "vsubz.y $vf5y,$vf3y,$vf3z\n\t"
        "vaddw.y $vf5y,$vf5y,$vf0w\n\t"
        "vsuby.z $vf6z,$vf3z,$vf3y\n\t"
        "vsubx.y $vf7y,$vf2y,$vf3x\n\t"
        "vaddx.y $vf7y,$vf7y,$vf1x\n\t"
        "vsubx.y $vf8y,$vf2y,$vf2x\n\t"
        "lqc2 $vf1,0(%4)\n\t"
        "lqc2 $vf2,0(%5)\n\t"
        "lqc2 $vf3,0(%6)\n\t"
        "lqc2 $vf4,0(%7)\n\t"
        "vmulay.xyz ACCxyz,$vf1xyz,$vf7y\n\t"
        "vmadday.xyz ACCxyz,$vf2xyz,$vf8y\n\t"
        "vmadday.xyz ACCxyz,$vf3xyz,$vf5y\n\t"
        "vmaddz.xyz $vf1xyz,$vf4xyz,$vf6z\n\t"
        "vmove.w $vf1w,$vf0w\n\t"
        "sqc2 $vf1,0(%3)\n\t"
        "nop"
        : "=r"(scratch)
        : "f"(parameter), "r"(scratch), "r"(destination), "r"(tangent_first),
          "r"(tangent_second), "r"(endpoint_first), "r"(endpoint_second)
        : "memory");
    return destination;
}

/*
 * MMathCalcHermitePrm: central-difference tangent pair for a Hermite/Catmull-
 * Rom segment. tangent_start = (point_end - point_prev) * 0.5, tangent_end =
 * (point_next - point_start) * 0.5. The 0.5 factor comes from vitof4 (integer
 * 8 converted with a divide-by-16 shift), not a compiler literal.
 */
void MMathCalcHermitePrm(HermiteVector *tangent_start, HermiteVector *tangent_end,
                          const HermiteVector *point_prev, const HermiteVector *point_start,
                          const HermiteVector *point_end, const HermiteVector *point_next)
{
    __asm__ __volatile__(
        "lqc2 vf1,0(%2)\n\t"
        "lqc2 vf2,0(%3)\n\t"
        "lqc2 vf3,0(%4)\n\t"
        "lqc2 vf4,0(%5)\n\t"
        "viaddi vi5,vi0,8\n\t"
        "vmfir.x vf5x,vi5\n\t"
        "vitof4.x vf5x,vf5x\n\t"
        "vsub.xyz vf6xyz,vf3xyz,vf1xyz\n\t"
        "vsub.xyz vf7xyz,vf4xyz,vf2xyz\n\t"
        "vmulx.xyz vf6xyz,vf6xyz,vf5x\n\t"
        "vmulx.xyz vf7xyz,vf7xyz,vf5x\n\t"
        "sqc2 vf6,0(%0)\n\t"
        "sqc2 vf7,0(%1)\n\t"
        "nop\n\t"
        :
        : "r"(tangent_start), "r"(tangent_end), "r"(point_prev), "r"(point_start),
          "r"(point_end), "r"(point_next)
        : "memory"
    );
}

/*
 * MMathDeg2RadVector/I and MMathRad2DegVector/I: scale the XYZ lanes of an
 * aligned four-lane vector by the degrees<->radians conversion factor, W
 * untouched: the lane-selective VU0
 * macro-mode multiply is what the C says, not an approximation of it. The
 * conversion factor is scaffold-owned .lit4 data (config/tu/main/tu219.json
 * data_ownership; declared `extern const float` in m_math.h under its
 * existing splat/symbol-map name, config/symbols/main.txt) read into a local
 * pinned to $f8 -- the original object's own scratch register for this
 * COP1->GPR->VF conduit. The destination pointer is likewise pinned to $2,
 * the register the original object returns it in (tu-worker.md, "register T
 * x asm(...)"): with both locals declared in that order the compiler emits
 * them in the original's own order -- daddu/move into $2 first, then the
 * lit4 load into $f8 -- instead of the unpinned allocator's choice of $f0
 * scheduled ahead of the return copy.
 */
void *MMathDeg2RadVector(void *destination, const Vector4 *source)
{
    register void *result asm("$2") = destination;
    register float scale asm("$f8") = degrees_to_radians;
    __asm__ __volatile__(
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,vf1\n\t"
        "lqc2 vf2,0(%1)\n\t"
        "vmulx.xyz vf2xyz,vf2xyz,vf1x\n\t"
        "sqc2 vf2,0(%0)\n\t"
        "nop\n\t"
        : "+r"(result)
        : "r"(source), "f"(scale)
        : "$8", "memory"
    );
    return result;
}

void *MMathDeg2RadVectorI(void *destination, const Vector4 *source)
{
    register void *result asm("$2") = destination;
    register float scale asm("$f8") = degrees_to_radians_integer;
    __asm__ __volatile__(
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,vf1\n\t"
        "lqc2 vf2,0(%1)\n\t"
        "vitof0.xyz vf2xyz,vf2xyz\n\t"
        "vmulx.xyz vf2xyz,vf2xyz,vf1x\n\t"
        "sqc2 vf2,0(%0)\n\t"
        "nop\n\t"
        : "+r"(result)
        : "r"(source), "f"(scale)
        : "$8", "memory"
    );
    return result;
}

void *MMathRad2DegVector(void *destination, const Vector4 *source)
{
    register void *result asm("$2") = destination;
    register float scale asm("$f8") = radians_to_degrees;
    __asm__ __volatile__(
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,vf1\n\t"
        "lqc2 vf2,0(%1)\n\t"
        "vmulx.xyz vf2xyz,vf2xyz,vf1x\n\t"
        "sqc2 vf2,0(%0)\n\t"
        "nop\n\t"
        : "+r"(result)
        : "r"(source), "f"(scale)
        : "$8", "memory"
    );
    return result;
}

void *MMathRad2DegVectorI(void *destination, const Vector4 *source)
{
    register void *result asm("$2") = destination;
    register float scale asm("$f8") = radians_to_degrees_integer;
    __asm__ __volatile__(
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,vf1\n\t"
        "lqc2 vf2,0(%1)\n\t"
        "vmulx.xyz vf2xyz,vf2xyz,vf1x\n\t"
        "vftoi0.xyz vf2xyz,vf2xyz\n\t"
        "sqc2 vf2,0(%0)\n\t"
        "nop\n\t"
        : "+r"(result)
        : "r"(source), "f"(scale)
        : "$8", "memory"
    );
    return result;
}

/*
 * MMathNormalizeVector2: normalize the XYZ lanes of an aligned four-lane
 * vector with the VU0 macro-mode reciprocal-square-root pipeline (Q
 * register), forcing W to 1.0 (vf0w) on the way out.
 */
void *MMathNormalizeVector2(void *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "vmul.xyz vf2xyz,vf1xyz,vf1xyz\n\t"
        "vaddy.x vf2x,vf2x,vf2y\n\t"
        "vaddz.x vf2x,vf2x,vf2z\n\t"
        "vrsqrt Q,vf0w,vf2x\n\t"
        "vmove.w vf2w,vf0w\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf2xyz,vf1xyz,Q\n\t"
        "sqc2 vf2,0(%0)\n\t"
        "nop\n\t"
        : "+r"(destination)
        : "r"(source)
        : "memory"
    );
    return destination;
}

/*
 * The six length/distance functions below are recovered as C with
 * constrained ee-vu-cop2 inline assembly. Their C-visible shape comes from
 * the accepted record's own asm_contract: each takes aligned vectors through a0/a1 and returns the
 * reduced length in f0.
 */

float MMathCalcLength(const Vector4 *vector)
{
    float result;
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "vmul.xyz vf1xyz,vf1xyz,vf1xyz\n\t"
        "vaddy.x vf1x,vf1x,vf1y\n\t"
        "vaddz.x vf1x,vf1x,vf1z\n\t"
        "vsqrt Q,vf1x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf1x,vf0x,Q\n\t"
        "qmfc2 $8,vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(result)
        : "r"(vector)
        : "$8", "memory"
    );
    return result;
}

float MMathCalcLengthXY(const Vector4 *vector)
{
    float result;
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "vmul.xy vf1xy,vf1xy,vf1xy\n\t"
        "vaddy.x vf1x,vf1x,vf1y\n\t"
        "vsqrt Q,vf1x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf1x,vf0x,Q\n\t"
        "qmfc2 $8,vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(result)
        : "r"(vector)
        : "$8", "memory"
    );
    return result;
}

float MMathCalcLengthXZ(const Vector4 *vector)
{
    float result;
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "vmul.xz vf1xz,vf1xz,vf1xz\n\t"
        "vaddz.x vf1x,vf1x,vf1z\n\t"
        "vsqrt Q,vf1x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf1x,vf0x,Q\n\t"
        "qmfc2 $8,vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(result)
        : "r"(vector)
        : "$8", "memory"
    );
    return result;
}

float MMathCalcLengthYZ(const Vector4 *vector)
{
    float result;
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "vmul.yz vf1yz,vf1yz,vf1yz\n\t"
        "vaddz.y vf1y,vf1y,vf1z\n\t"
        "vsqrt Q,vf1y\n\t"
        "vwaitq\n\t"
        "vaddq.x vf1x,vf0x,Q\n\t"
        "qmfc2 $8,vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(result)
        : "r"(vector)
        : "$8", "memory"
    );
    return result;
}

float MMathCalcDist(const Vector4 *first, const Vector4 *second)
{
    float result;
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "lqc2 vf2,0(%2)\n\t"
        "vsub.xyz vf1xyz,vf2xyz,vf1xyz\n\t"
        "vmul.xyz vf1xyz,vf1xyz,vf1xyz\n\t"
        "vaddy.x vf1x,vf1x,vf1y\n\t"
        "vaddz.x vf1x,vf1x,vf1z\n\t"
        "vsqrt Q,vf1x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf1x,vf0x,Q\n\t"
        "qmfc2 $8,vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(result)
        : "r"(first), "r"(second)
        : "$8", "memory"
    );
    return result;
}

float MMathCalcDistXZ(const Vector4 *first, const Vector4 *second)
{
    float result;
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "lqc2 vf2,0(%2)\n\t"
        "vsub.xz vf1xz,vf2xz,vf1xz\n\t"
        "vmul.xz vf1xz,vf1xz,vf1xz\n\t"
        "vaddz.x vf1x,vf1x,vf1z\n\t"
        "vsqrt Q,vf1x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf1x,vf0x,Q\n\t"
        "qmfc2 $8,vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(result)
        : "r"(first), "r"(second)
        : "$8", "memory"
    );
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/m_math", MMathCalcDirVector);

/* sef.c (main); no header is published for it yet. */
extern float srsAtan2(float deltaX, float deltaZ);

/*
 * The call to srsAtan2 is the last statement of this function, but the
 * original keeps a real jal for it followed by its own epilogue rather than
 * tail-jumping into srsAtan2; a value read after the call reproduces that
 * (this compiler otherwise folds a trailing call with nothing after it into
 * a sibling jump).
 */
void MMathCalcDir(const Vector4 *from, const Vector4 *to)
{
    float toX;

    srsAtan2(to->x - from->x, to->z - from->z);
    toX = to->x;
}

INCLUDE_ASM("asm/main/nonmatchings/m_math", MMathCalcAngle);

INCLUDE_ASM("asm/main/nonmatchings/m_math", MMathCalcAngleCam);

/*
 * MMathAddRotateVectorY: rotate "offset" about the Y axis by "angle" (VU0
 * macro-mode sin/cos at 0xe8/0x20) and add the result to "base", storing the
 * sum at "destination".
 */
Vector4 *MMathAddRotateVectorY(Vector4 *destination, float angle, const Vector4 *base, const Vector4 *offset)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf6\n\t"
        "vmove.x vf4, vf6\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf5, vf1\n\t"
        "vmove.x vf4, vf6\n\t"
        "vcallms 0x20\n\t"
        "vmove.x vf6, vf1\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 0(%2)\n\t"
        "vaddz.x vf4, vf0, vf3z\n\t"
        "vsubx.z vf4, vf0, vf3x\n\t"
        "vmulax.xz ACC, vf3, vf5x\n\t"
        "vmaddx.xz vf3, vf4, vf6x\n\t"
        "vadd.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%3)\n\t"
        "nop"
        : : "f"(angle), "r"(base), "r"(offset), "r"(destination) : "$8", "memory");
    return destination;
}

/*
 * MMathCalcOffset: compose a position offset from two lane-packed angles
 * (yaw in "first"'s w and "second"'s y, pitch chained through "second"'s x)
 * with VU0 macro-mode sin/cos (0xe8/0x20), scale it by "second"'s w and add
 * it to "first", storing the sum at "destination".
 */
Vector4 *MMathCalcOffset(Vector4 *destination, const Vector4 *first, const Vector4 *second)
{
    __asm__ __volatile__(
        "lqc2 vf5, 0(%0)\n\t"
        "lqc2 vf6, 0(%1)\n\t"
        "vaddy.w vf7, vf5, vf6y\n\t"
        "vaddw.x vf4, vf0, vf7w\n\t"
        "vcallms 0xE8\n\t"
        "vaddx.z vf7, vf0, vf1x\n\t"
        "vaddw.x vf4, vf0, vf7w\n\t"
        "vcallms 0x20\n\t"
        "vmove.x vf7, vf1\n\t"
        "vmove.x vf4, vf6\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y vf7, vf0, vf1x\n\t"
        "vmove.x vf4, vf6\n\t"
        "vcallms 0xE8\n\t"
        "vmulx.xz vf7, vf7, vf1x\n\t"
        "vmulaw.xyz ACC, vf7, vf6w\n\t"
        "vmaddw.xyz vf5, vf5, vf0w\n\t"
        "sqc2 vf5, 0(%2)\n\t"
        "nop"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/*
 * MMathCalcOffsetXYZ: the single-angle-chain counterpart of MMathCalcOffset
 * (angle taken from "first"'s w only), same VU0 macro-mode sin/cos shape.
 */
Vector4 *MMathCalcOffsetXYZ(Vector4 *destination, const Vector4 *first, const Vector4 *second)
{
    __asm__ __volatile__(
        "lqc2 vf5, 0(%0)\n\t"
        "lqc2 vf6, 0(%1)\n\t"
        "vaddw.x vf4, vf0, vf5w\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf7, vf1\n\t"
        "vaddw.x vf4, vf0, vf5w\n\t"
        "vcallms 0x20\n\t"
        "vaddz.x vf2, vf0, vf6z\n\t"
        "vsubx.z vf2, vf0, vf6x\n\t"
        "vmulax.xz ACC, vf6, vf7x\n\t"
        "vmaddx.xz vf6, vf2, vf1x\n\t"
        "vadd.xyz vf5, vf5, vf6\n\t"
        "sqc2 vf5, 0(%2)\n\t"
        "nop"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* MMathSubVectorMulS: destination = (first - second) * scale (xyz only). */
Vector4 *MMathSubVectorMulS(Vector4 *destination, const Vector4 *first, const Vector4 *second, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf3\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf1, vf1, vf2\n\t"
        "vmulx.xyz vf1, vf1, vf3x\n\t"
        "sqc2 vf1, 0(%3)\n\t"
        "nop"
        : : "f"(scale), "r"(first), "r"(second), "r"(destination) : "$8", "memory");
    return destination;
}

/* MMathSubVectorDivS: destination = (first - second) / scale (xyz only). */
Vector4 *MMathSubVectorDivS(Vector4 *destination, const Vector4 *first, const Vector4 *second, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf3\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "vdiv Q, vf0w, vf3x\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf1, vf1, vf2\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf1, vf1, Q\n\t"
        "sqc2 vf1, 0(%3)\n\t"
        "nop"
        : : "f"(scale), "r"(first), "r"(second), "r"(destination) : "$8", "memory");
    return destination;
}

/*
 * MMathDivVector: destination.xyz = first.xyz / second.xyz (lane by lane,
 * VU0 macro-mode divider unit); destination.w is passed through from first.
 */
Vector4 *MMathDivVector(Vector4 *destination, const Vector4 *first, const Vector4 *second)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vdiv Q, vf1x, vf2x\n\t"
        "vmove.w vf3, vf1\n\t"
        "vwaitq\n\t"
        "vaddq.x vf3, vf0, Q\n\t"
        "vdiv Q, vf1y, vf2y\n\t"
        "vwaitq\n\t"
        "vaddq.y vf3, vf0, Q\n\t"
        "vdiv Q, vf1z, vf2z\n\t"
        "vwaitq\n\t"
        "vaddq.z vf3, vf0, Q\n\t"
        "sqc2 vf3, 0(%2)\n\t"
        "nop"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* MMathDivVectorS: destination.xyz = source.xyz / scale. */
Vector4 *MMathDivVectorS(Vector4 *destination, const Vector4 *source, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf2\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "vdiv Q, vf0w, vf2x\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf1, vf1, Q\n\t"
        "sqc2 vf1, 0(%2)\n\t"
        "nop"
        : : "f"(scale), "r"(source), "r"(destination) : "$8", "memory");
    return destination;
}

/* MMathDivVector4: destination = first / second, all four lanes. */
Vector4 *MMathDivVector4(Vector4 *destination, const Vector4 *first, const Vector4 *second)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vdiv Q, vf1x, vf2x\n\t"
        "vsub.w vf3, vf3, vf3\n\t"
        "vwaitq\n\t"
        "vaddq.x vf3, vf0, Q\n\t"
        "vdiv Q, vf1y, vf2y\n\t"
        "vwaitq\n\t"
        "vaddq.y vf3, vf0, Q\n\t"
        "vdiv Q, vf1z, vf2z\n\t"
        "vwaitq\n\t"
        "vaddq.z vf3, vf0, Q\n\t"
        "vdiv Q, vf1w, vf2w\n\t"
        "vwaitq\n\t"
        "vaddq.w vf3, vf3, Q\n\t"
        "sqc2 vf3, 0(%2)\n\t"
        "nop"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* MMathDivVectorS4: destination = source / scale, all four lanes. */
Vector4 *MMathDivVectorS4(Vector4 *destination, const Vector4 *source, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf2\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "vdiv Q, vf0w, vf2x\n\t"
        "vwaitq\n\t"
        "vmulq.xyzw vf1, vf1, Q\n\t"
        "sqc2 vf1, 0(%2)\n\t"
        "nop"
        : : "f"(scale), "r"(source), "r"(destination) : "$8", "memory");
    return destination;
}

INCLUDE_ASM("asm/main/nonmatchings/m_math", MMathIsVectorEqual);

INCLUDE_ASM("asm/main/nonmatchings/m_math", MMathIsVectorEqual4);

/* MMathVectorDotProduct: xyz dot product, reduced through vf1's x lane. */
float MMathVectorDotProduct(const Vector4 *first, const Vector4 *second)
{
    float result;

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(first) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(second) : "memory");
    __asm__ __volatile__("vmul.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("vaddy.x vf1, vf1, vf1y" : : : "memory");
    __asm__ __volatile__("vaddz.x vf1, vf1, vf1z" : : : "memory");
    __asm__ __volatile__(
        "qmfc2.ni $8, vf1\n\t"
        "mtc1 $8, %0"
        : "=f"(result) :  : "$8", "memory");
    return result;
}

/* MMathVectorCrossProduct: destination.xyz = first x second, w forced to 1.0. */
Vector4 *MMathVectorCrossProduct(Vector4 *destination, const Vector4 *first, const Vector4 *second)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vopmula.xyz ACC, vf1, vf2\n\t"
        "vopmsub.xyz vf1, vf2, vf1\n\t"
        "vmove.w vf1, vf0\n\t"
        "sqc2 vf1, 0(%2)\n\t"
        "nop"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* MMathVectorInterpolation: destination = first * (1 - parameter) + second * parameter. */
Vector4 *MMathVectorInterpolation(Vector4 *destination, const Vector4 *first, const Vector4 *second, float parameter)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf3\n\t"
        "vsubx.w vf3, vf0, vf3x\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vmulaw.xyz ACC, vf1, vf3w\n\t"
        "vmaddx.xyz vf1, vf2, vf3x\n\t"
        "sqc2 vf1, 0(%3)\n\t"
        "nop"
        : : "f"(parameter), "r"(first), "r"(second), "r"(destination) : "$8", "memory");
    return destination;
}

ACCEPTED_ASM("src/main/m_math", MMathSpecialVectorSub);

Matrix4 *MMathRotateMatrixX(Matrix4 *out, const Matrix4 *matrix, float angle)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,$vf4\n\t"
        "vcallms 0xe8\n\t"
        "vmove.x $vf5x,$vf1x\n\t"
        "qmtc2 $8,$vf4\n\t"
        "vcallms 0x20\n\t"
        "vmove.x $vf6x,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "sqc2 $vf1,0(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf1xyzw,$vf3xyzw,$vf6x\n\t"
        "sqc2 $vf1,16(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf3xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf1xyzw,$vf2xyzw,$vf6x\n\t"
        "sqc2 $vf1,32(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "f"(angle) : "$8", "memory");
    return out;
}

Matrix4 *MMathRotateMatrixY(Matrix4 *out, const Matrix4 *matrix, float angle)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,$vf4\n\t"
        "vcallms 0xe8\n\t"
        "vmove.x $vf5x,$vf1x\n\t"
        "qmtc2 $8,$vf4\n\t"
        "vcallms 0x20\n\t"
        "vmove.x $vf6x,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "sqc2 $vf2,16(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf1xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf2xyzw,$vf3xyzw,$vf6x\n\t"
        "sqc2 $vf2,0(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf3xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf2xyzw,$vf1xyzw,$vf6x\n\t"
        "sqc2 $vf2,32(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "f"(angle) : "$8", "memory");
    return out;
}

Matrix4 *MMathRotateMatrixZ(Matrix4 *out, const Matrix4 *matrix, float angle)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,$vf4\n\t"
        "vcallms 0xe8\n\t"
        "vmove.x $vf5x,$vf1x\n\t"
        "qmtc2 $8,$vf4\n\t"
        "vcallms 0x20\n\t"
        "vmove.x $vf6x,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "sqc2 $vf3,32(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf1xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf3xyzw,$vf2xyzw,$vf6x\n\t"
        "sqc2 $vf3,0(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf3xyzw,$vf1xyzw,$vf6x\n\t"
        "sqc2 $vf3,16(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "f"(angle) : "$8", "memory");
    return out;
}

Matrix4 *MMathRotateMatrixXY(Matrix4 *out, const Matrix4 *matrix, const Vector4 *angles)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "lqc2 $vf7,0(%2)\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.x $vf5x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.y $vf5y,$vf0y,$vf1x\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0x20\n\t"
        "vaddx.x $vf6x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y $vf6y,$vf0y,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf7xyzw,$vf3xyzw,$vf6x\n\t"
        "vmulax.xyzw ACCxyzw,$vf3xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf3xyzw,$vf2xyzw,$vf6x\n\t"
        "vmulay.xyzw ACCxyzw,$vf1xyzw,$vf5y\n\t"
        "vmsuby.xyzw $vf8xyzw,$vf3xyzw,$vf6y\n\t"
        "vmulay.xyzw ACCxyzw,$vf3xyzw,$vf5y\n\t"
        "vmaddy.xyzw $vf3xyzw,$vf1xyzw,$vf6y\n\t"
        "sqc2 $vf8,0(%1)\n\t"
        "sqc2 $vf7,16(%1)\n\t"
        "sqc2 $vf3,32(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

Matrix4 *MMathRotateMatrixYX(Matrix4 *out, const Matrix4 *matrix, const Vector4 *angles)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "lqc2 $vf7,0(%2)\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.x $vf5x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.y $vf5y,$vf0y,$vf1x\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0x20\n\t"
        "vaddx.x $vf6x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y $vf6y,$vf0y,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "vmulay.xyzw ACCxyzw,$vf1xyzw,$vf5y\n\t"
        "vmsuby.xyzw $vf7xyzw,$vf3xyzw,$vf6y\n\t"
        "vmulay.xyzw ACCxyzw,$vf3xyzw,$vf5y\n\t"
        "vmaddy.xyzw $vf3xyzw,$vf1xyzw,$vf6y\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf8xyzw,$vf3xyzw,$vf6x\n\t"
        "vmulax.xyzw ACCxyzw,$vf3xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf3xyzw,$vf2xyzw,$vf6x\n\t"
        "sqc2 $vf7,0(%1)\n\t"
        "sqc2 $vf8,16(%1)\n\t"
        "sqc2 $vf3,32(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

Matrix4 *MMathRotateMatrixXYZ(Matrix4 *out, const Matrix4 *matrix, const Vector4 *angles)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "lqc2 $vf7,0(%2)\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.x $vf5x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.y $vf5y,$vf0y,$vf1x\n\t"
        "vaddz.x $vf4x,$vf0x,$vf7z\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.z $vf5z,$vf0z,$vf1x\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0x20\n\t"
        "vaddx.x $vf6x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y $vf6y,$vf0y,$vf1x\n\t"
        "vaddz.x $vf4x,$vf0x,$vf7z\n\t"
        "vcallms 0x20\n\t"
        "vaddx.z $vf6z,$vf0z,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf7xyzw,$vf3xyzw,$vf6x\n\t"
        "vmulax.xyzw ACCxyzw,$vf3xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf3xyzw,$vf2xyzw,$vf6x\n\t"
        "vmulay.xyzw ACCxyzw,$vf1xyzw,$vf5y\n\t"
        "vmsuby.xyzw $vf8xyzw,$vf3xyzw,$vf6y\n\t"
        "vmulay.xyzw ACCxyzw,$vf3xyzw,$vf5y\n\t"
        "vmaddy.xyzw $vf3xyzw,$vf1xyzw,$vf6y\n\t"
        "vmulaz.xyzw ACCxyzw,$vf8xyzw,$vf5z\n\t"
        "vmaddz.xyzw $vf1xyzw,$vf7xyzw,$vf6z\n\t"
        "vmulaz.xyzw ACCxyzw,$vf7xyzw,$vf5z\n\t"
        "vmsubz.xyzw $vf2xyzw,$vf8xyzw,$vf6z\n\t"
        "sqc2 $vf1,0(%1)\n\t"
        "sqc2 $vf2,16(%1)\n\t"
        "sqc2 $vf3,32(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

Matrix4 *MMathRotateMatrixYXZ(Matrix4 *out, const Matrix4 *matrix, const Vector4 *angles)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitMatrix)\n\t"
        "addiu $8,$8,%%lo(McMathUnitMatrix)\n\t"
        "movz %0,$8,%0\n\t"
        "lqc2 $vf7,0(%2)\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.x $vf5x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.y $vf5y,$vf0y,$vf1x\n\t"
        "vaddz.x $vf4x,$vf0x,$vf7z\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.z $vf5z,$vf0z,$vf1x\n\t"
        "vaddx.x $vf4x,$vf0x,$vf7x\n\t"
        "vcallms 0x20\n\t"
        "vaddx.x $vf6x,$vf0x,$vf1x\n\t"
        "vaddy.x $vf4x,$vf0x,$vf7y\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y $vf6y,$vf0y,$vf1x\n\t"
        "vaddz.x $vf4x,$vf0x,$vf7z\n\t"
        "vcallms 0x20\n\t"
        "vaddx.z $vf6z,$vf0z,$vf1x\n\t"
        "lqc2 $vf1,0(%0)\n\t"
        "lqc2 $vf2,16(%0)\n\t"
        "lqc2 $vf3,32(%0)\n\t"
        "lqc2 $vf4,48(%0)\n\t"
        "vmulay.xyzw ACCxyzw,$vf1xyzw,$vf5y\n\t"
        "vmsuby.xyzw $vf7xyzw,$vf3xyzw,$vf6y\n\t"
        "vmulay.xyzw ACCxyzw,$vf3xyzw,$vf5y\n\t"
        "vmaddy.xyzw $vf3xyzw,$vf1xyzw,$vf6y\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf5x\n\t"
        "vmaddx.xyzw $vf8xyzw,$vf3xyzw,$vf6x\n\t"
        "vmulax.xyzw ACCxyzw,$vf3xyzw,$vf5x\n\t"
        "vmsubx.xyzw $vf3xyzw,$vf2xyzw,$vf6x\n\t"
        "vmulaz.xyzw ACCxyzw,$vf7xyzw,$vf5z\n\t"
        "vmaddz.xyzw $vf1xyzw,$vf8xyzw,$vf6z\n\t"
        "vmulaz.xyzw ACCxyzw,$vf8xyzw,$vf5z\n\t"
        "vmsubz.xyzw $vf2xyzw,$vf7xyzw,$vf6z\n\t"
        "sqc2 $vf1,0(%1)\n\t"
        "sqc2 $vf2,16(%1)\n\t"
        "sqc2 $vf3,32(%1)\n\t"
        "sqc2 $vf4,48(%1)\n\t"
        "nop"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

Vector4 *MMathApplyMatrix(Vector4 *out, const Matrix4 *matrix, const Vector4 *vector)
{
    __asm__ __volatile__(
        "lqc2 $vf1,0(%2)\n\t"
        "lqc2 $vf2,0(%1)\n\t"
        "lqc2 $vf3,16(%1)\n\t"
        "lqc2 $vf4,32(%1)\n\t"
        "lqc2 $vf5,48(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf1x\n\t"
        "vmadday.xyzw ACCxyzw,$vf3xyzw,$vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw,$vf4xyzw,$vf1z\n\t"
        "vmaddw.xyzw $vf1xyzw,$vf5xyzw,$vf1w\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        "nop"
        : : "r"(out), "r"(matrix), "r"(vector) : "memory");
    return out;
}

Matrix4 *MMathMulMatrix(Matrix4 *out, const Matrix4 *left, const Matrix4 *right)
{
    __asm__ __volatile__(
        "lqc2 $vf2,0(%1)\n\t"
        "lqc2 $vf3,16(%1)\n\t"
        "lqc2 $vf4,32(%1)\n\t"
        "lqc2 $vf5,48(%1)\n\t"
        "lqc2 $vf1,0(%2)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf1x\n\t"
        "vmadday.xyzw ACCxyzw,$vf3xyzw,$vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw,$vf4xyzw,$vf1z\n\t"
        "vmaddw.xyzw $vf6xyzw,$vf5xyzw,$vf1w\n\t"
        "sqc2 $vf6,0(%0)\n\t"
        "lqc2 $vf1,16(%2)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf1x\n\t"
        "vmadday.xyzw ACCxyzw,$vf3xyzw,$vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw,$vf4xyzw,$vf1z\n\t"
        "vmaddw.xyzw $vf6xyzw,$vf5xyzw,$vf1w\n\t"
        "sqc2 $vf6,16(%0)\n\t"
        "lqc2 $vf1,32(%2)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf1x\n\t"
        "vmadday.xyzw ACCxyzw,$vf3xyzw,$vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw,$vf4xyzw,$vf1z\n\t"
        "vmaddw.xyzw $vf6xyzw,$vf5xyzw,$vf1w\n\t"
        "sqc2 $vf6,32(%0)\n\t"
        "lqc2 $vf1,48(%2)\n\t"
        "vmulax.xyzw ACCxyzw,$vf2xyzw,$vf1x\n\t"
        "vmadday.xyzw ACCxyzw,$vf3xyzw,$vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw,$vf4xyzw,$vf1z\n\t"
        "vmaddw.xyzw $vf6xyzw,$vf5xyzw,$vf1w\n\t"
        "sqc2 $vf6,48(%0)\n\t"
        "nop"
        : : "r"(out), "r"(left), "r"(right) : "memory");
    return out;
}

Matrix4 *MMathScaleMatrix(Matrix4 *out, const Matrix4 *matrix, const Vector4 *scale)
{
    __asm__ __volatile__(
        "lqc2 $vf5,0(%2)\n\t"
        "lqc2 $vf1,0(%1)\n\t"
        "lqc2 $vf2,16(%1)\n\t"
        "lqc2 $vf3,32(%1)\n\t"
        "lqc2 $vf4,48(%1)\n\t"
        "vmulx.xyzw $vf1xyzw,$vf1xyzw,$vf5x\n\t"
        "vmuly.xyzw $vf2xyzw,$vf2xyzw,$vf5y\n\t"
        "vmulz.xyzw $vf3xyzw,$vf3xyzw,$vf5z\n\t"
        "vmulw.xyzw $vf4xyzw,$vf4xyzw,$vf5w\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        "sqc2 $vf2,16(%0)\n\t"
        "sqc2 $vf3,32(%0)\n\t"
        "sqc2 $vf4,48(%0)\n\t"
        "nop"
        : : "r"(out), "r"(matrix), "r"(scale) : "memory");
    return out;
}

Matrix4 *MMathScaleMatrixS(Matrix4 *out, const Matrix4 *matrix, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,$vf5\n\t"
        "lqc2 $vf1,0(%1)\n\t"
        "lqc2 $vf2,16(%1)\n\t"
        "lqc2 $vf3,32(%1)\n\t"
        "lqc2 $vf4,48(%1)\n\t"
        "vmulx.xyzw $vf1xyzw,$vf1xyzw,$vf5x\n\t"
        "vmulx.xyzw $vf2xyzw,$vf2xyzw,$vf5x\n\t"
        "vmulx.xyzw $vf3xyzw,$vf3xyzw,$vf5x\n\t"
        "vmulx.xyzw $vf4xyzw,$vf4xyzw,$vf5x\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        "sqc2 $vf2,16(%0)\n\t"
        "sqc2 $vf3,32(%0)\n\t"
        "sqc2 $vf4,48(%0)\n\t"
        "nop"
        : : "r"(out), "r"(matrix), "f"(scale) : "$8", "memory");
    return out;
}

Matrix4 *MMathTranslateMatrix(Matrix4 *out, const Matrix4 *matrix, const Vector4 *translation)
{
    __asm__ __volatile__(
        "lqc2 $vf5,0(%2)\n\t"
        "lqc2 $vf1,0(%1)\n\t"
        "lqc2 $vf2,16(%1)\n\t"
        "lqc2 $vf3,32(%1)\n\t"
        "lqc2 $vf4,48(%1)\n\t"
        "vmulax.xyzw ACCxyzw,$vf1xyzw,$vf5x\n\t"
        "vmadday.xyzw ACCxyzw,$vf2xyzw,$vf5y\n\t"
        "vmaddaz.xyzw ACCxyzw,$vf3xyzw,$vf5z\n\t"
        "vmaddw.xyzw $vf4xyzw,$vf4xyzw,$vf0w\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        "sqc2 $vf2,16(%0)\n\t"
        "sqc2 $vf3,32(%0)\n\t"
        "sqc2 $vf4,48(%0)\n\t"
        "nop"
        : : "r"(out), "r"(matrix), "r"(translation) : "memory");
    return out;
}

/*
 * MMathCalcMatrixVector: build an orientation matrix that faces "direction",
 * starting from the identity (built in VU0 registers via vmr32 rotations of
 * the hardwired vf0 = (0,0,0,1), instead of loading McMathUnitMatrix from
 * memory) and then applying a yaw rotation (atan2 of direction.x,
 * direction.z) followed by a pitch rotation (atan2 of direction.y, the
 * vector's XZ length). When direction has no XZ component the yaw rotation
 * is skipped (undefined for a vector pointing straight up/down).
 */
Matrix4 *MMathCalcMatrixVector(Matrix4 *out, const Vector4 *direction)
{
    float directionX;

    __asm__ __volatile__("vmr32.xyzw vf1, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf2, vf1" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf3, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf0, 48(%0)" : : "r"(out) : "memory");
    __asm__ __volatile__("sqc2 vf1, 32(%0)" : : "r"(out) : "memory");
    __asm__ __volatile__("sqc2 vf2, 16(%0)" : : "r"(out) : "memory");
    __asm__ __volatile__("sqc2 vf3, 0(%0)" : : "r"(out) : "memory");

    directionX = direction->x;
    if (directionX == 0.0f && direction->z == 0.0f) {
        MMathRotateMatrixX(out, (const Matrix4 *) out, srsAtan2(direction->y, 0.0f));
    } else {
        MMathRotateMatrixY(out, (const Matrix4 *) out, srsAtan2(directionX, direction->z));
        MMathRotateMatrixX(out, (const Matrix4 *) out, srsAtan2(direction->y, MMathCalcLengthXZ(direction)));
    }
    return out;
}

/*
 * MMathCalcVectorMatrix: loads the matrix row at offset 0x20 (the third row)
 * and writes VF00 - row to the output. VF00 is the VU0 hardwired constant
 * (0, 0, 0, 1), so this negates the row's x/y/z and produces w = 1 - row.w.
 */
Vector4 *MMathCalcVectorMatrix(Vector4 *out, const Matrix4 *matrix)
{
    __asm__ __volatile__(
        "lqc2 $vf1,32(%1)\n\t"
        "vsub.xyzw $vf2xyzw,$vf0xyzw,$vf1xyzw\n\t"
        "sqc2 $vf2,0(%0)\n\t"
        "nop"
        : : "r"(out), "r"(matrix) : "memory");
    return out;
}

/*
 * MMathCalcAngleMatrix: recover the yaw/pitch pair of "matrix" from the
 * direction vector MMathCalcVectorMatrix reads out of its third row. out->z
 * and out->w come from the leading "sqc2 vf0" (VU0's hardwired (0,0,0,1))
 * and are never overwritten; out->x is yaw, out->y is pitch.
 */
Vector4 *MMathCalcAngleMatrix(Vector4 *out, const Matrix4 *matrix)
{
    Vector4 direction;

    MMathCalcVectorMatrix(&direction, matrix);
    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(out) : "memory");
    out->x = srsAtan2(direction.y, MMathCalcLengthXZ(&direction));
    out->y = srsAtan2(direction.x, direction.z);
    return out;
}

/*
 * MMathRotTransPers: transform "point" by "matrix", perspective-divide by
 * the resulting w, then scale/offset it into screen space with
 * camera->screenScale and camera->screenOffset (fields at 0x80/0x70,
 * StudioCamera) and store the fixed-point result at "destination".
 */
void MMathRotTransPers(Vector4 *destination, const StudioCamera *camera, const Matrix4 *matrix, const Vector4 *point)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 16(%1)\n\t"
        "lqc2 vf4, 32(%1)\n\t"
        "lqc2 vf5, 48(%1)\n\t"
        "vmulax.xyzw ACC, vf2, vf1x\n\t"
        "vmadday.xyzw ACC, vf3, vf1y\n\t"
        "vmaddaz.xyzw ACC, vf4, vf1z\n\t"
        "vmaddw.xyzw vf1, vf5, vf1w\n\t"
        "vdiv Q, vf0w, vf1w\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "lqc2 vf3, 0(%3)\n\t"
        "vsub.xyzw vf4, vf0, vf0\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf1, vf1, Q\n\t"
        "vmula.xyz ACC, vf1, vf2\n\t"
        "vmaddw.xyz vf4, vf3, vf0w\n\t"
        "vftoi4.xy vf1, vf4\n\t"
        "vftoi0.zw vf1, vf4\n\t"
        "sqc2 vf1, 0(%4)\n\t"
        "nop"
        : : "r"(point), "r"(matrix), "r"(&camera->screenScale), "r"(&camera->screenOffset), "r"(destination) : "memory");
}

INCLUDE_ASM("asm/main/nonmatchings/m_math", MMathRotTransPersClip);
