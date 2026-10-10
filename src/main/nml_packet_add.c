#include "common.h"

#include "shared.h"

#include "main/xgl_2.h"

#include "nml_packet_add.h"

#include "main/xgl_packet.h"

#include "main/ssd_init.h"

typedef struct NmlAttributePacket NmlAttributePacket;

/* One current packet is shared by the SDK writer and attribute allocator. */
typedef union NmlCurrentPacket {
    XglPacket *sdk;
    NmlAttributePacket *attributes;
} NmlCurrentPacket;

static NmlCurrentPacket s_pPacket = { 0 };

static void *s_pCacheTexture = 0;

static void *s_pMatrixCache = 0;

static void *s_pModelCache = 0;

static void *s_pModelLayout = 0;

static void *s_pLightLayout = 0;

static int s_nProgType = -1;

static int s_nReflRotType = -1;

static float s_inReflRotX = 0.0f;

static float s_inReflRotY = 0.0f;

const char eye_name[8] = "eye";

const char lenz_name[8] = "lenz";

/* VIF command MSCAL: start the VU1 microprogram at immediate * 8. */

#define VIF_CODE_MSCAL 0x14000000

/* VIF command FLUSH: wait for the VU1 microprogram to finish. */

#define VIF_CODE_FLUSH 0x11000000

/*
 * One s_aUcodeTbl row (0x1c bytes; the 168-byte symbol holds six rows).
 * programs[type - 1] lists the VU1 program byte addresses of microcode family
 * `type`, indexed by the material's variant slot.
 */

typedef struct UcodeTableRow {
    u32 *programs[7];
} UcodeTableRow;

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
typedef union GsTag {
    u64 value;
    u32 words[2];
} GsTag;

GsTag g_aGsTag[162] = {{0}};

RssdWorkFlags RssdWork = {0};

char RssdStrWork[32] = {0};

extern int g_aSubWindow[4];

/* One VIF unpack value passed to sceVif1PkAddUpkData128 by value. */

typedef unsigned int Quadword __attribute__((mode(TI)));

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

typedef union NmlPacketData128 {
    u32 words[4];
    Quadword quad;
} NmlPacketData128;

extern void sceVif1PkAddUpkData128(NmlPacket packet, Quadword data);

extern void *memcpy(void *destination, const void *source, unsigned int count);


extern int g_nGsEntry;

const char D_004DC4D0[4] = "";

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

#define VIF_CODE_MSCNT 0x17000000

extern int sceVif1PkSize(XglPacket *packet);

typedef struct NmlZbufRenderState {
    unsigned char unmodeled_00[8];
    u16 depth_buffer_base;
} NmlZbufRenderState;

extern NmlZbufRenderState sRender;

static void _CurMatrixSet(const Matrix4 matrix)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)\n\t"
        "nop"
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
        "sqc2 vf30, 48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _CurMatrixMul(const Matrix4 matrix)
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
        "vmulw.xyzw vf29, vf4, vf0w\n\t"
        "nop"
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
        "sqc2 vf31, 0(%1)\n\t"
        "nop"
        :
        : "r"(source), "r"(destination)
        : "memory"
    );
}

static void _CurMatrixMul33norm(Vector4 *destination, const Vector4 *source)
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
        "sqc2 vf25, 32(%1)\n\t"
        "nop"
        :
        : "r"(source), "r"(destination)
        : "memory"
    );
}

static void _CurSetViewScaleTrans(const Vector4 *view_scale, const Vector4 *view_translation)
{
    __asm__ __volatile__(
        "lqc2 vf25, 0(%0)\n\t"
        "lqc2 vf26, 0(%1)\n\t"
        "nop"
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

static void _CurSetMatrix(const Matrix4 matrix)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFlush);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFlushWide);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddReflectParam);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddTransMicrocodeInit);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddTransMicrocode);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddScreen);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddBlockMaterial);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketMakeCircleTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSendCircleTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketTextureTrans);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddParts);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddTextureParam);

