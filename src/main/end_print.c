#include "common.h"
#include "shared.h"
#include "main/end_print.h"
#include "end_print.h"
#include "main/xgl_jpeg.h"
#include "main/xgl_packet.h"
#include "main/xgl_2.h"
#include "main/xgl_prim_add.h"

#define NULL ((void *)0)

#define SPRITE_UV(u, v, width, height) { (u), (v), (width), (height) }

/* EE scratchpad RAM base, used for VIF1 direct-mode packets. */
#define END_PRINT_SCRATCHPAD ((void *)SCRATCHPAD_BASE)

/* Size of the screen the print routines clip to. */
#define END_PRINT_SCREEN_WIDTH 512
#define END_PRINT_SCREEN_HEIGHT 448

/* GS primitive coordinates are 12.4 fixed point; screen (0, 0) sits at
 * (0x700, 0x720) minus half a pixel. */
#define END_PRINT_GS_ORIGIN_X 0x700
#define END_PRINT_GS_ORIGIN_Y 0x720
#define END_PRINT_GS_OFFSET_X (END_PRINT_GS_ORIGIN_X * 16 - 8)
#define END_PRINT_GS_OFFSET_Y (END_PRINT_GS_ORIGIN_Y * 16 - 8)

/* GS Z buffer values are 24 bits. */
#define END_PRINT_DEPTH_MASK 0xFFFFFF

/* Window frame: 3 pixel border pieces (win_uv[0..7]) around interior tiles of
 * up to 128 pixels (win_uv[8]). */
#define WINDOW_BORDER 3
#define WINDOW_BORDER_BOTH_SIDES (WINDOW_BORDER * 2)
#define WINDOW_TILE_SIZE 128
#define WINDOW_BORDER_PIECES 8
#define WINDOW_INTERIOR_UV 8

/* GIF tag: low word is NLOOP | EOP, high word is NREG | PRIM | PRE; the third
 * word lists the registers (4 bits each). */
#define GIF_NREG_1 0x10000000
/* PRE set, PRIM = type | IIP/TME/ABE/AA/FST bits as noted. */
#define GIF_HI_SPRITE_TEX_NREG5 0x50AB4000 /* sprite, TME, ABE, FST */
#define GIF_HI_SPRITE_TEX_NREG4 0x40AB4000
#define GIF_HI_SPRITE_PLAIN_NREG4 0x40834000 /* sprite, FST */
#define GIF_HI_TRISTRIP_TEX_NREG3 0x30AE4000 /* tristrip, IIP, TME, ABE, FST */
#define GIF_HI_TRISTRIP_ST_NREG3 0x302A4000 /* tristrip, TME, ABE (ST coordinates) */
#define GIF_HI_TRISTRIP_RIBBON 0x20A64000 /* tristrip, IIP, ABE, FST */
#define GIF_HI_LINESTRIP 0x20654000 /* linestrip, IIP, ABE, AA */
#define GIF_HI_POINT 0x20204000 /* point, ABE */
#define GIF_HI_TRIFAN 0x2026C000 /* trifan, IIP, ABE */
#define GIF_REGS_RGBAQ_XYZ2 0x51
#define GIF_REGS_RGBAQ_UV_XYZ2 0x531
#define GIF_REGS_RGBAQ_ST_XYZ2 0x521
#define GIF_REGS_UV_XYZ2_X2 0x5353
#define GIF_REGS_RGBAQ_UV_XYZ2_UV_XYZ2 0x53531
#define GIF_REGS_AD_RGBAQ_XYZ2_X2 0x551E

/* SCE_GS_FRAME_1 / BITBLTBUF buffer-width field = 8 (512 pixels). */
#define GS_BUFFER_WIDTH_512 0x80000
/* SCE_GS_ZBUF_1: ZMSK, Z buffer writes disabled. */
#define GS_ZBUF_ZMSK (1ULL << 32)

/* SCE_GS_ALPHA_1 value:(Cs - Cd) * As + Cd. */
#define GS_ALPHA_BLEND 0x44
/* SCE_GS_TEST_1 values. */
#define GS_TEST_ANEQ_ZGEQUAL 0x5000D /* alpha test NOTEQUAL, Z test GEQUAL */
#define GS_TEST_ANEQ_ZGEQUAL_AFAIL_ZB 0x5200D /* same, failed pixels still write Z */
#define GS_TEST_ANEQ_ZALWAYS 0x3000D /* alpha test NOTEQUAL, Z test ALWAYS */
#define GS_TEST_ZALWAYS 0x30000 /* alpha test off, Z test ALWAYS */
#define GS_TEST_COLOR_ONLY 0x31001 /* ATST NEVER, failed pixels write color only */
#define GS_TEST_Z_ONLY 0x32001 /* ATST NEVER, failed pixels write Z only */
#define GS_CLAMP_PRINT 0x3FFFF0
#define GS_CLAMP_PRINT_BACK 0x7FDFF0

/* UV-table high byte already shifted: base values of the packed uv word. */
#define TAG_UV_UPPER_A ((END_PRINT_UV_TAG << 8) + 0)
#define TAG_UV_LOWER_A ((END_PRINT_UV_TAG << 8) + 26)
#define NUM_UV_DIGIT_0 ((END_PRINT_UV_NUM << 8) + 0)
#define NUM_UV_SLASH ((END_PRINT_UV_NUM << 8) + 3)
#define NUM_UV_COLON ((END_PRINT_UV_NUM << 8) + 4)
#define NUM_UV_MINUS ((END_PRINT_UV_NUM << 8) + 6)
#define NUM_UV_PERIOD ((END_PRINT_UV_NUM << 8) + 9)

/* PrintNumberInfo.flags: low nibble selects the digit style (added to the
 * NUM table entry), bit 4 right-aligns the number. */
#define PRINT_NUMBER_STYLE_MASK 0xF
#define PRINT_NUMBER_RIGHT_ALIGN 0x10

/* CLUT addresses (uv_clut) passed to endPrintInfoSet by the text/number and window printers. */
#define CLUT_TAG_NUM 0xFDB00
#define CLUT_WINDOW 0xFDE00

/* GS VRAM offset added to a window texture entry's vramAddress. */
#define WINDOW_TEX_VRAM_BASE 0xF0000

/* Tag text: a leading byte 1 right-aligns the string (the loop first measures
 * it), byte 3 is followed by a custom advance in pixels, a space advances 8. */
#define TAG_TEXT_RIGHT_ALIGN 1
#define TAG_TEXT_ADVANCE_CODE 3
#define TAG_TEXT_SPACE_WIDTH 8

typedef struct EndPrintRenderState {
    unsigned char unmodeled_00[4];
    short scissor_width;                 /* +0x04 */
    short scissor_height;                /* +0x06 */
    unsigned char unmodeled_08[0x0c];
    unsigned short draw_back_value;      /* +0x14 */
    unsigned char unmodeled_16[0x0a];
    unsigned short draw_back_reset_value; /* +0x20 */
} EndPrintRenderState;

typedef struct SpriteUv {
    short u;      /* +0x00 */
    short v;      /* +0x02 */
    short width;  /* +0x04 */
    short height; /* +0x06 */
} SpriteUv;

/*
 * uv_clut: a fixed table of UV/CLUT entries; only the CLUT id at +0x04 of
 * each 8-byte entry is read here.
 */
typedef struct UvClutEntry {
    const SpriteUv *uv;            /* +0x00 */
    int clut;                      /* +0x04 */
} UvClutEntry;

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

typedef struct PrintTagFontInfo {
    short x;                  /* +0x00 */
    unsigned short y;         /* +0x02 */
    int depth;                /* +0x04 */
    unsigned char color[4];   /* +0x08..+0x0b */
    unsigned char *text;      /* +0x0c */
} PrintTagFontInfo;

typedef struct PrintNumberInfo {
    short x;                /* +0x00 */
    unsigned short y;       /* +0x02 */
    int depth;              /* +0x04 */
    unsigned char color[4]; /* +0x08..+0x0b */
    int value;              /* +0x0c */
    unsigned short digits;  /* +0x10 */
    unsigned short flags;   /* +0x12 */
} PrintNumberInfo;

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

typedef struct PrintThumbnailInfo {
    short x;                       /* +0x00 */
    short y;                       /* +0x02 */
    int depth;                     /* +0x04 */
    unsigned char unmodeled_08[4]; /* +0x08..+0x0b */
    const void *image;             /* +0x0c */
} PrintThumbnailInfo;

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

typedef struct PrintJpegInfo {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    int depth;                      /* +0x04 */
    short width;                    /* +0x08 */
    short height;                   /* +0x0a */
    short offsetX;                  /* +0x0c */
    short offsetY;                  /* +0x0e */
    unsigned char unmodeled_10[0x24]; /* +0x10..+0x33 */
    const unsigned char *image;     /* +0x34 */
} PrintJpegInfo;

enum { QUAD_X, QUAD_Y, QUAD_DEPTH };

/* PrintSpriteQuad.coordinateMode: vertex 0 plus the UV entry's size, or all four corners given. */
enum { QUAD_MODE_ORIGIN_SIZE, QUAD_MODE_CORNERS };
#define QUAD_FLAG_DEFAULT_COLOR 1

typedef struct PrintSpriteVertex {
    int position[3];
    unsigned char unmodeled_0c[4];
} PrintSpriteVertex;

typedef struct PrintSpriteColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} PrintSpriteColor;

typedef struct PrintSpriteQuad {
    unsigned char flags;
    signed char coordinateMode;
    unsigned short uv;
    unsigned char unmodeled_04[12];
    PrintSpriteVertex vertices[4];
    PrintSpriteColor colors[4];
} PrintSpriteQuad;

typedef struct PrintFrameInfo {
    short x, y;
    int depth;
    short width, height;
    unsigned char color[4];
} PrintFrameInfo;

typedef struct PrintUnsignedFrameInfo {
    unsigned short x, y;
    int depth;
    unsigned short width, height;
    unsigned char color[4];
} PrintUnsignedFrameInfo;

typedef struct EtherLineEndpoint {
    float x;
    float y;
    float depth;
    unsigned char unmodeled_0c[4];
    unsigned char color[4];
    unsigned char unmodeled_14[12];
} EtherLineEndpoint;

extern EndPrintRenderState sRender;
extern void (*func_tbl_4[])(FontDrawCursor *context, int param);
extern FontExtFunc func_tbl2_5[];

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void eMessageSpriteReset(void);
static void endPrintInfoSet(FontDrawCursor *context, int clut, int flag);
static void subPrintSprite(FontDrawCursor *context, PrintSpriteInfo *sprite);
static void subPrintLine(FontDrawCursor *context, PrintPointInfo *line);
static void subPrintRibbon(FontDrawCursor *context, PrintPointInfo *ribbon);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

static void PrintCircleCore(XglPacket *packet, void *buffer, PrintCircleInfo *circle);

extern void endSpriteSet(PrintSpriteInfo *sprite, int flags);
extern WindowTexDirectory *WindowTexAddrGet(int index);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

static const SpriteUv cur_uv[4] = {
    SPRITE_UV(0, 192, 16, 16),
    SPRITE_UV(64, 160, 16, 16),
    SPRITE_UV(0, 176, 16, 16),
    SPRITE_UV(64, 176, 16, 16)
};

