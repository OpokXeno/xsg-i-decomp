#include "common.h"

#include "shared.h"

void nmlModelSetFilterGunosys(int *model, int flags, int count);

void nmlModelSetFilterStealth(int *model, int flags);

struct NmlFilterRenderView {
    u8 unmodeled_00[2];
    short pixelStorageMode;
    u8 unmodeled_04[0x10];
    u16 framebufferPage;
    u8 unmodeled_16[0x0a];
    u16 displayBufferBase;
};

extern struct NmlFilterRenderView sRender;

struct NmlFilterGsRegisterEntry {
    u64 value;
    u64 registerId;
};

extern void nmlPacketGsInit(void);

extern void packet_gs_entry64(int registerId, void *entry);

extern void nmlPacketAddGsFlush(void);

extern void nmlPacketAddGsFrame(int frame, int offset);

extern void nmlPacketAddGsFBA(int value);

extern void nmlPacketAddGsZbuf(int zBuffer);

static void _CurSetMatrix(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf27,0(%0)\n\t"
        "lqc2 $vf28,16(%0)\n\t"
        "lqc2 $vf29,32(%0)\n\t"
        "lqc2 $vf30,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _CurSetViewScaleTrans(const float viewScale[4], const float viewTrans[4])
{
    __asm__ __volatile__(
        "lqc2 $vf25,0(%0)\n\t"
        "lqc2 $vf26,0(%1)\n\t"
        "nop"
        :
        : "r"(viewScale), "r"(viewTrans)
        : "memory"
    );
}

static int _CurRotTransPersClip(Vector4 *destination, const Vector4 *vector)
{
    register int clipFlags asm("$2");
    /* The SQC2 destination is an EE32 address in GPR4. After that
     * hardware use ends, the same word carries the masked clip flags. */
    register unsigned int destinationOrFlags asm("$4") = (unsigned int)destination;

    __asm__ __volatile__(
        "ctc2 $0,$vi18\n\t"
        "lqc2 $vf31,0(%2)\n\t"
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
        "vmulaw.xyzw ACC,vf26xyzw,vf0w\n\t"
        "vmadd.xyzw vf31xyzw,vf31xyzw,vf25xyzw\n\t"
        "vftoi4.xyw vf23xyw,vf31xyw\n\t"
        "vftoi0.z vf23z,vf31z\n\t"
        "vsub.w vf23w,vf23w,vf23w\n\t"
        "sqc2 $vf23,0(%1)\n\t"
        "cfc2 %0,$vi18\n\t"
        "nop"
        : "=r"(clipFlags)
        : "r"(destinationOrFlags), "r"(vector)
        : "memory"
    );
    clipFlags &= 0x3f;
    destinationOrFlags = clipFlags;
    __asm__ __volatile__("" : : "r"(destinationOrFlags), "r"(clipFlags));
    return destinationOrFlags;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlModelSetFilterGunosys);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlModelSetFilterStealth);

static void _WeightToGlobalPlaceInit(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf10,0(%0)\n\t"
        "lqc2 $vf11,16(%0)\n\t"
        "lqc2 $vf12,32(%0)\n\t"
        "lqc2 $vf13,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", _WeightToGlobalPlaceVec);

static float _VectorLengthSQ(const Vector4 *first, const Vector4 *second)
{
    register float lengthSq asm("$f0");

    __asm__ __volatile__(
        "lqc2 $vf10,0(%1)\n\t"
        "lqc2 $vf11,0(%2)\n\t"
        "vsub.xyz vf10xyz,vf11xyz,vf10xyz\n\t"
        "vmul.xyz vf10xyz,vf10xyz,vf10xyz\n\t"
        "vaddz.x vf10x,vf10x,vf10z\n\t"
        "vaddy.x vf10x,vf10x,vf10y\n\t"
        "qmfc2 $2,$vf10\n\t"
        "mtc1 $2,$f2\n\t"
        "mtc1 $2,%0"
        : "=f"(lengthSq)
        : "r"(first), "r"(second)
        : "$2", "memory"
    );
    return lengthSq;
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterStealthMake);

void nmlFilterSetPacket(int *model, int count)
{
    int flags;
    int i;

    flags = model[0x234 / 4];
    for (i = 0; i < 31; i++) {
        switch (flags & (1 << i)) {
        case 1:
            flags &= ~1;
            nmlModelSetFilterGunosys(model, flags, count);
            break;
        case 2:
            flags &= ~2;
            nmlModelSetFilterStealth(model, flags);
            break;
        }
    }
}

