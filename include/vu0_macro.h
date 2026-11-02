#ifndef VU0_MACRO_H
#define VU0_MACRO_H

/*
 * The original's VU0 macro-mode helpers. Each one is a single asm statement
 * that loads its operands into fixed VU0 registers with lqc2, runs the VU0
 * operation and stores the result with sqc2. Every address is forced into a
 * GPR and nothing is scheduled across the statement ("memory"). The fixed
 * registers and load order are part of the original code: each library
 * (xgl, MMath, nml, sef, ...) has its own register convention, so the same
 * operation written by two libraries is two helpers, not one.
 */

/*
 * Vector operations through vf2/vf3, the second operand loaded first (used
 * across the game code), and the four-lane difference through vf20/vf21.
 *
 *   Vu0AddVectorXYZ(d, a, b)  d.xyz = a.xyz + b.xyz, d.w = a.w
 *   Vu0SubVectorXYZ(d, a, b)  d.xyz = a.xyz - b.xyz, d.w = a.w
 *   Vu0SubVector(d, a, b)     d = a - b
 */

#define Vu0AddVectorXYZ(destination, first, second) \
    __asm__ __volatile__( \
        "lqc2 vf3, 0(%2)\n" \
        "lqc2 vf2, 0(%1)\n" \
        "vadd.xyz vf2, vf2, vf3\n" \
        "sqc2 vf2, 0(%0)\n" \
        : \
        : "r" (destination), "r" (first), "r" (second) \
        : "memory")

#define Vu0SubVectorXYZ(destination, minuend, subtrahend) \
    __asm__ __volatile__( \
        "lqc2 vf3, 0(%2)\n" \
        "lqc2 vf2, 0(%1)\n" \
        "vsub.xyz vf2, vf2, vf3\n" \
        "sqc2 vf2, 0(%0)\n" \
        : \
        : "r" (destination), "r" (minuend), "r" (subtrahend) \
        : "memory")

#define Vu0SubVector(destination, minuend, subtrahend) \
    __asm__ __volatile__( \
        "lqc2 vf20, 0(%1)\n" \
        "lqc2 vf21, 0(%2)\n" \
        "vsub.xyzw vf20, vf20, vf21\n" \
        "sqc2 vf20, 0(%0)\n" \
        : \
        : "r" (destination), "r" (minuend), "r" (subtrahend) \
        : "memory")

/*
 * The sef effect library's vector operations through vf1/vf2, the first
 * operand loaded first; the scale goes through $8 (mfc1, qmtc2). Its
 * particle drawing keeps the inverse view in vf16..vf19, the particle
 * rotation in vf13..vf15 and builds the four corners of a sprite quad in
 * vf1..vf4 (centre p, half extents w and h, in one coordinate plane).
 *
 *   Vu0SefAddVectorXYZ(d, a, b)      d.xyz = a.xyz + b.xyz, d.w = a.w
 *   Vu0SefSubVectorXYZ(d, a, b)      d.xyz = a.xyz - b.xyz, d.w = a.w
 *   Vu0SefMulVectorXYZ(d, a, b)      d.xyz = a.xyz * b.xyz, d.w = a.w
 *   Vu0SefScaleVectorXYZ(d, v, s)    d.xyz = v.xyz * s, d.w = v.w
 *   Vu0SefScaleVectorXYZNI(d, v, s)  the same with a non-interlocking qmtc2.ni
 *   Vu0SefIntToFloatVector(d, v)     d = (float) v, all four lanes
 *   Vu0SefSetInverseView(m)          vf16..vf19 = m
 *   Vu0SefSetRotation(m)             vf13..vf15 = m[0..2]
 *   Vu0SefQuadCornersXY(w, h, p)     vf1..vf4 = p -/+ w on x, +/- h on y
 *   Vu0SefQuadCornersXZ(w, h, p)     vf1..vf4 = p -/+ w on x, +/- h on z
 *   Vu0SefQuadCornersYZ(w, h, p)     vf1..vf4 = p -/+ w on y, +/- h on z
 */

