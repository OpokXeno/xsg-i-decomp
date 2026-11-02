/*
 * OV02 original TU 12: 0x00a0c488..0x00a0de20 (19 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/menu_top_command_check.h"
#include "main/open_close_main.h"
#include "main/xgl_cd.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"
#include "main/xgl_sound.h"
#include "main/xgl_2.h"
#include "main/xgl_jpeg.h"
#include "ov02/ov02.h"
#include "ov02/tya_uml.h"

/* Work buffer tyaUmlDispInit hands to tyaUmlDispInit2: it is also the base of the 1 MiB file area. */
#define TYA_UML_WORK_BUFFER 0x01000000
#define TYA_UML_FILE_AREA_SIZE 0x100000
#define TYA_UML_SECTOR_SIZE 2048
/* Bytes reserved below the text file for the filtered identifier table tbl. */
#define TYA_UML_FILTERED_TABLE_SIZE 0x800
#define TYA_UML_IMAGE_COUNT 8
/* Bytes of the text header copied in front of the text stream. */
#define TYA_UML_HEADER_SIZE 96
#define TYA_UML_ROW_HEIGHT 24
/* Entries of the index bar disp_index prints. */
#define TYA_UML_INDEX_COUNT 26

/* Entries of the identifier tables: id - TYA_UML_ID_BASE is the entry id. */
#define TYA_UML_ID_BASE 9000
#define TYA_UML_TABLE_SEPARATOR (-1)
#define TYA_UML_TABLE_END (-2)
#define TYA_UML_TABLE_LOCKED (-3)

/* tyaUmlDatabaseMain steps. */
#define TYA_UML_DB_INIT 0
#define TYA_UML_DB_LOAD_HEADER 1
#define TYA_UML_DB_LIST 2
#define TYA_UML_DB_VIEW_TEXT 3
#define TYA_UML_DB_CLOSE 4
#define TYA_UML_DB_CLOSE_2 5
#define TYA_UML_DB_CLOSE_3 6

/* Rows scrolled by one step of the text view. */
#define TYA_UML_SCROLL_STEP 8

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
/* The 32-byte request block xglJpegDecode reads: source and destination
 * pointers, the decoded size written back at +0x14/+0x16, and the flag byte
 * at +0x1b that tyaUmlDispLoad sets before the decode. */
typedef struct TyaUmlJpegRequest {
    u8 *source;
    u8 *destination;
    u8 unmodeled_08[0x0C];
    s16 width;
    s16 height;
    u8 unmodeled_18[0x03];
    u8 scanFlag;
    u8 unmodeled_1c[0x04];
} TyaUmlJpegRequest;
/* One 8-byte entry of the texture table that starts 32 bytes into the file. */
typedef struct TyaUmlTableEntry {
    u32 dataOffset;
    u16 originX;
    u16 originY;
} TyaUmlTableEntry;
/* The 16-byte VIF header written at the start of each texture packet. */
typedef struct TyaUmlTextureHeader {
    u32 dmaTag;
    u32 vifWord;
    u32 reserved08;
    u32 reserved0c;
} TyaUmlTextureHeader;
/* The 96-byte header block copied from the file to the buffer. */
typedef struct TyaUmlHeader {
    u8 unmodeled_00[0x60];
} TyaUmlHeader;
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

extern unsigned char SaveData[];
extern char name_0[0x12];
extern const unsigned char D_00A13310[4];
extern u64 TestEnv_2[14];
extern u64 InitEnv_3[4];
extern int TestEnv_4[24];
extern int TestEnv_5[16];
extern int TestEnv_6[28];
/* Defined in src/main/xgl_flags.c. */
extern int xglFlagsGet(int bit_offset, int bit_count);
extern const char D_00A133B0[24];
/* Font control streams: the index-bar header, the highlighted and the normal
 * entry colour, the two-byte entry labels (through idx.7) and the suffix
 * template msg.8, whose bytes 2 and 3 take the high and low suffix byte. */
