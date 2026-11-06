#ifndef INCLUDE_MAIN_XGL_PACKET_H
#define INCLUDE_MAIN_XGL_PACKET_H

#include "shared.h"

/* The image block filled by sceGsSetDefLoadImage and sent as five quadwords. */
typedef struct XglLoadImage {
    u64 giftag;
    unsigned char unmodeled_08[0x58];
} XglLoadImage;

/*
 * The VIF1 direct-mode packet builder object. xglPacketInit constructs two
 * of them back to back inside asPacketSource, 0x28 bytes apart (main
 * 0x0022c458-0x0022c4b4 writes both entries' +0x20 and +0x24 at that
 * stride), so sizeof(XglPacket) is 0x28. Only the members the functions of
 * this TU touch are named; everything the sceVif1Pk* library alone reads or
 * writes stays unmodeled.
 */
struct XglPacket {
    /* xglFontReloadTexture decrements this forward cursor by four words. */
    u32 *write_cursor;
    /*
     * +0x04: the address xglPacketSend hands to xglDmaDirectSrcChain as the
     * DMA source-chain start whenever a packet is pending (main
     * 0x0022c520-0x0022c548). No function of this TU writes it -- neither
     * xglPacketInit nor xglPacketMove touches +0x04 -- so it is set by the
     * sceVif1Pk* library: sceVif1PkInit (main 0x0020ac68) stores its base
     * argument at +0x00 and +0x04, and xglPacketInit passes it each entry's
     * buffer base (0x00c00000 and 0x00e00000).
     */
    u32 start;
    /*
 * +0x08 and +0x0c: xglPrimAddGifTagDirect (main 0x00222c00 lw v0,8(s0);
 * main 0x00222c0c lw v0,0xc(s0)) tests both for non-zero to decide
 * whether a direct-mode block the sceVif1Pk* library opened is still
 * open, so it appends to that block instead of resetting/opening/
 * closing a new one (compare SGsSendPacket, src/main/sv_get_image_
 * item.c, which always resets/opens/closes without this check).
 * Neither word is written by any function of this TU, so both are set
 * by the sceVif1Pk* library.
 */
    u32 directCode;
    u32 *directState;
    unsigned char unmodeled_10[0x10];
    /*
     * +0x20: the packet's fixed top-of-buffer address. xglPacketInit sets it
     * once per entry after both sceVif1PkInit calls (main 0x0022c49c for the
     * first entry, value 0x00e00000; main 0x0022c4a0 for the second, value
     * 0x01000000): the end of the buffer whose base went to sceVif1PkInit.
     */
    u32 limit;
    /*
     * +0x24: the write cursor. xglPacketInit seeds it equal to `limit` (main
     * 0x0022c48c/0x0022c490) and xglPacketMove reloads `limit` into it at
     * the start of every frame (main 0x0022c4e4/0x0022c4f0), so writers that
     * fill the packet backward from the top always restart from the same
     * address. main/tu107's nmlPacketSetAttributeData and
     * nmlPacketSetAttributeAlloc16N read it, decrement it by the size being
     * requested and store it back, then use the new value as the memcpy
     * destination or return it as the allocation (main 0x00238e28-0x00238e3c
     * and 0x00238fe0-0x00238ff4),
     * which is why the member is byte-addressed instead of scaled.
     */
    u8 *cursor;
};

/* The entry list starts at entryOffset and contains count records. */
typedef struct XglTextureTransSet {
    unsigned char unmodeled_00[8];
    int count;
    u32 entryOffset;
} XglTextureTransSet;

extern XglPacket *xglPacketGetCurrent(void);

/* VIFcodes. */
#define VIF_CODE_FLUSHE 0x11000000
#define VIF_CODE_DIRECTHL 0x51000000

