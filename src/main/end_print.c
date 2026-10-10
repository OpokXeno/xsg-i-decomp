#include "common.h"

#include "shared.h"

#include "end_print.h"

#include "main/xgl_jpeg.h"

#define NULL ((void *)0)

#define SPRITE_UV_BYTES(u, v, width, height) \
    (u) & 255, (u) >> 8, (v) & 255, (v) >> 8, \
    (width) & 255, (width) >> 8, (height) & 255, (height) >> 8

typedef struct EndPrintRenderState {
    unsigned char unmodeled_00[4];
    short scissor_width;                 /* +0x04 */
    short scissor_height;                /* +0x06 */
    unsigned char unmodeled_08[0x0c];
    unsigned short draw_back_value;      /* +0x14 */
    unsigned char unmodeled_16[0x0a];
    unsigned short draw_back_reset_value; /* +0x20 */
} EndPrintRenderState;

extern EndPrintRenderState sRender;

/* 511 callback entries plus the adjacent terminal/overflow entry. The
 * scaffold's D_00532A38 label is the interior address PrintFunc[511]. */

static PrintFuncEntry PrintFunc[512];

static PrintFuncEntry *pPrintFuncTop;

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

extern void eMessageSpriteReset(void);

extern void xglFontReloadTexture(EndPrintContext *context, int mode);

/*
 * uv_clut: a fixed table of UV/CLUT entries; only the CLUT id at +0x04 of
 * each 8-byte entry is read here.
 */

typedef struct UvClutEntry {
    const unsigned char *uv;       /* +0x00 */
    int clut;                      /* +0x04 */
} UvClutEntry;

static const unsigned char cur_uv[32] = {
    SPRITE_UV_BYTES(0, 192, 16, 16),
    SPRITE_UV_BYTES(64, 160, 16, 16),
    SPRITE_UV_BYTES(0, 176, 16, 16),
    SPRITE_UV_BYTES(64, 176, 16, 16)
};

static const unsigned char others_uv[200] = {
    SPRITE_UV_BYTES(199, 227, 8, 16),
    SPRITE_UV_BYTES(200, 243, 7, 13),
    SPRITE_UV_BYTES(16, 192, 18, 12),
    SPRITE_UV_BYTES(34, 192, 18, 12),
    SPRITE_UV_BYTES(88, 192, 18, 12),
    SPRITE_UV_BYTES(70, 192, 18, 12),
    SPRITE_UV_BYTES(143, 217, 47, 10),
    SPRITE_UV_BYTES(122, 192, 11, 16),
    SPRITE_UV_BYTES(188, 243, 10, 13),
    SPRITE_UV_BYTES(218, 0, 29, 20),
    SPRITE_UV_BYTES(218, 20, 29, 20),
    SPRITE_UV_BYTES(209, 194, 43, 62),
    SPRITE_UV_BYTES(160, 80, 16, 16),
    SPRITE_UV_BYTES(218, 40, 22, 22),
    SPRITE_UV_BYTES(218, 62, 22, 22),
    SPRITE_UV_BYTES(160, 0, 29, 20),
    SPRITE_UV_BYTES(160, 20, 29, 20),
    SPRITE_UV_BYTES(160, 40, 29, 20),
    SPRITE_UV_BYTES(160, 60, 29, 20),
    SPRITE_UV_BYTES(189, 0, 29, 20),
    SPRITE_UV_BYTES(189, 20, 29, 20),
    SPRITE_UV_BYTES(189, 40, 29, 20),
    SPRITE_UV_BYTES(189, 60, 29, 20),
    SPRITE_UV_BYTES(0, 192, 16, 16),
    SPRITE_UV_BYTES(176, 80, 8, 8)
};

static const unsigned char pad_uv[64] = {
    SPRITE_UV_BYTES(0, 208, 22, 24),
    SPRITE_UV_BYTES(22, 208, 22, 24),
    SPRITE_UV_BYTES(44, 208, 22, 24),
    SPRITE_UV_BYTES(66, 208, 22, 24),
    SPRITE_UV_BYTES(198, 132, 29, 20),
    SPRITE_UV_BYTES(198, 92, 29, 20),
    SPRITE_UV_BYTES(227, 132, 29, 20),
    SPRITE_UV_BYTES(227, 92, 29, 20)
};

static const unsigned char icom_uv[376] = {
    SPRITE_UV_BYTES(388, 67, 20, 24),
    SPRITE_UV_BYTES(408, 67, 20, 24),
    SPRITE_UV_BYTES(428, 67, 20, 24),
    SPRITE_UV_BYTES(448, 67, 20, 24),
    SPRITE_UV_BYTES(468, 67, 20, 24),
    SPRITE_UV_BYTES(488, 67, 20, 24),
    SPRITE_UV_BYTES(388, 91, 20, 24),
    SPRITE_UV_BYTES(408, 91, 20, 24),
    SPRITE_UV_BYTES(428, 91, 20, 24),
    SPRITE_UV_BYTES(448, 91, 20, 24),
    SPRITE_UV_BYTES(468, 91, 20, 24),
    SPRITE_UV_BYTES(488, 91, 20, 24),
    SPRITE_UV_BYTES(388, 115, 20, 24),
    SPRITE_UV_BYTES(408, 115, 20, 24),
    SPRITE_UV_BYTES(428, 115, 20, 24),
    SPRITE_UV_BYTES(388, 139, 20, 24),
    SPRITE_UV_BYTES(408, 139, 20, 24),
    SPRITE_UV_BYTES(428, 139, 20, 24),
    SPRITE_UV_BYTES(448, 139, 20, 24),
    SPRITE_UV_BYTES(468, 139, 20, 24),
    SPRITE_UV_BYTES(488, 139, 20, 24),
    SPRITE_UV_BYTES(388, 163, 20, 24),
    SPRITE_UV_BYTES(408, 163, 20, 24),
    SPRITE_UV_BYTES(428, 163, 20, 24),
    SPRITE_UV_BYTES(448, 163, 20, 24),
    SPRITE_UV_BYTES(468, 163, 20, 24),
    SPRITE_UV_BYTES(488, 163, 20, 24),
    SPRITE_UV_BYTES(388, 187, 20, 24),
    SPRITE_UV_BYTES(428, 187, 20, 24),
    SPRITE_UV_BYTES(408, 187, 20, 24),
    SPRITE_UV_BYTES(448, 187, 20, 24),
    SPRITE_UV_BYTES(484, 187, 16, 16),
    SPRITE_UV_BYTES(468, 187, 16, 16),
    SPRITE_UV_BYTES(388, 211, 20, 24),
    SPRITE_UV_BYTES(256, 162, 20, 24),
    SPRITE_UV_BYTES(276, 162, 20, 24),
    SPRITE_UV_BYTES(296, 162, 20, 24),
    SPRITE_UV_BYTES(316, 162, 20, 24),
    SPRITE_UV_BYTES(336, 162, 20, 24),
    SPRITE_UV_BYTES(356, 162, 20, 24),
    SPRITE_UV_BYTES(276, 186, 20, 24),
    SPRITE_UV_BYTES(296, 186, 20, 24),
    SPRITE_UV_BYTES(256, 186, 20, 24),
    SPRITE_UV_BYTES(209, 232, 32, 24),
    SPRITE_UV_BYTES(448, 115, 20, 24),
    SPRITE_UV_BYTES(388, 67, 20, 24),
    SPRITE_UV_BYTES(408, 67, 20, 24)
};

static const unsigned char face_puti_uv[80] = {
    SPRITE_UV_BYTES(896, 0, 20, 24),
    SPRITE_UV_BYTES(916, 0, 20, 24),
    SPRITE_UV_BYTES(936, 0, 20, 24),
    SPRITE_UV_BYTES(956, 0, 20, 24),
    SPRITE_UV_BYTES(896, 24, 20, 24),
    SPRITE_UV_BYTES(916, 24, 20, 24),
    SPRITE_UV_BYTES(936, 24, 20, 24),
    SPRITE_UV_BYTES(976, 24, 20, 24),
    SPRITE_UV_BYTES(976, 0, 20, 24),
    SPRITE_UV_BYTES(996, 0, 20, 24)
};

