#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "m_math.h"
#include "main/m_math.h"
#include "main/sdv.h"
#include "vu0_macro.h"

#define MMATH_PI 3.1415927f
#define MMATH_TWO_PI 6.283185f

extern float srsAtan2(float deltaX, float deltaZ);

float MMathMakeRandom(void)
{
    return (float)xglSRand() * 3.051851e-05f;
}

float MMathMakeRandom2PI(void)
{
    return (float)xglSRand() * 0.00019175344f;
}

/* Returns the signed angle from first to second in radians, taking the shorter way round. */
float MMathCalcRotNear(float first, float second)
{
    float difference = fmodf(second, MMATH_TWO_PI) - fmodf(first, MMATH_TWO_PI);

    if (__builtin_fabsf(difference) > MMATH_PI) {
        if (difference < 0.0f)
            difference += MMATH_TWO_PI;
        else
            difference -= MMATH_TWO_PI;
    }
    return difference;
}

/* Returns the signed angle from first to second in radians, adjusted so its magnitude is at least pi. */
float MMathCalcRotFar(float first, float second)
{
    float difference = fmodf(second, MMATH_TWO_PI) - fmodf(first, MMATH_TWO_PI);

    if (__builtin_fabsf(difference) < MMATH_PI) {
        if (difference < 0.0f)
            difference += MMATH_TWO_PI;
        else
            difference -= MMATH_TWO_PI;
    }
    return difference;
}

/*
 * Evaluates a cubic Hermite curve at parameter t from the two tangents and two
 * endpoints, stores the result in destination and returns destination.
 *
 * The parameter goes to VU0 through the library's fixed scratch GPR: $10
 * here (t2), because $8 and $9 would be the fifth and sixth argument
 * registers of this six-argument function.
 */
float *MMathCalcHermite(float *destination, float parameter,
                         HermiteVector *tangent_first, HermiteVector *tangent_second,
                         HermiteVector *endpoint_first, HermiteVector *endpoint_second)
{
    __asm__ __volatile__(
        "mfc1 $10, %0\n"
        "qmtc2 $10, vf1\n"
        "vaddx.xyz vf2, vf0, vf1x\n"
        "vmulx.xyz vf2, vf2, vf1x\n"
        "vmulx.y vf2, vf2, vf1x\n"
        "vaddw.xyzw vf3, vf0, vf0w\n"
        "vaddw.xyzw vf3, vf3, vf0w\n"
        "vmr32.xyzw vf3, vf3\n"
        "vmul.xyz vf3, vf3, vf2\n"
        "vsubz.y vf5, vf3, vf3z\n"
        "vaddw.y vf5, vf5, vf0w\n"
        "vsuby.z vf6, vf3, vf3y\n"
        "vsubx.y vf7, vf2, vf3x\n"
        "vaddx.y vf7, vf7, vf1x\n"
        "vsubx.y vf8, vf2, vf2x\n"
        "lqc2 vf1, 0(%2)\n"
        "lqc2 vf2, 0(%3)\n"
        "lqc2 vf3, 0(%4)\n"
        "lqc2 vf4, 0(%5)\n"
        "vmulay.xyz ACC, vf1, vf7y\n"
        "vmadday.xyz ACC, vf2, vf8y\n"
        "vmadday.xyz ACC, vf3, vf5y\n"
        "vmaddz.xyz vf1, vf4, vf6z\n"
        "vmove.w vf1, vf0\n"
        "sqc2 vf1, 0(%1)\n"
        :
        : "f"(parameter), "r"(destination), "r"(tangent_first),
          "r"(tangent_second), "r"(endpoint_first), "r"(endpoint_second)
        : "$10", "memory");
    return destination;
}

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
        "sqc2 vf7,0(%1)"
        :
        : "r"(tangent_start), "r"(tangent_end), "r"(point_prev), "r"(point_start),
          "r"(point_end), "r"(point_next)
        : "memory"
    );
}

void *MMathDeg2RadVector(void *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "li.s $f8, 0.017453292\n"
        "mfc1 $8, $f8\n"
        "qmtc2 $8, vf1\n"
        "lqc2 vf2, 0(%1)\n"
        "vmulx.xyz vf2, vf2, vf1x\n"
        "sqc2 vf2, 0(%0)\n"
        : : "r"(destination), "r"(source) : "$8", "$f8", "memory");
    return destination;
}

