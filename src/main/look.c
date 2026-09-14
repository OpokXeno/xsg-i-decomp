#include "common.h"
#include "look.h"

                                              /* MARK provisional: .lit4 0x004d8564 */

float xglAsin(float value)
{
    /* MARK: explicit NaN-fallback call structure -- emits sqrt.s + c.eq.s/bc1t
     * plus jal sqrtf@0x003240f8 on the NaN path; a plain sqrtf() call cannot
     * reproduce this (S2 evidence). */
    float root = __builtin_sqrtf(1.0f - value);
    float result;
    float cubic = xglAsin_cubic;
    float quadratic = xglAsin_quadratic;
    float linear = xglAsin_linear;
    float pi_over_two_approx = xglAsin_pi_over_two_approx;

    result = value * cubic;
    result += quadratic;
    result *= value;
    result -= linear;
    result *= value;
    result += pi_over_two_approx;
    return xglAsin_pi_over_two - root * result;
}

void look_limit(float *angle, float limit)
{
    /* MARK: __builtin_fabsf required -- emits inline abs.s, no call (S1: plain
     * fabsf() adds jal fabsf and mismatches). */
    if (__builtin_fabsf(*angle) < limit) {
        *angle = 0.0f;
        return;
    }
    if (limit <= *angle) {
        *angle -= limit;
    }
    if (*angle <= -limit) {
        *angle += limit;
    }
}

void look_limit_add(float *angle, float target, float step)
{
    /* MARK: __builtin_fabsf required -- emits inline abs.s, no call (S1). */
    float delta = *angle - target;

    if (__builtin_fabsf(delta) < step) {
        *angle = target;
        return;
    }
    if (step <= delta) {
        *angle -= step;
    }
    delta = *angle - target;
    if (delta <= -step) {
        *angle += step;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/look", look_get_place);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_target_init);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_target_doit);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_sebo_cont);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_head_cont);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_eyeL_cont);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_eyeR_cont);

INCLUDE_ASM("asm/main/nonmatchings/look", LOOK_eye_cont);
