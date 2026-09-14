/*
 * TU-local declarations of main/tu246 (src/main/camera.c).
 */

#ifndef SRC_MAIN_CAMERA_H
#define SRC_MAIN_CAMERA_H

#include "shared.h"

/* Bounded spline record at camera + 0x4d0.
 * Only offsets 4/6/8/10/12/16 carry use-site evidence:
 *  +4 weight_mode, +10 sample_count, +12 component_count, +16 samples
 *  are stored by SPL_init (main:0x0030b970 disassembly);
 *  +6 first_key and +8 last_key are stored by CAMERA_rotateSPL itself.
 * Offset +0 is never touched by either body, so it is kept as opaque
 * reserved bytes, not a named semantic field. */
typedef struct CameraSpline {
    unsigned char reserved0[4];
    unsigned short weight_mode;
    unsigned short first_key;
    unsigned short last_key;
    unsigned short sample_count;
    unsigned int component_count;
    const float *samples;
} CameraSpline;

extern void SPL_init(CameraSpline *spline, unsigned int weight_mode,
                     const float *samples, unsigned int sample_count,
                     unsigned int component_count);

typedef struct JavaEnvironment JavaEnvironment;

/*
 * A Java `float[]` as the VM lays it out, which is what a native camera method
 * receives for its `[F` parameter.
 *
 * Every SPL entry point of this TU reads the same two words of it and nothing
 * else: Java_xeno_Camera_rotateSPL/transSPL/viewSPL (through CAMERA_rotateSPL
 * 0x002fb654, CAMERA_transSPL 0x002fb968, CAMERA_viewSPL 0x002fbb98) and
 * Java_xeno_Camera_rollSPL__aFI (0x002fbca4) / fovSPL__aFI (0x002fb56c) load
 * +0x04 and +0x08 and hand them to SPL_init as its sample count and its sample
 * pointer.
 *
 * +0x04 is the array's element count, not its byte size: the three-component
 * callers pass `>> 2` with component_count 3 and the one-component callers
 * `>> 1` with component_count 1, and a spline key is one weight float plus one
 * float per component (`SplinePoint` in src/main/spl.h, from the accepted
 * setWeightLen family at main:0x0030ba08), so length / (components + 1) is the
 * key count in both shapes.
 */
typedef struct JavaFloatArray {
    /* +0x00: the object header word every VM reference carries (SceneObject in
     * include/shared.h). No camera body reads it; what it points at is not
     * recovered here, so it is a word of storage and not a named field. */
    unsigned int : 32;
    unsigned int length;    /* +0x04: element count */
    const float *elements;  /* +0x08 */
} JavaFloatArray;

/*
 * One entry of the engine's camera table `tcamera` (main:0x00465e10).
 * TCAMERA_get (0x00308618) returns `tcamera + index * 0x12c0` and
 * Java_xeno_Camera_create__I (0x002fb790) hands that same address to Java as
 * the xeno.Camera object, so the jobject a camera native receives *is* a
 * CameraWork; TCAMERA_update (0x00308fb0) walks the eight entries with the
 * same 0x12c0 stride.
 *
 * Four animation channels drive the studio camera this record commands, in
 * this order: translation, view (the look-at point, and the rotation angles
 * directly in CAMERA_MODE_ROTATE_SPLINE), roll and field of view.
 * `mode[c]` selects what feeds channel `c` and `frame[c]` is the frame that
 * channel has reached; TCAMERA_update reads mode[0..3] at +0x0c..+0x18 and
 * advances frame[0..3] at +0x1c..+0x28 (0x00309154, 0x00309224, 0x00309300,
 * 0x00309330 and 0x003091a0, 0x003092bc, 0x00309324, 0x00309350).
 * `mode` is an array in the original: TCAMERA_init (0x00309428) stores the
 * idle mode into the four words with one backwards indexed loop rather than
 * four stores. `frame` is grouped the same way because each channel handler
 * advances exactly the word at +0x1c + 4 * channel; that grouping is an
 * inference from the parallel, not from an indexed access.
 *
 * Partial type: it stops at +0x2c. The record is 0x12c0 bytes long, and more
 * of it is evidenced -- the four channel payloads at +0x30, +0x4d0, +0x970 and
 * +0xe10 (stride 0x4a0), a float at +0x2c the debug light cursor edits
 * (updateCursorMode0 0x00267d3c), the peer written at +0x12b0 by
 * Java_xeno_Camera_start (0x002fbf1c) and the float at +0x12b4 seeded by
 * create (0x002fb7d8) -- but nothing evidences the internal extent of a
 * channel payload, so no member may be declared for any of them without
 * inventing the 0x4a0 span in between (docs/naming.md). The one this TU needs,
 * the view channel at +0x4d0, keeps the offset spelling it was accepted with.
 */
