/*
 * OV02 original TU 14: 0x00a0e938..0x00a0f674 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "umn_event_text_symbol_check.h"

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextSymbolCheck);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextCharCheck);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextNumberGet);

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

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextMessageMake);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextMake);

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
    int aligned = (work + 0xF) & ~0xF;
    int next = aligned + 0x800;

    umn_text = (char *)aligned;
    if (mode == 0) {
        xglCdReadFile(f_name_0, (void *)aligned, 0, 1);
    } else {
        MenuLoadFile(f_name_0, (void *)aligned);
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
