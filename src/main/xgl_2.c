#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "xgl_2.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglAtan2);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy64);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy64b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy16);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy16b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy8);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy8b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy4);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy4b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy2);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy2b);

void xglVectorDiv(Vector4 *destination, const Vector4 *numerator,
                  const Vector4 *divisor)
{
    destination->x = numerator->x / divisor->x;
    destination->y = numerator->y / divisor->y;
    destination->z = numerator->z / divisor->z;
    destination->w = numerator->w / divisor->w;
}

void xglVectorDivXYZ(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor)
{
    destination->x = numerator->x / divisor->x;
    destination->y = numerator->y / divisor->y;
    destination->z = numerator->z / divisor->z;
    destination->w = numerator->w;
}

void xglVectorMulAdd(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *addend)
{
    destination->x = left->x * right->x + addend->x;
    destination->y = left->y * right->y + addend->y;
    destination->z = left->z * right->z + addend->z;
    destination->w = left->w * right->w + addend->w;
}

void xglVectorMulAddXYZ(Vector4 *destination, const Vector4 *left,
                        const Vector4 *right, const Vector4 *addend)
{
    destination->x = left->x * right->x + addend->x;
    destination->y = left->y * right->y + addend->y;
    destination->z = left->z * right->z + addend->z;
    destination->w = left->w;
}

void xglVectorMulSub(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *subtrahend)
{
    destination->x = left->x * right->x - subtrahend->x;
    destination->y = left->y * right->y - subtrahend->y;
    destination->z = left->z * right->z - subtrahend->z;
    destination->w = left->w * right->w - subtrahend->w;
}

void xglVectorMulSubXYZ(Vector4 *destination, const Vector4 *left,
                        const Vector4 *right, const Vector4 *subtrahend)
{
    destination->x = left->x * right->x - subtrahend->x;
    destination->y = left->y * right->y - subtrahend->y;
    destination->z = left->z * right->z - subtrahend->z;
    destination->w = left->w;
}

void xglVectorDivAdd(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor, const Vector4 *addend)
{
    destination->x = numerator->x / divisor->x + addend->x;
    destination->y = numerator->y / divisor->y + addend->y;
    destination->z = numerator->z / divisor->z + addend->z;
    destination->w = numerator->w / divisor->w + addend->w;
}

void xglVectorDivAddXYZ(Vector4 *destination, const Vector4 *numerator,
                        const Vector4 *divisor, const Vector4 *addend)
{
    destination->x = numerator->x / divisor->x + addend->x;
    destination->y = numerator->y / divisor->y + addend->y;
    destination->z = numerator->z / divisor->z + addend->z;
    destination->w = numerator->w;
}

void xglVectorDivSub(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor, const Vector4 *subtrahend)
{
    destination->x = numerator->x / divisor->x - subtrahend->x;
    destination->y = numerator->y / divisor->y - subtrahend->y;
    destination->z = numerator->z / divisor->z - subtrahend->z;
    destination->w = numerator->w / divisor->w - subtrahend->w;
}

void xglVectorDivSubXYZ(Vector4 *destination, const Vector4 *numerator,
                        const Vector4 *divisor, const Vector4 *subtrahend)
{
    destination->x = numerator->x / divisor->x - subtrahend->x;
    destination->y = numerator->y / divisor->y - subtrahend->y;
    destination->z = numerator->z / divisor->z - subtrahend->z;
    destination->w = numerator->w;
}

void xglVectorScale(float scale, Vector4 *destination, const Vector4 *source)
{
    destination->x = source->x * scale;
    destination->y = source->y * scale;
    destination->z = source->z * scale;
    destination->w = source->w * scale;
}

void xglVectorScaleXYZ(float scale, Vector4 *destination, const Vector4 *source)
{
    destination->x = source->x * scale;
    destination->y = source->y * scale;
    destination->z = source->z * scale;
    destination->w = source->w;
}

void xglVectorScaleAdd(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale + other->x;
    destination->y = source->y * scale + other->y;
    destination->z = source->z * scale + other->z;
    destination->w = source->w * scale + other->w;
}

void xglVectorScaleAddXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale + other->x;
    destination->y = source->y * scale + other->y;
    destination->z = source->z * scale + other->z;
    destination->w = source->w;
}

