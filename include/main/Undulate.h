#ifndef INCLUDE_MAIN_UNDULATE_H
#define INCLUDE_MAIN_UNDULATE_H

#include "common.h"
#include "shared.h"

/*
 * UnduDataGetHeader's result (src/main/Undulate.c): the bounded four-component
 * result that layout.c, call_java_method.c, map_1.c and game_debug_menu.c read:
 * a position (x, y, z) and the facing angle (w), passed whole to the Vector4
 * geometry helpers.
 */
typedef struct LayoutHeader {
    Vector4 locator;
} LayoutHeader;

/*
 * The 0x40-byte query block UnduCheck reads and writes through UnduParamInit
 * (src/main/Undulate.c owns both). One definition for every partial view:
 * get.c's UnduParam (UnduTemp, UnduTest; the excluded and included masks at
 * +0x30 and +0x38), set_motion.c's belt-surface read (UnduTemp.attribute at
 * +0x20) and the camera probes of game_camera.c (queryCursor, resultFlags,
 * limit, height, exclude). Offsets are the ones each view used.
 */
typedef struct UnduParam {
    int queryFlags;                       /* +0x00 */
    int queryCursor;                      /* +0x04 */
    short attrMask;                       /* +0x08 */
    unsigned short resultFlags;           /* +0x0a */
    unsigned char unmodeled_0c[4];
    float limit;                          /* +0x10 */
    float height;                         /* +0x14 */
    LayoutHeader *header;                 /* +0x18 */
    void *exclude;                        /* +0x1c */
    unsigned long long attribute;         /* +0x20 */
    unsigned char unmodeled_28[8];
    long long excluded_units;             /* +0x30: Check_Undu stores the excluded mask. */
    long long query_mode;                 /* +0x38: Check_Undu stores the included mask. */
} UnduParam;

/* Functions and data used by other translation units. */
extern signed char FLAG_FRAME_60;
extern int StudioEntrySkip;
extern float UnduGet2(void *queryAddress, float x, float z);
LayoutHeader *UnduDataGetHeader(int map_index, unsigned int unit_index);
float UnduGet(float x, float z);
union UnduColiBlob;
void UnduInit(union UnduColiBlob *data);

#endif /* INCLUDE_MAIN_UNDULATE_H */