static const unsigned char tag_uv[416] = {
    SPRITE_UV_BYTES(253, 0, 10, 16),
    SPRITE_UV_BYTES(263, 0, 10, 16),
    SPRITE_UV_BYTES(273, 0, 10, 16),
    SPRITE_UV_BYTES(283, 0, 10, 16),
    SPRITE_UV_BYTES(293, 0, 9, 16),
    SPRITE_UV_BYTES(302, 0, 9, 16),
    SPRITE_UV_BYTES(311, 0, 10, 16),
    SPRITE_UV_BYTES(321, 0, 10, 16),
    SPRITE_UV_BYTES(331, 0, 6, 16),
    SPRITE_UV_BYTES(337, 0, 8, 16),
    SPRITE_UV_BYTES(345, 0, 10, 16),
    SPRITE_UV_BYTES(355, 0, 9, 16),
    SPRITE_UV_BYTES(364, 0, 13, 16),
    SPRITE_UV_BYTES(377, 0, 12, 16),
    SPRITE_UV_BYTES(389, 0, 10, 16),
    SPRITE_UV_BYTES(399, 0, 9, 16),
    SPRITE_UV_BYTES(408, 0, 10, 16),
    SPRITE_UV_BYTES(418, 0, 10, 16),
    SPRITE_UV_BYTES(428, 0, 10, 16),
    SPRITE_UV_BYTES(438, 0, 10, 16),
    SPRITE_UV_BYTES(448, 0, 10, 16),
    SPRITE_UV_BYTES(458, 0, 10, 16),
    SPRITE_UV_BYTES(468, 0, 13, 16),
    SPRITE_UV_BYTES(481, 0, 10, 16),
    SPRITE_UV_BYTES(491, 0, 10, 16),
    SPRITE_UV_BYTES(501, 0, 11, 16),
    SPRITE_UV_BYTES(253, 16, 8, 11),
    SPRITE_UV_BYTES(261, 16, 8, 11),
    SPRITE_UV_BYTES(269, 16, 8, 11),
    SPRITE_UV_BYTES(277, 16, 8, 11),
    SPRITE_UV_BYTES(285, 16, 7, 11),
    SPRITE_UV_BYTES(292, 16, 7, 11),
    SPRITE_UV_BYTES(299, 16, 8, 11),
    SPRITE_UV_BYTES(307, 16, 8, 11),
    SPRITE_UV_BYTES(315, 16, 5, 11),
    SPRITE_UV_BYTES(320, 16, 6, 11),
    SPRITE_UV_BYTES(326, 16, 8, 11),
    SPRITE_UV_BYTES(334, 16, 7, 11),
    SPRITE_UV_BYTES(341, 16, 10, 11),
    SPRITE_UV_BYTES(351, 16, 9, 11),
    SPRITE_UV_BYTES(360, 16, 8, 11),
    SPRITE_UV_BYTES(368, 16, 8, 11),
    SPRITE_UV_BYTES(376, 16, 8, 11),
    SPRITE_UV_BYTES(384, 16, 8, 11),
    SPRITE_UV_BYTES(392, 16, 8, 11),
    SPRITE_UV_BYTES(400, 16, 7, 11),
    SPRITE_UV_BYTES(407, 16, 8, 11),
    SPRITE_UV_BYTES(415, 16, 9, 11),
    SPRITE_UV_BYTES(424, 16, 11, 11),
    SPRITE_UV_BYTES(435, 16, 9, 11),
    SPRITE_UV_BYTES(444, 16, 8, 11),
    SPRITE_UV_BYTES(452, 16, 8, 11)
};

static const unsigned char win_uv[96] = {
    SPRITE_UV_BYTES(141, 141, 3, 3),
    SPRITE_UV_BYTES(0, 141, 128, 3),
    SPRITE_UV_BYTES(144, 141, 3, 3),
    SPRITE_UV_BYTES(141, 0, 3, 128),
    SPRITE_UV_BYTES(144, 0, 3, 128),
    SPRITE_UV_BYTES(141, 144, 3, 3),
    SPRITE_UV_BYTES(0, 144, 128, 3),
    SPRITE_UV_BYTES(144, 144, 3, 3),
    SPRITE_UV_BYTES(0, 0, 128, 128),
    SPRITE_UV_BYTES(154, 208, 44, 19),
    SPRITE_UV_BYTES(316, 187, 20, 24),
    SPRITE_UV_BYTES(512, 186, 165, 24)
};

static const unsigned char num_uv[80] = {
    SPRITE_UV_BYTES(88, 227, 11, 16),
    SPRITE_UV_BYTES(88, 243, 10, 13),
    SPRITE_UV_BYTES(199, 227, 8, 16),
    SPRITE_UV_BYTES(200, 243, 7, 13),
    SPRITE_UV_BYTES(188, 243, 10, 13),
    SPRITE_UV_BYTES(88, 211, 11, 16),
    SPRITE_UV_BYTES(99, 211, 11, 16),
    SPRITE_UV_BYTES(121, 211, 11, 16),
    SPRITE_UV_BYTES(132, 211, 11, 16),
    SPRITE_UV_BYTES(143, 211, 4, 4)
};

static const unsigned char pad2_uv[176] = {
    SPRITE_UV_BYTES(256, 67, 22, 24),
    SPRITE_UV_BYTES(278, 67, 22, 24),
    SPRITE_UV_BYTES(300, 67, 22, 24),
    SPRITE_UV_BYTES(322, 67, 22, 24),
    SPRITE_UV_BYTES(344, 67, 22, 24),
    SPRITE_UV_BYTES(366, 67, 22, 24),
    SPRITE_UV_BYTES(256, 91, 22, 24),
    SPRITE_UV_BYTES(278, 91, 22, 24),
    SPRITE_UV_BYTES(300, 91, 22, 24),
    SPRITE_UV_BYTES(322, 91, 22, 24),
    SPRITE_UV_BYTES(344, 91, 22, 24),
    SPRITE_UV_BYTES(366, 91, 22, 24),
    SPRITE_UV_BYTES(256, 115, 22, 24),
    SPRITE_UV_BYTES(278, 115, 22, 24),
    SPRITE_UV_BYTES(300, 115, 22, 24),
    SPRITE_UV_BYTES(322, 115, 22, 24),
    SPRITE_UV_BYTES(344, 115, 22, 24),
    SPRITE_UV_BYTES(366, 115, 22, 24),
    SPRITE_UV_BYTES(256, 139, 22, 24),
    SPRITE_UV_BYTES(278, 139, 22, 24),
    SPRITE_UV_BYTES(300, 139, 22, 24),
    SPRITE_UV_BYTES(322, 139, 22, 24)
};

static const unsigned char face_agws_uv[112] = {
    SPRITE_UV_BYTES(768, 93, 63, 48),
    SPRITE_UV_BYTES(832, 93, 63, 48),
    SPRITE_UV_BYTES(896, 93, 63, 48),
    SPRITE_UV_BYTES(960, 93, 63, 48),
    SPRITE_UV_BYTES(768, 141, 63, 48),
    SPRITE_UV_BYTES(832, 141, 63, 48),
    SPRITE_UV_BYTES(896, 141, 63, 48),
    SPRITE_UV_BYTES(960, 141, 63, 48),
    SPRITE_UV_BYTES(768, 93, 63, 32),
    SPRITE_UV_BYTES(832, 93, 63, 32),
    SPRITE_UV_BYTES(896, 93, 63, 32),
    SPRITE_UV_BYTES(960, 93, 63, 32),
    SPRITE_UV_BYTES(768, 141, 63, 32),
    SPRITE_UV_BYTES(832, 141, 63, 32)
};

static const unsigned char name_uv[8] = {
    SPRITE_UV_BYTES(256, 210, 105, 24)
};

static const unsigned char others2_uv[24] = {
    SPRITE_UV_BYTES(0, 208, 384, 48),
    SPRITE_UV_BYTES(0, 201, 500, 7),
    SPRITE_UV_BYTES(230, 56, 10, 72)
};

static const unsigned char face00_uv[16] = {
    SPRITE_UV_BYTES(512, 0, 64, 93),
    SPRITE_UV_BYTES(576, 0, 64, 93)
};

static const unsigned char face01_uv[16] = {
    SPRITE_UV_BYTES(640, 0, 64, 93),
    SPRITE_UV_BYTES(704, 0, 64, 93)
};

static const unsigned char face02_uv[16] = {
    SPRITE_UV_BYTES(512, 93, 64, 93),
    SPRITE_UV_BYTES(576, 93, 64, 93)
};