#define Vu0SefAddVectorXYZ(destination, first, second) \
    __asm__ __volatile__( \
        "lqc2 vf1, 0(%1)\n" \
        "lqc2 vf2, 0(%2)\n" \
        "vadd.xyz vf1, vf1, vf2\n" \
        "sqc2 vf1, 0(%0)\n" \
        : \
        : "r" (destination), "r" (first), "r" (second) \
        : "memory")

#define Vu0SefSubVectorXYZ(destination, minuend, subtrahend) \
    __asm__ __volatile__( \
        "lqc2 vf1, 0(%1)\n" \
        "lqc2 vf2, 0(%2)\n" \
        "vsub.xyz vf1, vf1, vf2\n" \
        "sqc2 vf1, 0(%0)\n" \
        : \
        : "r" (destination), "r" (minuend), "r" (subtrahend) \
        : "memory")

#define Vu0SefMulVectorXYZ(destination, first, second) \
    __asm__ __volatile__( \
        "lqc2 vf1, 0(%1)\n" \
        "lqc2 vf2, 0(%2)\n" \
        "vmul.xyz vf1, vf1, vf2\n" \
        "sqc2 vf1, 0(%0)\n" \
        : \
        : "r" (destination), "r" (first), "r" (second) \
        : "memory")

#define Vu0SefScaleVectorXYZ(destination, vector, scale) \
    __asm__ __volatile__( \
        "lqc2 vf1, 0(%1)\n" \
        "mfc1 $8, %2\n" \
        "qmtc2 $8, vf2\n" \
        "vmulx.xyz vf1, vf1, vf2x\n" \
        "sqc2 vf1, 0(%0)\n" \
        : \
        : "r" (destination), "r" (vector), "f" (scale) \
        : "$8", "memory")

#define Vu0SefScaleVectorXYZNI(destination, vector, scale) \
    __asm__ __volatile__( \
        "lqc2 vf1, 0(%1)\n" \
        "mfc1 $8, %2\n" \
        "qmtc2.ni $8, vf2\n" \
        "vmulx.xyz vf1, vf1, vf2x\n" \
        "sqc2 vf1, 0(%0)\n" \
        : \
        : "r" (destination), "r" (vector), "f" (scale) \
        : "$8", "memory")

#define Vu0SefIntToFloatVector(destination, source) \
    __asm__ __volatile__( \
        "lqc2 vf1, 0(%1)\n" \
        "vitof0.xyzw vf1, vf1\n" \
        "sqc2 vf1, 0(%0)\n" \
        : \
        : "r" (destination), "r" (source) \
        : "memory")

#define Vu0SefSetInverseView(matrix) \
    __asm__ __volatile__( \
        "lqc2 vf16, 0(%0)\n" \
        "lqc2 vf17, 16(%0)\n" \
        "lqc2 vf18, 32(%0)\n" \
        "lqc2 vf19, 48(%0)\n" \
        : \
        : "r" (matrix) \
        : "memory")

#define Vu0SefSetRotation(matrix) \
    __asm__ __volatile__( \
        "lqc2 vf13, 0(%0)\n" \
        "lqc2 vf14, 16(%0)\n" \
        "lqc2 vf15, 32(%0)\n" \
        : \
        : "r" (matrix) \
        : "memory")

