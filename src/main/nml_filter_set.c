#include "common.h"

#include "shared.h"

#include "nml_filter_set.h"

static void nmlModelSetFilterGunosys(const LayoutStore *model, int flags, int count);

static void nmlModelSetFilterStealth(const LayoutStore *model, int flags);

struct NmlFilterRenderView {
    u8 unmodeled_00[2];
    short pixelStorageMode;
    short frameWidth;
    short frameHeight;
    u8 unmodeled_08[0x0a];
    u16 opaqueTexturePage;
    u16 framebufferPage;
    u8 unmodeled_16[0x0a];
    u16 displayBufferBase;
};

extern struct NmlFilterRenderView sRender;

struct NmlFilterGsRegisterEntry {
    u64 value;
    u64 registerId;
};

/* One scratchpad write contains two of the owner's actual GS slots. */

extern void nmlPacketGsInit(void);

/* Defining-owner interface; normal header transport remains an integration need. */
extern void packet_gs_entry64(u32 registerId, const u64 *value);

extern void nmlPacketAddGsFlush(void);

extern void nmlPacketAddGsFrame(int frame, int offset);

extern void nmlPacketAddGsFBA(int value);

extern void nmlPacketAddGsZbuf(int zBuffer);

extern void nmlPacketAddGsFlushWide(void);

struct NmlFilterCubeFaces {
    int vertices[6][4];
};

/* Six original cube faces, each containing four corner indices. */

/* The projection helper's VFT0I/SQC2 output is integer GS coordinates. */

struct NmlFilterGsPair {
    GsTag payload;
    GsTag registerSlot;
};

typedef int NmlFilterGridVector[4] __attribute__((aligned(16)));

enum {
    NML_GRID_X,
    NML_GRID_Y,
    NML_GRID_U,
    NML_GRID_V
};

struct NmlFilterGridPoint {
    NmlFilterGridVector original;
    NmlFilterGridVector displaced;
};

extern struct NmlStealthDescriptor s_aStealthDat[10];

extern struct NmlStealthDescriptor s_inStealthUnitDat;

extern void *nmlPacketSetAttributeAlloc16N(int records);

extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);

extern void xglVectorScaleXYZ(Vector4 *destination, const Vector4 *source,
    float scale);

extern float xglAtan2(float x, float y);

static void _CurSetMatrix(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf27,0(%0)\n\t"
        "lqc2 $vf28,16(%0)\n\t"
        "lqc2 $vf29,32(%0)\n\t"
        "lqc2 $vf30,48(%0)"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _CurSetViewScaleTrans(const float viewScale[4], const float viewTrans[4])
{
    __asm__ __volatile__(
        "lqc2 $vf25,0(%0)\n\t"
        "lqc2 $vf26,0(%1)"
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

#define NML_FILTER_EMIT(id, payload) do { \
    packet[entryCount * 2].value = (payload); \
    packet[entryCount * 2 + 1].value = (id); \
    ++entryCount; \
} while (0)

#define NML_FILTER_VERTEX(point) do { \
    NML_FILTER_EMIT(3, (u64)(point)->displaced.u | \
        ((u64)(point)->displaced.v << 16)); \
    NML_FILTER_EMIT(4, (u64)(point)->displaced.x | \
        ((u64)(point)->displaced.y << 16) | 0x7fffffff00000000ULL); \
} while (0)

static void nmlModelSetFilterGunosys(const LayoutStore *modelArg, int flags, int count)
{
    int entryCount = 0;
    const LayoutStore *model = modelArg;
    struct NmlFilterGridPoint grid[4][5];
    struct NmlFilterGsPair *packet = (struct NmlFilterGsPair *)0x70000000;
    struct NmlFilterGsPair *alphaEntry;
    struct NmlFilterGsPair *clampEntry;
    struct NmlFilterGsPair *entry;
    int row;
    int column;
    int remaining;

    packet[0].registerSlot.value = 0x3f;
    packet[0].payload.value = 0;
    entryCount = 1;
    if (model->fields.filterFlags & 4) {
        packet[entryCount].payload.value = (u64)(0x24020000 | (sRender.framebufferPage << 5)) |
            0x2000000640000000ULL;
    } else {
        packet[entryCount].payload.value = (u64)(0x24020000 | (sRender.opaqueTexturePage << 5)) |
            0x2000000640000000ULL;
    }
    packet[entryCount].registerSlot.value = 6;
    ++entryCount;
    alphaEntry = (struct NmlFilterGsPair *)0x70000020;
    entryCount += 2;
    alphaEntry->payload.value = ((u64)(int)model->fields.filterAlpha << 32) | 100;
    alphaEntry->registerSlot.value = 0x42;
    clampEntry = (struct NmlFilterGsPair *)0x70000030;
    clampEntry->registerSlot.value = 8;
    clampEntry->payload.value = 0;
    if (flags) {
        packet[entryCount].payload.value = 0x5f001;
    } else {
        packet[entryCount].payload.value = 0x5d001;
    }
    packet[entryCount++].registerSlot.value = 0x47;
    packet[entryCount].payload.value = 0x3f80000000808080ULL;
    packet[entryCount].registerSlot.value = 1;
    ++entryCount;

    {
        /* Each row has four spans and one closing grid point. */
        for (row = 0; row < 4; ++row) {
            for (column = 0; column < 5; ++column) {
                struct NmlFilterGridPoint *point = &grid[row][column];
                point->original[NML_GRID_X] = ((column * 128) + 1792) << 4;
                point->original[NML_GRID_Y] = ((row * 448 / 3) + 1824) << 4;
                point->original[NML_GRID_U] = (column * 128) << 4;
                point->original[NML_GRID_V] = (row * 448 / 3) << 4;
            }
        }
    }
    for (row = 0; row < 4; ++row) {
        for (column = 0; column < 5; ++column) {
            struct NmlFilterGridPoint *point = &grid[row][column];
            float phase = (float)((count + row * 4 + column) % 16) * 0.0625f;
            float angle = (phase + phase) * 3.1415927f;
            float sine = xglSin(angle);
            int displacementX = (int)(model->fields.filterAmplitude * sine);
            float cosine = xglCos(angle);
            int displacementY = (int)(model->fields.filterAmplitude * cosine);
            displacementX = (column & 1) ? displacementX : -displacementX;
            displacementY = (column & 1) ? displacementY : -displacementY;
            point->displaced[NML_GRID_X] = point->original[NML_GRID_X] + displacementX;
            point->displaced[NML_GRID_U] = point->original[NML_GRID_U];
            point->displaced[NML_GRID_V] = point->original[NML_GRID_V];
            point->displaced[NML_GRID_Y] = point->original[NML_GRID_Y] + displacementY;
        }
    }
    for (row = 0; row < 3; ++row) {
        packet[entryCount].payload.value = 0x154;
        packet[entryCount].registerSlot.value = 0;
        ++entryCount;
        packet[entryCount].payload.value = (u64)grid[row][0].displaced[NML_GRID_U] |
            ((u64)grid[row][0].displaced[NML_GRID_V] << 16);
        packet[entryCount].registerSlot.value = 3;
        ++entryCount;
        packet[entryCount].payload.value = 0x7fffffff00000000ULL | (u64)grid[row][0].displaced[NML_GRID_X] |
            ((u64)grid[row][0].displaced[NML_GRID_Y] << 16);
        packet[entryCount].registerSlot.value = 4;
        ++entryCount;
        packet[entryCount].payload.value = (u64)grid[row + 1][0].displaced[NML_GRID_U] |
            ((u64)grid[row + 1][0].displaced[NML_GRID_V] << 16);
        packet[entryCount].registerSlot.value = 3;
        ++entryCount;
        packet[entryCount].payload.value = (u64)grid[row + 1][0].displaced[NML_GRID_X] |
            0x7fffffff00000000ULL | ((u64)grid[row + 1][0].displaced[NML_GRID_Y] << 16);
        packet[entryCount].registerSlot.value = 4;
        ++entryCount;
        for (column = 1; column < 5; ++column) {
            packet[entryCount].payload.value = (u64)grid[row][column].displaced[NML_GRID_U] |
                ((u64)grid[row][column].displaced[NML_GRID_V] << 16);
            packet[entryCount].registerSlot.value = 3;
            ++entryCount;
            packet[entryCount].payload.value = 0x7fffffff00000000ULL | (u64)grid[row][column].displaced[NML_GRID_X] |
                ((u64)grid[row][column].displaced[NML_GRID_Y] << 16);
            packet[entryCount].registerSlot.value = 4;
            ++entryCount;
            packet[entryCount].payload.value = (u64)grid[row + 1][column].displaced[NML_GRID_U] |
                ((u64)grid[row + 1][column].displaced[NML_GRID_V] << 16);
            packet[entryCount].registerSlot.value = 3;
            ++entryCount;
            packet[entryCount].payload.value = (u64)grid[row + 1][column].displaced[NML_GRID_X] |
                0x7fffffff00000000ULL | ((u64)grid[row + 1][column].displaced[NML_GRID_Y] << 16);
            packet[entryCount].registerSlot.value = 4;
            ++entryCount;
        }
    }
    nmlPacketGsInit();
    nmlPacketAddGsFrame(sRender.displayBufferBase, 0);
    nmlPacketAddGsFBA(0);
    if (entryCount > 0) {
        remaining = entryCount;
        entry = packet;
        do {
            --remaining;
            packet_gs_entry64(entry->registerSlot.value, &entry->payload.value);
            entry++;
        } while (remaining != 0);
    }
    nmlPacketAddGsFlushWide();
}

#undef NML_FILTER_EMIT
#undef NML_FILTER_VERTEX

/* Base tint of the stealth outline (red, green, blue, unused). */
struct NmlStealthTint {
    float channels[4];
} __attribute__((aligned(16)));

/* Original read-only data remains scaffolding, with no recovery credit. */

/* One strip vertex: its UV entry, then its XYZ entry whose register slot holds
 * the clip flags of the vertex and of the two before it in the strip. */
#define NML_STEALTH_VERTEX(vertex, entryIndex, clipFlags) { \
    struct NmlFilterGsPair *uvEntry = &packet[entryIndex]; \
    uvEntry->payload.value = (u64)geometry->uv[vertex].u | \
        ((u64)geometry->uv[vertex].v << 16); \
    uvEntry->registerSlot.value = 3; \
    { \
        u64 positionX = geometry->xyz[vertex].x; \
        u64 positionY = geometry->xyz[vertex].y; \
        struct NmlFilterGsPair *xyzEntry = &packet[(entryIndex) + 1]; \
        xyzEntry->payload.value = positionX | (positionY << 16) | \
            ((u64)geometry->xyz[vertex].z << 32); \
        xyzEntry->registerSlot.value = (clipFlags); \
    } \
}

