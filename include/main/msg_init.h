#ifndef INCLUDE_MAIN_MSG_INIT_H
#define INCLUDE_MAIN_MSG_INIT_H

/* Message buffers: MSG_BUFFER_COUNT slots, each with a workspace of
 * MSG_BUFFER_WORKSPACE_SIZE bytes. */
#define MSG_BUFFER_COUNT 8
#define MSG_BUFFER_WORKSPACE_SIZE 0x800

typedef struct MsgBuffer {
    int active;
    unsigned int capacity;
    unsigned char **entry_table;
    unsigned char *cursor;
    unsigned char workspace[MSG_BUFFER_WORKSPACE_SIZE];
} MsgBuffer;

/* The one word MSG_queuePop stores for a control code: the argument value for
 * codes 0x80, 0x82 and 0x83, or the text pointer for code 0x85. */
typedef union MsgQueueLabel {
    int value;
    const unsigned char *text;
} MsgQueueLabel;

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
    MsgQueueLabel label;
} MsgQueue;

typedef struct MsgQueueInfo {
    short count;
    short lines;
    short width;
} MsgQueueInfo;

void MSG_queueGetInfo(MsgQueue *queue, MsgQueueInfo *info, unsigned char *cursor);
MsgBuffer *MBUF_create(int entry_size, int entry_count);

/* Functions and data used by other translation units. */
void MSG_init(void);
unsigned int MSG_convert(unsigned char *destination, int capacity, const char *text, int length);

#endif /* INCLUDE_MAIN_MSG_INIT_H */