typedef struct CameraWork {
    /* +0x00: the object header word. Java_xeno_Camera_create__I (0x002fb7dc)
     * and STAGE_instance (0x0025aef8) both fill it from +0x18 of the loaded
     * class record; no camera body reads it and what it denotes is not
     * recovered, so it is a word of storage and not a named field. */
    unsigned int : 32;
    /* +0x04: the studio camera this record commands, as
     * xglStudioGetCamera2(int camera_id) indexes them. Written by create from
     * the Java argument (0x002fb7e4); camera_change (0x002fb598) compares it
     * against the same 0..7 range it passes to xglStudioGetCamera2. */
    int camera_id;
    /* +0x08: no body of this TU, of src/main/tcamera.c, of TCAMERA_init,
     * GAME_initCamera, STAGE_instance or of the db_light_write camera cursors
     * reads or writes it. It is the word the next member's offset requires,
     * not a field. */
    unsigned int : 32;
    int mode[4];   /* +0x0c: CameraChannelMode, indexed by CameraChannel */
    int frame[4];  /* +0x1c: frames elapsed in each channel */
} CameraWork;

/*
 * The channel order of CameraWork.mode / CameraWork.frame, read off
 * TCAMERA_update: each index dispatches to that channel's handlers and uses
 * that channel's payload.
 */
enum CameraChannel {
    CAMERA_CHANNEL_TRANSLATE = 0, /* payload +0x30,  TCAMERA_trans* */
    CAMERA_CHANNEL_VIEW = 1,      /* payload +0x4d0, TCAMERA_viewSPL and rotateSPL */
    CAMERA_CHANNEL_ROLL = 2,      /* payload +0x970, TCAMERA_rollSPL */
    CAMERA_CHANNEL_FOV = 3        /* payload +0xe10, TCAMERA_fovSPL */
};

/*
 * What a channel's mode word selects, from TCAMERA_update's two dispatches
 * (0x0030913c..0x00309228 for the translate channel, 0x00309228..0x00309304
 * for the view channel) and TCAMERA_init's default.
 */
enum CameraChannelMode {
    CAMERA_MODE_IDLE = 0x10,          /* TCAMERA_init 0x0030944c; no handler */
    CAMERA_MODE_MOTION_PACK = 0x11,   /* TCAMERA_transMPack / viewMPack */
    CAMERA_MODE_SPLINE = 0x12,        /* TCAMERA_transSPL/viewSPL/rollSPL/fovSPL */
    CAMERA_MODE_ROTATE_SPLINE = 0x13, /* TCAMERA_rotateSPL, view channel only */
    CAMERA_MODE_CONSTANT = 0x14,      /* TCAMERA_transCNS / viewCNS */
    CAMERA_MODE_MOTION_PACK2 = 0x15   /* TCAMERA_transMPack2 */
};

/*
 * The native-method argument block CAMERA_rotateSPL is called with: one
 * four-byte slot per Java argument, `this` first, exactly as
 * Java_xeno_Camera_rotateSPL__aFI (0x002fb6f8) and __aFIII (0x002fb728) pass
 * it on. The two key slots are declared by the halfword the body reads out of
 * each: the compiler takes the low half of the slot (lhu 0xc, lhu 0x10 at
 * 0x002fb6c4/0x002fb6cc) because the spline's own keys are halfwords.
 */
typedef struct CameraSplineRequest {
    CameraWork *camera;            /* +0x00: the jobject */
    const JavaFloatArray *samples; /* +0x04 */
    unsigned int weight_mode;      /* +0x08: SPL_init's weight selector */
    /* +0x0c and +0x10: the two key slots. Only the low halfword of each is
     * read (lhu 0xc, lhu 0x10 at 0x002fb6c4/0x002fb6cc), because a spline key
     * is a halfword; the high half of each four-byte argument slot is the
     * width the block forces and is never read, so it is a bitfield of storage
     * and not a field. Reading the whole slot as an `unsigned int` instead
     * still compiles to the two `lhu`, but lets the scheduler hoist the second
     * load above the first store: form f01, 8 bytes differ at 0x002fb6c8. */
    unsigned short first_key;      /* +0x0c */
    unsigned int : 16;             /* +0x0e */
    unsigned short last_key;       /* +0x10 */
} CameraSplineRequest;

extern StudioCamera *xglStudioGetCamera2(int camera_id);

#endif /* SRC_MAIN_CAMERA_H */