extern const char D_00A13400[16];
extern const char D_00A13410[8];
extern const char D_00A13418[8];
extern const char *idx_7;
extern char msg_8[5];

static void tyaUmlDispType3(TyaUmlDispParam *parameter);
static void tyaUmlDispType3Sub0(FontDrawCursor *cursor, void *argument);

static u8 *buffer;
static s16 base_tbl[0x140];

static s16 *tbl;

static void mark(TyaUmlParser *parser, TyaUmlDispLine *line)
{
    line->marks[line->markCount] = line->baseColumn + (parser->column - (line->column - parser->lineStartColumn));
    line->markCount++;
}

static void tail(TyaUmlParser *parser, TyaUmlDispLine *line)
{
    line->tailColumn = line->baseColumn + (parser->column - (line->column - parser->lineStartColumn));
}

static int tyaUmlDispLoad(TyaUmlDispParam *parameter)
{
    TyaUmlJpegRequest request;
    TyaUmlTableEntry *table;
    TyaUmlTextureHeader *packet;
    unsigned short resource_id;
    unsigned int number;
    u32 owner;
    u8 *file;
    u8 *out;
    const u8 *cursor;
    u8 c;
    u8 code;
    u32 bold;
    u32 shake;
    int count;
    int size;
    int blockSize;
    int bytes;
    int qwc;
    int i;

    {
        int slot;

        for (slot = TYA_UML_IMAGE_COUNT - 1; slot >= 0; slot--)
            image[slot].environment = 0;
    }

    resource_id = parameter->resource_id;
    parameter->render_pending = 0;
    parameter->render_scrolling = 0;

    number = resource_id;
    name_0[12] = '0' + number % 10;
    number /= 10;
    name_0[11] = '0' + number % 10;
    number /= 10;
    name_0[10] = '0' + number % 10;
    number /= 10;
    name_0[9] = '0' + number % 10;

    out = buffer;
    size = xglCdGetFileSize(name_0);
    blockSize = (size + TYA_UML_SECTOR_SIZE - 1) & -TYA_UML_SECTOR_SIZE;
    file = buffer - blockSize + TYA_UML_FILE_AREA_SIZE;
    xglCdReadFile(name_0, file, 0, 1);

    *(TyaUmlHeader *)out = *(TyaUmlHeader *)file;
    out += TYA_UML_HEADER_SIZE;
    cursor = file + TYA_UML_HEADER_SIZE;
    owner = (u32)parameter;
    bold = 0;
    shake = 0;

    for (;;) {
        if (*cursor == 0) {
            break;
        }
        if (*cursor < 32) {
            count = xglFontGetSPcodeSize(*cursor, cursor);

            while (count >= 0) {
                *out++ = *cursor;
                cursor++;
                count--;
            }
        } else if (*cursor == '@') {
            cursor++;
            code = *cursor++;
            switch (code) {
            case '(':
                if ((*(unsigned short *)(SaveData + 0x102e2) & 0x10) == 0)
                    shake = 1;
                break;
            case ')':
                shake = 0;
                break;
            case '<':
                if ((*(unsigned short *)(SaveData + 0x102e2) & 0x30) != 0x30)
                    bold = 1;
                break;
            case '>':
                bold = 0;
                break;
            case '*': {
                u32 callback = (u32)mark;

                out[0] = OV02_FONT_OP_CALLBACK;
                out[1] = callback >> 16;
                out[2] = callback >> 8;
                out[3] = callback;
                out[4] = owner >> 24;
                out[5] = owner >> 16;
                out[6] = owner >> 8;
                out[7] = owner;
                out += 8;
                break;
            }
            }
        } else if (shake != 0 || bold != 0) {
            int index;

            out[0] = OV02_FONT_OP_EFFECT_PACKET;
            out[1] = -125;
            out[2] = -125;
            out[3] = 87;
            out[4] = 8;
            out[5] = -120;
            out[6] = -1;
            out[7] = -11;
            cursor++;
            index = xglSRand() & 3;
            out[8] = D_00A13310[index];
            out[9] = 19;
            out[10] = 0;
            out[11] = 0;
            out += 12;
        } else {
            *out++ = *cursor;
            cursor++;
        }
    }

    out[0] = OV02_FONT_OP_CALLBACK;
    out[1] = (u32)tail >> 16;
    out[2] = (u32)tail >> 8;
    out[3] = (u32)tail;
    out[4] = owner >> 24;
    out[5] = owner >> 16;
    out[6] = owner >> 8;
    out[7] = owner;
    out[8] = 0;
    out = (u8 *)(((u32)out + 24) & -16);

    table = (TyaUmlTableEntry *)(file + 32);
    for (i = 0; i < TYA_UML_IMAGE_COUNT; i++) {
        if (table[i].dataOffset != 0) {
            memset(&request, 0, 32);
            request.source = file + table[i].dataOffset;
            request.destination = out + 16;
            request.scanFlag = 1;
            xglJpegDecode(&request);

            image[i].x = table[i].originX;
            image[i].y = table[i].originY;
            bytes = request.width * request.height * 2;
            qwc = bytes / 16;

            packet = (TyaUmlTextureHeader *)out;
            packet->dmaTag = 0x8000 + qwc;
            packet->vifWord = 0x08000000;
            packet->reserved08 = 0;
            packet->reserved0c = 0;
            image[i].environment = out;
            out += bytes;
            out = (u8 *)(((u32)out + 15) & -16);
            image[i].width = request.width;
            image[i].height = request.height;
            image[i].count = qwc + 1;
        }
    }

    parameter->loaded_resource_id = parameter->resource_id;
    parameter->bufferUsed = out - buffer;
    return 0;
}

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
 * texture_trans's VIF direct-data scratch packet (GCC names it TestEnv.1);
 * only element 8 (the width/height quadword texture_trans writes before
 * each transfer) is touched here, so the remaining template bytes are
 * supplied by the reference image, not generated here.
 */