#define NML_STEALTH_CLIP(vertex) (geometry->uv[vertex].clipFlags)

static void nmlModelSetFilterStealth(const LayoutStore *model, int flags)
{
    struct NmlStealthTint baseTint = { { 0.5f, 1.0f, 0.8f, 1.0f } };
    float outlineColor[3];
    struct NmlFilterGsPair *packet = (struct NmlFilterGsPair *)0x70000000;
    struct NmlFilterGsPair *entry;
    const struct NmlStealthGeometry *geometry;
    int remaining;
    int part;

    packet[0].payload.value = 0;
    packet[0].registerSlot.value = 0x3f;
    {
        struct NmlFilterGsPair *textureEntry = &packet[1];
        textureEntry->payload.value = (u64)(0x24020000 | (sRender.framebufferPage << 5)) |
            0x2000000640000000ULL;
        textureEntry->registerSlot.value = 6;
    }
    {
        struct NmlFilterGsPair *alphaEntry = &packet[2];
        alphaEntry->payload.value = ((u64)(int)model->fields.filterAlpha << 32) | 100;
        alphaEntry->registerSlot.value = 0x42;
    }
    {
        struct NmlFilterGsPair *clampEntry = &packet[3];
        clampEntry->payload.value = 0;
        clampEntry->registerSlot.value = 8;
    }
    {
        struct NmlFilterGsPair *testEntry = &packet[4];
        testEntry->payload.value = 0x3f001;
        testEntry->registerSlot.value = 0x47;
    }
    {
        struct NmlFilterGsPair *colorEntry = &packet[5];
        colorEntry->payload.value = 0x3f80000000808080ULL;
        colorEntry->registerSlot.value = 1;
    }
    nmlPacketGsInit();
    nmlPacketAddGsFrame(sRender.displayBufferBase, 0);
    nmlPacketAddGsFBA(0);
    entry = packet;
    for (remaining = 6 - 1; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64(entry->registerSlot.words[0], &entry->payload.value);
    }
    nmlPacketAddGsFlushWide();

    for (part = 0; part < model->fields.stealthPartCount; part++) {
        geometry = model->fields.stealthParts[part];
        packet[0].payload.value = 0x154;
        packet[0].registerSlot.value = 0;
        NML_STEALTH_VERTEX(13, 1, NML_STEALTH_CLIP(13));
        NML_STEALTH_VERTEX(12, 3,
            NML_STEALTH_CLIP(12) | NML_STEALTH_CLIP(13));
        NML_STEALTH_VERTEX(9, 5,
            NML_STEALTH_CLIP(9) | NML_STEALTH_CLIP(12) | NML_STEALTH_CLIP(13));
        NML_STEALTH_VERTEX(8, 7,
            NML_STEALTH_CLIP(8) | NML_STEALTH_CLIP(9) | NML_STEALTH_CLIP(12));
        NML_STEALTH_VERTEX(11, 9,
            NML_STEALTH_CLIP(11) | NML_STEALTH_CLIP(8) | NML_STEALTH_CLIP(9));
        NML_STEALTH_VERTEX(10, 11,
            NML_STEALTH_CLIP(10) | NML_STEALTH_CLIP(11) | NML_STEALTH_CLIP(8));
        NML_STEALTH_VERTEX(1, 13,
            NML_STEALTH_CLIP(1) | NML_STEALTH_CLIP(10) | NML_STEALTH_CLIP(11));
        NML_STEALTH_VERTEX(0, 15,
            NML_STEALTH_CLIP(0) | NML_STEALTH_CLIP(1) | NML_STEALTH_CLIP(10));
        NML_STEALTH_VERTEX(5, 17,
            NML_STEALTH_CLIP(5) | NML_STEALTH_CLIP(0) | NML_STEALTH_CLIP(1));
        NML_STEALTH_VERTEX(4, 19,
            NML_STEALTH_CLIP(4) | NML_STEALTH_CLIP(5) | NML_STEALTH_CLIP(0));
        NML_STEALTH_VERTEX(3, 21,
            NML_STEALTH_CLIP(3) | NML_STEALTH_CLIP(4) | NML_STEALTH_CLIP(5));
        NML_STEALTH_VERTEX(2, 23,
            NML_STEALTH_CLIP(2) | NML_STEALTH_CLIP(3) | NML_STEALTH_CLIP(4));
        NML_STEALTH_VERTEX(7, 25,
            NML_STEALTH_CLIP(7) | NML_STEALTH_CLIP(4) | NML_STEALTH_CLIP(5));
        NML_STEALTH_VERTEX(6, 27,
            NML_STEALTH_CLIP(6) | NML_STEALTH_CLIP(7) | NML_STEALTH_CLIP(4));
        nmlPacketGsInit();
        entry = packet;
        for (remaining = 28; remaining >= 0; remaining--, entry++) {
            packet_gs_entry64(entry->registerSlot.words[0], &entry->payload.value);
        }
        nmlPacketAddGsFlushWide();
    }

    outlineColor[0] = baseTint.channels[0] * 128.0f;
    outlineColor[1] = baseTint.channels[1] * 128.0f;
    outlineColor[2] = baseTint.channels[2] * 128.0f;
    packet[0].payload.value = 0x3d001;
    packet[0].registerSlot.value = 0x47;
    {
        struct NmlFilterGsPair *alphaEntry = &packet[1];
        alphaEntry->payload.value = ((u64)(int)model->fields.filterAmplitude << 32) | 100;
        alphaEntry->registerSlot.value = 0x42;
    }
    {
        struct NmlFilterGsPair *colorEntry = &packet[2];
        colorEntry->payload.value = (u64)(int)outlineColor[0] |
            ((u64)(int)outlineColor[1] << 8) | ((u64)(int)outlineColor[2] << 16) | 0x3f80000000000000ULL;
        colorEntry->registerSlot.value = 1;
    }
    {
        struct NmlFilterGsPair *primEntry = &packet[3];
        primEntry->payload.value = 0x46;
        primEntry->registerSlot.value = 0;
    }
    {
        struct NmlFilterGsPair *topCornerEntry = &packet[4];
        topCornerEntry->payload.value = 0x72007000;
        topCornerEntry->registerSlot.value = 4;
    }
    {
        struct NmlFilterGsPair *bottomCornerEntry = &packet[5];
        bottomCornerEntry->payload.value = 0x8e009000;
        bottomCornerEntry->registerSlot.value = 4;
    }
    entry = packet;
    for (remaining = 6 - 1; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64(entry->registerSlot.words[0], &entry->payload.value);
    }
    nmlPacketAddGsFlushWide();
}