void *MMathDeg2RadVectorI(void *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "li.s $f8, 0.017453292\n"
        "mfc1 $8, $f8\n"
        "qmtc2 $8, vf1\n"
        "lqc2 vf2, 0(%1)\n"
        "vitof0.xyz vf2, vf2\n"
        "vmulx.xyz vf2, vf2, vf1x\n"
        "sqc2 vf2, 0(%0)\n"
        : : "r"(destination), "r"(source) : "$8", "$f8", "memory");
    return destination;
}

void *MMathRad2DegVector(void *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "li.s $f8, 57.29578\n"
        "mfc1 $8, $f8\n"
        "qmtc2 $8, vf1\n"
        "lqc2 vf2, 0(%1)\n"
        "vmulx.xyz vf2, vf2, vf1x\n"
        "sqc2 vf2, 0(%0)\n"
        : : "r"(destination), "r"(source) : "$8", "$f8", "memory");
    return destination;
}

void *MMathRad2DegVectorI(void *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "li.s $f8, 57.29578\n"
        "mfc1 $8, $f8\n"
        "qmtc2 $8, vf1\n"
        "lqc2 vf2, 0(%1)\n"
        "vmulx.xyz vf2, vf2, vf1x\n"
        "vftoi0.xyz vf2, vf2\n"
        "sqc2 vf2, 0(%0)\n"
        : : "r"(destination), "r"(source) : "$8", "$f8", "memory");
    return destination;
}

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
        "sqc2 vf2,0(%0)"
        : "+r"(destination)
        : "r"(source)
        : "memory"
    );
    return destination;
}

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

/* Stores the normalized vector from source to target in destination and returns it. */
Vector4 *MMathCalcDirVector(Vector4 *destination, const Vector4 *source,
                            const Vector4 *target)
{
    __asm__ __volatile__(
        "lqc2 vf1,0(%1)\n\t"
        "lqc2 vf2,0(%2)\n\t"
        "vsub.xyz vf2,vf2,vf1\n\t"
        "sqc2 vf2,0(%0)"
        :
        : "r"(destination), "r"(source), "r"(target)
        : "memory");
    return MMathNormalizeVector2(destination, destination);
}

/* Calls srsAtan2 on the XZ offset from `from` to `to`; the angle is not returned. */
void MMathCalcDir(const Vector4 *from, const Vector4 *to)
{
    srsAtan2(to->x - from->x, to->z - from->z);
}

/*
 * Writes the angles from source to target into out: x from the height difference
 * and XZ length, y from the XZ offset. A null source or target uses McMathUnitVector.
 */
Vector4 *MMathCalcAngle(Vector4 *out, Vector4 *source, Vector4 *target)
{
    Vector4 difference;
    float lengthXZ = 0.0f;

    __asm__ __volatile__(
        "lui $8, %%hi(McMathUnitVector)\n"
        "addiu $8, $8, %%lo(McMathUnitVector)\n"
        "movz %1, $8, %1\n"
        "movz %2, $8, %2\n"
        "lqc2 vf1, 0(%1)\n"
        "lqc2 vf2, 0(%2)\n"
        "vsub.xyz vf3, vf2, vf1\n"
        "vmul.xz vf4, vf3, vf3\n"
        "vaddz.x vf4, vf4, vf4z\n"
        "vsqrt Q, vf4x\n"
        "sqc2 vf3, 0(%4)\n"
        "sqc2 vf0, 0(%3)\n"
        "vwaitq\n"
        "vaddq.x vf1, vf0, Q\n"
        "qmfc2 $8, vf1\n"
        "mtc1 $8, %0\n"
        : "+f"(lengthXZ), "+r"(source), "+r"(target)
        : "r"(out), "r"(&difference)
        : "$8", "memory");

    out->x = srsAtan2(difference.y, lengthXZ);
    out->y = srsAtan2(difference.x, difference.z);
    return out;
}

/* Like MMathCalcAngle, with the x and z offsets negated. */
Vector4 *MMathCalcAngleCam(Vector4 *out, Vector4 *source, Vector4 *target)
{
    Vector4 difference;
    float lengthXZ = 0.0f;

    __asm__ __volatile__(
        "lui $8, %%hi(McMathUnitVector)\n"
        "addiu $8, $8, %%lo(McMathUnitVector)\n"
        "movz %1, $8, %1\n"
        "movz %2, $8, %2\n"
        "lqc2 vf1, 0(%1)\n"
        "lqc2 vf2, 0(%2)\n"
        "vsub.xyz vf3, vf1, vf2\n"
        "vmul.xz vf4, vf3, vf3\n"
        "vaddz.x vf4, vf4, vf4z\n"
        "vsqrt Q, vf4x\n"
        "vsub.y vf3, vf2, vf1\n"
        "sqc2 vf3, 0(%4)\n"
        "sqc2 vf0, 0(%3)\n"
        "vwaitq\n"
        "vaddq.x vf1, vf0, Q\n"
        "qmfc2 $8, vf1\n"
        "mtc1 $8, %0\n"
        : "+f"(lengthXZ), "+r"(source), "+r"(target)
        : "r"(out), "r"(&difference)
        : "$8", "memory");

    out->x = srsAtan2(difference.y, lengthXZ);
    out->y = srsAtan2(difference.x, difference.z);
    return out;
}

