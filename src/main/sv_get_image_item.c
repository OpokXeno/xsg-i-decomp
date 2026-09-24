#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

/*
 * One 0x24-byte entry of the working image list svImageListCreate clears
 * and svGetImageItem indexes; only its address is ever taken in this
 * allocation, so its fields stay unmodeled.
 */
typedef struct SvImageItem {
    unsigned char unmodeled_00[0x24];
} SvImageItem;

/*
 * The single working image list: a 0x1C-byte header (untouched by this
 * allocation) followed by 0x100 SvImageItem entries (0x1C + 0x100*0x24 =
 * 0x241C, svImageListCreate's own clear size).
 */
typedef struct SvImageList {
    unsigned char unmodeled_header[0x1C];
    SvImageItem items[0x100];
} SvImageList;

extern SvImageList _imageList;

void *svGetImageItem(int index) {
    return &_imageList.items[index];
}

void svImageListCreate(void) {
    memset(&_imageList, 0, sizeof(SvImageList));
}

/*
 * One 0x24-byte entry of an image mapper's item table: only the leading
 * dword svImageListDestroy clears is modeled.
 */
typedef struct SvImageMapperItem {
    int used; /* 0x00, cleared when non-zero */
    unsigned char unmodeled_04[0x24 - 4];
} SvImageMapperItem;

/*
 * SvTypeListFields and SvTypeList (below, next to svDeleteImageMapper) name
 * the same physical 0x241C-byte per-type record svGetTypeList indexes; they
 * are not two different objects. svDeleteImageMapper's own text completes
 * the SvTypeList tag with only its one evidenced field (entryCount, +0x14)
 * and is published, unallocated prelude here, so it is restated byte-
 * identical and never grown (AGENTS.md, "Source and acceptance": a
 * published struct cannot grow). svImageListDestroy and svAddImageMapper
 * (this allocation) sit earlier in the file and evidence more of the same
 * record than that completion models, so they read/write it through this
 * second tag naming only their own fields, at the identical offsets:
 * `total` (+0x00, unconditionally cleared), `size` (+0x04, read and
 * archived into `savedSize` at +0x0C before the clear), `width`/`height`
 * (+0x10/+0x12, the two fields svAddImageMapper stores its own second and
 * fourth arguments into, both cleared here) and `entryCount` (+0x14, the
 * same field and offset SvTypeList already names). Bytes 0x08-0x0B stay an
 * explicit unmodeled span; nothing in this allocation touches them.
 */
typedef struct SvTypeList SvTypeList;

typedef struct SvTypeListFields {
    int total;         /* 0x00 */
    int size;           /* 0x04 */
    unsigned char unmodeled_08[4];
    int savedSize;       /* 0x0C */
    short width;          /* 0x10 */
    short height;         /* 0x12 */
    short entryCount;     /* 0x14 */
} SvTypeListFields;

void svImageListDestroy(SvTypeList *typeList)
{
  SvTypeListFields *header = (SvTypeListFields *) typeList;
  SvImageMapperItem *item = (SvImageMapperItem *) (((unsigned char *) typeList) + 0x24);
  int count = 0xFF;

  do
  {
    if (item->used != 0)
    {
      item->used = 0;
    }
    count -= 1;
    item += 1;
  }
  while (count >= 0);

  header->savedSize = header->size;
  header->total = 0;
  header->width = 0;
  header->height = 0;
  header->entryCount = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svImageListAlloc);

extern unsigned char _imageMapper[];

static SvTypeList * svGetTypeList(int type) {
    return (SvTypeList *) (_imageMapper + (type * 0x241C));
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetImageListItemSub);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetImageListItem);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetResFromName);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetPrmFromName);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svLoadTexture);

/*
 * Only `data`, `destAddress` and `format` are evidenced (svLoadClut's three
 * field reads); the bytes between `data` and `destAddress` are an explicit
 * unmodeled span rather than an invented field.
 */
typedef struct ClutUploadRequest {
    void *data;                     /* 0x00, CLUT pixel data source address */
    unsigned char unmodeled_04[0x0E];
    unsigned short destAddress;     /* 0x12, GS local CLUT destination */
    unsigned short format;          /* 0x14, 8 => 256-entry CLUT, else 16-entry */
} ClutUploadRequest;

/*
 * Parameter roles as the SDK body (main 0x0020b208) packs them: GS
 * BITBLTBUF DBP/DPSM/DBW from basePointer/pixelFormat/bufferWidth, TRXPOS
 * DSAX/DSAY from x/y, TRXREG RRW/RRH from width/height, then `qwc`
 * quadwords of `image` sent by reference.
 */
