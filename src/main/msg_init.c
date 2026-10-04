#include "common.h"

typedef struct Mif2Argument {
    int type;
    union {
        int number;
        const char *text;
    } value;
} Mif2Argument;

static int ctrlCodeFlags = 0;

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
char *MIF2_dummy(char *out, int argc, const Mif2Argument *args)
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

char *MIF2_gaiji(char *out, int argc, const Mif2Argument *args)
{
    int glyph = args[0].value.number + 160;
    char *end = &out[2];

    out[0] = -83;
    out[1] = glyph;
    *end = '\0';
    return end;
}

char *MIF2_code(char *out, int argc, const Mif2Argument *args)
{
    char *end = &out[1];
    unsigned char code = args[0].value.number;

    out[0] = code;
    end[0] = '\0';
    return end;
}

int strcmp(const char *, const char *);

/*
 * One row of the mfunc[] control-code table, which a NULL name terminates.
 * MSG_convert (still asm) calls the matched row's handler as
 * handler(out, argc, args), including the placeholder MIF2_dummy row. That
 * handler ignores args and returns out unchanged.
 */
typedef struct MsgFuncEntry {
    char *(*handler)(char *out, int argc, const Mif2Argument *args);
    const char *name;
} MsgFuncEntry;

extern char *MIF2_clear(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_waitkey(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_wait(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_color(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_close(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_ruby(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_font(char *out, int argc, const Mif2Argument *args);
extern char *MIF2_label(char *out, int argc, const Mif2Argument *args);
extern const char D_004DA410[];
extern const char D_004DA408[];
extern const char D_004DA400[];
extern const char D_004DA3F8[];
extern const char D_004DA3F0[];
extern const char D_004DA3E8[];
extern const char D_004DA3E0[];
extern const char D_004DA3D8[];
extern const char D_004DA3D0[];
extern const char D_004DA3C8[];
extern const char D_004DA3C0[];
extern const char D_004DA3B8[];

static MsgFuncEntry mfunc[] = {
    {MIF2_clear, D_004DA410},
    {MIF2_waitkey, D_004DA408},
    {MIF2_dummy, D_004DA400},
    {MIF2_wait, D_004DA3F8},
    {MIF2_color, D_004DA3F0},
    {MIF2_close, D_004DA3E8},
    {MIF2_dummy, D_004DA3E0},
    {MIF2_ruby, D_004DA3D8},
    {MIF2_font, D_004DA3D0},
    {MIF2_label, D_004DA3C8},
    {MIF2_gaiji, D_004DA3C0},
    {MIF2_code, D_004DA3B8},
    {0, 0},
};

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
 * Each message buffer has a 16-byte header followed by its 0x800-byte
 * workspace. MBUF_create2 sets the active flag and capacity, and points the
 * cursor at the workspace. MBUF_create also uses the header's table pointer
 * for the per-entry pointers stored inside that workspace.
 */
typedef struct MsgBuffer {
    int active;
    unsigned int capacity;
    unsigned char **entry_table;
    unsigned char *cursor;
    unsigned char workspace[0x800];
} MsgBuffer;

MsgBuffer msg_buffer[8];

void MBUF_init(void)
{
    int slot;

    slot = 7;
    do {
        msg_buffer[slot].active = 0;
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



const char D_004DA3B8[8] = "code";

const char D_004DA3C0[8] = "gaiji";

const char D_004DA3C8[8] = "label";

const char D_004DA3D0[8] = "font";

const char D_004DA3D8[8] = "ruby";

const char D_004DA3E0[8] = "tips";

const char D_004DA3E8[8] = "close";

const char D_004DA3F0[8] = "color";

const char D_004DA3F8[8] = "wait";

const char D_004DA400[8] = "signal";

const char D_004DA408[8] = "waitkey";

const char D_004DA410[8] = "clear";