static const unsigned char face03_uv[16] = {
    SPRITE_UV_BYTES(640, 93, 64, 93),
    SPRITE_UV_BYTES(704, 93, 64, 93)
};

static const unsigned char face04_uv[16] = {
    SPRITE_UV_BYTES(768, 0, 64, 93),
    SPRITE_UV_BYTES(832, 0, 64, 93)
};

static const unsigned char thumnail_uv[8] = {
    SPRITE_UV_BYTES(0, 0, 128, 112)
};

static const unsigned char ether_uv[40] = {
    SPRITE_UV_BYTES(0, 0, 38, 48),
    SPRITE_UV_BYTES(38, 0, 38, 48),
    SPRITE_UV_BYTES(76, 0, 38, 48),
    SPRITE_UV_BYTES(160, 0, 13, 16),
    SPRITE_UV_BYTES(114, 0, 46, 56)
};

static const unsigned char seg_uv[192] = {
    SPRITE_UV_BYTES(0, 48, 72, 58),
    SPRITE_UV_BYTES(0, 106, 72, 58),
    SPRITE_UV_BYTES(72, 56, 46, 57),
    SPRITE_UV_BYTES(160, 16, 10, 22),
    SPRITE_UV_BYTES(173, 0, 28, 25),
    SPRITE_UV_BYTES(221, 0, 20, 24),
    SPRITE_UV_BYTES(201, 0, 20, 24),
    SPRITE_UV_BYTES(72, 56, 46, 57),
    SPRITE_UV_BYTES(118, 56, 42, 53),
    SPRITE_UV_BYTES(160, 56, 18, 61),
    SPRITE_UV_BYTES(178, 56, 14, 57),
    SPRITE_UV_BYTES(192, 56, 21, 72),
    SPRITE_UV_BYTES(213, 56, 17, 68),
    SPRITE_UV_BYTES(72, 113, 34, 34),
    SPRITE_UV_BYTES(106, 113, 30, 30),
    SPRITE_UV_BYTES(0, 183, 47, 9),
    SPRITE_UV_BYTES(47, 183, 47, 9),
    SPRITE_UV_BYTES(94, 183, 43, 9),
    SPRITE_UV_BYTES(0, 192, 47, 9),
    SPRITE_UV_BYTES(47, 192, 47, 9),
    SPRITE_UV_BYTES(94, 192, 43, 9),
    SPRITE_UV_BYTES(256, 0, 88, 169),
    SPRITE_UV_BYTES(512, 0, 240, 48),
    SPRITE_UV_BYTES(512, 48, 240, 24)
};

static const unsigned char umn_uv[96] = {
    SPRITE_UV_BYTES(0, 0, 102, 128),
    SPRITE_UV_BYTES(102, 0, 102, 128),
    SPRITE_UV_BYTES(0, 128, 102, 128),
    SPRITE_UV_BYTES(102, 128, 102, 128),
    SPRITE_UV_BYTES(256, 48, 20, 24),
    SPRITE_UV_BYTES(276, 48, 20, 24),
    SPRITE_UV_BYTES(296, 48, 20, 24),
    SPRITE_UV_BYTES(316, 48, 20, 24),
    SPRITE_UV_BYTES(256, 0, 38, 48),
    SPRITE_UV_BYTES(294, 0, 38, 48),
    SPRITE_UV_BYTES(332, 0, 38, 48),
    SPRITE_UV_BYTES(512, 0, 511, 96)
};

static const unsigned char btl_msg_uv[184] = {
    SPRITE_UV_BYTES(256, 47, 22, 24),
    SPRITE_UV_BYTES(278, 47, 22, 24),
    SPRITE_UV_BYTES(300, 47, 22, 24),
    SPRITE_UV_BYTES(322, 47, 22, 24),
    SPRITE_UV_BYTES(344, 47, 22, 24),
    SPRITE_UV_BYTES(256, 71, 22, 24),
    SPRITE_UV_BYTES(278, 71, 22, 24),
    SPRITE_UV_BYTES(300, 71, 22, 24),
    SPRITE_UV_BYTES(322, 71, 22, 24),
    SPRITE_UV_BYTES(344, 71, 22, 24),
    SPRITE_UV_BYTES(366, 71, 22, 24),
    SPRITE_UV_BYTES(256, 95, 22, 24),
    SPRITE_UV_BYTES(278, 95, 22, 24),
    SPRITE_UV_BYTES(300, 95, 22, 24),
    SPRITE_UV_BYTES(322, 95, 22, 24),
    SPRITE_UV_BYTES(344, 95, 22, 24),
    SPRITE_UV_BYTES(366, 95, 22, 24),
    SPRITE_UV_BYTES(256, 119, 22, 24),
    SPRITE_UV_BYTES(278, 119, 22, 24),
    SPRITE_UV_BYTES(300, 119, 22, 24),
    SPRITE_UV_BYTES(322, 119, 22, 24),
    SPRITE_UV_BYTES(344, 119, 22, 24),
    SPRITE_UV_BYTES(366, 119, 22, 24)
};

static const unsigned char btl_msg_uv2[64] = {
    SPRITE_UV_BYTES(353, 143, 22, 24),
    SPRITE_UV_BYTES(375, 143, 22, 24),
    SPRITE_UV_BYTES(397, 143, 22, 24),
    SPRITE_UV_BYTES(419, 143, 22, 24),
    SPRITE_UV_BYTES(281, 143, 18, 46),
    SPRITE_UV_BYTES(299, 143, 18, 46),
    SPRITE_UV_BYTES(317, 143, 18, 46),
    SPRITE_UV_BYTES(335, 143, 18, 46)
};

static const unsigned char btl_msg_uv3[48] = {
    SPRITE_UV_BYTES(160, 0, 20, 24),
    SPRITE_UV_BYTES(180, 0, 20, 24),
    SPRITE_UV_BYTES(200, 0, 20, 24),
    SPRITE_UV_BYTES(160, 24, 9, 10),
    SPRITE_UV_BYTES(169, 24, 13, 10),
    SPRITE_UV_BYTES(182, 24, 11, 10)
};

static const unsigned char umn2_uv[32] = {
    SPRITE_UV_BYTES(370, 0, 38, 40),
    SPRITE_UV_BYTES(408, 0, 40, 40),
    SPRITE_UV_BYTES(370, 40, 38, 40),
    SPRITE_UV_BYTES(408, 40, 38, 40)
};

static const unsigned char umn3_uv[32] = {
    SPRITE_UV_BYTES(448, 0, 22, 26),
    SPRITE_UV_BYTES(470, 0, 22, 26),
    SPRITE_UV_BYTES(448, 26, 22, 26),
    SPRITE_UV_BYTES(470, 26, 22, 26)
};

static UvClutEntry uv_clut[26] = {
    { cur_uv, 0x000FDE00 },
    { others_uv, 0x000FDE00 },
    { pad_uv, 0x000FDE00 },
    { icom_uv, 0x000FDF00 },
    { face_puti_uv, 0x000FEF00 },
    { tag_uv, 0x000FDB00 },
    { win_uv, 0x000FDE00 },
    { num_uv, 0x000FDB00 },
    { pad2_uv, 0x000FDF00 },
    { face_agws_uv, 0x000FEE00 },
    { name_uv, 0x000FDB00 },
    { others2_uv, 0x000FDF00 },
    { face00_uv, 0x000FE300 },
    { face01_uv, 0x000FE600 },
    { face02_uv, 0x000FE700 },
    { face03_uv, 0x000FEA00 },
    { face04_uv, 0x000FEB00 },
    { thumnail_uv, 0 },
    { ether_uv, 0x000FDE00 },
    { seg_uv, 0x000FDB00 },
    { umn_uv, 0x000FDE00 },
    { btl_msg_uv, 0x000FDA00 },
    { btl_msg_uv2, 0x000FD700 },
    { btl_msg_uv3, 0x000FD600 },
    { umn2_uv, 0x000FDF00 },
    { umn3_uv, 0x000FDB00 },
};

/* Sprite coordinates, color and packed UV/CLUT indices consumed by the draw routines. */
typedef struct PrintSpriteInfo {
    short x, y;
    int depth;
    short width, height;
    unsigned char color[4];
    short uv; /* low byte: UV table entry; high byte: CLUT table entry */
    unsigned char column, row;
} PrintSpriteInfo;

typedef struct PrintPointInfo {
    short x;                /* +0x00 */
    short y;                /* +0x02 */
    int depth;              /* +0x04 */
    unsigned char color[4]; /* +0x08..+0x0b */
} PrintPointInfo;

