/*
 * TU-local declarations of main/tu114 (src/main/ssd_3.c).
 */

#ifndef SRC_MAIN_SSD_3_H
#define SRC_MAIN_SSD_3_H

#include "main/ssd_init.h"

/*
 * RssdWork is the RSSD RPC/background-wave work area, main:0x004aa080
 * (owned by main/tu110, src/main/ssd_init.c, whose RssdCallFunc is still
 * INCLUDE_ASM). Its full RssdWorkFlags layout is not yet in the harvested
 * main/ssd_init.h, so this TU models only the leading `flags` word it reads
 * and writes.
 */
typedef struct RssdWorkArea {
    int flags; /* +0x000: bit 5 is the per-call success status (RSSD_FLAG_SUCCESS) */
} RssdWorkArea;

extern RssdWorkArea RssdWork;

/* RssdWorkArea.flags bit 5: set/cleared around an RssdCallFunc call to report the RPC's outcome. */
#define RSSD_FLAG_SUCCESS 0x20

/*
 * Sends one RSSD command over SIF RPC; see main:0x0023fff0 (still
 * INCLUDE_ASM, main/tu110). `request` may be null, otherwise its 32 bytes
 * are copied into the SIF RPC buffer, and `size` bytes of `data` are copied
 * after it.
 */
int RssdCallFunc(int command, RssdRequest *request, void *data, int size);

/*
 * RssdStrWork is the RSSD background-stream cache, main:0x004aa280, ELF
 * symbol size 0x20 bytes. SsdInitPcmStream stores its `channel` argument at
 * +0x04 right after opening the stream (main:0x00241a08); SsdInitVagStreamStereo
 * and SsdInitVagStreamMono do the same at +0x14 (main:0x00241c10,
 * main:0x00241c74). No recovered function reads either field back, so bytes
 * no recovered function accesses stay explicit unmodeled spans.
 */
typedef struct RssdStreamWork {
    unsigned char _unmodeled_00[4];    /* +0x00..0x03 */
    int pcm_channel;                   /* +0x04: SsdInitPcmStream's channel */
    unsigned char _unmodeled_08[0x0c]; /* +0x08..0x13 */
    int vag_channel;                   /* +0x14: SsdInitVagStreamStereo/Mono's channel */
    unsigned char _unmodeled_18[8];    /* +0x18..0x1f */
} RssdStreamWork;

extern RssdStreamWork RssdStrWork;

#endif /* SRC_MAIN_SSD_3_H */