/* Stores base plus offset rotated about the Y axis by angle into destination. */
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
        "sqc2 vf2, 0(%3)"
        : : "f"(angle), "r"(base), "r"(offset), "r"(destination) : "$8", "memory");
    return destination;
}

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
        "sqc2 vf5, 0(%2)"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

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
        "sqc2 vf5, 0(%2)"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* Stores (first - second) * scale in destination. */
Vector4 *MMathSubVectorMulS(Vector4 *destination, const Vector4 *first, const Vector4 *second, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf3\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf1, vf1, vf2\n\t"
        "vmulx.xyz vf1, vf1, vf3x\n\t"
        "sqc2 vf1, 0(%3)"
        : : "f"(scale), "r"(first), "r"(second), "r"(destination) : "$8", "memory");
    return destination;
}

/* Stores (first - second) / scale in destination. */
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
        "sqc2 vf1, 0(%3)"
        : : "f"(scale), "r"(first), "r"(second), "r"(destination) : "$8", "memory");
    return destination;
}

/* Stores the component-wise quotient first / second in destination; w is copied from first. */
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
        "sqc2 vf3, 0(%2)"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* Divides the xyz components of source by scale and stores them in destination. */
Vector4 *MMathDivVectorS(Vector4 *destination, const Vector4 *source, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf2\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "vdiv Q, vf0w, vf2x\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf1, vf1, Q\n\t"
        "sqc2 vf1, 0(%2)"
        : : "f"(scale), "r"(source), "r"(destination) : "$8", "memory");
    return destination;
}

/* Stores the component-wise quotient first / second in destination, including w. */
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
        "sqc2 vf3, 0(%2)"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* Divides all four components of source by scale and stores them in destination. */
Vector4 *MMathDivVectorS4(Vector4 *destination, const Vector4 *source, float scale)
{
    __asm__ __volatile__(
        "mfc1 $8, %0\n\t"
        "qmtc2.ni $8, vf2\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "vdiv Q, vf0w, vf2x\n\t"
        "vwaitq\n\t"
        "vmulq.xyzw vf1, vf1, Q\n\t"
        "sqc2 vf1, 0(%2)"
        : : "f"(scale), "r"(source), "r"(destination) : "$8", "memory");
    return destination;
}

/* Returns 1 when the 128-bit contents of the two vectors are identical, 0 otherwise. */
short MMathIsVectorEqual(const Vector4 *first, const Vector4 *second)
{
    short equal;

    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "lq $9,0(%2)\n\t"
        "pxor $10,$8,$9\n\t"
        "addi %0,$0,1\n\t"
        "movn %0,$0,$10\n\t"
        "pexew $10,$10\n\t"
        "movn %0,$0,$10\n\t"
        : "=&r"(equal)
        : "r"(first), "r"(second)
        : "$8", "$9", "$10");
    return equal;
}

/* Returns 1 when the 128-bit contents of the two vectors are identical, 0 otherwise. */
short MMathIsVectorEqual4(const Vector4 *first, const Vector4 *second)
{
    short equal;

    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "lq $9,0(%2)\n\t"
        "pxor $10,$8,$9\n\t"
        "addi %0,$0,1\n\t"
        "movn %0,$0,$10\n\t"
        "pcpyud $10,$10,$0\n\t"
        "movn %0,$0,$10\n\t"
        : "=&r"(equal)
        : "r"(first), "r"(second)
        : "$8", "$9", "$10");
    return equal;
}

/* Returns the dot product of the xyz components of first and second. */
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

/* Stores the cross product of the xyz components in destination; w is set to zero. */
Vector4 *MMathVectorCrossProduct(Vector4 *destination, const Vector4 *first, const Vector4 *second)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vopmula.xyz ACC, vf1, vf2\n\t"
        "vopmsub.xyz vf1, vf2, vf1\n\t"
        "vmove.w vf1, vf0\n\t"
        "sqc2 vf1, 0(%2)"
        : : "r"(first), "r"(second), "r"(destination) : "memory");
    return destination;
}

