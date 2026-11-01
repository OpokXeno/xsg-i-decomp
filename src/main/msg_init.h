#ifndef SRC_MAIN_MSG_INIT_H
#define SRC_MAIN_MSG_INIT_H

#include "shared.h"

/* MBUF_init/MBUF_create (not yet recovered) establish the rest of the mbuf
 * layout; only the leading word is evidenced here. */
typedef struct MsgBuffer {
    int active;
    unsigned int capacity;
    unsigned char **entry_table;
    unsigned char *cursor;
    unsigned char workspace[0x800];
} MsgBuffer;

/*
 * Message queue state: the text lives in buffer, read from head up to tail.
 * width is the pixel width of the current line, count the number of glyphs
 * handed out so far.
 */
typedef struct MsgQueue {
    unsigned char *buffer;
    unsigned char unmodeled_04[4];
    short head;
    short tail;
    short size;
    short count;
    short width;
    short last;
    short limit;
    unsigned char unmodeled_22[6];
    int label;
} MsgQueue;

#endif
