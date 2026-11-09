#ifndef INCLUDE_OV02_UMN_EVENT_TEXT_SYMBOL_CHECK_H
#define INCLUDE_OV02_UMN_EVENT_TEXT_SYMBOL_CHECK_H

/* Mails of the mailbox: mail ids index the header table, the save record and the history tree. */
#define UMN_MAIL_COUNT 128

/* UmnHistoryNode.children, UmnMailHeader.reply_ids and UmnMailHeader.kosmos_items counts. */
#define UMN_MAIL_HISTORY_CHILDREN 3
#define UMN_MAIL_REPLY_COUNT 3
#define UMN_MAIL_KOSMOS_ITEMS 5

/* Second byte of the save record: the attachment id (UmnMailHeader.attach) plus this flag once it is taken. */
#define UMN_MAIL_ATTACH_TAKEN 0x80

/* Bytes UmnEventTextInit reserves for uet_text_buf and the event script UmnEventTextMake reads. */
#define UMN_EVENT_TEXT_BUFFER_SIZE 0x2000

/* UmnEventTextRecord.value: the speaker named by a "char" tag of the event text. */
#define UMN_EVENT_SPEAKER_UKUN 1
#define UMN_EVENT_SPEAKER_SHION 2

/*
 * One 0x90-byte mail header record per mail_id (UmnMailHeaderCreate clears 128
 * of them, 0x4800 bytes, and fills them from the header.lst records).
 */
typedef struct UmnMailHeader {
    char sender[0x40];                  /* +0x00: the Sender line of header.lst */
    char subject[0x40];                 /* +0x40 */
    unsigned char reply_ids[UMN_MAIL_REPLY_COUNT];        /* +0x80: the mails the player can send as a reply, 0 = unused */
    unsigned char unmodeled_83;
    unsigned short scenario_limit;      /* +0x84 */
    unsigned char attach;               /* +0x86: attachment id, 0 = none */
    unsigned char kosmos_items[UMN_MAIL_KOSMOS_ITEMS];      /* +0x87 */
    int cost;                           /* +0x8C */
} UmnMailHeader;

/* One 8-byte speech record after the record-count header of uet_text_buf. */
typedef struct UmnEventTextRecord {
    short value;
    unsigned char count;
    signed char number;
    char *text;
} UmnEventTextRecord;

/*
 * The umn mail part of the save record (SaveData + 0x1015C): the ids of the
 * mails received, the kosmos item counters and the plugin unlock bit set.
 */
typedef struct UmnMailSaveData {
    signed char mail_ids[UMN_MAIL_COUNT];
    unsigned char kosmos_items[UMN_MAIL_KOSMOS_ITEMS];
    unsigned char unmodeled_85;
    unsigned short plugin_unlock_flags;
} UmnMailSaveData;

/* One node of the mail history tree: the ids of up to three child mails, -1 for none. */
typedef struct UmnHistoryNode {
    signed char children[UMN_MAIL_HISTORY_CHILDREN];
    unsigned char unmodeled_03;
} UmnHistoryNode;

/* Functions and data used by other translation units. */
extern UmnMailSaveData umnMailSaveData;
extern int UmnEventTextMakeWork;
int UmnEventTextMake(int event_no);
UmnEventTextRecord *UmnEventTextNextGet(int delta);
int UmnMailEventCheck(int mail_id);
int UmnEventEndMailCheck(int mail_id);
int UmnHistoryTreeLoad(int work);
UmnHistoryNode *UmnHistoryTreeGet(unsigned int history_id);
void UmnKosmosSpecialGetCheck(int id);
unsigned char *UmnMailAttachGet(int box_id);
void UmnMailAttachSet(int box_id);
UmnMailHeader *UmnMailHeaderGet(int mail_id);
int UmnMailHeaderCreate(int work);
char *UmnPluginTextGet(unsigned int plugin_id);
int UmnTextLoad(int work, int mode);
int UmnMailCompulsionDownLoadCheck(int mail_id);
int UmnEventTextInit(int work);

#endif /* INCLUDE_OV02_UMN_EVENT_TEXT_SYMBOL_CHECK_H */
