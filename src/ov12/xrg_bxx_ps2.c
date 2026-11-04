/*
 * OV12 original TU 81: 0x00a48548..0x00a487c0 (2 functions)
 */
#include "common.h"
#include "shared.h"

const char D_00A58D10[16] = "log2 error %d";

const char D_00A58D20[24] = "../xrg_bxx_ps2.euc.c";

extern void RgError(const char *message, const char *source_file, int line,
                    ...);

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

    RgError(D_00A58D10, D_00A58D20, 50, bucket_count);
    return 0;
}

const char D_00A58D38[16] = "pPic != NIL";

const char D_00A58D48[16] = "pHead != NIL";

const char D_00A58D58[8] = "muri";

const char D_00A58D60[32] = "unknown BPP type %d";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/* Partial view of the texture object: only the members this function reads. */
typedef struct XrgBxxPs2Texture {
    unsigned char unmodeled_00[0x24];
    int bpp;
    unsigned char unmodeled_28[0x14];
    u32 paletteBase;
    unsigned char unmodeled_40[0x18];
    u32 header;
} XrgBxxPs2Texture;

/* Partial view of the archive header returned by RgBxxGetHeader. */
typedef struct XrgBxxPs2Header {
    unsigned char unmodeled_00[0x10];
    int width4;
    int width8;
    int width16;
    int width24;
    u32 height;
} XrgBxxPs2Header;

extern XrgBxxPs2Header *RgBxxGetHeader(int header);

long long XrgBxxPs2Tex0(XrgBxxPs2Texture *texture)
{
    long long tex0;
    long long pixelBits;
    u64 paletteBits;
    u64 tbpCldBits;
    XrgBxxPs2Header *header;
    u32 width = 0;
    u32 psm = 0;
    int bpp;
    u32 height;
    u32 widthLog2;
    u32 heightLog2;

    if (texture == 0) {
        assert_prog(D_00A58D38, D_00A58D20, 69);
    }
    header = RgBxxGetHeader(texture->header);
    if (header == 0) {
        assert_prog(D_00A58D48, D_00A58D20, 71);
    }
    bpp = texture->bpp;
    height = header->height;
    switch (texture->bpp) {
    case 4:
        psm = 20;
        width = header->width4;
        break;
    case 8:
        psm = 19;
        width = header->width8;
        break;
    case 16:
        psm = 2;
        width = header->width16;
        break;
    case 24:
        psm = 1;
        width = header->width24;
        break;
    case 32:
        psm = 0;
        width = header->width24;
        break;
    case -8:
        RgError(D_00A58D58, D_00A58D20, 95, texture->bpp);
        break;
    default:
        RgError(D_00A58D60, D_00A58D20, 98, texture->bpp);
        break;
    }
    widthLog2 = _Log2(width << 6);
    heightLog2 = _Log2(height);
    tbpCldBits = 0x3800ULL | (1ULL << 61);
    tex0 = widthLog2;
    tex0 = tex0 << 26;
    tex0 |= (u64)0x8000 << 19;
    pixelBits = psm;
    pixelBits <<= 20;
    pixelBits |= (u64)width << 14;
    paletteBits = (u64)heightLog2 << 30;
    paletteBits |= (u64)(u32)(texture->paletteBase + 0x3800) << 37;
    tex0 |= tbpCldBits;
    pixelBits |= paletteBits;
    tex0 |= pixelBits;
    return tex0;
}