void xglVectorScaleSub(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale - other->x;
    destination->y = source->y * scale - other->y;
    destination->z = source->z * scale - other->z;
    destination->w = source->w * scale - other->w;
}

void xglVectorScaleSubXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale - other->x;
    destination->y = source->y * scale - other->y;
    destination->z = source->z * scale - other->z;
    destination->w = source->w;
}

void xglVectorInter(float fraction, Vector4 *destination,
                    const Vector4 *start, const Vector4 *end)
{
    destination->x = (end->x - start->x) * fraction + start->x;
    destination->y = (end->y - start->y) * fraction + start->y;
    destination->z = (end->z - start->z) * fraction + start->z;
    destination->w = (end->w - start->w) * fraction + start->w;
}

void xglVectorInterXYZ(float fraction, Vector4 *destination,
                       const Vector4 *start, const Vector4 *end)
{
    destination->x = (end->x - start->x) * fraction + start->x;
    destination->y = (end->y - start->y) * fraction + start->y;
    destination->z = (end->z - start->z) * fraction + start->z;
    destination->w = end->w;
}

void xglVectorInner(float *result, const Vector4 *left, const Vector4 *right)
{
    float dot = left->x * right->x;
    *result = dot;
    dot += left->y * right->y;
    *result = dot;
    dot += left->z * right->z;
    *result = dot;
}

float xglVectorInner4(const Vector4 *left, const Vector4 *right)
{
    return left->x * right->x + left->y * right->y
         + left->z * right->z + left->w * right->w;
}

void xglVectorOuter(Vector4 *destination, const Vector4 *left,
                    const Vector4 *right)
{
    destination->x = left->y * right->z - left->z * right->y;
    destination->y = left->z * right->x - left->x * right->z;
    destination->z = left->x * right->y - left->y * right->x;
    destination->w = left->w;
}

/*
 * xgl_lengths. All three routines are leaf VU0 vsqrt
 * reductions with no scalar control flow: vmul.xyz squares the (already
 * vsub.xyz-subtracted, for the two distance routines) xyz lanes, vaddy.x/
 * vaddz.x reduce them to one scalar (xglPointLengthXZ omits the Y term,
 * matching its name), vsqrt/vwaitq compute the square root into Q, and
 * vaddq.x/qmfc2 add the architectural VF0.x (zero) and transfer the result
 * to a GPR. Only the VU0 reduction is inline assembly.
 *
 * xglPointLength/xglPointLengthXZ return that value as an ordinary float:
 * cc1 emits the mtc1 that copies the "=r" GPR result into $f0 itself
 * (CP-0161), never a hand-written mfc1/mtc1.
 *
 * xglVectorLength instead writes the value through its destination pointer
 * and returns void, which the original does with a plain GPR store (sw), not
 * a COP1 store. Two measured properties of the compiler drive the C shape:
 *
 * - The "=r" output is a `register float length asm("$2")`, not a plain
 *   unpinned `float` local. An unpinned local either lands in a different
 *   GPR (a1, when the store is written as a raw GPR pointer cast) or, kept
 *   as `float` and stored through a plain `float *`, gets copied into $f0
 *   with an extra register move before the store becomes a COP1 `s.s`
 *   (`swc1`) instead of the original's GPR `sw` -- both measured non-exact.
 *   Pinning the output to $2 reproduces the original's own choice of v0 and
 *   keeps the value a GPR value, so the store the compiler picks for it is
 *   the plain integer one.
 * - The store goes through `volatile float *out = destination`, not a plain
 *   `*destination`. A plain store here is a legal, side-effect-free
 *   candidate for this compiler's own post-reload delay-slot-fill pass
 *   (dbr_schedule), which moves it into the return's `jr` delay slot --
 *   4 bytes short of the original's literal `sw; jr; nop` order (measured).
 *   The `volatile` qualifier is ordinary C, not inline assembly: it keeps
 *   the store from being scheduled into that delay slot, without adding any
 *   instruction or any second asm statement of its own. It is carried by a
 *   local pointer rather than a cast at the store, which is the same
 *   qualifier conversion and keeps the emitted code identical (measured).
 */