#define Vu0SefQuadCornersXY(halfWidth, halfHeight, center) \
    __asm__ __volatile__( \
        "lqc2 vf5, 0(%2)\n" \
        "mfc1 $2, %0\n" \
        "mfc1 $3, %1\n" \
        "qmtc2 $2, vf6\n" \
        "qmtc2 $3, vf7\n" \
        "vmove.zw vf1, vf5\n" \
        "vmove.zw vf2, vf5\n" \
        "vmove.zw vf3, vf5\n" \
        "vmove.zw vf4, vf5\n" \
        "vsubx.x vf1, vf5, vf6x\n" \
        "vaddx.x vf2, vf5, vf6x\n" \
        "vsubx.x vf3, vf5, vf6x\n" \
        "vaddx.x vf4, vf5, vf6x\n" \
        "vaddx.y vf1, vf5, vf7x\n" \
        "vaddx.y vf2, vf5, vf7x\n" \
        "vsubx.y vf3, vf5, vf7x\n" \
        "vsubx.y vf4, vf5, vf7x\n" \
        : \
        : "f" (halfWidth), "f" (halfHeight), "r" (center) \
        : "$2", "$3", "memory")

#define Vu0SefQuadCornersXZ(halfWidth, halfHeight, center) \
    __asm__ __volatile__( \
        "lqc2 vf5, 0(%2)\n" \
        "mfc1 $2, %0\n" \
        "mfc1 $3, %1\n" \
        "qmtc2 $2, vf6\n" \
        "qmtc2 $3, vf7\n" \
        "vmove.yw vf1, vf5\n" \
        "vmove.yw vf2, vf5\n" \
        "vmove.yw vf3, vf5\n" \
        "vmove.yw vf4, vf5\n" \
        "vsubx.x vf1, vf5, vf6x\n" \
        "vaddx.x vf2, vf5, vf6x\n" \
        "vsubx.x vf3, vf5, vf6x\n" \
        "vaddx.x vf4, vf5, vf6x\n" \
        "vaddx.z vf1, vf5, vf7x\n" \
        "vaddx.z vf2, vf5, vf7x\n" \
        "vsubx.z vf3, vf5, vf7x\n" \
        "vsubx.z vf4, vf5, vf7x\n" \
        : \
        : "f" (halfWidth), "f" (halfHeight), "r" (center) \
        : "$2", "$3", "memory")

#define Vu0SefQuadCornersYZ(halfWidth, halfHeight, center) \
    __asm__ __volatile__( \
        "lqc2 vf5, 0(%2)\n" \
        "mfc1 $2, %0\n" \
        "mfc1 $3, %1\n" \
        "qmtc2 $2, vf6\n" \
        "qmtc2 $3, vf7\n" \
        "vmove.xw vf1, vf5\n" \
        "vmove.xw vf2, vf5\n" \
        "vmove.xw vf3, vf5\n" \
        "vmove.xw vf4, vf5\n" \
        "vsubx.y vf1, vf5, vf6x\n" \
        "vaddx.y vf2, vf5, vf6x\n" \
        "vsubx.y vf3, vf5, vf6x\n" \
        "vaddx.y vf4, vf5, vf6x\n" \
        "vaddx.z vf1, vf5, vf7x\n" \
        "vaddx.z vf2, vf5, vf7x\n" \
        "vsubx.z vf3, vf5, vf7x\n" \
        "vsubx.z vf4, vf5, vf7x\n" \
        : \
        : "f" (halfWidth), "f" (halfHeight), "r" (center) \
        : "$2", "$3", "memory")

/*
 * Matrices and constants (vf0 = (0, 0, 0, 1)); a matrix applies to the
 * point (v.xyz, 1) in vf31.
 *
 *   Vu0ApplyMatrixXYZ(d, m, v)      d.xyz = m * (v.xyz, 1), d.w = v.w
 *   Vu0ApplyMatrix33XYZ(d, m, v)    d.xyz = m (3x3) * v.xyz, d.w = v.w
 *   Vu0ApplyMatrix2XYZ(d, a, b, v)  d.xyz = b * (a * (v.xyz, 1)), d.w = v.w
 *   Vu0UnitMatrix(d)                m = identity
 *   Vu0SetOrigin(d)                 v = (0, 0, 0, 1)
 */

