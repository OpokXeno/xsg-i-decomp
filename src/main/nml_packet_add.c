#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/nml_packet_add.h"
#include "nml_packet_add.h"
#include "main/xgl_packet.h"
#include "main/ssd_init.h"

/* VIF command MSCAL: start the VU1 microprogram at immediate * 8. */
#define VIF_CODE_MSCAL 0x14000000

/* VIF command FLUSH: wait for the VU1 microprogram to finish. */
#define VIF_CODE_FLUSH 0x11000000

#define VIF_CODE_MSCNT 0x17000000

typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union NmlPacketData128 {
    u32 words[4];
    float lanes[4];
    Quadword quad;
} NmlPacketData128;

typedef unsigned int GameLoopStateWords[];

typedef struct NmlScreenGsScratchEntry {
    GsTag payload;
    GsTag address;
} NmlScreenGsScratchEntry;

typedef struct NmlAttributePacket NmlAttributePacket;

/* One current packet is shared by the SDK writer and attribute allocator. */
typedef union NmlCurrentPacket {
    XglPacket *sdk;
    NmlAttributePacket *attributes;
} NmlCurrentPacket;

/*
 * One s_aUcodeTbl row (0x1c bytes; the 168-byte symbol holds six rows).
 * programs[type - 1] lists the VU1 program byte addresses of microcode family
 * `type`, indexed by the material's variant slot.
 */
typedef struct UcodeTableRow {
    u32 *programs[7];
} UcodeTableRow;

/* The allocation cursor addresses byte payloads and aligned quadword
 * payloads in the same packet buffer. Both views are used below. */
typedef union NmlAttributeCursor {
    u8 *bytes;
    Quadword *quadwords;
} NmlAttributeCursor;

struct NmlAttributePacket {
    unsigned char unmodeled_00[0x20];
    u32 limit;
    NmlAttributeCursor cursor;
};

typedef struct NmlPixelTestPacket {
    u32 loops;
    u32 format;
    u32 registers;
    u32 register_high;
    u64 first_value;
    u64 first_address;
    u64 second_value;
    u64 second_address;
} NmlPixelTestPacket;

typedef struct NmlZbufRenderState {
    unsigned char unmodeled_00[2];
    short frame_psm;
    short frame_width;
    short frame_height; /* +0x6 */
    u16 depth_buffer_base;
    unsigned char unmodeled_0a[0x10]; /* +0xa */
    u16 texture_buffer_base; /* +0x1a */
    unsigned char unmodeled_1c[4]; /* +0x1c */
    u16 display_buffer_base; /* +0x20 */
} NmlZbufRenderState;

typedef union NmlReflectionPacketTag {
    u32 words[4];
    u64 halves[2];
} NmlReflectionPacketTag;

typedef union NmlCircleVector {
    Vector4 vector;
    float lanes[4];
    int words[4];
    u32 unsigned_words[4];
} NmlCircleVector;

typedef union NmlCircleOffsetTable {
    float values[4];
    u64 pairs[2];
} NmlCircleOffsetTable;

typedef struct NmlCircleScratchpad {
    NmlCircleVector position[4];
    NmlCircleVector transform[4];
    NmlCircleVector projected[4];
    NmlCircleVector perspective[4];
} NmlCircleScratchpad;

typedef struct NmlCircleGsRecord {
    GsTag value;
    GsTag address;
} NmlCircleGsRecord;

typedef union NmlPartsScratchMatrix {
    Matrix4 values;
    Matrix xgl_matrix;
    NmlScreenQuadword quadwords[4];
    Vector4 rows[4];
    float elements[16];
} NmlPartsScratchMatrix;

typedef struct NmlPartsLightPayload {
    Vector4 rows[3];
    Vector4 current_point;
} NmlPartsLightPayload;

typedef struct NmlPartsWorkingScratch {
    NmlPartsScratchMatrix part;
    NmlPartsScratchMatrix normal;
    NmlPartsReferenceScratch reference_point;
    NmlPartsLightPayload light_payload;
    NmlPartsScratchMatrix inverse_part;
    NmlPartsScratchMatrix projected_placement;
    NmlPartsScratchMatrix clip;
} NmlPartsWorkingScratch;

extern const u32 D_004AE5A0[];
extern const u8 D_004AE5B0[];
extern GameLoopStateWords GameLoopState;
extern const float inReflRotYStep;
extern int g_aSubWindow[4];
extern int g_nGsEntry;
extern NmlZbufRenderState sRender;
extern const u8 NewVu1EnvGsKick[];
extern const u8 NewVu1GsKick[];
extern const u8 NewVu1ProGsKick[];
extern const u8 NewVu1AddEnvGsKick[];
extern const u8 NewVu1AddGsKick[];
extern const u8 NewVu1TexEnvGsKick[];
extern const u8 NewVu1DropGsKick[];
extern const u8 NewVu1EnvGsKickWide[];
extern const u8 NewVu1GsKickWide[];
extern const u8 NewVu1ProGsKickWide[];
extern const u8 NewVu1AddEnvGsKickWide[];
extern const u8 NewVu1AddGsKickWide[];
extern const u8 NewVu1TexEnvGsKickWide[];
extern const u8 NewVu1DropGsKickWide[];
extern const u8 D_004B19F0[];
extern const u8 D_004B54B0[];
extern const u8 D_004B9710[];
extern const u8 D_004B6F80[];
extern const u8 D_004B0A50[];
extern const u8 D_004BD4A0[];
extern const u32 D_004B0A40[];
extern const u32 D_004B19E0[];
extern const u32 D_004B54A0[];
extern const u32 D_004B6F70[];
extern const u32 D_004B9700[];
extern const u32 D_004BD490[];
extern const float alphaLimitHigh;
extern const float alphaLimitLow;
extern const float vectorLengthHigh;
extern const float vectorLengthLow;
extern const float previousVectorLengthHigh;
extern const float previousVectorLengthLow;
extern float circleRadiusScale;
extern const float circleHeightOffset;
extern const NmlCircleOffsetTable circleXOffsetTable;
extern const NmlCircleOffsetTable circleZOffsetTable;

/* One VIF unpack value passed to sceVif1PkAddUpkData128 by value. */
extern void sceVif1PkAddUpkData128(XglPacket *packet, Quadword data);
extern void *memcpy(void *destination, const void *source, unsigned int count);
extern int sceVif1PkSize(XglPacket *packet);

void nmlPacketAddPixelTestPacket(int first_address, u64 first_value,
                               u64 first_test, int second_address,
                               u64 second_value, u64 second_test);

extern void sceGsSetDefLoadImage(XglLoadImage *image, short address,
                               short width, short psm, short x, short y,
                               short w, short h);

extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data,
                                 int count);

extern void sceVif1PkCloseDirectCode(XglPacket *packet);
extern void xglPacketTextureTrans(const void *texture);
static void nmlPacketTextureTrans(LayoutStore *model);
static int s_nPixelTestType;

static int s_nModelParts;

static NmlCurrentPacket s_pPacket = { 0 };

static const void *s_pCacheTexture = 0;

static const u16 *s_pMatrixCache = 0;

static void *s_pModelCache = 0;

static const LayoutStore *s_pModelLayout = 0;

static void *s_pLightLayout = 0;

static int s_nProgType = -1;

static int s_nReflRotType = -1;

static float s_inReflRotX = 0.0f;

static float s_inReflRotY = 0.0f;

const char eye_name[8] = "eye";

const char lenz_name[8] = "lenz";

static u32 s_aUcodeEnvAdrZbuf[12] = { 1456U, 1456U, 1456U, 1456U, 6032U, 6032U, 6032U, 6032U, 10320U, 10320U, 10320U, 10320U };

static u32 s_aUcodeAdrZbuf[6] = { 3032U, 3552U, 5144U, 5144U, 5144U, 5144U };

static u32 s_aUcodeProSkipAdr[7] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

static u32 s_aUcodeAddEnvAdrZbuf[12] = { 1784U, 1784U, 1784U, 1784U, 6488U, 6488U, 6488U, 6488U, 10904U, 10904U, 10904U, 10904U };

static u32 s_aUcodeAddAdrZbuf[5] = { 0U, 0U, 0U, 0U, 0U };

static u32 s_aUcodeTexEnvNull[5] = { 0U, 0U, 0U, 0U, 0U };

static u32 s_aUcodeDropNull[5] = { 0U, 0U, 0U, 0U, 0U };

static u32 s_aUcodeEnvFaceAdr[12] = { 2160U, 3312U, 4536U, 5920U, 6680U, 7760U, 8904U, 10208U, 11128U, 12264U, 13472U, 14840U };

static u32 s_aUcodeFaceAdr[6] = { 3984U, 4576U, 5704U, 6744U, 7856U, 9128U };

static u32 s_aUcodeProFaceAdr[6] = { 2216U, 3000U, 3704U, 4584U, 5272U, 6200U };

static u32 s_aUcodeAddEnvFaceAdr[12] = { 2520U, 3704U, 4960U, 6376U, 7168U, 8280U, 9456U, 10792U, 11744U, 12912U, 14152U, 15552U };

static u32 s_aUcodeAddFaceAdr[5] = { 3808U, 8008U, 8304U, 8600U, 9272U };

static u32 s_aUcodeTexEnv[5] = { 3432U, 0U, 1776U, 2224U, 2744U };

static u32 s_aUcodeDrop[5] = { 3200U, 0U, 1776U, 2296U, 2712U };

static u32 s_aUcodeEnvBackAdr[12] = { 2216U, 3368U, 4592U, 5976U, 6736U, 7816U, 8960U, 10264U, 11184U, 12320U, 13528U, 14896U };

static u32 s_aUcodeBackAdr[6] = { 4824U, 4896U, 5792U, 5792U, 5792U, 5792U };

static u32 s_aUcodeProBackAdr[6] = { 0U, 0U, 0U, 0U, 0U, 0U };

static u32 s_aUcodeAddEnvBackAdr[12] = { 2576U, 3760U, 5016U, 6432U, 7224U, 8336U, 9512U, 10848U, 11800U, 12968U, 14208U, 15608U };

static u32 s_aUcodeAddBackAdr[5] = { 4392U, 9760U, 9760U, 9760U, 9760U };

static u32 s_aUcodeEnvAdr[12] = { 1456U, 2272U, 3424U, 4648U, 6032U, 6792U, 7872U, 9016U, 10320U, 11240U, 12376U, 13584U };

static u32 s_aUcodeAdr[6] = { 3032U, 3552U, 5144U, 5880U, 6920U, 8032U };

static u32 s_aUcodeProAdr[6] = { 2216U, 3000U, 3704U, 4584U, 5272U, 6200U };

static u32 s_aUcodeAddEnvAdr[12] = { 1784U, 2632U, 3816U, 5072U, 6488U, 7280U, 8392U, 9568U, 10904U, 11856U, 13024U, 14264U };

static u32 s_aUcodeAddAdr[5] = { 3232U, 4464U, 5048U, 5912U, 6880U };

static u32 s_aUcodeEnvNullAdr[12] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };

static u32 s_aUcodeAddEnvNullAdr[12] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };

static UcodeTableRow s_aUcodeTbl[6] = {
    {{s_aUcodeEnvAdrZbuf, s_aUcodeAdrZbuf, s_aUcodeProSkipAdr,
      s_aUcodeAddEnvAdrZbuf, s_aUcodeAddAdrZbuf, s_aUcodeTexEnvNull,
      s_aUcodeDropNull}},
    {{s_aUcodeEnvFaceAdr, s_aUcodeFaceAdr, s_aUcodeProFaceAdr,
      s_aUcodeAddEnvFaceAdr, s_aUcodeAddFaceAdr, s_aUcodeTexEnv,
      s_aUcodeDrop}},
    {{s_aUcodeEnvBackAdr, s_aUcodeBackAdr, s_aUcodeProBackAdr,
      s_aUcodeAddEnvBackAdr, s_aUcodeAddBackAdr, s_aUcodeTexEnv,
      s_aUcodeDrop}},
    {{s_aUcodeEnvAdr, s_aUcodeAdr, s_aUcodeProAdr,
      s_aUcodeAddEnvAdr, s_aUcodeAddAdr, s_aUcodeTexEnv,
      s_aUcodeDrop}},
    {{s_aUcodeEnvAdr, s_aUcodeFaceAdr, s_aUcodeProFaceAdr,
      s_aUcodeAddEnvAdr, s_aUcodeAddFaceAdr, s_aUcodeTexEnv,
      s_aUcodeDrop}},
    {{s_aUcodeEnvNullAdr, s_aUcodeBackAdr, s_aUcodeProBackAdr,
      s_aUcodeAddEnvNullAdr, s_aUcodeAddBackAdr, s_aUcodeTexEnv,
      s_aUcodeDrop}}
};

/*
 * The GS packet buffer, 0x510 bytes of 8-byte slots: the GIF tag occupies the
 * first two slots, then each queued register write takes two slots, its data
 * at [2 * count + 2] and its register address at [2 * count + 3]. Some writers
 * store the data as two 32-bit words.
 */
GsTag g_aGsTag[162] = {{0}};

RssdWorkFlags RssdWork = {0};

char RssdStrWork[32] = {0};

const char nmlEmptyName[4] = "";

static int s_nLightPara;

