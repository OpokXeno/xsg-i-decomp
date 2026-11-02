#ifndef INCLUDE_OV02_TSK_UMN_MAIL_PAS_H
#define INCLUDE_OV02_TSK_UMN_MAIL_PAS_H

/* Flags in the first byte of a mail's save record (UmnMailDataGet(mail_id)[0]). */
#define UMN_MAIL_FLAG_RECEIVED 0x01      /* the mail is in the mailbox and can be shown in a folder */
#define UMN_MAIL_FLAG_READ 0x02          /* the mail has been opened */
#define UMN_MAIL_FLAG_EVENT_SEEN 0x04    /* its event (UmnMailEventCheck) has been started */
#define UMN_MAIL_FLAG_REPLY 0x08         /* the mail is one of the replies chosen for another mail */
#define UMN_MAIL_REPLY_MASK 0x30         /* reply chosen for this mail: name index + 1 */
#define UMN_MAIL_REPLY_SHIFT 4
/* The mail that must be answered (a reply chosen, UMN_MAIL_REPLY_MASK) before the player can leave it. */
#define UMN_MAIL_ID_REPLY_REQUIRED 30

/* Functions and data used by other translation units. */
void UmnMail(int menu_id);

#endif /* INCLUDE_OV02_TSK_UMN_MAIL_PAS_H */
