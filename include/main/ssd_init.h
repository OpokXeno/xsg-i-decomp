#ifndef INCLUDE_MAIN_SSD_INIT_H
#define INCLUDE_MAIN_SSD_INIT_H

/*
 * One argument word of an RSSD request. Most commands pass plain integers;
 * SsdTransferSampling/SsdTransferSamplingNext (main/tu112) and the sequence
 * data wrappers of main/tu113 pass a buffer address in the same slot, so the
 * word is a union of the two views (both are 32-bit on the EE).
 */
typedef union RssdRequestWord {
    int value;
    void *pointer;
} RssdRequestWord;

/*
 * The 32-byte RSSD RPC request record.
 *
 * RssdCallFunc (main:0x0023fff0, still INCLUDE_ASM) copies all 32 bytes of a
 * non-null request into the SIF RPC buffer RssdWork.response_source with
 * four unaligned ldl/ldr -> sdl/sdr pairs (0x00240050..0x0024008c), then
 * writes two fields of that copy from its own arguments: the +0x00 halfword
 * (`sh $21,0($17)`, command) and the +0x0c word (`sw $16,12($17)`, size).
 * No caller writes header[0..3] (+0x00..+0x0f), but every wrapper reserves
 * the whole record on its stack (all of them have a 0x30-byte frame, however
 * many argument words the command uses).
 * arg[0..3] (+0x10..+0x1f) are the command's own argument words.
 */
typedef struct RssdRequest {
    int header[4];
    RssdRequestWord arg[4];
} RssdRequest;

#endif /* INCLUDE_MAIN_SSD_INIT_H */
