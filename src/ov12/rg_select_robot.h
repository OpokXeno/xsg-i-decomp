/*
 * TU-local declarations of ov12/tu071 (src/ov12/rg_select_robot.c).
 */

#ifndef SRC_OV12_RG_SELECT_ROBOT_H
#define SRC_OV12_RG_SELECT_ROBOT_H

#include "ov12/rg_dispmodel.h"

typedef struct RgSelectRobot RgSelectRobot;

/*
 * The observed access view: RgSelectRobotScreenPos writes the preview
 * screen-position interpolation value at +0x138. CreateRgSelectRobot
 * allocates 0x168 bytes for one; no member or size beyond +0x138 is
 * claimed.
 */
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

typedef struct RgSelectAction {
    unsigned char unmodeled_00[4];
    unsigned int *actorFlags;
} RgSelectAction;

/* The allocation size CreateRgSelectRobot requests for one RgSelectRobot. */
#define RG_SELECT_ROBOT_SIZE 0x168

/* Loader state evidenced by the bounded adjacent load routines. */
extern int s_bLoading;

extern unsigned int s_uReqNum;

extern unsigned int s_uOkNum;

extern void *s_pWhoAreYou;

/* Shared main-unit CD read cancellation/reset entry. */
extern void xglCdReadCancel(void);

#endif /* SRC_OV12_RG_SELECT_ROBOT_H */
