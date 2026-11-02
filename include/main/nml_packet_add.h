#ifndef INCLUDE_MAIN_NML_PACKET_ADD_H
#define INCLUDE_MAIN_NML_PACKET_ADD_H

#include "shared.h"
#include "main/ssd_init.h"
#include "main/xgl_2.h"

/* One stealth part: two joints, outline widths, end extensions and the
 * longitudinal stretch applied to each of the four cross-section rings. */
struct NmlStealthDescriptor {
    int firstJoint;
    int secondJoint;
    unsigned char unmodeled_08[4];
    int sortClass;
    float firstWidth;
    float secondWidth;
    float firstExtension;
    float secondExtension;
    float longitudinalStretch[4];
};

typedef union GsTag {
    u64 value;
    u32 words[2];
} GsTag;

/* RssdWork storage is defined by main/tu107; its layout is owned by main/tu110. */
extern RssdWorkFlags RssdWork;

extern const char nmlEmptyName[4];

/* GIF tag words: EOP flag, one register per loop, register A+D. */
#define NML_GIF_EOP 0x8000
#define NML_GIF_NREG_1 0x10000000
#define NML_GIF_REG_AD 0xE

/* GS pixel storage formats (PSM). */
enum NmlGsPsm {
    NML_GS_PSMCT32 = 0,
    NML_GS_PSMT8 = 0x13,
    NML_GS_PSMT4 = 0x14
};

/* GS privileged-register addresses used as the A+D address of a GIF packet. */
enum NmlGsRegister {
    NML_GS_REG_PRIM = 0x00,
    NML_GS_REG_RGBAQ = 0x01,
    NML_GS_REG_ST = 0x02,
    NML_GS_REG_UV = 0x03,
    NML_GS_REG_XYZF2 = 0x04,
    NML_GS_REG_TEX0_1 = 0x06,
    NML_GS_REG_CLAMP_1 = 0x08,
    NML_GS_REG_TEX1_1 = 0x14,
    NML_GS_REG_FOGCOL = 0x3d,
    NML_GS_REG_TEXFLUSH = 0x3f,
    NML_GS_REG_SCISSOR_1 = 0x40,
    NML_GS_REG_SCISSOR_2 = 0x41,
    NML_GS_REG_ALPHA_1 = 0x42,
    NML_GS_REG_ALPHA_2 = 0x43,
    NML_GS_REG_TEST_1 = 0x47,
    NML_GS_REG_TEST_2 = 0x48,
    NML_GS_REG_PABE = 0x49,
    NML_GS_REG_FBA_1 = 0x4a,
    NML_GS_REG_FBA_2 = 0x4b,
    NML_GS_REG_FRAME_1 = 0x4c,
    NML_GS_REG_FRAME_2 = 0x4d,
    NML_GS_REG_ZBUF_1 = 0x4e,
    NML_GS_REG_ZBUF_2 = 0x4f,
    NML_GS_REG_BITBLTBUF = 0x50,
    NML_GS_REG_TRXPOS = 0x51,
    NML_GS_REG_TRXREG = 0x52,
    NML_GS_REG_TRXDIR = 0x53
};

/* GS TEST register bits and the combinations the renderer loads. */
enum NmlGsTest {
    NML_GS_TEST_ATE = 0x1,
    NML_GS_TEST_AFAIL_FB_ONLY = 0x1000,
    NML_GS_TEST_AFAIL_RGB_ONLY = 0x3000,
    NML_GS_TEST_DATE = 0x4000,
    NML_GS_TEST_DATM = 0x8000,
    NML_GS_TEST_ZTE = 0x10000,
    NML_GS_TEST_ZTST_ALWAYS = 0x20000,
    NML_GS_TEST_ZTST_GEQUAL = 0x40000,
    NML_GS_TEST_ZTST_GREATER = 0x60000,
    NML_GS_TEST_ZGEQUAL = 0x50000,      /* depth test on, GEQUAL */
    NML_GS_TEST_ZALWAYS = 0x30000,      /* depth test on, ALWAYS (no depth reject) */
    /* alpha test NEVER with failure writing the frame buffer only: colour
     * is drawn, depth is not written. */
    NML_GS_TEST_COLOR_ONLY = 0x31001
};