extern void sceVif1PkRefLoadImage(XglPacket *packet,
                                  unsigned short basePointer,
                                  unsigned char pixelFormat,
                                  unsigned short bufferWidth,
                                  const void *image, unsigned int qwc,
                                  unsigned int x, unsigned int y,
                                  unsigned int width, unsigned int height);

void svLoadClut(ClutUploadRequest *clut)
{
    XglPacket *packet = xglPacketGetCurrent();
    unsigned short dest = clut->destAddress;

    if (clut->format == 8) {
        sceVif1PkRefLoadImage(packet, dest, 0, 1, clut->data, 0x40, 0, 0,
                              0x10, 0x10);
    } else {
        sceVif1PkRefLoadImage(packet, dest, 0, 1, clut->data, 4, 0, 0, 8, 2);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svLoadImageList);

s16 svLoadImageList(void *);

s16 svLoadMapperList(void)
{
  s16 ret;
  s16 loadResult;
  int reverseIndex;
  int typeOffset;

  typeOffset = 0;
  reverseIndex = 0x27;
  do
  {
    ret = *((s16 *) (((unsigned char *) (((s16 *) ((s16 *) (((int) ((void *) _imageMapper)) + 0x14))) + ((reverseIndex - 39) * -4622))) + 0));
    if (ret > 0)
    {
      loadResult = svLoadImageList((void *) (typeOffset + ((int) ((void *) _imageMapper))));
      ret = loadResult;
    }
    typeOffset += 0x241C;
    reverseIndex -= 1;
  }
  while (reverseIndex >= 0);
  return ret;
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetSizeBit);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddImage);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddClut);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddModel);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAddScript);

int svImageListAlloc();

/*
 * One svAddScript2 sub-record, at +0x1C of a 0x24-byte slot of the type
 * list's item array. Only the fields this function itself writes are
 * modeled, at their byte offset from this sub-record's own base: `codePtr`
 * (+0x00, the script's own +0xC0 address), `active` (+0x08, always 1),
 * `kind` (+0x0C, always 5, matching this function's own "script" name),
 * `id` (+0x0E, the script's own +0xA halfword), two cleared halfwords
 * (+0x18/+0x1A) and `dataPtr` (+0x20, the script's own +0x14 address).
 * +0x20 (slot +0x3C) lands past the slot's own 0x24-byte stride, into the
 * next slot's leading bytes -- evidenced by the original bytes, not
 * treated as a bug.
 */
typedef struct SvScriptSlot {
    void *codePtr;                  /* 0x00 */
    unsigned char unmodeled_04[4];
    int active;                      /* 0x08 */
    short kind;                       /* 0x0C */
    short id;                          /* 0x0E */
    unsigned char unmodeled_10[8];
    short clearedA;                     /* 0x18 */
    short clearedB;                      /* 0x1A */
    unsigned char unmodeled_1C[4];
    void *dataPtr;                        /* 0x20 */
} SvScriptSlot;

static int svAddScript2(int typeListAddr, void *script)
{
  int index;
  void *codePtr;
  void *dataPtr;
  void *slot;
  SvScriptSlot *sub;

  index = svImageListAlloc(typeListAddr);
  codePtr = (unsigned char *) script + 0xC0;
  dataPtr = (unsigned char *) script + 0x14;
  slot = (void *) (typeListAddr + (index * 0x24));
  if (index < 0)
  {
    return -1;
  }
  sub = (SvScriptSlot *) ((unsigned char *) slot + 0x1C);
  sub->kind = 5;
  sub->dataPtr = dataPtr;
  sub->codePtr = codePtr;
  sub->id = (short) *((u16 *) (((unsigned char *) script) + 0xA));
  sub->active = 1;
  sub->clearedB = 0;
  sub->clearedA = 0;
  return index;
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svAnalyzeChunk);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svInitImageMapper);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svInitRefImage);

int svAnalyzeChunk(SvTypeList *typeList, void *chunk);

void svAddImageMapper(int type, int width, void *chunk, int height) {
    SvTypeList *typeList;
    SvTypeListFields *header;

    typeList = svGetTypeList(type);
    header = (SvTypeListFields *) typeList;
    if (header->entryCount != 0) {
        svImageListDestroy(typeList);
    }
    header->height = (short) height;
    header->width = (short) width;
    header->entryCount = (short) ((u16) header->entryCount + 1);
    svAnalyzeChunk(typeList, chunk);
}

/*
 * One image mapper's list: svGetTypeList (LOCAL in the original symbol
 * table) returns entry `type` of _imageMapper, 0x241c bytes apart. Only the
 * entry count is modeled: svDeleteImageMapper tests it (lh +0x14) and
 * svImageListDestroy clears it; the rest of the record stays an explicit
 * unmodeled span.
 */
