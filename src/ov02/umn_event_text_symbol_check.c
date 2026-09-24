/*
 * OV02 original TU 14: 0x00a0e938..0x00a0f674 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "umn_event_text_symbol_check.h"

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextSymbolCheck);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextCharCheck);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextNumberGet);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextGyouJump);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextMessageMake);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextMake);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnEventTextNextGet);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnMailEventCheck);

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

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnKosmosSpecialGetCheck);

unsigned char *UmnMailAttachGet(int box_id)
{
    return umn_attach_tbl + box_id * 3;
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnMailAttachSet);

/* One 0x90-byte mail header record per mail_id. */
UmnMailHeader *UmnMailHeaderGet(int mail_id)
{
    return &UmnMailHeaderBuf[mail_id];
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnMailHeaderCreate);

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

INCLUDE_ASM("asm/nonmatchings/ov02/umn_event_text_symbol_check", UmnMailCompulsionDownLoadCheck);

int UmnEventTextInit(int work)
{
    int aligned = (work + 0xF) & ~0xF;

    uet_text_buf = (char *)aligned;
    uet_flag = 0;
    return aligned + 0x2000;
}