struct NmlStealthDescriptor s_aStealthDat[10] = { { .firstJoint = 9, .secondJoint = 12, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.18000000715255737f, .secondWidth = 0.18000000715255737f, .firstExtension = 0.0f, .secondExtension = -0.10000000149011612f, .longitudinalStretch = { 1.0f, 0.94999998807907104f, 0.89999997615814209f, 0.60000002384185791f } }, { .firstJoint = 1, .secondJoint = 2, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.14000000059604645f, .secondWidth = 0.14000000059604645f, .firstExtension = 0.10000000149011612f, .secondExtension = 0.0f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 2, .secondJoint = 3, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.11999999731779099f, .secondWidth = 0.18000000715255737f, .firstExtension = -0.10000000149011612f, .secondExtension = 0.10000000149011612f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 5, .secondJoint = 6, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.14000000059604645f, .secondWidth = 0.14000000059604645f, .firstExtension = 0.10000000149011612f, .secondExtension = 0.0f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 6, .secondJoint = 7, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.11999999731779099f, .secondWidth = 0.18000000715255737f, .firstExtension = -0.10000000149011612f, .secondExtension = 0.10000000149011612f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 12, .secondJoint = 13, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.17000000178813934f, .secondWidth = 0.17000000178813934f, .firstExtension = -0.25f, .secondExtension = 0.05000000074505806f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 15, .secondJoint = 17, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.079999998211860657f, .secondWidth = 0.079999998211860657f, .firstExtension = 0.0f, .secondExtension = 0.0f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 17, .secondJoint = 19, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.090000003576278687f, .secondWidth = 0.090000003576278687f, .firstExtension = -0.0f, .secondExtension = 0.15000000596046448f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 21, .secondJoint = 23, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.079999998211860657f, .secondWidth = 0.079999998211860657f, .firstExtension = 0.0f, .secondExtension = 0.0f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } }, { .firstJoint = 23, .secondJoint = 25, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.090000003576278687f, .secondWidth = 0.090000003576278687f, .firstExtension = -0.0f, .secondExtension = 0.15000000596046448f, .longitudinalStretch = { 1.0f, 0.89999997615814209f, 0.69999998807907104f, 0.0f } } };

struct NmlStealthDescriptor s_inStealthUnitDat = { .firstJoint = 10000, .secondJoint = 10000, .unmodeled_08 = { 0, 0, 0, 0 }, .sortClass = 1, .firstWidth = 0.059999998658895493f, .secondWidth = 0.059999998658895493f, .firstExtension = 0.0f, .secondExtension = 0.0f, .longitudinalStretch = { 1.0f, 0.94999998807907104f, 0.89999997615814209f, 0.60000002384185791f } };

char __ps2_klibinfo__[16] = "PsIIlibkernl2420";

static NmlPacketData128 s_inLc0[4];

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static const float s_aNormalTbl[114][4];

static inline void nmlPacketSelectLighting(LayoutStore *model,
                                           const u32 *material_flags)
{
    s_pLightLayout = model;
    s_nLightPara = 0x20000 & *material_flags;
}

static inline void nmlPacketAppendGsWords(u32 address, u32 high, u32 low)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = low;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = high;
    g_aGsTag[g_nGsEntry++ * 2 + 3].value = address;
}

static inline void nmlPacketAppendGsValue(u32 address, const u64 *value)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = *value;
    g_aGsTag[g_nGsEntry++ * 2 + 3].value = address;
}

static inline void nmlPacketAppendGsFields(u32 address, u32 high,
                                           const u32 *low)
{
    u32 low_word = *low;

    nmlPacketAppendGsWords(address, high, low_word);
}

static inline NmlCircleGsRecord *nmlCircleGsRecord(unsigned int index)
{
    return (NmlCircleGsRecord *)(0x70001000u +
                                 index * sizeof(NmlCircleGsRecord));
}

static inline void nmlCircleSetGsRecord(NmlCircleGsRecord *record,
                                        int address, u64 value)
{
    record->value.value = value;
    record->address.value = address;
}

static inline int nmlCircleSetPositionRecord(NmlCircleGsRecord *record,
                                              u64 packed_position,
                                              const int *clip_flags,
                                              int previous_flags,
                                              int earlier_flags)
{
    int flags;

    record->value.value = packed_position;
    flags = *clip_flags;
    record->address.value = (flags | previous_flags) | earlier_flags;
    return flags;
}

static inline u64 nmlCircleMakeTextureParameter(u32 texture_base)
{
    int low = 0x19304000;
    u64 parameter;

    low |= texture_base;
    parameter = low;
    parameter |= (u64)(texture_base + 0x80u) << 37;
    parameter |= 0x2000000580000000ULL;
    return parameter;
}

static inline u32 nmlPacketInvalidatePixelModel(
    NmlModel *const *resource, const u32 *material_flags)
{
    s_nPixelTestType = -1;
    s_pModelCache = *resource;
    return *material_flags;
}

static inline u64 nmlPacketMakeGsTest(int alpha_test_enabled, int alpha_test,
                                      int alpha_reference, int alpha_failure,
                                      int destination_alpha_enabled,
                                      int destination_alpha, int depth_enabled,
                                      int depth_test)
{
    u64 test = (u64)alpha_reference << 4;

    test |= (u64)depth_test << 17;
    test |= (u64)depth_enabled << 16;
    test |= (u64)alpha_failure << 12;
    test |= (u64)destination_alpha_enabled << 14;
    test |= (u64)destination_alpha << 15;
    test |= (u64)(alpha_test_enabled | (alpha_test << 1));
    return test;
}

static inline u64 nmlPacketMakeGsFixedAlpha(int alpha_value)
{
    return ((u64)alpha_value << 32) | 0x68;
}

static inline void nmlPacketSetPixelBlend(LayoutStore *model,
                                          int pixel_type, float alpha,
                                          float reflect_alpha,
                                          int alpha_reference,
                                          int use_alpha_reference)
{
    if (s_nPixelTestType != pixel_type) {
        int alpha_value =
            (int)(model->fields.transparency * 128.0f * alpha);
        u64 test_value = 0x50000;
        u64 blend_value;

        if (reflect_alpha < 2.0f)
            alpha_value = (int)(reflect_alpha * 128.0f * alpha);
        if (use_alpha_reference)
            test_value = nmlPacketMakeGsTest(1, 3, alpha_reference,
                                             0, 0, 0, 1, 2);
        blend_value = nmlPacketMakeGsFixedAlpha(alpha_value);
        s_nPixelTestType = pixel_type;
        nmlPacketAppendGsWords(0x47, 0, (u32)test_value);
        nmlPacketAppendGsValue(0x43, &blend_value);
        nmlPacketAddPixelTestPacket(0x47, test_value, 0x50000,
                                    0x42, *(u64 *)&model->fields.alpha,
                                    blend_value);
    }
}

static inline void nmlPartsCopyGeometryMatrix(NmlPartsScratchMatrix *destination,
                                              const Vector4 *source)
{
    __asm__ __volatile__(
        "lq $2,0(%1)\n\t"
        "sq $2,0(%0)\n\t"
        "lq $2,16(%1)\n\t"
        "sq $2,16(%0)\n\t"
        "lq $2,32(%1)\n\t"
        "sq $2,32(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)"
        :
        : "r"(destination->values), "r"(source)
        : "$2", "memory");
}

static inline void nmlPartsCopyLayoutMatrix(NmlPartsScratchMatrix *destination,
                                             const Vector4 *source)
{
    __asm__ __volatile__(
        "lq $2,0(%1)\n\t"
        "sq $2,0(%0)\n\t"
        "lq $2,16(%1)\n\t"
        "sq $2,16(%0)\n\t"
        "lq $2,32(%1)\n\t"
        "sq $2,32(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)"
        :
        : "r"(destination->values), "r"(source)
        : "$2", "memory");
}

static inline float nmlPartsSetRowW(NmlPartsScratchMatrix *matrix, float row_w)
{
    matrix->rows[0].w = row_w;
    matrix->rows[1].w = row_w;
    matrix->rows[2].w = row_w;
    return row_w;
}

static inline void nmlPartsSetNormalTranslation(NmlPartsScratchMatrix *normal,
    const NmlPartsReferenceScratch *point, int components)
{
    int component;

    for (component = 0; component < components; component++) {
        normal->values[component][3] = point->components[component];
        normal->values[3][component] = point->components[component];
    }
}

static void _CurMatrixSet(Matrix4 matrix)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _CurMatrixGet(Matrix4 matrix)
{
    __asm__ __volatile__(
        "sqc2 vf27, 0(%0)\n\t"
        "sqc2 vf28, 16(%0)\n\t"
        "sqc2 vf29, 32(%0)\n\t"
        "sqc2 vf30, 48(%0)"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _CurMatrixMul(Matrix4 matrix)
{
    __asm__ __volatile__(
        "lqc2 vf2, 0(%0)\n\t"
        "lqc2 vf3, 16(%0)\n\t"
        "lqc2 vf4, 32(%0)\n\t"
        "lqc2 vf5, 48(%0)\n\t"
        "vmulax.xyzw ACC, vf27, vf2x\n\t"
        "vmadday.xyzw ACC, vf28, vf2y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf2z\n\t"
        "vmaddw.xyzw vf2, vf30, vf2w\n\t"
        "vmulax.xyzw ACC, vf27, vf3x\n\t"
        "vmadday.xyzw ACC, vf28, vf3y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf3z\n\t"
        "vmaddw.xyzw vf3, vf30, vf3w\n\t"
        "vmulax.xyzw ACC, vf27, vf4x\n\t"
        "vmadday.xyzw ACC, vf28, vf4y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf4z\n\t"
        "vmaddw.xyzw vf4, vf30, vf4w\n\t"
        "vmulax.xyzw ACC, vf27, vf5x\n\t"
        "vmadday.xyzw ACC, vf28, vf5y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf5z\n\t"
        "vmaddw.xyzw vf30, vf30, vf5w\n\t"
        "vmulw.xyzw vf27, vf2, vf0w\n\t"
        "vmulw.xyzw vf28, vf3, vf0w\n\t"
        "vmulw.xyzw vf29, vf4, vf0w"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _CurApplyMatrix(Vector4 *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "vmulax.xyzw ACC, vf27, vf31x\n\t"
        "vmadday.xyzw ACC, vf28, vf31y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf31z\n\t"
        "vmaddw.xyzw vf31, vf30, vf0w\n\t"
        "sqc2 vf31, 0(%1)"
        :
        : "r"(source), "r"(destination)
        : "memory"
    );
}

static void _CurMatrixMul33norm(Vector4 *destination, Matrix4 source)
{
    __asm__ __volatile__(
        "lqc2 vf20, 0(%0)\n\t"
        "lqc2 vf21, 16(%0)\n\t"
        "lqc2 vf22, 32(%0)\n\t"
        "vmulax.xyz ACC, vf27, vf20x\n\t"
        "vmadday.xyz ACC, vf28, vf20y\n\t"
        "vmaddz.xyz vf20, vf29, vf20z\n\t"
        "vmulax.xyz ACC, vf27, vf21x\n\t"
        "vmadday.xyz ACC, vf28, vf21y\n\t"
        "vmaddz.xyz vf21, vf29, vf21z\n\t"
        "vmulax.xyz ACC, vf27, vf22x\n\t"
        "vmadday.xyz ACC, vf28, vf22y\n\t"
        "vmaddz.xyz vf22, vf29, vf22z\n\t"
        "vaddz.x vf25, vf0, vf20z\n\t"
        "vaddy.x vf24, vf0, vf20y\n\t"
        "vaddx.x vf23, vf0, vf20x\n\t"
        "vaddz.y vf25, vf0, vf21z\n\t"
        "vaddy.y vf24, vf0, vf21y\n\t"
        "vaddx.y vf23, vf0, vf21x\n\t"
        "vaddz.z vf25, vf0, vf22z\n\t"
        "vaddy.z vf24, vf0, vf22y\n\t"
        "vaddx.z vf23, vf0, vf22x\n\t"
        "vmula.xyz ACC, vf23, vf23\n\t"
        "vmadda.xyz ACC, vf24, vf24\n\t"
        "vmadd.xyz vf10, vf25, vf25\n\t"
        "vrsqrt Q, vf0w, vf10x\n\t"
        "vwaitq\n\t"
        "vmulq.x vf23, vf23, Q\n\t"
        "vmulq.x vf24, vf24, Q\n\t"
        "vmulq.x vf25, vf25, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf10y\n\t"
        "vwaitq\n\t"
        "vmulq.y vf23, vf23, Q\n\t"
        "vmulq.y vf24, vf24, Q\n\t"
        "vmulq.y vf25, vf25, Q\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vrsqrt Q, vf0w, vf10z\n\t"
        "vwaitq\n\t"
        "vmulq.z vf23, vf23, Q\n\t"
        "vmulq.z vf24, vf24, Q\n\t"
        "vmulq.z vf25, vf25, Q\n\t"
        "sqc2 vf23, 0(%1)\n\t"
        "sqc2 vf24, 16(%1)\n\t"
        "sqc2 vf25, 32(%1)"
        :
        : "r"(source), "r"(destination)
        : "memory"
    );
}

static void _CurSetViewScaleTrans(const Vector4 *view_scale, const Vector4 *view_translation)
{
    __asm__ __volatile__(
        "lqc2 vf25, 0(%0)\n\t"
        "lqc2 vf26, 0(%1)"
        :
        : "r"(view_scale), "r"(view_translation)
        : "memory"
    );
}

static int _CurRotTransPersClip(
    Vector4 *projectedPosition,
    Vector4 *perspectiveCoordinates,
    const Vector4 *position,
    const Vector4 *coordinates)
{
    register int clipFlags asm("$2");
    /* The SQC2 destination is an EE32 address in GPR4. After that
     * hardware use ends, the same word carries the masked clip flags. */
    register unsigned int destinationOrFlags asm("$4") = (unsigned int)projectedPosition;

    __asm__ __volatile__(
        "ctc2 $0,$vi18\n\t"
        "lqc2 $vf31,0(%3)\n\t"
        "lqc2 $vf20,0(%4)\n\t"
        "vmulax.xyzw ACC,vf27xyzw,vf31x\n\t"
        "vmadday.xyzw ACC,vf28xyzw,vf31y\n\t"
        "vmaddaz.xyzw ACC,vf29xyzw,vf31z\n\t"
        "vmaddw.xyzw vf31xyzw,vf30xyzw,vf0w\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vclipw.xyz vf31xyz,vf31w\n\t"
        "vdiv Q,vf0w,vf31w\n\t"
        "vwaitq\n\t"
        "vmulq.xyzw vf31xyzw,vf31xyzw,Q\n\t"
        "vmulq.xyzw vf20xyzw,vf20xyzw,Q\n\t"
        "vmulaw.xyzw ACC,vf26xyzw,vf0w\n\t"
        "vmadd.xyzw vf31xyzw,vf31xyzw,vf25xyzw\n\t"
        "vftoi4.xyw vf23xyw,vf31xyw\n\t"
        "vftoi0.z vf23z,vf31z\n\t"
        "vsub.w vf23w,vf23w,vf23w\n\t"
        "sqc2 $vf23,0(%1)\n\t"
        "sqc2 $vf20,0(%2)\n\t"
        "cfc2 %0,$vi18\n\t"
        "nop"
        : "=r"(clipFlags)
        : "r"(destinationOrFlags), "r"(perspectiveCoordinates),
          "r"(position), "r"(coordinates)
        : "memory"
    );
    clipFlags &= 0x3f;
    destinationOrFlags = clipFlags;
    __asm__ __volatile__("" : : "r"(destinationOrFlags), "r"(clipFlags));
    return destinationOrFlags;
}

static void _CurSetMatrix(Matrix4 matrix)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)"
        :
        : "r"(matrix)
        : "memory"
    );
}