#define Vu0ApplyMatrixXYZ(destination, matrix, vector) \
    __asm__ __volatile__( \
        "lqc2 vf31, 0(%2)\n" \
        "lqc2 vf27, 0(%1)\n" \
        "lqc2 vf28, 16(%1)\n" \
        "lqc2 vf29, 32(%1)\n" \
        "lqc2 vf30, 48(%1)\n" \
        "vmulax.xyz ACC, vf27, vf31x\n" \
        "vmadday.xyz ACC, vf28, vf31y\n" \
        "vmaddaz.xyz ACC, vf29, vf31z\n" \
        "vmaddw.xyz vf31, vf30, vf0w\n" \
        "sqc2 vf31, 0(%0)\n" \
        : \
        : "r" (destination), "r" (matrix), "r" (vector) \
        : "memory")

#define Vu0ApplyMatrix33XYZ(destination, matrix, vector) \
    __asm__ __volatile__( \
        "lqc2 vf31, 0(%2)\n" \
        "lqc2 vf27, 0(%1)\n" \
        "lqc2 vf28, 16(%1)\n" \
        "lqc2 vf29, 32(%1)\n" \
        "lqc2 vf30, 48(%1)\n" \
        "vmulax.xyz ACC, vf27, vf31x\n" \
        "vmadday.xyz ACC, vf28, vf31y\n" \
        "vmaddz.xyz vf31, vf29, vf31z\n" \
        "sqc2 vf31, 0(%0)\n" \
        : \
        : "r" (destination), "r" (matrix), "r" (vector) \
        : "memory")

#define Vu0ApplyMatrix2XYZ(destination, first, second, vector) \
    __asm__ __volatile__( \
        "lqc2 vf31, 0(%3)\n" \
        "lqc2 vf27, 0(%1)\n" \
        "lqc2 vf28, 16(%1)\n" \
        "lqc2 vf29, 32(%1)\n" \
        "lqc2 vf30, 48(%1)\n" \
        "vmulax.xyz ACC, vf27, vf31x\n" \
        "vmadday.xyz ACC, vf28, vf31y\n" \
        "vmaddaz.xyz ACC, vf29, vf31z\n" \
        "vmaddw.xyz vf31, vf30, vf0w\n" \
        "lqc2 vf27, 0(%2)\n" \
        "lqc2 vf28, 16(%2)\n" \
        "lqc2 vf29, 32(%2)\n" \
        "lqc2 vf30, 48(%2)\n" \
        "vmulax.xyz ACC, vf27, vf31x\n" \
        "vmadday.xyz ACC, vf28, vf31y\n" \
        "vmaddaz.xyz ACC, vf29, vf31z\n" \
        "vmaddw.xyz vf31, vf30, vf0w\n" \
        "sqc2 vf31, 0(%0)\n" \
        : \
        : "r" (destination), "r" (first), "r" (second), "r" (vector) \
        : "memory")

#define Vu0UnitMatrix(destination) \
    __asm__ __volatile__( \
        "vmr32.xyzw vf1, vf0\n" \
        "vmr32.xyzw vf2, vf1\n" \
        "vmr32.xyzw vf3, vf2\n" \
        "sqc2 vf0, 48(%0)\n" \
        "sqc2 vf1, 32(%0)\n" \
        "sqc2 vf2, 16(%0)\n" \
        "sqc2 vf3, 0(%0)\n" \
        : \
        : "r" (destination) \
        : "memory")

#define Vu0SetOrigin(destination) \
    __asm__ __volatile__( \
        "sqc2 vf0, 0(%0)\n" \
        : \
        : "r" (destination) \
        : "memory")

