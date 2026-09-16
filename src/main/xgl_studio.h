#ifndef SRC_MAIN_XGL_STUDIO_H
#define SRC_MAIN_XGL_STUDIO_H

#include "shared.h"

/*
 * The light block of a studio record (0xf0 bytes). This TU owns the
 * definition: every accessor here only ever hands out or receives the
 * block's address (xglLightInit(xglStudioGetLight2())), so nothing in
 * xgl_studio.c itself indexes a field -- the evidenced layout below comes
 * from main/tu154's src/main/js_class_light_get_peer.c, whose
 * JS_classLight_setColor/JS_classLight_setDirection2 index into this same
 * 0xf0-byte span through xglStudioGetLight():
 *  +0x00 ambient_color: a Vector4, read whole; used when light_type == 0.
 *  +0x10 lights[3]: three { Vector4 color; Vector4 direction; } pairs,
 *        selected by lights[light_type - 1] for light_type in 1..3.
 *        JS_classLight_setColor writes lights[light_type - 1].color at byte
 *        offset light_type*0x20 - 0x10 from the StudioLight base (i.e. the
 *        base already at +0x10 for light_type==1, stepping 0x20 per type);
 *        JS_classLight_setDirection2 writes
 *        lights[light_type - 1].direction at byte offset light_type*0x20
 *        from that same base (0x10 further into the same pair).
 *  +0x70 unmodeled_70[0x80]: no accessor in either TU indexes past +0x70,
 *        so the remainder to the full 0xf0 span stays opaque.
 */
typedef struct StudioLightEntry {
    Vector4 color;
    Vector4 direction;
} StudioLightEntry;

typedef struct StudioLight {
    Vector4 ambient_color;
    StudioLightEntry lights[3];
    unsigned char unmodeled_70[0x80];
} StudioLight;

extern void xglStudioGetLight(StudioLight **light_out);

#endif