/* Stores first * (1 - parameter) + second * parameter in the xyz components of destination. */
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
        "sqc2 vf1, 0(%3)"
        : : "f"(parameter), "r"(first), "r"(second), "r"(destination) : "$8", "memory");
    return destination;
}

Vector4 *MMathSpecialVectorSub(Vector4 *destination, const Vector4 *first, const Vector4 *second, float parameter)
{
    __asm__ __volatile__(
        "lui $8,%%hi(McMathUnitVector)\n\t"
        "addiu $8,$8,%%lo(McMathUnitVector)\n\t"
        "movz %0,$8,%0\n\t"
        "li.s $f8,0.001953125\n\t"
        "mul.s $f8,$f8,%3\n\t"
        "mfc1 $8,$f8\n\t"
        "qmtc2 $8,$vf4\n\t"
        "lqc2 $vf1,0(%2)\n\t"
        "lqc2 $vf2,0(%0)\n\t"
        "vmr32.xyzw $vf3xyzw,$vf0xyzw\n\t"
        "vsub.xyz $vf1xyz,$vf3xyz,$vf1xyz\n\t"
        "vmulax.xyz ACCxyz,$vf1xyz,$vf4x\n\t"
        "vmaddw.xyz $vf1xyz,$vf2xyz,$vf0w\n\t"
        "sqc2 $vf1,0(%1)"
        : "+r"(second) : "r"(destination), "r"(first), "f"(parameter) : "$8", "$f8", "memory");
    return destination;
}

/*
 * Stores matrix multiplied by a rotation about the X axis by angle in out.
 * A null matrix uses McMathUnitMatrix.
 */
Matrix4 *MMathRotateMatrixX(Matrix4 *out, Matrix4 *matrix, float angle)
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
        "sqc2 $vf1,32(%1)"
        : "+r"(matrix) : "r"(out), "f"(angle) : "$8", "memory");
    return out;
}

/*
 * Stores matrix multiplied by a rotation about the Y axis by angle in out.
 * A null matrix uses McMathUnitMatrix.
 */
Matrix4 *MMathRotateMatrixY(Matrix4 *out, Matrix4 *matrix, float angle)
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
        "sqc2 $vf2,32(%1)"
        : "+r"(matrix) : "r"(out), "f"(angle) : "$8", "memory");
    return out;
}

/*
 * Stores matrix multiplied by a rotation about the Z axis by angle in out.
 * A null matrix uses McMathUnitMatrix.
 */
Matrix4 *MMathRotateMatrixZ(Matrix4 *out, Matrix4 *matrix, float angle)
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
        "sqc2 $vf3,16(%1)"
        : "+r"(matrix) : "r"(out), "f"(angle) : "$8", "memory");
    return out;
}

/*
 * Stores matrix multiplied by rotations about the X and Y axes, by the x and y
 * components of angles, in out.
 */
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
        "sqc2 $vf4,48(%1)"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

/*
 * Stores matrix multiplied by rotations about the Y and X axes, by the y and x
 * components of angles, in out.
 */
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
        "sqc2 $vf4,48(%1)"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

/* Stores matrix multiplied by rotations about X, Y and Z, by the components of angles, in out. */
Matrix4 *MMathRotateMatrixXYZ(Matrix4 *out, Matrix4 *matrix, const Vector4 *angles)
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
        "sqc2 $vf4,48(%1)"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

/* Stores matrix multiplied by rotations about Y, X and Z, by the components of angles, in out. */
Matrix4 *MMathRotateMatrixYXZ(Matrix4 *out, Matrix4 *matrix, const Vector4 *angles)
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
        "sqc2 $vf4,48(%1)"
        : "+r"(matrix) : "r"(out), "r"(angles) : "$8", "memory");
    return out;
}

/* Stores matrix multiplied by vector in out. */
Vector4 *MMathApplyMatrix(Vector4 *out, Matrix4 *matrix, const Vector4 *vector)
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
        "sqc2 $vf1,0(%0)"
        : : "r"(out), "r"(matrix), "r"(vector) : "memory");
    return out;
}

/* Stores left multiplied by right in out. */
Matrix4 *MMathMulMatrix(Matrix4 *out, Matrix4 *left, Matrix4 *right)
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
        "sqc2 $vf6,48(%0)"
        : : "r"(out), "r"(left), "r"(right) : "memory");
    return out;
}

/* Multiplies each column of matrix by the matching component of scale, storing the result in out. */
Matrix4 *MMathScaleMatrix(Matrix4 *out, Matrix4 *matrix, const Vector4 *scale)
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
        "sqc2 $vf4,48(%0)"
        : : "r"(out), "r"(matrix), "r"(scale) : "memory");
    return out;
}