void xglVectorLength(float *destination, const Vector4 *vector)
{
    register float length asm("$2");
    volatile float *out = destination;

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "vmul.xyz vf3xyz, vf3xyz, vf3xyz\n\t"
        "vaddy.x vf3x, vf3x, vf3y\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vsqrt Q, vf3x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf4x, vf0x, Q\n\t"
        "qmfc2 %0, vf4"
        : "=r"(length)
        : "r"(vector)
        : "memory"
    );
    *out = length;
}

float xglPointLength(const Vector4 *point1, const Vector4 *point2)
{
    float length;

    __asm__ __volatile__(
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "vmul.xyz vf3xyz, vf2xyz, vf2xyz\n\t"
        "vaddy.x vf3x, vf3x, vf3y\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vsqrt Q, vf3x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf4x, vf0x, Q\n\t"
        "qmfc2 %0, vf4"
        : "=r"(length)
        : "r"(point1), "r"(point2)
        : "memory"
    );
    return length;
}

float xglPointLengthXZ(const Vector4 *point1, const Vector4 *point2)
{
    float length;

    __asm__ __volatile__(
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "vmul.xyz vf3xyz, vf2xyz, vf2xyz\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vsqrt Q, vf3x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf4x, vf0x, Q\n\t"
        "qmfc2 %0, vf4"
        : "=r"(length)
        : "r"(point1), "r"(point2)
        : "memory"
    );
    return length;
}

/*
 * XYZ is normalized in place; the loaded W lane is retained through the
 * xyz-only vmulq scale and the full sqc2 store (see include/shared.h's
 * Vector4 comment).
 */
void xglVectorNormal(Vector4 *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "lqc2 vf2, 0(%1)\n\t"
        "vmul.xyz vf3xyz, vf2xyz, vf2xyz\n\t"
        "vaddy.x vf3x, vf3x, vf3y\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vrsqrt Q, vf0w, vf3x\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf2xyz, vf2xyz, Q\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        "nop\n\t"
        :
        : "r"(destination), "r"(source)
        : "memory"
    );
}

void xglVectorClamp(Vector4 *destination, const Vector4 *source,
                    float minimum, float maximum)
{
    destination->x = source->x < minimum ? minimum
                    : maximum < source->x ? maximum : source->x;
    destination->y = source->y < minimum ? minimum
                    : maximum < source->y ? maximum : source->y;
    destination->z = source->z < minimum ? minimum
                    : maximum < source->z ? maximum : source->z;
    destination->w = source->w < minimum ? minimum
                    : maximum < source->w ? maximum : source->w;
}

void xglVectorClampXYZ(Vector4 *destination, const Vector4 *source,
                       float minimum, float maximum)
{
    destination->x = source->x < minimum ? minimum
                    : maximum < source->x ? maximum : source->x;
    destination->y = source->y < minimum ? minimum
                    : maximum < source->y ? maximum : source->y;
    destination->z = source->z < minimum ? minimum
                    : maximum < source->z ? maximum : source->z;
    destination->w = source->w;
}

/*
 * The plane's normal is the (unnormalized) cross product of the two edges
 * p1-p0 and p2-p1, normalized in place, and the plane's w is the negated
 * distance of p0 along that normal (Ax+By+Cz+D=0 with D = -normal.p0).
 *
 * edge0/edge1 keep the w lane of their vsub.xyz right-hand operand (p1, p2
 * respectively): only xyz is subtracted, w is storage (see include/shared.h's
 * Vector4 comment).
 */
void xglPlaneParameter(Vector4 *destination, const Vector4 *p0,
                       const Vector4 *p1, const Vector4 *p2)
{
    Vector4 edge0;
    Vector4 edge1;

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&edge0), "r"(p0), "r"(p1)
        : "memory"
    );

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&edge1), "r"(p1), "r"(p2)
        : "memory"
    );

    xglVectorOuter(destination, &edge0, &edge1);
    xglVectorNormal(destination, destination);
    xglVectorInner(&destination->w, destination, p0);
    destination->w = -destination->w;
}

/*
 * ACC accumulates the transformed vector one row at a time: row 0 times X,
 * then rows 1-3 times Y, Z and W (the last vmaddw includes the source's own
 * W lane instead of retaining it, unlike the xyz-only helpers above).
 */