static const SpriteUv others_uv[25] = {
    SPRITE_UV(199, 227, 8, 16),
    SPRITE_UV(200, 243, 7, 13),
    SPRITE_UV(16, 192, 18, 12),
    SPRITE_UV(34, 192, 18, 12),
    SPRITE_UV(88, 192, 18, 12),
    SPRITE_UV(70, 192, 18, 12),
    SPRITE_UV(143, 217, 47, 10),
    SPRITE_UV(122, 192, 11, 16),
    SPRITE_UV(188, 243, 10, 13),
    SPRITE_UV(218, 0, 29, 20),
    SPRITE_UV(218, 20, 29, 20),
    SPRITE_UV(209, 194, 43, 62),
    SPRITE_UV(160, 80, 16, 16),
    SPRITE_UV(218, 40, 22, 22),
    SPRITE_UV(218, 62, 22, 22),
    SPRITE_UV(160, 0, 29, 20),
    SPRITE_UV(160, 20, 29, 20),
    SPRITE_UV(160, 40, 29, 20),
    SPRITE_UV(160, 60, 29, 20),
    SPRITE_UV(189, 0, 29, 20),
    SPRITE_UV(189, 20, 29, 20),
    SPRITE_UV(189, 40, 29, 20),
    SPRITE_UV(189, 60, 29, 20),
    SPRITE_UV(0, 192, 16, 16),
    SPRITE_UV(176, 80, 8, 8)
};

static const SpriteUv pad_uv[8] = {
    SPRITE_UV(0, 208, 22, 24),
    SPRITE_UV(22, 208, 22, 24),
    SPRITE_UV(44, 208, 22, 24),
    SPRITE_UV(66, 208, 22, 24),
    SPRITE_UV(198, 132, 29, 20),
    SPRITE_UV(198, 92, 29, 20),
    SPRITE_UV(227, 132, 29, 20),
    SPRITE_UV(227, 92, 29, 20)
};

static const SpriteUv icom_uv[47] = {
    SPRITE_UV(388, 67, 20, 24),
    SPRITE_UV(408, 67, 20, 24),
    SPRITE_UV(428, 67, 20, 24),
    SPRITE_UV(448, 67, 20, 24),
    SPRITE_UV(468, 67, 20, 24),
    SPRITE_UV(488, 67, 20, 24),
    SPRITE_UV(388, 91, 20, 24),
    SPRITE_UV(408, 91, 20, 24),
    SPRITE_UV(428, 91, 20, 24),
    SPRITE_UV(448, 91, 20, 24),
    SPRITE_UV(468, 91, 20, 24),
    SPRITE_UV(488, 91, 20, 24),
    SPRITE_UV(388, 115, 20, 24),
    SPRITE_UV(408, 115, 20, 24),
    SPRITE_UV(428, 115, 20, 24),
    SPRITE_UV(388, 139, 20, 24),
    SPRITE_UV(408, 139, 20, 24),
    SPRITE_UV(428, 139, 20, 24),
    SPRITE_UV(448, 139, 20, 24),
    SPRITE_UV(468, 139, 20, 24),
    SPRITE_UV(488, 139, 20, 24),
    SPRITE_UV(388, 163, 20, 24),
    SPRITE_UV(408, 163, 20, 24),
    SPRITE_UV(428, 163, 20, 24),
    SPRITE_UV(448, 163, 20, 24),
    SPRITE_UV(468, 163, 20, 24),
    SPRITE_UV(488, 163, 20, 24),
    SPRITE_UV(388, 187, 20, 24),
    SPRITE_UV(428, 187, 20, 24),
    SPRITE_UV(408, 187, 20, 24),
    SPRITE_UV(448, 187, 20, 24),
    SPRITE_UV(484, 187, 16, 16),
    SPRITE_UV(468, 187, 16, 16),
    SPRITE_UV(388, 211, 20, 24),
    SPRITE_UV(256, 162, 20, 24),
    SPRITE_UV(276, 162, 20, 24),
    SPRITE_UV(296, 162, 20, 24),
    SPRITE_UV(316, 162, 20, 24),
    SPRITE_UV(336, 162, 20, 24),
    SPRITE_UV(356, 162, 20, 24),
    SPRITE_UV(276, 186, 20, 24),
    SPRITE_UV(296, 186, 20, 24),
    SPRITE_UV(256, 186, 20, 24),
    SPRITE_UV(209, 232, 32, 24),
    SPRITE_UV(448, 115, 20, 24),
    SPRITE_UV(388, 67, 20, 24),
    SPRITE_UV(408, 67, 20, 24)
};

static const SpriteUv face_puti_uv[10] = {
    SPRITE_UV(896, 0, 20, 24),
    SPRITE_UV(916, 0, 20, 24),
    SPRITE_UV(936, 0, 20, 24),
    SPRITE_UV(956, 0, 20, 24),
    SPRITE_UV(896, 24, 20, 24),
    SPRITE_UV(916, 24, 20, 24),
    SPRITE_UV(936, 24, 20, 24),
    SPRITE_UV(976, 24, 20, 24),
    SPRITE_UV(976, 0, 20, 24),
    SPRITE_UV(996, 0, 20, 24)
};

static const SpriteUv tag_uv[52] = {
    SPRITE_UV(253, 0, 10, 16),
    SPRITE_UV(263, 0, 10, 16),
    SPRITE_UV(273, 0, 10, 16),
    SPRITE_UV(283, 0, 10, 16),
    SPRITE_UV(293, 0, 9, 16),
    SPRITE_UV(302, 0, 9, 16),
    SPRITE_UV(311, 0, 10, 16),
    SPRITE_UV(321, 0, 10, 16),
    SPRITE_UV(331, 0, 6, 16),
    SPRITE_UV(337, 0, 8, 16),
    SPRITE_UV(345, 0, 10, 16),
    SPRITE_UV(355, 0, 9, 16),
    SPRITE_UV(364, 0, 13, 16),
    SPRITE_UV(377, 0, 12, 16),
    SPRITE_UV(389, 0, 10, 16),
    SPRITE_UV(399, 0, 9, 16),
    SPRITE_UV(408, 0, 10, 16),
    SPRITE_UV(418, 0, 10, 16),
    SPRITE_UV(428, 0, 10, 16),
    SPRITE_UV(438, 0, 10, 16),
    SPRITE_UV(448, 0, 10, 16),
    SPRITE_UV(458, 0, 10, 16),
    SPRITE_UV(468, 0, 13, 16),
    SPRITE_UV(481, 0, 10, 16),
    SPRITE_UV(491, 0, 10, 16),
    SPRITE_UV(501, 0, 11, 16),
    SPRITE_UV(253, 16, 8, 11),
    SPRITE_UV(261, 16, 8, 11),
    SPRITE_UV(269, 16, 8, 11),
    SPRITE_UV(277, 16, 8, 11),
    SPRITE_UV(285, 16, 7, 11),
    SPRITE_UV(292, 16, 7, 11),
    SPRITE_UV(299, 16, 8, 11),
    SPRITE_UV(307, 16, 8, 11),
    SPRITE_UV(315, 16, 5, 11),
    SPRITE_UV(320, 16, 6, 11),
    SPRITE_UV(326, 16, 8, 11),
    SPRITE_UV(334, 16, 7, 11),
    SPRITE_UV(341, 16, 10, 11),
    SPRITE_UV(351, 16, 9, 11),
    SPRITE_UV(360, 16, 8, 11),
    SPRITE_UV(368, 16, 8, 11),
    SPRITE_UV(376, 16, 8, 11),
    SPRITE_UV(384, 16, 8, 11),
    SPRITE_UV(392, 16, 8, 11),
    SPRITE_UV(400, 16, 7, 11),
    SPRITE_UV(407, 16, 8, 11),
    SPRITE_UV(415, 16, 9, 11),
    SPRITE_UV(424, 16, 11, 11),
    SPRITE_UV(435, 16, 9, 11),
    SPRITE_UV(444, 16, 8, 11),
    SPRITE_UV(452, 16, 8, 11)
};

static const SpriteUv win_uv[12] = {
    SPRITE_UV(141, 141, 3, 3),
    SPRITE_UV(0, 141, 128, 3),
    SPRITE_UV(144, 141, 3, 3),
    SPRITE_UV(141, 0, 3, 128),
    SPRITE_UV(144, 0, 3, 128),
    SPRITE_UV(141, 144, 3, 3),
    SPRITE_UV(0, 144, 128, 3),
    SPRITE_UV(144, 144, 3, 3),
    SPRITE_UV(0, 0, 128, 128),
    SPRITE_UV(154, 208, 44, 19),
    SPRITE_UV(316, 187, 20, 24),
    SPRITE_UV(512, 186, 165, 24)
};

static const SpriteUv num_uv[10] = {
    SPRITE_UV(88, 227, 11, 16),
    SPRITE_UV(88, 243, 10, 13),
    SPRITE_UV(199, 227, 8, 16),
    SPRITE_UV(200, 243, 7, 13),
    SPRITE_UV(188, 243, 10, 13),
    SPRITE_UV(88, 211, 11, 16),
    SPRITE_UV(99, 211, 11, 16),
    SPRITE_UV(121, 211, 11, 16),
    SPRITE_UV(132, 211, 11, 16),
    SPRITE_UV(143, 211, 4, 4)
};

static const SpriteUv pad2_uv[22] = {
    SPRITE_UV(256, 67, 22, 24),
    SPRITE_UV(278, 67, 22, 24),
    SPRITE_UV(300, 67, 22, 24),
    SPRITE_UV(322, 67, 22, 24),
    SPRITE_UV(344, 67, 22, 24),
    SPRITE_UV(366, 67, 22, 24),
    SPRITE_UV(256, 91, 22, 24),
    SPRITE_UV(278, 91, 22, 24),
    SPRITE_UV(300, 91, 22, 24),
    SPRITE_UV(322, 91, 22, 24),
    SPRITE_UV(344, 91, 22, 24),
    SPRITE_UV(366, 91, 22, 24),
    SPRITE_UV(256, 115, 22, 24),
    SPRITE_UV(278, 115, 22, 24),
    SPRITE_UV(300, 115, 22, 24),
    SPRITE_UV(322, 115, 22, 24),
    SPRITE_UV(344, 115, 22, 24),
    SPRITE_UV(366, 115, 22, 24),
    SPRITE_UV(256, 139, 22, 24),
    SPRITE_UV(278, 139, 22, 24),
    SPRITE_UV(300, 139, 22, 24),
    SPRITE_UV(322, 139, 22, 24)
};

static const SpriteUv face_agws_uv[14] = {
    SPRITE_UV(768, 93, 63, 48),
    SPRITE_UV(832, 93, 63, 48),
    SPRITE_UV(896, 93, 63, 48),
    SPRITE_UV(960, 93, 63, 48),
    SPRITE_UV(768, 141, 63, 48),
    SPRITE_UV(832, 141, 63, 48),
    SPRITE_UV(896, 141, 63, 48),
    SPRITE_UV(960, 141, 63, 48),
    SPRITE_UV(768, 93, 63, 32),
    SPRITE_UV(832, 93, 63, 32),
    SPRITE_UV(896, 93, 63, 32),
    SPRITE_UV(960, 93, 63, 32),
    SPRITE_UV(768, 141, 63, 32),
    SPRITE_UV(832, 141, 63, 32)
};