typedef struct PrintCircleInfo {
    short x;                      /* +0x00 */
    short y;                      /* +0x02 */
    int depth;                    /* +0x04 */
    unsigned char color[4];       /* +0x08..+0x0b */
    int radius;                   /* +0x0c */
    int pointCount;               /* +0x10 */
    unsigned char pointColor[4];  /* +0x14..+0x17 */
} PrintCircleInfo;

static void endPrintInfoSet(EndPrintContext *context, int clut, int flag);

static void subPrintSprite(EndPrintContext *context, PrintSpriteInfo *sprite);

static void subPrintLine(EndPrintContext *context, PrintPointInfo *line);

static void subPrintRibbon(EndPrintContext *context, PrintPointInfo *ribbon);

#include "main/xgl_packet.h"

/* EE scratchpad RAM base (src/ov01/gr_gp_init.c's grPacketSend and
 * src/ov01/m_gs.c's MGsGPTerm build the same VIF1 direct-mode packet at the
 * same address). */

#define SCRATCHPAD_BASE ((void *)0x70000000)

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

/* PrintCircleCore (this TU, still asm) draws into the scratchpad buffer this
 * function points it at. */

static void PrintCircleCore(XglPacket *packet, void *buffer, PrintCircleInfo *circle);

/*
 * The JPEG-decode setup context: only the fields endDecodeJpeg itself
 * reads or writes are named. +0x10/+0x14 are filled from the source and
 * destination words already stored at +0x30/+0x34, and +0x20..+0x27 is
 * cleared before the decode call; the rest of the struct is untouched
 * here.
 */

typedef struct EndDecodeJpegContext {
    unsigned char unmodeled_00[8]; /* +0x00..+0x07 */
    short width;                   /* +0x08 */
    short height;                  /* +0x0a */
    unsigned char unmodeled_0c[4]; /* +0x0c..+0x0f */
    int decodeSource;              /* +0x10 */
    int decodeDestination;         /* +0x14 */
    unsigned char unmodeled_18[8]; /* +0x18..+0x1f */
    short decodeParam[4];          /* +0x20..+0x27 */
    unsigned char unmodeled_28[8]; /* +0x28..+0x2f */
    int source;                    /* +0x30 */
    int destination;               /* +0x34 */
} EndDecodeJpegContext;



/* 511 callback entries plus the adjacent terminal/overflow entry. The
 * scaffold's D_00532A38 label is the interior address PrintFunc[511]. */

extern void xglPrimAddGifTagDirect(XglPacket *packet, const void *data, int count);

/*
 * uv_clut: a fixed table of UV/CLUT entries; only the CLUT id at +0x04 of
 * each 8-byte entry is read here.
 */

/*
 * The sprite descriptor subPrintSprite (still asm) draws; only the CLUT
 * index byte at +0x11 is read by PrintSprite00.
 */

typedef struct SpriteUv {
    short u;      /* +0x00 */
    short v;      /* +0x02 */
    short width;  /* +0x04 */
    short height; /* +0x06 */
} SpriteUv;

typedef struct PrintTagFontInfo {
    short x;                  /* +0x00 */
    unsigned short y;         /* +0x02 */
    int depth;                /* +0x04 */
    unsigned char color[4];   /* +0x08..+0x0b */
    unsigned char *text;      /* +0x0c */
} PrintTagFontInfo;

extern void endSpriteSet(PrintSpriteInfo *sprite, int flags);

typedef struct PrintNumberInfo {
    short x;                /* +0x00 */
    unsigned short y;       /* +0x02 */
    int depth;              /* +0x04 */
    unsigned char color[4]; /* +0x08..+0x0b */
    int value;              /* +0x0c */
    unsigned short digits;  /* +0x10 */
    unsigned short flags;   /* +0x12 */
} PrintNumberInfo;

/* EE scratchpad RAM base (src/ov01/gr_gp_init.c's grPacketSend and
 * src/ov01/m_gs.c's MGsGPTerm build the same VIF1 direct-mode packet at the
 * same address). */

/* PrintCircleCore (this TU, still asm) draws into the scratchpad buffer this
 * function points it at. */




/* 511 callback entries plus the adjacent terminal/overflow entry. The
 * scaffold's D_00532A38 label is the interior address PrintFunc[511]. */

typedef struct WindowTexEntry {
    unsigned int bufferWidthAndFormat; /* +0x00 */
    unsigned int clutAddress;          /* +0x04 */
    unsigned int vramAddress;          /* +0x08 */
    int size;                          /* +0x0c */
    int dataOffset;                    /* +0x10 */
} WindowTexEntry;

typedef struct WindowTexDirectory {
    unsigned char unmodeled_00[8]; /* +0x00..+0x07 */
    int entryCount;                /* +0x08 */
    int entryOffset;               /* +0x0c */
} WindowTexDirectory;

extern WindowTexDirectory *WindowTexAddrGet(int index);

extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

extern u64 ePrintEnv_0[12];



typedef struct PrintLinePoint {
    int x;                    /* +0x00 */
    int y;                    /* +0x04 */
    int z;                    /* +0x08 */
    int terminator;           /* +0x0c */
    unsigned char color[4];   /* +0x10..+0x13 */
    unsigned char unmodeled_14[12]; /* +0x14..+0x1f */
} PrintLinePoint;

typedef struct PrintRect {
    short x;      /* +0x00 */
    short y;      /* +0x02 */
    int depth;    /* +0x04 */
    short width;  /* +0x08 */
    short height; /* +0x0a */
} PrintRect;

extern u64 ThumImageEnv_1[12];

extern u64 ThumDrawEnv_2[24];

typedef struct PrintThumbnailInfo {
    short x;                       /* +0x00 */
    short y;                       /* +0x02 */
    int depth;                     /* +0x04 */
    unsigned char unmodeled_08[4]; /* +0x08..+0x0b */
    const void *image;             /* +0x0c */
} PrintThumbnailInfo;



extern float xglSin(float angle);

extern float xglCos(float angle);

extern u64 Env_3[12];

typedef struct PrintBackQuadVertex {
    int x;            /* +0x00 */
    int y;            /* +0x04 */
    int z;            /* +0x08 */
    int unmodeled_0c; /* +0x0c */
} PrintBackQuadVertex;

typedef struct PrintBackQuad {
    PrintBackQuadVertex vertex[4]; /* +0x00..+0x3f */
    float uv[4];                   /* +0x40..+0x4f */
} PrintBackQuad;

typedef struct PrintBackSpriteInfo {
    short x;                /* +0x00 */
    short y;                /* +0x02 */
    int depth;              /* +0x04 */
    unsigned char color[4]; /* +0x08..+0x0b */
    short width;            /* +0x0c */
    short height;           /* +0x0e */
    short u;                /* +0x10 */
    short v;                /* +0x12 */
} PrintBackSpriteInfo;

extern u64 sJpegEnv00_7[16];

extern u64 JpegEnv01_8[20];

extern u64 JpegEnv10_9[16];

extern u64 JpegEnv11_10[20];

typedef struct PrintJpegInfo {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    int depth;                      /* +0x04 */
    short width;                    /* +0x08 */
    short height;                   /* +0x0a */
    short offsetX;                  /* +0x0c */
    short offsetY;                  /* +0x0e */
    unsigned char unmodeled_10[0x24]; /* +0x10..+0x33 */
    const void *image;              /* +0x34 */
} PrintJpegInfo;

/* The original direct-mode writers use EE scratchpad RAM. */

#ifndef SCRATCHPAD_BASE

#endif

int ePrintWHGet(void)
{
    return 0;
}

void endPrintInit(void)
{
    pPrintFuncTop = PrintFunc;
    PrintFunc[0].func = NULL;
    eMessageSpriteReset();
}

void endPrintWinTexLoad(XglPacket *packet)
{
    int i;
    WindowTexDirectory *directory;
    WindowTexEntry *entry;
    unsigned int image;
    unsigned int imageLow;

    i = 0;
    directory = WindowTexAddrGet(0);
    entry = (WindowTexEntry *)((unsigned char *)directory + directory->entryOffset);
    for (; i < directory->entryCount; i++, entry++) {
        image = entry->bufferWidthAndFormat;
        imageLow = image & 0xFFFF;
        ePrintEnv_0[4] = ((u64)((entry->vramAddress + 0xF0000) >> 6) << 32) |
                         ((u64)(image >> 16) << 48);
        ePrintEnv_0[8] = imageLow | ((u64)entry->clutAddress << 32);
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, ePrintEnv_0, 24);
        sceVif1PkRef(packet, (unsigned char *)directory + entry->dataOffset, entry->size, 0, 0, 0);
    }
}

