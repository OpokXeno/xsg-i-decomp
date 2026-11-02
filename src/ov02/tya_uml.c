/*
 * OV02 original TU 12: 0x00a0c488..0x00a0de20 (19 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_font.h"

typedef unsigned char u8;
typedef signed short s16;
extern int xglCdGetFileSize(const char *name);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
static u8 *buffer;
static s16 base_tbl[0x140];

static s16 *tbl;
extern void tyaUmlDispInit2(u8 *work_buffer);

typedef struct TyaUmlDispParam {
    int resource_id;
    u8 unmodeled_04[4];
    s16 render_x;
    s16 render_y;
    s16 render_width;
    s16 render_height;
    u64 color;
    s16 clip_x;
    s16 clip_y;
    s16 clip_width;
    s16 clip_height;
    int loaded_resource_id;
    u8 unmodeled_24[4];
    u8 render_pending;
} TyaUmlDispParam;

typedef struct TyaUmlRenderSize {
    u8 unmodeled_00[4];
    s16 width;
    s16 height;
} TyaUmlRenderSize;

extern TyaUmlRenderSize sRender;
static void tyaUmlDispType3(TyaUmlDispParam *parameter);
extern void tyaUmlDispType3Sub0(FontDrawCursor *cursor, void *argument);
/* Font control stream consumed by xglFontPrint; individual opcodes remain
 * unmodeled, but all 24 original bytes and the call-site type are known. */
static const char D_00A13380[24] = {
    0x0D, 0x00, 0x15, 0x02, 0x01, 0x0C, 0x0C, 0x80,
    0x80, 0x80, 0x15, 0x02, 0x02, 0x04, 0x19, 0x03,
    0x15, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
};
/* Original 24-byte database path owner at the immediately following address. */
const char db_fileno_path[24] = "data\\umn\\db_fileno.txt";

/*
 * The parser cursor mark() and tail() read from: only the two columns they
 * use are evidenced, so the rest of the struct stays unmodeled.
 */
typedef struct TyaUmlParser {
    u8 unmodeled_00[0x0A];
    s16 column;                /* 0x0A */
    u8 unmodeled_0c[0x06];
    s16 lineStartColumn;       /* 0x12 */
} TyaUmlParser;

/*
 * The display line mark() and tail() write into: only the fields those two
 * functions touch are evidenced, so the rest of the struct stays unmodeled.
 */
typedef struct TyaUmlDispLine {
    u8 unmodeled_00[0x0A];
    s16 column;                /* 0x0A */
    u8 unmodeled_0c[0x0E];
    s16 baseColumn;            /* 0x1A */
    u8 unmodeled_1c[0x02];
    s16 tailColumn;            /* 0x1E: written by tail() */
    u8 unmodeled_20[0x08];
    u8 markCount;              /* 0x28: number of entries stored in marks[] */
    u8 unmodeled_29[0x03];
    /*
     * 0x2C: boundary columns stored by mark(); bounded by the separate
     * halfword tyaUmlDatabaseMain writes at 0x50 of the same line.
     */
    s16 marks[18];
} TyaUmlDispLine;

static void mark(TyaUmlParser *parser, TyaUmlDispLine *line)
{
    line->marks[line->markCount] = line->baseColumn + (parser->column - (line->column - parser->lineStartColumn));
    line->markCount++;
}

static void tail(TyaUmlParser *parser, TyaUmlDispLine *line)
{
    line->tailColumn = line->baseColumn + (parser->column - (line->column - parser->lineStartColumn));
}

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", tyaUmlDispLoad);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data,
                                    int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);

/*
 * texture_trans's VIF direct-data scratch packet (GCC names it TestEnv.1);
 * only element 8 (the width/height quadword texture_trans writes before
 * each transfer) is touched here, so the remaining template bytes are
 * supplied by the reference image, not generated here.
 */
static u64 TestEnv_1_00A108B0[12] = {
    0, 0x5100000500000000ULL, 0x1000000000000004ULL, 14,
    0x0208380000000000ULL, 0x50, 0, 0x51, 0, 0x52, 0, 0x53,
};

/*
 * One 0x10-byte entry of the eight-slot texture table tyaUmlDispInit2
 * clears and texture_trans transfers through sceVif1PkRef: only the two
 * dimension fields, the environment pointer and the transfer count are
 * evidenced, so the rest of the record stays unmodeled.
 */
