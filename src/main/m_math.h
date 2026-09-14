/*
 * TU-local declarations of main/tu219 (src/main/m_math.c).
 *
 */

#ifndef SRC_MAIN_M_MATH_H
#define SRC_MAIN_M_MATH_H

float MMathMakeRandom2PI(void);

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