static void PrintFlush(EndPrintContext *context)
{
    PrintFuncEntry *cursor;
    void (*func)(EndPrintContext *context, int param);
    int param;

    xglFontReloadTexture(context, 2);
    cursor = PrintFunc;
    do {
        func = cursor->func;
        if (func == NULL)
            break;
        param = cursor->param;
        cursor++;
        func(context, param);
    } while (1);
    endPrintInit();
    xglFontReloadTexture(context, 1);
    eMessageSpriteReset();
}

void endPrintExtFuncPack(void (*func)(EndPrintContext *context, int param), int param)
{
    PrintFuncEntry *entry;

    entry = pPrintFuncTop;
    entry->func = func;
    entry->param = param;
    entry++;
    pPrintFuncTop = entry;
    if ((u32)(PrintFunc + 511) < (u32)entry) {
        endPrintInit();
        entry = pPrintFuncTop;
    }
    entry->func = NULL;
}

static void endPrintInfoSet(EndPrintContext *context, int clut, int flag)
{
    u64 *command;
    int count;
    unsigned short useClut;

    command = context->scratch + 2;
    command[1] = 0x3F;
    command[0] = 0;
    command = context->scratch + 4;
    command[0] = 0x44;
    command[1] = 0x42;
    command = context->scratch + 6;
    command[0] = 0x3FFFF0;
    command[1] = 8;
    command = context->scratch + 8;
    count = 3;
    useClut = flag & 1;
    if (useClut) {
        if (clut != 0) {
            command[0] = ((u64)((unsigned int)clut >> 6) << 37) | 0x2000000629343C00ULL;
        } else {
            command[0] = 0x00000005DC00BC00ULL;
        }
        command[1] = 6;
        count = 4;
        command += 2;
    }
    command[0] = 0x5000D;
    count++;
    command[1] = 0x47;
    command = context->scratch;
    command[0] = count + ((u64)0x8000 << 45);
    command[1] = 0xE;
    xglPrimAddGifTagDirect(context->packet, command, count + 1);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endSpriteSet);

static void subPrintSprite(EndPrintContext *context, PrintSpriteInfo *sprite)
{
    int *words;
    const SpriteUv *table;
    int uvLeft;
    int uvTop;
    int width;
    int height;
    int left;
    int top;

    words = (int *)context->scratch;
    words[0] = 0x8001;
    words[1] = 0x50AB4000;
    words[2] = 0x53531;
    words[3] = 0;
    table = (const SpriteUv *)uv_clut[(sprite->uv >> 8) & 0xFF].uv;
    uvLeft = table[sprite->uv & 0xFF].u;
    uvTop = table[sprite->uv & 0xFF].v;
    width = table[sprite->uv & 0xFF].width;
    height = table[sprite->uv & 0xFF].height;
    words = (int *)context->scratch + 4;
    words[0] = sprite->color[0];
    words[1] = sprite->color[1];
    words[2] = sprite->color[2];
    words[3] = sprite->color[3];
    words = (int *)context->scratch + 8;
    left = sprite->x * 16 + 0x6FF8;
    top = sprite->y * 16 + 0x71F8;
    words[0] = (uvLeft + width * sprite->column) * 16;
    words[1] = (uvTop + height * sprite->row) * 16;
    words[2] = 0;
    words[3] = 0;
    words[4] = left;
    words[5] = top;
    words[6] = sprite->depth & 0xFFFFFF;
    words[7] = 0;
    words[8] = words[0] + width * 16;
    words[9] = words[1] + height * 16;
    words[10] = 0;
    words[11] = 0;
    words[12] = left + sprite->width * 16;
    words[13] = top + sprite->height * 16;
    words[14] = words[6];
    words[15] = 0;
    xglPrimAddGifTagDirect(context->packet, context->scratch, 6);
}

/* The high byte of the packed UV word selects the CLUT on little-endian EE. */
static void PrintSprite00(EndPrintContext *context, PrintSpriteInfo *sprite)
{
    endPrintInfoSet(context, uv_clut[((const unsigned char *)&sprite->uv)[1]].clut, 1);
    subPrintSprite(context, sprite);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintSprite01);

static void PrintPoint(EndPrintContext *context, PrintPointInfo *point)
{
    int *words;
    int tagHigh;
    int tagLow;

    endPrintInfoSet(context, 0, 0);
    tagLow = 0x8001;
    tagHigh = 0x20204000;
    words = (int *)context->scratch;
    words[0] = tagLow;
    words[1] = tagHigh;
    words[2] = 0x51;
    words[3] = 0;
    words = (int *)context->scratch + 4;
    words[0] = point->color[0];
    words[1] = point->color[1];
    words[2] = point->color[2];
    words[3] = point->color[3];
    words = (int *)context->scratch + 8;
    words[0] = point->x * 16 + 0x6FF8;
    words[1] = point->y * 16 + 0x71F8;
    words[2] = point->depth & 0xFFFFFF;
    words[3] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->scratch, 3);
}

