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

extern void *classJava_xeno_Unit;
extern void *classJava_xeno_Chr;

extern const char layout_peer[];
extern const char layout_px[];
extern const char layout_py[];
extern const char layout_pz[];
extern const char layout_ry[];
extern const float layout_unit_pi;
extern const float layout_chr_pi;

extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);
extern void *loadConstString(const char *bytes, int length);
/* canon: config/header-canon.json, as src/main/toolkit.h and src/main/chr.h
 * spell it. The former `void *` return here forced every use site to respell
 * the +16 read as a cast. */
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

#endif
