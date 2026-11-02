#ifndef SRC_OV11_CAJINO_GET_H
#define SRC_OV11_CAJINO_GET_H

/* One eight-byte viewer text record: the sample's title and info strings.
 * Both fields are pointers into .rodata; text[0] is the title, text[1] the info. */
struct ViewerText {
    char *text[2];
};

/* 30 records (0xF0 bytes) at ov11:0x00a0ba20. */
extern struct ViewerText ViewerText[30];

#endif
