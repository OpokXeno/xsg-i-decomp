/*
 * OV02 original TU 14: 0x00a0e938..0x00a0f674 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_message.h"
#include "main/xgl_cd.h"
#include "main/xgl_font.h"
#include "ov02/ov02.h"
#include "ov02/umn_event_text_symbol_check.h"

/* Tags of the event text, UmnEventTextSymbol.value: the names are in eventTextTagTable. */
#define UMN_EVENT_TAG_SPEECH 1       /* "speech": opens a block */
#define UMN_EVENT_TAG_CHAR 2         /* "char": the speaker of the block */
#define UMN_EVENT_TAG_MSG 3          /* "msg": the message text */
#define UMN_EVENT_TAG_MOT 4          /* "mot": the motion number */
#define UMN_EVENT_TAG_SPEECH_END 5   /* "/speech" */
#define UMN_EVENT_TAG_MEMO 6         /* "memo": skipped up to the closing tag */
#define UMN_EVENT_TAG_MEMO_END 7     /* "/memo" */
#define UMN_EVENT_TAG_COUNT 7
#define UMN_EVENT_SPEAKER_COUNT 2

/* UmnEventTextNumberGet bases. */
#define UMN_EVENT_NUMBER_OCTAL 0
#define UMN_EVENT_NUMBER_DECIMAL 1

/* Mail ids listed in event_tbl and compulsion_down_load_tbl. */
/* uet_text_buf layout: a 4-byte count header, then 8-byte speech records. */
#define UMN_EVENT_TEXT_HEADER_SIZE 4
#define UMN_EVENT_TEXT_RECORD_SIZE 8

#define UMN_EVENT_MAIL_COUNT 18
#define UMN_COMPULSION_MAIL_COUNT 13

/* Attachment kinds (UmnMailHeader.attach) and the size of an umn_attach_tbl record: kind, id, quantity. */
#define UMN_ATTACH_PLUGIN 1
#define UMN_ATTACH_ITEM 2
#define UMN_ATTACH_WEAPON 3
#define UMN_ATTACH_MONEY 4
#define UMN_ATTACH_RECORD_SIZE 3
#define UMN_ATTACH_COUNT 23

/* Work area blocks the loaders hand back: the history tree is aligned to a CD sector. */
#define UMN_HISTORY_TREE_SIZE 0x800
#define UMN_TEXT_BLOCK_SIZE 0x800
/* umn_text holds one 0x80-byte record for each of the first six plugins. */
#define UMN_PLUGIN_TEXT_COUNT 6
#define UMN_PLUGIN_TEXT_SIZE 0x80
#define UMN_MAIL_HEADER_TABLE_SIZE 0x4800

/*
 * event_end_mail (ov02 .data, 2 bytes: 0x1D, 0x1E) is the one configured
 * follow-up mail: UmnEventEndMailCheck unlocks follow_mail_id's data byte 0
 * and registers it into the mailbox once trigger_mail_id has been read.
 */
typedef struct EventEndMail {
    signed char trigger_mail_id;
    signed char follow_mail_id;
} EventEndMail;

/* Record-count header of uet_text_buf; the speech records follow it. */
typedef struct UmnEventTextRecordBuf {
    short count;
    unsigned short cur;
    UmnEventTextRecord records[1];
} UmnEventTextRecordBuf;

/* Mail-list file records consumed while UmnMailHeaderCreate builds the
 * runtime header table. */
typedef struct UmnMailListBody {
    unsigned char unmodeled_00[8];
    unsigned char attach[3];
    unsigned char kind;
    unsigned short flags;
    unsigned char plugin;
    unsigned char unmodeled_0f;
    int data;
    unsigned char unmodeled_14[0x4c];
    unsigned char text[1];
} UmnMailListBody;

typedef struct UmnMailList {
    int size;
    int mail_id;
    UmnMailListBody body;
} UmnMailList;

typedef struct UmnEventTextSymbol {
    unsigned char *text;
    short length;
    short value;
} UmnEventTextSymbol;