typedef struct SvTypeList {
    unsigned char unmodeled_00[0x14];
    short entryCount; /* 0x14 */
} SvTypeList;

static SvTypeList *svGetTypeList(int type);
void svImageListDestroy(SvTypeList *typeList);

void svDeleteImageMapper(int type)
{
    SvTypeList *typeList = svGetTypeList(type);

    if (typeList->entryCount != 0) {
        svImageListDestroy(typeList);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDeleteImageMapperID);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDeleteImageMapperData);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svFileLoadScript);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svGetScript);

/*
 * SGsSendPacket's own address precedes SGsPacket's definition (below, before
 * SGsInitGifPacket, outside this allocation): the same four fields, at the
 * same evidenced offsets that definition documents, restated here under a
 * local tag so this allocation adds text only next to its own function.
 */
typedef struct SGsPacketReset {
    int count;               /* 0x0 */
    int tagIndex;             /* 0x4 */
    int loopCount;             /* 0x8 */
    void *base;               /* 0xC, starts at the scratchpad (0x70000000) */
} SGsPacketReset;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddCode(XglPacket *packet, unsigned int code);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data,
                                    int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void sceVif1PkTerminate(XglPacket *packet);

void SGsSendPacket(SGsPacketReset *packet)
{
    XglPacket *vifPacket;

    if (packet->count != 0) {
        vifPacket = xglPacketGetCurrent();
        sceVif1PkCnt(vifPacket, 0);
        sceVif1PkAddCode(vifPacket, 0x11000000);
        sceVif1PkOpenDirectHLCode(vifPacket, 0);
        sceVif1PkAddDirectDataN(vifPacket, packet->base, packet->count);
        sceVif1PkCloseDirectHLCode(vifPacket);
        sceVif1PkTerminate(vifPacket);
        packet->count = 0;
        packet->tagIndex = 0;
        packet->loopCount = 1;
        packet->base = (void *)0x70000000;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsTexFlush);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsInitEnv);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsSetZTestEnv);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsRestoreEnv);

