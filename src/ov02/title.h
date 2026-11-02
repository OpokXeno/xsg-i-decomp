/*
 * TU-local declarations of ov02/tu002 (src/ov02/title.c).
 */

#ifndef SRC_OV02_TITLE_H
#define SRC_OV02_TITLE_H

#include "shared.h"

/* Only the ten bytes touched by particle_reset are recovered here. */
typedef struct TitleParticleView {
    u16 x;
    s16 z;
    s16 y;
    s16 depth;
    s16 speed;
} TitleParticleView;

/* One 128-bit GS/VIF quadword of the scratchpad packet: two 64-bit halves. */
typedef struct TitleGsQword {
    u64 lo;
    u64 hi;
} TitleGsQword;

/* A+D register write: a 64-bit value and the GS register address it goes to. */
typedef struct TitleGsAd {
    u64 data;
    u64 reg;
} TitleGsAd;

/* PACKED-mode RGBAQ colour: four 32-bit words. */
typedef struct TitleGsRgbaq {
    u32 r;
    u32 g;
    u32 b;
    u32 a;
} TitleGsRgbaq;

/* PACKED-mode UV texel: two 32-bit words; the high half is not written. */
typedef struct TitleGsUv {
    u32 u;
    u32 v;
    u8 unmodeled_08[8];
} TitleGsUv;

/* PACKED-mode XYZ2 vertex; the ADC flag is bit 15 of adc. */
typedef struct TitleGsXyz2 {
    u32 x;
    u32 y;
    u32 z;
    u32 adc;
} TitleGsXyz2;

/* The 36-quadword (0x240-byte) packet that build_mipmap writes at 0x70000000. */
typedef struct TitleMipmapPacket {
    TitleGsQword vif_direct;
    TitleGsQword frame_tag;
    TitleGsAd frame_1;
    TitleGsAd tex0_1;
    TitleGsAd tex1_1;
    TitleGsAd test_1;
    TitleGsQword vertex_tag;
    TitleGsRgbaq rgbaq_0;
    TitleGsUv uv_0;
    TitleGsXyz2 xyz_0;
    TitleGsUv uv_1;
    TitleGsXyz2 xyz_1;
    TitleGsQword texture_tag;
    TitleGsAd tex0_texture;
    TitleGsRgbaq rgbaq_1;
    TitleGsUv uv_2;
    TitleGsXyz2 xyz_2;
    TitleGsUv uv_3;
    TitleGsXyz2 xyz_3;
    TitleGsRgbaq rgbaq_2;
    TitleGsUv uv_4;
    TitleGsXyz2 xyz_4;
    TitleGsUv uv_5;
    TitleGsXyz2 xyz_5;
    TitleGsRgbaq rgbaq_3;
    TitleGsUv uv_6;
    TitleGsXyz2 xyz_6;
    TitleGsUv uv_7;
    TitleGsXyz2 xyz_7;
    TitleGsQword mip_tag;
    TitleGsAd frame_2;
    TitleGsAd miptbp1;
    TitleGsAd miptbp2;
    TitleGsAd tex1_2;
    TitleGsAd tex0_2;
    TitleGsAd texture_flush;
} TitleMipmapPacket;

extern u8 D_2A007F8[];

typedef struct TitleMcRequest {
    int slot;
    const unsigned char *path;
    unsigned char *buffer;
    int size;
} TitleMcRequest;

extern int xglMcRequest(int card, int operation, const TitleMcRequest *arguments, int *result);

typedef struct TitleJpegRequest {
    u8 *source;
    u8 *output_end;
    u8 unmodeled_08[0x18];
} TitleJpegRequest;

typedef struct TitleRecordHeader {
    u8 unmodeled_00[8];
    u32 count;
    u32 first;
} TitleRecordHeader;

typedef struct TitleRecord {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 w12;
    u32 w16;
} TitleRecord;

typedef struct TitleFrameCopy {
    u8 bytes[0x70];
} TitleFrameCopy;

extern u64 TestEnv_0_00A0F780[12];
extern u64 TransEnv_1_00A0F7E0[4];
extern u64 FlushEnv_2_00A0F800[6];
extern u64 TestEnv_3[22];
extern u64 image_4[36];
extern u64 FlushEnv_5[6];
extern TitleFrameCopy TestEnvOrg_6;
extern u64 TestEnv_10[12];
extern u32 TestEnv_11[52];
extern u64 TestEnv_12[20];

#endif /* SRC_OV02_TITLE_H */
