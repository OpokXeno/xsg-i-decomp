#ifndef INCLUDE_MAIN_XGL_2_H
#define INCLUDE_MAIN_XGL_2_H

#include "shared.h"

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
extern void xglVectorLength(float *destination, const Vector4 *vector);

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
extern void xglMatrixStackUnit(void);

void xglMatrixStackScale(const float scale[4]);

extern void xglMatrixStackRotX(float angle);

extern void xglMatrixStackRotY(float angle);

extern void xglMatrixStackRotZ(float angle);

extern void xglMatrixStackSave(float matrix[4][4]);

unsigned short xglSRand(void);

void xglMatrixStackTrans(const float translation[4]);

#endif /* INCLUDE_MAIN_XGL_2_H */
