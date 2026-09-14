/*
 * OV12 original TU 86: 0x00a4a620..0x00a4c840 (42 functions)
 */
#include "common.h"
#include "xrg_paint2d.h"

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _openVifGif_00A4A620);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _openVifGifAD_00A4A698);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _closeVifGif_00A4A700);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _InitReq);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _DeriveInfoReq);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcRectangle);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcUV);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcPosOnScreen);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcPosOnScreenFloat);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcOffset);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Geom2D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Line3D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _DrawNoTex);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _DrawWithTex);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _InitPaint);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _DestructPaint);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", CreateXrgPaint2D_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", DisposeXrgPaint2D_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _WrapperDestruct_00A4B910);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", InstanceOfXrgPaint2D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DSetDrawID);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DSetDrawPrio);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffset2D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffset2DDot);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffsetCenter);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffset3D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffset3DForce);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffsetResult);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffsetLinear);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DColor);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DAlpha);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DUseTexture);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DSetUVOffset);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DSetUVSize);

/*
 * InitXrgPaint2DRect clears the rectangle request consumed by the adjacent
 * XrgPaint2DDrawRect/XrgPaint2DDrawXYWH routines: an all-zero rectangle at the
 * screen origin, unrotated, at its natural size.  The layout the members come
 * from is recovered in src/ov12/xrg_paint2d.h.
 *
 * The store order (mode, scale, height, width, y, x, angle) is the one the
 * accepted form was matched with and is unchanged here, so this form isolates
 * the layout recovery from the schedule.
 */

void InitXrgPaint2DRect(XrgPaint2DRect *rectangle, int mode)
{
    rectangle->mode = mode;
    rectangle->scale = 1.0f;
    rectangle->height = 0;
    rectangle->width = 0;
    rectangle->y = 0;
    rectangle->x = 0;
    rectangle->angle = 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DDrawRect);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DDrawXYWH);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", InitXrgPaint2DLine3D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DDrawLine);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Paint2DFlush);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Paint2DClearReq);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DFlush);
