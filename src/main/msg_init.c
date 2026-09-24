#include "common.h"

extern int ctrlCodeFlags;

void MSG_init(void)
{
    ctrlCodeFlags = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_print2);

void MSG_send(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_getSize);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_dump);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queueReset);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queueGetInfo);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queuePop);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_copyln);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queuePush);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", matchBracket);

/*
 * Placeholder entry of the `mfunc` control-code table (listed twice there).
 * MSG_convert calls every entry as handler(out, argc, args) and continues
 * writing at the returned pointer; the other MIF2_* handlers emit their
 * control bytes at `out` and return the end. This one emits nothing: it only
 * walks the argument count and hands `out` back unchanged.
 */
char *MIF2_dummy(char *out, int argc)
{
    int i;

    for (i = 0; i < argc; i++) {
    }
    return out;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_setArgs);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", rubyFont);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_ruby);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_font);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_color);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_label);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_clear);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_close);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_waitkey);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_wait);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_gaiji);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_code);

int strcmp(const char *, const char *);

/*
 * One row of the mfunc[] control-code table, which a NULL name terminates.
 * MSG_convert (still asm) calls the matched row's handler as
 * handler(out, argc), the same two parameters the placeholder row's own
 * MIF2_dummy above declares, and continues writing at the pointer it returns.
 */
typedef struct MsgFuncEntry {
    char *(*handler)(char *out, int argc);
    const char *name;
} MsgFuncEntry;

extern MsgFuncEntry mfunc[];

/*
 * Looks up name in the mfunc[] control-code table and returns the matching
 * row, or NULL if the table is empty or no row's name matches.
 */
static MsgFuncEntry *findMsgFunc(const char *name)
{
    MsgFuncEntry *entry = mfunc;

    if (entry->name != 0) {
        do {
            if (strcmp(name, entry->name) == 0) {
                return entry;
            }
            entry++;
        } while (entry->name != 0);
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_convert);

/*
 * One record of the 8-slot mbuf pool msg_buffer reserves (2064 bytes each,
 * 8 * 2064 = msg_buffer's own 0x4080 size). MBUF_dispose (below) evidences
 * the leading word as a plain int, the same field MBUF_init clears here for
 * every slot.
 */
typedef struct MsgBuffer {
    int count;
    unsigned char unmodeled_04[2064 - 4];
} MsgBuffer;

extern MsgBuffer msg_buffer[8];

void MBUF_init(void)
{
    int slot;

    slot = 7;
    do {
        msg_buffer[slot].count = 0;
        slot--;
    } while (slot >= 0);
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MBUF_create2);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MBUF_create);

/*
 * MBUF_init/MBUF_create (not yet recovered) establish the rest of the mbuf
 * layout; only the leading word is evidenced here.
 */
void MBUF_dispose(int *mbuf)
{
    mbuf[0] = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", _copyCTRLCode);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", _skipCTRLCode);