void nmlPacketAddGsFlush(void)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    s_pPacket.sdk = packet;
    if (g_nGsEntry > 0) {
        g_aGsTag[0].words[1] = 0x10000000;
        g_aGsTag[1].words[0] = 0xE;
        g_aGsTag[0].words[0] = g_nGsEntry + 0x8000;
        g_aGsTag[1].words[1] = 0;
        sceVif1PkCnt(s_pPacket.sdk, 0);
        sceVif1PkOpenUpkCode(s_pPacket.sdk, 0x3D5, 0x6C, 1, 1);
        sceVif1PkAddUpkData128N(s_pPacket.sdk, g_aGsTag, g_nGsEntry + 1);
        sceVif1PkCloseUpkCode(s_pPacket.sdk);
        switch (s_nProgType) {
        case 7:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1DropGsKick >> 3) | VIF_CODE_MSCAL);
            break;
        case 1:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1EnvGsKick >> 3) | VIF_CODE_MSCAL);
            break;
        case 2:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1GsKick >> 3) | VIF_CODE_MSCAL);
            break;
        case 3:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1ProGsKick >> 3) | VIF_CODE_MSCAL);
            break;
        case 4:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1AddEnvGsKick >> 3) | VIF_CODE_MSCAL);
            break;
        case 5:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1AddGsKick >> 3) | VIF_CODE_MSCAL);
            break;
        case 6:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1TexEnvGsKick >> 3) | VIF_CODE_MSCAL);
            break;
        }
        sceVif1PkAddCode(s_pPacket.sdk, 0x17000000);
        nmlPacketAddWaitMicrocode();
    }
    g_nGsEntry = 0;
}

void nmlPacketAddGsFlushWide(void)
{
    XglPacket *packet = xglPacketGetCurrent();

    s_pPacket.sdk = packet;
    if (g_nGsEntry > 0) {
        g_aGsTag[0].words[1] = 0x10000000;
        g_aGsTag[1].words[0] = 0xE;
        g_aGsTag[0].words[0] = g_nGsEntry + 0x8000;
        g_aGsTag[1].words[1] = 0;
        sceVif1PkCnt(s_pPacket.sdk, 0);
        nmlPacketAddWaitMicrocode();
        sceVif1PkOpenUpkCode(s_pPacket.sdk, 0x2DC, 0x6C, 1, 1);
        sceVif1PkAddUpkData128N(s_pPacket.sdk, g_aGsTag, g_nGsEntry + 1);
        sceVif1PkCloseUpkCode(s_pPacket.sdk);
        switch (s_nProgType) {
        case 7:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1DropGsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        case 1:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1EnvGsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        case 2:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1GsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        case 3:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1ProGsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        case 4:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1AddEnvGsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        case 5:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1AddGsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        case 6:
            sceVif1PkAddCode(s_pPacket.sdk,
                             ((u32)NewVu1TexEnvGsKickWide >> 3) | VIF_CODE_MSCAL);
            break;
        }
        sceVif1PkAddCode(s_pPacket.sdk, 0x17000000);
        nmlPacketAddWaitMicrocode();
    }
    g_nGsEntry = 0;
}

void nmlPacketAddReflectParam(const NmlModelBlock *material,
                              const LayoutStore *model)
{
    NmlCurrentPacket *current_packet = &s_pPacket;
    NmlReflectionPacketTag tags[5];
    float reflection_alpha;

    s_pPacket.sdk = xglPacketGetCurrent();
    reflection_alpha = model->fields.reflTransparency;
    if ((model->fields.render_status & 0x100000) &&
        (strstr(material->name, eye_name) != 0))
        reflection_alpha = 2.0f;
    sceVif1PkCnt(current_packet->sdk, 0);

    tags[0].words[0] = 0x8004;
    tags[0].words[1] = 0x10000000;
    tags[0].words[2] = 0xE;
    tags[0].words[3] = 0;
    tags[1].halves[0] = 0;
    tags[1].halves[1] = 0x3F;
    tags[2].words[0] = material->textureParameters[0].buffer +
                       (sRender.texture_buffer_base << 5);
    tags[2].words[1] = material->textureParameters[0].width +
                       (sRender.texture_buffer_base << 10);
    tags[2].halves[1] = 6;
    tags[3].words[0] = material->textureParameters[1].buffer;
    tags[3].words[1] = material->textureParameters[1].width;
    tags[3].halves[1] = 0x14;
    tags[4].words[0] = material->textureParameters[2].buffer;
    tags[4].words[1] = material->textureParameters[2].width;
    tags[4].halves[1] = 8;
    sceVif1PkOpenUpkCode(current_packet->sdk, 0x3D5, 0x6C, 1, 1);
    sceVif1PkAddUpkData128N(current_packet->sdk, tags, 5);
    sceVif1PkCloseUpkCode(current_packet->sdk);

    tags[2].words[0] = material->textureParameters[3].buffer +
                       (sRender.texture_buffer_base << 5);
    tags[2].words[1] = material->textureParameters[3].width +
                       (sRender.texture_buffer_base << 10);
    tags[3].words[0] = material->textureParameters[4].buffer;
    tags[3].words[1] = material->textureParameters[4].width;
    tags[4].words[0] = material->textureParameters[5].buffer;
    tags[4].words[1] = material->textureParameters[5].width;
    sceVif1PkOpenUpkCode(current_packet->sdk, 0x3DA, 0x6C, 1, 1);
    sceVif1PkAddUpkData128N(current_packet->sdk, tags, 5);
    sceVif1PkCloseUpkCode(current_packet->sdk);

    {
        u32 alpha[4];

        alpha[0] = (int)(material->reflectAlpha * 128.0f);
        if (reflection_alpha < 2.0f)
            alpha[0] = (int)(reflection_alpha * 128.0f);
        alpha[1] = alpha[0];
        alpha[2] = alpha[0];
        alpha[3] = alpha[0];
        sceVif1PkOpenUpkCode(current_packet->sdk, 0x3F1, 0x6C, 1, 1);
        sceVif1PkAddUpkData128N(current_packet->sdk, alpha, 1);
        sceVif1PkCloseUpkCode(current_packet->sdk);
    }
}

void nmlPacketAddTransMicrocodeInit(void)
{
    Matrix4 matrix;
    NmlCurrentPacket selected;
    XglPacket *packet = xglPacketGetCurrent();

    selected.sdk = packet;
    s_pPacket = selected;
    sceVif1PkRef(packet, D_004AE5B0, D_004AE5A0[0], 0, 0, 0);
    s_nProgType = 2;
    s_nPixelTestType = -1;
    s_pCacheTexture = 0;
    s_pMatrixCache = 0;
    s_pModelCache = 0;
    s_nModelParts = 0;
    s_pLightLayout = 0;
    s_inReflRotX = xglSin(s_inReflRotY);
    if (!(GameLoopState[4] & 1))
        s_inReflRotY += inReflRotYStep;
    xglMatrixUnit(matrix);
    selected = s_pPacket;
    sceVif1PkCnt(selected.sdk, 0);
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 1004, 108, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk, matrix, 3);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);
    s_nReflRotType = 1;
    /* The SDK zero-vector operation clears all four light parcels. */
    __asm__ __volatile__(
        "sq $0, 0(%0)\n\t"
        "sq $0, 16(%0)\n\t"
        "sq $0, 32(%0)\n\t"
        "sq $0, 48(%0)"
        : : "r"(s_inLc0) : "memory"
    );
    s_inLc0[3].lanes[0] = s_inLc0[3].lanes[1] =
        s_inLc0[3].lanes[2] = s_inLc0[3].lanes[3] = 1.0f;
}

void nmlPacketAddTransMicrocode(const NmlModelBlock *material,
                                const LayoutStore *model)
{
    NmlCurrentPacket selected;
    u32 status;
    int program_type;

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    status = model->fields.render_status;
    program_type = 2;
    if (status & 0x8000) {
        program_type = 6;
    } else if (status & 0x800) {
        program_type = 3;
    } else if (status & 0x80000) {
        program_type = 7;
    } else {
        if (status & 0x02001000) {
            program_type = 5;
            if (material->flags & 1) {
                program_type = 4;
            } else if ((material->materialFlagsQword & 0x10010) == 0x10) {
                program_type = 2;
            }
        } else {
            program_type = (material->flags & 1) ? 1 : 2;
        }
    }

    switch (program_type) {
    case 3:
        if (s_nProgType == 3)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004B54B0, D_004B54A0[0], 0, 0, 0);
        s_nProgType = 3;
        selected = s_pPacket;
        sceVif1PkCnt(selected.sdk, 0);
        selected = s_pPacket;
        sceVif1PkOpenUpkCode(selected.sdk, 0x350, 0x6C, 1, 1);
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, s_aNormalTbl, 0x72);
        selected = s_pPacket;
        sceVif1PkCloseUpkCode(selected.sdk);
        return;
    case 4:
        if (s_nProgType == 4)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004B9710, D_004B9700[0], 0, 0, 0);
        s_nProgType = 4;
        return;
    case 5:
        if (s_nProgType == 5)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004B6F80, D_004B6F70[0], 0, 0, 0);
        s_nProgType = 5;
        selected = s_pPacket;
        sceVif1PkCnt(selected.sdk, 0);
        selected = s_pPacket;
        sceVif1PkOpenUpkCode(selected.sdk, 0x350, 0x6C, 1, 1);
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, s_aNormalTbl, 0x72);
        selected = s_pPacket;
        sceVif1PkCloseUpkCode(selected.sdk);
        return;
    case 6:
        if (s_nProgType == 6)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004B0A50, D_004B0A40[0], 0, 0, 0);
        s_nProgType = 6;
        return;
    case 7:
        if (s_nProgType == 7)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004BD4A0, D_004BD490[0], 0, 0, 0);
        s_nProgType = 7;
        return;
    case 1:
        if (s_nProgType == 1)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004B19F0, D_004B19E0[0], 0, 0, 0);
        s_nProgType = 1;
        return;
    case 2:
        if (s_nProgType == 2)
            return;
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, D_004AE5B0, D_004AE5A0[0], 0, 0, 0);
        s_nProgType = 2;
        return;
    default:
        return;
    }

}

void nmlPacketAddScreen(const StudioCamera *camera)
{
    NmlPacketData128 translation;
    XglPacket *packet = xglPacketGetCurrent();

    s_pPacket.sdk = packet;
    sceVif1PkCnt(packet, 0);
    /* The SDK copy pins its quadword scratch register; the second parcel
     * below is an ordinary native-TI argument load. */
    __asm__ __volatile__(
        "lq $2, 0(%1)\n\t"
        "sq $2, 0(%0)"
        :
        : "r"(&translation), "r"(&camera->screenOffset)
        : "$2", "memory"
    );
    translation.lanes[3] = 0.0f;
    if ((sRender.texture_buffer_base << 5) != 0x3800)
        translation.lanes[3] = 3.1415927f;
    sceVif1PkOpenUpkCode(s_pPacket.sdk, 967, 108, 1, 1);
    sceVif1PkAddUpkData128(s_pPacket.sdk, translation.quad);
    sceVif1PkCloseUpkCode(s_pPacket.sdk);
    sceVif1PkOpenUpkCode(s_pPacket.sdk, 966, 108, 1, 1);
    /* The scale vector is sent as one native quadword; StudioCamera has no
     * quadword view of it yet. */
    sceVif1PkAddUpkData128(s_pPacket.sdk, *(const Quadword *)&camera->screenScale);
    sceVif1PkCloseUpkCode(s_pPacket.sdk);
}

