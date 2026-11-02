#ifndef INCLUDE_MAIN_MAPTBL_H
#define INCLUDE_MAIN_MAPTBL_H

#include "shared.h"

/* Map ids are group * MAP_GROUP_SIZE + index in the group; maptbl holds
 * MAP_GROUP_COUNT entries (the last one is the terminator). */
#define MAP_GROUP_SIZE 100
#define MAP_GROUP_COUNT 20
#define MAP_ID_LIMIT 0x71e /* first invalid map id */

/* UseTestPath: which directory MAP_getPath returns. */
enum MapPathMode {
    MAP_PATH_DEFAULT = 0, /* data\map\ */
    MAP_PATH_TEST = 1,    /* data\map\test\ */
    MAP_PATH_CHECK = 2    /* data\map\check\ */
};

typedef struct {
    char **names;
    int count;
} MapNameGroup;

typedef struct CfCameraDefinition {
    unsigned char pedestal_mode;          /* +0x00 */
    unsigned char pedestal_interpolation; /* +0x01 */
    unsigned char unmodeled_02[6];
    float current_yaw;                    /* +0x08 */
    unsigned char unmodeled_0c[4];
    float offset[3];                      /* +0x10 */
    float perspective;                    /* +0x1c */
    union {
        struct {
            int first_mode;     /* +0x20 */
            float first_value;  /* +0x24 */
            int second_mode;    /* +0x28 */
            float second_value; /* +0x2c */
        } offset;
        float pedestal[4];
    } parameters;                         /* +0x20 */
    /* GameCamera reloads scratchpad->definition after storing angles[1]:
     * the original stores the angles through this union (alias set 0). */
    union {
        float values[4];
        unsigned int words[4];
    } angles;                             /* +0x30 */
    float lock_position[3];               /* +0x40 */
    int lock_mode;                        /* +0x4c */
    float interpolation[2];               /* +0x50 */
    unsigned char unmodeled_58[8];
    float fog_parameters[4];              /* +0x60 */
    Vector4 fog_color;                    /* +0x70 */
    unsigned char unmodeled_80[8];
    struct {
        float depth;        /* +0x88, +0x90, +0x98 */
        short width;        /* +0x8c, +0x94, +0x9c */
        short height;       /* +0x8e, +0x96, +0x9e */
    } defocus[3];                         /* +0x88 */
} CfCameraDefinition;

extern MapNameGroup maptbl[MAP_GROUP_COUNT];
extern CfCameraDefinition CfCameraDefine[33];
extern StudioCamera save_cam;

#endif /* INCLUDE_MAIN_MAPTBL_H */