static int add_exec_prog(NmlMaterialRenderState *material, NmlModelRenderState *model,
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
    proreal = model->proreal;
    if (model->proreal_override != 0)
        proreal = model->proreal_override;

    if ((model->render_status & 0x100000) != 0 &&
        (strstr(material->name, lenz_name) != 0 ||
         strstr(material->name, eye_name) != 0))
        eye_material = 1;

    s_pPacket.sdk = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket.sdk, 0);

    switch (s_nProgType) {
    case 7: {
        int slot = 0;

        if (material->material_flags & 0x10)
            slot = 1;
        else if (material->render_flags & 1) {
            slot = 2;
            if (material->render_flags & 2)
                slot = 3;
            else if (material->render_flags & 4)
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

        if (material->render_flags & 1) {
            slot = 2;
            if (material->render_flags & 2)
                slot = 3;
            else if (material->render_flags & 4)
                slot = 4;
        }
        else if (material->material_flags & 0x10)
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

        if ((material->material_flags & 0x10) == 0) {
            slot = 1;
            if ((material->material_flags & 0x1000) &&
                (!(model->render_level & 0x10) || eye_material))
                slot = 4;
            else if ((material->material_flags & 8) &&
                     (!(model->render_level & 2) || eye_material)) {
                if ((material->material_flags & 0x800) ||
                    ((model->render_level & 1) && !eye_material))
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

        if (material->render_flags & 2)
            slot = 4;
        else if (material->render_flags & 4)
            slot = 8;
        else
            slot = 0;
        if ((material->material_flags & 0x1000) &&
            (!(model->render_level & 0x10) || eye_material))
            slot += 3;
        else if ((material->material_flags & 8) &&
                 (!(model->render_level & 2) || eye_material)) {
            if ((material->material_flags & 0x800) ||
                ((model->render_level & 1) && !eye_material))
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

        if (material->material_flags & 0x10) {
            slot = 3;
            if (proreal->projection_entry)
                slot = 4;
        }
        else if (material->render_flags & 1) {
            if (material->render_flags & 2)
                slot = 1;
            else if (material->render_flags & 4)
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

        if (material->render_flags & 2)
            slot = 4;
        else if (material->render_flags & 4)
            slot = 8;
        else
            slot = 0;
        if ((material->material_flags & 0x1000) &&
            (!(model->render_level & 0x10) || eye_material))
            slot += 3;
        else if ((material->material_flags & 8) &&
                 (!(model->render_level & 2) || eye_material)) {
            if ((material->material_flags & 0x800) ||
                ((model->render_level & 1) && !eye_material))
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

        if ((material->material_flags & 0x10) == 0) {
            slot = 2;
            if ((material->material_flags & 0x1000) &&
                (!(model->render_level & 0x10) || eye_material))
                slot = 5;
            else if ((material->material_flags & 8) &&
                     (!(model->render_level & 2) || eye_material)) {
                if ((material->material_flags & 0x800) ||
                    ((model->render_level & 1) && !eye_material))
                    slot = 3;
                else
                    slot = 4;
            }
        }
        else if (material->render_flags & 0x10)
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

void nmlPacketAddExecProg(NmlMaterialRenderState *material,
                          NmlModelRenderState *model,
                          int allow_extended_programs,
                          int add_transform_data)
{
    if (add_transform_data) {
        if (!add_exec_prog(material, model, 0))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if ((material->render_flags & 0x80)
        || (model->render_status & 0x100)) {
        if (!add_exec_prog(material, model, 1))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if ((material->render_flags & 0x100)
        || (model->render_status & 0x200)) {
        if (!add_exec_prog(material, model, 2))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if (material->render_flags & 0x200) {
        if (add_exec_prog(material, model, 2))
            nmlPacketAddTransData(material);
        nmlPacketAddWaitMicrocode();
        if (!add_exec_prog(material, model, 1))
            return;
        nmlPacketAddTransData(material);
        return;
    }

    if (allow_extended_programs
        && !(material->render_flags & 0x400)
        && !(model->render_status & 0x8)) {
        if (model->render_level & 0x4) {
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

void nmlPacketAddTransData(NmlMaterialRenderState *material)
{
    typedef struct NmlMaterialTransPrefix {
        char name[0x20];
        u32 render_flags;
        int data_offset;
        int data_size;
    } NmlMaterialTransPrefix;
    NmlMaterialTransPrefix *transfer = (void *)material;
    NmlCurrentPacket selected;

    selected.sdk = xglPacketGetCurrent();
    s_pPacket = selected;
    /* The span starts data_offset bytes into the material (32-bit EE addresses). */
    sceVif1PkRef(selected.sdk, (void *)(transfer->data_offset + (u32)material),
                 transfer->data_size / 16, 0, 0, 0);
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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSetAttributeData64N);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSetAttributeData16N);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGifTag);

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

void nmlPacketAddFog(NmlModelRenderState *model, int viewport_index)
{
    s_pPacket.sdk = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket.sdk, 0);

    if (g_aSubWindow[viewport_index] != 0)
        model->fog_color[3] = ((int)(model->fog_intensity[viewport_index] * 255.0f)) << 4;

    sceVif1PkOpenUpkCode(s_pPacket.sdk, 998, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket.sdk, model->fog_color, 1);
    sceVif1PkCloseUpkCode(s_pPacket.sdk);
    sceVif1PkOpenUpkCode(s_pPacket.sdk, 1007, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket.sdk, model->fog_parameters, 1);
    sceVif1PkCloseUpkCode(s_pPacket.sdk);
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddPixelControl);

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

void nmlPacketAddReflRot(const NmlMaterialRenderState *material,
                         const NmlModelRenderState *model)
{
    float matrix[4][4];
    int type = 1;
    NmlCurrentPacket selected;

    if ((material->material_flags & 0x2000u) != 0)
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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddScreenClear);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFogCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFrame);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFrame1);

void nmlPacketAddGsPAbe(u64 pabe)
{
    g_aGsTag[g_nGsEntry * 2 + 2].value = pabe;
    g_aGsTag[g_nGsEntry * 2 + 3].value = 73;
    g_nGsEntry++;
}

void nmlPacketAddGsPrmode(NmlMaterialRenderState *material)
{
    u64 mode;

    mode = 72;
    if (material->material_flags & 1)
        mode = 88;
    else if (material->material_flags & 2)
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
