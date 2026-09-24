#ifndef LAYOUT_RECOVERY_PRIVATE_H
#define LAYOUT_RECOVERY_PRIVATE_H

/*
 * u8/u16 and the JavaField record come from the shared header rather than
 * being respelled here.
 * JavaField is the record lookupClassField returns; its `offset` member is
 * the byte offset of the named field inside an instance of the class, which
 * is the only member this translation unit reads.
 */
#include "shared.h"

/* UnduDataGetHeader returns this bounded four-component result. */
typedef struct LayoutHeader {
    float components[4];
} LayoutHeader;

/*
 * The Java xeno.Effect peer LAYOUT_mapID_setEffect's field lookup resolves
 * to: only the position and the one rotation component this TU writes
 * (swc1 at 0x2f6fe4/0x2f6fec/0x2f6ff4/0x2f6ffc, offsets +0x80/+0x84/+0x88/
 * +0x94), matching the position/rotation pair LAYOUT_mapID_setUnit and
 * LAYOUT_mapID_setChr read at their own peer's +0x10/+0x20 and +0x10/+0x50.
 */
typedef struct LayoutEffectPeer {
    unsigned char unmodeled_000[0x80];
    float position[3]; /* +0x80 */
    unsigned char unmodeled_08c[0x94 - 0x8C];
    float rotation_y; /* +0x94 */
} LayoutEffectPeer;

extern void *classJava_xeno_Unit;
extern void *classJava_xeno_Chr;
extern void *classJava_xeno_Effect;
extern void *classJava_xeno_util_Layout;

extern const char layout_peer[];
extern const char layout_px[];
extern const char layout_py[];
extern const char layout_pz[];
extern const char layout_ry[];
extern const float layout_unit_pi;
extern const float layout_chr_pi;
/* Same role as layout_unit_pi/layout_chr_pi (div.s at 0x2f70c0, a scaled
 * conversion applied to the effect's stored rotation component) but not
 * present in config/symbols/main.txt under this address, so it keeps its
 * splat name. */
extern float D_004D83A4;

extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);
extern void *loadConstString(const char *bytes, int length);
/* canon: config/header-canon.json, as src/main/toolkit.h and src/main/chr.h
 * spell it. The former `void *` return here forced every use site to respell
 * the +16 read as a cast. */
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
/* canon: config/header-canon.json (SceneObject/SceneClass are already
 * available through shared.h); restated here since the defining TU's
 * header is not shared. */
extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);

/* The script VM's per-thread context: an opaque pointer this TU's JNI
 * entry points receive but never dereference. */
typedef struct JThread JThread;

/*
 * A Java object reference: the class-pointer header (unused here) followed
 * by an int field this TU reads as a map id (lw a1,4(s1) at
 * 0x2f712c/0x2f7164/0x2f719c).
 */
typedef struct LayoutMapIdObject {
    unsigned char unmodeled_00[4];
    int map_id; /* +0x4 */
} LayoutMapIdObject;

/*
 * Java_xeno_util_Layout_set__Ljava_lang_Object_I's call block: the object
 * carrying the map id, the Chr/Unit/Effect target object, and the int
 * argument (lw a1,-13260(gp)/8c500004/8c520008/8c510000 at
 * 0x2f7108-0x2f711c).
 */
typedef struct LayoutSetArgs {
    LayoutMapIdObject *map; /* +0x0 */
    u8 *target;             /* +0x4 */
    int unit_id;             /* +0x8 */
} LayoutSetArgs;

/* Java_xeno_util_Layout_getManager__I's own manager singleton (main VA
 * 0x004DC6B0). The word right after it (0x004DC6B4) has no entry in
 * config/symbols/main.txt and keeps its splat name. */
extern int defaultLayout;
extern int D_004DC6B4;

#endif