typedef struct UmnEventTextSymbolTable {
    UmnEventTextSymbol entry[UMN_EVENT_TAG_COUNT];
} UmnEventTextSymbolTable;

/* Scaffold-owned symbol table at 0x00A13588 (no recovered C definition). */
extern const UmnEventTextSymbolTable eventTextTagTable;

typedef struct UmnEventTextCharTable {
    UmnEventTextSymbol entry[UMN_EVENT_SPEAKER_COUNT];
} UmnEventTextCharTable;

/* Scaffold-owned speaker-name table at 0x00A135D0 (no recovered C definition). */
extern const UmnEventTextCharTable eventTextSpeakerTable;

extern char *eMessageNextWaitKeySearch(char *text);

/*
 * Declared int, unlike main/data.h's void dataWpnBoxInc (measured: the void
 * prototype moves the loop-bound compare in UmnMailAttachSet from $v1 to $v0).
 */
extern int dataItmBoxInc(int id);
extern int dataWpnBoxInc(int id);
extern int dataMoneyBoxInc(int amount);

/* "data\\endou\\umn\\histree.bin" and "data\\umn\\header.lst", ov02 .data; no witnessed rename. */
const char historyTreePath[] = "data\\endou\\umn\\histree.bin";
const char mailHeaderListPath[] = "data\\umn\\header.lst";
static unsigned char umn_attach_tbl[UMN_ATTACH_COUNT * UMN_ATTACH_RECORD_SIZE] = {
    1, 1, 1, 1, 2, 1, 1, 3, 1, 2, 2, 5,
    2, 6, 5, 2, 10, 5, 4, 4, 1, 4, 6, 1,
    2, 23, 2, 3, 20, 1, 4, 4, 1, 4, 8, 1,
    1, 4, 1, 4, 72, 1, 4, 48, 1, 4, 24, 1,
    4, 60, 1, 4, 40, 1, 1, 5, 1, 3, 22, 1,
    4, 10, 2, 4, 20, 2, 3, 21, 1
};
static unsigned char event_tbl[UMN_EVENT_MAIL_COUNT] = {
    0x00, 0x09, 0x06, 0x05, 0x11, 0x16, 0x1d, 0x29, 0x2c,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x43, 0x44, 0x47, 0x4d
};
static EventEndMail event_end_mail = { 0x1d, 0x1e };
static unsigned char kosmos_special_tbl[4][4] = {
    { 0x29, 0x37, 0x00, 0x00 },
    { 0x42, 0x38, 0x00, 0x00 },
    { 0x46, 0x39, 0x3a, 0x00 },
    { 0x4a, 0x3b, 0x00, 0x00 }
};
static signed char compulsion_down_load_tbl[UMN_COMPULSION_MAIL_COUNT] = {
    0x09, 0x11, 0x26, 0x27, 0x2d, 0x2e, 0x35,
    0x36, 0x37, 0x39, 0x3a, 0x44, 0x4d
};
int UmnEventTextMakeWork = 0;
UmnEventTextRecordBuf *uet_text_buf = 0;
int uet_flag = 0;
unsigned char *uet_file_end = 0;
UmnHistoryNode *UmnHistoryTreeBuf = 0;
UmnMailHeader *UmnMailHeaderBuf = 0;
char *umn_text = 0;

/*
 * After a '<' the text is compared against the seven symbol names; on a
 * match *cursor advances past the name and its terminator and the symbol's
 * value is returned, otherwise 0.
 */
int UmnEventTextSymbolCheck(unsigned char **cursor)
{
    int i;
    int n;

    if (**cursor == '<') {
        UmnEventTextSymbolTable table;

        *cursor = *cursor + 1;
        table = eventTextTagTable;
        for (i = 0; i < UMN_EVENT_TAG_COUNT; i++) {
            unsigned char *input = *cursor;
            unsigned char *text = table.entry[i].text;

            for (n = 0; n < table.entry[i].length; n++) {
                if (input[n] != text[n]) {
                    break;
                }
            }
            if (n == table.entry[i].length) {
                *cursor += n + 1;
                return table.entry[i].value;
            }
        }
    }
    return 0;
}