typedef struct TyaUmlImage {
    u8 unmodeled_00[4];
    u16 width;          /* 0x04: masked to an even value before use */
    s16 height;         /* 0x06 */
    void *environment;  /* 0x08: cleared by tyaUmlDispInit2 */
    int count;           /* 0x0C: sceVif1PkRef's transfer count */
} TyaUmlImage;

static void texture_trans(XglPacket **packet, TyaUmlImage *texture)
{
    long long dimensions;

    /*
     * Packs the masked width (low half, sign-extended) and height (high
     * half) into the scratch packet's second quadword.
     */
    dimensions = (long long)(s16)(texture->width & 0xFFFE) |
                 ((long long)texture->height << 32);
    TestEnv_1_00A108B0[8] = dimensions;
    sceVif1PkAddDirectDataN(*packet, TestEnv_1_00A108B0, 6);
    sceVif1PkCloseDirectHLCode(*packet);
    sceVif1PkRef(*packet, texture->environment, texture->count, 0,
                 texture->count | 0x51000000, 0);
    sceVif1PkCnt(*packet, 0);
    sceVif1PkOpenDirectHLCode(*packet, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", tyaUmlDispType3Sub0);

static void tyaUmlDispType3Sub1(FontDrawCursor *cursor, void *argument)
{
    u64 packed_dimensions;
    u64 *command_words;
    s16 width = sRender.width;
    s16 height = sRender.height;

    packed_dimensions = ((u64)(width - 1) << 16) |
                        ((u64)(height - 1) << 48);
    command_words = cursor->work.qwords;
    command_words[0] = 0x1000000000008001ULL;
    command_words[1] = 14;
    command_words[2] = packed_dimensions;
    command_words[3] = 64;
    sceVif1PkAddDirectDataN(cursor->packet, command_words, 2);
}

static void tyaUmlDispType3(TyaUmlDispParam *parameter)
{
    int render_x = parameter->render_x - 1792;
    int render_y = parameter->render_y - 1824;
    int x;
    int y;

    parameter->render_pending = 0;
    xglFontPrintExtFunc((unsigned int)(parameter->color + 1),
                        tyaUmlDispType3Sub0, parameter);
    xglFontPrint(0, 0, (int)(parameter->color + 15), D_00A13380);

    x = render_x - parameter->clip_x;
    y = render_y - parameter->clip_y;
    xglFontPrint(x, y, (int)(parameter->color + 15), buffer + 96);
    xglFontPrintExtFunc((unsigned int)(parameter->color + 1),
                        tyaUmlDispType3Sub1, parameter);
}

extern void *memset(void *dest, int value, unsigned int size);
extern int tyaUmlDispLoad(TyaUmlDispParam *parameter);
extern TyaUmlImage image[8];

int tyaUmlDispMain(TyaUmlDispParam *parameter)
{
    if (parameter == 0)
        return -1;

    if (parameter->resource_id < 0) {
        memset(buffer, 0, 100);
        memset(image, 0, 128);
        tyaUmlDispType3(parameter);
        return 0;
    }
    if (parameter->resource_id != parameter->loaded_resource_id)
        return tyaUmlDispLoad(parameter);

    tyaUmlDispType3(parameter);
    return 0;
}

/*
 * One record of the text archive find_text walks: `size` counts the bytes
 * after the size word itself (the next record starts at size + 4), and a zero
 * size ends the archive. Only the fields find_text reads are evidenced.
 */
typedef struct TyaUmlTextRecord {
    int size;                  /* 0x00 */
    int id;                    /* 0x04 */
    u8 unmodeled_08[0x0C];
    u16 flag;                  /* 0x14: event flag that unlocks the text */
    u8 unmodeled_16[0x52];
    u8 text[1];                /* 0x68 */
} TyaUmlTextRecord;

/* Defined in src/main/xgl_flags.c. */
extern int xglFlagsGet(int bit_offset, int bit_count);

/* Returns the text of record `id`, or 0 if it is missing or still locked. */
static u8 *find_text(TyaUmlTextRecord *record, int id)
{
    for (;;) {
        int size = record->size;

        if (size == 0)
            return 0;
        if (id == record->id) {
            u8 *text = record->text;

            if (xglFlagsGet(record->flag, 1) == 0)
                text = 0;
            return text;
        }
        record = (TyaUmlTextRecord *)((u8 *)record + size + 4);
    }
}

static int get_fileno(int target_id, int *file_index)
{
    s16 *entry = tbl;
    int index = 0;

    while (*entry != -2) {
        s16 value = *entry;

        if (value != -1 && value != -3) {
            index++;
            if (value - 9000 == target_id) {
                *file_index = index;
                return value;
            }
        }
        entry++;
    }

    *file_index = -1;
    return -1;
}

static int get_indexno(int target_id)
{
    s16 *entry = base_tbl;
    int index = 0;

    while (*entry != -2) {
        s16 value = *entry;

        if (value == -1)
            index++;
        if (value != -1 && value != -3 && value - 9000 == target_id)
            return index;
        entry++;
    }

    return -1;
}

/*
 * The inverse of get_indexno: walks base_tbl counting n down at every -1
 * separator and returns the running entry index once n reaches zero.
 */
static int get_selectno(int n)
{
    s16 *entry = base_tbl;
    int index = 0;

    while (*entry != -2) {
        s16 value = *entry;

        if (value == -1)
            n--;
        if (value != -1 && value != -3) {
            if (n == 0)
                return index;
            index++;
        }
        entry++;
    }

    return -1;
}

/* Parses db_fileno.txt into the base identifier table. */
static void dbfileno_load(void)
{
    u8 *source = buffer;
    s16 *destination = base_tbl;
    int length;

    length = xglCdGetFileSize(db_fileno_path);
    xglCdReadFile(db_fileno_path, source, 0, 1);
    source[length] = 0;

    while (*source != 0) {
        if ((u8)(*source - '0') < 10) {
            int value = 0;

            do {
                value = value * 10 + *source++ - '0';
            } while ((u8)(*source - '0') < 10);
            *destination = (s16)value;
        } else {
            if (*source != '-')
                goto skip_destination;
            *destination = -1;
        }
        destination++;

    do {
    skip_destination:
        if ((u8)*source == 0)
            goto done;
    } while (*source++ != '\n');
    }

done:
    *destination = -2;
}

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", dbheader_load);

/*
 * The database view tyaUmlDatabaseMain drives: it starts with the display
 * parameters it hands to tyaUmlDispMain/tyaUmlDispType3/tyaUmlDispLoad, so
 * the record embeds a TyaUmlDispParam. Only the index cursor disp_index
 * reads is modeled; the rest stays unmodeled.
 */
typedef struct TyaUmlDatabase {
    TyaUmlDispParam display;   /* 0x00 */
    u8 unmodeled_30[0x26];
    s16 indexCursor;           /* 0x56: highlighted entry of the index bar */
} TyaUmlDatabase;

/* Font control streams: the index-bar header, the highlighted and the normal
 * entry colour, the two-byte entry labels (through idx.7) and the suffix
 * template msg.8, whose bytes 2 and 3 take the high and low suffix byte. */
extern const char D_00A13400[16];
extern const char D_00A13410[8];
extern const char D_00A13418[8];
extern const char *idx_7;
extern char msg_8[5];
extern void xglFontPrintDirectOT(int ot, const char *text);

/* Prints the 26-entry index bar, highlighting database->indexCursor. */
static void disp_index(TyaUmlDatabase *database, int suffix)
{
    TyaUmlDispParam *display = &database->display;
    int ot = (int)(display->color + 4);
    int i;

    xglFontPrint(display->render_x + display->render_width - 0x824,
                 display->render_y + display->render_height - 0x71C,
                 ot, D_00A13400);
    for (i = 0; i < 26; i++) {
        if (i == database->indexCursor)
            xglFontPrintDirectOT(ot, D_00A13410);
        else
            xglFontPrintDirectOT(ot, D_00A13418);
        xglFontPrintDirectOT(ot, idx_7 + i * 2);
        if (suffix != 0) {
            msg_8[2] = suffix >> 8;
            msg_8[3] = suffix;
            xglFontPrintDirectOT(ot, msg_8);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", tyaUmlDatabaseMain);

int tyaUmlDispParamReset(TyaUmlDispParam *parameter)
{
    parameter->resource_id = -1;
    parameter->render_x = 0;
    parameter->render_y = 0;
    parameter->render_width = 512;
    parameter->render_height = 448;
    parameter->color = 0x00FFFF00ULL;
    parameter->clip_x = 0;
    parameter->clip_y = 0;
    parameter->clip_width = 512;
    parameter->clip_height = 448;
    parameter->loaded_resource_id = -1;
    return 0;
}

TyaUmlImage image[8] = {{0}};

void tyaUmlDispInit2(u8 *work_buffer)
{
    int i;

    buffer = work_buffer;
    for (i = 7; i >= 0; i--)
        image[i].environment = 0;
}

void tyaUmlDispInit(void)
{
    tyaUmlDispInit2((u8 *)0x01000000);
}
