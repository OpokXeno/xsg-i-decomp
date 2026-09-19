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

/*
 * Every case ends with its own "queue the program or clear result" tail.  The
 * compiler cross-jumps the seven copies back into the single tail seen in the
 * original, but before that each copy is a reference to result, which is what
 * keeps result in a callee-saved register (s4).  Written once after the switch result is spilled to
 * the stack and the program pointers take s4..s8 instead.
 */

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurMatrixSet);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurMatrixGet);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurMatrixMul);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurApplyMatrix_002364E0);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurMatrixMul33norm);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurSetViewScaleTrans_002365C0);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurRotTransPersClip_002365D0);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", _CurSetMatrix_00236640);

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

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddGifTagStandard);

INCLUDE_ASM("asm/main/nonmatchings/nml_packet_add", nmlPacketAddFog);

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