void nmlPacketAddPixelTestPacket(int first_address, u64 first_value,
                               u64 first_test, int second_address,
                               u64 second_value, u64 second_test)
{
    NmlPixelTestPacket packet;
    NmlCurrentPacket selected;
    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    sceVif1PkCnt(selected.sdk, 0);
    packet.first_value = first_test;
    packet.second_value = second_test;
    packet.first_address = first_address;
    packet.second_address = second_address;
    packet.loops = 0x8002;
    packet.format = 0x10000000;
    packet.registers = 14;
    packet.register_high = 0;
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3fa, 0x6c, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk, &packet, 3);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);
    packet.first_address = first_address;
    packet.first_value = first_value;
    packet.second_address = second_address;
    packet.second_value = second_value;
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3fd, 0x6c, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk, &packet, 3);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);
}

void nmlPacketAddWaitMicrocode(void)
{
    NmlCurrentPacket selected;
    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    sceVif1PkCnt(selected.sdk, 0);
    selected = s_pPacket;
    sceVif1PkAddCode(selected.sdk, VIF_CODE_FLUSH);
}

void nmlPacketAddBlockMaterial(NmlModelBlock *material,
                               LayoutStore *model,
                               int use_base_alpha, int view_index)
{
    NmlCurrentPacket selected;
    NmlPacketData128 material_tag;
    NmlPacketData128 alpha_options;
    NmlPacketData128 *material_tag_data;
    const NmlProRealParam *proreal;
    float material_alpha;
    float vector_length;

    proreal = model->fields.proreal;
    if (model->fields.prorealOverride != 0)
        proreal = model->fields.prorealOverride;
    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    sceVif1PkCnt(selected.sdk, 0);

    material->materialParameters.lanes[3] =
        material->materialAlphaScale * model->fields.transparency;
    {
        union {
            float value;
            u32 bits;
        } vector_scale;

        vector_scale.value =
            model->fields.weightedMatrices[material->partId]
                .rows[1].w;
        if (vector_scale.bits & 1) {
            material->materialParameters.lanes[3] *=
                vector_scale.value;
        }
    }
    if (use_base_alpha) {
        material->materialParameters.lanes[3] =
            material->materialAlphaScale;
    }

    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3C8, 0x6C, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk,
                            &material->materialParameters, 5);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);

    material_alpha = model->fields.defaultBlockAlpha;
    material_tag_data = &material_tag;
    if (view_index != model->fields.window)
        material_alpha = model->fields.viewportAlpha[view_index];
    if ((model->fields.render_status & 0x100000) &&
        ((strstr(material->name, lenz_name) != 0) ||
         (strstr(material->name, eye_name) != 0)))
        material_alpha = model->fields.specialBlockAlpha;

    material_tag_data->words[0] = material->alphaTag;
    material_tag.lanes[1] = material_alpha;
    material_tag_data->words[2] = 0;
    if ((alphaLimitHigh < model->fields.alphaLimit) ||
        (model->fields.alphaLimit < alphaLimitLow))
        material_tag_data->words[2] = 1;

    xglVectorLength(&vector_length,
                    &model->fields.weightedMatrices[material->partId].rows[0]);
    if ((vectorLengthHigh < vector_length) || (vector_length < vectorLengthLow))
        material_tag_data->words[2] = 1;

    if (material->flags & 1) {
        const Vector4 *previous_block =
            &model->fields.weightedMatrices[material->matrixIndices[1] - 1]
                 .rows[0];

        xglVectorLength(&vector_length, previous_block);
        if ((previousVectorLengthHigh < vector_length) || (vector_length < previousVectorLengthLow))
            material_tag_data->words[2] = 1;
    }

    material_tag_data->words[3] = material->alphaMode;
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3F0, 0x6C, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk, &material_tag, 1);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);

    alpha_options.words[2] = 0;
    if (model->fields.alphaOptions & 1)
        alpha_options.lanes[2] = model->fields.alphaOptionsValue;
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3F9, 0x6C, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk, &alpha_options, 1);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);

    if (model->fields.render_status & 0x800) {
        selected = s_pPacket;
        sceVif1PkOpenUpkCode(selected.sdk, 0x3C9, 0x6C, 1, 1);
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, proreal->unmodeled_140, 1);
        selected = s_pPacket;
        sceVif1PkCloseUpkCode(selected.sdk);
    }

    nmlPacketSelectLighting(model, &material->materialFlags);
    selected = s_pPacket;
    sceVif1PkCnt(selected.sdk, 0);
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3D1, 0x6C, 1, 1);
    if (model->fields.render_status & 0x800) {
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, proreal->block_material_data, 4);
    } else if (model->fields.render_status & 0x80000) {
        model->fields.lightDirection[3][3] = (float)(0x80 - (int)(
            model->fields.texMapSize * model->fields.transparency *
            model->fields.fogIntensity[view_index]));
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, model, 4);
    } else {
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, model, 4);
    }
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);

    if (material->materialFlags & 0x20000) {
        selected = s_pPacket;
        sceVif1PkOpenUpkCode(selected.sdk, 0x3D1, 0x6C, 1, 1);
        selected = s_pPacket;
        sceVif1PkAddUpkData128N(selected.sdk, s_inLc0, 4);
        selected = s_pPacket;
        sceVif1PkCloseUpkCode(selected.sdk);
    }
}

void nmlPacketMakeCircleTexture(LayoutStore *model,
                                StudioCamera *camera)
{
    NmlCircleScratchpad *scratch = (NmlCircleScratchpad *)0x70000000;
    NmlCircleVector *transform = (NmlCircleVector *)0x70000040;
    float radius_scale;
    float height_offset;
    float *matrix;
    NmlCircleOffsetTable x_offsets;
    NmlCircleOffsetTable z_offsets;
    u32 texture_base;
    int i;
    int first_clip_flags;
    int second_clip_flags;
    int third_clip_flags;

    x_offsets = circleXOffsetTable;
    z_offsets = circleZOffsetTable;

    matrix = transform->lanes;

    matrix[14] = 1.0f;
    matrix[0] = 0.0f;
    matrix[1] = 0.0f;
    matrix[2] = 1.0f;
    matrix[4] = 1.0f;
    matrix[5] = 0.0f;
    matrix[6] = 1.0f;
    matrix[8] = 0.0f;
    matrix[9] = 1.0f;
    matrix[10] = 1.0f;
    matrix[12] = 1.0f;
    radius_scale = circleRadiusScale;
    matrix[13] = 1.0f;

    _CurSetViewScaleTrans(&camera->screenScale, &camera->screenOffset);
    _CurSetMatrix(camera->viewMatrix);
    height_offset = circleHeightOffset;

    for (i = 0; i < 4;) {
        int clip_flags;
        NmlCircleVector *position = &scratch->position[i];
        NmlCircleVector *projected = &scratch->projected[i];
        NmlCircleVector *perspective = &scratch->perspective[i];
        NmlCircleVector *vertex_transform = &transform[i];

        position->vector.x =
            model->fields.center.v.x +
            x_offsets.values[i] * model->fields.circleScale * radius_scale;
        position->vector.z =
            model->fields.center.v.z +
            z_offsets.values[i] * model->fields.circleScale * radius_scale;
        position->vector.y =
            model->fields.place[3][1] + height_offset;
        i++;
        if (model->fields.shadowHeightEnabled != 0)
            position->vector.y =
                model->fields.shadowHeight + height_offset;

        clip_flags = _CurRotTransPersClip(&projected->vector,
            &perspective->vector, &position->vector, &vertex_transform->vector);
        if (clip_flags != 0)
            projected->words[3] = 12;
        else
            projected->words[3] = 4;
    }

    nmlCircleSetGsRecord(nmlCircleGsRecord(0), 0x3f, 0);
    texture_base = sRender.texture_buffer_base << 5;
    nmlCircleSetGsRecord(nmlCircleGsRecord(1), 6,
                         nmlCircleMakeTextureParameter(texture_base));
    nmlCircleSetGsRecord(nmlCircleGsRecord(2), 0x14, 0x60);
    nmlCircleSetGsRecord(nmlCircleGsRecord(3), 0x47, 0x71001);
    nmlCircleSetGsRecord(nmlCircleGsRecord(4), 8, 0);
    nmlCircleSetGsRecord(nmlCircleGsRecord(5), 0x42, 0x89);
    nmlCircleSetGsRecord(nmlCircleGsRecord(6), 0, 0x54);
    nmlCircleSetGsRecord(nmlCircleGsRecord(7), 1,
        ((u64)scratch->perspective[0].unsigned_words[2] << 32) | 0x80808080);
    nmlCircleSetGsRecord(nmlCircleGsRecord(8), 2,
        scratch->perspective[0].unsigned_words[0] |
        ((u64)scratch->perspective[0].unsigned_words[1] << 32));
    first_clip_flags = nmlCircleSetPositionRecord(nmlCircleGsRecord(9),
        scratch->projected[0].words[0] |
        ((u64)scratch->projected[0].words[1] << 16) |
        ((u64)scratch->projected[0].words[2] << 32),
        &scratch->projected[0].words[3], 0, 0);

    nmlCircleSetGsRecord(nmlCircleGsRecord(10), 1,
        ((u64)scratch->perspective[1].unsigned_words[2] << 32) | 0x80808080);
    nmlCircleSetGsRecord(nmlCircleGsRecord(11), 2,
        scratch->perspective[1].unsigned_words[0] |
        ((u64)scratch->perspective[1].unsigned_words[1] << 32));
    second_clip_flags = nmlCircleSetPositionRecord(nmlCircleGsRecord(12),
        scratch->projected[1].words[0] |
        ((u64)scratch->projected[1].words[1] << 16) |
        ((u64)scratch->projected[1].words[2] << 32),
        &scratch->projected[1].words[3], first_clip_flags, 0);

    nmlCircleSetGsRecord(nmlCircleGsRecord(13), 1,
        ((u64)scratch->perspective[2].unsigned_words[2] << 32) | 0x80808080);
    nmlCircleSetGsRecord(nmlCircleGsRecord(14), 2,
        scratch->perspective[2].unsigned_words[0] |
        ((u64)scratch->perspective[2].unsigned_words[1] << 32));
    third_clip_flags = nmlCircleSetPositionRecord(nmlCircleGsRecord(15),
        scratch->projected[2].words[0] |
        ((u64)scratch->projected[2].words[1] << 16) |
        ((u64)scratch->projected[2].words[2] << 32),
        &scratch->projected[2].words[3], second_clip_flags, first_clip_flags);

    nmlCircleSetGsRecord(nmlCircleGsRecord(16), 1,
        ((u64)scratch->perspective[3].unsigned_words[2] << 32) | 0x80808080);
    nmlCircleSetGsRecord(nmlCircleGsRecord(17), 2,
        scratch->perspective[3].unsigned_words[0] |
        ((u64)scratch->perspective[3].unsigned_words[1] << 32));
    nmlCircleSetPositionRecord(nmlCircleGsRecord(18),
        scratch->projected[3].words[0] |
        ((u64)scratch->projected[3].words[1] << 16) |
        ((u64)scratch->projected[3].words[2] << 32),
        &scratch->projected[3].words[3], third_clip_flags, second_clip_flags);

    g_nGsEntry = 0;
    {
        int remaining;
        NmlCircleGsRecord *cursor = (NmlCircleGsRecord *)0x70001000;

        for (remaining = 18; remaining >= 0; remaining--) {
            nmlPacketAppendGsValue(cursor->address.words[0],
                                   &cursor->value.value);
            cursor++;
        }
    }
    nmlPacketAddGsFlushWide();
}

void nmlPacketSendCircleTexture(const void *indices, const void *palette)
{
    XglLoadImage image;
    NmlCurrentPacket selected;
    XglPacket *packet = xglPacketGetCurrent();
    u8 *palette_copy;

    selected.sdk = packet;
    s_pPacket = selected;
    sceVif1PkCnt(packet, 0);
    selected = s_pPacket;
    sceVif1PkAddCode(selected.sdk, VIF_CODE_FLUSH);
    sceGsSetDefLoadImage(&image, sRender.texture_buffer_base << 5,
                         1, 19, 0, 0, 64, 64);
    image.giftag |= 0x8000;
    selected = s_pPacket;
    sceVif1PkOpenDirectCode(selected.sdk, 0);
    selected = s_pPacket;
    sceVif1PkAddDirectDataN(selected.sdk, &image, 5);
    selected = s_pPacket;
    sceVif1PkCloseDirectCode(selected.sdk);
    selected = s_pPacket;
    sceVif1PkRef(selected.sdk, indices, 258, 0, 0, 0);
    selected = s_pPacket;
    sceVif1PkCnt(selected.sdk, 0);
    sceGsSetDefLoadImage(&image, (sRender.texture_buffer_base << 5) + 128,
                         1, 0, 0, 0, 16, 16);
    image.giftag |= 0x8000;
    selected = s_pPacket;
    sceVif1PkOpenDirectCode(selected.sdk, 0);
    selected = s_pPacket;
    sceVif1PkAddDirectDataN(selected.sdk, &image, 5);
    selected = s_pPacket;
    sceVif1PkCloseDirectCode(selected.sdk);
    palette_copy = nmlPacketSetAttributeData16N(palette, 66);
    selected = s_pPacket;
    sceVif1PkRef(selected.sdk, palette_copy, 66, 0, 0, 0);
}