/*
 * Resident VU0 state kept across calls: the current matrix in vf27..vf30,
 * the view scale/translation in vf25/vf26 and the clip matrices in vf12..vf19.
 *
 *   Vu0SetCurrentMatrix(m)               vf27..vf30 = m
 *   Vu0GetCurrentMatrix(m)               m = vf27..vf30
 *   Vu0ApplyCurrentMatrix(d, v)          d = current matrix * (v.xyz, 1)
 *   Vu0CurrentRotTransPersClip(f, d, v)  d = screen point of v, f = clip flags & 0x3f
 *   Vu0SetViewScaleTranslation(s, t)     vf25 = s, vf26 = t
 *   Vu0SetClipMatrices(c)                vf12..vf19 = c[0], c[1]
 */

#define Vu0SetCurrentMatrix(matrix) \
    __asm__ __volatile__( \
        "lqc2 vf27, 0(%0)\n" \
        "lqc2 vf28, 16(%0)\n" \
        "lqc2 vf29, 32(%0)\n" \
        "lqc2 vf30, 48(%0)\n" \
        : \
        : "r" (matrix) \
        : "memory")

#define Vu0GetCurrentMatrix(matrix) \
    __asm__ __volatile__( \
        "sqc2 vf27, 0(%0)\n" \
        "sqc2 vf28, 16(%0)\n" \
        "sqc2 vf29, 32(%0)\n" \
        "sqc2 vf30, 48(%0)\n" \
        : \
        : "r" (matrix) \
        : "memory")

#define Vu0ApplyCurrentMatrix(destination, vector) \
    __asm__ __volatile__( \
        "lqc2 vf31, 0(%1)\n" \
        "vmulax.xyzw ACC, vf27, vf31x\n" \
        "vmadday.xyzw ACC, vf28, vf31y\n" \
        "vmaddaz.xyzw ACC, vf29, vf31z\n" \
        "vmaddw.xyzw vf31, vf30, vf0w\n" \
        "sqc2 vf31, 0(%0)\n" \
        : \
        : "r" (destination), "r" (vector) \
        : "memory")

#define Vu0CurrentRotTransPersClip(flags, destination, vector) \
    __asm__ __volatile__( \
        "ctc2 $0, $vi18\n" \
        "lqc2 vf31, 0(%2)\n" \
        "vmulax.xyzw ACC, vf27, vf31x\n" \
        "vmadday.xyzw ACC, vf28, vf31y\n" \
        "vmaddaz.xyzw ACC, vf29, vf31z\n" \
        "vmaddw.xyzw vf31, vf30, vf0w\n" \
        "vnop\n" \
        "vnop\n" \
        "vnop\n" \
        "vclipw.xyz vf31, vf31w\n" \
        "vdiv Q, vf0w, vf31w\n" \
        "vwaitq\n" \
        "vmulq.xyzw vf31, vf31, Q\n" \
        "vmulaw.xyzw ACC, vf26, vf0w\n" \
        "vmadd.xyzw vf31, vf31, vf25\n" \
        "vftoi4.xyw vf23, vf31\n" \
        "vftoi0.z vf23, vf31\n" \
        "vsub.w vf23, vf23, vf23\n" \
        "sqc2 vf23, 0(%1)\n" \
        "cfc2 $2, $vi18\n" \
        "nop\n" \
        "andi $2, $2, 0x3f\n" \
        "move %0, $2\n" \
        : "=r" (flags) \
        : "r" (destination), "r" (vector) \
        : "$2", "memory")

#define Vu0SetViewScaleTranslation(scale, translation) \
    __asm__ __volatile__( \
        "lqc2 vf25, 0(%0)\n" \
        "lqc2 vf26, 0(%1)\n" \
        : \
        : "r" (scale), "r" (translation) \
        : "memory")

#define Vu0SetClipMatrices(clip) \
    __asm__ __volatile__( \
        "lqc2 vf12, 0(%0)\n" \
        "lqc2 vf13, 16(%0)\n" \
        "lqc2 vf14, 32(%0)\n" \
        "lqc2 vf15, 48(%0)\n" \
        "lqc2 vf16, 64(%0)\n" \
        "lqc2 vf17, 80(%0)\n" \
        "lqc2 vf18, 96(%0)\n" \
        "lqc2 vf19, 112(%0)\n" \
        : \
        : "r" (clip) \
        : "memory")

