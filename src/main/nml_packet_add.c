#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "nml_packet_add.h"
#include "main/xgl_packet.h"

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

extern UcodeTableRow s_aUcodeTbl[];

extern int g_aSubWindow[4];

/* One VIF unpack value passed to sceVif1PkAddUpkData128 by value. */
typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union NmlPacketData128 {
    u32 words[4];
    Quadword quad;
} NmlPacketData128;

extern void sceVif1PkAddUpkData128(NmlPacket packet, Quadword data);

/*
 * Every case ends with its own "queue the program or clear result" tail.  The
 * compiler cross-jumps the seven copies back into the single tail seen in the
 * original, but before that each copy is a reference to result, which is what
 * keeps result in a callee-saved register (s4).  Written once after the switch result is spilled to
 * the stack and the program pointers take s4..s8 instead.
 */

/*
 * _CurMatrixSet loads a 4x4 matrix into vf27..vf30, the VU0 macro-mode
 * registers this TU's packet builder keeps as its persistent "current
 * matrix" across _CurMatrixMul and _CurApplyMatrix (main VA 0x00236448,
 * ee-vu-cop2, docs/ps2-capabilities.md).
 */
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

/*
 * _CurMatrixGet stores the VU0 macro-mode "current matrix" vf27..vf30
 * _CurMatrixSet/_CurSetMatrix left live back into the caller's 4x4 matrix
 * (main VA 0x00236460, ee-vu-cop2, docs/ps2-capabilities.md).
 */
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

/*
 * _CurMatrixMul loads a 4x4 matrix and concatenates it onto the VU0
 * macro-mode "current matrix" _CurMatrixSet left in vf27..vf30, writing the
 * product's three scaled rows back into vf27..vf29 (main VA 0x00236478,
 * ee-vu-cop2, docs/ps2-capabilities.md).
 */
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

/*
 * _CurApplyMatrix transforms one vector by the VU0 macro-mode "current
 * matrix" (vf27..vf30) a prior _CurMatrixSet/_CurMatrixMul left live,
 * storing the result to destination (main VA 0x002364e0, ee-vu-cop2,
 * docs/ps2-capabilities.md).
 */
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

/*
 * _CurMatrixMul33norm transforms the caller's three-row orientation matrix
 * by the VU0 macro-mode "current matrix" vf27..vf29 a prior _CurMatrixSet/
 * _CurMatrixMul left live, normalizes each transformed row to unit length
 * and stores the three rows to destination (main VA 0x00236500,
 * ee-vu-cop2, docs/ps2-capabilities.md).
 */
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

/*
 * _CurSetViewScaleTrans loads the view-scale vector into the VU0 macro-mode
 * register vf25 and the view-translation vector into vf26, the same
 * persistent "current matrix" state family the _CurMatrixSet/_CurMatrixMul/
 * _CurApplyMatrix group reads (main VA 0x002365c0, ee-vu-cop2,
 * docs/ps2-capabilities.md).
 */
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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurRotTransPersClip_002365D0);

/*
 * _CurSetMatrix loads a 4x4 matrix into the VU0 macro-mode "current matrix"
 * registers vf27..vf30, the same persistent state _CurMatrixSet writes (main
 * VA 0x00236640, ee-vu-cop2, docs/ps2-capabilities.md).
 */
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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddPixelTestPacket);

void nmlPacketAddWaitMicrocode(void)
{
    s_pPacket = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket, 0);
    sceVif1PkAddCode(s_pPacket, VIF_CODE_FLUSH);
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddBlockMaterial);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketMakeCircleTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSendCircleTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketTextureTrans);

/* The cached texture built by the (still asm) texture cache builder;
 * cleared to invalidate it, like s_pMatrixCache/s_pModelCache/
 * s_pModelLayout/s_pLightLayout below. */
extern void *s_pCacheTexture;

void nmlPacketClrTextureCache(void)
{
    s_pCacheTexture = 0;
}

/* The cached VU1 upload state a model was last sent with: matrix data,
 * model geometry and the light/model attribute layouts. Cleared together
 * to force the next nmlPacket* draw call to resend all four. */