void xglVectorMulMat(Vector4 *destination, const Matrix4 matrix,
                     const Vector4 *vector)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%2)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf1x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf1z\n\t"
        "vmaddw.xyzw vf31xyzw, vf30xyzw, vf1w\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        "nop\n\t"
        :
        : "r"(destination), "r"(matrix), "r"(vector)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglRotTransPers);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglRotTransPersN);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixUnit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixUnit4s);

/*
 * Each destination row is right's row transformed by left: ACC accumulates
 * right-row.x * left-row0 + .y * left-row1 + .z * left-row2 + .w * left-row3,
 * one right row per vmulax/vmadday/vmaddaz/vmaddw chain.
 */
void xglMatrixMul(Matrix *destination, Matrix *left, Matrix *right)
{
    __asm__ __volatile__(
        "lqc2 vf2, 0(%2)\n\t"
        "lqc2 vf3, 16(%2)\n\t"
        "lqc2 vf4, 32(%2)\n\t"
        "lqc2 vf5, 48(%2)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf30xyzw, vf2w\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf3x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf3y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf3z\n\t"
        "vmaddw.xyzw vf3xyzw, vf30xyzw, vf3w\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf4x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf4y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf4z\n\t"
        "vmaddw.xyzw vf4xyzw, vf30xyzw, vf4w\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf5x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf5y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf5z\n\t"
        "vmaddw.xyzw vf30xyzw, vf30xyzw, vf5w\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        "sqc2 vf3, 16(%0)\n\t"
        "sqc2 vf4, 32(%0)\n\t"
        "sqc2 vf30, 48(%0)\n\t"
        "nop\n\t"
        :
        : "r"(destination), "r"(left), "r"(right)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixReverse);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixInverse);

void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                    const float scale[4])
{
    int column;

    for (column = 0; column < 4; ++column) {
        destination[0][column] = source[0][column] * scale[0];
        destination[1][column] = source[1][column] * scale[1];
        destination[2][column] = source[2][column] * scale[2];
        destination[3][column] = source[3][column] * scale[3];
    }
}

void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                    const float translation[4])
{
    int column;

    for (column = 0; column < 4; ++column) {
        destination[0][column] = source[0][column];
        destination[1][column] = source[1][column];
        destination[2][column] = source[2][column];
        destination[3][column] = source[0][column] * translation[0]
                                + source[1][column] * translation[1]
                                + source[2][column] * translation[2]
                                + source[3][column] * translation[3];
    }
}

extern char Vu0CallSin[];
extern char Vu0CallCos[];

/*
 * xglMatrixRotV (ee-vu-cop2, re-treatment of the accepted
 * src/main/xgl_2/xglMatrixRotV.s -- config/units/math-spark-cont01-00229f30.json):
 * builds a row-major 4x4 rotation matrix for `angle` radians about `axis`
 * (Rodrigues' rotation formula: rotation = cos*I + sin*[axis]x +
 * (1-cos)*axis(x)axis) and multiplies `source` by it into `destination`.
 *
 * The sine and cosine of `angle` come from the resident VU0 macro-mode
 * trigonometric microprogram (Vu0CallSin/Vu0CallCos, dispatched through
 * vi27/CMSAR0 with vcallmsr, the same hardware sequence as xglMatrixRotX/Y/Z
 * above). The original fetches cosine four times and sine once rather than
 * caching one result across the whole function -- each `cosineFor*` local
 * below stands for one of those independent dispatches -- so this source
 * asks for it again each time it is needed and lets the compiler place it.
 * Only the VU0 dispatch is inline assembly; the two scaled-axis helper calls
 * and the Rodrigues arithmetic that builds `rotation` are ordinary C.
 *
 * `axis`'s x/y/z components and the two scaled-axis vectors' components are
 * read into scalar locals (`ax`/`ay`/`az`, `oneMinusCos*`, `sin*`) once, right
 * before their row's arithmetic, rather than re-read through the pointers at
 * each use: every value here is read again after an intervening VU0-dispatch
 * block, and the original keeps each in a register across that dispatch
 * instead of reloading it (docs/tu-worker.md, "The delay slot of a recovered
 * function" documents the analogous asm-scheduling sensitivity of this
 * hardware sequence). `ay` is read out of its natural x/y/z order, right
 * before it is first used by row 1, because it is not needed until then and
 * the original's own register schedule places its load there.
 *
 */
