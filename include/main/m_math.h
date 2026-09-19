#ifndef INCLUDE_MAIN_M_MATH_H
#define INCLUDE_MAIN_M_MATH_H

#include "shared.h"

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
                         HermiteVector *endpoint_first, HermiteVector *endpoint_second);

#endif /* INCLUDE_MAIN_M_MATH_H */
