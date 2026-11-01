/*
 * TU-local declarations of ov01/tu007 (src/ov01/entry_first_init.c).
 */

#ifndef SRC_OV01_ENTRY_FIRST_INIT_H
#define SRC_OV01_ENTRY_FIRST_INIT_H

#include "shared.h"

int fadeEndChk(void);

/* The battle entry sequencer's current phase, one of the entryPhaseNN
   functions of this TU, all shaped int (void). */
typedef int (*EntryPhaseFunc)(void);

/*
 * vramCopyObj's task-specific payload past the shared ObjectTask head
 * (include/shared.h): the source/destination frame-buffer addresses it
 * hands to grVramCopyFBtoFB before releasing the one-shot task.
 */
typedef struct VramCopyObjTask {
    ObjectTask task;               /* +0x00 */
    unsigned char unmodeled_14[0x1C - 0x14];
    int srcAddr;                   /* +0x1C */
    int dstAddr;                   /* +0x20 */
} VramCopyObjTask;

/*
 * fadeOutObj's task-specific payload past the shared ObjectTask head: a
 * countdown (`life`) that fadeOutObj decrements by `speed` each tick, and a
 * `delay` tick count fadeOutObj counts down to zero first, before it starts
 * touching `life` at all.  When `life` reaches zero, fadeOutObj draws the
 * final frame, sets fadeFlag and releases the task.
 */
typedef struct FadeObjTask {
    ObjectTask task;                /* +0x00 */
    unsigned char unmodeled_14[0x1C - 0x14];
    int life;                       /* +0x1C */
    int speed;                      /* +0x20 */
    int delay;                      /* +0x24 */
} FadeObjTask;

/*
 * entryPhase35's view of the think-script spawn table thinkMonsTblGet
 * returns: 8-byte entries, a charaId halfword at +0x00 (`lh $3,0x0($4)`,
 * entries+unitIdx*8) that is the end-of-table sentinel (0 stops spawning)
 * and the per-entry pointer entryPhase35 hands to unitEnCreate as its
 * s16 *pl argument. The remaining 6 bytes are unitEnCreate's own, not read
 * here.
 */
typedef struct ThinkMonsEntry {
    short charaId;              /* +0x00 */
    unsigned char unmodeled_02[6];
} ThinkMonsEntry;

/*
 * grGpInit's packet record (src/ov01/gr_gp_init.c keeps its own GrPacket
 * TU-local): encEffAppend reads data at +0x0 (`lw $3,0x0($17)`) and count at
 * +0x4 (`lw $2,0x4($17)`).
 */
typedef struct GrPacket {
    void *data;  /* +0x0 */
    int count;   /* +0x4 */
} GrPacket;

/* One GIF packed-mode quadword of that packet, as encEffAppend fills it: a
 * GIFtag (`sd` of the control word at +0x0, of the register list at +0x8),
 * or a UV, RGBAQ or XYZ2 register write (`sw` per 32-bit lane). */
typedef union GrGifQuad {
    struct {
        u64 control;      /* +0x0 */
        u64 register_id;  /* +0x8 */
    } tag;
    struct {
        int u;            /* +0x0 */
        int v;            /* +0x4 */
    } uv;
    struct {
        int r;            /* +0x0 */
        int g;            /* +0x4 */
        int b;            /* +0x8 */
        int a;            /* +0xC */
    } rgba;
    struct {
        int x;            /* +0x0 */
        int y;            /* +0x4 */
        int z;            /* +0x8 */
        int flags;        /* +0xC */
    } xyz;
} GrGifQuad;

/* The packet's quadword at index, from a fresh read of packet->data. */
#define GR_PACKET_QUAD(packet, index) ((GrGifQuad *)(packet)->data + (index))

#endif /* SRC_OV01_ENTRY_FIRST_INIT_H */