void nmlFilterSetFrameToBuffer(int displayBufferBase)
{
    struct NmlFilterGsRegisterEntry *gsEntry3f;
    struct NmlFilterGsRegisterEntry *gsEntry50;
    struct NmlFilterGsRegisterEntry *gsEntry51;
    struct NmlFilterGsRegisterEntry *gsEntry52;
    struct NmlFilterGsRegisterEntry *gsEntry53;
    struct NmlFilterGsRegisterEntry *entry;
    int remaining;
    u64 bitbltBuffer;
    u64 sourceBuffer;
    long long pixelStorageMode;

    remaining = 4;
    pixelStorageMode = sRender.pixelStorageMode;
    sourceBuffer = ((u64)sRender.framebufferPage << 37) | ((u64)0x8000 << 36);
    bitbltBuffer = 0x80000 | ((displayBufferBase & 0xffff) << 5) |
        (pixelStorageMode << 56) | sourceBuffer | (pixelStorageMode << 24);
    gsEntry50 = (struct NmlFilterGsRegisterEntry *)0x70000010;
    gsEntry51 = (struct NmlFilterGsRegisterEntry *)0x70000020;
    gsEntry52 = (struct NmlFilterGsRegisterEntry *)0x70000030;
    gsEntry53 = (struct NmlFilterGsRegisterEntry *)0x70000040;
    gsEntry3f = (struct NmlFilterGsRegisterEntry *)0x70000000;
    gsEntry3f->value = 0;
    gsEntry3f->registerId = 0x3f;
    gsEntry50->value = bitbltBuffer;
    gsEntry50->registerId = 0x50;
    gsEntry51->value = 0;
    gsEntry51->registerId = 0x51;
    gsEntry52->value = ((u64)0x1c0 << 32) | 0x200;
    gsEntry52->registerId = 0x52;
    gsEntry53->value = 2;
    gsEntry53->registerId = 0x53;
    entry = (struct NmlFilterGsRegisterEntry *)0x70000000;
    nmlPacketGsInit();
    for (; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64((int)entry->registerId, entry);
    }
    nmlPacketAddGsFlush();
}

void nmlFilterSetBufferToFrame(int frameBufferBase)
{
    struct NmlFilterGsRegisterEntry *gsEntry3f;
    struct NmlFilterGsRegisterEntry *gsEntry50;
    struct NmlFilterGsRegisterEntry *gsEntry51;
    struct NmlFilterGsRegisterEntry *gsEntry52;
    struct NmlFilterGsRegisterEntry *gsEntry53;
    struct NmlFilterGsRegisterEntry *entry;
    int remaining;
    u64 bitbltBuffer;
    u64 sourceBuffer;
    long long pixelStorageMode;

    remaining = 4;
    pixelStorageMode = sRender.pixelStorageMode;
    sourceBuffer = ((u64)(frameBufferBase & 0xffff) << 37) | ((u64)0x8000 << 36);
    bitbltBuffer = 0x80000 | (sRender.framebufferPage << 5) |
        (pixelStorageMode << 56) | sourceBuffer | (pixelStorageMode << 24);
    gsEntry50 = (struct NmlFilterGsRegisterEntry *)0x70000010;
    gsEntry51 = (struct NmlFilterGsRegisterEntry *)0x70000020;
    gsEntry52 = (struct NmlFilterGsRegisterEntry *)0x70000030;
    gsEntry53 = (struct NmlFilterGsRegisterEntry *)0x70000040;
    gsEntry3f = (struct NmlFilterGsRegisterEntry *)0x70000000;
    gsEntry3f->value = 0;
    gsEntry3f->registerId = 0x3f;
    gsEntry50->value = bitbltBuffer;
    gsEntry50->registerId = 0x50;
    gsEntry51->value = 0;
    gsEntry51->registerId = 0x51;
    gsEntry52->value = ((u64)0x1c0 << 32) | 0x200;
    gsEntry52->registerId = 0x52;
    gsEntry53->value = 2;
    gsEntry53->registerId = 0x53;
    entry = (struct NmlFilterGsRegisterEntry *)0x70000000;
    nmlPacketGsInit();
    for (; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64((int)entry->registerId, entry);
    }
    nmlPacketAddGsFlush();
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetBufferRender);