void xglMatrixRotV(Matrix *destination, Matrix *source, Vector4 *axis,
                   float angle)
{
    Vector4 oneMinusCosAxis;
    Vector4 sinAxis;
    Matrix rotation;
    float cosineForScale;
    float sine;
    float cosineForRow0;
    float cosineForRow1;
    float cosineForRow2;
    float ax;
    float ay;
    float az;
    float oneMinusCosX;
    float oneMinusCosY;
    float oneMinusCosZ;
    float sinX;
    float sinY;
    float sinZ;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForScale)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    xglVectorScaleXYZ(1.0f - cosineForScale, &oneMinusCosAxis, axis);

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    xglVectorScaleXYZ(sine, &sinAxis, axis);

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForRow0)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    ax = axis->x;
    oneMinusCosX = oneMinusCosAxis.x;
    oneMinusCosY = oneMinusCosAxis.y;
    oneMinusCosZ = oneMinusCosAxis.z;
    ay = axis->y;
    sinY = sinAxis.y;
    sinZ = sinAxis.z;
    rotation.elements[0] = oneMinusCosX * ax + cosineForRow0;
    rotation.elements[1] = oneMinusCosY * ax + sinZ;
    rotation.elements[2] = oneMinusCosZ * ax - sinY;
    rotation.elements[3] = 0.0f;
    rotation.elements[4] = oneMinusCosX * ay - sinZ;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForRow1)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    sinX = sinAxis.x;
    az = axis->z;
    rotation.elements[5] = oneMinusCosY * ay + cosineForRow1;
    rotation.elements[6] = oneMinusCosZ * ay + sinX;
    rotation.elements[7] = 0.0f;
    rotation.elements[8] = oneMinusCosX * az + sinY;
    rotation.elements[9] = oneMinusCosY * az - sinX;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForRow2)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    rotation.elements[10] = oneMinusCosZ * az + cosineForRow2;
    rotation.elements[11] = 0.0f;
    rotation.elements[12] = 0.0f;
    rotation.elements[13] = 0.0f;
    rotation.elements[14] = 0.0f;
    rotation.elements[15] = 1.0f;

    xglMatrixMul(destination, source, &rotation);
}

/*
 * xglMatrixRotX/Y/Z (main/tu100, re-treatment of the accepted
 * src/main/xgl_2/rots.s -- config/units/math-spark-cont01-0022a0f8-rots.json,
 * src/math/main/spark-cont01-0022a0f8-rots/notes.md is the pinned semantic
 * evidence this reconstruction preserves) toward readable C with constrained
 * inline assembly.
 *
 * Each helper rotates `source` about one coordinate axis by `angle` radians
 * and writes the result to `destination`. `angle` is sent through vf4 to the
 * resident VU0 macro-mode trigonometric microprogram twice: the returned sine
 * and cosine are kept in a 16-byte stack pair (cosSin[0] = cos, cosSin[1] =
 * sin), loaded back as one quadword into vf1, and combined with the two matrix
 * rows the axis affects through COP2 ACC multiply/accumulate operations, while
 * the unaffected row and the translation row are moved verbatim with aligned
 * EE lq/sq.
 *
 * `Vu0CallSin`/`Vu0CallCos` are the resident microprogram's own entry symbols
 * (src/main/vu0/Vu0MicroCode.dvp, exact_vu_microcode, verbatim from the retail
 * symbol table); the VU0 audit's generated config/symbols/main.vu0-symbols.ld
 * carries their VU instruction-memory byte addresses into this link
 * (`Vu0CallSin = 0x020;`, `Vu0CallCos = 0x0E8;`). CMSAR0 takes a microprogram
 * start address in 8-byte instruction pairs, so each entry is divided by 8
 * before it is written to vi27 (docs/ee-reference/vu.md, "vcallms ... does not
 * turn the microprogram's bytes into the EE function body"): they are entry
 * points of a separate program, not EE data this TU owns.
 *
 * Only the VU0 dispatch and the COP2 row combination are inline assembly. The
 * `mfc1` that hands `angle` to qmtc2 and the `mtc1` that takes each returned
 * value back to COP1 are the compiler's own transfers, emitted because the
 * dispatch reads a float through an "r" operand and returns one through an
 * "=r" operand; they are not written here.
 */