/*
 * A helper of one library: nml (vf10/vf11), used by two of its TUs.
 *
 *   Vu0DistanceSquaredXYZ(r, a, b)  r = |b.xyz - a.xyz|^2
 */

#define Vu0DistanceSquaredXYZ(result, first, second) \
    __asm__ __volatile__( \
        "lqc2 vf10, 0(%1)\n" \
        "lqc2 vf11, 0(%2)\n" \
        "vsub.xyz vf10, vf11, vf10\n" \
        "vmul.xyz vf10, vf10, vf10\n" \
        "vaddz.x vf10, vf10, vf10z\n" \
        "vaddy.x vf10, vf10, vf10y\n" \
        "qmfc2 $2, vf10\n" \
        "mtc1 $2, $f2\n" \
        "mtc1 $2, %0\n" \
        : "=f" (result) \
        : "r" (first), "r" (second) \
        : "$2", "memory")

/*
 * VU0 micro subroutines. Sine and cosine are called with vcallms at
 * Vu0CallSin (0x20) and Vu0CallCos (0xe8): argument in vf4, result in vf1.
 * The others pass the address / 8 in vi27 and call it with vcallmsr.
 *
 *   Vu0Sin(r, x)                 r = sin(x)
 *   Vu0Cos(r, x)                 r = cos(x)
 *   Vu0CallMicro(a)              call
 *   Vu0CallMicroArgument(a, x)   vf4 = x (a GPR value), call
 *   Vu0CallMicroVector(a, v)     vf4 = *v, call
 *   Vu0CallMicroResult(r, a, x)  vf4 = x, call, r = vf1 (a GPR value)
 */

#define Vu0Sin(result, angle) \
    __asm__ __volatile__( \
        "mfc1 $8, %1\n" \
        "qmtc2 $8, vf4\n" \
        "vcallms 0x20\n" \
        "qmfc2.i $8, vf1\n" \
        "mtc1 $8, %0\n" \
        : "=f" (result) \
        : "f" (angle) \
        : "$8", "memory")

#define Vu0Cos(result, angle) \
    __asm__ __volatile__( \
        "mfc1 $8, %1\n" \
        "qmtc2 $8, vf4\n" \
        "vcallms 0xe8\n" \
        "qmfc2.i $8, vf1\n" \
        "mtc1 $8, %0\n" \
        : "=f" (result) \
        : "f" (angle) \
        : "$8", "memory")

#define Vu0CallMicro(address) \
    __asm__ __volatile__( \
        "ctc2.i %0, $vi27\n" \
        "vnop\n" \
        "vnop\n" \
        "vcallmsr $vi27\n" \
        : \
        : "r" (address) \
        : "memory")

#define Vu0CallMicroArgument(address, argument) \
    __asm__ __volatile__( \
        "ctc2.i %0, $vi27\n" \
        "vnop\n" \
        "qmtc2 %1, vf4\n" \
        "vcallmsr $vi27\n" \
        : \
        : "r" (address), "r" (argument) \
        : "memory")

#define Vu0CallMicroVector(address, vector) \
    __asm__ __volatile__( \
        "ctc2.i %0, $vi27\n" \
        "vnop\n" \
        "lqc2 vf4, 0(%1)\n" \
        "vcallmsr $vi27\n" \
        : \
        : "r" (address), "r" (vector) \
        : "memory")

#define Vu0CallMicroResult(result, address, argument) \
    __asm__ __volatile__( \
        "ctc2.i %1, $vi27\n" \
        "vnop\n" \
        "qmtc2 %2, vf4\n" \
        "vcallmsr $vi27\n" \
        "qmfc2.i %0, vf1\n" \
        : "=r" (result) \
        : "r" (address), "r" (argument) \
        : "memory")

#endif
