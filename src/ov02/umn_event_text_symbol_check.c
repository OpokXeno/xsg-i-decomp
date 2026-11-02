/*
 * OV02 original TU 14: 0x00a0e938..0x00a0f674 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "umn_event_text_symbol_check.h"

const char D_00A13620[] = "data\\endou\\umn\\histree.bin";
const char D_00A13640[] = "data\\umn\\header.lst";
static unsigned char umn_attach_tbl[0x45] = {
    1, 1, 1, 1, 2, 1, 1, 3, 1, 2, 2, 5,
    2, 6, 5, 2, 10, 5, 4, 4, 1, 4, 6, 1,
    2, 23, 2, 3, 20, 1, 4, 4, 1, 4, 8, 1,
    1, 4, 1, 4, 72, 1, 4, 48, 1, 4, 24, 1,
    4, 60, 1, 4, 40, 1, 1, 5, 1, 3, 22, 1,
    4, 10, 2, 4, 20, 2, 3, 21, 1
};
static unsigned char event_tbl[18] = {
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
static signed char compulsion_down_load_tbl[13] = {
    0x09, 0x11, 0x26, 0x27, 0x2d, 0x2e, 0x35,
    0x36, 0x37, 0x39, 0x3a, 0x44, 0x4d
};
int UmnEventTextMakeWork = 0;
char *uet_text_buf = 0;
int uet_flag = 0;
int uet_file_end = 0;
int *UmnHistoryTreeBuf = 0;
UmnMailHeader *UmnMailHeaderBuf = 0;
char *umn_text = 0;

typedef struct UmnEventTextSymbol {
    unsigned char *text;
    short length;
    short value;
} UmnEventTextSymbol;

typedef struct UmnEventTextSymbolTable {
    UmnEventTextSymbol entry[7];
} UmnEventTextSymbolTable;

/* Scaffold-owned symbol table at 0x00A13588 (no recovered C definition). */
extern const UmnEventTextSymbolTable D_00A13588;

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
        table = D_00A13588;
        for (i = 0; i < 7; i++) {
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

typedef struct UmnEventTextCharTable {
    UmnEventTextSymbol entry[2];
} UmnEventTextCharTable;

/* Scaffold-owned speaker-name table at 0x00A135D0 (no recovered C definition). */
extern const UmnEventTextCharTable D_00A135D0;

int UmnEventTextCharCheck(unsigned char **cursor)
{
    UmnEventTextCharTable table;
    unsigned char *input;
    unsigned char *text;
    int i;
    int n;

    i = 0;
    table = D_00A135D0;
    input = *cursor;
    for (; 2 > i; i++) {
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
    if (base == 1) {
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
 * A '\\' escape stores its numeric value; value 31 also skips the newline
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
                value = UmnEventTextNumberGet(cursor, 0);
                if (value == 31) {
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
    int aligned;
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
    aligned = (UmnEventTextMakeWork + 0xF) & ~0xF;
    size = xglCdGetFileSize(name);
    if (size < 0) {
        return -1;
    }
    xglCdReadFile(name, (void *) aligned, 0, 1);
    cursor = (unsigned char *) aligned;
    write = (unsigned char *) uet_text_buf;
    buf = (UmnEventTextRecordBuf *) uet_text_buf;
    uet_file_end = aligned + size;

    count = 0;
    scan = (unsigned char *) aligned;
    if (scan < (unsigned char *) uet_file_end) {
        do {
            switch (UmnEventTextSymbolCheck(&scan)) {
            case 6:
                do {
                    scan++;
                } while (UmnEventTextSymbolCheck(&scan) != 7);
                break;
            case 1:
                count++;
                break;
            }
            scan++;
        } while (scan < (unsigned char *) uet_file_end);
    }

    head = (UmnEventTextRecordBuf *) write;
    head->count = count;
    head->cur = 0;
    records = (unsigned char *) head + 4;
    write = records + count * 8;

    index = 0;
    UmnEventTextGyouJump(&cursor);
    while (cursor < (unsigned char *) uet_file_end) {
        record = (UmnEventTextRecord *) ((char *) buf + index * 8 + 4);
        tag = UmnEventTextSymbolCheck(&cursor);
        switch (tag) {
        case 6:
            do {
                cursor++;
            } while (UmnEventTextSymbolCheck(&cursor) != 7);
            break;
        case 0:
            return -1;
        case 1:
            UmnEventTextNumberGet(&cursor, 1);
            record->count = 0;
            record->text = 0;
            do {
                record->number = 0;
            } while (0);
            record->value = 0;
            break;
        case 2:
            record->value = UmnEventTextCharCheck(&cursor);
            break;
        case 4:
            record->number = UmnEventTextNumberGet(&cursor, 1);
            break;
        case 3:
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
        case 5:
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
void *UmnEventTextNextGet(int delta)
{
    UmnEventTextRecordBuf *buf = (UmnEventTextRecordBuf *) uet_text_buf;
    void *result;
    int wide_cur;
    short cur;

    wide_cur = buf->cur + delta;
    buf->cur = wide_cur;
    cur = wide_cur;
    result = 0;
    if (cur >= buf->count) {
        return result;
    }
    result = (char *) buf + cur * 8 + 4;
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

    for (i = 0; i < 18; i++) {
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
    int aligned = (work + 0x7FF) & ~0x7FF;
    int next = aligned + 0x800;

    UmnHistoryTreeBuf = (int *)aligned;
    xglCdReadFile(D_00A13620, (void *)aligned, 0, 0);
    return next;
}

int *UmnHistoryTreeGet(unsigned int history_id)
{
    int *tree = UmnHistoryTreeBuf;
    int *entry = 0;

    if (history_id < 128) {
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
    return umn_attach_tbl + box_id * 3;
}

void UmnMailAttachSet(int box_id)
{
    UmnAttachmentState *state = &D_4A1A0C;
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
    case 1:
        state->plugin_unlock_flags |= 1 << attachment[1];
        break;
    case 2:
        if (attachment[2] != 0) {
            index = 0;
            do {
                dataItmBoxInc(attachment[1]);
                index++;
                quantity = attachment[2];
            } while (index < (short)quantity);
        }
        break;
    case 3:
        if (attachment[2] != 0) {
            index = 0;
            do {
                dataWpnBoxInc(attachment[1]);
                index++;
                quantity = attachment[2];
            } while (index < (short)quantity);
        }
        break;
    case 4:
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

typedef struct UmnMailHeaderView {
    char sender[0x40];
    char title[0x40];
    unsigned char attach[3];
    unsigned char unmodeled_83;
    unsigned short flags;
    unsigned char kind;
    unsigned char level[5];
    int data;
} UmnMailHeaderView;

int UmnMailHeaderCreate(int work)
{
    int aligned = (work + 0xF) & ~0xF;
    unsigned char *list = (unsigned char *)(aligned + 0x4800);
    unsigned char *record = list;
    unsigned char *cursor = list;
    UmnMailList *entry;
    UmnMailListBody *body;
    UmnMailHeaderView *header;
    unsigned char *sender;
    unsigned char *title;
    int size;
    int slot;
    int level;
    int i;

    UmnMailHeaderBuf = (UmnMailHeader *)aligned;
    memset((void *)aligned, 0, 0x4800);
    xglCdReadFile(D_00A13640, list, 0, 0);
    size = ((UmnMailList *)cursor)->size;
    while (size != 0) {
        cursor = cursor + 4;
        entry = (UmnMailList *)(cursor - 4);
        header = (UmnMailHeaderView *)UmnMailHeaderBuf;
        header += entry->mail_id;
        cursor = cursor + 4;
        body = (UmnMailListBody *)cursor;
        for (i = 0; i < 3; i++) {
            header->attach[i] = body->attach[i];
        }
        header->flags = body->flags;
        header->kind = body->kind;
        slot = body->plugin >> 5;
        level = (body->plugin & 3) + 1;
        memset(header->level, 0, 5);
        if (slot >= 1 && slot <= 4) {
            header->level[slot - 1] = level;
        }
        header->data = body->data;
        cursor = cursor + 96;
        sender = cursor;
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
        eMessageCpy(header->title, (char *)sender + 9);
        eMessageCpy(header->sender, (char *)title + 8);
        record = record + size + 4;
        cursor = record;
        size = ((UmnMailList *)cursor)->size;
    }
    return aligned + 0x4800;
}

/* One 0x80-byte plugin text record for the first six ids, falling back to
 * the text base for anything else. */
char *UmnPluginTextGet(unsigned int plugin_id)
{
    char *text = umn_text;
    char *entry = text;

    if (plugin_id < 6) {
        entry = umn_text + plugin_id * 0x80;
    }
    text = entry;
    return text;
}

int UmnTextLoad(int work, int mode)
{
    static char f_name[] = "data\\endou\\umn\\umntxt.bin";
    int aligned = (work + 0xF) & ~0xF;
    int next = aligned + 0x800;

    umn_text = (char *)aligned;
    if (mode == 0) {
        xglCdReadFile(f_name, (void *)aligned, 0, 1);
    } else {
        MenuLoadFile(f_name, (void *)aligned);
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

    for (i = 0; i < 13; i++) {
        if (compulsion_down_load_tbl[i] == mail_id) {
            data = UmnMailDataGet(mail_id);
            if (data[1] & 0x80) {
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

    uet_text_buf = (char *)aligned;
    uet_flag = 0;
    return aligned + 0x2000;
}