#undef NML_STEALTH_CLIP

#undef NML_STEALTH_VERTEX

static void _WeightToGlobalPlaceInit(float matrix[4][4])
{
    __asm__ __volatile__(
        "lqc2 $vf10,0(%0)\n\t"
        "lqc2 $vf11,16(%0)\n\t"
        "lqc2 $vf12,32(%0)\n\t"
        "lqc2 $vf13,48(%0)"
        :
        : "r"(matrix)
        : "memory"
    );
}

static void _WeightToGlobalPlaceVec(Vector4 *destination,
    const Matrix4 *jointMatrix, const Matrix4 *jointAuxMatrix)
{
    __asm__ __volatile__(
        "lq $2,0(%2)\n\t"
        "lq $3,16(%2)\n\t"
        "lq $4,32(%2)\n\t"
        "lqc2 $vf3,48(%2)\n\t"
        "qmfc2.ni $5,$vf0\n\t"
        "pextlw $6,$3,$2\n\t"
        "pextuw $7,$3,$2\n\t"
        "pextlw $8,$5,$4\n\t"
        "pextuw $9,$5,$4\n\t"
        "pcpyld $2,$8,$6\n\t"
        "pcpyud $3,$6,$8\n\t"
        "pcpyld $4,$9,$7\n\t"
        "qmtc2.ni $2,$vf5\n\t"
        "qmtc2.ni $3,$vf6\n\t"
        "qmtc2.ni $4,$vf7\n\t"
        "vmulax.xyz ACC,$vf5,$vf3x\n\t"
        "vmadday.xyz ACC,$vf6,$vf3y\n\t"
        "vmaddz.xyz $vf2,$vf7,$vf3z\n\t"
        "vmove.w $vf2,$vf0\n\t"
        "lqc2 $vf27,0(%1)\n\t"
        "lqc2 $vf28,16(%1)\n\t"
        "lqc2 $vf29,32(%1)\n\t"
        "lqc2 $vf30,48(%1)\n\t"
        "vsub.xyz $vf2,$vf0,$vf2\n\t"
        "vmulax.xyzw ACC,$vf27,$vf2x\n\t"
        "vmadday.xyzw ACC,$vf28,$vf2y\n\t"
        "vmaddaz.xyzw ACC,$vf29,$vf2z\n\t"
        "vmaddw.xyzw $vf31,$vf30,$vf2w\n\t"
        "vmulax.xyzw ACC,$vf10,$vf31x\n\t"
        "vmadday.xyzw ACC,$vf11,$vf31y\n\t"
        "vmaddaz.xyzw ACC,$vf12,$vf31z\n\t"
        "vmaddw.xyzw $vf31,$vf13,$vf0w\n\t"
        "sqc2 $vf31,0(%0)"
        :
        : "r"(destination), "r"(jointMatrix), "r"(jointAuxMatrix)
        : "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9",
          "memory"
    );
}

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

/* Private owner proposals; original data stays unchanged scaffolding. */
static void _WeightToGlobalPlaceVec(Vector4 *destination,
    const Matrix4 *jointMatrix, const Matrix4 *jointAuxMatrix);

/* Offset one outline vertex across the bone by a signed width. */
#define NML_STEALTH_EDGE(vertex, center, centerY, depth, sign, width) { \
    geometry->xyz[vertex].x = (int)((float)geometry->xyz[center].x sign side.y * (width)); \
    geometry->xyz[vertex].y = (int)((centerY) sign side.x * (width)); \
    geometry->xyz[vertex].z = (depth); \
}

#define NML_STEALTH_EDGE_UV(vertex, center, centerY, sign, width) { \
    geometry->uv[vertex].u = (int)((float)geometry->xyz[center].x sign side.y * (width)); \
    geometry->uv[vertex].v = (int)((centerY) sign side.x * (width)); \
}

/* Blend a centre vertex with an outline vertex. */
#define NML_STEALTH_BAND(vertex, center, centerY, depth, edge) { \
    geometry->xyz[vertex].x = (int)((float)geometry->xyz[center].x * weight + \
        (float)geometry->xyz[edge].x * (1.0f - weight)); \
    geometry->xyz[vertex].y = (int)((centerY) * weight + \
        (float)geometry->xyz[edge].y * (1.0f - weight)); \
    geometry->xyz[vertex].z = (depth); \
}

#define NML_STEALTH_BAND_UV(vertex, center, centerY, edge) { \
    geometry->uv[vertex].u = (int)((float)geometry->xyz[center].x * weight + \
        (float)geometry->xyz[edge].x * (1.0f - weight)); \
    geometry->uv[vertex].v = (int)((centerY) * weight + \
        (float)geometry->xyz[edge].y * (1.0f - weight)); \
}

/* Push a vertex along the bone by the ring's longitudinal stretch. */
#define NML_STEALTH_EXTEND_XYZ(vertex, sign, amount) { \
    geometry->xyz[vertex].x += (int)(sign longitudinal.x * (amount)); \
    geometry->xyz[vertex].y += (int)(sign longitudinal.y * (amount)); \
}

