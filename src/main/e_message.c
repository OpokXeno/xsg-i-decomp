#include "common.h"

/* Number of message sprites currently allocated by the print routines. */
extern int msg_spr_count;

void eMessageSpriteReset(void)
{
    msg_spr_count = 0;
}

void eMessageDrawType00(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageHalfSpaseCheck);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageNextGyou);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageNextGyouMaxGet);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageNextWaitKeySearch);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageDrawType01);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageMain);

/*
 * The message object eMessageModeChange, eMessageDraw and eMessageMain
 * receive.  Callers build it on their stack and it extends well past the
 * mode byte (eMessageMain also reads +0x00 and +0x18, TextTest fills +0x04
 * to +0x1C); only the mode byte eMessageModeChange writes is modelled.
 * eMessageMain dispatches on it with lbu, so it is unsigned.
 */
typedef struct EMessageParam {
    unsigned char unmodeled_00;
    unsigned char mode;
} EMessageParam;

extern void eMessageMain(EMessageParam *message);

void eMessageModeChange(EMessageParam *message, unsigned char mode)
{
    message->mode = mode;
}

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageSet);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageTextChange);

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageNextPage);

void eMessageDraw(EMessageParam *message)
{
    eMessageMain(message);
}

/* Current write position of the message text being built. */
extern char *MessageCpyEnd;

extern void eMessageCat(char *src);

void eMessageCpy(char *dst, char *src)
{
    MessageCpyEnd = dst;
    eMessageCat(src);
}

INCLUDE_ASM("asm/main/nonmatchings/e_message", eMessageCat);