int UmnEventTextCharCheck(unsigned char **cursor)
{
    UmnEventTextCharTable table;
    unsigned char *input;
    unsigned char *text;
    int i;
    int n;

    i = 0;
    table = eventTextSpeakerTable;
    input = *cursor;
    for (; UMN_EVENT_SPEAKER_COUNT > i; i++) {
        text = table.entry[i].text;
        n = 0;
        for (; n < table.entry[i].length; n++) {
            if (input[n] != text[n]) {
                break;
            }
        }
        if (n == table.entry[i].length) {
            *cursor += n;
            return table.entry[i].value;
        }
    }
    return 0;
}

int UmnEventTextNumberGet(unsigned char **cursor, int base)
{
    unsigned char c;
    unsigned char value;
    int radix;
    unsigned int max_digit;

    value = 0;
    if (base == UMN_EVENT_NUMBER_DECIMAL) {
        max_digit = '9';
        radix = 10;
    } else {
        max_digit = '7';
        radix = 8;
    }
    for (;;) {
        unsigned int digit = **cursor;

        if (digit < '0' || digit > max_digit) {
            break;
        }
        c = **cursor;
        value *= radix;
        value += c - '0';
        *cursor = *cursor + 1;
    }
    return value;
}

/* Advances *str past every consecutive newline. */
void UmnEventTextGyouJump(unsigned char **str)
{
    unsigned char *cursor = *str;
    unsigned char *next;

    if (*cursor == '\n') {
        do {
            next = cursor + 1;
            *str = next;
            cursor = next;
        } while (*cursor == '\n');
    }
}

/*
 * Copies one message body from *cursor into *dest up to the closing '<'.
 * A '\\' escape stores its numeric value; EMESSAGE_CODE_WAITKEY also skips the newline
 * run that follows. Terminates *dest and leaves *cursor 6 bytes past the '<'.
 */
void UmnEventTextMessageMake(unsigned char **cursor, unsigned char **dest)
{
    unsigned char *out;
    unsigned char *p;
    unsigned char c;
    int value;

    p = *cursor;
    if (*p != '<') {
        do {
            c = *p;
            if (c == '\\') {
                unsigned char *out;
                *cursor = p + 1;
                value = UmnEventTextNumberGet(cursor, UMN_EVENT_NUMBER_OCTAL);
                if (value == EMESSAGE_CODE_WAITKEY) {
                    UmnEventTextGyouJump(cursor);
                }
                out = *dest;
                *out++ = value;
                *dest = out;
            } else {
                unsigned char *out;
                out = *dest;
                *out++ = c;
                *cursor = p + 1;
                *dest = out;
            }
            p = *cursor;
        } while (*p != '<');
    }
    out = *dest;
    *out++ = 0;
    *dest = out;
    *cursor += 6;
}

/*
 * Loads event NN's script into the work arena, counts its speech blocks and
 * fills the record table at uet_text_buf from the event text.
 */
