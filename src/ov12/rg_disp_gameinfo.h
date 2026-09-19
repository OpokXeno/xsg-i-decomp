/*
 * TU-local declarations of ov12/tu038 (src/ov12/rg_disp_gameinfo.c).
 */

#ifndef SRC_OV12_RG_DISP_GAMEINFO_H
#define SRC_OV12_RG_DISP_GAMEINFO_H

/*
 * Opaque handle to a src/ov12/xrg_paint2d.c (ov12/tu086) painter object; that
 * TU is not recovered yet, so this TU only ever moves the pointer.
 */
typedef struct XrgPaint2D XrgPaint2D;

/*
 * The size RgHeapAlloc is called with (CreateRgDispGameInfo, ov12:0x00a23794).
 * No member beyond +0x58 is claimed: nothing in this TU reads or writes
 * +0x5c..+0x6f.
 */
#define RG_DISP_GAME_INFO_SIZE 0x70

/*
 * Field layout evidenced by this TU's own functions:
 *   hostRobot  +0x00  _SetHostRobot stores it (RgDispGameInfoSetHostRobot
 *              forwards to it).
 *   subRobot   +0x04  _SetSubRobot stores it (RgDispGameInfoSetSubRobot
 *              forwards to it).
 *   paint      +0x08  every paint helper (_DisposeInfo, _paint_one_texture_alpha,
 *              _paint_set_tex_alpha, RgDispGameInfoSetStudio) forwards it to the
 *              XrgPaint2D/DisposeXrgPaint2D_sub calls.
 *   dispTex    +0x0C  _InitInfo stores RgBattleCommonDataGetDispTex()'s result;
 *              _paint_one_texture_alpha / _paint_set_tex_alpha pass it to
 *              RgBxxGetPic as the picture archive.
 *   studio     +0x58  RgDispGameInfoSetStudio stores it and forwards it to
 *              XrgPaint2DSetDrawID.
 * +0x10..+0x57 is an untouched gap: this TU's own _InitInfo (ov12:0x00a22a78,
 * not part of this allocation) writes floats at +0x10/+0x50, an int at +0x54
 * and ints at +0x5c/+0x60 (besides seeding studio with -2), none of which
 * any function of this allocation reads or writes.
 */
typedef struct RgDispGameInfo {
    void *hostRobot;
    void *subRobot;
    XrgPaint2D *paint;
    int dispTex;
    unsigned char unmodeled_10[0x58 - 0x10];
    int studio;
} RgDispGameInfo;

#endif /* SRC_OV12_RG_DISP_GAMEINFO_H */
