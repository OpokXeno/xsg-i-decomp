#ifndef INCLUDE_OV12_RG_SELECT_ROBOT_H
#define INCLUDE_OV12_RG_SELECT_ROBOT_H

#include "shared.h"

#include "ov12/rg_dispmodel.h"

/*
 * RgDispModel is defined by src/ov12/rg_dispmodel.c (ov12/tu049); only the
 * pointer identity CreateXrgDispModelImpl returns and RgDispModelSetMode
 * takes is used here.
 */
typedef struct RgDispModel RgDispModel;

/*
 * _InitSelRob stores the preview-actor heap arena's base pointer at
 * +0x164; _DestructSelRob frees it back from the same field.
 */
struct RgSelectRobot {
    union {
        struct {
            unsigned char unmodeled_00[0x138];
            float screenPos;
            unsigned char unmodeled_13c[0x28];
            void *heapBuffer;
        };
        struct {
            unsigned char unmodeled_00_typed[4];
            unsigned int *actorFlags;        /* +0x04 */
            unsigned char unmodeled_08[0x120]; /* +0x08 */
            int hasActions;                  /* +0x128 */
            struct RgSelectAction *actions[3]; /* +0x12c */
            float screenPosTyped;            /* +0x138 */
            RgFileSysData *fileData;         /* +0x13c */
            RgDispModel *displayModel;       /* +0x140 */
            unsigned char unmodeled_144[0x14]; /* +0x144 */
            float displayPosition;           /* +0x158 */
            unsigned char unmodeled_15c[8];  /* +0x15c */
            void *heapBufferTyped;           /* +0x164 */
        };
    };
};

#endif /* INCLUDE_OV12_RG_SELECT_ROBOT_H */
