/*
 * TU-local declarations of main/tu263 (src/main/look.c).
 */

#ifndef SRC_MAIN_LOOK_H
#define SRC_MAIN_LOOK_H

float xglAsin(float value);

/* MARK provisional: original .lit4 external witnesses, no original symbols. */
extern const float xglAsin_cubic;

void look_limit(float *angle, float limit);

/* MARK provisional: .lit4 0x004d8554 */
extern const float xglAsin_quadratic;

void look_limit_add(float *angle, float target, float step);

/* MARK provisional: .lit4 0x004d8558 */
extern const float xglAsin_linear;

/* MARK provisional: .lit4 0x004d855c */
extern const float xglAsin_pi_over_two_approx;

/* MARK provisional: .lit4 0x004d8560 */
extern const float xglAsin_pi_over_two;

void LOOK_eyeL_cont(int model, int target, int step);

/* MARK provisional: .lit4 0x004d85a8 */
extern const float D_004D85A8;

void LOOK_eyeR_cont(int model, int target, int step);

/* MARK provisional: .lit4 0x004d85ac */
extern const float D_004D85AC;

#endif /* SRC_MAIN_LOOK_H */