static void subPrintLine(EndPrintContext *context, PrintPointInfo *line)
{
    int *words;
    PrintPointInfo *point;
    PrintPointInfo *scan;
    int count;
    int qwords;

    words = (int *)context->scratch;
    count = 0;
    scan = line + 1;
    if (line->depth != 0) {
        do {
            count++;
        } while ((scan++)->depth != 0);
    }
    words[0] = 0x8000 + count;
    words[1] = 0x20654000;
    qwords = 1;
    words[2] = 0x51;
    words[3] = 0;
    point = line;
    words += 4;
    while (count > 0) {
        count--;
        qwords += 2;
        words[0] = point->color[0];
        words[1] = point->color[1];
        words[2] = point->color[2];
        words[3] = point->color[3];
        words += 4;
        words[0] = point->x * 16 + 0x6FF8;
        words[1] = point->y * 16 + 0x71F8;
        words[2] = line->depth & 0xFFFFFF;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(context->packet, context->scratch, qwords);
}

/* Deferred print callbacks receive their descriptor address in an int slot. */
static void PrintLine(EndPrintContext *context, int lineAddress)
{
    endPrintInfoSet(context, 0, 0);
    subPrintLine(context, (PrintPointInfo *)lineAddress);
}

static void PrintLine2(EndPrintContext *context, PrintLinePoint *line)
{
    int *words;
    PrintLinePoint *point;
    PrintLinePoint *scan;
    int count;
    int qwords;

    endPrintInfoSet(context, 0, 0);
    scan = line + 1;
    words = (int *)context->scratch;
    count = 0;
    if (line->terminator != 0) {
        do {
            count++;
        } while ((scan++)->terminator != 0);
    }
    words[0] = 0x8000 + count;
    words[1] = 0x20654000;
    qwords = 1;
    words[2] = 0x51;
    words[3] = 0;
    point = line;
    words += 4;
    while (count > 0) {
        count--;
        qwords += 2;
        words[0] = point->color[0];
        words[1] = point->color[1];
        words[2] = point->color[2];
        words[3] = point->color[3];
        words += 4;
        words[0] = point->x;
        words[1] = point->y;
        words[2] = point->z;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(context->packet, context->scratch, qwords);
}

void endPrintDirectRibbon(PrintPointInfo *ribbon)
{
    XglPacket *packet;
    u64 *header;
    int *words;
    PrintPointInfo *point;
    int count;
    int qwords;

    packet = xglPacketGetCurrent();
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    header = SCRATCHPAD_BASE;
    header[0] = ((u64)0x10000000 << 32) | 3;
    header[1] = 14;
    header[2] = 0;
    header[3] = 0x3F;
    header[4] = 0x44;
    header[5] = 0x42;
    header[6] = 0x31001;
    header[7] = 0x47;
    sceVif1PkAddDirectDataN(packet, SCRATCHPAD_BASE, 4);
    point = ribbon;
    count = 0;
    if ((point++)->depth != 0) {
        do {
            count++;
        } while ((point++)->depth != 0);
    }
    ((int *)SCRATCHPAD_BASE)[0] = 0x8000 + count;
    ((int *)SCRATCHPAD_BASE)[1] = 0x20A64000;
    ((int *)SCRATCHPAD_BASE)[2] = 0x51;
    ((int *)SCRATCHPAD_BASE)[3] = 0;
    point = ribbon;
    words = (int *)SCRATCHPAD_BASE + 4;
    qwords = 1;
    while (count > 0) {
        count--;
        qwords += 2;
        words[0] = point->color[0];
        words[1] = point->color[1];
        words[2] = point->color[2];
        words[3] = point->color[3];
        words += 4;
        words[0] = point->x * 16 + 0x6FF8;
        words[1] = point->y * 16 + 0x71F8;
        words[2] = ribbon->depth & 0xFFFFFF;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(packet, SCRATCHPAD_BASE, qwords);
    sceVif1PkCloseDirectHLCode(packet);
}

static void subPrintRibbon(EndPrintContext *context, PrintPointInfo *ribbon)
{
    int *words;
    PrintPointInfo *point;
    PrintPointInfo *scan;
    int count;
    int qwords;

    words = (int *)context->scratch;
    count = 0;
    scan = ribbon + 1;
    if (ribbon->depth != 0) {
        do {
            count++;
        } while ((scan++)->depth != 0);
    }
    words[0] = 0x8000 + count;
    words[1] = 0x20A64000;
    qwords = 1;
    words[2] = 0x51;
    words[3] = 0;
    point = ribbon;
    words += 4;
    while (count > 0) {
        count--;
        qwords += 2;
        words[0] = point->color[0];
        words[1] = point->color[1];
        words[2] = point->color[2];
        words[3] = point->color[3];
        words += 4;
        words[0] = point->x * 16 + 0x6FF8;
        words[1] = point->y * 16 + 0x71F8;
        words[2] = ribbon->depth & 0xFFFFFF;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(context->packet, context->scratch, qwords);
}

static void PrintRibbon(EndPrintContext *context, int ribbonAddress)
{
    endPrintInfoSet(context, 0, 0);
    subPrintRibbon(context, (PrintPointInfo *)ribbonAddress);
}

static void PrintTagFont(EndPrintContext *context, PrintTagFontInfo *tag)
{
    PrintSpriteInfo sprite;
    unsigned char *text;
    unsigned char *start;
    unsigned char *measure;
    int x;
    int shift;
    int baseHeight;
    unsigned int character;
    unsigned int symbol;

    shift = 0;
    text = tag->text;
    x = tag->x;
    endPrintInfoSet(context, 0xFDB00, 1);
    if (*text == 1) {
        start = text + 1;
        measure = start;
        while ((character = *measure) != 0) {
            if ((unsigned char)(character - 'A') < 26) {
                sprite.uv = *measure - 'A' + 0x500;
            } else if ((unsigned char)(character - 'a') < 26) {
                sprite.uv = *measure - 'a' + 0x51A;
            } else {
                symbol = (unsigned char)character;
                if ((unsigned char)(character - '0') < 10) {
                    sprite.uv = 0x700;
                } else if (symbol == ':') {
                    sprite.uv = 0x704;
                } else if (symbol == '/') {
                    sprite.uv = 0x703;
                } else if (symbol == '.') {
                    sprite.uv = 0x709;
                } else if (symbol == '-') {
                    sprite.uv = 0x706;
                } else if (symbol == ' ') {
                    shift += 8;
                    measure++;
                    continue;
                } else if (symbol == 3) {
                    measure++;
                    shift += *measure;
                    measure++;
                    continue;
                }
            }
            endSpriteSet(&sprite, 1);
            measure++;
            shift += sprite.width;
        }
        text = start;
    }
    sprite.uv = 0x500;
    endSpriteSet(&sprite, 255);
    sprite.color[0] = tag->color[0];
    sprite.color[1] = tag->color[1];
    sprite.color[2] = tag->color[2];
    sprite.color[3] = tag->color[3];
    baseHeight = sprite.height;
    while ((character = *text) != 0) {
        if ((unsigned char)(character - 'A') < 26) {
            sprite.uv = *text - 'A' + 0x500;
        } else if ((unsigned char)(character - 'a') < 26) {
            sprite.uv = *text - 'a' + 0x51A;
        } else {
            symbol = (unsigned char)character;
            if ((unsigned char)(character - '0') < 10) {
                sprite.uv = 0x700;
                endSpriteSet(&sprite, 1);
                sprite.column = *text - '0';
                sprite.x = x - shift;
                sprite.y = tag->y + (baseHeight - sprite.height);
                sprite.depth = tag->depth - 1;
                subPrintSprite(context, &sprite);
                return;
            } else if (symbol == ':') {
                sprite.uv = 0x704;
            } else if (symbol == '/') {
                sprite.uv = 0x703;
            } else if (symbol == '.') {
                sprite.uv = 0x709;
            } else if (symbol == '-') {
                sprite.uv = 0x706;
            } else if (symbol == ' ') {
                x += 8;
                text++;
                continue;
            } else if (symbol == 3) {
                text++;
                x += *text;
                text++;
                continue;
            }
        }
        endSpriteSet(&sprite, 1);
        sprite.x = x - shift;
        sprite.y = tag->y + (baseHeight - sprite.height);
        sprite.depth = tag->depth - 1;
        subPrintSprite(context, &sprite);
        text++;
        x += sprite.width;
    }
}

static void ScissorSet(EndPrintContext *context, PrintRect *rect)
{
    u64 *scratch;
    int x;
    int y;
    int width;
    int height;

    scratch = context->scratch;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8001;
    scratch[1] = 14;
    x = rect->x;
    y = rect->y;
    width = rect->width;
    height = rect->height;
    if (x < 0) {
        width += x;
        if (width <= 0) {
            width = 1;
        }
        x = 0;
    } else if (x + width >= 512) {
        if (x >= 512) {
            return;
        }
        width -= x + width - 512;
    }
    if (y < 0) {
        height += y;
        if (height <= 0) {
            height = 1;
        }
        y = 0;
    } else if (y + height >= 448) {
        if (y >= 448) {
            return;
        }
        height -= y + height - 448;
    }
    scratch[2] = x | ((u64)(x + width - 1) << 16) | ((u64)y << 32) |
                 ((u64)(y + height - 1) << 48);
    scratch[3] = 64;
    sceVif1PkAddDirectDataN(context->packet, scratch, 2);
}

static void ScissorReset(EndPrintContext *context)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8001;
    scratch[1] = 14;
    scratch[2] = ((u64)(sRender.scissor_width - 1) << 16) |
                 ((u64)(sRender.scissor_height - 1) << 48);
    scratch[3] = 64;
    sceVif1PkAddDirectDataN(context->packet, scratch, 2);
}

static void PrintNumber(EndPrintContext *context, PrintNumberInfo *number)
{
    PrintSpriteInfo sprite;
    int offset;
    int x;
    int i;
    int value;
    int leadingDigit;

    offset = 0;
    endPrintInfoSet(context, 0xFDB00, 1);
    if (number->value < 0) {
        sprite.uv = 0x706;
        endSpriteSet(&sprite, 255);
        sprite.color[0] = number->color[0];
        sprite.color[1] = number->color[1];
        sprite.color[2] = number->color[2];
        sprite.color[3] = number->color[3];
        sprite.y = number->y;
        sprite.depth = number->depth;
        x = number->x;
        if (number->flags & 0x10) {
            offset = -(sprite.width * number->digits);
        }
        for (i = 0; i < number->digits; i++) {
            sprite.x = x + offset;
            subPrintSprite(context, &sprite);
            x += sprite.width;
        }
    } else {
        sprite.uv = (number->flags & 0xF) + 0x700;
        endSpriteSet(&sprite, 255);
        value = number->value;
        if (value < 0) {
            value = 0;
        }
        sprite.y = number->y;
        sprite.depth = number->depth;
        x = number->x + sprite.width * (number->digits - 1);
        if (number->flags & 0x10) {
            offset = -(sprite.width * number->digits);
        }
        /* The original routine reduces a copy to its leading decimal digit. */
        leadingDigit = value;
        while (leadingDigit / 10 != 0) {
            leadingDigit = leadingDigit / 10;
        }
        for (i = 0; i < number->digits; i++) {
            sprite.x = x + offset;
            x -= sprite.width;
            sprite.column = value % 10;
            value = value / 10;
            sprite.color[0] = number->color[0];
            sprite.color[1] = number->color[1];
            sprite.color[2] = number->color[2];
            sprite.color[3] = number->color[3];
            subPrintSprite(context, &sprite);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintWindow);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintFrame);

static void PrintThumbnail(EndPrintContext *context, PrintThumbnailInfo *thumbnail)
{
    int *words;

    sceVif1PkCloseDirectHLCode(context->packet);
    sceVif1PkRef(context->packet, ThumImageEnv_1, 6, 0, 0, 0);
    sceVif1PkRef(context->packet, thumbnail->image, 3586, 0, 0, 0);
    sceVif1PkCnt(context->packet, 0);
    sceVif1PkOpenDirectHLCode(context->packet, 0);
    words = (int *)ThumDrawEnv_2;
    words[36] = thumbnail->x * 16 + 0x6FF8;
    words[37] = thumbnail->y * 16 + 0x71F8;
    words[38] = thumbnail->depth;
    words[44] = thumbnail->x * 16 + 0x77F8;
    words[45] = thumbnail->y * 16 + 0x77D8;
    words[46] = thumbnail->depth;
    sceVif1PkAddDirectDataN(context->packet, ThumDrawEnv_2, 12);
}

static void PrintCircleCore(XglPacket *packet, void *buffer, PrintCircleInfo *circle)
{
    u64 *header;
    int *words;
    int count;
    int i;
    float angle;

    header = buffer;
    header[0] = ((u64)0x10000000 << 32) | 2;
    header[1] = 14;
    header[2] = 68;
    header[3] = 66;
    header[4] = 0x5200D;
    header[5] = 71;
    sceVif1PkAddDirectDataN(packet, buffer, 3);
    words = (int *)buffer + 4;
    words[0] = circle->color[0];
    words[1] = circle->color[1];
    words[2] = circle->color[2];
    words[3] = circle->color[3];
    words = (int *)buffer + 8;
    words[0] = circle->x * 16 + 0x6FF8;
    words[1] = circle->y * 16 + 0x71F8;
    words[2] = circle->depth;
    words[3] = 0;
    words = (int *)buffer + 12;
    count = 2;
    for (i = 0; i < circle->pointCount; i++) {
        angle = 6.2831855f / ((float)circle->pointCount - 1.0f) * (float)i;
        words[0] = circle->pointColor[0];
        words[1] = circle->pointColor[1];
        words[2] = circle->pointColor[2];
        words[3] = circle->pointColor[3];
        words += 4;
        count++;
        words[0] = (unsigned int)(((float)(circle->x + 0x700) + xglSin(angle) * (float)circle->radius) * 16.0f - 8.0f);
        words[1] = (unsigned int)(((float)(circle->y + 0x720) + xglCos(angle) * (float)circle->radius) * 16.0f - 8.0f);
        words[2] = circle->depth;
        words[3] = 0;
        words += 4;
        count++;
    }
    words = (int *)buffer;
    words[0] = 0x8000 + count / 2;
    words[1] = 0x2026C000;
    words[2] = 0x51;
    words[3] = 0;
    sceVif1PkAddDirectDataN(packet, buffer, count + 1);
}

void endPrintDirectCircle(int circleAddress) {
    XglPacket *vif1Packet;

    vif1Packet = xglPacketGetCurrent();
    sceVif1PkCnt(vif1Packet, 0);
    sceVif1PkOpenDirectHLCode(vif1Packet, 0);
    PrintCircleCore(vif1Packet, SCRATCHPAD_BASE, (PrintCircleInfo *)circleAddress);
    sceVif1PkCloseDirectHLCode(vif1Packet);
}

static void PrintCircle(EndPrintContext *context, int circleAddress)
{
    PrintCircleCore(context->packet, context->scratch, (PrintCircleInfo *)circleAddress);
}

static void FontTexReload(EndPrintContext *context)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64) 0x10000000 << 32) | 0x8000;
    scratch[1] = 0xE;
    scratch[3] = 0x3F;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 1);
    xglFontReloadTexture(context, 2);
}

