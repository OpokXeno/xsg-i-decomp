#ifndef EVT_ITEM_PRIVATE_H
#define EVT_ITEM_PRIVATE_H

#include "shared.h"

/*
 * Only the window's first halfword is evidenced here: both callbacks compare it
 * against 2 to decide that the window has finished (lhu 0x0 at 0x002c662c in
 * taskItemGet), so the state word is named and the rest of the window is not
 * modelled.
 */
typedef struct ItemGetWindow {
    unsigned short state;
} ItemGetWindow;

typedef struct EventItemTask EventItemTask;

/*
 * The scheduler places this task in a 0x80-byte pool node whose first 16 bytes
 * are the task header xglTaskEntryNext/xglTaskRemove maintain; everything after
 * them is the item-get task's own record, and the four members below are the
 * ones this unit's own code establishes.
 *
 * CreateEvtItemGetTask (0x002c6790) fills a fresh node in: `category` is its
 * first argument (sb 0x24 at 0x002c67f8), `window_created` starts cleared (sb $0
 * 0x30 at 0x002c6800) and `item_name` takes the GetItemName result (sw 0x18 at
 * 0x002c6810).  taskEvtItemGet then builds the window once and stores it in
 * `window` (sw 0x20; the same slot taskItemGet at 0x002c6388 writes at
 * 0x002c6618 and reads back at 0x002c6624), and both callbacks read
 * `window_created` as a signed byte (lb 0x30 at 0x002c63b0) and `category` as a
 * signed byte (lb 0x24 at 0x002c6460, 0x002c646c).
 *
 * The spans between them are left unmodelled on purpose.  taskItemGet writes
 * more of the record than this unit's C reads -- a halfword at +0x26, bytes at
 * +0x28 and +0x29, a word at +0x2c -- but only from the ItemBoxTbl row it
 * copies, which does not establish what those members mean, and the two words
 * at +0x10/+0x14 are only ever cleared.  So they stay byte ranges rather than
 * invented members (docs/naming.md).
 */
struct EventItemTask {
    XglTaskPrefix task;              /* +0x00 */
    unsigned char unmodeled_10[8];   /* +0x10 */
    const char *item_name;           /* +0x18 */
    unsigned char unmodeled_1c[4];   /* +0x1c */
    ItemGetWindow *window;           /* +0x20 */
    signed char category;            /* +0x24 */
    unsigned char unmodeled_25[11];  /* +0x25 */
    signed char window_created;      /* +0x30 */
};

typedef unsigned int GameLoopStateWords[];

extern const char evt_item_format_special[];

extern const char evt_item_format_normal[];

extern ItemGetWindow *createItemGetWin(const char *message);

char *dataEvtItmNameGet(int index);

extern unsigned char EvtItemTbl[];

/* GameLoopState is opaque here; only these two scalar offsets are evidenced. */
extern GameLoopStateWords GameLoopState;

#endif
