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

int eMessageHalfSpaseCheck(unsigned char *text)
{
    unsigned char character = *text++;
    int half_spaces = 0;

    while ((signed char)character == ' ') {
        character = *text++;
        half_spaces++;
    }
    return half_spaces;
}

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

typedef struct EMessagePageState {
    unsigned char flags;
    unsigned char mode;
    unsigned char unmodeled_02[26];
    unsigned char page_available;
    unsigned char page_index;
    unsigned char page_count;
} EMessagePageState;

int eMessageNextPage(EMessagePageState *message, int reset_page)
{
    int next_page;
    unsigned char wrapped_page;
    unsigned char next_mode;

    if (message->page_available != 0) {
        if (reset_page == 0) {
            if ((message->flags & 0x80) != 0) {
                next_page = message->page_index + 1;
                wrapped_page = next_page;
                message->page_index = next_page;
                if (message->page_count < wrapped_page) {
                    message->page_index = message->page_count;
                    return 0;
                }
                next_mode = 34;
            } else {
                next_mode = 32;
            }
            message->mode = next_mode;
            return 1;
        } else {
            message->page_index = 0;
        }
    }
    return 0;
}

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