static void FontTexChange(EndPrintContext *context, WindowTexDirectory **window)
{
    u64 *scratch;
    int i;
    WindowTexDirectory *directory;
    WindowTexEntry *entry;
    unsigned int image;
    unsigned int imageLow;

    i = 0;
    scratch = context->scratch;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8000;
    scratch[1] = 0xE;
    scratch[3] = 0x3F;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 1);
    sceVif1PkCloseDirectHLCode(context->packet);
    directory = *window;
    entry = (WindowTexEntry *)((unsigned char *)directory + directory->entryOffset);
    for (; i < directory->entryCount; i++, entry++) {
        image = entry->bufferWidthAndFormat;
        imageLow = image & 0xFFFF;
        Env_3[4] = ((u64)((entry->vramAddress + 0xF0000) >> 6) << 32) |
                   ((u64)(image >> 16) << 48);
        Env_3[8] = imageLow | ((u64)entry->clutAddress << 32);
        sceVif1PkCnt(context->packet, 0);
        sceVif1PkAddDataN(context->packet, Env_3, 24);
        sceVif1PkRef(context->packet, (unsigned char *)directory + entry->dataOffset, entry->size, 0, 0, 0);
    }
    sceVif1PkCnt(context->packet, 0);
    sceVif1PkOpenDirectHLCode(context->packet, 0);
}

static void DrawBackSet(DrawBackContext *context)
{
    u64 *scratch;
    unsigned int draw_back_control;

    scratch = context->scratch;
    draw_back_control = sRender.draw_back_value;
    draw_back_control |= 0x80000;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8003;
    scratch[1] = 14;
    scratch[3] = 63;
    scratch[4] = (u64)0x8000 << 17;
    scratch[5] = 78;
    scratch[6] = draw_back_control;
    scratch[7] = 76;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 4);
}

static void DrawBackReset(DrawBackContext *context)
{
    u64 *scratch;
    unsigned int draw_back_control;

    scratch = context->scratch;
    draw_back_control = sRender.draw_back_reset_value;
    draw_back_control |= 0x80000;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8003;
    scratch[1] = 14;
    scratch[3] = 63;
    scratch[4] = (u64)0x3100 << 16;
    scratch[5] = 78;
    scratch[6] = draw_back_control;
    scratch[7] = 76;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 4);
}

static void ScreenClear(EndPrintContext *context)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64)0x40AB4000 << 32) | 0x8001;
    scratch[1] = 0x551E;
    scratch[2] = 0x31001;
    scratch[3] = 0x47;
    scratch[4] = 0;
    scratch[5] = 0;
    scratch = context->scratch + 6;
    ((int *)scratch)[0] = 0x6FF8;
    ((int *)scratch)[1] = 0x71F8;
    ((int *)scratch)[2] = 0;
    ((int *)scratch)[3] = 0;
    ((int *)scratch)[4] = 0x8FF8;
    ((int *)scratch)[5] = 0x8DF8;
    ((int *)scratch)[6] = 0;
    ((int *)scratch)[7] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->scratch, 5);
}

static void ZScissor(EndPrintContext *context, PrintRect *rect)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64)0x40834000 << 32) | 0x8001;
    scratch[1] = 0x551E;
    scratch[2] = 0x32001;
    scratch[3] = 0x47;
    scratch[4] = 0;
    scratch[5] = 0;
    scratch = context->scratch + 6;
    ((int *)scratch)[0] = rect->x * 16 + 0x6FF8;
    ((int *)scratch)[1] = rect->y * 16 + 0x71F8;
    ((int *)scratch)[2] = rect->depth;
    ((int *)scratch)[3] = 0;
    ((int *)scratch)[4] = (rect->x + rect->width) * 16 + 0x6FF8;
    ((int *)scratch)[5] = (rect->y + rect->height) * 16 + 0x71F8;
    ((int *)scratch)[6] = rect->depth;
    ((int *)scratch)[7] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->scratch, 5);
}