static const SpriteUv others2_uv[3] = {
    SPRITE_UV(0, 208, 384, 48),
    SPRITE_UV(0, 201, 500, 7),
    SPRITE_UV(230, 56, 10, 72)
};

static const SpriteUv face00_uv[2] = {
    SPRITE_UV(512, 0, 64, 93),
    SPRITE_UV(576, 0, 64, 93)
};

static const SpriteUv face01_uv[2] = {
    SPRITE_UV(640, 0, 64, 93),
    SPRITE_UV(704, 0, 64, 93)
};

static const SpriteUv face02_uv[2] = {
    SPRITE_UV(512, 93, 64, 93),
    SPRITE_UV(576, 93, 64, 93)
};

static const SpriteUv face03_uv[2] = {
    SPRITE_UV(640, 93, 64, 93),
    SPRITE_UV(704, 93, 64, 93)
};

static const SpriteUv face04_uv[2] = {
    SPRITE_UV(768, 0, 64, 93),
    SPRITE_UV(832, 0, 64, 93)
};

static const SpriteUv ether_uv[5] = {
    SPRITE_UV(0, 0, 38, 48),
    SPRITE_UV(38, 0, 38, 48),
    SPRITE_UV(76, 0, 38, 48),
    SPRITE_UV(160, 0, 13, 16),
    SPRITE_UV(114, 0, 46, 56)
};

static const SpriteUv seg_uv[24] = {
    SPRITE_UV(0, 48, 72, 58),
    SPRITE_UV(0, 106, 72, 58),
    SPRITE_UV(72, 56, 46, 57),
    SPRITE_UV(160, 16, 10, 22),
    SPRITE_UV(173, 0, 28, 25),
    SPRITE_UV(221, 0, 20, 24),
    SPRITE_UV(201, 0, 20, 24),
    SPRITE_UV(72, 56, 46, 57),
    SPRITE_UV(118, 56, 42, 53),
    SPRITE_UV(160, 56, 18, 61),
    SPRITE_UV(178, 56, 14, 57),
    SPRITE_UV(192, 56, 21, 72),
    SPRITE_UV(213, 56, 17, 68),
    SPRITE_UV(72, 113, 34, 34),
    SPRITE_UV(106, 113, 30, 30),
    SPRITE_UV(0, 183, 47, 9),
    SPRITE_UV(47, 183, 47, 9),
    SPRITE_UV(94, 183, 43, 9),
    SPRITE_UV(0, 192, 47, 9),
    SPRITE_UV(47, 192, 47, 9),
    SPRITE_UV(94, 192, 43, 9),
    SPRITE_UV(256, 0, 88, 169),
    SPRITE_UV(512, 0, 240, 48),
    SPRITE_UV(512, 48, 240, 24)
};

static const SpriteUv umn_uv[12] = {
    SPRITE_UV(0, 0, 102, 128),
    SPRITE_UV(102, 0, 102, 128),
    SPRITE_UV(0, 128, 102, 128),
    SPRITE_UV(102, 128, 102, 128),
    SPRITE_UV(256, 48, 20, 24),
    SPRITE_UV(276, 48, 20, 24),
    SPRITE_UV(296, 48, 20, 24),
    SPRITE_UV(316, 48, 20, 24),
    SPRITE_UV(256, 0, 38, 48),
    SPRITE_UV(294, 0, 38, 48),
    SPRITE_UV(332, 0, 38, 48),
    SPRITE_UV(512, 0, 511, 96)
};

static const SpriteUv btl_msg_uv[23] = {
    SPRITE_UV(256, 47, 22, 24),
    SPRITE_UV(278, 47, 22, 24),
    SPRITE_UV(300, 47, 22, 24),
    SPRITE_UV(322, 47, 22, 24),
    SPRITE_UV(344, 47, 22, 24),
    SPRITE_UV(256, 71, 22, 24),
    SPRITE_UV(278, 71, 22, 24),
    SPRITE_UV(300, 71, 22, 24),
    SPRITE_UV(322, 71, 22, 24),
    SPRITE_UV(344, 71, 22, 24),
    SPRITE_UV(366, 71, 22, 24),
    SPRITE_UV(256, 95, 22, 24),
    SPRITE_UV(278, 95, 22, 24),
    SPRITE_UV(300, 95, 22, 24),
    SPRITE_UV(322, 95, 22, 24),
    SPRITE_UV(344, 95, 22, 24),
    SPRITE_UV(366, 95, 22, 24),
    SPRITE_UV(256, 119, 22, 24),
    SPRITE_UV(278, 119, 22, 24),
    SPRITE_UV(300, 119, 22, 24),
    SPRITE_UV(322, 119, 22, 24),
    SPRITE_UV(344, 119, 22, 24),
    SPRITE_UV(366, 119, 22, 24)
};

static const SpriteUv btl_msg_uv2[8] = {
    SPRITE_UV(353, 143, 22, 24),
    SPRITE_UV(375, 143, 22, 24),
    SPRITE_UV(397, 143, 22, 24),
    SPRITE_UV(419, 143, 22, 24),
    SPRITE_UV(281, 143, 18, 46),
    SPRITE_UV(299, 143, 18, 46),
    SPRITE_UV(317, 143, 18, 46),
    SPRITE_UV(335, 143, 18, 46)
};

static const SpriteUv btl_msg_uv3[6] = {
    SPRITE_UV(160, 0, 20, 24),
    SPRITE_UV(180, 0, 20, 24),
    SPRITE_UV(200, 0, 20, 24),
    SPRITE_UV(160, 24, 9, 10),
    SPRITE_UV(169, 24, 13, 10),
    SPRITE_UV(182, 24, 11, 10)
};

static const SpriteUv umn2_uv[4] = {
    SPRITE_UV(370, 0, 38, 40),
    SPRITE_UV(408, 0, 40, 40),
    SPRITE_UV(370, 40, 38, 40),
    SPRITE_UV(408, 40, 38, 40)
};

static const SpriteUv umn3_uv[4] = {
    SPRITE_UV(448, 0, 22, 26),
    SPRITE_UV(470, 0, 22, 26),
    SPRITE_UV(448, 26, 22, 26),
    SPRITE_UV(470, 26, 22, 26)
};

static const SpriteUv name_uv[1] = {
    SPRITE_UV(256, 210, 105, 24)
};

static const SpriteUv thumnail_uv[1] = {
    SPRITE_UV(0, 0, 128, 112)
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

/* 511 callback entries plus one terminal entry. */
#define PRINT_FUNC_MAX 511
static PrintFuncEntry PrintFunc[PRINT_FUNC_MAX + 1];

static PrintFuncEntry *pPrintFuncTop;

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static u64 ePrintEnv_0[12];

static u64 ThumImageEnv_1[12];

static u64 ThumDrawEnv_2[24];

static u64 Env_3[12];

static u64 sJpegEnv00_7[16];

static u64 JpegEnv01_8[20];

static u64 JpegEnv10_9[16];

static u64 JpegEnv11_10[20];

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

/* Sends each window texture of the directory at WindowTexAddrGet(0) to packet: a GS state block, then the data. */
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
        ePrintEnv_0[4] = ((u64)((entry->vramAddress + WINDOW_TEX_VRAM_BASE) >> 6) << 32) |
                         ((u64)(image >> 16) << 48);
        ePrintEnv_0[8] = imageLow | ((u64)entry->clutAddress << 32);
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, ePrintEnv_0, 24);
        sceVif1PkRef(packet, (unsigned char *)directory + entry->dataOffset, entry->size, 0, 0, 0);
    }
}

