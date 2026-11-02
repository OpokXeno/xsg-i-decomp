#ifndef INCLUDE_MAIN_UNDULATE_H
#define INCLUDE_MAIN_UNDULATE_H

#include "common.h"
#include "shared.h"

/* UnduCheck / UnduParamInit height meaning "no collision surface found". */
#define UNDU_NO_HEIGHT (-1000.0f)

/*
 * UnduDataGetHeader identifier: with this bit set the low 15 bits index the
 * header array directly instead of being searched for in the identifiers.
 */
#define UNDU_HEADER_DIRECT 0x8000
#define UNDU_HEADER_INDEX_MASK 0x7fff

/* UnduParam.attrMask (the query flags UnduCheck reads) */
enum {
    UNDU_QUERY_MODE_MASK = 0x7,         /* what to do on a wall hit */
    UNDU_QUERY_MODE_BLOCK = 1,          /* stop the move */
    UNDU_QUERY_MODE_SLIDE = 2,          /* slide along the wall edge */
    UNDU_QUERY_MODE_CLIP = 3,           /* clip the move to where it meets the edge */
    UNDU_QUERY_SAME_LEVEL = 0x10,       /* only accept ground within the height tolerance of the sample */
    UNDU_QUERY_HEIGHT_TABLE = 0x20,     /* surfaces of the height-table kind take GameLoopState.attributeHeights */
    UNDU_QUERY_VERTEX_RANGE = 0x400,    /* reject ground outside the triangle's vertical extent */
    UNDU_QUERY_FLOOR_BELOW = 0x800      /* only accept ground at or below the sample */
};

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