extern void *s_pMatrixCache;
extern void *s_pModelCache;
extern void *s_pModelLayout;
extern void *s_pLightLayout;

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

    s_pPacket = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket, 0);

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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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
            sceVif1PkAddCode(s_pPacket, (program >> 3) | VIF_CODE_MSCAL);
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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddTransData);

extern void *memcpy(void *destination, const void *source, unsigned int count);

/*
 * Both allocators here (and nmlPacketSetAttributeData16N/64/64N, still
 * INCLUDE_ASM in this TU) share one pattern: fetch the current packet,
 * decrement its cursor (main/xgl_packet.h, "+0x24: the write cursor") by the
 * requested size, and hand back the new cursor as the reserved storage's
 * address. A plain `packet->cursor` read after the subtraction lets gcc 2.96
 * -O2 prove the earlier write cannot alias it and cache the value across the
 * memcpy call, dropping the reload the original object performs; reading
 * through a cast on the field's own address defeats that alias proof.
 */
u8 *nmlPacketSetAttributeData(const void *data, u32 size)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    s_pPacket = packet;
    *(u8 **)&packet->cursor -= size;
    memcpy(*(u8 **)&s_pPacket->cursor, data, size);
    return *(u8 **)&s_pPacket->cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSetAttributeData64);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSetAttributeData64N);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketSetAttributeData16N);

/* Same aliasing proof as nmlPacketSetAttributeData above (main 0x00238e00). */
u8 *nmlPacketSetAttributeAlloc16N(u32 count)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    s_pPacket = packet;
    *(u8 **)&packet->cursor -= count * 0x10;
    return *(u8 **)&s_pPacket->cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGifTag);

void nmlPacketAddGifTagStandard(int eop, int count)
{
    NmlPacketData128 gif_tag;

    s_pPacket = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket, 0);

    gif_tag.words[0] = 0x8000;
    gif_tag.words[1] = ((u32)eop << 0x13) | ((u32)count << 0x15) | 0x30064000;
    gif_tag.words[2] = 0x412;
    gif_tag.words[3] = 0;
    sceVif1PkOpenUpkCode(s_pPacket, 0x3f3, 0x6c, 1, 1);
    sceVif1PkAddUpkData128(s_pPacket, gif_tag.quad);
    sceVif1PkCloseUpkCode(s_pPacket);
}

void nmlPacketAddFog(NmlModelRenderState *model, int viewport_index)
{
    s_pPacket = xglPacketGetCurrent();
    sceVif1PkCnt(s_pPacket, 0);

    if (g_aSubWindow[viewport_index] != 0)
        model->fog_color[3] = ((int)(model->fog_intensity[viewport_index] * 255.0f)) << 4;

    sceVif1PkOpenUpkCode(s_pPacket, 998, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket, model->fog_color, 1);
    sceVif1PkCloseUpkCode(s_pPacket);
    sceVif1PkOpenUpkCode(s_pPacket, 1007, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket, model->fog_parameters, 1);
    sceVif1PkCloseUpkCode(s_pPacket);
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddPixelControl);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketDirectData);

void nmlPacketSetCurrent(void)
{
    s_pPacket = xglPacketGetCurrent();
}

void nmlPacketAddReflRot(const NmlMaterialRenderState *material,
                         const NmlModelRenderState *model)
{
    float matrix[4][4];
    int type = 1;

    if ((material->material_flags & 0x2000u) != 0)
        type = 2;

    s_pPacket = xglPacketGetCurrent();

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

    sceVif1PkCnt(s_pPacket, 0);
    sceVif1PkOpenUpkCode(s_pPacket, 1004, 108, 1, 1);
    sceVif1PkAddUpkData128N(s_pPacket, matrix, 3);
    sceVif1PkCloseUpkCode(s_pPacket);
    s_nReflRotType = type;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddScreenClear);

extern int g_nGsEntry;

void nmlPacketGsInit(void)
{
    g_nGsEntry = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsClamp);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsPixeltest);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsPixeltest1);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsZbuf);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsZbuf1);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsTexture);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsAlpha);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsAlpha1);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsScissor);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsScissor1);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFBA);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFBA1);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFogCol);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFrame);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFrame1);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsPAbe);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsPrmode);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsPrmodecont);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", packet_gs_entry32);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", packet_gs_entry64);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGsFba);
