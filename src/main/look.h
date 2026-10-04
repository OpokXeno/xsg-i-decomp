/*
 * TU-local declarations of main/tu263 (src/main/look.c).
 */

#ifndef SRC_MAIN_LOOK_H
#define SRC_MAIN_LOOK_H

float xglAsin(float value);

/* MARK provisional: original .lit4 external witnesses, no original symbols. */
#define xglAsin_cubic (-0.01872929931f)

void look_limit(float *angle, float limit);

/* MARK provisional: .lit4 0x004d8554 */
#define xglAsin_quadratic 0.07426100224f

void look_limit_add(float *angle, float target, float step);

/* MARK provisional: .lit4 0x004d8558 */
#define xglAsin_linear 0.2121143937f

/* MARK provisional: .lit4 0x004d855c */
#define xglAsin_pi_over_two_approx 1.570728779f

/* MARK provisional: .lit4 0x004d8560 */
#define xglAsin_pi_over_two 1.570796371f

void LOOK_eyeL_cont(int model, int target, int step);

/* MARK provisional: .lit4 0x004d85a8 */
#define D_004D85A8 0.03999999911f

void LOOK_eyeR_cont(int model, int target, int step);

/* MARK provisional: .lit4 0x004d85ac */
#define D_004D85AC (-0.03999999911f)

#endif /* SRC_MAIN_LOOK_H */