static u64 TestEnv_1_00A108B0[12] = {
    0, 0x5100000500000000ULL, 0x1000000000000004ULL, 14,
    0x0208380000000000ULL, 0x50, 0, 0x51, 0, 0x52, 0, 0x53,
};

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

static void tyaUmlDispType3Sub0(FontDrawCursor *cursor, void *argument)
{
    XglPacket **packet = &cursor->packet;
    TyaUmlDispParam *parameter = argument;
    TyaUmlImage *first = &image[0];
    u8 *work = buffer;
    int rect_left = parameter->render_x - OV02_GS_ORIGIN_X;
    int rect_top = parameter->render_y - OV02_GS_ORIGIN_Y;
    int rect_bottom = rect_top + parameter->render_height - 1;
    int rect_right = rect_left + parameter->render_width - 1;
    int row;
    int column;
    int i;
    u16 height;

    TestEnv_2[12] = (u64)rect_left | ((u64)rect_right << 16) |
                    ((u64)rect_top << 32) | ((u64)rect_bottom << 48);
    sceVif1PkAddDirectDataN(*packet, TestEnv_2, 7);

    if (first->environment != 0) {
        texture_trans(packet, first);
        TestEnv_4[4] = work[4];
        TestEnv_4[5] = work[5];
        TestEnv_4[6] = work[6];
        TestEnv_4[7] = work[7];
        TestEnv_4[8] = 0;
        TestEnv_4[9] = 0;
        TestEnv_4[14] = (int)parameter->color;
        TestEnv_4[16] = (s16)first->width << 4;
        height = first->height;
        TestEnv_4[17] = (s16)height << 4;
        TestEnv_4[22] = TestEnv_4[14];
        sceVif1PkAddDirectDataN(*packet, InitEnv_3, 2);

        height = first->height;
        row = -(parameter->clip_y % (s16)height);
        if (row < parameter->render_height) {
            do {
                TestEnv_4[13] = (parameter->render_y + row) << 4;
                TestEnv_4[21] = TestEnv_4[13] + ((s16)first->height << 4);
                column = -(parameter->clip_x % (s16)first->width);
                while (column < parameter->render_width) {
                    int left = (parameter->render_x + column) << 4;

                    TestEnv_4[12] = left;
                    TestEnv_4[20] = left + ((s16)first->width << 4);
                    sceVif1PkAddDirectDataN(*packet, TestEnv_4, 6);
                    column += (s16)first->width;
                }
                row += (s16)first->height;
            } while (row < parameter->render_height);
        }
    } else {
        TestEnv_5[4] = work[4];
        TestEnv_5[5] = work[5];
        TestEnv_5[6] = work[6];
        TestEnv_5[7] = work[7];
        TestEnv_5[8] = parameter->render_x << 4;
        TestEnv_5[9] = parameter->render_y << 4;
        TestEnv_5[10] = (int)parameter->color;
        TestEnv_5[12] = (parameter->render_x + parameter->render_width) << 4;
        TestEnv_5[13] = (parameter->render_y + parameter->render_height) << 4;
        TestEnv_5[14] = (int)parameter->color;
        sceVif1PkAddDirectDataN(*packet, TestEnv_5, 4);
    }

    for (i = 1; i < TYA_UML_IMAGE_COUNT; i++) {
        TyaUmlImage *tile = &image[i];
        int dx;
        int dy;
        int end_x;
        int end_y;

        if (tile->environment == 0)
            continue;
        dx = tile->x - parameter->clip_x;
        dy = tile->y - parameter->clip_y;
        end_x = dx + (s16)tile->width;
        end_y = dy + tile->height;
        if (parameter->render_width < dx || end_x < 0 ||
            parameter->render_height < dy || end_y < 0)
            continue;

        texture_trans(packet, tile);
        TestEnv_6[16] = (parameter->render_x - parameter->clip_x + tile->x) << 4;
        TestEnv_6[17] = (parameter->render_y - parameter->clip_y + tile->y) << 4;
        TestEnv_6[18] = parameter->color + 1;
        TestEnv_6[20] = (s16)tile->width << 4;
        TestEnv_6[21] = (s16)tile->height << 4;
        TestEnv_6[24] = TestEnv_6[16] + TestEnv_6[20];
        TestEnv_6[25] = TestEnv_6[17] + TestEnv_6[21];
        TestEnv_6[26] = TestEnv_6[18];
        sceVif1PkAddDirectDataN(*packet, TestEnv_6, 7);
    }

    xglFontReloadTexture(cursor, 1);
}