extern char Vu0CallSin[];
extern char Vu0CallCos[];

void xglMatrixRotX(Matrix4 destination, const Matrix4 source, float angle)
{
    float cosSin[4];
    float *pair = cosSin;
    float sine;
    float cosine;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    cosSin[1] = sine;
    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosine)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    cosSin[0] = cosine;

    __asm__ __volatile__(
        "lq $2,0(%1)\n\t"
        "lqc2 vf28,16(%1)\n\t"
        "lqc2 vf29,32(%1)\n\t"
        "lqc2 vf1,0(%2)\n\t"
        "sq $2,0(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,vf28xyzw,vf1x\n\t"
        "vmaddy.xyzw vf27xyzw,vf29xyzw,vf1y\n\t"
        "vmulax.xyzw ACCxyzw,vf29xyzw,vf1x\n\t"
        "vmsuby.xyzw vf30xyzw,vf28xyzw,vf1y\n\t"
        "sqc2 vf27,16(%0)\n\t"
        "sqc2 vf30,32(%0)"
        :
        : "r"(destination), "r"(source), "r"(pair)
        : "$2", "memory"
    );
}

void xglMatrixRotY(Matrix4 destination, const Matrix4 source, float angle)
{
    float cosSin[4];
    float *pair = cosSin;
    float sine;
    float cosine;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    cosSin[1] = sine;
    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosine)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    cosSin[0] = cosine;

    __asm__ __volatile__(
        "lq $2,16(%1)\n\t"
        "lqc2 vf27,0(%1)\n\t"
        "lqc2 vf29,32(%1)\n\t"
        "lqc2 vf1,0(%2)\n\t"
        "sq $2,16(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,vf27xyzw,vf1x\n\t"
        "vmsuby.xyzw vf28xyzw,vf29xyzw,vf1y\n\t"
        "vmulax.xyzw ACCxyzw,vf29xyzw,vf1x\n\t"
        "vmaddy.xyzw vf30xyzw,vf27xyzw,vf1y\n\t"
        "sqc2 vf28,0(%0)\n\t"
        "sqc2 vf30,32(%0)"
        :
        : "r"(destination), "r"(source), "r"(pair)
        : "$2", "memory"
    );
}

void xglMatrixRotZ(Matrix4 destination, const Matrix4 source, float angle)
{
    float cosSin[4];
    float *pair = cosSin;
    float sine;
    float cosine;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    cosSin[1] = sine;
    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosine)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    cosSin[0] = cosine;

    __asm__ __volatile__(
        "lq $2,32(%1)\n\t"
        "lqc2 vf27,0(%1)\n\t"
        "lqc2 vf28,16(%1)\n\t"
        "lqc2 vf1,0(%2)\n\t"
        "sq $2,32(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,vf27xyzw,vf1x\n\t"
        "vmaddy.xyzw vf29xyzw,vf28xyzw,vf1y\n\t"
        "vmulax.xyzw ACCxyzw,vf28xyzw,vf1x\n\t"
        "vmsuby.xyzw vf30xyzw,vf27xyzw,vf1y\n\t"
        "sqc2 vf29,0(%0)\n\t"
        "sqc2 vf30,16(%0)"
        :
        : "r"(destination), "r"(source), "r"(pair)
        : "$2", "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixFrustum);

/*
 * xglMatrixStackUnit/.../RTPS (ee-vu-cop2): the eighteen thin VU0
 * matrix-stack wrappers, re-treated as readable C from the accepted
 * stack.s. Every wrapper except Frustum, Save and Load loads a VU0
 * microprogram entry -- one of the Vu0Call* symbols exported by
 * src/main/vu0/Vu0MicroCode.dvp through config/symbols/main.vu0-symbols.ld
 * -- shifts it from a VU byte address to an instruction-pair index and
 * dispatches it with vcallmsr. The original `lui %hi(SYM)/addiu %lo(SYM)/
 * srl ,3` is exactly the C expression `(unsigned int)Vu0CallSYM >> 3`: the
 * compiler emits it from ordinary address arithmetic against the linked
 * symbol, so only ctc2.i/vnop/vcallmsr and the VF transfers are inline
 * assembly. An angle is handed to the block as an ordinary "r" operand and
 * the compiler emits the COP1 transfer itself, which is what reproduces the
 * original's mfc1 between the lui and the addiu (CP-0161). Save/Load
 * dispatch no microprogram at all: they are sqc2/lqc2 quartets over
 * vf28..vf31, the resident current-matrix registers (Vu0MicroCode.dvp,
 * "THE RESIDENT MATRIX"), with ctc2.i to $vi0 used only for its
 * interlocked-wait side effect (vi0 is hardwired zero, so the transferred
 * value itself is discarded). Frustum needs no VF transfer: it writes its
 * two quadwords of frustum parameters straight into the VU0 data window at
 * 0x11004800 with ordinary volatile stores, then synchronises before the
 * dispatch.
 */
void xglMatrixStackUnit(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackUnit >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackMul(const Matrix4 matrix)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackMul >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 $vf4,0(%1)\n\t"
        "lqc2 $vf5,16(%1)\n\t"
        "lqc2 $vf6,32(%1)\n\t"
        "lqc2 $vf7,48(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(matrix)
        : "memory"
    );
}

