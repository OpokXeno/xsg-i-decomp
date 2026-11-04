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
            int robotID;                     /* +0x00, RgSelectRobotDisp prints it as "ROB %d" (lw +0x00) */
            unsigned int *actorFlags;        /* +0x04 */
            int disposeCounter;              /* +0x08 */
            char names[4][64];              /* +0x0c..+0x10b */
            unsigned int nameCount;          /* +0x10c */
            void *bufferSlots[5];            /* +0x110..+0x123 */
            struct RgSelectRobot *parent;    /* +0x124 */
            int hasActions;                  /* +0x128 */
            struct RgSelectAction *actions[3]; /* +0x12c */
            float screenPosTyped;            /* +0x138 */
            RgFileSysData *fileData;         /* +0x13c */
            RgDispModel *displayModel;       /* +0x140 */
            int actionActive[3];             /* +0x144 */
            unsigned int drawCount;          /* +0x150 */
            float displayYaw;                /* +0x154 */
            float displayPosition;           /* +0x158 */
            unsigned char *equipTable;       /* +0x15c, provisional */
            int equipSlot;                   /* +0x160, provisional */
            void *heapBufferTyped;           /* +0x164 */
        };
    };
};

typedef struct RgSelectAction {
    int weaponID;                    /* +0x00, RgSelectRobotDisp prints it as "WEP(%d) %d" (lw +0x00) */
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
