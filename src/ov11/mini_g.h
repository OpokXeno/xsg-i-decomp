/*
 * TU-local declarations of ov11/tu001 (src/ov11/mini_g.c).
 *
 */

#ifndef SRC_OV11_MINI_G_H
#define SRC_OV11_MINI_G_H

/* Gwork is the original 0x250-byte local work block. Its current consumers
 * use a word-array view; the individual state fields remain partially modeled. */
extern char *SaveWork;

static int Gwork[148];

#endif /* SRC_OV11_MINI_G_H */
