/*
 * OV02 original TU 12: 0x00a0c488..0x00a0de20 (19 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_font.h"
#include "main/xgl_2.h"
#include "main/xgl_jpeg.h"

typedef unsigned char u8;
typedef signed short s16;
extern int xglCdGetFileSize(const char *name);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
static u8 *buffer;
static s16 base_tbl[0x140];

static s16 *tbl;
extern void *memset(void *dest, int value, unsigned int size);
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
    int bufferUsed;
    u8 render_pending;
    u8 render_scrolling;
} TyaUmlDispParam;

typedef struct TyaUmlRenderSize {
    u8 unmodeled_00[4];
    s16 width;
    s16 height;
} TyaUmlRenderSize;

extern TyaUmlRenderSize sRender;
static void tyaUmlDispType3(TyaUmlDispParam *parameter);
static void tyaUmlDispType3Sub0(FontDrawCursor *cursor, void *argument);

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

/*
 * One 0x10-byte entry of the eight-slot texture table tyaUmlDispInit2
 * clears and texture_trans transfers through sceVif1PkRef: only the origin,
 * the two dimension fields, the environment pointer and the transfer count
 * are evidenced.
 */
typedef struct TyaUmlImage {
    s16 x;              /* 0x00: copied from the texture table entry at +4 */
    s16 y;              /* 0x02: copied from the texture table entry at +6 */
    u16 width;          /* 0x04: masked to an even value before use */
    s16 height;         /* 0x06 */
    void *environment;  /* 0x08: cleared by tyaUmlDispInit2 */
    int count;          /* 0x0C: sceVif1PkRef's transfer count */
} TyaUmlImage;

extern TyaUmlImage image[8];

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