/* Field values passed to nmlPacketMakeGsTest. */
enum NmlGsTestMode {
    NML_GS_ATST_LEQUAL = 3,
    NML_GS_ATST_GEQUAL = 5,
    NML_GS_ZTST_GEQUAL = 2
};

/* RGBAQ colour 0x80 per channel: unmodulated (1.0) vertex colour. */
#define NML_GS_RGBA_NEUTRAL 0x80808080

/* GS PRIM register: primitive type SPRITE, alpha blending enable. */
#define NML_GS_PRIM_SPRITE 6
#define NML_GS_PRIM_ABE 0x40
#define NML_GS_PRIM_TRISTRIP 4
#define NML_GS_PRIM_TME 0x10
#define NML_GS_PRIM_FST 0x100

/* XYZF2 high word: Z = 0xffffff (far plane), F = 0x7f. */
#define NML_GS_XYZF_FAR_Z 0x7fffffff00000000ULL

/* XYZ2 / XYZF2 corners of the 512x448 screen in 12.4 GS coordinates. */
#define NML_GS_XY_TOP_LEFT 0x72007000
#define NML_GS_XY_BOTTOM_RIGHT 0x8e009000u

/* RGBAQ high word: Q = 1.0f. */
#define NML_GS_RGBAQ_Q_ONE 0x3f80000000000000ULL

/* GS ZBUF register: 24-bit depth format (PSMZ24). */
#define NML_GS_ZBUF_PSMZ24 0x01000000

/* GS ALPHA register settings (A,B,C,D selectors in bits 0-7, FIX in 32-39). */
enum NmlGsAlpha {
    NML_GS_ALPHA_SUBTRACT = 0x42,       /* (0 - Cs) * As + Cd */
    NML_GS_ALPHA_BLEND_FIXED = 0x64,    /* (Cs - Cd) * FIX + Cd */
    NML_GS_ALPHA_BLEND = 0x44,          /* (Cs - Cd) * As + Cd */
    NML_GS_ALPHA_ADD = 0x48,            /* Cs * As + Cd */
    NML_GS_ALPHA_FIXED = 0x68,          /* Cs * FIX + Cd, FIX in the high word */
    NML_GS_ALPHA_DEST_BY_SRC = 0x89     /* Cd * As */
};

/* Display size of the 512x448 frame. */
#define NML_SCREEN_WIDTH 512
#define NML_SCREEN_HEIGHT 448

/* Display-area scissor of the 512x448 frame: SCAX1 / SCAY1. */
#define NML_SCISSOR_MAX_X 0x1ff
#define NML_SCISSOR_MAX_Y 0x1bf
#define NML_GS_SCISSOR_FULL 0x01bf000001ff0000ULL

/* Render-kind microcode families (VU1 programs) selected for a block. */
enum NmlMicrocode {
    NML_UCODE_ENV = 1,        /* NewVu1EnvGsKick */
    NML_UCODE_PLAIN = 2,      /* NewVu1GsKick */
    NML_UCODE_PROREAL = 3,    /* NewVu1ProGsKick */
    NML_UCODE_ADD_ENV = 4,    /* NewVu1AddEnvGsKick */
    NML_UCODE_ADD = 5,        /* NewVu1AddGsKick */
    NML_UCODE_TEX_ENV = 6,    /* NewVu1TexEnvGsKick */
    NML_UCODE_DROP = 7        /* NewVu1DropGsKick */
};

void nmlPacketAddGsScissor(u64 scissor);

/* Functions and data used by other translation units. */
void nmlPacketGsInit(void);
void nmlPacketAddGsFlush(void);
void nmlPacketAddGsFrame(int frame_base, int frame_mask);
void nmlPacketAddGsFlushWide(void);
void nmlPacketAddGsFrame1(int frame_base, int frame_mask);
void *nmlPacketSetAttributeData16N(const void *data, int count);
void nmlPacketSetCurrent(void);

#endif /* INCLUDE_MAIN_NML_PACKET_ADD_H */