static void nmlPacketTextureTrans(LayoutStore *model)
{
    XglLoadImage image;
    NmlCurrentPacket selected;
    NmlModel *resource;
    NmlTextureResourceSet *set;
    u32 texture_set_address;
    const u32 *offset;
    int index;

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    resource = model->fields.model;
    if (resource->textureSetOffset == 0)
        return;
    /* Serialized references use byte offsets in the 32-bit EE address space. */
    texture_set_address = resource->textureSetOffset + (u32)resource;
    set = (NmlTextureResourceSet *)texture_set_address;
    offset = set->entry_offsets;
    for (index = 0; index < set->count; index++) {
        NmlTextureResourceEntry *entry;
        u32 entry_address;
        int address;

        entry_address = offset[index] + (u32)set;
        entry = (NmlTextureResourceEntry *)entry_address;
        selected = s_pPacket;
        sceVif1PkCnt(selected.sdk, 0);
        selected = s_pPacket;
        sceVif1PkAddCode(selected.sdk, VIF_CODE_FLUSH);
        address = sRender.texture_buffer_base << 5;
        address += entry->vram_offset;
        sceGsSetDefLoadImage(&image, (short)address,
                            entry->buffer_width, entry->psm, 0, 0,
                            entry->width, entry->height);
        image.giftag |= 0x8000;
        selected = s_pPacket;
        sceVif1PkOpenDirectCode(selected.sdk, 0);
        selected = s_pPacket;
        sceVif1PkAddDirectDataN(selected.sdk, &image, 5);
        selected = s_pPacket;
        sceVif1PkCloseDirectCode(selected.sdk);
        /* Pixel bytes follow the fixed image descriptor. */
        selected = s_pPacket;
        sceVif1PkRef(selected.sdk, entry + 1, entry->pixel_bytes / 16,
                     0, 0, 0);
    }
}

void nmlPacketClrTextureCache(void)
{
    s_pCacheTexture = 0;
}

void nmlPacketClrModelCache(void)
{
    s_pMatrixCache = 0;
    s_pModelCache = 0;
    s_pModelLayout = 0;
    s_pLightLayout = 0;
}

int nmlPacketAddTexture(LayoutStore *model)
{
    NmlProRealParam *proreal = model->fields.proreal;
    NmlCurrentPacket selected;
    XglPacket *packet;
    int size;
    u32 texture_reference;
    const struct XglTextureTransSet *texture;

    if (model->fields.prorealOverride)
        proreal = model->fields.prorealOverride;
    packet = xglPacketGetCurrent();
    selected.sdk = packet;
    s_pPacket = selected;
    size = sceVif1PkSize(packet);
    selected = s_pPacket;
    packet = selected.sdk;
    if (size * 0x10u +
            (packet->limit - (u32)packet->cursor) + 0x10000u >
        0x200000u)
        return 1;

    if (model->fields.render_status & 0x800u) {
        const void *cached_texture;
        texture = proreal->texture;
        cached_texture = s_pCacheTexture;
        if (texture && cached_texture != texture) {
            s_pCacheTexture = texture;
            xglPacketTextureTrans(texture);
            selected = s_pPacket;
            packet = selected.sdk;
        }
    } else {
        texture_reference = model->fields.model->textureSetOffset;
        if (texture_reference - 1 <= 0x7ffffeu) {
            if (s_pCacheTexture != (void *)texture_reference) {
                nmlPacketTextureTrans(model);
                s_pCacheTexture = (void *)model->fields.model->textureSetOffset;
                selected = s_pPacket;
                packet = selected.sdk;
            }
        } else {
            texture = model->fields.texture;
            if (texture && s_pCacheTexture != texture) {
                const void *const *texture_slot = &model->fields.texture;
                s_pCacheTexture = texture;
                xglPacketTextureTrans(*texture_slot);
                selected = s_pPacket;
                packet = selected.sdk;
            }
        }
    }
    size = sceVif1PkSize(packet);
    {
        XglPacket *finished_packet = s_pPacket.sdk;

        return size * 0x10u +
                (finished_packet->limit - (u32)finished_packet->cursor) +
                0x10000u > 0x200000u;
    }
}

int nmlPacketAddParts(const NmlModelBlock *block,
                      LayoutStore *model,
                      StudioCamera *camera)
{
    NmlCurrentPacket selected;
    NmlCurrentPacket current;
    XglPacket *packet;
    NmlPartsWorkingScratch matrices;
    NmlModel *resource;
    NmlProRealParam *proreal;
    const u16 *vertex_indices;
    const u16 *bone_indices = block->matrixIndices;
    const int *geometry_offsets;
    const Vector4 *block_vectors;
    const Vector4 *geometry_rows;
    int i;
    int matrix_changed;
    int code;
    int size;

    proreal = model->fields.proreal;
    if (model->fields.prorealOverride != 0)
        proreal = model->fields.prorealOverride;
    resource = model->fields.model;
    packet = xglPacketGetCurrent();
    current.sdk = packet;
    s_pPacket = current;
    sceVif1PkCnt(packet, 0);
    size = sceVif1PkSize(s_pPacket.sdk);
    if (size * 0x10u + (s_pPacket.sdk->limit - (u32)s_pPacket.sdk->cursor) +
        0x10000u > 0x200000u)
        return 1;
    {
        if (block->flags & 0x10) {
            geometry_offsets = (const int *)((const u8 *)resource +
                              resource->geometryTableOffset);
            block_vectors = (const Vector4 *)((const u8 *)resource +
                            geometry_offsets[block->geometryIndex] + 0x30);
            vertex_indices = (const u16 *)((const u8 *)block +
                              block->vertexIndexOffset);
            for (i = 0; i < block->vertexIndexCount; i++) {
                u16 code_index = block->vertexIndexBase + i * 2;
                selected = s_pPacket;
                sceVif1PkOpenUpkCode(selected.sdk, code_index, 0x6c, 1, 1);
                selected = s_pPacket;
                sceVif1PkAddUpkData128N(
                    selected.sdk,
                    &block_vectors[vertex_indices[i] * 2], 2);
                selected = s_pPacket;
                sceVif1PkCloseUpkCode(selected.sdk);
            }
        }

        if (s_pModelLayout != model) {
            s_pModelLayout = model;
            s_pMatrixCache = 0;
        }
        if (s_nModelParts != block->partId) {
            s_nModelParts = block->partId;
            s_pMatrixCache = 0;
        }
        if (s_pMatrixCache != 0 &&
            memcmp(s_pMatrixCache, bone_indices, 0x3e) != 0)
            s_pMatrixCache = 0;
        matrix_changed = s_pMatrixCache == 0;
        s_pMatrixCache = bone_indices;

        {
            geometry_rows = model->fields.weightedMatrices->rows;
            if (*bone_indices != 0) {
                code = 0x2dc;
                do {
                    u16 bone_index = *bone_indices++;

                    geometry_rows += (bone_index - 1) *
                        (sizeof(NmlWeightedMatrix) / sizeof(Vector4));

                    selected = s_pPacket;
                    sceVif1PkOpenUpkCode(selected.sdk, code & 0xffff,
                                         0x6c, 1, 1);
                    selected = s_pPacket;
                    sceVif1PkAddUpkData128N(selected.sdk,
                                            geometry_rows, 4);
                    selected = s_pPacket;
                    sceVif1PkCloseUpkCode(selected.sdk);
                    code += 4;
                    geometry_rows = model->fields.weightedMatrices->rows;
                } while (*bone_indices != 0);
            }
        }

        if (matrix_changed) {
            const u16 *cached_bones = s_pMatrixCache;
            const Vector4 *base_rows =
                geometry_rows + cached_bones[0] * 4 - 4;
            float homogeneous_w;

            nmlPartsCopyGeometryMatrix(&matrices.part, base_rows);
            homogeneous_w = 0.0f;
            nmlPartsSetRowW(&matrices.part, homogeneous_w);

            if (!(block->flags & 1) &&
                !(block->materialFlags & 0x10)) {
                xglMatrixInverse(matrices.inverse_part.values,
                                 matrices.part.values);
            } else {
                /* Byte arithmetic with the offset first: the original adds
                 * partCount * 0x40 to the base in that operand order, which
                 * no pointer expression produces. */
                const Vector4 *layout_rows =
                    (const Vector4 *)(resource->partCount * sizeof(NmlWeightedMatrix) + (u32)base_rows);

                nmlPartsCopyLayoutMatrix(&matrices.inverse_part, layout_rows);
            }

            _CurMatrixSet(matrices.inverse_part.values);
            _CurMatrixMul(model->fields.inversePlace);
            if (model->fields.render_status & 0x800) {
                _CurMatrixMul33norm(matrices.normal.rows,
                    proreal->normal_transform);
            } else {
                _CurMatrixMul33norm(matrices.normal.rows,
                                    model->fields.lightColor);
            }
            _CurApplyMatrix(&matrices.reference_point.vector,
                            &camera->position.vector);

            if (model->fields.render_status & 0x02001000) {
                NmlPartsLightPayload *light_payload = &matrices.light_payload;
                const Vector4 *source_points = model->fields.pointPosition;
                _CurApplyMatrix(&matrices.light_payload.current_point,
                                source_points);
                matrices.light_payload.rows[0].x = matrices.light_payload.current_point.x;
                matrices.light_payload.rows[1].x = matrices.light_payload.current_point.y;
                matrices.light_payload.rows[2].x = matrices.light_payload.current_point.z;
                _CurApplyMatrix(&matrices.light_payload.current_point,
                                &source_points[1]);
                matrices.light_payload.rows[0].y = matrices.light_payload.current_point.x;
                matrices.light_payload.rows[1].y = matrices.light_payload.current_point.y;
                matrices.light_payload.rows[2].y = matrices.light_payload.current_point.z;
                _CurApplyMatrix(&matrices.light_payload.current_point,
                                &source_points[2]);
                matrices.light_payload.rows[0].z = matrices.light_payload.current_point.x;
                matrices.light_payload.rows[1].z = matrices.light_payload.current_point.y;
                matrices.light_payload.rows[2].z = matrices.light_payload.current_point.z;
                selected = s_pPacket;
                sceVif1PkOpenUpkCode(selected.sdk, 0x3f5, 0x6c, 1, 1);
                selected = s_pPacket;
                sceVif1PkAddUpkData128N(selected.sdk,
                                        light_payload, 4);
                selected = s_pPacket;
                sceVif1PkCloseUpkCode(selected.sdk);
                selected = s_pPacket;
                sceVif1PkOpenUpkCode(selected.sdk, 0x3e9, 0x6c, 1, 1);
                selected = s_pPacket;
                sceVif1PkAddUpkData128N(selected.sdk,
                                        model->fields.pointColor, 3);
                selected = s_pPacket;
                sceVif1PkCloseUpkCode(selected.sdk);
            }

            if (model->fields.render_status & 0x800) {
                xglMatrixMul(&matrices.projected_placement.xgl_matrix,
                    &proreal->projection_transform,
                    &model->fields.place);
                selected = s_pPacket;
                sceVif1PkOpenUpkCode(selected.sdk, 0x3ec, 0x6c, 1, 1);
                selected = s_pPacket;
                sceVif1PkAddUpkData128N(selected.sdk,
                                        matrices.projected_placement.values, 4);
                selected = s_pPacket;
                sceVif1PkCloseUpkCode(selected.sdk);
                matrices.reference_point = proreal->reference_point;
            }
            matrices.normal.rows[3].w = 0.0f;

            nmlPartsSetNormalTranslation(&matrices.normal,
                                         &matrices.reference_point, 3);
            if (model->fields.render_status & 0x800) {
                const NmlScreenQuadword *source_row = &proreal->final_normal_row;
                NmlScreenQuadword *destination_row = &matrices.normal.quadwords[3];

                __asm__ __volatile__(
                    "lq $2,0(%1)\n\t"
                    "sq $2,0(%0)"
                    :
                    : "r"(destination_row), "r"(source_row)
                    : "$2", "memory");
            }

            selected = s_pPacket;
            sceVif1PkOpenUpkCode(selected.sdk, 0x3cd, 0x6c, 1, 1);
            selected = s_pPacket;
            sceVif1PkAddUpkData128N(selected.sdk,
                                    matrices.normal.values, 4);
            selected = s_pPacket;
            sceVif1PkCloseUpkCode(selected.sdk);

        }
        if (matrix_changed) {
            if (model->fields.render_status & 0x8000) {
                _CurMatrixSet(model->fields.projectionMatrix);
            } else {
                _CurMatrixSet(camera->viewMatrix);
                if (model->fields.render_status & 0x80000)
                    _CurMatrixMul(model->fields.projectionMatrix);
            }
            if (model->fields.render_status & 1) {
                _CurMatrixMul(model->fields.place);
                _CurMatrixMul(matrices.part.values);
            } else {
                _CurMatrixMul(matrices.part.values);
            }
            _CurMatrixGet(matrices.clip.values);
            selected = s_pPacket;
            sceVif1PkOpenUpkCode(selected.sdk, 0x3c2, 0x6c, 1, 1);
            selected = s_pPacket;
            sceVif1PkAddUpkData128N(selected.sdk,
                                    matrices.clip.values, 4);
            selected = s_pPacket;
            sceVif1PkCloseUpkCode(selected.sdk);
        }
    }
    size = sceVif1PkSize(s_pPacket.sdk);
    return size * 0x10u +
           (s_pPacket.sdk->limit - (u32)s_pPacket.sdk->cursor) + 0x10000u >
           0x200000u;
}

