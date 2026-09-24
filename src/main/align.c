#include "common.h"
#include "shared.h"
#include "align.h"

void alignXYAxis(void) {

}

/*
 * alignXAxisEuler: derive the yaw/pitch Euler pair that rotates an object's
 * local X axis onto "direction". The VU0 macro-mode block reads direction as
 * a quadword, forms x^2+y^2 and x^2+y^2+z^2, and uses two reciprocal-square-
 * root passes (Q) to rescale the components by 1/sqrt(x^2+y^2) and by
 * 1/length, packing the four ratios into components. components.y/components.x
 * hold sin/cos of the heading about Z (their ratio is atan2(y,x), unchanged
 * by the common horizontal scale); components.z/components.w hold sin/cos of
 * the elevation about Y. euler->x (no additional roll once X is aligned) and
 * euler->w (homogeneous term) are fixed.
 */
void alignXAxisEuler(const Vector4 *direction, Vector4 *euler) {
    Vector4 components;
    float pitch;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%1)\n\t"
        "vsubw.y vf11, vf0, vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vmul.xyz vf2, vf1, vf1\n\t"
        "vmul.y vf1, vf1, vf11\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.x vf3, vf2, vf2y\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf3x\n\t"
        "vaddz.x vf4, vf3, vf2z\n\t"
        "vwaitq\n\t"
        "vaddq.x vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf4x\n\t"
        "vmulx.xy vf10, vf1, vf5x\n\t"
        "vwaitq\n\t"
        "vaddq.w vf5, vf0, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vdiv Q, vf0w, vf5x\n\t"
        "vmulw.z vf10, vf1, vf5w\n\t"
        "vwaitq\n\t"
        "vmulq.w vf10, vf5, Q\n\t"
        "sqc2 vf10, 0(%0)"
        : : "r"(&components), "r"(direction)
        : "memory");

    euler->w = 1.0f;
    euler->z = xglAtan2(components.y, components.x);
    pitch = xglAtan2(components.z, components.w);
    euler->x = 0.0f;
    euler->y = pitch;
}

INCLUDE_ASM("asm/main/nonmatchings/align", alignXAxis);

INCLUDE_ASM("asm/main/nonmatchings/align", alignYAxis);

INCLUDE_ASM("asm/main/nonmatchings/align", alignZAxis);
