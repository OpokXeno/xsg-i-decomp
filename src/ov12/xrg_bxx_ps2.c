/*
 * OV12 original TU 81: 0x00a48548..0x00a487c0 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_bxx_ps2.h"

/* VRAM base (TBP0 and the palette CBP offset) the textures are uploaded to. */
#define XRG_BXX_PS2_TEXTURE_BASE 0x3800

const char bxxPs2Log2ErrorFormat[16] = "log2 error %d";

const char bxxPs2SourcePath[24] = "../xrg_bxx_ps2.euc.c";


/* Returns ceil(log2(bucket_count)), at least 1; reports an error for counts above 4096. */
static int _Log2(u32 bucket_count)
{
    if (bucket_count < 3U) {
        return 1;
    }
    if (bucket_count < 5U) {
        return 2;
    }
    if (bucket_count < 9U) {
        return 3;
    }
    if (bucket_count < 17U) {
        return 4;
    }
    if (bucket_count < 33U) {
        return 5;
    }
    if (bucket_count < 65U) {
        return 6;
    }
    if (bucket_count < 129U) {
        return 7;
    }
    if (bucket_count < 257U) {
        return 8;
    }
    if (bucket_count < 513U) {
        return 9;
    }
    if (bucket_count < 1025U) {
        return 10;
    }
    if (bucket_count < 2049U) {
        return 11;
    }
    if (bucket_count < 4097U) {
        return 12;
    }

    RgError(bxxPs2Log2ErrorFormat, bxxPs2SourcePath, 50, bucket_count);
    return 0;
}

const char bxxPs2AssertPicNotNil[16] = "pPic != NIL";

const char bxxPs2AssertHeadNotNil[16] = "pHead != NIL";

const char bxxPs2ImpossibleText[8] = "muri";

const char bxxPs2UnknownBppFormat[32] = "unknown BPP type %d";

/* Builds the GS TEX0 value of a texture from its bit depth, its header sizes and its palette base. */
long long XrgBxxPs2Tex0(RgBxxPic *texture)
{
    long long tex0;
    long long pixelBits;
    u64 paletteBits;
    u64 tbpCldBits;
    RgBxxHeader *header;
    u32 width = 0;
    u32 psm = SCE_GS_PSMCT32;
    int bpp;
    u32 height;
    u32 widthLog2;
    u32 heightLog2;

    if (texture == 0) {
        assert_prog(bxxPs2AssertPicNotNil, bxxPs2SourcePath, 69);
    }
    header = RgBxxGetHeader(texture->pOwner);
    if (header == 0) {
        assert_prog(bxxPs2AssertHeadNotNil, bxxPs2SourcePath, 71);
    }
    bpp = texture->bpp;
    height = header->height;
    switch (texture->bpp) {
    case 4:
        psm = SCE_GS_PSMT4;
        width = header->width4;
        break;
    case 8:
        psm = SCE_GS_PSMT8;
        width = header->width8;
        break;
    case 16:
        psm = SCE_GS_PSMCT16;
        width = header->width16;
        break;
    case 24:
        psm = SCE_GS_PSMCT24;
        width = header->width24;
        break;
    case 32:
        psm = SCE_GS_PSMCT32;
        width = header->width24;
        break;
    case -8:
        RgError(bxxPs2ImpossibleText, bxxPs2SourcePath, 95, texture->bpp);
        break;
    default:
        RgError(bxxPs2UnknownBppFormat, bxxPs2SourcePath, 98, texture->bpp);
        break;
    }
    widthLog2 = _Log2(width << 6);
    heightLog2 = _Log2(height);
    tbpCldBits = XRG_BXX_PS2_TEXTURE_BASE | (1ULL << GS_TEX0_CLD_SHIFT);
    tex0 = widthLog2;
    tex0 = tex0 << GS_TEX0_TW_SHIFT;
    tex0 |= (u64)1 << GS_TEX0_TCC_SHIFT;
    pixelBits = psm;
    pixelBits <<= GS_TEX0_PSM_SHIFT;
    pixelBits |= (u64)width << GS_TEX0_TBW_SHIFT;
    paletteBits = (u64)heightLog2 << GS_TEX0_TH_SHIFT;
    paletteBits |= (u64)(u32)(texture->paletteBase + XRG_BXX_PS2_TEXTURE_BASE) << GS_TEX0_CBP_SHIFT;
    tex0 |= tbpCldBits;
    pixelBits |= paletteBits;
    tex0 |= pixelBits;
    return tex0;
}
