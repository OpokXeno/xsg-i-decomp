#ifndef INCLUDE_MAIN_XGL_FLAGS_H
#define INCLUDE_MAIN_XGL_FLAGS_H

extern void xglFlagsInitial(void);

/* Functions and data used by other translation units. */
int xglFlagsGet1(int bit_offset);
int xglFlagsSet1(int bit_offset, int value);
int xglFlagsSet(int bit_offset, int bit_count, int value);

#endif /* INCLUDE_MAIN_XGL_FLAGS_H */