int UmnEventTextMake(int event_no)
{
    char name[] = "data\\umn\\event00.txt";
    int size;
    unsigned char *aligned;
    int count;
    int index;
    unsigned char *scan;
    unsigned char *cursor;
    unsigned char *write;
    unsigned char *records;
    unsigned char *text;
    UmnEventTextRecordBuf *buf;
    UmnEventTextRecordBuf *head;
    UmnEventTextRecord *record;
    unsigned int tag;

    name[14] = '0' + event_no / 10;
    name[15] = '0' + event_no % 10;
    aligned = (unsigned char *) ((UmnEventTextMakeWork + 0xF) & ~0xF);
    size = xglCdGetFileSize(name);
    if (size < 0) {
        return -1;
    }
    xglCdReadFile(name, aligned, 0, 1);
    cursor = aligned;
    write = (unsigned char *) uet_text_buf;
    buf = uet_text_buf;
    uet_file_end = aligned + size;

    count = 0;
    scan = aligned;
    if (scan < uet_file_end) {
        do {
            switch (UmnEventTextSymbolCheck(&scan)) {
            case UMN_EVENT_TAG_MEMO:
                do {
                    scan++;
                } while (UmnEventTextSymbolCheck(&scan) != UMN_EVENT_TAG_MEMO_END);
                break;
            case UMN_EVENT_TAG_SPEECH:
                count++;
                break;
            }
            scan++;
        } while (scan < uet_file_end);
    }

    head = (UmnEventTextRecordBuf *) write;
    head->count = count;
    head->cur = 0;
    records = (unsigned char *) head + UMN_EVENT_TEXT_HEADER_SIZE;
    write = records + count * UMN_EVENT_TEXT_RECORD_SIZE;

    index = 0;
    UmnEventTextGyouJump(&cursor);
    while (cursor < uet_file_end) {
        record = &buf->records[index];
        tag = UmnEventTextSymbolCheck(&cursor);
        switch (tag) {
        case UMN_EVENT_TAG_MEMO:
            do {
                cursor++;
            } while (UmnEventTextSymbolCheck(&cursor) != UMN_EVENT_TAG_MEMO_END);
            break;
        case 0:
            return -1;
        case UMN_EVENT_TAG_SPEECH:
            UmnEventTextNumberGet(&cursor, UMN_EVENT_NUMBER_DECIMAL);
            record->count = 0;
            record->text = 0;
            do {
                record->number = 0;
            } while (0);
            record->value = 0;
            break;
        case UMN_EVENT_TAG_CHAR:
            record->value = UmnEventTextCharCheck(&cursor);
            break;
        case UMN_EVENT_TAG_MOT:
            record->number = UmnEventTextNumberGet(&cursor, UMN_EVENT_NUMBER_DECIMAL);
            break;
        case UMN_EVENT_TAG_MSG:
            text = write;
            record->text = (char *) text;
            UmnEventTextGyouJump(&cursor);
            UmnEventTextMessageMake(&cursor, &write);
            if (*text != 0) {
                do {
                    text = (unsigned char *) eMessageNextWaitKeySearch((char *) text);
                    record->count++;
                } while (*text != 0);
            }
            break;
        case UMN_EVENT_TAG_SPEECH_END:
            index++;
            break;
        default:
            break;
        }
        UmnEventTextGyouJump(&cursor);
    }
    uet_flag = 1;
    return 0;
}

/*
 * uet_text_buf holds a signed record count followed by 8-byte records;
 * advances the cursor by delta and returns the record at the new position,
 * or NULL once it runs past count.
 */
UmnEventTextRecord *UmnEventTextNextGet(int delta)
{
    UmnEventTextRecordBuf *buf = uet_text_buf;
    UmnEventTextRecord *result;
    int wide_cur;
    short cur;

    wide_cur = buf->cur + delta;
    buf->cur = wide_cur;
    cur = wide_cur;
    result = 0;
    if (cur >= buf->count) {
        return result;
    }
    result = &buf->records[cur];
    return result;
}

/*
 * event_tbl lists the mail_id values UmnEventEndMailCheck's mailbox check
 * applies to; bit 2 of the mail's data byte marks it already handled.
 */
int UmnMailEventCheck(int mail_id)
{
    unsigned char *data;
    int i;

    for (i = 0; i < UMN_EVENT_MAIL_COUNT; i++) {
        if (event_tbl[i] == mail_id) {
            data = UmnMailDataGet(mail_id);
            if (*data & 4) {
                break;
            }
            return i + 1;
        }
    }
    return 0;
}

/*
 * Reading trigger_mail_id unlocks follow_mail_id: its data byte's bit 0 is
 * set once and the mail is registered into the mailbox.
 */