/* Runs every queued print callback, then resets the queue and the message sprites. */
static void PrintFlush(FontDrawCursor *context)
{
    PrintFuncEntry *cursor;
    void (*func)(FontDrawCursor *context, int param);
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

/* Appends a deferred print callback and its parameter to the queue; resets the queue when it overflows. */
void endPrintExtFuncPack(void (*func)(FontDrawCursor *context, int param), int param)
{
    PrintFuncEntry *entry;

    entry = pPrintFuncTop;
    entry->func = func;
    entry->param = param;
    entry++;
    pPrintFuncTop = entry;
    if ((PrintFunc + PRINT_FUNC_MAX) < entry) {
        endPrintInit();
        entry = pPrintFuncTop;
    }
    entry->func = NULL;
}

/* Emits the GS state packet (texture, alpha, clamp and test) for a print; bit 0 of flag selects the CLUT texture. */
static void endPrintInfoSet(FontDrawCursor *context, int clut, int flag)
{
    u64 *command;
    int count;
    unsigned short useClut;

    command = context->work.qwords + 2;
    command[1] = SCE_GS_TEXFLUSH;
    command[0] = 0;
    command = context->work.qwords + 4;
    command[0] = GS_ALPHA_BLEND;
    command[1] = SCE_GS_ALPHA_1;
    command = context->work.qwords + 6;
    command[0] = GS_CLAMP_PRINT;
    command[1] = SCE_GS_CLAMP_1;
    command = context->work.qwords + 8;
    count = 3;
    useClut = flag & 1;
    if (useClut) {
        if (clut != 0) {
            command[0] = ((u64)((unsigned int)clut >> 6) << 37) | 0x2000000629343C00ULL;
        } else {
            command[0] = 0x00000005DC00BC00ULL;
        }
        command[1] = SCE_GS_TEX0_1;
        count = 4;
        command += 2;
    }
    command[0] = GS_TEST_ANEQ_ZGEQUAL;
    count++;
    command[1] = SCE_GS_TEST_1;
    command = context->work.qwords;
    command[0] = count + ((u64)GIF_NREG_1 << 32);
    command[1] = GIF_REG_AD;
    xglPrimAddGifTagDirect(context->packet, command, count + 1);
}

/* Fills the size, color and/or cell fields of a sprite, as selected by flags. */
void endSpriteSet(PrintSpriteInfo *sprite, int flags)
{
    unsigned int entry;
    const SpriteUv *table;

    entry = ((const unsigned char *)&sprite->uv)[0];
    table = uv_clut[((const unsigned char *)&sprite->uv)[1]].uv;
    if (flags & END_SPRITE_SET_SIZE) {
        sprite->width = table[entry].width;
        sprite->height = table[entry].height;
    }
    if (flags & END_SPRITE_SET_COLOR) {
        sprite->color[0] = sprite->color[1] = sprite->color[2] = sprite->color[3] = END_PRINT_COLOR_NEUTRAL;
    }
    if (flags & END_SPRITE_SET_CELL) {
        sprite->row = sprite->column = 0;
    }
}

/* Emits one textured sprite's GIF packet from its uv cell, position, size and depth. */
static void subPrintSprite(FontDrawCursor *context, PrintSpriteInfo *sprite)
{
    int *words;
    const SpriteUv *table;
    int uvLeft;
    int uvTop;
    int width;
    int height;
    int left;
    int top;

    words = context->work.words;
    words[0] = GIF_TAG_EOP | 1;
    words[1] = GIF_HI_SPRITE_TEX_NREG5;
    words[2] = GIF_REGS_RGBAQ_UV_XYZ2_UV_XYZ2;
    words[3] = 0;
    table = uv_clut[(sprite->uv >> 8) & 0xFF].uv;
    uvLeft = table[sprite->uv & 0xFF].u;
    uvTop = table[sprite->uv & 0xFF].v;
    width = table[sprite->uv & 0xFF].width;
    height = table[sprite->uv & 0xFF].height;
    words = context->work.words + 4;
    words[0] = sprite->color[0];
    words[1] = sprite->color[1];
    words[2] = sprite->color[2];
    words[3] = sprite->color[3];
    words = context->work.words + 8;
    left = sprite->x * 16 + END_PRINT_GS_OFFSET_X;
    top = sprite->y * 16 + END_PRINT_GS_OFFSET_Y;
    words[0] = (uvLeft + width * sprite->column) * 16;
    words[1] = (uvTop + height * sprite->row) * 16;
    words[2] = 0;
    words[3] = 0;
    words[4] = left;
    words[5] = top;
    words[6] = sprite->depth & END_PRINT_DEPTH_MASK;
    words[7] = 0;
    words[8] = words[0] + width * 16;
    words[9] = words[1] + height * 16;
    words[10] = 0;
    words[11] = 0;
    words[12] = left + sprite->width * 16;
    words[13] = top + sprite->height * 16;
    words[14] = words[6];
    words[15] = 0;
    xglPrimAddGifTagDirect(context->packet, context->work.qwords, 6);
}

/* The high byte of the packed UV word selects the CLUT on little-endian EE. */
static void PrintSprite00(FontDrawCursor *context, PrintSpriteInfo *sprite)
{
    endPrintInfoSet(context, uv_clut[((const unsigned char *)&sprite->uv)[1]].clut, 1);
    subPrintSprite(context, sprite);
}

/*
 * Emits a textured quad from four vertices. In origin-and-size mode the far
 * corners are derived first; both modes then convert positions to GS coordinates.
 */
static void PrintSprite01(FontDrawCursor *context, PrintSpriteQuad *sprite)
{
    int cornerU[4];
    int cornerV[4];
    const SpriteUv *table;
    unsigned int entry;
    int *words;
    int i;
    unsigned char useDefaultColor;

    entry = sprite->uv & 255;
    table = uv_clut[sprite->uv >> 8].uv;
    endPrintInfoSet(context, uv_clut[sprite->uv >> 8].clut, 1);
    switch (sprite->coordinateMode) {
    case QUAD_MODE_ORIGIN_SIZE: {
        const int left = sprite->vertices[0].position[QUAD_X];
        const int top = sprite->vertices[0].position[QUAD_Y];
        const int quadDepth = sprite->vertices[0].position[QUAD_DEPTH];
        sprite->vertices[1].position[QUAD_X] = left;
        sprite->vertices[3].position[QUAD_X] = left + table[entry].width;
        sprite->vertices[2].position[QUAD_Y] = top;
        sprite->vertices[3].position[QUAD_Y] = top + table[entry].height;
        sprite->vertices[3].position[QUAD_DEPTH] = quadDepth;
        sprite->vertices[2].position[QUAD_DEPTH] = quadDepth;
        sprite->vertices[1].position[QUAD_Y] = top + table[entry].height;
        sprite->vertices[2].position[QUAD_X] = left + table[entry].width;
        sprite->vertices[1].position[QUAD_DEPTH] = quadDepth;
    }
    case QUAD_MODE_CORNERS:
        useDefaultColor = sprite->flags & QUAD_FLAG_DEFAULT_COLOR;
        if (useDefaultColor) {
            for (i = 0; i < 4; i++) {
                sprite->colors[i].red = sprite->colors[i].green =
                    sprite->colors[i].blue = sprite->colors[i].alpha = END_PRINT_COLOR_NEUTRAL;
            }
        }
        for (i = 0; i < 4; i++) {
            sprite->vertices[i].position[QUAD_X] = sprite->vertices[i].position[QUAD_X] * 16 + END_PRINT_GS_OFFSET_X;
            sprite->vertices[i].position[QUAD_Y] = sprite->vertices[i].position[QUAD_Y] * 16 + END_PRINT_GS_OFFSET_Y;
        }
        break;
    }
    /* Texture corners in tristrip order: (u, v), (u, v + h), (u + w, v), (u + w, v + h). */
    cornerU[0] = table[entry].u * 16;
    cornerU[1] = cornerU[0];
    cornerU[3] = (table[entry].u + table[entry].width) * 16;
    cornerU[2] = cornerU[3];
    cornerV[0] = table[entry].v * 16;
    cornerV[2] = cornerV[0];
    cornerV[3] = (table[entry].v + table[entry].height) * 16;
    cornerV[1] = cornerV[3];
    words = context->work.words;
    words[0] = GIF_TAG_EOP | 4;
    words[1] = GIF_HI_TRISTRIP_TEX_NREG3;
    words[2] = GIF_REGS_RGBAQ_UV_XYZ2;
    words[3] = 0;
    words += 4;
    for (i = 0; i < 4; i++) {
        words[0] = sprite->colors[i].red;
        words[1] = sprite->colors[i].green;
        words[2] = sprite->colors[i].blue;
        words[3] = sprite->colors[i].alpha;
        words += 4;
        words[0] = cornerU[i];
        words[1] = cornerV[i];
        words[2] = 0;
        words[3] = 0;
        words += 4;
        words[0] = sprite->vertices[i].position[QUAD_X];
        words[1] = sprite->vertices[i].position[QUAD_Y];
        words[2] = sprite->vertices[i].position[QUAD_DEPTH];
        words[3] = 0;
        words += 4;
    }
    xglPrimAddGifTagDirect(context->packet, context->work.qwords, 13);
}

/* Draws one point primitive at the point's position, with its color and depth. */
static void PrintPoint(FontDrawCursor *context, PrintPointInfo *point)
{
    int *words;
    int tagHigh;
    int tagLow;

    endPrintInfoSet(context, 0, 0);
    tagLow = GIF_TAG_EOP | 1;
    tagHigh = GIF_HI_POINT;
    words = context->work.words;
    words[0] = tagLow;
    words[1] = tagHigh;
    words[2] = GIF_REGS_RGBAQ_XYZ2;
    words[3] = 0;
    words = context->work.words + 4;
    words[0] = point->color[0];
    words[1] = point->color[1];
    words[2] = point->color[2];
    words[3] = point->color[3];
    words = context->work.words + 8;
    words[0] = point->x * 16 + END_PRINT_GS_OFFSET_X;
    words[1] = point->y * 16 + END_PRINT_GS_OFFSET_Y;
    words[2] = point->depth & END_PRINT_DEPTH_MASK;
    words[3] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, 3);
}

/* Emits a line strip starting at line and ending at the first point whose depth is 0. */
static void subPrintLine(FontDrawCursor *context, PrintPointInfo *line)
{
    int *words;
    PrintPointInfo *point;
    PrintPointInfo *scan;
    int count;
    int qwords;

    words = context->work.words;
    count = 0;
    scan = line + 1;
    if (line->depth != 0) {
        do {
            count++;
        } while ((scan++)->depth != 0);
    }
    words[0] = GIF_TAG_EOP + count;
    words[1] = GIF_HI_LINESTRIP;
    qwords = 1;
    words[2] = GIF_REGS_RGBAQ_XYZ2;
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
        words[0] = point->x * 16 + END_PRINT_GS_OFFSET_X;
        words[1] = point->y * 16 + END_PRINT_GS_OFFSET_Y;
        words[2] = line->depth & END_PRINT_DEPTH_MASK;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, qwords);
}

/* Deferred print callbacks receive their descriptor address in an int slot. */
static void PrintLine(FontDrawCursor *context, int lineAddress)
{
    endPrintInfoSet(context, 0, 0);
    subPrintLine(context, (PrintPointInfo *)lineAddress);
}

/* Emits a line strip through the points from line while their terminator is nonzero. */
static void PrintLine2(FontDrawCursor *context, PrintLinePoint *line)
{
    int *words;
    PrintLinePoint *point;
    PrintLinePoint *scan;
    int count;
    int qwords;

    endPrintInfoSet(context, 0, 0);
    scan = line + 1;
    words = context->work.words;
    count = 0;
    if (line->terminator != 0) {
        do {
            count++;
        } while ((scan++)->terminator != 0);
    }
    words[0] = GIF_TAG_EOP + count;
    words[1] = GIF_HI_LINESTRIP;
    qwords = 1;
    words[2] = GIF_REGS_RGBAQ_XYZ2;
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
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, qwords);
}

/* Sends a tristrip ribbon through the scratchpad packet; the points run until the first with depth 0. */
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
    header = END_PRINT_SCRATCHPAD;
    header[0] = ((u64)GIF_NREG_1 << 32) | 3;
    header[1] = GIF_REG_AD;
    header[2] = 0;
    header[3] = SCE_GS_TEXFLUSH;
    header[4] = GS_ALPHA_BLEND;
    header[5] = SCE_GS_ALPHA_1;
    header[6] = GS_TEST_COLOR_ONLY;
    header[7] = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(packet, END_PRINT_SCRATCHPAD, 4);
    point = ribbon;
    count = 0;
    if ((point++)->depth != 0) {
        do {
            count++;
        } while ((point++)->depth != 0);
    }
    ((int *)END_PRINT_SCRATCHPAD)[0] = GIF_TAG_EOP + count;
    ((int *)END_PRINT_SCRATCHPAD)[1] = GIF_HI_TRISTRIP_RIBBON;
    ((int *)END_PRINT_SCRATCHPAD)[2] = GIF_REGS_RGBAQ_XYZ2;
    ((int *)END_PRINT_SCRATCHPAD)[3] = 0;
    point = ribbon;
    words = (int *)END_PRINT_SCRATCHPAD + 4;
    qwords = 1;
    while (count > 0) {
        count--;
        qwords += 2;
        words[0] = point->color[0];
        words[1] = point->color[1];
        words[2] = point->color[2];
        words[3] = point->color[3];
        words += 4;
        words[0] = point->x * 16 + END_PRINT_GS_OFFSET_X;
        words[1] = point->y * 16 + END_PRINT_GS_OFFSET_Y;
        words[2] = ribbon->depth & END_PRINT_DEPTH_MASK;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(packet, END_PRINT_SCRATCHPAD, qwords);
    sceVif1PkCloseDirectHLCode(packet);
}

/* Emits a tristrip ribbon GIF packet from ribbon through the first point whose depth is 0. */
static void subPrintRibbon(FontDrawCursor *context, PrintPointInfo *ribbon)
{
    int *words;
    PrintPointInfo *point;
    PrintPointInfo *scan;
    int count;
    int qwords;

    words = context->work.words;
    count = 0;
    scan = ribbon + 1;
    if (ribbon->depth != 0) {
        do {
            count++;
        } while ((scan++)->depth != 0);
    }
    words[0] = GIF_TAG_EOP + count;
    words[1] = GIF_HI_TRISTRIP_RIBBON;
    qwords = 1;
    words[2] = GIF_REGS_RGBAQ_XYZ2;
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
        words[0] = point->x * 16 + END_PRINT_GS_OFFSET_X;
        words[1] = point->y * 16 + END_PRINT_GS_OFFSET_Y;
        words[2] = ribbon->depth & END_PRINT_DEPTH_MASK;
        words[3] = 0;
        words += 4;
        point++;
    }
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, qwords);
}

static void PrintRibbon(FontDrawCursor *context, int ribbonAddress)
{
    endPrintInfoSet(context, 0, 0);
    subPrintRibbon(context, (PrintPointInfo *)ribbonAddress);
}

/*
 * Draws a text tag one character sprite at a time. A leading TAG_TEXT_RIGHT_ALIGN
 * byte right-aligns the string.
 */
