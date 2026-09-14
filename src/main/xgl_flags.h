/*
 * TU-local declarations of main/tu095 (src/main/xgl_flags.c).
 */

#ifndef SRC_MAIN_XGL_FLAGS_H
#define SRC_MAIN_XGL_FLAGS_H

/*
 * Target-specific bug-preserving storage view.  The original loads this
 * eight-byte window after copying only the calculated byte prefix.  This
 * union preserves the observed GCC 2.96 LA29/EE code path; it does not claim
 * portable ISO C semantics for a partially copied union object.
 */
union XglPackedFlagWindow {
    signed long long signed_value;
    unsigned char bytes[8];
};

int xglFlagsGet(int bit_offset, int bit_count);

int xglFlagsGet1(int bit_offset);

int xglFlagsGet2(int bit_offset);

int xglFlagsGet4(int bit_offset);

int xglFlagsGet8(int bit_offset);

int xglFlagsGet16(int bit_offset);

int xglFlagsGet32(int bit_offset);

unsigned long long xglFlagsGet64(int bit_offset);

#endif /* SRC_MAIN_XGL_FLAGS_H */