void nmlPacketAddTextureParam(NmlModelBlock *material,
                              LayoutStore *model)
{
    NmlProRealParam *proreal;

    proreal = model->fields.proreal;
    if (model->fields.prorealOverride != 0)
        proreal = model->fields.prorealOverride;
    s_pPacket.sdk = xglPacketGetCurrent();
    if (model->fields.render_status & 0x800) {
        u64 texture_filter = 0x60;

        nmlPacketAppendGsWords(0x3f, 0, 0);
        nmlPacketAppendGsValue(6, &proreal->texture_gs_parameters[0].value);
        nmlPacketAppendGsValue(0x14, &texture_filter);
        nmlPacketAppendGsValue(8, &proreal->texture_gs_parameters[1].value);
    } else if (material->materialFlags & 1) {
        u32 tex0_low = material->textureParameters[0].buffer +
                       (sRender.texture_buffer_base << 5);
        u32 tex0_high = material->textureParameters[0].width +
                        (sRender.texture_buffer_base << 10);

        nmlPacketAppendGsWords(0x3f, 0, 0);
        nmlPacketAppendGsWords(6, tex0_high, tex0_low);
        nmlPacketAppendGsWords(0x14, *&material->textureParameters[1].width,
                               *&material->textureParameters[1].buffer);
        nmlPacketAppendGsFields(8, material->textureParameters[2].width,
                                &material->textureParameters[2].buffer);
    }
}