static void PrintTagFont(FontDrawCursor *context, PrintTagFontInfo *tag)
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
    endPrintInfoSet(context, CLUT_TAG_NUM, 1);
    /* Measure a right-aligned tag first; the draw pass starts shift pixels to the left. */
    if (*text == TAG_TEXT_RIGHT_ALIGN) {
        start = text + 1;
        measure = start;
        while ((character = *measure) != 0) {
            if ((unsigned char)(character - 'A') < 26) {
                sprite.uv = *measure - 'A' + TAG_UV_UPPER_A;
            } else if ((unsigned char)(character - 'a') < 26) {
                sprite.uv = *measure - 'a' + TAG_UV_LOWER_A;
            } else {
                symbol = (unsigned char)character;
                if ((unsigned char)(character - '0') < 10) {
                    sprite.uv = NUM_UV_DIGIT_0;
                } else if (symbol == ':') {
                    sprite.uv = NUM_UV_COLON;
                } else if (symbol == '/') {
                    sprite.uv = NUM_UV_SLASH;
                } else if (symbol == '.') {
                    sprite.uv = NUM_UV_PERIOD;
                } else if (symbol == '-') {
                    sprite.uv = NUM_UV_MINUS;
                } else if (symbol == ' ') {
                    shift += TAG_TEXT_SPACE_WIDTH;
                    measure++;
                    continue;
                } else if (symbol == TAG_TEXT_ADVANCE_CODE) {
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
    sprite.uv = TAG_UV_UPPER_A;
    endSpriteSet(&sprite, END_SPRITE_SET_ALL);
    sprite.color[0] = tag->color[0];
    sprite.color[1] = tag->color[1];
    sprite.color[2] = tag->color[2];
    sprite.color[3] = tag->color[3];
    baseHeight = sprite.height;
    /* Draws the characters up to the first digit; a digit is drawn as its column and ends the call. */
    while ((character = *text) != 0) {
        if ((unsigned char)(character - 'A') < 26) {
            sprite.uv = *text - 'A' + TAG_UV_UPPER_A;
        } else if ((unsigned char)(character - 'a') < 26) {
            sprite.uv = *text - 'a' + TAG_UV_LOWER_A;
        } else {
            symbol = (unsigned char)character;
            if ((unsigned char)(character - '0') < 10) {
                sprite.uv = NUM_UV_DIGIT_0;
                endSpriteSet(&sprite, 1);
                sprite.column = *text - '0';
                sprite.x = x - shift;
                sprite.y = tag->y + (baseHeight - sprite.height);
                sprite.depth = tag->depth - 1;
                subPrintSprite(context, &sprite);
                return;
            } else if (symbol == ':') {
                sprite.uv = NUM_UV_COLON;
            } else if (symbol == '/') {
                sprite.uv = NUM_UV_SLASH;
            } else if (symbol == '.') {
                sprite.uv = NUM_UV_PERIOD;
            } else if (symbol == '-') {
                sprite.uv = NUM_UV_MINUS;
            } else if (symbol == ' ') {
                x += TAG_TEXT_SPACE_WIDTH;
                text++;
                continue;
            } else if (symbol == TAG_TEXT_ADVANCE_CODE) {
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

/* Sets the GS scissor to rect clipped to the print screen; draws nothing when rect starts past the right or bottom edge. */
static void ScissorSet(FontDrawCursor *context, PrintRect *rect)
{
    u64 *scratch;
    int x;
    int y;
    int width;
    int height;

    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | GIF_TAG_EOP | 1;
    scratch[1] = GIF_REG_AD;
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
    } else if (x + width >= END_PRINT_SCREEN_WIDTH) {
        if (x >= END_PRINT_SCREEN_WIDTH) {
            return;
        }
        width -= x + width - END_PRINT_SCREEN_WIDTH;
    }
    if (y < 0) {
        height += y;
        if (height <= 0) {
            height = 1;
        }
        y = 0;
    } else if (y + height >= END_PRINT_SCREEN_HEIGHT) {
        if (y >= END_PRINT_SCREEN_HEIGHT) {
            return;
        }
        height -= y + height - END_PRINT_SCREEN_HEIGHT;
    }
    scratch[2] = x | ((u64)(x + width - 1) << 16) | ((u64)y << 32) |
                 ((u64)(y + height - 1) << 48);
    scratch[3] = SCE_GS_SCISSOR_1;
    sceVif1PkAddDirectDataN(context->packet, scratch, 2);
}

/* Restores the scissor to the render state's full size. */
static void ScissorReset(FontDrawCursor *context)
{
    u64 *scratch;

    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | GIF_TAG_EOP | 1;
    scratch[1] = GIF_REG_AD;
    scratch[2] = ((u64)(sRender.scissor_width - 1) << 16) |
                 ((u64)(sRender.scissor_height - 1) << 48);
    scratch[3] = SCE_GS_SCISSOR_1;
    sceVif1PkAddDirectDataN(context->packet, scratch, 2);
}

/*
 * Draws a number with the digit sprites, or a leading minus sign for negative
 * values. flags pick the digit style (low nibble) and right alignment (bit 4).
 */
static void PrintNumber(FontDrawCursor *context, PrintNumberInfo *number)
{
    PrintSpriteInfo sprite;
    int offset;
    int x;
    int i;
    int value;
    int leadingDigit;

    offset = 0;
    endPrintInfoSet(context, CLUT_TAG_NUM, 1);
    if (number->value < 0) {
        sprite.uv = NUM_UV_MINUS;
        endSpriteSet(&sprite, END_SPRITE_SET_ALL);
        sprite.color[0] = number->color[0];
        sprite.color[1] = number->color[1];
        sprite.color[2] = number->color[2];
        sprite.color[3] = number->color[3];
        sprite.y = number->y;
        sprite.depth = number->depth;
        x = number->x;
        if (number->flags & PRINT_NUMBER_RIGHT_ALIGN) {
            offset = -(sprite.width * number->digits);
        }
        for (i = 0; i < number->digits; i++) {
            sprite.x = x + offset;
            subPrintSprite(context, &sprite);
            x += sprite.width;
        }
    } else {
        sprite.uv = (number->flags & PRINT_NUMBER_STYLE_MASK) + NUM_UV_DIGIT_0;
        endSpriteSet(&sprite, END_SPRITE_SET_ALL);
        value = number->value;
        if (value < 0) {
            value = 0;
        }
        sprite.y = number->y;
        sprite.depth = number->depth;
        x = number->x + sprite.width * (number->digits - 1);
        if (number->flags & PRINT_NUMBER_RIGHT_ALIGN) {
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

/*
 * Draws a window frame: tiled interior strips when the frame is tall enough,
 * then the eight border pieces.
 */
static void PrintWindow(FontDrawCursor *context, PrintFrameInfo *frame)
{
    short left[8];
    short top[8];
    short width[8];
    short height[8];
    int count;
    int *words;
    int *command;
    int i;
    short windowHeight;
    unsigned short frameHeight;
    unsigned short frameX;
    unsigned short frameY;
    unsigned short frameWidth;
    int innerX;
    int innerY;
    int right;
    int bottom;
    int innerWidth;
    int innerHeight;
    int depth;
    int strip;

    command = context->work.words;
    endPrintInfoSet(context, CLUT_WINDOW, 1);
    words = command;
    words[1] = GIF_NREG_1;
    words[2] = words[0] = 1;
    count = 2;
    words[3] = 0;
    windowHeight = frame->height;
    words += 4;
    words[0] = frame->color[0];
    frameHeight = frame->height;
    words[1] = frame->color[2];
    words[2] = frame->color[1];
    words[3] = frame->color[3];
    words += 4;
    /* Interior fill: strips of up to WINDOW_TILE_SIZE rows from win_uv[WINDOW_INTERIOR_UV]. */
    if (windowHeight >= WINDOW_BORDER_BOTH_SIDES + 1) {
        int remaining = windowHeight - WINDOW_BORDER_BOTH_SIDES;
        int strips;

        strips = remaining / WINDOW_TILE_SIZE + 1;
        words[0] = strips;
        words[1] = GIF_HI_SPRITE_TEX_NREG4;
        words[2] = GIF_REGS_UV_XYZ2_X2;
        words[3] = 0;
        words += 4;
        for (strip = 0; strip < strips; strip++) {
            int textureV;
            int depthWord;
            int stripWidth;
            const SpriteUv *interiorUv = win_uv;
            unsigned short windowX = frame->x;
            unsigned short windowY = frame->y;
            unsigned short windowWidth = frame->width;

            words[0] = interiorUv[WINDOW_INTERIOR_UV].u * 16;
            textureV = interiorUv[WINDOW_INTERIOR_UV].v * 16;
            words[4] = ((short)windowX + WINDOW_BORDER) * 16 + END_PRINT_GS_OFFSET_X;
            words[1] = textureV;
            words[2] = 0;
            words[3] = 0;
            words[5] = ((short)windowY + WINDOW_BORDER + strip * WINDOW_TILE_SIZE) * 16 + END_PRINT_GS_OFFSET_Y;
            depthWord = frame->depth;
            words[7] = 0;
            words[6] = depthWord & END_PRINT_DEPTH_MASK;
            if (remaining > WINDOW_TILE_SIZE) {
                words[13] = words[5] + WINDOW_TILE_SIZE * 16;
                words[9] = words[1] + interiorUv[WINDOW_INTERIOR_UV].height * 16;
            } else {
                words[9] = textureV + remaining * 16;
                words[13] = words[5] + remaining * 16;
            }
            remaining -= WINDOW_TILE_SIZE;
            words[10] = 0;
            words[11] = 0;
            words[15] = 0;
            words[8] = words[0] + interiorUv[WINDOW_INTERIOR_UV].width * 16;
            stripWidth = ((short)windowWidth - WINDOW_BORDER_BOTH_SIDES) * 16;
            words[12] = words[4] + stripWidth;
            words[14] = words[6];
            words += 16;
        }
        count = strips * 4 + 3;
    }
    /* Border: eight pieces from win_uv[0..7], placed by the left/top/width/height tables. */
    frameX = frame->x;
    frameY = frame->y;
    frameWidth = frame->width;
    words[0] = GIF_TAG_EOP | WINDOW_BORDER_PIECES;
    left[5] = left[3] = left[0] = frameX;
    innerX = frameX + WINDOW_BORDER;
    left[6] = left[1] = innerX;
    right = innerX + frameWidth - WINDOW_BORDER_BOTH_SIDES;
    left[2] = right;
    left[4] = right;
    innerWidth = frameWidth - WINDOW_BORDER_BOTH_SIDES;
    top[2] = top[1] = top[0] = frameY;
    innerY = frameY + WINDOW_BORDER;
    top[3] = innerY;
    left[7] = right;
    top[4] = innerY;
    bottom = innerY + frameHeight - WINDOW_BORDER_BOTH_SIDES;
    top[5] = bottom;
    innerHeight = frameHeight - WINDOW_BORDER_BOTH_SIDES;
    top[6] = bottom;
    top[7] = bottom;
    width[0] = WINDOW_BORDER;
    width[2] = WINDOW_BORDER;
    width[7] = width[5] = width[4] = width[3] = WINDOW_BORDER;
    width[6] = width[1] = innerWidth;
    height[6] = height[5] = height[2] = height[1] = height[0] = WINDOW_BORDER;
    height[7] = WINDOW_BORDER;
    height[4] = height[3] = innerHeight;
    words[1] = GIF_HI_SPRITE_TEX_NREG4;
    words[2] = GIF_REGS_UV_XYZ2_X2;
    words[3] = 0;
    words += 4;
    for (i = 0; i < WINDOW_BORDER_PIECES; i++) {
        words[0] = win_uv[i].u * 16;
        words[1] = win_uv[i].v * 16;
        words[4] = left[i] * 16 + END_PRINT_GS_OFFSET_X;
        words[5] = top[i] * 16 + END_PRINT_GS_OFFSET_Y;
        words[2] = 0;
        words[3] = 0;
        depth = frame->depth & END_PRINT_DEPTH_MASK;
        words[8] = words[0] + win_uv[i].width * 16;
        words[9] = words[1] + win_uv[i].height * 16;
        words[12] = words[4] + width[i] * 16;
        words[13] = words[5] + height[i] * 16;
        words[14] = words[6] = depth;
        words[7] = 0;
        words[10] = 0;
        words[11] = 0;
        words[15] = 0;
        words += 16;
    }
    xglPrimAddGifTagDirect(context->packet, command, count + i * 4 + 1);
}

/* Draws the eight border pieces of a PrintUnsignedFrameInfo window frame. */
static void PrintFrame(FontDrawCursor *context, int param)
{
    const PrintUnsignedFrameInfo *frame;
    short left[8];
    short top[8];
    short width[8];
    short height[8];
    int *words;
    int *command;
    int i;
    int innerX;
    int innerY;
    int right;
    int bottom;
    int innerWidth;
    int innerHeight;
    int depth;
    int frameX;
    int frameY;
    int frameWidth;
    int frameHeight;

    frame = (const PrintUnsignedFrameInfo *)param;
    command = context->work.words;
    endPrintInfoSet(context, CLUT_WINDOW, 1);
    words = command;
    words[1] = GIF_NREG_1;
    words[2] = words[0] = 1;
    words[3] = 0;
    words += 4;
    words[0] = frame->color[0];
    words[1] = frame->color[2];
    words[2] = frame->color[1];
    words[3] = frame->color[3];
    frameX = frame->x;
    frameWidth = frame->width;
    frameY = frame->y;
    left[5] = left[3] = left[0] = frameX;
    innerX = frameX + WINDOW_BORDER;
    left[6] = left[1] = innerX;
    right = innerX + frameWidth - WINDOW_BORDER_BOTH_SIDES;
    left[2] = right;
    left[4] = right;
    innerWidth = frameWidth - WINDOW_BORDER_BOTH_SIDES;
    top[2] = top[1] = top[0] = frameY;
    frameHeight = frame->height;
    innerY = frameY + WINDOW_BORDER;
    top[3] = innerY;
    left[7] = right;
    top[4] = innerY;
    bottom = innerY + frameHeight - WINDOW_BORDER_BOTH_SIDES;
    top[5] = bottom;
    innerHeight = frameHeight - WINDOW_BORDER_BOTH_SIDES;
    top[6] = bottom;
    top[7] = bottom;
    width[0] = WINDOW_BORDER;
    width[2] = WINDOW_BORDER;
    width[7] = width[5] = width[4] = width[3] = WINDOW_BORDER;
    width[6] = width[1] = innerWidth;
    height[6] = height[5] = height[2] = height[1] = height[0] = WINDOW_BORDER;
    height[7] = WINDOW_BORDER;
    height[4] = height[3] = innerHeight;
    words += 4;
    words[0] = GIF_TAG_EOP | WINDOW_BORDER_PIECES;
    words[1] = GIF_HI_SPRITE_TEX_NREG4;
    words[2] = GIF_REGS_UV_XYZ2_X2;
    words[3] = 0;
    words += 4;
    for (i = 0; i < WINDOW_BORDER_PIECES; i++) {
        words[0] = win_uv[i].u * 16;
        words[1] = win_uv[i].v * 16;
        words[4] = left[i] * 16 + END_PRINT_GS_OFFSET_X;
        words[5] = top[i] * 16 + END_PRINT_GS_OFFSET_Y;
        words[2] = 0;
        words[3] = 0;
        depth = frame->depth & END_PRINT_DEPTH_MASK;
        words[8] = words[0] + win_uv[i].width * 16;
        words[9] = words[1] + win_uv[i].height * 16;
        words[12] = words[4] + width[i] * 16;
        words[13] = words[5] + height[i] * 16;
        words[14] = words[6] = depth;
        words[7] = 0;
        words[10] = 0;
        words[11] = 0;
        words[15] = 0;
        words += 16;
    }
    xglPrimAddGifTagDirect(context->packet, command, i * 4 + 3);
}

/* Sends the thumbnail image reference and draws its quad at the thumbnail's position and depth. */
static void PrintThumbnail(FontDrawCursor *context, PrintThumbnailInfo *thumbnail)
{
    int *words;

    sceVif1PkCloseDirectHLCode(context->packet);
    sceVif1PkRef(context->packet, ThumImageEnv_1, 6, 0, 0, 0);
    sceVif1PkRef(context->packet, thumbnail->image, 3586, 0, 0, 0);
    sceVif1PkCnt(context->packet, 0);
    sceVif1PkOpenDirectHLCode(context->packet, 0);
    words = (int *)ThumDrawEnv_2;
    words[36] = thumbnail->x * 16 + END_PRINT_GS_OFFSET_X;
    words[37] = thumbnail->y * 16 + END_PRINT_GS_OFFSET_Y;
    words[38] = thumbnail->depth;
    words[44] = thumbnail->x * 16 + 0x77F8;
    words[45] = thumbnail->y * 16 + 0x77D8;
    words[46] = thumbnail->depth;
    sceVif1PkAddDirectDataN(context->packet, ThumDrawEnv_2, 12);
}

/*
 * Writes the GIF packet for a circle (a trifan of pointCount rim points) into
 * buffer and sends it on packet.
 */
static void PrintCircleCore(XglPacket *packet, void *buffer, PrintCircleInfo *circle)
{
    u64 *header;
    int *words;
    int count;
    int i;
    float angle;

    header = buffer;
    header[0] = ((u64)GIF_NREG_1 << 32) | 2;
    header[1] = GIF_REG_AD;
    header[2] = GS_ALPHA_BLEND;
    header[3] = SCE_GS_ALPHA_1;
    header[4] = GS_TEST_ANEQ_ZGEQUAL_AFAIL_ZB;
    header[5] = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(packet, buffer, 3);
    words = (int *)buffer + 4;
    words[0] = circle->color[0];
    words[1] = circle->color[1];
    words[2] = circle->color[2];
    words[3] = circle->color[3];
    words = (int *)buffer + 8;
    words[0] = circle->x * 16 + END_PRINT_GS_OFFSET_X;
    words[1] = circle->y * 16 + END_PRINT_GS_OFFSET_Y;
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
        words[0] = (unsigned int)(((float)(circle->x + END_PRINT_GS_ORIGIN_X) + xglSin(angle) * (float)circle->radius) * 16.0f - 8.0f);
        words[1] = (unsigned int)(((float)(circle->y + END_PRINT_GS_ORIGIN_Y) + xglCos(angle) * (float)circle->radius) * 16.0f - 8.0f);
        words[2] = circle->depth;
        words[3] = 0;
        words += 4;
        count++;
    }
    words = (int *)buffer;
    words[0] = GIF_TAG_EOP + count / 2;
    words[1] = GIF_HI_TRIFAN;
    words[2] = GIF_REGS_RGBAQ_XYZ2;
    words[3] = 0;
    sceVif1PkAddDirectDataN(packet, buffer, count + 1);
}

void endPrintDirectCircle(int circleAddress) {
    XglPacket *vif1Packet;

    vif1Packet = xglPacketGetCurrent();
    sceVif1PkCnt(vif1Packet, 0);
    sceVif1PkOpenDirectHLCode(vif1Packet, 0);
    PrintCircleCore(vif1Packet, END_PRINT_SCRATCHPAD, (PrintCircleInfo *)circleAddress);
    sceVif1PkCloseDirectHLCode(vif1Packet);
}

static void PrintCircle(FontDrawCursor *context, int circleAddress)
{
    PrintCircleCore(context->packet, context->work.qwords, (PrintCircleInfo *)circleAddress);
}

static void FontTexReload(FontDrawCursor *context)
{
    u64 *scratch;

    scratch = context->work.qwords;
    scratch[0] = ((u64) GIF_NREG_1 << 32) | GIF_TAG_EOP;
    scratch[1] = GIF_REG_AD;
    scratch[3] = SCE_GS_TEXFLUSH;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 1);
    xglFontReloadTexture(context, 2);
}

/* Flushes the texture cache, then sends each window texture of *window to the packet. */
static void FontTexChange(FontDrawCursor *context, WindowTexDirectory **window)
{
    u64 *scratch;
    int i;
    WindowTexDirectory *directory;
    WindowTexEntry *entry;
    unsigned int image;
    unsigned int imageLow;

    i = 0;
    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | GIF_TAG_EOP;
    scratch[1] = GIF_REG_AD;
    scratch[3] = SCE_GS_TEXFLUSH;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 1);
    sceVif1PkCloseDirectHLCode(context->packet);
    directory = *window;
    entry = (WindowTexEntry *)((unsigned char *)directory + directory->entryOffset);
    for (; i < directory->entryCount; i++, entry++) {
        image = entry->bufferWidthAndFormat;
        imageLow = image & 0xFFFF;
        Env_3[4] = ((u64)((entry->vramAddress + WINDOW_TEX_VRAM_BASE) >> 6) << 32) |
                   ((u64)(image >> 16) << 48);
        Env_3[8] = imageLow | ((u64)entry->clutAddress << 32);
        sceVif1PkCnt(context->packet, 0);
        sceVif1PkAddDataN(context->packet, Env_3, 24);
        sceVif1PkRef(context->packet, (unsigned char *)directory + entry->dataOffset, entry->size, 0, 0, 0);
    }
    sceVif1PkCnt(context->packet, 0);
    sceVif1PkOpenDirectHLCode(context->packet, 0);
}

/* Sets the back-buffer frame and Z state from the render state's draw_back_value. */
static void DrawBackSet(FontDrawCursor *context)
{
    u64 *scratch;
    unsigned int draw_back_control;

    scratch = context->work.qwords;
    draw_back_control = sRender.draw_back_value;
    draw_back_control |= GS_BUFFER_WIDTH_512;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | GIF_TAG_EOP | 3;
    scratch[1] = GIF_REG_AD;
    scratch[3] = SCE_GS_TEXFLUSH;
    scratch[4] = GS_ZBUF_ZMSK;
    scratch[5] = SCE_GS_ZBUF_1;
    scratch[6] = draw_back_control;
    scratch[7] = SCE_GS_FRAME_1;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 4);
}

/* Restores the back-buffer frame and Z state from the render state's draw_back_reset_value. */
static void DrawBackReset(FontDrawCursor *context)
{
    u64 *scratch;
    unsigned int draw_back_control;

    scratch = context->work.qwords;
    draw_back_control = sRender.draw_back_reset_value;
    draw_back_control |= GS_BUFFER_WIDTH_512;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | GIF_TAG_EOP | 3;
    scratch[1] = GIF_REG_AD;
    scratch[3] = SCE_GS_TEXFLUSH;
    scratch[4] = (u64)0x3100 << 16;
    scratch[5] = SCE_GS_ZBUF_1;
    scratch[6] = draw_back_control;
    scratch[7] = SCE_GS_FRAME_1;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 4);
}

/* Draws a full-screen sprite over the 512x448 print area with the color-only test. */
static void ScreenClear(FontDrawCursor *context)
{
    u64 *scratch;

    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_HI_SPRITE_TEX_NREG4 << 32) | GIF_TAG_EOP | 1;
    scratch[1] = GIF_REGS_AD_RGBAQ_XYZ2_X2;
    scratch[2] = GS_TEST_COLOR_ONLY;
    scratch[3] = SCE_GS_TEST_1;
    scratch[4] = 0;
    scratch[5] = 0;
    scratch = context->work.qwords + 6;
    ((int *)scratch)[0] = END_PRINT_GS_OFFSET_X;
    ((int *)scratch)[1] = END_PRINT_GS_OFFSET_Y;
    ((int *)scratch)[2] = 0;
    ((int *)scratch)[3] = 0;
    ((int *)scratch)[4] = END_PRINT_GS_OFFSET_X + END_PRINT_SCREEN_WIDTH * 16;
    ((int *)scratch)[5] = END_PRINT_GS_OFFSET_Y + END_PRINT_SCREEN_HEIGHT * 16;
    ((int *)scratch)[6] = 0;
    ((int *)scratch)[7] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, 5);
}

/* Writes the Z buffer over rect without touching color. */
static void ZScissor(FontDrawCursor *context, PrintRect *rect)
{
    u64 *scratch;

    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_HI_SPRITE_PLAIN_NREG4 << 32) | GIF_TAG_EOP | 1;
    scratch[1] = GIF_REGS_AD_RGBAQ_XYZ2_X2;
    scratch[2] = GS_TEST_Z_ONLY;
    scratch[3] = SCE_GS_TEST_1;
    scratch[4] = 0;
    scratch[5] = 0;
    scratch = context->work.qwords + 6;
    ((int *)scratch)[0] = rect->x * 16 + END_PRINT_GS_OFFSET_X;
    ((int *)scratch)[1] = rect->y * 16 + END_PRINT_GS_OFFSET_Y;
    ((int *)scratch)[2] = rect->depth;
    ((int *)scratch)[3] = 0;
    ((int *)scratch)[4] = (rect->x + rect->width) * 16 + END_PRINT_GS_OFFSET_X;
    ((int *)scratch)[5] = (rect->y + rect->height) * 16 + END_PRINT_GS_OFFSET_Y;
    ((int *)scratch)[6] = rect->depth;
    ((int *)scratch)[7] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, 5);
}

