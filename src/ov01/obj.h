/*
 * TU-local declarations of ov01/tu001 (src/ov01/obj.c).
 */

#ifndef SRC_OV01_OBJ_H
#define SRC_OV01_OBJ_H

#include "shared.h"

/* The object task pool objInit clears: slot count and the 0x80-byte slot size. */
#define OBJ_TASK_CAPACITY 50
#define OBJ_TASK_SLOT_SIZE 0x80

/* The objWork pool: slot count, and the value ObjectWork.used holds while a slot is taken. */
#define OBJ_WORK_COUNT 60
#define OBJ_WORK_IN_USE 1

/* Queued commands per object (ObjectCommandQueue.entries). */
#define OBJ_CMD_QUEUE_CAPACITY 5

/* The scheduler callback xglTaskEntryNext stores in XglTaskPrefix.callback. */
typedef int (*ObjectSchedulerCallback)(XglTaskPrefix *task);

XglTaskPrefix *xglTaskEntryPrev(XglTaskScheduler *scheduler,
                                ObjectSchedulerCallback callback,
                                XglTaskPrefix *entry);

/*
 * The object task node this unit allocates.  ObjectTask (include/shared.h)
 * carries the scheduler prefix and the work pointer; the callback slot at
 * +0x18 is established here: objEntrySub clears it, objEntry2 stores the
 * caller's callback, objExec2 calls it, and objRemove clears it again.  The
 * word at +0x14 is not read by this unit, so this is not the complete 0x80-byte
 * node.
 */
typedef struct {
    ObjectTask base;                /* +0x00 */
    unsigned char unmodeled_14[4];  /* +0x14 */
    ObjectTaskCallback exec;        /* +0x18 */
} ObjectTaskNode;

/* The bit curCmdSet (src/ov01/battle_init.c) sets on the same object's flags
 * word when it installs a command whose selector is non-zero, and
 * objCmdClear clears. */
#define OBJCMD_QUEUE_PENDING 0x2

/* One 20-byte queued command (the record curCmdSet copies in
 * src/ov01/battle_init.c); objCmdPtrGet/objCmdTailGet/objCmdPop index it. */
typedef struct {
    int selector;
    int argument;
    int word08;
    int word0c;
    int word10;
} ObjectCommandEntry;

typedef struct {
    int flags;
    ObjectCommandEntry entries[5];
    int writeIndex;
    int readIndex;
    int word70;
    int word74;
} ObjectCommandQueue;

/*
 * A ring of indices fifoInit establishes and fifoPop/fifoEdGet/fifoStGet/
 * fifoNumGet read: base is the wraparound reset value (0 in every evidenced
 * call), maxIndex is the highest valid cursor value (capacity - 1),
 * readIndex/writeIndex are the cursors fifoStGet/fifoEdGet return, and count
 * is the element count fifoNumGet returns.  No element storage is modelled:
 * this unit only tracks the cursors into a buffer its callers own.
 */
typedef struct {
    int base;       /* +0x00 */
    int maxIndex;   /* +0x04 */
    int readIndex;  /* +0x08 */
    int writeIndex; /* +0x0C */
    int count;      /* +0x10 */
} Fifo;

/*
 * The +0x90..+0x110 tail of an objWork slot (declared below): objStdInit
 * clears all 0x80 bytes then seeds two homogeneous components to 1.0f.
 * The Vector4 at +0x00 is the object's position: objStdInit sets only its
 * w component (the classic homogeneous-point default; x/y/z stay at the
 * preceding memset's zero), and objStdMove advances x/y/z by `velocity` and
 * snaps them onto `target`. The Vector4 at +0x30 is `scale`: objStdInit sets
 * all four of its components to 1.0f, the standard multiplicative-identity
 * pattern. objStdMove adds `acceleration` (+0x50) to `velocity` (+0x40) every
 * update and stops each axis at `target` (+0x60).
 */
typedef struct {
    Vector4 homogeneousVector;         /* +0x00: only .w is evidenced */
    unsigned char unmodeled_10[0x20];  /* +0x10 */
    Vector4 scale;                     /* +0x30 */
    Vector4 velocity;                  /* +0x40: objStdMove adds it to +0x00 */
    Vector4 acceleration;              /* +0x50: objStdMove adds it to velocity */
    Vector4 target;                    /* +0x60: where objStdMove stops +0x00 */
    unsigned char unmodeled_70[0x10];  /* +0x70 */
} ObjectWorkTransform;

/*
 * One 0x110-byte slot of the 60-entry objWork pool (config/symbols/ov01.txt:
 * objWork, 0x00A51970). objWorkInit clears `used` for every slot at startup
 * (sw $0,0(slot) x60 at 0x00a00558) and objWorkFree clears it when a slot
 * returns to the pool (sw $0,0($4) at 0x00a00628); objStdInit initializes
 * the `transform` tail. src/ov01/menu.c independently models this same
 * slot's +0x70..+0x84 span as DmgNumWork once objEntry2 hands it to an
 * objDmgNum task, a span that does not overlap `used` or `transform`.
 */
typedef struct {
    int used;                         /* +0x00 */
    unsigned char unmodeled_04[0x8C]; /* +0x04 */
    ObjectWorkTransform transform;    /* +0x90 */
} ObjectWork;


void objInit(void);

void objExec(void);

extern void objExecSub(XglTaskScheduler *manager);

void objExec2(void);

void objEntry(ObjectSchedulerCallback schedulerCallback);

ObjectTask *objEntry2(ObjectSchedulerCallback schedulerCallback, ObjectTaskCallback callback);

extern ObjectTask *objEntrySub(XglTaskScheduler *manager, ObjectSchedulerCallback schedulerCallback, int reverse);

void objEntryRev(ObjectSchedulerCallback schedulerCallback);

ObjectTask *objEntry2Rev(ObjectSchedulerCallback schedulerCallback, ObjectTaskCallback callback);

void objRemove(ObjectTask *task);

void objWorkInit(void);

extern void objWorkFree(ObjectWork *work);

extern ObjectWork *objWorkGet(void);

extern const char objEntryErrorText[];

extern const char objEntryPureErrorText[];
extern const char objWorkGetErrorText[];
extern const char objCmdPushBuffOverText[];
extern const char objFifoPushErrorText[];

extern const char objWorkFreeErrorText[];

void objStdInit(ObjectTask *task);

extern int printf(const char *format, ...);

extern const char objRemoveError[];

extern const char objRemovePureError[];

void objCmdClear(ObjectTask *task);

ObjectCommandEntry *objCmdPtrGet(ObjectTask *task);

ObjectCommandEntry *objCmdTailGet(ObjectTask *task);

extern const char objCmdPopBuffOverText[];

ObjectCommandEntry *objCmdPush(ObjectTask *task);

ObjectCommandEntry *objCmdPop(ObjectTask *task);

int objCmdNext(ObjectTask *task);

ObjectTask *objEntryPure(ObjectSchedulerCallback callback);

int objStdMove(ObjectTask *task);

void objHatoVec(ObjectTask *task, int steps);

void fifoInit(Fifo *fifo, int capacity);

extern const char objFifoPopErrorText[];

int fifoPush(Fifo *fifo);

int fifoPop(Fifo *fifo);

int fifoEdGet(Fifo *fifo);

int fifoStGet(Fifo *fifo);

int fifoNumGet(Fifo *fifo);

#endif /* SRC_OV01_OBJ_H */
