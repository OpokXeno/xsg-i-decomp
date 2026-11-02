#ifndef MAIN_TWSYS_INIT_PRIVATE_H
#define MAIN_TWSYS_INIT_PRIVATE_H

#include "main/xgl_font.h"

/* Declaration proposal owned by main/tu142, msg_init.c.
 * The queue is embedded in text windows at offset 0xAC. */
#include "main/msg_init.h"
#include "main/ew_sprt_set_cursor_uv.h"
#include "main/xgl_pad.h"

extern XglPadRecord PadData[2];

int MSG_queuePop(MsgQueue *queue, unsigned char *output, int glyphLimit);
void MSG_queueReset(MsgQueue *queue, unsigned int flags);
char *strcpy(char *destination, const char *source);

/* MBUF storage and allocation are defined by main/tu142, msg_init.c. */
MsgBuffer *MBUF_create2(void);

/* EW slot types are defined by main/tu155, ew_sprt_set_cursor_uv.c. */
EwWidget *EW_create(int index, int type);

int EW_addComponent(EwContainerWidget *widget, int index, EwWidget *child);

#endif
