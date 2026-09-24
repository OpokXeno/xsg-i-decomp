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
 * _InitInfo (ov12:0x00a22a78) is now part of this allocation. It writes
 * +0x10/+0x50 (float), +0x54/+0x5c/+0x60 (int) and seeds studio with -2; see
 * the member comment below for the named fields this adds past +0x58.
 */

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
/*
 * _InitInfo now claims these fields inside the former +0x10..+0x57 gap and
 * past +0x58 (the note above predates this TU's own _InitInfo allocation).
 * Each is written exactly once by _InitInfo and never read by any claimed
 * function in this TU (_PassTimeInfo's body is a no-op beyond its null
 * check; _DispInfo, the only function that could plausibly read them back,
 * is still INCLUDE_ASM). The seeded value is the only evidence, so the
 * names below record offset and type, not an unproven role:
 *   value1  +0x10  float, seeded to 1.0f.
 *   value2  +0x50  float, seeded to 1.0f.
 *   value3  +0x54  int, seeded to 2.
 *   value4  +0x5C  int, seeded to 0.
 *   value5  +0x60  int, seeded to 0; +0x64..+0x6F stays an untouched gap.
 * +0x14..+0x4F stays an untouched gap between value1 and value2.
 */
typedef struct RgDispGameInfo {
    void *hostRobot;
    void *subRobot;
    XrgPaint2D *paint;
    int dispTex;
    float value1;
    unsigned char unmodeled_14[0x50 - 0x14];
    float value2;
    int value3;
    int studio;
    int value4;
    int value5;
    unsigned char unmodeled_64[0x70 - 0x64];
} RgDispGameInfo;

#endif /* SRC_OV12_RG_DISP_GAMEINFO_H */