static void tyaUmlDispType3Sub1(FontDrawCursor *cursor, void *argument)
{
    u64 packed_dimensions;
    u64 *command_words;
    s16 width = sRender.reso.frame_width;
    s16 height = sRender.reso.frame_height;

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
    int render_x = parameter->render_x - OV02_GS_ORIGIN_X;
    int render_y = parameter->render_y - OV02_GS_ORIGIN_Y;
    int x;
    int y;

    parameter->render_pending = 0;
    xglFontPrintExtFunc((unsigned int)(parameter->color + 1),
                        tyaUmlDispType3Sub0, parameter);
    xglFontPrint(0, 0, (int)(parameter->color + 15), D_00A13380);

    x = render_x - parameter->clip_x;
    y = render_y - parameter->clip_y;
    xglFontPrint(x, y, (int)(parameter->color + 15), buffer + TYA_UML_HEADER_SIZE);
    xglFontPrintExtFunc((unsigned int)(parameter->color + 1),
                        tyaUmlDispType3Sub1, parameter);
}

int tyaUmlDispMain(TyaUmlDispParam *parameter)
{
    if (parameter == 0)
        return -1;

    if (parameter->resource_id < 0) {
        memset(buffer, 0, 100);
        memset(image, 0, sizeof(image));
        tyaUmlDispType3(parameter);
        return 0;
    }
    if (parameter->resource_id != parameter->loaded_resource_id)
        return tyaUmlDispLoad(parameter);

    tyaUmlDispType3(parameter);
    return 0;
}

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

    while (*entry != TYA_UML_TABLE_END) {
        s16 value = *entry;

        if (value != TYA_UML_TABLE_SEPARATOR && value != TYA_UML_TABLE_LOCKED) {
            index++;
            if (value - TYA_UML_ID_BASE == target_id) {
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

    while (*entry != TYA_UML_TABLE_END) {
        s16 value = *entry;

        if (value == TYA_UML_TABLE_SEPARATOR)
            index++;
        if (value != TYA_UML_TABLE_SEPARATOR && value != TYA_UML_TABLE_LOCKED && value - TYA_UML_ID_BASE == target_id)
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

    while (*entry != TYA_UML_TABLE_END) {
        s16 value = *entry;

        if (value == TYA_UML_TABLE_SEPARATOR)
            n--;
        if (value != TYA_UML_TABLE_SEPARATOR && value != TYA_UML_TABLE_LOCKED) {
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
            *destination = TYA_UML_TABLE_SEPARATOR;
        }
        destination++;

    do {
    skip_destination:
        if ((u8)*source == 0)
            goto done;
    } while (*source++ != '\n');
    }

done:
    *destination = TYA_UML_TABLE_END;
}

static void dbheader_load(TyaUmlDatabase *database)
{
    int size;
    int blockSize;
    u8 *file;
    s16 *dest;
    u8 *out;
    const s16 *source;
    int lines = 0;
    int found = 0;

    source = base_tbl;
    out = buffer;
    dbfileno_load();
    size = xglCdGetFileSize(D_00A133B0);
    blockSize = (size + TYA_UML_SECTOR_SIZE - 1) & -TYA_UML_SECTOR_SIZE;
    file = buffer - blockSize + TYA_UML_FILE_AREA_SIZE;
    xglCdReadFile(D_00A133B0, file, 0, 1);
    tbl = (s16 *)(file - TYA_UML_FILTERED_TABLE_SIZE);
    dest = tbl;

    memset(out, 0, 100);
    out += TYA_UML_HEADER_SIZE;
    memset(image, 0, sizeof(image));

    out[0] = 8;
    out[1] = 8;
    out[3] = 24;
    out[2] = 0;
    out += 4;

    for (;;) {
        s16 value;

        while (*source == TYA_UML_TABLE_SEPARATOR) {
            *dest++ = *source;
            source++;
        }
        value = *source;
        if (value == TYA_UML_TABLE_END) {
            *dest = value;
            *out = 0;
            database->foundCount = found;
            database->lastEntry = lines - 1;
            return;
        } else {
            u8 *text = find_text((TyaUmlTextRecord *)file, *source);

            if (text != 0) {
                *dest++ = *source;
                while (*text != 0) {
                    u8 ch = *text;

                    if (ch < 32) {
                        int count = xglFontGetSPcodeSize(ch, text);

                        for (; count >= 0; count--) {
                            *out++ = *text++;
                        }
                    } else {
                        *out++ = *text++;
                    }
                }
                found++;
            } else {
                *dest++ = TYA_UML_TABLE_LOCKED;
            }
            *out++ = '\n';
            source++;
            lines++;
        }
    }
}

/* Prints the 26-entry index bar, highlighting database->indexCursor. */
static void disp_index(TyaUmlDatabase *database, int suffix)
{
    TyaUmlDispParam *display = &database->display;
    int ot = (int)(display->color + 4);
    int i;

    xglFontPrint(display->render_x + display->render_width - 0x824,
                 display->render_y + display->render_height - 0x71C,
                 ot, D_00A13400);
    for (i = 0; i < TYA_UML_INDEX_COUNT; i++) {
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

int tyaUmlDatabaseMain(TyaUmlDatabase *database)
{
    ECursolMainData *cursorB;
    ECursolMainData *cursorA;
    MenuTopMenuPanel *menu;
    WindowDX *window;

    if (database == 0)
        return -1;

    window = database->window;
    menu = database->menu;
    cursorA = database->cursorA;
    cursorB = database->cursorB;

    switch (database->state) {
    case TYA_UML_DB_INIT:
        database->listCursor = 0;
        database->indexCursor = 0;
        WindowDXSet(window);
        window->x = database->display.render_x - OV02_GS_ORIGIN_X;
        window->style = (int)(database->display.color + 1);
        window->y = database->display.render_y - OV02_GS_ORIGIN_Y;
        window->width = database->display.render_width;
        window->height = database->display.render_height;
        window->state = WINDOWDX_STATE_OPEN;
        MenuSelectWindow(window, menu);
        WindowDXMain(window);
        menu->marker.active = 1;
        menu->rows = 0;
        menu->text = 0;
        eCursolSet(cursorA, 1);
        cursorA->state = E_MODE_SHOW_START;
        cursorA->x = window->x + window->width - 17;
        cursorA->y = window->y + 4;
        cursorA->work = (int)(database->display.color + 2);
        eCursolSet(cursorB, 2);
        cursorB->state = E_MODE_SHOW_START;
        cursorB->work = (int)(database->display.color + 2);
        cursorB->x = window->x + window->width - 17;
        cursorB->y = window->y + window->height - 19;
        database->topRow = 0;
        database->rowsPerPage = database->display.render_height / TYA_UML_ROW_HEIGHT;
        /* fall through */
    case TYA_UML_DB_LOAD_HEADER:
        dbheader_load(database);
        database->state = TYA_UML_DB_LIST;
        /* fall through */
    case TYA_UML_DB_LIST:
        {
            s16 origCursor;
            s16 indexCursor;
            int direction;
            int limit;
            s16 sel;
            s16 fileNo;
            int suffix;
            u16 pressed;
            u16 repeat;

            origCursor = database->listCursor;
            if (PadData.mappedRepeat & PAD_UP)
                database->listCursor--;
            if (PadData.mappedRepeat & PAD_DOWN)
                database->listCursor++;

            while (0 < database->topRow && database->listCursor < database->topRow + 1)
                database->topRow--;

            while (database->topRow + database->rowsPerPage <= database->lastEntry
                && database->topRow + database->rowsPerPage - 2 < database->listCursor)
                database->topRow++;

            if (database->listCursor < 0)
                database->listCursor = 0;
            if (database->listCursor > database->lastEntry)
                database->listCursor = database->lastEntry;
            if (database->listCursor != origCursor)
                xglSoundEffectNormalDirect(SE_CURSOR);

            menu->cursor = database->listCursor - database->topRow;
            database->display.clip_y = database->topRow * TYA_UML_ROW_HEIGHT;
            database->indexCursor = get_indexno(database->listCursor);
            fileNo = get_fileno(database->listCursor, &suffix);
            database->fileIndex = suffix;

            if (PadData.half_2a & PAD_CIRCLE) {
                if (fileNo >= 0) {
                    window->state = WINDOWDX_STATE_READY;
                    xglSoundEffectNormalDirect(SE_DECIDE);
                    database->display.resource_id = fileNo;
                    database->display.clip_y = 0;
                    tyaUmlDispLoad(&database->display);
                    database->state = TYA_UML_DB_VIEW_TEXT;
                    return 0;
                }
                xglSoundEffectNormalDirect(SE_BUZZER);
            }
            if (PadData.half_2a & PAD_CROSS) {
                database->indexCursor = TYA_UML_INDEX_COUNT;
                database->state = TYA_UML_DB_CLOSE;
                window->state = WINDOWDX_STATE_CLOSE;
            }

            pressed = PadData.mappedPressed;
            repeat = PadData.mappedRepeat;
            indexCursor = database->indexCursor;
            limit = ((pressed & (PAD_LEFT | PAD_RIGHT | PAD_SQUARE)) == 0) ? indexCursor : -1;
            direction = 0;
            if (PadData.half_2a & PAD_SQUARE)
                direction = 1;
            if (pressed & PAD_RIGHT)
                direction = 1;
            if (pressed & PAD_LEFT)
                direction = -1;
            if (repeat & PAD_RIGHT)
                direction = 1;
            if (repeat & PAD_LEFT)
                direction = -1;

            while (direction != 0) {
                indexCursor = indexCursor + direction;
                if (indexCursor < 0)
                    indexCursor = TYA_UML_INDEX_COUNT - 2;
                if (indexCursor >= TYA_UML_INDEX_COUNT - 1)
                    indexCursor = 0;
                sel = get_selectno(indexCursor);
                if (sel >= 0) {
                    database->listCursor = sel;
                    break;
                }
            }

            if (limit >= 0 && ((direction > 0 && indexCursor < limit) || (direction < 0 && limit < indexCursor))) {
                indexCursor = limit;
                database->listCursor = get_selectno(indexCursor);
            }
            if (indexCursor != database->indexCursor)
                xglSoundEffectNormalDirect(SE_CURSOR);
            database->indexCursor = indexCursor;

            WindowDXMain(window);
            MenuSelectWindow(window, menu);
            if (window->state == WINDOWDX_STATE_READY) {
                if (database->topRow != 0)
                    eCursolMain(cursorA);
                if (database->topRow + database->rowsPerPage - 1 != database->lastEntry)
                    eCursolMain(cursorB);
                tyaUmlDispType3(&database->display);
            }
            disp_index(database, 0);
            break;
        }
    case TYA_UML_DB_VIEW_TEXT:
        {
            s16 max;

            max = (s16)(database->display.clip_height - database->display.render_height);
            if (max < 0)
                max = 0;
            if (PadData.mappedRepeat & PAD_UP)
                database->display.clip_y -= TYA_UML_SCROLL_STEP;
            if (PadData.mappedRepeat & PAD_DOWN)
                database->display.clip_y += TYA_UML_SCROLL_STEP;
            if (database->display.clip_y < 0)
                database->display.clip_y = 0;
            if (database->display.clip_y > max)
                database->display.clip_y = max;
            if (PadData.half_2a & PAD_CROSS) {
                xglSoundEffectNormalDirect(SE_CANCEL);
                database->display.resource_id = -1;
                database->state = TYA_UML_DB_LOAD_HEADER;
            }
            tyaUmlDispMain(&database->display);
            if (database->display.clip_y != 0)
                eCursolMain(cursorA);
            if (database->display.clip_y != max)
                eCursolMain(cursorB);
            return 0;
        }
    case TYA_UML_DB_CLOSE:
    case TYA_UML_DB_CLOSE_2:
    case TYA_UML_DB_CLOSE_3:
        WindowDXMain(window);
        MenuSelectWindow(window, menu);
        disp_index(database, database->state - 3);
        database->state = database->state + 1;
        return 0;
    default:
        return 1;
    }
    return 0;
}

int tyaUmlDispParamReset(TyaUmlDispParam *parameter, int mode)
{
    parameter->resource_id = -1;
    parameter->render_x = 0;
    parameter->render_y = 0;
    parameter->render_width = OV02_SCREEN_WIDTH;
    parameter->render_height = OV02_SCREEN_HEIGHT;
    parameter->color = 0x00FFFF00ULL;
    parameter->clip_x = 0;
    parameter->clip_y = 0;
    parameter->clip_width = OV02_SCREEN_WIDTH;
    parameter->clip_height = OV02_SCREEN_HEIGHT;
    parameter->loaded_resource_id = -1;
    return 0;
}

TyaUmlImage image[8] = {{0}};

void tyaUmlDispInit2(u8 *work_buffer)
{
    int i;

    buffer = work_buffer;
    for (i = TYA_UML_IMAGE_COUNT - 1; i >= 0; i--)
        image[i].environment = 0;
}

void tyaUmlDispInit(void)
{
    tyaUmlDispInit2((u8 *)TYA_UML_WORK_BUFFER);
}