/* GIF tag fields. */
#define GIF_TAG_EOP 0x8000
#define GIF_TAG_PRE 0x4000
#define GIF_TAG_PRIM_SHIFT 15
#define GIF_TAG_NREG_SHIFT 28
#define GIF_TAG_HIGH_NREG_1 0x10000000U
#define GIF_TAG_NREG_1 0x1000000000000000
#define GIF_TAG_FLG_IMAGE 0x0800000000000000
#define GIF_REG_RGBAQ 1
#define GIF_REG_UV 3
#define GIF_REG_XYZ2 5
#define GIF_REG_TEX0_1 6
#define GIF_REG_AD 14

/* GS PRIM types and attribute bits. */
#define GS_PRIM_TRISTRIP 4
#define GS_PRIM_SPRITE 6
#define GS_PRIM_TME 0x10
#define GS_PRIM_ABE 0x40
#define GS_PRIM_FST 0x100

/* GS register addresses as used in A+D data and by sceGs*. */
#define SCE_GS_PRIM 0x00
#define SCE_GS_RGBAQ 0x01
#define SCE_GS_XYZF2 0x04
#define SCE_GS_XYZ2 0x05
#define SCE_GS_TEX0_1 0x06
#define SCE_GS_A_D 0x0E
#define SCE_GS_TEX1_1 0x14
#define SCE_GS_CLAMP_1 0x08
#define SCE_GS_MIPTBP1_1 0x34
#define SCE_GS_MIPTBP2_1 0x36
#define SCE_GS_XYOFFSET_1 0x18
#define SCE_GS_XYOFFSET_2 0x19
#define SCE_GS_PRMODECONT 0x1A
#define SCE_GS_TEXA 0x3B
#define SCE_GS_TEXFLUSH 0x3F
#define SCE_GS_SCISSOR_1 0x40
#define SCE_GS_SCISSOR_2 0x41
#define SCE_GS_ALPHA_1 0x42
#define SCE_GS_DIMX 0x44
#define SCE_GS_DTHE 0x45
#define SCE_GS_COLCLAMP 0x46
#define SCE_GS_TEST_1 0x47
#define SCE_GS_TEST_2 0x48
#define SCE_GS_FRAME_1 0x4C
#define SCE_GS_FRAME_2 0x4D
#define SCE_GS_ZBUF_1 0x4E
#define SCE_GS_ZBUF_2 0x4F
#define SCE_GS_BITBLTBUF 0x50
#define SCE_GS_TRXPOS 0x51
#define SCE_GS_TRXREG 0x52
#define SCE_GS_TRXDIR 0x53

/* Bit positions of the FRAME and BITBLTBUF fields. */
#define GS_FRAME_FBW_SHIFT 16
#define GS_FRAME_PSM_SHIFT 24
#define GS_BITBLTBUF_SBW_SHIFT 16
#define GS_BITBLTBUF_SPSM_SHIFT 24
#define GS_BITBLTBUF_DBP_SHIFT 32
#define GS_BITBLTBUF_DBW_SHIFT 48
#define GS_BITBLTBUF_DPSM_SHIFT 56

/* Bit positions of the TEX0 fields. */
#define GS_TEX0_TBW_SHIFT 14
#define GS_TEX0_PSM_SHIFT 20
#define GS_TEX0_TW_SHIFT 26
#define GS_TEX0_TH_SHIFT 30
#define GS_TEX0_TCC_SHIFT 34
#define GS_TEX0_CBP_SHIFT 37
#define GS_TEX0_CLD_SHIFT 61

/* GS alpha 0x80 is fully opaque. */
#define GS_ALPHA_OPAQUE 0x80

/* GS pixel storage modes. */
#define SCE_GS_PSMCT32 0
#define SCE_GS_PSMCT16 2
#define SCE_GS_PSMT8 0x13
#define SCE_GS_PSMT4 0x14
#define SCE_GS_PSMCT24 1
#define SCE_GS_PSMZ24 0x31

#endif /* INCLUDE_MAIN_XGL_PACKET_H */