#define NML_STEALTH_EXTEND_UV(vertex, sign, amount) { \
    geometry->uv[vertex].u += (int)(sign longitudinal.x * (amount)); \
    geometry->uv[vertex].v += (int)(sign longitudinal.y * (amount)); \
}

/* Interpolate the endpoint outside a screen edge in both texture and screen
 * coordinates. */
#define NML_STEALTH_CLIP_ENDPOINT(numerator, denominator) { \
    float fraction = __builtin_fabsf((float)(numerator) / (float)(denominator)); \
    float complement = 1.0f - fraction; \
    uv[outside].u = (int)(fraction * (float)uv[1 - outside].u + complement * (float)uv[outside].u); \
    uv[outside].v = (int)(fraction * (float)uv[1 - outside].v + complement * (float)uv[outside].v); \
    xyz[outside].x = (int)(fraction * (float)xyz[1 - outside].x + complement * (float)xyz[outside].x); \
    xyz[outside].y = (int)(fraction * (float)xyz[1 - outside].y + complement * (float)xyz[outside].y); \
}

void nmlFilterStealthMake(int partIndex, StudioCamera *camera,
    LayoutStore *model, float widthScale)
{
    Vector4 direction;
    Vector4 side;
    struct {
        float x;
        float y;
    } longitudinal;
    Vector4 center;
    Vector4 fallback;
    struct {
        int left;
        int right;
        int top;
        int bottom;
    } clipBounds;
    struct NmlStealthJointData *jointData = model->fields.jointData;
    struct NmlStealthGeometry *geometry;
    const struct NmlStealthDescriptor *descriptor;
    Vector4 *firstEndpoint;
    float firstY;
    float secondY;
    float weight;
    int firstDepth;
    int secondDepth;
    int jointCount;
    int index;
    union NmlStealthUV *uv;

    geometry = nmlPacketSetAttributeAlloc16N(31);
    model->fields.stealthParts[partIndex] = geometry;
    firstEndpoint = &geometry->endpoints[0];
    jointCount = model->fields.jointData->jointCount;
    descriptor = &s_aStealthDat[partIndex];
    if (descriptor->firstJoint < jointCount &&
        descriptor->secondJoint < jointCount) {
        _WeightToGlobalPlaceInit(model->fields.modelToWorld);
        _WeightToGlobalPlaceVec(firstEndpoint,
            &((const Matrix4 *)((const unsigned char *)jointData +
                jointData->jointMatrixOffset))[descriptor->firstJoint],
            &((const Matrix4 *)((const unsigned char *)jointData +
                jointData->jointAuxMatrixOffset))[descriptor->firstJoint]);
        _WeightToGlobalPlaceVec(&geometry->endpoints[1],
            &((const Matrix4 *)((const unsigned char *)jointData +
                jointData->jointMatrixOffset))[descriptor->secondJoint],
            &((const Matrix4 *)((const unsigned char *)jointData +
                jointData->jointAuxMatrixOffset))[descriptor->secondJoint]);
    } else {
        descriptor = &s_inStealthUnitDat;
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)"
            : : "r"(&fallback), "r"(&model->fields.fallbackSphere) : "$2", "memory");
        fallback.y -= fallback.w * 0.3f;
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)"
            : : "r"(firstEndpoint), "r"(&fallback) : "$2", "memory");
        fallback.y += fallback.w * 0.6f;
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)"
            : : "r"(&geometry->endpoints[1]), "r"(&fallback) : "$2", "memory");
    }

    __asm__ __volatile__(
        "lqc2 $vf3,0(%1)\n\t"
        "lqc2 $vf2,0(%0)\n\t"
        "vadd.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
        "sqc2 $vf2,0(%2)"
        : : "r"(firstEndpoint), "r"(&geometry->endpoints[1]), "r"(&center) : "memory");
    xglVectorScaleXYZ(&center, &center, 0.5f);
    geometry->sortClass = (float)descriptor->sortClass;
    firstEndpoint->w = _VectorLengthSQ(&camera->position, &center);
    if (descriptor->firstExtension != 0.0f) {
        __asm__ __volatile__(
            "lqc2 $vf3,0(%0)\n\t"
            "lqc2 $vf2,0(%1)\n\t"
            "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
            "sqc2 $vf2,0(%2)"
            : : "r"(firstEndpoint), "r"(&geometry->endpoints[1]), "r"(&direction) : "memory");
        xglVectorNormal(&direction, &direction);
        firstEndpoint->x -= direction.x * descriptor->firstExtension;
        firstEndpoint->y -= direction.y * descriptor->firstExtension;
        firstEndpoint->z -= direction.z * descriptor->firstExtension;
    }
    if (descriptor->secondExtension != 0.0f) {
        __asm__ __volatile__(
            "lqc2 $vf3,0(%0)\n\t"
            "lqc2 $vf2,0(%1)\n\t"
            "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
            "sqc2 $vf2,0(%2)"
            : : "r"(firstEndpoint), "r"(&geometry->endpoints[1]), "r"(&direction) : "memory");
        xglVectorNormal(&direction, &direction);
        geometry->endpoints[1].x += direction.x * descriptor->secondExtension;
        geometry->endpoints[1].y += direction.y * descriptor->secondExtension;
        geometry->endpoints[1].z += direction.z * descriptor->secondExtension;
    }

    _CurSetMatrix(camera->viewMatrix);
    _CurSetViewScaleTrans(&camera->screenScale.x, &camera->screenOffset.x);
    _CurRotTransPersClip(&geometry->xyz[0].vector, firstEndpoint);
    _CurRotTransPersClip(&geometry->xyz[1].vector, &geometry->endpoints[1]);
    direction.x = (float)geometry->xyz[1].x - (float)geometry->xyz[0].x;
    direction.y = (float)geometry->xyz[1].y - (float)geometry->xyz[0].y;
    direction.z = 0.0f;
    xglVectorNormal(&direction, &direction);
    side.z = xglAtan2(direction.x, direction.y);
    side.x = widthScale * xglCos(side.z + 1.5707964f);
    side.y = widthScale * xglSin(side.z + 1.5707964f);
    firstY = (float)geometry->xyz[0].y;
    firstDepth = geometry->xyz[0].z;
    secondDepth = geometry->xyz[1].z;
    secondY = (float)geometry->xyz[1].y;

    NML_STEALTH_EDGE(6, 0, firstY, firstDepth, -, descriptor->firstWidth);
    NML_STEALTH_EDGE(12, 0, firstY, firstDepth, +, descriptor->firstWidth);
    NML_STEALTH_EDGE(7, 1, secondY, secondDepth, -, descriptor->secondWidth);
    NML_STEALTH_EDGE(13, 1, secondY, secondDepth, +, descriptor->secondWidth);
    NML_STEALTH_EDGE_UV(6, 0, firstY, -, descriptor->firstWidth);
    NML_STEALTH_EDGE_UV(12, 0, firstY, +, descriptor->firstWidth);
    NML_STEALTH_EDGE_UV(7, 1, secondY, -, descriptor->secondWidth);
    NML_STEALTH_EDGE_UV(13, 1, secondY, +, descriptor->secondWidth);
    weight = 0.75f;
    NML_STEALTH_BAND(4, 0, firstY, firstDepth, 6);
    NML_STEALTH_BAND(10, 0, firstY, firstDepth, 12);
    NML_STEALTH_BAND(5, 1, secondY, secondDepth, 7);
    NML_STEALTH_BAND(11, 1, secondY, secondDepth, 13);
    weight = 0.9f;
    NML_STEALTH_BAND_UV(4, 0, firstY, 6);
    NML_STEALTH_BAND_UV(10, 0, firstY, 12);
    NML_STEALTH_BAND_UV(5, 1, secondY, 7);
    NML_STEALTH_BAND_UV(11, 1, secondY, 13);
    weight = 0.5f;
    NML_STEALTH_BAND(2, 0, firstY, firstDepth, 6);
    NML_STEALTH_BAND(8, 0, firstY, firstDepth, 12);
    NML_STEALTH_BAND(3, 1, secondY, secondDepth, 7);
    NML_STEALTH_BAND(9, 1, secondY, secondDepth, 13);
    weight = 0.7f;
    NML_STEALTH_BAND_UV(2, 0, firstY, 6);
    NML_STEALTH_BAND_UV(8, 0, firstY, 12);
    NML_STEALTH_BAND_UV(3, 1, secondY, 7);
    NML_STEALTH_BAND_UV(9, 1, secondY, 13);

    longitudinal.x = direction.x * descriptor->firstWidth * widthScale;
    longitudinal.y = direction.y * descriptor->firstWidth * widthScale;
    NML_STEALTH_EXTEND_XYZ(0, -, descriptor->longitudinalStretch[0]);
    NML_STEALTH_EXTEND_XYZ(1, +, descriptor->longitudinalStretch[0]);
    geometry->uv[0].u = geometry->xyz[0].x;
    geometry->uv[0].v = geometry->xyz[0].y;
    geometry->uv[1].u = geometry->xyz[1].x;
    geometry->uv[1].v = geometry->xyz[1].y;
    NML_STEALTH_EXTEND_XYZ(4, -, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_XYZ(5, +, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_XYZ(10, -, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_XYZ(11, +, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_UV(4, -, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_UV(5, +, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_UV(10, -, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_UV(11, +, descriptor->longitudinalStretch[1]);
    NML_STEALTH_EXTEND_XYZ(2, -, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_XYZ(3, +, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_XYZ(8, -, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_XYZ(9, +, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_UV(2, -, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_UV(3, +, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_UV(8, -, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_UV(9, +, descriptor->longitudinalStretch[2]);
    NML_STEALTH_EXTEND_XYZ(6, -, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_XYZ(7, +, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_XYZ(12, -, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_XYZ(13, +, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_UV(6, -, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_UV(7, +, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_UV(12, -, descriptor->longitudinalStretch[3]);
    NML_STEALTH_EXTEND_UV(13, +, descriptor->longitudinalStretch[3]);

    uv = &geometry->uv[0];
    for (index = 0; index < 14; index++, uv++) {
        uv->u -= (4096 - sRender.frameWidth) * 8;
        uv->v -= (4096 - sRender.frameHeight) * 8;
        uv->clipFlags = 4;
    }
    uv = &geometry->uv[0];
    {
        union NmlStealthXYZ *xyz = &geometry->xyz[0];

        for (index = 0; index < 7; index++, xyz += 2, uv += 2) {
            int outside;

            clipBounds.left = 16;
            clipBounds.right = (sRender.frameWidth - 1) << 4;
            clipBounds.bottom = (sRender.frameHeight - 1) << 4;
            clipBounds.top = 16;
            if (xyz[0].z < 0) {
                uv[0].clipFlags = 12;
            }
            if (xyz[1].z < 0) {
                uv[1].clipFlags = 12;
            }
            if (uv[0].u < clipBounds.left && uv[1].u < clipBounds.left) {
                uv[0].clipFlags = 12;
                uv[1].clipFlags = 12;
            }
            if (uv[0].v < clipBounds.top && uv[1].v < clipBounds.top) {
                uv[0].clipFlags = 12;
                uv[1].clipFlags = 12;
            }
            if (uv[0].u > clipBounds.right && uv[1].u > clipBounds.right) {
                uv[0].clipFlags = 12;
                uv[1].clipFlags = 12;
            }
            if (uv[0].v > clipBounds.bottom && uv[1].v > clipBounds.bottom) {
                uv[0].clipFlags = 12;
                uv[1].clipFlags = 12;
            }

            outside = 0;
            if ((uv[0].u < clipBounds.left && uv[1].u > clipBounds.left) ||
                (outside = (uv[1].u < clipBounds.left &&
                    uv[0].u > clipBounds.left)) != 0) {
                NML_STEALTH_CLIP_ENDPOINT(uv[outside].u,
                    uv[1 - outside].u - uv[outside].u);
            }
            outside = 0;
            if ((uv[0].v < clipBounds.top && uv[1].v > clipBounds.top) ||
                (outside = (uv[1].v < clipBounds.top &&
                    uv[0].v > clipBounds.top)) != 0) {
                NML_STEALTH_CLIP_ENDPOINT(uv[outside].v,
                    uv[1 - outside].v - uv[outside].v);
            }
            outside = 0;
            if ((uv[0].u > clipBounds.right && uv[1].u < clipBounds.right) ||
                (outside = (uv[1].u > clipBounds.right &&
                    uv[0].u < clipBounds.right)) != 0) {
                NML_STEALTH_CLIP_ENDPOINT(uv[outside].u - clipBounds.right,
                    uv[outside].u - uv[1 - outside].u);
            }
            outside = 0;
            if ((uv[0].v > clipBounds.bottom && uv[1].v < clipBounds.bottom) ||
                (outside = (uv[1].v > clipBounds.bottom &&
                    uv[0].v < clipBounds.bottom)) != 0) {
                NML_STEALTH_CLIP_ENDPOINT(uv[outside].v - clipBounds.bottom,
                    uv[outside].v - uv[1 - outside].v);
            }
            uv[0].u &= 0x1fff;
            uv[0].v &= 0x1fff;
            uv[1].u &= 0x1fff;
            uv[1].v &= 0x1fff;
        }
    }
}

#undef NML_STEALTH_EDGE
#undef NML_STEALTH_EDGE_UV
#undef NML_STEALTH_BAND
#undef NML_STEALTH_BAND_UV
#undef NML_STEALTH_EXTEND_XYZ
#undef NML_STEALTH_EXTEND_UV
#undef NML_STEALTH_CLIP_ENDPOINT

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
        packet_gs_entry64((int)entry->registerId, &entry->value);
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
        packet_gs_entry64((int)entry->registerId, &entry->value);
    }
    nmlPacketAddGsFlush();
}

void nmlFilterSetBufferRender(float opacity)
{
    struct NmlFilterGsRegisterEntry *entry;
    int remaining;

    ((struct NmlFilterGsRegisterEntry *)0x70000000)->registerId = 0x3f;
    ((struct NmlFilterGsRegisterEntry *)0x70000000)->value = 0;
    ((struct NmlFilterGsRegisterEntry *)0x70000010)->value =
        0x24020000 | ((u64)(sRender.framebufferPage << 5)) |
        ((u64)0x20000006 << 32) | ((u64)0x4000 << 16);
    ((struct NmlFilterGsRegisterEntry *)0x70000010)->registerId = 6;
    ((struct NmlFilterGsRegisterEntry *)0x70000020)->value = 0;
    ((struct NmlFilterGsRegisterEntry *)0x70000020)->registerId = 0x14;
    ((struct NmlFilterGsRegisterEntry *)0x70000030)->value = 0;
    ((struct NmlFilterGsRegisterEntry *)0x70000030)->registerId = 8;
    ((struct NmlFilterGsRegisterEntry *)0x70000040)->value = 0x31001;
    ((struct NmlFilterGsRegisterEntry *)0x70000040)->registerId = 0x47;
    ((struct NmlFilterGsRegisterEntry *)0x70000050)->value =
        ((u64)(int)(opacity * 128.0f) << 32) | 0x64;
    ((struct NmlFilterGsRegisterEntry *)0x70000050)->registerId = 0x42;
    ((struct NmlFilterGsRegisterEntry *)0x70000060)->value =
        0x3f80000080808080ULL;
    ((struct NmlFilterGsRegisterEntry *)0x70000060)->registerId = 1;
    ((struct NmlFilterGsRegisterEntry *)0x70000070)->value = 0x116;
    if (opacity < 1.0f) {
        ((struct NmlFilterGsRegisterEntry *)0x70000070)->value = 0x156;
    }
    ((struct NmlFilterGsRegisterEntry *)0x70000070)->registerId = 0;
    ((struct NmlFilterGsRegisterEntry *)0x70000080)->value = 0;
    ((struct NmlFilterGsRegisterEntry *)0x70000080)->registerId = 3;
    ((struct NmlFilterGsRegisterEntry *)0x70000090)->value = 0x72007000;
    ((struct NmlFilterGsRegisterEntry *)0x70000090)->registerId = 4;
    ((struct NmlFilterGsRegisterEntry *)0x700000A0)->value = 0x1C102000;
    ((struct NmlFilterGsRegisterEntry *)0x700000A0)->registerId = 3;
    ((struct NmlFilterGsRegisterEntry *)0x700000B0)->value = 0x8E009000;
    ((struct NmlFilterGsRegisterEntry *)0x700000B0)->registerId = 4;

    entry = (struct NmlFilterGsRegisterEntry *)0x70000000;
    nmlPacketGsInit();
    for (remaining = 11; remaining >= 0; remaining--, entry++) {
        packet_gs_entry64((u32)entry->registerId, &entry->value);
    }
    nmlPacketAddGsFlush();
}

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
        packet_gs_entry64((int)entry->registerId, &entry->value);
    }
    nmlPacketAddGsFlush();
}

void nmlFilterSetTexClear(int frameBase)
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
        packet_gs_entry64((int)entry->registerId, &entry->value);
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
        packet_gs_entry64((int)entry->registerId, &entry->value);
    }
    nmlPacketAddGsFlush();
}

extern void nmlPacketAddGsFrame1(int frame, int offset);

void nmlFilterSetFlatRender(LayoutStore *model)
{
    struct NmlFilterRenderView *render;
    GsTag *gsEntry3f;
    GsTag *gsEntry6;
    GsTag *gsEntry14;
    GsTag *gsEntry8;
    GsTag *gsEntry42;
    GsTag *gsEntry47First;
    GsTag *gsEntry1;
    GsTag *gsEntry0;
    GsTag *gsEntry3First;
    GsTag *gsEntry4First;
    GsTag *gsEntry3Last;
    GsTag *gsEntry4Last;
    GsTag *gsEntry47Last;
    GsTag *entry;
    int remaining;
    u64 texture;
    GsTag alphaPayload;
    int clampRegisterId;

    if (model == 0) {
        return;
    } else {
        clampRegisterId = 8;
        render = &sRender;
        texture = (u64)(0x24020000 | (render->framebufferPage << 5)) |
            0x2000000640000000ULL;
        gsEntry6 = (GsTag *)0x70000010;
        gsEntry14 = (GsTag *)0x70000020;
        gsEntry8 = (GsTag *)0x70000030;
        gsEntry3f = (GsTag *)0x70000000;
        gsEntry3f[0].value = 0;
        gsEntry3f[1].value = 0x3f;
        gsEntry6[0].value = texture;
        gsEntry6[1].value = 6;
        gsEntry14[0].value = 0x60;
        gsEntry14[1].value = 0x14;
        gsEntry8[0].value = 0;
        gsEntry8[1].value = clampRegisterId;
        alphaPayload.value = ((u64)(int)(model->fields.transparency * 128.0f) << 32) | 0x68;
        gsEntry42 = (GsTag *)0x70000040;
        gsEntry47First = (GsTag *)0x70000050;
        gsEntry1 = (GsTag *)0x70000060;
        gsEntry0 = (GsTag *)0x70000070;
        gsEntry3First = (GsTag *)0x70000080;
        gsEntry4First = (GsTag *)0x70000090;
        gsEntry3Last = (GsTag *)0x700000a0;
        gsEntry4Last = (GsTag *)0x700000b0;
        gsEntry47Last = (GsTag *)0x700000c0;
        gsEntry42[0] = alphaPayload;
        gsEntry42[1].value = 0x42;
        gsEntry47First[0].value = 0x30000;
        gsEntry47First[1].value = 0x47;
        gsEntry1[0].value = 0x3f80000080808080ULL;
        gsEntry1[1].value = 1;
        gsEntry0[0].value = 0x156;
        gsEntry0[1].value = 0;
        gsEntry3First[0].value = 0;
        gsEntry3First[1].value = 3;
        gsEntry4First[0].value = 0x72007000;
        gsEntry4First[1].value = 4;
        gsEntry3Last[0].value = 0x1c002000;
        gsEntry3Last[1].value = 3;
        gsEntry4Last[0].value = 0x8e009000ULL;
        gsEntry4Last[1].value = 4;
        gsEntry47Last[0].value = 0x50000;
        gsEntry47Last[1].value = 0x47;
        entry = (GsTag *)0x70000000;
        remaining = 12;
        nmlPacketGsInit();
        nmlPacketAddGsZbuf(1);
        nmlPacketAddGsFrame(render->displayBufferBase, 0);
        nmlPacketAddGsFrame1(render->displayBufferBase, 0);
        for (; remaining >= 0; remaining--, entry += 2) {
            packet_gs_entry64(entry[1].words[0], &entry[0].value);
        }
        nmlPacketAddGsFlushWide();
    }
}

void nmlFilterSetTexFillDraw(int frameBase, LayoutStore *model)
{
    struct NmlFilterGsPair *packet = (struct NmlFilterGsPair *)0x70000000;
    unsigned short frame = frameBase;
    int remaining;
    float fillScale = 128.0f;
    struct NmlFilterGsPair *alphaEntry = (struct NmlFilterGsPair *)0x70000010;
    u64 alphaBits;
    float mapAlpha;
    float fillWeight;
    u64 testRegister;

    remaining = 4;
    mapAlpha = (float)model->fields.textureMapAlpha;
    fillWeight = model->fields.textureFillWeight;
    packet[0].payload.value = 0x35001;
    packet[0].registerSlot.value = 0x47;
    alphaBits = ((u64)(fillScale - ((fillScale - (fillScale - mapAlpha)) * fillWeight)) << 24);
    alphaEntry->payload.value = alphaBits | 0x3F80000000808080ULL;
    alphaEntry->registerSlot.value = 1;
    {
        struct NmlFilterGsPair *modeEntry = (struct NmlFilterGsPair *)0x70000020;
        struct NmlFilterGsPair *clampEntry = (struct NmlFilterGsPair *)0x70000030;
        struct NmlFilterGsPair *colorEntry = (struct NmlFilterGsPair *)0x70000040;

        modeEntry->payload.value = 6;
        modeEntry->registerSlot.value = 0;
        clampEntry->payload.value = 0x72007000;
        clampEntry->registerSlot.value = 4;
        colorEntry->payload.value = 0x7FF08000;
        colorEntry->registerSlot.value = 4;
    }
    nmlPacketGsInit();
    {
        struct NmlFilterGsPair *blockEntry = packet;

        do {
            --remaining;
            packet_gs_entry64(blockEntry->registerSlot.words[0], &blockEntry->payload.value);
            blockEntry++;
        } while (remaining >= 0);
    }
    nmlPacketAddGsFlush();

    packet[0].payload.value = 0x24020000 | (frame << 5) | 0x2000000640000000ULL;
    packet[0].registerSlot.value = 6;
    {
        struct NmlFilterGsPair *scissorEntry = (struct NmlFilterGsPair *)0x70000010;
        struct NmlFilterGsPair *modeEntry = (struct NmlFilterGsPair *)0x70000020;
        struct NmlFilterGsPair *clampEntry = (struct NmlFilterGsPair *)0x70000030;
        struct NmlFilterGsPair *colorEntry = (struct NmlFilterGsPair *)0x70000040;

        scissorEntry->payload.value = 0x60;
        scissorEntry->registerSlot.value = 0x14;
        modeEntry->payload.value = 0x31001;
        modeEntry->registerSlot.value = 0x47;
        clampEntry->payload.value = 0;
        clampEntry->registerSlot.value = 8;
        colorEntry->payload.value = 0x3F80000080808080ULL;
        colorEntry->registerSlot.value = 1;
    }
    nmlPacketGsInit();
    nmlPacketAddGsFrame(frame, 0);
    {
        struct NmlFilterGsPair *blockEntry = packet;

        remaining = 4;
        do {
            --remaining;
            packet_gs_entry64(blockEntry->registerSlot.words[0], &blockEntry->payload.value);
            blockEntry++;
        } while (remaining >= 0);
    }
    nmlPacketAddGsFlush();

    {
        u64 textureFlush;
        u64 uvSlot;
        u64 xyzSlot;
        u64 firstUv;
        u64 firstXyz;
        u64 secondUv;
        u64 secondXyz;
        u64 spritePrimitive;
        struct NmlFilterGsPair *gsEntry1;
        struct NmlFilterGsPair *gsEntry2;
        struct NmlFilterGsPair *gsEntry3;
        struct NmlFilterGsPair *gsEntry4;
        struct NmlFilterGsPair *gsEntry5;
        struct NmlFilterGsPair *gsEntry6;
        struct NmlFilterGsPair *gsEntry7;
        struct NmlFilterGsPair *gsEntry8;
        struct NmlFilterGsPair *gsEntry9;
        struct NmlFilterGsPair *gsEntry10;
        struct NmlFilterGsPair *gsEntry11;

        do {
            textureFlush = 0x3F;
            gsEntry1 = (struct NmlFilterGsPair *)0x70000010;
            gsEntry2 = (struct NmlFilterGsPair *)0x70000020;
            gsEntry3 = (struct NmlFilterGsPair *)0x70000030;
            gsEntry4 = (struct NmlFilterGsPair *)0x70000040;
            gsEntry5 = (struct NmlFilterGsPair *)0x70000050;
            gsEntry8 = (struct NmlFilterGsPair *)0x70000080;
            gsEntry9 = (struct NmlFilterGsPair *)0x70000090;
            gsEntry10 = (struct NmlFilterGsPair *)0x700000A0;
            gsEntry6 = (struct NmlFilterGsPair *)0x70000060;
            gsEntry7 = (struct NmlFilterGsPair *)0x70000070;
            gsEntry11 = (struct NmlFilterGsPair *)0x700000B0;
        } while (0);
        do {
            spritePrimitive = 0x116;
            uvSlot = 3;
            xyzSlot = 4;
            firstUv = 0x100010;
            firstXyz = 0x72108010;
            secondUv = 0x0DF00FF0;
            secondXyz = 0x7FF08FF0;
        } while (0);
        packet[0].payload.value = 0;
        packet[0].registerSlot.value = textureFlush;
        gsEntry1->payload.value = spritePrimitive;
        gsEntry1->registerSlot.value = 0;
        gsEntry2->payload.value = firstUv;
        gsEntry2->registerSlot.value = uvSlot;
        gsEntry3->payload.value = firstXyz;
        gsEntry3->registerSlot.value = xyzSlot;
        gsEntry4->payload.value = secondUv;
        gsEntry4->registerSlot.value = uvSlot;
        gsEntry5->payload.value = secondXyz;
        gsEntry5->registerSlot.value = xyzSlot;
        gsEntry6->payload.value = 0;
        gsEntry6->registerSlot.value = 0x3F;
        gsEntry7->payload.value = spritePrimitive;
        gsEntry7->registerSlot.value = 0;
        gsEntry8->payload.value = 0x101010;
        gsEntry8->registerSlot.value = 3;
        gsEntry9->payload.value = 0x72107010;
        gsEntry9->registerSlot.value = 4;
        gsEntry10->payload.value = 0x0DF01FF0;
        gsEntry10->registerSlot.value = 3;
        gsEntry11->payload.value = 0x7FF07FF0;
        gsEntry11->registerSlot.value = 4;
    }
    nmlPacketGsInit();
    {
        struct NmlFilterGsPair *blockEntry = packet;

        remaining = 11;
        do {
            --remaining;
            packet_gs_entry64(blockEntry->registerSlot.words[0], &blockEntry->payload.value);
            blockEntry++;
        } while (remaining >= 0);
    }
    nmlPacketAddGsFlush();

    do {
        testRegister = 0x47;
        packet[0].payload.value = 0x31001;
        packet[0].registerSlot.value = testRegister;
        {
            struct NmlFilterGsPair *gsEntry1 = (struct NmlFilterGsPair *)0x70000010;
            struct NmlFilterGsPair *gsEntry2 = (struct NmlFilterGsPair *)0x70000020;
            struct NmlFilterGsPair *gsEntry3 = (struct NmlFilterGsPair *)0x70000030;
            struct NmlFilterGsPair *gsEntry4 = (struct NmlFilterGsPair *)0x70000040;
            struct NmlFilterGsPair *gsEntry5 = (struct NmlFilterGsPair *)0x70000050;
            struct NmlFilterGsPair *gsEntry6 = (struct NmlFilterGsPair *)0x70000060;
            struct NmlFilterGsPair *gsEntry7 = (struct NmlFilterGsPair *)0x70000070;

            gsEntry1->payload.value = 0x3F80000080000000ULL;
            gsEntry1->registerSlot.value = 1;
            gsEntry2->payload.value = 2;
            gsEntry2->registerSlot.value = 0;
            gsEntry3->payload.value = 0x72007FF0;
            gsEntry3->registerSlot.value = 4;
            gsEntry4->payload.value = 0x72007000;
            gsEntry4->registerSlot.value = 4;
            gsEntry5->payload.value = 0x7FF07000;
            gsEntry5->registerSlot.value = 4;
            gsEntry6->payload.value = 0x7FF07FF0;
            gsEntry6->registerSlot.value = 4;
            gsEntry7->payload.value = 0x72007FF0;
            gsEntry7->registerSlot.value = 4;
        }
        nmlPacketGsInit();
        {
            struct NmlFilterGsPair *blockEntry = packet;

            remaining = 7;
            do {
                --remaining;
                packet_gs_entry64(blockEntry->registerSlot.words[0], &blockEntry->payload.value);
                blockEntry++;
            } while (remaining >= 0);
        }
    } while (0);
    nmlPacketAddGsFlush();
}

extern StudioCamera *xglStudioGetActiveCamera(void);

typedef union NmlFilterPositionView {
    Vector4 vector;
    unsigned long long words[2];
} NmlFilterPositionView;

int nmlFilterSetVolumeCubeRender(Vector4 *minimum, Vector4 *maximum)
{
    int facingCamera[6];
    Vector4 corners[8];
    Vector4 *cornerBase;
    int projected[8][4];
    int vertexClip[8];
    int faceClip[6];
    struct NmlFilterCubeFaces faces;
    int needsClipping;
    StudioCamera *camera;
    NmlFilterPositionView *cameraPosition;
    struct NmlFilterGsPair *packet = (struct NmlFilterGsPair *)0x70000000;
    int iteration;

    camera = xglStudioGetActiveCamera();
    needsClipping = 0;
    memset(facingCamera, 0, sizeof(facingCamera));
    cameraPosition = (NmlFilterPositionView *)&camera->position;
    memset(faceClip, 0, sizeof(faceClip));
    faces = (struct NmlFilterCubeFaces){ .vertices = { { 1, 0, 5, 4 }, { 3, 2, 7, 6 }, { 0, 3, 4, 7 }, { 2, 1, 6, 5 }, { 0, 1, 3, 2 }, { 6, 5, 7, 4 } } };

    corners[0].z = corners[3].z = corners[4].z = corners[7].z = minimum->z;
    corners[1].z = corners[2].z = corners[5].z = corners[6].z = maximum->z;
    corners[0].x = corners[1].x = corners[4].x = corners[5].x = minimum->x;
    corners[3].x = corners[2].x = corners[7].x = corners[6].x = maximum->x;
    corners[4].y = corners[5].y = corners[6].y = corners[7].y = minimum->y;
    corners[0].y = corners[1].y = corners[2].y = corners[3].y = maximum->y;

    if (minimum->x < cameraPosition->vector.x)
        facingCamera[0] = 1;
    if (cameraPosition->vector.x < maximum->x)
        facingCamera[1] = 1;
    if (minimum->z < cameraPosition->vector.z)
        facingCamera[2] = 1;
    if (cameraPosition->vector.z < maximum->z)
        facingCamera[3] = 1;
    if (minimum->y < cameraPosition->vector.y)
        facingCamera[5] = 1;
    if (cameraPosition->vector.y < maximum->y)
        facingCamera[4] = 1;

    _CurSetMatrix(camera->viewMatrix);
    _CurSetViewScaleTrans(&camera->screenScale.x, &camera->screenOffset.x);
    cornerBase = &corners[0];
    for (iteration = 0; iteration < 8; iteration++) {
        vertexClip[iteration] = _CurRotTransPersClip(projected[iteration], &cornerBase[iteration]);
    }
    {
        int *clipEntry = faceClip;
        const int (*faceVertices)[4] = faces.vertices;
        for (iteration = 5; iteration >= 0; iteration--, faceVertices++, clipEntry++) {
            *clipEntry = vertexClip[(*faceVertices)[0]] +
                         vertexClip[(*faceVertices)[1]] +
                         vertexClip[(*faceVertices)[2]] +
                         vertexClip[(*faceVertices)[3]];
        }
    }

    packet->payload.value = ((u64)sRender.displayBufferBase |
        ((u64)sRender.pixelStorageMode << 24 |
         (u64)(sRender.frameWidth / 64) << 16)) | 0x00ffffff00000000ULL;
    packet->registerSlot.value = 0x4c;
    {
        struct NmlFilterGsPair *alphaCorrection = &packet[1];
        alphaCorrection->registerSlot.value = 0x4a;
        alphaCorrection->payload.value = 0;
    }
    {
        struct NmlFilterGsPair *color = &packet[2];
        color->payload.value = 0x3f80000000000000ULL;
        color->registerSlot.value = 1;
    }
    {
        struct NmlFilterGsPair *test = &packet[3];
        test->payload.value = 0x31001;
        test->registerSlot.value = 0x47;
    }
    {
        struct NmlFilterGsPair *primitive = &packet[4];
        primitive->payload.value = 6;
        primitive->registerSlot.value = 0;
    }
    {
        struct NmlFilterGsPair *firstVertex = &packet[5];
        firstVertex->payload.value = 0x72007000;
        firstVertex->registerSlot.value = 4;
    }
    {
        struct NmlFilterGsPair *lastVertex = &packet[6];
        lastVertex->payload.value = 0x8e009000ULL;
        lastVertex->registerSlot.value = 4;
    }
    {
        struct NmlFilterGsPair *entry = packet;
        for (iteration = 6; iteration >= 0; iteration--, entry++)
            packet_gs_entry64(entry->registerSlot.words[0], &entry->payload.value);
    }
    nmlPacketAddGsFlush();

    packet->payload.value = 0x51001;
    packet->registerSlot.value = 0x47;
    {
        struct NmlFilterGsPair *alphaCorrection = (struct NmlFilterGsPair *)0x70000010;
        struct NmlFilterGsPair *color = (struct NmlFilterGsPair *)0x70000020;
        alphaCorrection->registerSlot.value = 0x4a;
        alphaCorrection->payload.value = 1;
        color->payload.value = 0x3f80000000000000ULL;
        color->registerSlot.value = 1;
    }
    nmlPacketGsInit();
    {
        struct NmlFilterGsPair *entry = packet;
        for (iteration = 2; iteration >= 0; iteration--, entry++)
            packet_gs_entry64(entry->registerSlot.words[0], &entry->payload.value);
    }
    nmlPacketAddGsFlush();

#define NML_CUBE_CORNER(indices, index) { \
    struct NmlFilterGsPair *corner = &packet[1 + (index)]; \
    int vertexIndex = (indices)[index]; \
    u64 coordinates = (u64)projected[vertexIndex][0] | \
        ((u64)projected[vertexIndex][1] << 16) | \
        ((u64)projected[vertexIndex][2] << 32); \
    corner->registerSlot.value = 4; \
    corner->payload.value = coordinates; \
}
#define NML_CUBE_FACE(indices) { \
    packet->payload.value = 4; \
    packet->registerSlot.value = 0; \
    NML_CUBE_CORNER(indices, 0); \
    NML_CUBE_CORNER(indices, 1); \
    NML_CUBE_CORNER(indices, 2); \
    NML_CUBE_CORNER(indices, 3); \
    { \
        struct NmlFilterGsPair *entry = packet; \
        int remaining; \
        for (remaining = 4; remaining >= 0; remaining--) { \
            packet_gs_entry64(entry->registerSlot.words[0], &entry->payload.value); \
            entry += 1; \
        } \
    } \
    nmlPacketAddGsFlush(); \
}
    for (iteration = 0; iteration < 6; iteration++) {
        if (facingCamera[iteration] == 0) {
            if (faceClip[iteration] != 0) {
                needsClipping = 1;
            } else {
                NML_CUBE_FACE(faces.vertices[iteration]);
            }
        }
    }
    packet->registerSlot.value = 0x4a;
    packet->payload.value = 0;
    packet_gs_entry64(packet->registerSlot.words[0], &packet->payload.value);
    nmlPacketAddGsFlush();
    for (iteration = 0; iteration < 6; iteration++) {
        if (facingCamera[iteration] != 0) {
            if (faceClip[iteration] != 0) {
                needsClipping = 1;
            } else {
                NML_CUBE_FACE(faces.vertices[iteration]);
            }
        }
    }
#undef NML_CUBE_FACE
#undef NML_CUBE_CORNER
    return needsClipping;
}

