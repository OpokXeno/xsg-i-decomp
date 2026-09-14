/*
 * TU-local declarations of ov12/tu086 (src/ov12/xrg_paint2d.c).
 *
 */

#ifndef SRC_OV12_XRG_PAINT2D_H
#define SRC_OV12_XRG_PAINT2D_H

/*
 * The rectangle request the caller fills in and hands to XrgPaint2DDrawRect,
 * which copies it into the paint object's request ring: `mode` reaches the
 * request at +0x08, the four words at +0x10..+0x1f reach it as one quadword
 * (lqc2/sqc2 at 0x00a4c0ec/0x00a4c0f0) and `scale`/`angle` reach +0x20/+0x24
 * with lwc1/swc1 (0x00a4c0f4..0x00a4c100).  Every field below is named from
 * what _Geom2D (0x00a4ad80), the geometry callback _InitReq installs for such
 * a request, does with the copy:
 *
 *   mode    the request's mode word.  _Geom2D loads it once (0x00a4adcc) and
 *           uses it as a bit set - bit 0x20 takes the size from the bound
 *           picture instead of width/height (0x00a4ade0), bit 0x40 applies
 *           `scale` (0x00a4ae38), bit 0x80 applies `angle` (0x00a4af24), bit
 *           0x100 a per-frame jitter (0x00a4afd8) - and the low four bits as
 *           two 2-bit anchor selectors, one per axis (0x00a4aeb0..0x00a4af0c:
 *           0 keeps the corner, 1 subtracts half the extent, 2 subtracts the
 *           whole extent).  An unknown selector reaches RgError with the
 *           game's own message "unknown XrgPaint2DGeom mode (%x)" (D_00A59070,
 *           0x00a4af00), which is where the word's name comes from.
 *   x, y    the screen position in whole pixels: _Geom2D loads them with lw,
 *           shifts left by 4 and converts with cvt.s.w (0x00a4ae60..0x00a4ae94),
 *           i.e. it turns integer pixels into the GS's 12.4 subpixel units,
 *           and the final vftoi0 (0x00a4b064) writes them back as integers.
 *           The three callers in rg_select_agws compute them with integer
 *           addiu (_DrawWepListDisp 0x00a39b20/0x00a39b24), never as floats.
 *   width,  the extent in whole pixels, converted the same way
 *   height  (0x00a4ae10..0x00a4ae34) and used as the second corner's offset.
 *   scale   a multiplier on width and height, mul.s at 0x00a4ae50/0x00a4ae54;
 *           InitXrgPaint2DRect seeds it with 1.0f.
 *   angle   the rotation about Z applied to the four corners,
 *           XrgRotMatrixZ(0x00a4af84) takes it in $f12.
 *
 * Nothing in the six images reads or writes +0x04..+0x0f: this TU's only
 * writers of a rectangle are InitXrgPaint2DRect and XrgPaint2DDrawXYWH
 * (0x00a4c1c0..0x00a4c1d0), the only reader is XrgPaint2DDrawRect, and the
 * three external callers all go through XrgPaint2DDrawXYWH.  The span is the
 * alignment the quadword payload forces after the mode word - XrgPaint2DDrawRect
 * reaches +0x10 with lqc2, which requires a 16-byte aligned address, and the
 * sibling descriptor InitXrgPaint2DLine3D builds (0x00a4c1f8) has the same
 * shape: mode at +0x00, untouched +0x04..+0x0f, and two XrgClearVector
 * vectors at +0x10 and +0x20.  It is declared as that alignment, not as a
 * field.  The type stops at +0x28; XrgPaint2DDrawXYWH's frame shows the
 * object is 0x30 bytes, and nothing evidences +0x28..+0x2f.
 */
typedef struct XrgPaint2DRect {
    int mode;
    unsigned char quadword_alignment_gap[12];
    int x;
    int y;
    int width;
    int height;
    float scale;
    float angle;
} XrgPaint2DRect;

#endif /* SRC_OV12_XRG_PAINT2D_H */
