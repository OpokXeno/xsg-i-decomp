/*
 * TU-local declarations of ov01/tu025 (src/ov01/m_ef_create_smp_01.c).
 */

#ifndef SRC_OV01_M_EF_CREATE_SMP_01_H
#define SRC_OV01_M_EF_CREATE_SMP_01_H

#include "shared.h"
#include "ov01/m_ef.h"

/* The SMP01 work area fnSMP01_PO000 (the object's post-process callback)
 * is handed: it reaches only the frame counter at +0x70, which it
 * increments once per call, firing sefHitEffect at 15 and MEfObjDestroy
 * at 25. The rest of the record belongs to the other bodies of this TU,
 * still unrecovered, so it stays unmodeled here.
 */
typedef struct Smp01State {
    unsigned char unmodeled_000[0x70]; /* 0x000 */
    int frame;                         /* 0x070 */
} Smp01State;

/* The GS direct-transfer packet MGsGPInit prepares and MGsGPTerm flushes
 * (same layout as m_gs.c's TU-local MGsPacket). */
typedef struct Smp01Packet {
    void *data;    /* 0x00 */
    int capacity;  /* 0x04 */
    void *current; /* 0x08 */
    int count;     /* 0x0C */
    int auxCount;  /* 0x10 */
} Smp01Packet;

/* svGetPrmFromName's output (SvImageParameter, src/main/sv_get_image_item.c):
 * the named texture's GS TEX0 value (the packet callback sends it as an A+D
 * TEX0_1 record) and its scaled width and height. */
typedef struct Smp01ImageParameter {
    u64 texture;
    float width;
    float height;
} Smp01ImageParameter;

enum { SMP01_SMOKE_TRAILS = 10, SMP01_SMOKE_POINTS = 3 };

/* The SMP01 work area (object + 0x20). */
typedef struct Smp01Work {
    unsigned char unmodeled_000[8];    /* 0x000 */
    Actor *sourceActor;            /* 0x008: the matrix source */
    u32 sourceCoordinate;              /* 0x00C: its coordinate selector */
    Vector4 sourceOffset;              /* 0x010 */
    unsigned char unmodeled_020[4];    /* 0x020 */
    Actor *targetActor;            /* 0x024: the coordinate source */
    unsigned char unmodeled_028[0x18]; /* 0x028 */
    int modelEntry;                    /* 0x040 */
    const char *textureName;           /* 0x044 */
    unsigned char unmodeled_048[0x28]; /* 0x048 */
    int frame;                         /* 0x070 */
    int phase;                         /* 0x074 */
    int phaseCount;                    /* 0x078 */
    unsigned char unmodeled_07C[4];    /* 0x07C */
    Vector4 spawnPoint;                /* 0x080 */
    Vector4 phaseStart;                /* 0x090 */
    Vector4 targetPosition;            /* 0x0A0 */
    Vector4 position;                  /* 0x0B0 */
    Vector4 previousPosition;          /* 0x0C0 */
    Vector4 angles;                    /* 0x0D0 */
    float phaseAngle;                  /* 0x0E0 */
    Smp01Packet packet;                /* 0x0E4 */
    u64 smokeTexture0First;            /* 0x0F8 */
    u64 smokeTexture0Second;           /* 0x100 */
    short smokeActive[SMP01_SMOKE_TRAILS];                    /* 0x108 */
    unsigned char unmodeled_11C[4];                           /* 0x11C */
    Vector4 smokePoints[SMP01_SMOKE_TRAILS][SMP01_SMOKE_POINTS]; /* 0x120 */
    float smokeAngles[SMP01_SMOKE_TRAILS][SMP01_SMOKE_POINTS];   /* 0x300 */
} Smp01Work;

typedef struct Smp01Object {
    unsigned char unmodeled_000[4];                                       /* 0x000 */
    void (*updateCallback)(struct Smp01Object *object, Smp01Work *work);  /* 0x004 */
    void (*drawCallback)(struct Smp01Object *object, Smp01Work *work);    /* 0x008 */
    void (*packetCallback)(struct Smp01Object *object, Smp01Work *work);  /* 0x00C */
    void (*postCallback)(void *object, void *work);                       /* 0x010 */
    unsigned char unmodeled_014[0x0C];                                    /* 0x014 */
    Smp01Work work;                                                       /* 0x020 */
} Smp01Object;

#endif