void xglMatrixStackReverse(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackReverse >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackInverse(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackInverse >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackScale(const float scale[4])
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackScale >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "lqc2 $vf4,0(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(scale)
        : "memory"
    );
}

void xglMatrixStackTrans(const float translation[4])
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackTrans >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "lqc2 $vf4,0(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(translation)
        : "memory"
    );
}

void xglMatrixStackRotV(const float axis[4], float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotV >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 $vf5,0(%1)\n\t"
        "qmtc2 %2,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(axis), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackRotX(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotX >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %1,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackRotY(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotY >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %1,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackRotZ(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotZ >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %1,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackFrustum(float left, float right, float bottom, float top,
                           float nearPlane, float farPlane)
{
    /* the quadword pair in VU0 data memory the microprogram reads */
    volatile float *window = (volatile float *)0x11004800;
    unsigned int entry = (unsigned int)Vu0CallMatrixStackFrustum >> 3;

    __asm__ __volatile__("ctc2.i %0,$vi27" : : "r"(entry) : "memory");
    window[0] = left;
    window[1] = bottom;
    window[2] = nearPlane;
    window[4] = right;
    window[5] = top;
    window[6] = farPlane;
    ((volatile int *)window)[7] = 0;
    ((volatile int *)window)[3] = 0;
    __asm__ __volatile__("sync" : : : "memory");
    __asm__ __volatile__("vcallmsr $vi27\n\t"
                         "nop"
                         : : : "memory");
}