void nmlFilterSetFrameAlphaClear(void)
{
    struct NmlFilterGsRegisterEntry *gsEntry42;
    struct NmlFilterGsRegisterEntry *gsEntry47;
    struct NmlFilterGsRegisterEntry *gsEntry1;
    struct NmlFilterGsRegisterEntry *gsEntry46;
    struct NmlFilterGsRegisterEntry *gsEntry4First;
    struct NmlFilterGsRegisterEntry *gsEntry4Last;
    struct NmlFilterGsRegisterEntry *entry;
    int remaining;

    gsEntry42 = (struct NmlFilterGsRegisterEntry *)0x70000000;
    gsEntry47 = (struct NmlFilterGsRegisterEntry *)0x70000010;
    gsEntry1 = (struct NmlFilterGsRegisterEntry *)0x70000020;
    gsEntry46 = (struct NmlFilterGsRegisterEntry *)0x70000030;
    gsEntry4First = (struct NmlFilterGsRegisterEntry *)0x70000040;
    gsEntry4Last = (struct NmlFilterGsRegisterEntry *)0x70000050;
    gsEntry42->value = 0x68;
    gsEntry42->registerId = 0x42;
    gsEntry47->value = 0x31001;
    gsEntry47->registerId = 0x47;
    gsEntry1->value = 0x3f80000001000000;
    gsEntry1->registerId = 1;
    gsEntry46->value = 0x46;
    gsEntry46->registerId = 0;
    gsEntry4First->value = 0x72007000;
    gsEntry4First->registerId = 4;
    gsEntry4Last->value = 0x8e009000;
    gsEntry4Last->registerId = 4;
    entry = (struct NmlFilterGsRegisterEntry *)0x70000000;

    nmlPacketGsInit();
    nmlPacketAddGsFrame(sRender.displayBufferBase, 0);
    nmlPacketAddGsFBA(0);
    for (remaining = 5; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64((int)entry->registerId, entry);
    }
    nmlPacketAddGsFlush();
}

void nmlFilterSetTexClear(void)
{
    struct NmlFilterGsRegisterEntry *gsEntry42;
    struct NmlFilterGsRegisterEntry *gsEntry47;
    struct NmlFilterGsRegisterEntry *gsEntry1;
    struct NmlFilterGsRegisterEntry *gsEntry46;
    struct NmlFilterGsRegisterEntry *gsEntry4First;
    struct NmlFilterGsRegisterEntry *gsEntry4Last;
    struct NmlFilterGsRegisterEntry *entry;
    int remaining;

    gsEntry42 = (struct NmlFilterGsRegisterEntry *)0x70000000;
    gsEntry47 = (struct NmlFilterGsRegisterEntry *)0x70000010;
    gsEntry1 = (struct NmlFilterGsRegisterEntry *)0x70000020;
    gsEntry46 = (struct NmlFilterGsRegisterEntry *)0x70000030;
    gsEntry4First = (struct NmlFilterGsRegisterEntry *)0x70000040;
    gsEntry4Last = (struct NmlFilterGsRegisterEntry *)0x70000050;
    gsEntry42->value = 0x8a;
    gsEntry42->registerId = 0x42;
    gsEntry47->value = 0x31001;
    gsEntry47->registerId = 0x47;
    gsEntry1->value = 0x3f80000080000000;
    gsEntry1->registerId = 1;
    gsEntry46->value = 0x46;
    gsEntry46->registerId = 0;
    gsEntry4First->value = 0x72007000;
    gsEntry4First->registerId = 4;
    gsEntry4Last->value = 0x7ff09000;
    gsEntry4Last->registerId = 4;
    entry = (struct NmlFilterGsRegisterEntry *)0x70000000;

    nmlPacketGsInit();
    for (remaining = 5; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64((int)entry->registerId, entry);
    }
    nmlPacketAddGsFlush();
}

void nmlFilterBackClear(void)
{
    struct NmlFilterGsRegisterEntry *gsEntry47;
    struct NmlFilterGsRegisterEntry *gsEntry1;
    struct NmlFilterGsRegisterEntry *gsEntry0;
    struct NmlFilterGsRegisterEntry *gsEntry4First;
    struct NmlFilterGsRegisterEntry *gsEntry4Last;
    struct NmlFilterGsRegisterEntry *entry;
    int remaining;

    gsEntry47 = (struct NmlFilterGsRegisterEntry *)0x70000000;
    gsEntry1 = (struct NmlFilterGsRegisterEntry *)0x70000010;
    gsEntry0 = (struct NmlFilterGsRegisterEntry *)0x70000020;
    gsEntry4First = (struct NmlFilterGsRegisterEntry *)0x70000030;
    gsEntry4Last = (struct NmlFilterGsRegisterEntry *)0x70000040;
    gsEntry47->value = 0x30000;
    gsEntry47->registerId = 0x47;
    gsEntry1->value = (u64)0xfe00 << 46;
    gsEntry1->registerId = 1;
    gsEntry0->value = 6;
    gsEntry0->registerId = 0;
    gsEntry4First->value = 0x72007000;
    gsEntry4First->registerId = 4;
    gsEntry4Last->value = 0x8e009000;
    gsEntry4Last->registerId = 4;
    entry = (struct NmlFilterGsRegisterEntry *)0x70000000;

    nmlPacketGsInit();
    nmlPacketAddGsZbuf(1);
    nmlPacketAddGsFrame(sRender.framebufferPage, 0);
    for (remaining = 4; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64((int)entry->registerId, entry);
    }
    nmlPacketAddGsFlush();
}

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetFlatRender);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetTexFillDraw);

INCLUDE_ASM("asm/main/nonmatchings/nml_filter_set", nmlFilterSetVolumeCubeRender);