static void PrintBackSprite(EndPrintContext *context, PrintBackQuad *quad)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64)0x10000000 << 32) | 5;
    scratch[1] = 14;
    scratch[2] = 68;
    scratch[3] = 66;
    scratch[4] = 0x7FDFF0;
    scratch[5] = 8;
    scratch[6] = 96;
    scratch[7] = 20;
    scratch[8] = (0x24020000 | (sRender.draw_back_value << 5)) | (((u64)0x20000006 << 32) | 0x40000000);
    scratch[9] = 6;
    scratch[10] = 0x30000;
    scratch[11] = 71;
    sceVif1PkAddDirectDataN(context->packet, scratch, 6);
    {
        short order[8] = { 0, 1, 0, 3, 2, 1, 2, 3 };
        int *words;
        float *texture;
        PrintBackQuadVertex *vertex;
        int i;
        int gray;

        ((int *)scratch)[0] = 0x8004;
        ((int *)scratch)[1] = 0x302A4000;
        ((int *)scratch)[2] = 0x521;
        ((int *)scratch)[3] = 0;
        words = (int *)context->scratch + 4;
        gray = 128;
        vertex = quad->vertex;
        for (i = 0; i < 4; i++) {
            words[3] = gray;
            words[2] = gray;
            words[1] = gray;
            words[0] = gray;
            words += 4;
            texture = (float *)words;
            texture[0] = quad->uv[order[i * 2]];
            texture[1] = quad->uv[order[i * 2 + 1]];
            texture[2] = 1.0f;
            words[3] = 0;
            words += 4;
            words[0] = vertex->x;
            words[1] = vertex->y;
            words[2] = vertex->z;
            words[3] = 0;
            words += 4;
            vertex++;
        }
    }
    sceVif1PkAddDirectDataN(context->packet, scratch, 13);
}

static void PrintBackSprite2(EndPrintContext *context, PrintBackSpriteInfo *sprite)
{
    u64 *scratch;
    int *words;

    scratch = context->scratch;
    scratch[0] = ((u64)0x10000000 << 32) | 5;
    scratch[1] = 14;
    scratch[2] = 68;
    scratch[3] = 66;
    scratch[4] = 0x7FDFF0;
    scratch[5] = 8;
    scratch[6] = 96;
    scratch[7] = 20;
    scratch[8] = (0x24020000 | (sRender.draw_back_value << 5)) | (((u64)0x20000006 << 32) | 0x40000000);
    scratch[9] = 6;
    scratch[10] = 0x3000D;
    scratch[11] = 71;
    sceVif1PkAddDirectDataN(context->packet, scratch, 6);
    ((int *)scratch)[0] = 0x8001;
    ((int *)scratch)[1] = 0x50AB4000;
    ((int *)scratch)[2] = 0x53531;
    ((int *)scratch)[3] = 0;
    words = (int *)context->scratch + 4;
    words[0] = sprite->color[0];
    words[1] = sprite->color[1];
    words[2] = sprite->color[2];
    words[3] = sprite->color[3];
    words = (int *)context->scratch + 8;
    words[0] = 0;
    words[1] = 0;
    words[2] = 0;
    words[3] = 0;
    words = (int *)context->scratch + 12;
    words[0] = sprite->x * 16 + 0x6FF8;
    words[1] = sprite->y * 16 + 0x71F8;
    words[2] = sprite->depth;
    words[3] = 0;
    words = (int *)context->scratch + 16;
    words[0] = sprite->u * 16;
    words[1] = sprite->v * 16;
    words[2] = 0;
    words[3] = 0;
    words = (int *)context->scratch + 20;
    words[0] = (sprite->x + sprite->width) * 16 + 0x6FF8;
    words[1] = (sprite->y + sprite->height) * 16 + 0x71F8;
    words[2] = sprite->depth;
    words[3] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->scratch, 6);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintEtherLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintExtFunc);

void endPrintDirectFrameCopy(XglPacket *packet, int tagLow, int tagHigh)
{
    /* This 12-qword VIF/GIF template is retained across calls; element 4 is
     * patched with the per-copy tag below. */
    static u64 FrameCopyEnv[12] = {
        0x0000000000000000ULL,
        0x5100000511000000ULL,
        0x1000000000008004ULL,
        0x000000000000000EULL,
        0x0000000000000000ULL,
        0x0000000000000050ULL,
        0x0000000000000000ULL,
        0x0000000000000051ULL,
        0x000001C000000200ULL,
        0x0000000000000052ULL,
        0x0000000000000002ULL,
        0x0000000000000053ULL,
    };

    if (packet != NULL) {
        FrameCopyEnv[4] = (u64) (u32) (tagLow | 0x80000) |
                          ((u64) tagHigh << 0x20) |
                          ((u64) 0x8000 << 0x24);
        sceVif1PkRef(packet, FrameCopyEnv, 6, 0, 0, 0);
    }
}

void endPrintDirectLine(PrintLinePoint *line)
{
    XglPacket *packet;
    u64 *header;
    int *words;
    PrintLinePoint *point;
    int count;
    int qwords;

    packet = xglPacketGetCurrent();
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    header = SCRATCHPAD_BASE;
    header[0] = ((u64)0x10000000 << 32) | 3;
    header[1] = 14;
    header[2] = 0;
    header[3] = 0x3F;
    header[4] = 0x44;
    header[5] = 0x42;
    header[6] = 0x31001;
    header[7] = 0x47;
    sceVif1PkAddDirectDataN(packet, SCRATCHPAD_BASE, 4);
    point = line;
    count = 0;
    if ((point++)->z != 0) {
        do {
            count++;
        } while ((point++)->z != 0);
    }
    ((int *)SCRATCHPAD_BASE)[0] = 0x8000 + count;
    ((int *)SCRATCHPAD_BASE)[1] = 0x20654000;
    ((int *)SCRATCHPAD_BASE)[2] = 0x51;
    ((int *)SCRATCHPAD_BASE)[3] = 0;
    point = line;
    words = (int *)SCRATCHPAD_BASE + 4;
    qwords = 1;
    while (count > 0) {
        count--;
        qwords += 2;
        words[0] = point->color[0];
        words[1] = point->color[1];
        words[2] = point->color[2];
        words[3] = point->color[3];
        words += 4;
        words[0] = point->x;
        words[1] = point->y;
        words[2] = point->z;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(packet, SCRATCHPAD_BASE, qwords);
    sceVif1PkCloseDirectHLCode(packet);
}

void endDecodeJpeg(EndDecodeJpegContext *context) {
    int source;
    int destination;

    source = context->source;
    destination = context->destination;
    context->width = 0x200;
    context->height = 0x1C0;
    context->decodeSource = source;
    context->decodeDestination = destination;
    context->decodeParam[0] = 0;
    context->decodeParam[1] = 0;
    context->decodeParam[2] = 0;
    context->decodeParam[3] = 0;
    xglJpegDecode(&context->decodeSource);
}

void endPrintJpeg(PrintJpegInfo *jpeg)
{
    XglPacket *packet;
    int *topWords;
    int *bottomWords;

    packet = xglPacketGetCurrent();
    sceVif1PkRef(packet, sJpegEnv00_7, 8, 0, 0, 0);
    sceVif1PkRef(packet, jpeg->image, 28672, 0, 0, 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    topWords = (int *)JpegEnv01_8;
    topWords[28] = (jpeg->x + jpeg->offsetX) * 16 + 0x6FF8;
    topWords[29] = (jpeg->y + jpeg->offsetY) * 16 + 0x71F8;
    topWords[30] = jpeg->depth;
    topWords[36] = (jpeg->x + jpeg->width + jpeg->offsetX) * 16 + 0x6FF8;
    topWords[37] = (jpeg->y + jpeg->height / 2 + jpeg->offsetY) * 16 + 0x71F8;
    topWords[38] = jpeg->depth;
    sceVif1PkAddDirectDataN(packet, JpegEnv01_8, 10);
    sceVif1PkCloseDirectHLCode(packet);
    sceVif1PkRef(packet, JpegEnv10_9, 8, 0, 0, 0);
    sceVif1PkRef(packet, (const unsigned char *)jpeg->image + 0x70000, 28672, 0, 0, 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    bottomWords = (int *)JpegEnv11_10;
    bottomWords[28] = (jpeg->x + jpeg->offsetX) * 16 + 0x6FF8;
    bottomWords[29] = (jpeg->y + jpeg->height / 2 + jpeg->offsetY) * 16 + 0x71F8;
    bottomWords[30] = jpeg->depth;
    bottomWords[36] = (jpeg->x + jpeg->width + jpeg->offsetX) * 16 + 0x6FF8;
    bottomWords[37] = (jpeg->y + jpeg->height + jpeg->offsetY) * 16 + 0x71F8;
    bottomWords[38] = jpeg->depth;
    sceVif1PkAddDirectDataN(packet, JpegEnv11_10, 10);
    sceVif1PkCloseDirectHLCode(packet);
}
