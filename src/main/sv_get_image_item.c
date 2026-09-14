#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetImageItem);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svImageListCreate);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svImageListDestroy);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svImageListAlloc);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetTypeList);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetImageListItemSub);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetImageListItem);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetResFromName);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetPrmFromName);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svLoadTexture);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svLoadClut);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svLoadImageList);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svLoadMapperList);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetSizeBit);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddImage);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddClut);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddModel);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddScript);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddScript2);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAnalyzeChunk);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svInitImageMapper);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svInitRefImage);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddImageMapper);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDeleteImageMapper);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDeleteImageMapperID);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDeleteImageMapperData);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svFileLoadScript);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetScript);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsSendPacket);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsTexFlush);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsInitEnv);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsSetZTestEnv);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsRestoreEnv);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefInitClipViewVolume);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefClipViewVolumeA);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefClipViewVolume);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefDrawParticle2D);

/*
 * svRotMatrixScale: for each of the four source rows (effect-record offsets
 * +0x00/+0x10/+0x20/+0x30) forms row.x*vf13 + row.y*vf14 + row.z*vf15 through
 * the VU0 accumulator (vf13/vf14/vf15 are caller-established persistent VU0
 * columns, read only -- docs/ee-reference/vu.md, "Persistent VF state and
 * call boundaries"), adds row.w*vf8 (vf8 is the source record's contribution
 * vector at +0xC0), scales the result row i by scale-vector lane i (the
 * source record's scale vector at +0x100, one lane per row across every
 * output lane), and stores the four transformed rows into dst. All six
 * source loads precede all four destination stores, so an aliased
 * destination cannot clobber a not-yet-loaded source row. The source
 * effect record's full layout beyond these three evidenced sub-vectors is
 * not yet recovered, so it is addressed here by byte offset instead of an
 * invented, padded struct (AGENTS.md, "Source and acceptance").
 *
 * The block ends its last COP2 store with an explicit trailing `nop`: cc1's
 * bare `j $31` epilogue (a leaf function, no frame) leaves its delay slot to
 * the assembler, which in reorder mode fills it by moving the preceding
 * `sqc2`. The original leaves that slot an unfilled `jr $31; nop`, so the
 * block ends with a `nop` for the assembler to move instead, restoring the
 * original instruction order and extent
 */
static void svRotMatrixScale(void *dst, const void *src)
{
    const char *base = (const char *)src;
    const char *contrib = base + 0xC0;
    const char *scale = base + 0x100;

    __asm__ __volatile__(
        "lqc2 vf8, 0(%2)\n\t"
        "lqc2 vf1, 0(%1)\n\t"
        "lqc2 vf2, 16(%1)\n\t"
        "lqc2 vf3, 32(%1)\n\t"
        "lqc2 vf4, 48(%1)\n\t"
        "lqc2 vf5, 0(%3)\n\t"
        "vmulax.xyzw ACCxyzw, vf13xyzw, vf1x\n\t"
        "vmadday.xyzw ACCxyzw, vf14xyzw, vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf15xyzw, vf1z\n\t"
        "vmaddw.xyzw vf9xyzw, vf8xyzw, vf1w\n\t"
        "vmulax.xyzw ACCxyzw, vf13xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf14xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf15xyzw, vf2z\n\t"
        "vmaddw.xyzw vf10xyzw, vf8xyzw, vf2w\n\t"
        "vmulax.xyzw ACCxyzw, vf13xyzw, vf3x\n\t"
        "vmadday.xyzw ACCxyzw, vf14xyzw, vf3y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf15xyzw, vf3z\n\t"
        "vmaddw.xyzw vf11xyzw, vf8xyzw, vf3w\n\t"
        "vmulax.xyzw ACCxyzw, vf13xyzw, vf4x\n\t"
        "vmadday.xyzw ACCxyzw, vf14xyzw, vf4y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf15xyzw, vf4z\n\t"
        "vmaddw.xyzw vf12xyzw, vf8xyzw, vf4w\n\t"
        "vmulx.xyzw vf9xyzw, vf9xyzw, vf5x\n\t"
        "vmuly.xyzw vf10xyzw, vf10xyzw, vf5y\n\t"
        "vmulz.xyzw vf11xyzw, vf11xyzw, vf5z\n\t"
        "vmulw.xyzw vf12xyzw, vf12xyzw, vf5w\n\t"
        "sqc2 vf9, 0(%0)\n\t"
        "sqc2 vf10, 16(%0)\n\t"
        "sqc2 vf11, 32(%0)\n\t"
        "sqc2 vf12, 48(%0)\n\t"
        "nop"
        : : "r"(dst), "r"(base), "r"(contrib), "r"(scale) : "memory");
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawModel);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefDrawParticle2);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefDrawParticle);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefDrawParticleList);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefDrawParticleListCf);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefDrawSchedulerEffect);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawSchedulerBlk);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawAlters);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawMissile);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawSchedulerParticle);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawScheduler2D);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawScheduler3D);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsInitGifPacket);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsOpenGifPacket);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsCloseGifPacket);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddReg);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifRGBA);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifXYZ2);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifData);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifSTQ);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifUV);
