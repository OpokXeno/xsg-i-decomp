/*
 * TU-local declarations of main/tu219 (src/main/m_math.c).
 *
 */

#ifndef SRC_MAIN_M_MATH_H
#define SRC_MAIN_M_MATH_H

#include "shared.h"

float MMathMakeRandom(void);

float MMathMakeRandom2PI(void);

/*
 * Scaffold-owned .lit4 constant (config/symbols/main.txt does not name it;
 * value 1/32767, the same convention as random_two_pi_factor below but
 * scaling xglSRand() to a 0..1 range instead of 0..2*pi).
 */
extern const float D_004D8374;

void MMathCalcDir(const Vector4 *from, const Vector4 *to);

extern float MMathCalcRotNear(float first, float second);

extern float fmodf(float value, float modulus);

float MMathCalcRotFar(float first, float second);

extern const float random_two_pi_factor;

extern const float near_two_pi;

extern const float near_pi;

extern const float far_two_pi;

extern const float far_pi;

/*
 * Scaffold-owned .lit4 constants ( data_ownership:
 * ".lit4": "asm"; config/symbols/main.txt already names them, same convention
 * as near_two_pi/far_two_pi above). Referenced from MMathDeg2RadVector/I and
 * MMathRad2DegVector/I so the compiler emits the original's own gp-relative
 * load and relocation against the named symbol, not a hand-coded $gp offset
 * or a folded literal.
 */
extern const float degrees_to_radians;
extern const float degrees_to_radians_integer;
extern const float radians_to_degrees;
extern const float radians_to_degrees_integer;

#endif /* SRC_MAIN_M_MATH_H */