/* Draws a back-buffer quad as a textured tristrip; each vertex takes its UV pair from quad->uv in a fixed order. */
static void PrintBackSprite(FontDrawCursor *context, PrintBackQuad *quad)
{
    u64 *scratch;

    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | 5;
    scratch[1] = GIF_REG_AD;
    scratch[2] = GS_ALPHA_BLEND;
    scratch[3] = SCE_GS_ALPHA_1;
    scratch[4] = GS_CLAMP_PRINT_BACK;
    scratch[5] = SCE_GS_CLAMP_1;
    scratch[6] = 96;
    scratch[7] = SCE_GS_TEX1_1;
    scratch[8] = (0x24020000 | (sRender.draw_back_value << 5)) | (((u64)0x20000006 << 32) | 0x40000000);
    scratch[9] = SCE_GS_TEX0_1;
    scratch[10] = GS_TEST_ZALWAYS;
    scratch[11] = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(context->packet, scratch, 6);
    {
        short order[8] = { 0, 1, 0, 3, 2, 1, 2, 3 };
        int *words;
        float *texture;
        PrintBackQuadVertex *vertex;
        int i;
        int gray;

        ((int *)scratch)[0] = GIF_TAG_EOP | 4;
        ((int *)scratch)[1] = GIF_HI_TRISTRIP_ST_NREG3;
        ((int *)scratch)[2] = GIF_REGS_RGBAQ_ST_XYZ2;
        ((int *)scratch)[3] = 0;
        words = context->work.words + 4;
        gray = END_PRINT_COLOR_NEUTRAL;
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

/* Draws a textured back-buffer sprite from its position, size and uv origin. */
static void PrintBackSprite2(FontDrawCursor *context, PrintBackSpriteInfo *sprite)
{
    u64 *scratch;
    int *words;

    scratch = context->work.qwords;
    scratch[0] = ((u64)GIF_NREG_1 << 32) | 5;
    scratch[1] = GIF_REG_AD;
    scratch[2] = GS_ALPHA_BLEND;
    scratch[3] = SCE_GS_ALPHA_1;
    scratch[4] = GS_CLAMP_PRINT_BACK;
    scratch[5] = SCE_GS_CLAMP_1;
    scratch[6] = 96;
    scratch[7] = SCE_GS_TEX1_1;
    scratch[8] = (0x24020000 | (sRender.draw_back_value << 5)) | (((u64)0x20000006 << 32) | 0x40000000);
    scratch[9] = SCE_GS_TEX0_1;
    scratch[10] = GS_TEST_ANEQ_ZALWAYS;
    scratch[11] = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(context->packet, scratch, 6);
    ((int *)scratch)[0] = GIF_TAG_EOP | 1;
    ((int *)scratch)[1] = GIF_HI_SPRITE_TEX_NREG5;
    ((int *)scratch)[2] = GIF_REGS_RGBAQ_UV_XYZ2_UV_XYZ2;
    ((int *)scratch)[3] = 0;
    words = context->work.words + 4;
    words[0] = sprite->color[0];
    words[1] = sprite->color[1];
    words[2] = sprite->color[2];
    words[3] = sprite->color[3];
    words = context->work.words + 8;
    words[0] = 0;
    words[1] = 0;
    words[2] = 0;
    words[3] = 0;
    words = context->work.words + 12;
    words[0] = sprite->x * 16 + END_PRINT_GS_OFFSET_X;
    words[1] = sprite->y * 16 + END_PRINT_GS_OFFSET_Y;
    words[2] = sprite->depth;
    words[3] = 0;
    words = context->work.words + 16;
    words[0] = sprite->u * 16;
    words[1] = sprite->v * 16;
    words[2] = 0;
    words[3] = 0;
    words = context->work.words + 20;
    words[0] = (sprite->x + sprite->width) * 16 + END_PRINT_GS_OFFSET_X;
    words[1] = (sprite->y + sprite->height) * 16 + END_PRINT_GS_OFFSET_Y;
    words[2] = sprite->depth;
    words[3] = 0;
    sceVif1PkAddDirectDataN(context->packet, context->work.qwords, 6);
}

/* Glow ribbons drawn around an ether line, each half as opaque as the last. */
#define ETHER_LINE_GLOW_STEPS 8

/*
 * Draws an ether line from two endpoints: a line strip, then the glow ribbons
 * around it.
 */
static void PrintEtherLine(FontDrawCursor *context, int param)
{
    const EtherLineEndpoint *endpoints;
    PrintPointInfo line[3];
    PrintPointInfo ribbon[5];
    float opacity;
    int i;
    u64 *command;

    endpoints = (const EtherLineEndpoint *)param;
    command = context->work.qwords;
    command[0] = ((u64)GIF_NREG_1 << 32) | 2;
    command[1] = GIF_REG_AD;
    command[2] = GS_ALPHA_BLEND;
    command[3] = SCE_GS_ALPHA_1;
    command[4] = GS_TEST_ANEQ_ZGEQUAL;
    command[5] = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(context->packet, command, 3);
    line[0].color[0] = endpoints[0].color[0];
    line[0].color[1] = endpoints[0].color[1];
    line[0].color[2] = endpoints[0].color[2];
    line[0].color[3] = endpoints[0].color[3];
    line[1].color[0] = endpoints[1].color[0];
    line[1].color[1] = endpoints[1].color[1];
    line[1].color[2] = endpoints[1].color[2];
    line[1].color[3] = endpoints[1].color[3];
    line[0].x = (int)(endpoints[0].x);
    line[0].y = (int)(endpoints[0].y);
    line[2].depth = 0;
    line[0].depth = (unsigned int)endpoints[0].depth;
    line[1].x = (int)(endpoints[1].x);
    line[1].y = (int)(endpoints[1].y);
    line[1].depth = (unsigned int)endpoints[1].depth;
    subPrintLine(context, line);
    opacity = 1.0f;
    ribbon[4].depth = 0;
    ribbon[0].depth = ribbon[1].depth = (unsigned int)endpoints[0].depth;
    ribbon[2].depth = ribbon[3].depth = (unsigned int)endpoints[1].depth;
    ribbon[0].color[0] = ribbon[1].color[0] = endpoints[0].color[0];
    ribbon[0].color[1] = ribbon[1].color[1] = endpoints[0].color[1];
    ribbon[0].color[2] = ribbon[1].color[2] = endpoints[0].color[2];
    ribbon[0].color[3] = ribbon[1].color[3] = endpoints[0].color[3];
    ribbon[2].color[0] = ribbon[3].color[0] = endpoints[1].color[0];
    ribbon[2].color[1] = ribbon[3].color[1] = endpoints[1].color[1];
    ribbon[2].color[2] = ribbon[3].color[2] = endpoints[1].color[2];
    ribbon[2].color[3] = ribbon[3].color[3] = endpoints[1].color[3];
    /* Level endpoints grow the glow up and down; sloped ones grow it on both axes. */
    if (endpoints[0].y == endpoints[1].y) {
        for (i = 0; i < ETHER_LINE_GLOW_STEPS; i++) {
            unsigned int startAlpha;
            unsigned int endAlpha;

            ribbon[0].x = (int)(endpoints[0].x - i);
            ribbon[1].x = ribbon[0].x;
            ribbon[2].x = (int)(endpoints[1].x + i);
            ribbon[3].x = ribbon[2].x;
            ribbon[0].y = (int)(endpoints[0].y - i);
            ribbon[1].y = (int)(endpoints[0].y + i);
            ribbon[2].y = (int)(endpoints[1].y - i);
            ribbon[3].y = (int)(endpoints[1].y + i);
            startAlpha = (unsigned int)(endpoints[0].color[3] * opacity);
            ribbon[0].color[3] = ribbon[1].color[3] = startAlpha;
            endAlpha = (unsigned int)(endpoints[1].color[3] * opacity);
            ribbon[2].color[3] = ribbon[3].color[3] = endAlpha;
            opacity *= 0.5f;
            subPrintRibbon(context, ribbon);
        }
    } else {
        for (i = 0; i < ETHER_LINE_GLOW_STEPS; i++) {
            unsigned int startAlpha;
            unsigned int endAlpha;

            ribbon[0].x = (int)(endpoints[0].x - i);
            ribbon[0].y = (int)(endpoints[0].y - i);
            ribbon[1].x = (int)(endpoints[0].x + i);
            ribbon[1].y = ribbon[0].y;
            ribbon[2].x = (int)(endpoints[1].x - i);
            ribbon[2].y = (int)(endpoints[1].y + i);
            ribbon[3].x = (int)(endpoints[1].x + i);
            ribbon[3].y = ribbon[2].y;
            startAlpha = (unsigned int)(endpoints[0].color[3] * opacity);
            ribbon[0].color[3] = ribbon[1].color[3] = startAlpha;
            endAlpha = (unsigned int)(endpoints[1].color[3] * opacity);
            ribbon[2].color[3] = ribbon[3].color[3] = endAlpha;
            opacity *= 0.5f;
            subPrintRibbon(context, ribbon);
        }
    }
}

/*
 * Dispatches a print command: standard commands are queued through func_tbl_4,
 * extended commands run through xglFontPrintExtFunc.
 */
void endPrintExtFunc(int order, int command, void *param)
{
    if (command < END_PRINT_CMD_EXT_BASE)
        goto standard_command;
    goto extended_command;
    do {
standard_command:
        if (command < END_PRINT_CMD_STANDARD_COUNT)
            endPrintExtFuncPack(func_tbl_4[command], (int)param);
        break;
extended_command:
        command -= END_PRINT_CMD_EXT_BASE;
        if (command < END_PRINT_EXT_CMD_LIMIT)
            xglFontPrintExtFunc(order, func_tbl2_5[command], param);
    } while (0);
}

/* Sends the frame-copy template to packet, with element 4 set from tagLow and tagHigh. */
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
        FrameCopyEnv[4] = (u64) (u32) (tagLow | GS_BUFFER_WIDTH_512) |
                          ((u64) tagHigh << 0x20) |
                          ((u64) GS_BUFFER_WIDTH_512 << 32);
        sceVif1PkRef(packet, FrameCopyEnv, 6, 0, 0, 0);
    }
}

