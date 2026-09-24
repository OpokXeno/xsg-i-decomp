/*
 * TU-local declarations of ov01/tu001 (src/ov01/obj.c).
 */

#ifndef SRC_OV01_OBJ_H
#define SRC_OV01_OBJ_H

#include "shared.h"

/* The manager heads are the free-list and active-list roots, not two active
 * execution lists.  The remaining slots are established by the same task
 * manager helpers; this wrapper only passes the manager through. */
typedef struct {
    ObjectTask *free_head;
    ObjectTask *active_head;
    ObjectTask *active_tail;
    ObjectTask *next_to_visit;
} TaskManager;

typedef void (*ObjectTaskCallback)(ObjectTask *task);

/*
 * The object task node as this unit's entry points allocate it.
 *
 * ObjectTask (include/shared.h) carries the part every consumer of an object
 * task shares: the 16-byte XglTaskPrefix the scheduler maintains and the work
 * pointer at +0x10 objRemove hands back to objWorkFree.  This unit establishes
 * one more member of the same record, the callback slot at +0x18: objEntrySub
 * (0x00a00378) clears it before it returns a fresh node (sw $0,0x18 at
 * 0x00a003d4), objEntry2 (0x00a00330) stores the caller's callback there,
 * objExec2 (0x00a002b8) loads it and calls it with the task, and objRemove
 * (0x00a00478) clears it again before releasing the node.  The word at +0x14
 * between them is written and read by nothing here and stays unmodelled, so
 * this type states what it knows and stops: it is not the complete 0x80-byte
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
 * The Vector4 at +0x00 has no evidenced role beyond that write: objStdInit
 * sets only its w component (the classic homogeneous-point default; x/y/z
 * stay at the preceding memset's zero) and no claimed function in this TU
 * reads it back. The Vector4 at +0x30 is `scale`: objStdInit sets all four
 * of its components to 1.0f, the standard multiplicative-identity pattern.
 */
typedef struct {
    Vector4 homogeneousVector;         /* +0x00: only .w is evidenced */
    unsigned char unmodeled_10[0x20];  /* +0x10 */
    Vector4 scale;                     /* +0x30 */
    unsigned char unmodeled_40[0x40];  /* +0x40 */
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

extern ObjectWork objWork[60];

int xglTaskRemove(XglTaskPrefix *task);

void objInit(void);

void objExec(void);

extern TaskManager taskMan;

extern void objExecSub(void *manager);

void objExec2(void);

void objEntry(void *argument);

ObjectTask *objEntry2(void *argument, ObjectTaskCallback callback);

extern ObjectTask *objEntrySub(void *manager, void *argument, int reverse);

void objEntryRev(void *argument);

ObjectTask *objEntry2Rev(void *argument, ObjectTaskCallback callback);

void objRemove(ObjectTask *task);

void objWorkInit(void);

extern void objWorkFree(void *work);

extern const char D_00A43860[];

void objStdInit(ObjectTask *task);

extern int printf(const char *format, ...);

extern const char objRemoveError[];

extern const char objRemovePureError[];

void objCmdClear(ObjectTask *task);

void *objCmdPtrGet(ObjectTask *task);

void *objCmdTailGet(ObjectTask *task);

extern const char D_00A438A0[];

void *objCmdPop(ObjectTask *task);

int objCmdNext(ObjectTask *task);

void fifoInit(Fifo *fifo, int capacity);

extern const char D_00A438D8[];

int fifoPop(Fifo *fifo);

int fifoEdGet(Fifo *fifo);

int fifoStGet(Fifo *fifo);

int fifoNumGet(Fifo *fifo);

#endif /* SRC_OV01_OBJ_H */