static int add_exec_prog(NmlModelBlock *material, LayoutStore *model,
                         int program_index)
{
    int eye_material;
    NmlProRealParam *proreal;
    int result = 1;
    u32 *type1_programs;
    u32 *type2_programs;
    u32 *type3_programs;
    u32 *type4_programs;
    u32 *type5_programs;
    u32 *type6_programs;
    u32 *type7_programs;
    u32 program;

    type1_programs = s_aUcodeTbl[program_index].programs[0];
    type2_programs = s_aUcodeTbl[program_index].programs[1];
    type3_programs = s_aUcodeTbl[program_index].programs[2];
    type4_programs = s_aUcodeTbl[program_index].programs[3];
    type5_programs = s_aUcodeTbl[program_index].programs[4];
    type6_programs = s_aUcodeTbl[program_index].programs[5];
    type7_programs = s_aUcodeTbl[program_index].programs[6];

    eye_material = 0;
    proreal = model->fields.proreal;
    if (model->fields.prorealOverride != 0)
        proreal = model->fields.prorealOverride;

    if ((model->fields.render_status & 0x100000) != 0 &&
        (strstr(material->name, lenz_name) != 0 ||
         strstr(material->name, eye_name) != 0))
        eye_material = 1;

    s_pPacket.sdk = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket.sdk, 0);

    switch (s_nProgType) {
    case 7: {
        int slot = 0;

        if (material->materialFlags & 0x10)
            slot = 1;
        else if (material->flags & 1) {
            slot = 2;
            if (material->flags & 2)
                slot = 3;
            else if (material->flags & 4)
                slot = 4;
        }
        program = type7_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    case 6: {
        int slot;

        if (material->flags & 1) {
            slot = 2;
            if (material->flags & 2)
                slot = 3;
            else if (material->flags & 4)
                slot = 4;
        }
        else if (material->materialFlags & 0x10)
            slot = 1;
        else
            slot = 0;
        program = type6_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    case 5: {
        int slot = 0;

        if ((material->materialFlags & 0x10) == 0) {
            slot = 1;
            if ((material->materialFlags & 0x1000) &&
                (!(model->fields.renderLevel & 0x10) || eye_material))
                slot = 4;
            else if ((material->materialFlags & 8) &&
                     (!(model->fields.renderLevel & 2) || eye_material)) {
                if ((material->materialFlags & 0x800) ||
                    ((model->fields.renderLevel & 1) && !eye_material))
                    slot = 2;
                else
                    slot = 3;
            }
        }
        program = type5_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    case 4: {
        int slot;

        if (material->flags & 2)
            slot = 4;
        else if (material->flags & 4)
            slot = 8;
        else
            slot = 0;
        if ((material->materialFlags & 0x1000) &&
            (!(model->fields.renderLevel & 0x10) || eye_material))
            slot += 3;
        else if ((material->materialFlags & 8) &&
                 (!(model->fields.renderLevel & 2) || eye_material)) {
            if ((material->materialFlags & 0x800) ||
                ((model->fields.renderLevel & 1) && !eye_material))
                slot += 1;
            else
                slot += 2;
        }
        program = type4_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    case 3: {
        int slot;

        if (material->materialFlags & 0x10) {
            slot = 3;
            if (proreal->projection_entry)
                slot = 4;
        }
        else if (material->flags & 1) {
            if (material->flags & 2)
                slot = 1;
            else if (material->flags & 4)
                slot = 2;
            else
                slot = 0;
        }
        else
            slot = 5;
        program = type3_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    case 1: {
        int slot;

        if (material->flags & 2)
            slot = 4;
        else if (material->flags & 4)
            slot = 8;
        else
            slot = 0;
        if ((material->materialFlags & 0x1000) &&
            (!(model->fields.renderLevel & 0x10) || eye_material))
            slot += 3;
        else if ((material->materialFlags & 8) &&
                 (!(model->fields.renderLevel & 2) || eye_material)) {
            if ((material->materialFlags & 0x800) ||
                ((model->fields.renderLevel & 1) && !eye_material))
                slot += 1;
            else
                slot += 2;
        }
        program = type1_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    case 2: {
        int slot;

        if ((material->materialFlags & 0x10) == 0) {
            slot = 2;
            if ((material->materialFlags & 0x1000) &&
                (!(model->fields.renderLevel & 0x10) || eye_material))
                slot = 5;
            else if ((material->materialFlags & 8) &&
                     (!(model->fields.renderLevel & 2) || eye_material)) {
                if ((material->materialFlags & 0x800) ||
                    ((model->fields.renderLevel & 1) && !eye_material))
                    slot = 3;
                else
                    slot = 4;
            }
        }
        else if (material->flags & 0x10)
            slot = 1;
        else
            slot = 0;
        program = type2_programs[slot];
        if (program != 0)
            sceVif1PkAddCode(s_pPacket.sdk, (program >> 3) | VIF_CODE_MSCAL);
        else
            result = 0;
        break;
    }

    default:
        break;
    }

    return result;
}

void nmlPacketAddExecProg(NmlModelBlock *material,
                          LayoutStore *model,
                          int allow_extended_programs,
                          int add_transform_data)
{
    if (add_transform_data) {
        if (!add_exec_prog(material, model, 0))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if ((material->flags & 0x80)
        || (model->fields.render_status & 0x100)) {
        if (!add_exec_prog(material, model, 1))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if ((material->flags & 0x100)
        || (model->fields.render_status & 0x200)) {
        if (!add_exec_prog(material, model, 2))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if (material->flags & 0x200) {
        if (add_exec_prog(material, model, 2))
            nmlPacketAddTransData(material);
        nmlPacketAddWaitMicrocode();
        if (!add_exec_prog(material, model, 1))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if (allow_extended_programs
        && !(material->flags & 0x400)
        && !(model->fields.render_status & 0x8)) {
        if (model->fields.renderLevel & 0x4) {
            if (!add_exec_prog(material, model, 3))
                return;
            nmlPacketAddTransData(material);
            return;
        }

        if (add_exec_prog(material, model, 5))
            nmlPacketAddTransData(material);
        nmlPacketAddWaitMicrocode();
        if (!add_exec_prog(material, model, 4))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if (!add_exec_prog(material, model, 3))
        return;
    nmlPacketAddTransData(material);
}

void nmlPacketAddTransData(NmlModelBlock *material)
{
    NmlCurrentPacket selected;

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    /* The span starts dataOffset bytes into the block (32-bit EE addresses). */
    sceVif1PkRef(selected.sdk, (void *)(material->dataOffset + (u32)material),
                 material->dataSize / 16, 0, 0, 0);
    nmlPacketAddWaitMicrocode();
}

u8 *nmlPacketSetAttributeData(const void *data, u32 size)
{
    NmlCurrentPacket selected;
    NmlAttributePacket *packet;
    NmlAttributePacket *current;

    packet = (void *)xglPacketGetCurrent();
    selected.sdk = (void *)packet;
    s_pPacket = selected;
    packet->cursor.bytes -= size;
    selected = s_pPacket;
    current = selected.attributes;
    memcpy(current->cursor.bytes, data, size);
    {
        NmlCurrentPacket finished = s_pPacket;
        return finished.attributes->cursor.bytes;
    }
}

u8 *nmlPacketSetAttributeData64(const void *data)
{
    NmlAttributePacket *packet;
    NmlCurrentPacket selected = { xglPacketGetCurrent() };

    s_pPacket = selected;
    packet = selected.attributes;
    packet->cursor.bytes -= 64;
    {
        NmlCurrentPacket allocated = s_pPacket;
        Quadword *destination = allocated.attributes->cursor.quadwords;
        const Quadword *source = data;
        /* The original SDK-style transfers at 0x00238e8c-0x00238ea8
         * reuse GPR2 for all four aligned quadwords. Address calculation
         * and allocation remain C; the block owns only these transfers. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n\t"
            "sq $2, 0(%0)\n\t"
            "lq $2, 16(%1)\n\t"
            "sq $2, 16(%0)\n\t"
            "lq $2, 32(%1)\n\t"
            "sq $2, 32(%0)\n\t"
            "lq $2, 48(%1)\n\t"
            "sq $2, 48(%0)"
            :
            : "r"(destination), "r"(source)
            : "$2", "memory");
    }
    {
        NmlCurrentPacket finished = s_pPacket;
        return finished.attributes->cursor.bytes;
    }
}

void *nmlPacketSetAttributeData64N(const void *data, int count)
{
    int remaining;
    XglPacket *packet;
    Quadword *destination;
    const Quadword *source;

    packet = xglPacketGetCurrent();
    s_pPacket.sdk = packet;
    remaining = count;
    packet->cursor -= count * 64;
    destination = (Quadword *)s_pPacket.sdk->cursor;
    source = data;
    /* One 64-byte attribute record per pass: four quadword copies, then the
     * count, pointer and branch of the original copy loop (delay slot nop). */
    __asm__ __volatile__(
        "1:\n\t"
        "lq $2, 0(%2)\n\t"
        "sq $2, 0(%1)\n\t"
        "lq $2, 16(%2)\n\t"
        "sq $2, 16(%1)\n\t"
        "lq $2, 32(%2)\n\t"
        "sq $2, 32(%1)\n\t"
        "lq $2, 48(%2)\n\t"
        "sq $2, 48(%1)\n\t"
        "addi %0, %0, -1\n\t"
        "addi %1, %1, 64\n\t"
        "addi %2, %2, 64\n\t"
        "bne $0, %0, 1b"
        : "+r"(remaining), "+r"(destination), "+r"(source)
        :
        : "$2", "memory");
    return s_pPacket.sdk->cursor;
}

void *nmlPacketSetAttributeData16N(const void *data, int count)
{
    NmlAttributePacket *packet;
    Quadword *destination;
    const Quadword *source;
    int remaining;

    packet = (void *)xglPacketGetCurrent();
    s_pPacket.attributes = packet;
    remaining = count;
    packet->cursor.bytes -= count * 16;
    destination = s_pPacket.attributes->cursor.quadwords;
    source = data;
    /* One 16-byte quadword per pass: copy, then the count, pointer and branch
     * of the original copy loop (the delay-slot nop is emitted by GCC). */
    __asm__ __volatile__(
        "1:\n\t"
        "lq $2, 0(%2)\n\t"
        "sq $2, 0(%1)\n\t"
        "addi %0, %0, -1\n\t"
        "addi %1, %1, 16\n\t"
        "addi %2, %2, 16\n\t"
        "bne $0, %0, 1b"
        : "+r"(remaining), "+r"(destination), "+r"(source)
        :
        : "$2", "memory");
    return s_pPacket.attributes->cursor.bytes;
}

u8 *nmlPacketSetAttributeAlloc16N(u32 count)
{
    NmlCurrentPacket selected;
    NmlAttributePacket *packet;
    NmlAttributePacket *current;

    packet = (void *)xglPacketGetCurrent();
    selected.sdk = (void *)packet;
    s_pPacket = selected;
    packet->cursor.bytes -= count * 0x10;
    current = s_pPacket.attributes;
    return current->cursor.bytes;
}

void nmlPacketAddGifTag(NmlModelBlock *material,
                        LayoutStore *model, int count)
{
    NmlPacketData128 giftag;
    NmlCurrentPacket *current_packet = &s_pPacket;
    u32 flags = model->fields.render_status;
    u32 end_of_packet = (flags & 0x01000002) != 0;

    if (flags & 0x2000)
        end_of_packet = 0;

    current_packet->sdk = xglPacketGetCurrent();
    sceVif1PkCnt(current_packet->sdk, 0);
    giftag.quad = material->gifTag;
    giftag.words[1] |= (count << 21) | (end_of_packet << 20);
    if (material->materialFlags & 1) {
        giftag.words[1] |= 0x00080000;
    } else {
        giftag.words[1] &= ~0x00080000;
    }
    if (model->fields.render_status & 0x8000) {
        giftag.words[1] &= ~((count << 21) | 0x00080000);
    }
    if (model->fields.render_status & 0x800) {
        giftag.words[1] |= 0x00080000;
    }
    sceVif1PkOpenUpkCode(current_packet->sdk, 0x3F3, 0x6C, 1, 1);
    sceVif1PkAddUpkData128(current_packet->sdk, giftag.quad);
    sceVif1PkCloseUpkCode(current_packet->sdk);
    giftag.words[1] = (giftag.words[1] & ~0x00080000) | 0x01200000;
    sceVif1PkOpenUpkCode(current_packet->sdk, 0x3F4, 0x6C, 1, 1);
    sceVif1PkAddUpkData128(current_packet->sdk, giftag.quad);
    sceVif1PkCloseUpkCode(current_packet->sdk);
    giftag.words[1] = (giftag.words[1] | 0x00080000) & 0xFEFFFFFF;
    sceVif1PkOpenUpkCode(current_packet->sdk, 0x3F2, 0x6C, 1, 1);
    sceVif1PkAddUpkData128(current_packet->sdk, giftag.quad);
    sceVif1PkCloseUpkCode(current_packet->sdk);
}

void nmlPacketAddGifTagStandard(int eop, int count)
{
    NmlPacketData128 gif_tag;
    NmlCurrentPacket selected;

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    sceVif1PkCnt(selected.sdk, 0);

    gif_tag.words[0] = 0x8000;
    gif_tag.words[1] = ((u32)eop << 0x13) | ((u32)count << 0x15) | 0x30064000;
    gif_tag.words[2] = 0x412;
    gif_tag.words[3] = 0;
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 0x3f3, 0x6c, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128(selected.sdk, gif_tag.quad);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);
}

void nmlPacketAddFog(LayoutStore *model, int viewport_index)
{
    s_pPacket.sdk = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket.sdk, 0);

    if (g_aSubWindow[viewport_index] != 0)
        model->fields.fogColor.intensity = ((int)(model->fields.fogIntensity[viewport_index] * 255.0f)) << 4;

    sceVif1PkOpenUpkCode(s_pPacket.sdk, 998, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket.sdk, &model->fields.fogColor, 1);
    sceVif1PkCloseUpkCode(s_pPacket.sdk);
    sceVif1PkOpenUpkCode(s_pPacket.sdk, 1007, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket.sdk, &model->fields.fogPara, 1);
    sceVif1PkCloseUpkCode(s_pPacket.sdk);
}

void nmlPacketAddPixelControl(NmlModelBlock *material,
                              LayoutStore *model,
                              u32 pixel_control_flags)
{
    NmlCurrentPacket selected;
    u16 viewport = pixel_control_flags;
    int pixel_reference = model->fields.pixelAlpha;
    float reflect_alpha = model->fields.reflTransparency;
    float alpha;
    int special_material = 0;
    u32 material_flags;
    u64 packed_alpha;
    int i;

    alpha = model->fields.defaultBlockAlpha;
    if (viewport != model->fields.window)
        alpha = model->fields.viewportAlpha[viewport];
    if (model->fields.render_status & 0x100000) {
        if (strstr(material->name, lenz_name) != 0 ||
            strstr(material->name, eye_name) != 0) {
            alpha = model->fields.specialBlockAlpha;
            reflect_alpha = 2.0f;
            special_material = 1;
            s_nPixelTestType = -1;
        }
    }

    for (i = 0; i < *&model->fields.partPixelAlphaCount; i++) {
        s_nPixelTestType = -1;
        if (((short *)((u8 *)model + i * 4))[361] ==
            *&material->partId) {
            pixel_reference = model->fields.partPixelAlpha[i].alpha;
            break;
        }
    }

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    material_flags = material->materialFlags;
    if (material_flags & 0x10)
        pixel_reference = 0x7F;
    if (s_pModelCache != model->fields.model) {
        material_flags = nmlPacketInvalidatePixelModel(
            &model->fields.model, &material->materialFlags);
    }

    if (pixel_control_flags & 0x10000) {
        if (material_flags & 4) {
            if (pixel_control_flags & 0x20000) {
                if (s_nPixelTestType != 1) {
                    u64 blend_mode = 0x68;

                    s_nPixelTestType = 1;
                    if (*&material->materialFlags & 0x8000) {
                        nmlPacketAppendGsWords(0x47, 0, 0x50000);
                    } else {
                        nmlPacketAppendGsWords(0x47, 0,
                            (u32)nmlPacketMakeGsTest(1, 5, pixel_reference,
                                                    0, 0, 0, 1, 2));
                    }
                    nmlPacketAppendGsValue(0x42, &blend_mode);
                }
            } else if (model->fields.render_status & 0x20) {
                nmlPacketSetPixelBlend(model, 2, alpha, reflect_alpha,
                                       pixel_reference, 0);
            } else {
                nmlPacketSetPixelBlend(model, 3, alpha, reflect_alpha,
                                       pixel_reference, 1);
            }
        } else if (pixel_control_flags & 0x20000) {
            u64 blend_mode;

            if (s_nPixelTestType != 4) {
                s_nPixelTestType = 4;
                nmlPacketAppendGsWords(0x47, 0, 0x50000);
                blend_mode = 0x68;
                nmlPacketAppendGsValue(0x42, &blend_mode);
                nmlPacketAddPixelTestPacket(0x47, 0x50000, 0x50000,
                                            0x42, *&model->fields.alpha,
                                            blend_mode);
            }
        } else {
            nmlPacketSetPixelBlend(model, 5, alpha, reflect_alpha,
                                   pixel_reference, 0);
        }
    } else if (material_flags & 4) {
        if (s_nPixelTestType != 6) {
            u64 test_value;
            u32 depth_base;

            pixel_reference =
                (int)(model->fields.transparency * (float)pixel_reference);
            test_value = nmlPacketMakeGsTest(1, 5, pixel_reference,
                                             0, 0, 0, 1, 2);
            depth_base = sRender.depth_buffer_base;

            nmlPacketAppendGsWords(0x47, 0, (u32)test_value);
            s_nPixelTestType = 6;
            nmlPacketAddPixelTestPacket(0x47, test_value, 0x50001, 0x4E,
                                        depth_base | 0x01000000,
                                        depth_base | (0x8080ULL << 0x11));
        }
    } else if (s_nPixelTestType != 7) {
        u32 depth_base = sRender.depth_buffer_base;

        packed_alpha = nmlPacketMakeGsFixedAlpha((int)(alpha * 128.0f));
        s_nPixelTestType = 7;
        nmlPacketAppendGsWords(0x47, 0, 0x50000);
        nmlPacketAppendGsValue(0x43, &packed_alpha);
        nmlPacketAddPixelTestPacket(0x47, 0x50000, 0x50000, 0x4E,
                                    depth_base | 0x01000000,
                                    depth_base | (0x8080ULL << 0x11));
    }

    if (special_material)
        s_nPixelTestType = -1;
}

int nmlPacketDirectData(const void *data, int size)
{
    s_pPacket.sdk = xglPacketGetCurrent();
    if (sceVif1PkSize(s_pPacket.sdk) * 0x10u +
            (s_pPacket.sdk->limit - (u32)s_pPacket.sdk->cursor) + 0x10000u >
        0x200000u)
        return 1;

    sceVif1PkRef(s_pPacket.sdk, data, size, 0, 0, 0);
    return 0;
}

void nmlPacketSetCurrent(void)
{
    s_pPacket.sdk = xglPacketGetCurrent();
}

void nmlPacketAddReflRot(const NmlModelBlock *material,
                         const LayoutStore *model)
{
    float matrix[4][4];
    int type = 1;
    NmlCurrentPacket selected;

    if ((material->materialFlags & 0x2000u) != 0)
        type = 2;

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;

    if (type != 1) {
        if (type != 2)
            return;
        if (s_nReflRotType == type)
            return;
        xglMatrixStackUnit();
        xglMatrixStackRotY(s_inReflRotY);
        xglMatrixStackRotX(s_inReflRotX);
        xglMatrixStackSave(matrix);
    } else {
        if (s_nReflRotType == type)
            return;
        xglMatrixUnit(matrix);
    }

    selected = s_pPacket;
    sceVif1PkCnt(selected.sdk, 0);
    selected = s_pPacket;
    sceVif1PkOpenUpkCode(selected.sdk, 1004, 108, 1, 1);
    selected = s_pPacket;
    sceVif1PkAddUpkData128N(selected.sdk, matrix, 3);
    selected = s_pPacket;
    sceVif1PkCloseUpkCode(selected.sdk);
    s_nReflRotType = type;
}

static inline void nmlScreenClearSetScratch(NmlScreenGsScratchEntry *entry,
                                          u32 address, u64 value)
{
    entry->payload.value = value;
    entry->address.value = address;
}

static inline void nmlScreenClearAppendGs(u32 address, const u64 *value)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = *value;
    g_aGsTag[g_nGsEntry * 2 + 3].value = address;
    g_nGsEntry++;
}

void nmlPacketAddScreenClear(void)
{
    u64 depth_format = 0x01000000;
    NmlScreenGsScratchEntry *scratch;
    NmlScreenGsScratchEntry *alpha_override = (NmlScreenGsScratchEntry *)0x70000000;
    NmlScreenGsScratchEntry *depth_buffer = (NmlScreenGsScratchEntry *)0x70000010;
    NmlScreenGsScratchEntry *pixel_test = (NmlScreenGsScratchEntry *)0x70000020;
    NmlScreenGsScratchEntry *frame_buffer = (NmlScreenGsScratchEntry *)0x70000030;
    NmlScreenGsScratchEntry *rgba = (NmlScreenGsScratchEntry *)0x70000040;
    NmlScreenGsScratchEntry *primitive = (NmlScreenGsScratchEntry *)0x70000050;
    NmlScreenGsScratchEntry *first_corner = (NmlScreenGsScratchEntry *)0x70000060;
    NmlScreenGsScratchEntry *last_corner = (NmlScreenGsScratchEntry *)0x70000070;
    int remaining;
    int frame_width;
    int frame_height;
    u64 rgba_value;

    rgba_value = 0x3f80000000000000ULL;

    nmlScreenClearSetScratch(alpha_override, 74, 0);
    nmlScreenClearSetScratch(depth_buffer, 78,
        sRender.depth_buffer_base | depth_format);
    nmlScreenClearSetScratch(pixel_test, 71, 0x30000);
    frame_width = sRender.frame_width;
    nmlScreenClearSetScratch(frame_buffer, 76,
        sRender.display_buffer_base |
        (u64)((frame_width / 64) * 0x10000) |
        ((u64)sRender.frame_psm << 24));
    nmlScreenClearSetScratch(rgba, 1, rgba_value);
    primitive->payload.value = 6;
    primitive->address.value = 0;
    frame_height = sRender.frame_height;
    nmlScreenClearSetScratch(first_corner, 4,
        ((u64)(2048 - frame_width) << 4) |
        ((u64)(2048 - frame_height) << 20));
    nmlScreenClearSetScratch(last_corner, 4,
        ((u64)(2048 + frame_width) << 4) |
        ((u64)(2048 + frame_height) << 20));
    scratch = (NmlScreenGsScratchEntry *)0x70000000;
    g_nGsEntry = 0;
    for (remaining = 7; remaining >= 0; remaining--) {
        nmlScreenClearAppendGs(scratch[0].address.words[0], &scratch[0].payload.value);
        scratch++;
    }
    nmlPacketAddGsFlushWide();
}

void nmlPacketGsInit(void)
{
    g_nGsEntry = 0;
}

void nmlPacketAddGsClamp(u64 clamp)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = clamp;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 8;
    g_nGsEntry++;
}

void nmlPacketAddGsPixeltest(u32 pixeltest)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = pixeltest;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = 0;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 71;
    g_nGsEntry++;
}

void nmlPacketAddGsPixeltest1(u32 pixeltest)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = pixeltest;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = 0;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 72;
    g_nGsEntry++;
}

void nmlPacketAddGsZbuf(u32 zbuf)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value =
        sRender.depth_buffer_base | ((u64)zbuf << 32) | 0x1000000;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 78;
    g_nGsEntry++;
}

void nmlPacketAddGsZbuf1(u32 zbuf)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value =
        sRender.depth_buffer_base | ((u64)zbuf << 32) | 0x1000000;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 79;
    g_nGsEntry++;
}

void nmlPacketAddGsTexture(void)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = 0;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = 0;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 63;
    g_nGsEntry++;
}

void nmlPacketAddGsAlpha(u64 alpha)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = alpha;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 66;
    g_nGsEntry++;
}

void nmlPacketAddGsAlpha1(u64 alpha)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = alpha;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 67;
    g_nGsEntry++;
}

void nmlPacketAddGsScissor(u64 scissor)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = scissor;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 64;
    g_nGsEntry++;
}

void nmlPacketAddGsScissor1(u64 scissor)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = scissor;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 65;
    g_nGsEntry++;
}

