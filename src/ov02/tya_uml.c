/*
 * OV02 original TU 12: 0x00a0c488..0x00a0de20 (19 functions)
 */
#include "common.h"
#include "shared.h"

typedef unsigned char u8;
typedef signed short s16;
extern int xglCdGetFileSize(const char *name);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern u8 *buffer;
extern s16 base_tbl[];
extern char db_fileno_path[];

extern s16 *tbl;
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

typedef struct TyaUmlGsPacket {
    XglPacket *packet;
    u8 unmodeled_04[0x28];
    u64 commands[4];
} TyaUmlGsPacket;

extern TyaUmlRenderSize sRender;
static void tyaUmlDispType3(TyaUmlDispParam *parameter);
extern void tyaUmlDispType3Sub0(void *context, void *argument);
extern void xglFontPrintExtFunc(unsigned int flags,
                                void (*draw)(void *context, void *argument),
                                void *argument);
extern const char D_00A13380[];

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
extern u64 TestEnv_1_00A108B0[12];

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

static void tyaUmlDispType3Sub1(void *context, void *argument)
{
    TyaUmlGsPacket *command = context;
    u64 packed_dimensions;
    u64 *command_words;
    s16 width = sRender.width;
    s16 height = sRender.height;

    packed_dimensions = ((u64)(width - 1) << 16) |
                        ((u64)(height - 1) << 48);
    command_words = command->commands;
    command_words[0] = 0x1000000000008001ULL;
    command_words[1] = 14;
    command_words[2] = packed_dimensions;
    command_words[3] = 64;
    sceVif1PkAddDirectDataN(command->packet, command_words, 2);
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

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", tyaUmlDispMain);

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", find_text);

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

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", get_indexno);

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", get_selectno);

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

INCLUDE_ASM("asm/nonmatchings/ov02/tya_uml", disp_index);

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

extern TyaUmlImage image[8];

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