static void sefInitClipViewVolume(void *view) {
    __asm__ __volatile__("lqc2 vf12, 0(%0)\n\tlqc2 vf13, 16(%0)\n\tlqc2 vf14, 32(%0)\n\tlqc2 vf15, 48(%0)\n\tlqc2 vf16, 64(%0)\n\tlqc2 vf17, 80(%0)\n\tlqc2 vf18, 96(%0)\n\tlqc2 vf19, 112(%0)\n\tnop" :  : "r"(view) : "memory");
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", sefClipViewVolumeA);

#define SEF_CLIP_VIEW_VOLUME_OFFSET 0x4F0

/*
 * sefDrawSchedulerEffect (still INCLUDE_ASM here) calls sefClipViewVolume and
 * branches on the returned v0, and sefClipViewVolume itself ends in a tail
 * jump to sefClipViewVolumeA (no jal, so its result is whatever
 * sefClipViewVolumeA returns): both are declared non-void here.
 */
static int sefClipViewVolumeA(void *scheduler);
static void sefInitClipViewVolume(void *view);

int sefClipViewVolume(void *scheduler, void *effect)
{
    sefInitClipViewVolume((unsigned char *)effect + SEF_CLIP_VIEW_VOLUME_OFFSET);
    return sefClipViewVolumeA(scheduler);
}

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

static void sefDrawSchedulerEffect(int schedulerIndex);
extern void SGsSetZTestEnv(int enable);
extern int srsAnalyzeEftNo(short effectId, unsigned char *charId,
                            int *effectCategory);
extern unsigned char charID_3;
extern int eftCate_4;
extern int eft_5;

/*
 * One 0xab0-byte record of the scheduler table sefGetScheduler returns
 * (src/main/sef.h, "The original scheduler table..."). Only the three
 * fields this function reads are named; sefIsDeadSchduler (sef.c) reads the
 * same in-use word raw through its own TU's SchedulerState, which this TU
 * cannot include (TU-local elsewhere), so this is a separate, narrower view
 * of the same bytes.
 */
typedef struct SchedulerRecord {
    unsigned char unmodeled_000[0x6b0];
    int inUse;                            /* +0x6b0 */
    unsigned char unmodeled_6b4[0xa78 - 0x6b4];
    short effectId;                       /* +0xa78 */
    unsigned char unmodeled_a7a[0xa8c - 0xa7a];
    int flags;                            /* +0xa8c */
    unsigned char unmodeled_a90[0xab0 - 0xa90];
} SchedulerRecord;

/* sef.c (main/tu211) defines this returning unsigned char *; declare it with
 * that exact type and cast at the one call site so a later cross-TU
 * publication of the real prototype cannot conflict with this one. */
extern unsigned char *sefGetScheduler(void);

#define SCHEDULER_COUNT 0x80

static void svDrawSchedulerBlk(int eftCate, int eftNo)
{
    short effectId;
    int schedulerIndex;
    SchedulerRecord *record;
    SchedulerRecord *scheduler;

    scheduler = (SchedulerRecord *)sefGetScheduler();
    schedulerIndex = 0;
    record = scheduler;
    do {
        if (record->inUse != 0) {
            effectId = record->effectId;
            if (eft_5 != effectId) {
                eft_5 = effectId;
                srsAnalyzeEftNo(effectId, &charID_3, &eftCate_4);
            }
            if (eftCate_4 == eftCate &&
                (eftNo == 0 || (eftNo > 0 && eftNo == eft_5))) {
                SGsSetZTestEnv(((record->flags >> 2) ^ 1) & 1);
                sefDrawSchedulerEffect(schedulerIndex);
            }
        }
        schedulerIndex++;
        record++;
    } while (schedulerIndex < SCHEDULER_COUNT);
}

extern int _draw3D;

/* Draws the alters whose group (+0x0E) equals layer. */
void sdvDrawAlters(int layer);

static void svDrawAlters(int layer)
{
    if (_draw3D == 0) {
        sdvDrawAlters(layer);
    }
}

extern int _scMslCate;

void MEfObjExec2nd(void);

static void svDrawMissile(int category)
{
    if (_draw3D == 0) {
        if (_scMslCate == category) {
            MEfObjExec2nd();
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawSchedulerParticle);

extern int _nEffect2D;
extern int _nowImage;

static void svDrawSchedulerParticle(void);

void svDrawScheduler2D(void)
{
    if (_nEffect2D != 0) {
        _nowImage = 0;
        _draw3D = 1;
        svDrawSchedulerParticle();
    }
}

void svDrawScheduler(void)
{
    _draw3D = 0;
    _nEffect2D = 0;
    _nowImage = 0;
    svDrawSchedulerParticle();
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", svDrawScheduler3D);

/*
 * The SGs* family (SGsInitGifPacket, SGsOpenGifPacket, SGsCloseGifPacket,
 * SGsAddReg, SGsAddGifXYZ2, SGsSendPacket and others in this TU) shares one
 * accumulator: a growing array of 16-byte GIF records at `base`, `count`
 * records long. SGsOpenGifPacket/SGsCloseGifPacket/SGsAddReg (still
 * INCLUDE_ASM here, read for this evidence per docs/naming.md) show that
 * `tagIndex` is the record index SGsOpenGifPacket saves so
 * SGsCloseGifPacket can patch the GIFtag it opened, and `loopCount` is
 * OR-ed, unshifted, into that tag's low (NLOOP) bits by SGsCloseGifPacket;
 * both functions here leave it at 1.
 */

/*
 * One 128-bit GIF packet record. The SGs* writers fill it as a whole
 * quadword (sq), as two doublewords (sd at 0x0/0x8: a register value and
 * its A+D address) or as four words (sw, SGsAddGifXYZ2).
 */
typedef union SGsGifRecord {
    unsigned long long dword[2];
    int word[4];
} SGsGifRecord;

typedef struct SGsPacket {
    int count;              /* 0x0 */
    int tagIndex;           /* 0x4 */
    int loopCount;          /* 0x8 */
    SGsGifRecord *base;     /* 0xC, starts at the scratchpad (0x70000000) */
} SGsPacket;

void SGsInitGifPacket(SGsPacket *packet)
{
    packet->loopCount = 1;
    packet->base = (SGsGifRecord *)0x70000000;
    packet->tagIndex = 0;
    packet->count = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsOpenGifPacket);

void SGsCloseGifPacket(SGsPacket *packet) {
    packet->base[packet->tagIndex].dword[0] |= packet->loopCount;
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddReg);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifRGBA);

/*
 * GS XYZF2 vertex: X/Y are (value*16 + screen offset), Z and F are the
 * raw depth and fog values -- the PS2 SDK's own XYZ2/XYZF2 naming.
 */
void SGsAddGifXYZ2(SGsPacket *packet, int x, int y, int z, int fog)
{
    packet->base[packet->count].word[0] = x * 0x10 + 0x7000;
    packet->base[packet->count].word[1] = y * 0x10 + 0x7200;
    packet->base[packet->count].word[2] = z;
    packet->base[packet->count].word[3] = fog;
    packet->count++;
}

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifData);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifSTQ);

INCLUDE_ASM("asm/main/nonmatchings/sv_get_image_item", SGsAddGifUV);