int UmnEventEndMailCheck(int mail_id)
{
    unsigned char *data;

    if (event_end_mail.trigger_mail_id == mail_id) {
        signed char *follow_ptr = &event_end_mail.follow_mail_id;

        data = UmnMailDataGet(*follow_ptr);
        if (!(*data & 1)) {
            *data |= 1;
            UmnMailBoxSet(*follow_ptr);
            return 1;
        }
    }
    return 0;
}

/*
 * work is the arena address UmnFirstLoad passes (UmnWorkEnd): the tree is
 * read into the next 0x800-aligned block and the address after it returned.
 */
int UmnHistoryTreeLoad(int work)
{
    int aligned = (work + UMN_HISTORY_TREE_SIZE - 1) & ~(UMN_HISTORY_TREE_SIZE - 1);
    int next = aligned + UMN_HISTORY_TREE_SIZE;

    UmnHistoryTreeBuf = (UmnHistoryNode *)aligned;
    xglCdReadFile(historyTreePath, UmnHistoryTreeBuf, 0, 0);
    return next;
}

UmnHistoryNode *UmnHistoryTreeGet(unsigned int history_id)
{
    UmnHistoryNode *tree = UmnHistoryTreeBuf;
    UmnHistoryNode *entry = 0;

    if (history_id < UMN_MAIL_COUNT) {
        entry = &tree[history_id];
    }
    return entry;
}

/*
 * The first kosmos_special_tbl record whose id matches copies its reward-code
 * characters, stopping at a 0 character, into UmnKosmosSpecialBox.
 */
void UmnKosmosSpecialGetCheck(int id)
{
    int i;
    int copied;
    int column;
    int slot;

    slot = 0;
    for (i = 0; i < 4; i++) {
        if (kosmos_special_tbl[i][0] == id) {
            copied = 0;
            column = 1;
            do {
                if (kosmos_special_tbl[i][column] == 0) {
                    break;
                }
                UmnKosmosSpecialBox[slot] = kosmos_special_tbl[i][column];
                slot++;
                column++;
                copied++;
            } while (copied < 3);
            return;
        }
    }
}

unsigned char *UmnMailAttachGet(int box_id)
{
    return umn_attach_tbl + box_id * UMN_ATTACH_RECORD_SIZE;
}

void UmnMailAttachSet(int box_id)
{
    UmnMailSaveData *state = &umnMailSaveData;
    unsigned char *attachment;
    int index;
    int scale;
    short exponent;
    int quantity;

    if (box_id <= 0) {
        return;
    }

    attachment = UmnMailAttachGet(box_id - 1);
    switch (attachment[0]) {
    case UMN_ATTACH_PLUGIN:
        state->plugin_unlock_flags |= 1 << attachment[1];
        break;
    case UMN_ATTACH_ITEM:
        if (attachment[2] != 0) {
            index = 0;
            do {
                dataItmBoxInc(attachment[1]);
                index++;
                quantity = attachment[2];
            } while (index < (short)quantity);
        }
        break;
    case UMN_ATTACH_WEAPON:
        if (attachment[2] != 0) {
            index = 0;
            do {
                dataWpnBoxInc(attachment[1]);
                index++;
                quantity = attachment[2];
            } while (index < (short)quantity);
        }
        break;
    case UMN_ATTACH_MONEY:
        scale = 10;
        quantity = attachment[2];
        exponent = quantity + 1;
        if (exponent != 0) {
            quantity = exponent;
            do {
                quantity--;
                scale *= 10;
            } while (quantity != 0);
        }
        dataMoneyBoxInc(attachment[1] * scale);
        break;
    }
}

/* One 0x90-byte mail header record per mail_id. */
UmnMailHeader *UmnMailHeaderGet(int mail_id)
{
    return &UmnMailHeaderBuf[mail_id];
}