void xglMatrixStackSave(float matrix[4][4])
{
    __asm__ __volatile__(
        "ctc2.i %0,$vi0\n\t"
        "sqc2 $vf28,0(%0)\n\t"
        "sqc2 $vf29,16(%0)\n\t"
        "sqc2 $vf30,32(%0)\n\t"
        "sqc2 $vf31,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

void xglMatrixStackLoad(float matrix[4][4])
{
    __asm__ __volatile__(
        "ctc2.i %0,$vi0\n\t"
        "lqc2 $vf28,0(%0)\n\t"
        "lqc2 $vf29,16(%0)\n\t"
        "lqc2 $vf30,32(%0)\n\t"
        "lqc2 $vf31,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

void xglMatrixStackPushUnit(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackPushUnit >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackPush(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackPush >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackPop(int count)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackPop >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "ctc2 %1,$vi1\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(count)
        : "memory"
    );
}

void xglMatrixStackMulVector(Vector4 *out, const Vector4 *vector)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackMulVector >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "lqc2 $vf4,0(%2)\n\t"
        "vcallmsr $vi27\n\t"
        "cfc2.i $0,$vi0\n\t"
        "sqc2 $vf5,0(%1)\n\t"
        "nop"
        :
        : "r"(entry), "r"(out), "r"(vector)
        : "memory"
    );
}

void xglMatrixStackRTPS(Vector4 *out, const Vector4 *point,
                        const Vector4 *scale, const Vector4 *offset)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRTPS >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 $vf4,0(%2)\n\t"
        "lqc2 $vf5,0(%3)\n\t"
        "lqc2 $vf6,0(%4)\n\t"
        "vcallmsr $vi27\n\t"
        "cfc2.i $0,$vi0\n\t"
        "sqc2 $vf7,0(%1)\n\t"
        "nop"
        :
        : "r"(entry), "r"(out), "r"(point), "r"(scale), "r"(offset)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrix2Quaternion);

/*
 * xglQuaternion2Matrix (ee-vu-cop2): expands a normalized quaternion into a
 * 4x4 row-major rotation matrix. VU0 macro-mode hardware code with no scalar
 * equivalent under the pinned contract; the body is the original instruction
 * sequence reproduced verbatim inside one asm block. Only the destination and
 * quaternion pointers cross the C boundary; the trailing `nop` restores the
 * original's `jr $31; nop` (docs/tu-worker.md "The delay slot of a recovered
 * function"): a bare `sqc2` before the compiler's own `jr $31` would otherwise
 * be pulled into the delay slot by the assembler's reorder-mode scheduling.
 *
 */
void xglQuaternion2Matrix(Matrix4 destination, const Vector4 *quaternion)
{
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "vaddw.x vf27x, vf0x, vf0w\n\t"
        "vaddw.y vf28y, vf0y, vf0w\n\t"
        "vaddw.z vf29z, vf0z, vf0w\n\t"
        "vadd.xyzw vf14xyzw, vf31xyzw, vf31xyzw\n\t"
        "vsub.w vf27w, vf27w, vf27w\n\t"
        "vsub.w vf28w, vf28w, vf28w\n\t"
        "vsub.w vf29w, vf29w, vf29w\n\t"
        "vmulx.xyzw vf15xyzw, vf31xyzw, vf14x\n\t"
        "vmuly.xyzw vf16xyzw, vf31xyzw, vf14y\n\t"
        "vmulz.xyzw vf17xyzw, vf31xyzw, vf14z\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.x vf20x, vf15x, vf16y\n\t"
        "vaddz.y vf20y, vf16y, vf17z\n\t"
        "vaddx.z vf20z, vf17z, vf15x\n\t"
        "vnop\n\t"
        "vsubw.z vf27z, vf15z, vf16w\n\t"
        "vaddw.y vf27y, vf15y, vf17w\n\t"
        "vsuby.x vf27x, vf27x, vf20y\n\t"
        "vsubw.x vf28x, vf16x, vf17w\n\t"
        "vaddw.z vf28z, vf16z, vf15w\n\t"
        "vsubz.y vf28y, vf28y, vf20z\n\t"
        "vsubw.y vf29y, vf17y, vf15w\n\t"
        "vaddw.x vf29x, vf17x, vf16w\n\t"
        "vsubx.z vf29z, vf29z, vf20x\n\t"
        "sqc2 vf0, 48(%0)\n\t"
        "sqc2 vf27, 0(%0)\n\t"
        "sqc2 vf28, 16(%0)\n\t"
        "sqc2 vf29, 32(%0)\n\t"
        "nop"
        :
        : "r"(destination), "r"(quaternion)
        : "memory"
    );
}

ACCEPTED_ASM("src/main/xgl_2", quat);

void xglRandSeedInit(void)
{
    iRandSeed = 0x12345678ULL;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglGeometryInit);

unsigned short xglSRand(void)
{
    iRandSeed = iRandSeed * 1103515245ULL + 12345ULL;
    return (unsigned short)((iRandSeed >> 16) & 0x7fff);
}

unsigned int xglLRand(void)
{
    int first = iRandSeed * 1103515245LL + 12345LL;
    unsigned long long next = (unsigned int)(first * 1103515245 + 12345);

    iRandSeed = next;
    return (unsigned int)(first << 16) + (unsigned int)(next >> 16);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglFRand);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglFSrand);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", F2I);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", I2F);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglSin);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglCos);