/* Sends a line strip through the scratchpad packet; the points run until the first with z 0. */
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
    header = END_PRINT_SCRATCHPAD;
    header[0] = ((u64)GIF_NREG_1 << 32) | 3;
    header[1] = GIF_REG_AD;
    header[2] = 0;
    header[3] = SCE_GS_TEXFLUSH;
    header[4] = GS_ALPHA_BLEND;
    header[5] = SCE_GS_ALPHA_1;
    header[6] = GS_TEST_COLOR_ONLY;
    header[7] = SCE_GS_TEST_1;
    sceVif1PkAddDirectDataN(packet, END_PRINT_SCRATCHPAD, 4);
    point = line;
    count = 0;
    if ((point++)->z != 0) {
        do {
            count++;
        } while ((point++)->z != 0);
    }
    ((int *)END_PRINT_SCRATCHPAD)[0] = GIF_TAG_EOP + count;
    ((int *)END_PRINT_SCRATCHPAD)[1] = GIF_HI_LINESTRIP;
    ((int *)END_PRINT_SCRATCHPAD)[2] = GIF_REGS_RGBAQ_XYZ2;
    ((int *)END_PRINT_SCRATCHPAD)[3] = 0;
    point = line;
    words = (int *)END_PRINT_SCRATCHPAD + 4;
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
    sceVif1PkAddDirectDataN(packet, END_PRINT_SCRATCHPAD, qwords);
    sceVif1PkCloseDirectHLCode(packet);
}