void nmlPacketAddGsFBA(u32 fba)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = fba != 0;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = 0;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 74;
    g_nGsEntry++;
}

void nmlPacketAddGsFBA1(u32 fba)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = fba != 0;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = 0;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 75;
    g_nGsEntry++;
}

void nmlPacketAddGsFogCol(const float color[3])
{
    u64 red = (u32)color[0];
    u64 green = (u64)(u32)color[1] << 8;
    u64 blue = (u64)(u32)color[2] << 16;
    int entry = g_nGsEntry * 2;

    g_aGsTag[entry + 2].words[0] = red | green | blue;
    g_nGsEntry++;
    g_aGsTag[entry + 2].words[1] = 0;
    g_aGsTag[entry + 3].value = 61;
}

void nmlPacketAddGsFrame(int frame_base, int frame_mask)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value =
        (frame_base & 0xffff) | ((u64)(sRender.frame_width / 64) << 16) |
        ((u64)sRender.frame_psm << 24) | ((u64)frame_mask << 32);
    g_aGsTag[g_nGsEntry * 2 + 3].value = 76;
    g_nGsEntry++;
}

void nmlPacketAddGsFrame1(int frame_base, int frame_mask)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value =
        (frame_base & 0xffff) | ((u64)(sRender.frame_width / 64) << 16) |
        ((u64)sRender.frame_psm << 24) | ((u64)frame_mask << 32);
    g_aGsTag[g_nGsEntry * 2 + 3].value = 77;
    g_nGsEntry++;
}

void nmlPacketAddGsPAbe(u64 pabe)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = pabe;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 73;
    g_nGsEntry++;
}

void nmlPacketAddGsPrmode(NmlModelBlock *material)
{
    u64 mode;

    mode = 72;
    if (material->materialFlags & 1)
        mode = 88;
    else if (material->materialFlags & 2)
        mode = 72;
    g_aGsTag[g_nGsEntry * 2 + 2].value = mode;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 27;
    g_nGsEntry++;
}

void nmlPacketAddGsPrmodecont(u64 prmode_control)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = prmode_control;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 26;
    g_nGsEntry++;
}

void packet_gs_entry32(u32 reg, u32 high, u32 low)
{
    g_aGsTag[g_nGsEntry * 2 + 2].words[0] = low;
    g_aGsTag[g_nGsEntry * 2 + 2].words[1] = high;
    g_aGsTag[g_nGsEntry++ * 2 + 3].value = reg;
}

void packet_gs_entry64(u32 reg, const u64 *value)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = *value;
    g_aGsTag[g_nGsEntry++ * 2 + 3].value = reg;
}

void nmlPacketAddGsFba(u64 fba)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = fba;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 74;
    g_nGsEntry++;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const NmlCircleOffsetTable circleXOffsetTable = { .values = { -1.0f, 1.0f, -1.0f, 1.0f } };

const NmlCircleOffsetTable circleZOffsetTable = { .values = { -1.0f, -1.0f, 1.0f, 1.0f } };

const float inReflRotYStep = 0.0099999997764825821f;

const float alphaLimitHigh = 1.0199999809265137f;

const float alphaLimitLow = 0.98000001907348633f;

const float vectorLengthHigh = 1.0199999809265137f;

const float vectorLengthLow = 0.98000001907348633f;

const float previousVectorLengthHigh = 1.0199999809265137f;

const float previousVectorLengthLow = 0.98000001907348633f;

float circleRadiusScale = 0.40000000596046448f;

const float circleHeightOffset = 0.004999999888241291f;

static int s_nModelParts = 0;

static int s_nLightPara = 0;

static int s_nPixelTestType = 0;

static const float s_aNormalTbl[114][4] = { { 0.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, -0.86599999666213989f, 0.5f, 1.0f }, { 0.19099999964237213f, -0.86599999666213989f, 0.46200001239776611f, 1.0f }, { 0.35400000214576721f, -0.86599999666213989f, 0.35400000214576721f, 1.0f }, { 0.46200001239776611f, -0.86599999666213989f, 0.19099999964237213f, 1.0f }, { 0.5f, -0.86599999666213989f, 0.0f, 1.0f }, { 0.46200001239776611f, -0.86599999666213989f, -0.19099999964237213f, 1.0f }, { 0.35400000214576721f, -0.86599999666213989f, -0.35400000214576721f, 1.0f }, { 0.19099999964237213f, -0.86599999666213989f, -0.46200001239776611f, 1.0f }, { 0.0f, -0.86599999666213989f, -0.5f, 1.0f }, { -0.19099999964237213f, -0.86599999666213989f, -0.46200001239776611f, 1.0f }, { -0.35400000214576721f, -0.86599999666213989f, -0.35400000214576721f, 1.0f }, { -0.46200001239776611f, -0.86599999666213989f, -0.19099999964237213f, 1.0f }, { -0.5f, -0.86599999666213989f, -0.0f, 1.0f }, { -0.46200001239776611f, -0.86599999666213989f, 0.19099999964237213f, 1.0f }, { -0.35400000214576721f, -0.86599999666213989f, 0.35400000214576721f, 1.0f }, { -0.19099999964237213f, -0.86599999666213989f, 0.46200001239776611f, 1.0f }, { 0.0f, -0.7070000171661377f, 0.7070000171661377f, 1.0f }, { 0.27099999785423279f, -0.7070000171661377f, 0.65299999713897705f, 1.0f }, { 0.5f, -0.7070000171661377f, 0.5f, 1.0f }, { 0.65299999713897705f, -0.7070000171661377f, 0.27099999785423279f, 1.0f }, { 0.7070000171661377f, -0.7070000171661377f, 0.0f, 1.0f }, { 0.65299999713897705f, -0.7070000171661377f, -0.27099999785423279f, 1.0f }, { 0.5f, -0.7070000171661377f, -0.5f, 1.0f }, { 0.27099999785423279f, -0.7070000171661377f, -0.65299999713897705f, 1.0f }, { 0.0f, -0.7070000171661377f, -0.7070000171661377f, 1.0f }, { -0.27099999785423279f, -0.7070000171661377f, -0.65299999713897705f, 1.0f }, { -0.5f, -0.7070000171661377f, -0.5f, 1.0f }, { -0.65299999713897705f, -0.7070000171661377f, -0.27099999785423279f, 1.0f }, { -0.7070000171661377f, -0.7070000171661377f, -0.0f, 1.0f }, { -0.65299999713897705f, -0.7070000171661377f, 0.27099999785423279f, 1.0f }, { -0.5f, -0.7070000171661377f, 0.5f, 1.0f }, { -0.27099999785423279f, -0.7070000171661377f, 0.65299999713897705f, 1.0f }, { 0.0f, -0.5f, 0.86599999666213989f, 1.0f }, { 0.33100000023841858f, -0.5f, 0.80000001192092896f, 1.0f }, { 0.6119999885559082f, -0.5f, 0.6119999885559082f, 1.0f }, { 0.80000001192092896f, -0.5f, 0.33100000023841858f, 1.0f }, { 0.86599999666213989f, -0.5f, 0.0f, 1.0f }, { 0.80000001192092896f, -0.5f, -0.33100000023841858f, 1.0f }, { 0.6119999885559082f, -0.5f, -0.6119999885559082f, 1.0f }, { 0.33100000023841858f, -0.5f, -0.80000001192092896f, 1.0f }, { 0.0f, -0.5f, -0.86599999666213989f, 1.0f }, { -0.33100000023841858f, -0.5f, -0.80000001192092896f, 1.0f }, { -0.6119999885559082f, -0.5f, -0.6119999885559082f, 1.0f }, { -0.80000001192092896f, -0.5f, -0.33100000023841858f, 1.0f }, { -0.86599999666213989f, -0.5f, -0.0f, 1.0f }, { -0.80000001192092896f, -0.5f, 0.33100000023841858f, 1.0f }, { -0.6119999885559082f, -0.5f, 0.6119999885559082f, 1.0f }, { -0.33100000023841858f, -0.5f, 0.80000001192092896f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.38299998641014099f, 0.0f, 0.92400002479553223f, 1.0f }, { 0.7070000171661377f, 0.0f, 0.7070000171661377f, 1.0f }, { 0.92400002479553223f, 0.0f, 0.38299998641014099f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.92400002479553223f, 0.0f, -0.38299998641014099f, 1.0f }, { 0.7070000171661377f, 0.0f, -0.7070000171661377f, 1.0f }, { 0.38299998641014099f, 0.0f, -0.92400002479553223f, 1.0f }, { 0.0f, 0.0f, -1.0f, 1.0f }, { -0.38299998641014099f, 0.0f, -0.92400002479553223f, 1.0f }, { -0.7070000171661377f, 0.0f, -0.7070000171661377f, 1.0f }, { -0.92400002479553223f, 0.0f, -0.38299998641014099f, 1.0f }, { -1.0f, 0.0f, -0.0f, 1.0f }, { -0.92400002479553223f, 0.0f, 0.38299998641014099f, 1.0f }, { -0.7070000171661377f, 0.0f, 0.7070000171661377f, 1.0f }, { -0.38299998641014099f, 0.0f, 0.92400002479553223f, 1.0f }, { 0.0f, 0.5f, 0.86599999666213989f, 1.0f }, { 0.33100000023841858f, 0.5f, 0.80000001192092896f, 1.0f }, { 0.6119999885559082f, 0.5f, 0.6119999885559082f, 1.0f }, { 0.80000001192092896f, 0.5f, 0.33100000023841858f, 1.0f }, { 0.86599999666213989f, 0.5f, 0.0f, 1.0f }, { 0.80000001192092896f, 0.5f, -0.33100000023841858f, 1.0f }, { 0.6119999885559082f, 0.5f, -0.6119999885559082f, 1.0f }, { 0.33100000023841858f, 0.5f, -0.80000001192092896f, 1.0f }, { 0.0f, 0.5f, -0.86599999666213989f, 1.0f }, { -0.33100000023841858f, 0.5f, -0.80000001192092896f, 1.0f }, { -0.6119999885559082f, 0.5f, -0.6119999885559082f, 1.0f }, { -0.80000001192092896f, 0.5f, -0.33100000023841858f, 1.0f }, { -0.86599999666213989f, 0.5f, -0.0f, 1.0f }, { -0.80000001192092896f, 0.5f, 0.33100000023841858f, 1.0f }, { -0.6119999885559082f, 0.5f, 0.6119999885559082f, 1.0f }, { -0.33100000023841858f, 0.5f, 0.80000001192092896f, 1.0f }, { 0.0f, 0.7070000171661377f, 0.7070000171661377f, 1.0f }, { 0.27099999785423279f, 0.7070000171661377f, 0.65299999713897705f, 1.0f }, { 0.5f, 0.7070000171661377f, 0.5f, 1.0f }, { 0.65299999713897705f, 0.7070000171661377f, 0.27099999785423279f, 1.0f }, { 0.7070000171661377f, 0.7070000171661377f, 0.0f, 1.0f }, { 0.65299999713897705f, 0.7070000171661377f, -0.27099999785423279f, 1.0f }, { 0.5f, 0.7070000171661377f, -0.5f, 1.0f }, { 0.27099999785423279f, 0.7070000171661377f, -0.65299999713897705f, 1.0f }, { 0.0f, 0.7070000171661377f, -0.7070000171661377f, 1.0f }, { -0.27099999785423279f, 0.7070000171661377f, -0.65299999713897705f, 1.0f }, { -0.5f, 0.7070000171661377f, -0.5f, 1.0f }, { -0.65299999713897705f, 0.7070000171661377f, -0.27099999785423279f, 1.0f }, { -0.7070000171661377f, 0.7070000171661377f, -0.0f, 1.0f }, { -0.65299999713897705f, 0.7070000171661377f, 0.27099999785423279f, 1.0f }, { -0.5f, 0.7070000171661377f, 0.5f, 1.0f }, { -0.27099999785423279f, 0.7070000171661377f, 0.65299999713897705f, 1.0f }, { 0.0f, 0.86599999666213989f, 0.5f, 1.0f }, { 0.19099999964237213f, 0.86599999666213989f, 0.46200001239776611f, 1.0f }, { 0.35400000214576721f, 0.86599999666213989f, 0.35400000214576721f, 1.0f }, { 0.46200001239776611f, 0.86599999666213989f, 0.19099999964237213f, 1.0f }, { 0.5f, 0.86599999666213989f, 0.0f, 1.0f }, { 0.46200001239776611f, 0.86599999666213989f, -0.19099999964237213f, 1.0f }, { 0.35400000214576721f, 0.86599999666213989f, -0.35400000214576721f, 1.0f }, { 0.19099999964237213f, 0.86599999666213989f, -0.46200001239776611f, 1.0f }, { 0.0f, 0.86599999666213989f, -0.5f, 1.0f }, { -0.19099999964237213f, 0.86599999666213989f, -0.46200001239776611f, 1.0f }, { -0.35400000214576721f, 0.86599999666213989f, -0.35400000214576721f, 1.0f }, { -0.46200001239776611f, 0.86599999666213989f, -0.19099999964237213f, 1.0f }, { -0.5f, 0.86599999666213989f, -0.0f, 1.0f }, { -0.46200001239776611f, 0.86599999666213989f, 0.19099999964237213f, 1.0f }, { -0.35400000214576721f, 0.86599999666213989f, 0.35400000214576721f, 1.0f }, { -0.19099999964237213f, 0.86599999666213989f, 0.46200001239776611f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