/* Multiplies every element of matrix by scale, storing the result in out. */
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
        "sqc2 $vf4,48(%0)"
        : : "r"(out), "r"(matrix), "f"(scale) : "$8", "memory");
    return out;
}

/* Stores matrix multiplied by the translation matrix for translation in out. */
Matrix4 *MMathTranslateMatrix(Matrix4 *out, Matrix4 *matrix, const Vector4 *translation)
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
        "sqc2 $vf4,48(%0)"
        : : "r"(out), "r"(matrix), "r"(translation) : "memory");
    return out;
}

/* Sets out to the identity rotated by the yaw (atan2 of x and z) and pitch (atan2 of y and XZ length) of direction. */
Matrix4 *MMathCalcMatrixVector(Matrix4 *out, const Vector4 *direction)
{
    float directionX;

    Vu0UnitMatrix(out);

    directionX = direction->x;
    if (directionX == 0.0f && direction->z == 0.0f) {
        MMathRotateMatrixX(out, out, srsAtan2(direction->y, 0.0f));
    } else {
        MMathRotateMatrixY(out, out, srsAtan2(directionX, direction->z));
        MMathRotateMatrixX(out, out, srsAtan2(direction->y, MMathCalcLengthXZ(direction)));
    }
    return out;
}

/* Stores the negation of the third column of matrix in out. */
Vector4 *MMathCalcVectorMatrix(Vector4 *out, const Matrix4 *matrix)
{
    __asm__ __volatile__(
        "lqc2 $vf1,32(%1)\n\t"
        "vsub.xyzw $vf2xyzw,$vf0xyzw,$vf1xyzw\n\t"
        "sqc2 $vf2,0(%0)"
        : : "r"(out), "r"(matrix) : "memory");
    return out;
}

/* Writes the x and y angles of the direction stored in the third column of matrix, negated, into out. */
Vector4 *MMathCalcAngleMatrix(Vector4 *out, const Matrix4 *matrix)
{
    Vector4 direction;

    MMathCalcVectorMatrix(&direction, matrix);
    Vu0SetOrigin(out);
    out->x = srsAtan2(direction.y, MMathCalcLengthXZ(&direction));
    out->y = srsAtan2(direction.x, direction.z);
    return out;
}

/* Transforms point by matrix, divides by w and stores the screen coordinates in destination. */
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
        "sqc2 vf1, 0(%4)"
        : : "r"(point), "r"(matrix), "r"(&camera->screenScale), "r"(&camera->screenOffset), "r"(destination) : "memory");
}

/* Like MMathRotTransPers, but clips the point; returns 0x8000 when the VU0 clip flags are set, 0 otherwise. */
int MMathRotTransPersClip(Vector4 *destination, const StudioCamera *camera, const Matrix4 *matrix, const Vector4 *point)
{
    int result;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%1)\n"
        "lqc2 vf2, 0(%2)\n"
        "lqc2 vf3, 16(%2)\n"
        "lqc2 vf4, 32(%2)\n"
        "lqc2 vf5, 48(%2)\n"
        "vmulax.xyzw ACC, vf2, vf1x\n"
        "vmadday.xyzw ACC, vf3, vf1y\n"
        "vmaddaz.xyzw ACC, vf4, vf1z\n"
        "vmaddw.xyzw vf1, vf5, vf1w\n"
        "vdiv Q, vf0w, vf1w\n"
        "ctc2 $0, $vi18\n"
        "lqc2 vf2, 0(%3)\n"
        "lqc2 vf3, 0(%4)\n"
        "li $9, 0x8000\n"
        "vwaitq\n"
        "vmulq.xyz vf4, vf1, Q\n"
        "vclipw.xyz vf4, vf0w\n"
        "vmula.xyz ACC, vf4, vf2\n"
        "vmaddw.xyz vf5, vf3, vf0w\n"
        "vftoi4.xy vf1, vf5\n"
        "vftoi0.z vf1, vf5\n"
        "cfc2 %0, $vi18\n"
        "nop\n"
        "movn %0, $9, %0\n"
        "qmtc2 %0, vf2\n"
        "vmr32.w vf1, vf2\n"
        "sqc2 vf1, 0(%5)\n"
        : "=r"(result)
        : "r"(point), "r"(matrix), "r"(&camera->screenScale), "r"(&camera->screenOffset), "r"(destination)
        : "$9", "memory");
    return result;
}
