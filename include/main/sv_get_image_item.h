#ifndef INCLUDE_MAIN_SV_GET_IMAGE_ITEM_H
#define INCLUDE_MAIN_SV_GET_IMAGE_ITEM_H

/*
 * The SGs* family (SGsInitGifPacket, SGsOpenGifPacket, SGsCloseGifPacket,
 * SGsAddReg, SGsAddGifXYZ2, SGsSendPacket and others in this TU) shares one
 * accumulator: a growing array of 16-byte GIF records at `base`, `count`
 * records long. SGsOpenGifPacket/SGsCloseGifPacket/SGsAddReg (still
 * INCLUDE_ASM here, read for this evidence per docs/naming.md) show that
 * `tagIndex` is the record index SGsOpenGifPacket saves so
 * SGsCloseGifPacket can patch the GIFtag it opened, and `loopCount` is
 * OR-ed, unshifted, into that tag's low (NLOOP) bits by SGsCloseGifPacket;
 * both functions here leave it at 1.
 */
typedef struct SGsPacket {
    int count;              /* +0x00 */
    int tagIndex;           /* +0x04 */
    int loopCount;          /* +0x08 */
    union SGsGifRecord *base;     /* +0x0c, starts at the scratchpad (0x70000000) */
} SGsPacket;

void *svGetImageListItem(int type, unsigned int flags);

/* Functions and data used by other translation units. */
extern SGsPacket _packet;
void SGsInitEnv(void);

#endif /* INCLUDE_MAIN_SV_GET_IMAGE_ITEM_H */
