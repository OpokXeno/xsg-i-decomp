/*
 * TU-local declarations of ov02/tu014 (src/ov02/umn_event_text_symbol_check.c).
 */

#ifndef SRC_OV02_UMN_EVENT_TEXT_SYMBOL_CHECK_H
#define SRC_OV02_UMN_EVENT_TEXT_SYMBOL_CHECK_H

/*
 * event_end_mail (ov02 .data, 2 bytes: 0x1D, 0x1E) is the one configured
 * follow-up mail: UmnEventEndMailCheck unlocks follow_mail_id's data byte 0
 * and registers it into the mailbox once trigger_mail_id has been read.
 */
typedef struct EventEndMail {
    signed char trigger_mail_id;
    signed char follow_mail_id;
} EventEndMail;

extern EventEndMail event_end_mail;

/* "data\\endou\\umn\\histree.bin", ov02 .data; no witnessed rename. */
extern const char D_00A13620[];

/* "data\\endou\\umn\\umntxt.bin" filename buffer, shared by UmnTextLoad's
 * synchronous and menu-load paths. */
extern char f_name_0[];

/* 128 history-tree words (UmnHistoryTreeGet), placed by UmnHistoryTreeLoad. */
extern int *UmnHistoryTreeBuf;

/* One mail header record: the 0x90-byte stride of UmnMailHeaderGet
 * (UmnMailHeaderCreate clears 128 of them, 0x4800 bytes). */
typedef struct UmnMailHeader {
    unsigned char unmodeled_00[0x90];
} UmnMailHeader;

extern UmnMailHeader *UmnMailHeaderBuf;

extern char *umn_text;

extern char *uet_text_buf;

extern int uet_flag;

/* main:0x00276180 */
extern void UmnMailBoxSet(int box_id);

/* main:0x002761D8 */
extern unsigned char *UmnMailDataGet(int box_id);

/* main:0x002751B0, defined in src/main/window_tex_load.c. */
extern int MenuLoadFile(const char *name, void *buffer);

#endif /* SRC_OV02_UMN_EVENT_TEXT_SYMBOL_CHECK_H */