/* Sets up the context for a full-screen (512x448) decode and runs xglJpegDecode. */
void endDecodeJpeg(EndDecodeJpegContext *context) {
    int source;
    int destination;

    source = context->source;
    destination = context->destination;
    context->width = END_PRINT_SCREEN_WIDTH;
    context->height = END_PRINT_SCREEN_HEIGHT;
    context->decodeSource = source;
    context->decodeDestination = destination;
    context->decodeParam[0] = 0;
    context->decodeParam[1] = 0;
    context->decodeParam[2] = 0;
    context->decodeParam[3] = 0;
    xglJpegDecode(&context->decodeSource);
}

/* A decoded 512x448 image is sent in two halves of this many quadwords. */
#define JPEG_HALF_QWORDS 28672

/* Sends a JPEG image as two halves, each with its own GS position block. */
void endPrintJpeg(PrintJpegInfo *jpeg)
{
    XglPacket *packet;
    int *topWords;
    int *bottomWords;

    packet = xglPacketGetCurrent();
    sceVif1PkRef(packet, sJpegEnv00_7, 8, 0, 0, 0);
    sceVif1PkRef(packet, jpeg->image, JPEG_HALF_QWORDS, 0, 0, 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    topWords = (int *)JpegEnv01_8;
    topWords[28] = (jpeg->x + jpeg->offsetX) * 16 + END_PRINT_GS_OFFSET_X;
    topWords[29] = (jpeg->y + jpeg->offsetY) * 16 + END_PRINT_GS_OFFSET_Y;
    topWords[30] = jpeg->depth;
    topWords[36] = (jpeg->x + jpeg->width + jpeg->offsetX) * 16 + END_PRINT_GS_OFFSET_X;
    topWords[37] = (jpeg->y + jpeg->height / 2 + jpeg->offsetY) * 16 + END_PRINT_GS_OFFSET_Y;
    topWords[38] = jpeg->depth;
    sceVif1PkAddDirectDataN(packet, JpegEnv01_8, 10);
    sceVif1PkCloseDirectHLCode(packet);
    sceVif1PkRef(packet, JpegEnv10_9, 8, 0, 0, 0);
    sceVif1PkRef(packet, jpeg->image + JPEG_HALF_QWORDS * 16, JPEG_HALF_QWORDS, 0, 0, 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    bottomWords = (int *)JpegEnv11_10;
    bottomWords[28] = (jpeg->x + jpeg->offsetX) * 16 + END_PRINT_GS_OFFSET_X;
    bottomWords[29] = (jpeg->y + jpeg->height / 2 + jpeg->offsetY) * 16 + END_PRINT_GS_OFFSET_Y;
    bottomWords[30] = jpeg->depth;
    bottomWords[36] = (jpeg->x + jpeg->width + jpeg->offsetX) * 16 + END_PRINT_GS_OFFSET_X;
    bottomWords[37] = (jpeg->y + jpeg->height + jpeg->offsetY) * 16 + END_PRINT_GS_OFFSET_Y;
    bottomWords[38] = jpeg->depth;
    sceVif1PkAddDirectDataN(packet, JpegEnv11_10, 10);
    sceVif1PkCloseDirectHLCode(packet);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static u64 ThumImageEnv_1[12] = { 0ULL, 5836665138832211968ULL, 1152921504606879748ULL, 14ULL, 624522604576768ULL, 80ULL, 0ULL, 81ULL, 481036337280ULL, 82ULL, 0ULL, 83ULL };

static u64 sJpegEnv00_7[16] = { 0ULL, 5836665138546999296ULL, 1152921504606846980ULL, 14ULL, 2313372464840704ULL, 80ULL, 0ULL, 81ULL, 962072674816ULL, 82ULL, 0ULL, 83ULL, 0ULL, 5836788266669441024ULL, 576460752303484928ULL, 0ULL };

static u64 JpegEnv10_9[16] = { 0ULL, 5836665138546999296ULL, 1152921504606846980ULL, 14ULL, 2313372464840704ULL, 80ULL, 0ULL, 81ULL, 962072674816ULL, 82ULL, 0ULL, 83ULL, 0ULL, 5836788266669441024ULL, 576460752303452160ULL, 0ULL };

static u64 ePrintEnv_0[12] = { 0x0000000000000000ULL, 0x5100000500000000ULL, 0x1000000000000004ULL, 0x000000000000000eULL, 0x00083c0000000000ULL, 0x0000000000000050ULL, 0x0000000000000000ULL, 0x0000000000000051ULL, 0x0000000000000000ULL, 0x0000000000000052ULL, 0x0000000000000000ULL, 0x0000000000000053ULL };

static u64 ThumDrawEnv_2[24] = { 0x1000000000000005ULL, 0x000000000000000eULL, 0x0000000000000000ULL, 0x000000000000003fULL, 0x0000000000000000ULL, 0x0000000000000008ULL, 0x0000000000000000ULL, 0x0000000000000014ULL, 0x00000005dc00b800ULL, 0x0000000000000006ULL, 0x0000000000050003ULL, 0x0000000000000047ULL, 0x508b400000008001ULL, 0x0000000000053531ULL, 0x0000008000000080ULL, 0x0000008000000080ULL, 0x0000008000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000066000000800ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL };

static u64 Env_3[12] = { 0x0000000000000000ULL, 0x5100000500000000ULL, 0x1000000000000004ULL, 0x000000000000000eULL, 0x0000000000000000ULL, 0x0000000000000050ULL, 0x0000000000000000ULL, 0x0000000000000051ULL, 0x0000000000000000ULL, 0x0000000000000052ULL, 0x0000000000000000ULL, 0x0000000000000053ULL };

static void (*func_tbl_4[])(FontDrawCursor *context, int param) = { PrintLine, (void (*)(FontDrawCursor *, int))PrintWindow, (void (*)(FontDrawCursor *, int))PrintSprite00, PrintRibbon, (void (*)(FontDrawCursor *, int))PrintPoint, (void (*)(FontDrawCursor *, int))ScissorSet, (void (*)(FontDrawCursor *, int))ScissorReset, (void (*)(FontDrawCursor *, int))PrintTagFont, (void (*)(FontDrawCursor *, int))PrintSprite01, PrintFrame, (void (*)(FontDrawCursor *, int))PrintNumber, (void (*)(FontDrawCursor *, int))PrintLine2, (void (*)(FontDrawCursor *, int))ZScissor, (void (*)(FontDrawCursor *, int))PrintThumbnail, (void (*)(FontDrawCursor *, int))FontTexChange, (void (*)(FontDrawCursor *, int))FontTexReload, PrintEtherLine, PrintCircle, (void (*)(FontDrawCursor *, int))DrawBackSet, (void (*)(FontDrawCursor *, int))DrawBackReset, (void (*)(FontDrawCursor *, int))ScreenClear, (void (*)(FontDrawCursor *, int))PrintBackSprite, (void (*)(FontDrawCursor *, int))PrintBackSprite2 };

static FontExtFunc func_tbl2_5[] = { (FontExtFunc)PrintFlush, (FontExtFunc)ScissorSet, (FontExtFunc)ScissorReset, (FontExtFunc)FontTexChange, (FontExtFunc)FontTexReload, (FontExtFunc)ZScissor };

static u64 JpegEnv01_8[20] = { 0x908b400000008001ULL, 0x000000053531eeeeULL, 0x0000000000000000ULL, 0x000000000000003fULL, 0x0000000000000000ULL, 0x0000000000000014ULL, 0x0000000664023800ULL, 0x0000000000000006ULL, 0x0000000000000005ULL, 0x0000000000000008ULL, 0x0000008000000080ULL, 0x0000008000000080ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x00007bf800007c00ULL, 0x0000000010000000ULL, 0x00000e0000002000ULL, 0x0000000000000000ULL, 0x000083f800008400ULL, 0x0000000010000000ULL };

static u64 JpegEnv11_10[20] = { 0x908b400000008001ULL, 0x000000053531eeeeULL, 0x0000000000000000ULL, 0x000000000000003fULL, 0x0000000000000000ULL, 0x0000000000000014ULL, 0x0000000664023800ULL, 0x0000000000000006ULL, 0x0000000000000005ULL, 0x0000000000000008ULL, 0x0000008000000080ULL, 0x0000008000000080ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x00007bf800007c00ULL, 0x0000000010000000ULL, 0x00000e0000002000ULL, 0x0000000000000000ULL, 0x000083f800008400ULL, 0x0000000010000000ULL };
