/*
 * TU-local declarations of ov12/tu069 (src/ov12/rg_select_agws.c).
 */

#ifndef SRC_OV12_RG_SELECT_AGWS_H
#define SRC_OV12_RG_SELECT_AGWS_H

typedef struct FloatArrow FloatArrow;

/*
 * The selection screen keeps two picture handles followed by the two arrow
 * x/y coordinate pairs.  _DispFloatArrow reads the visibility mask at 0x18;
 * this function only updates the four coordinate fields.  Scalar members are
 * used here (changed hypothesis vs the predecessor array-member layout);
 * offsets are identical: pic0 0x00, pic1 0x04, x0 0x08, x1 0x0c, y0 0x10,
 * y1 0x14, visibility_mask 0x18.
 */
struct FloatArrow {
    void *pic0;
    void *pic1;
    int x0;
    int x1;
    int y0;
    int y1;
    unsigned int visibility_mask;
};

extern float s_fSinRad;

#endif /* SRC_OV12_RG_SELECT_AGWS_H */