int UmnMailHeaderCreate(int work)
{
    int aligned = (work + 0xF) & ~0xF;
    unsigned char *list = (unsigned char *)(aligned + UMN_MAIL_HEADER_TABLE_SIZE);
    unsigned char *record = list;
    unsigned char *cursor = list;
    UmnMailList *entry;
    UmnMailListBody *body;
    UmnMailHeader *header;
    unsigned char *subject;
    unsigned char *title;
    int size;
    int slot;
    int level;
    int i;

    UmnMailHeaderBuf = (UmnMailHeader *)aligned;
    memset(UmnMailHeaderBuf, 0, UMN_MAIL_HEADER_TABLE_SIZE);
    xglCdReadFile(mailHeaderListPath, list, 0, 0);
    size = ((UmnMailList *)cursor)->size;
    while (size != 0) {
        cursor = cursor + 4;
        entry = (UmnMailList *)(cursor - 4);
        header = UmnMailHeaderBuf;
        header += entry->mail_id;
        cursor = cursor + 4;
        body = (UmnMailListBody *)cursor;
        for (i = 0; i < 3; i++) {
            header->name[i] = body->attach[i];
        }
        header->scenario_limit = body->flags;
        header->attach = body->kind;
        slot = body->plugin >> 5;
        level = (body->plugin & 3) + 1;
        memset(header->kosmos_items, 0, 5);
        if (slot >= 1 && slot <= 4) {
            header->kosmos_items[slot - 1] = level;
        }
        header->cost = body->data;
        cursor = cursor + 96;
        subject = cursor;
        while (*cursor != 0) {
            if (*cursor < 32) {
                cursor = cursor + xglFontGetSPcodeSize(*cursor, cursor);
                cursor = cursor + 1;
            } else {
                cursor = cursor + 1;
            }
        }
        cursor = cursor + 1;
        title = cursor;
        while (*cursor != 0) {
            if (*cursor < 32) {
                cursor = cursor + xglFontGetSPcodeSize(*cursor, cursor);
                cursor = cursor + 1;
            } else {
                cursor = cursor + 1;
            }
        }
        eMessageCpy(header->subject, subject + 9);
        eMessageCpy(header->title, title + 8);
        record = record + size + 4;
        cursor = record;
        size = ((UmnMailList *)cursor)->size;
    }
    return aligned + UMN_MAIL_HEADER_TABLE_SIZE;
}

/* One 0x80-byte plugin text record for the first six ids, falling back to
 * the text base for anything else. */
char *UmnPluginTextGet(unsigned int plugin_id)
{
    char *text = umn_text;
    char *entry = text;

    if (plugin_id < UMN_PLUGIN_TEXT_COUNT) {
        entry = umn_text + plugin_id * UMN_PLUGIN_TEXT_SIZE;
    }
    text = entry;
    return text;
}

int UmnTextLoad(int work, int mode)
{
    static char f_name[] = "data\\endou\\umn\\umntxt.bin";
    int aligned = (work + 0xF) & ~0xF;
    int next = aligned + UMN_TEXT_BLOCK_SIZE;

    umn_text = (char *)aligned;
    if (mode == 0) {
        xglCdReadFile(f_name, umn_text, 0, 1);
    } else {
        MenuLoadFile(f_name, umn_text);
    }
    return next;
}

/*
 * compulsion_down_load_tbl lists mail_id values with a forced-download
 * attachment; bit 7 of the mail data's second byte marks it downloaded.
 */
int UmnMailCompulsionDownLoadCheck(int mail_id)
{
    unsigned char *data;
    int i;

    for (i = 0; i < UMN_COMPULSION_MAIL_COUNT; i++) {
        if (compulsion_down_load_tbl[i] == mail_id) {
            data = UmnMailDataGet(mail_id);
            if (data[1] & UMN_MAIL_ATTACH_TAKEN) {
                break;
            }
            return 1;
        }
    }
    return 0;
}

int UmnEventTextInit(int work)
{
    int aligned = (work + 0xF) & ~0xF;

    uet_text_buf = (UmnEventTextRecordBuf *)aligned;
    uet_flag = 0;
    return aligned + UMN_EVENT_TEXT_BUFFER_SIZE;
}