extern unsigned char SaveData[];
extern char name_0[0x12];
extern const unsigned char D_00A13310[4];

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

        for (slot = 7; slot >= 0; slot--)
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
    blockSize = (size + 2047) & -2048;
    file = buffer - blockSize + 0x100000;
    xglCdReadFile(name_0, file, 0, 1);

    *(TyaUmlHeader *)out = *(TyaUmlHeader *)file;
    out += 96;
    cursor = file + 96;
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
            case 40:
                if ((*(unsigned short *)(SaveData + 0x102e2) & 0x10) == 0)
                    shake = 1;
                break;
            case 41:
                shake = 0;
                break;
            case 60:
                if ((*(unsigned short *)(SaveData + 0x102e2) & 0x30) != 0x30)
                    bold = 1;
                break;
            case 62:
                bold = 0;
                break;
            case 42: {
                u32 callback = (u32)mark;

                out[0] = 23;
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

            out[0] = 19;
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

    out[0] = 23;
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
    for (i = 0; i < 8; i++) {
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
    parameter->bufferUsed = (int)(out - buffer);
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

extern u64 TestEnv_2[14];
extern u64 InitEnv_3[4];
extern int TestEnv_4[24];
extern int TestEnv_5[16];
extern int TestEnv_6[28];

static void tyaUmlDispType3Sub0(FontDrawCursor *cursor, void *argument)
{
    XglPacket **packet = &cursor->packet;
    TyaUmlDispParam *parameter = argument;
    TyaUmlImage *first = &image[0];
    u8 *work = buffer;
    int rect_left = parameter->render_x - 1792;
    int rect_top = parameter->render_y - 1824;
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

    for (i = 1; i < 8; i++) {
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

/* Partial views of the objects the database view passes to the window,
 * menu-panel and cursor routines; only the fields this TU touches are named. */
typedef struct TyaUmlWindow {
    u16 x;                     /* 0x00 */
    u16 y;                     /* 0x02 */
    int style;                 /* 0x04 */
    u16 width;                 /* 0x08 */
    u16 height;                /* 0x0A */
    u8 unmodeled_0c[4];
    signed char state;         /* 0x10: 1 after setup, 3 when a file is chosen, 4 when cancelled */
} TyaUmlWindow;

typedef struct TyaUmlMenuPanel {
    u8 unmodeled_00[2];
    u16 rows;                  /* 0x02: cleared by setup */
    int cursor;                /* 0x04: highlighted row inside the visible page */
    const char *text;          /* 0x08: cleared by setup */
    u8 unmodeled_0c[0x1A9 - 0x0C];
    u8 markerShown;            /* 0x1A9: set by setup */
} TyaUmlMenuPanel;

typedef struct TyaUmlCursor {
    signed char state;         /* 0x00: 0x20 once set up */
    u8 unmodeled_01[3];
    short x;                   /* 0x04 */
    short y;                   /* 0x06 */
    int work;                  /* 0x08 */
} TyaUmlCursor;

extern PadPrefix PadData;
extern void WindowDXSet(void *window);
extern void WindowDXMain(void *window);
extern void MenuSelectWindow(void *window, void *panel);
extern void eCursolSet(void *cursor, int slot);
extern void eCursolMain(void *cursor);
extern void xglSoundEffectNormalDirect(int sound);

/*
 * The database view tyaUmlDatabaseMain drives: it starts with the display
 * parameters it hands to tyaUmlDispMain/tyaUmlDispType3/tyaUmlDispLoad, so
 * the record embeds a TyaUmlDispParam. Only the fields the database routines
 * read or write are modeled; the rest stays unmodeled.
 */
typedef struct TyaUmlDatabase {
    TyaUmlDispParam display;   /* 0x00 */
    u8 unmodeled_30[0x10];
    TyaUmlWindow *window;      /* 0x40 */
    TyaUmlMenuPanel *menu;     /* 0x44 */
    TyaUmlCursor *cursorA;     /* 0x48 */
    TyaUmlCursor *cursorB;     /* 0x4C */
    s16 state;                 /* 0x50: selects the step of tyaUmlDatabaseMain */
    s16 lastEntry;             /* 0x52: text lines read by dbheader_load, minus one */
    s16 listCursor;            /* 0x54 */
    s16 indexCursor;           /* 0x56: highlighted entry of the index bar */
    s16 foundCount;            /* 0x58: text records that had text */
    s16 fileIndex;             /* 0x5A */
    s16 topRow;                /* 0x5C */
    s16 rowsPerPage;           /* 0x5E */
} TyaUmlDatabase;

extern const char D_00A133B0[24];

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
    blockSize = (size + 2047) & -2048;
    file = buffer - blockSize + 0x100000;
    xglCdReadFile(D_00A133B0, file, 0, 1);
    tbl = (s16 *)(file - 0x800);
    dest = tbl;

    memset(out, 0, 100);
    out += 96;
    memset(image, 0, 128);

    out[0] = 8;
    out[1] = 8;
    out[3] = 24;
    out[2] = 0;
    out += 4;

    for (;;) {
        s16 value;

        while (*source == -1) {
            *dest++ = *source;
            source++;
        }
        value = *source;
        if (value == -2) {
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
                *dest++ = -3;
            }
            *out++ = '\n';
            source++;
            lines++;
        }
    }
}

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

int tyaUmlDatabaseMain(TyaUmlDatabase *database)
{
    TyaUmlCursor *cursorB;
    TyaUmlCursor *cursorA;
    TyaUmlMenuPanel *menu;
    TyaUmlWindow *window;

    if (database == 0)
        return -1;

    window = database->window;
    menu = database->menu;
    cursorA = database->cursorA;
    cursorB = database->cursorB;

    switch (database->state) {
    case 0:
        database->listCursor = 0;
        database->indexCursor = 0;
        WindowDXSet(window);
        window->x = database->display.render_x - 1792;
        window->style = (int)(database->display.color + 1);
        window->y = database->display.render_y - 1824;
        window->width = database->display.render_width;
        window->height = database->display.render_height;
        window->state = 1;
        MenuSelectWindow(window, menu);
        WindowDXMain(window);
        menu->markerShown = 1;
        menu->rows = 0;
        menu->text = 0;
        eCursolSet(cursorA, 1);
        cursorA->state = 0x20;
        cursorA->x = window->x + window->width - 17;
        cursorA->y = window->y + 4;
        cursorA->work = (int)(database->display.color + 2);
        eCursolSet(cursorB, 2);
        cursorB->state = 0x20;
        cursorB->work = (int)(database->display.color + 2);
        cursorB->x = window->x + window->width - 17;
        cursorB->y = window->y + window->height - 19;
        database->topRow = 0;
        database->rowsPerPage = database->display.render_height / 24;
        /* fall through */
    case 1:
        dbheader_load(database);
        database->state = 2;
        /* fall through */
    case 2:
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
            if (PadData.mappedRepeat & 0x1000)
                database->listCursor--;
            if (PadData.mappedRepeat & 0x4000)
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
                xglSoundEffectNormalDirect(3);

            menu->cursor = database->listCursor - database->topRow;
            database->display.clip_y = database->topRow * 24;
            database->indexCursor = get_indexno(database->listCursor);
            fileNo = get_fileno(database->listCursor, &suffix);
            database->fileIndex = suffix;

            if (PadData.half_2a & 0x20) {
                if (fileNo >= 0) {
                    window->state = 3;
                    xglSoundEffectNormalDirect(1);
                    database->display.resource_id = fileNo;
                    database->display.clip_y = 0;
                    tyaUmlDispLoad(&database->display);
                    database->state = 3;
                    return 0;
                }
                xglSoundEffectNormalDirect(5);
            }
            if (PadData.half_2a & 0x40) {
                database->indexCursor = 26;
                database->state = 4;
                window->state = 4;
            }

            pressed = PadData.mappedPressed;
            repeat = PadData.mappedRepeat;
            indexCursor = database->indexCursor;
            limit = ((pressed & 0xA080) == 0) ? indexCursor : -1;
            direction = 0;
            if (PadData.half_2a & 0x80)
                direction = 1;
            if (pressed & 0x2000)
                direction = 1;
            if (pressed & 0x8000)
                direction = -1;
            if (repeat & 0x2000)
                direction = 1;
            if (repeat & 0x8000)
                direction = -1;

            while (direction != 0) {
                indexCursor = indexCursor + direction;
                if (indexCursor < 0)
                    indexCursor = 24;
                if (indexCursor >= 25)
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
                xglSoundEffectNormalDirect(3);
            database->indexCursor = indexCursor;

            WindowDXMain(window);
            MenuSelectWindow(window, menu);
            if (window->state == 3) {
                if (database->topRow != 0)
                    eCursolMain(cursorA);
                if (database->topRow + database->rowsPerPage - 1 != database->lastEntry)
                    eCursolMain(cursorB);
                tyaUmlDispType3(&database->display);
            }
            disp_index(database, 0);
            break;
        }
    case 3:
        {
            s16 max;

            max = (s16)(database->display.clip_height - database->display.render_height);
            if (max < 0)
                max = 0;
            if (PadData.mappedRepeat & 0x1000)
                database->display.clip_y -= 8;
            if (PadData.mappedRepeat & 0x4000)
                database->display.clip_y += 8;
            if (database->display.clip_y < 0)
                database->display.clip_y = 0;
            if (database->display.clip_y > max)
                database->display.clip_y = max;
            if (PadData.half_2a & 0x40) {
                xglSoundEffectNormalDirect(2);
                database->display.resource_id = -1;
                database->state = 1;
            }
            tyaUmlDispMain(&database->display);
            if (database->display.clip_y != 0)
                eCursolMain(cursorA);
            if (database->display.clip_y != max)
                eCursolMain(cursorB);
            return 0;
        }
    case 4:
    case 5:
    case 6:
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
