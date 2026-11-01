#ifndef INCLUDE_MAIN_MSG_INIT_H
#define INCLUDE_MAIN_MSG_INIT_H

typedef struct MsgBuffer {
    int active;
    unsigned int capacity;
    unsigned char **entry_table;
    unsigned char *cursor;
    unsigned char workspace[0x800];
} MsgBuffer;

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

typedef struct MsgQueueInfo {
    short count;
    short lines;
    short width;
} MsgQueueInfo;

void MSG_queueGetInfo(MsgQueue *queue, MsgQueueInfo *info, unsigned char *cursor);
MsgBuffer *MBUF_create(int entry_size, int entry_count);

#endif /* INCLUDE_MAIN_MSG_INIT_H */
