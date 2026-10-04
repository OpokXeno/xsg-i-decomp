/*
 * TU-local declarations of main/tu158 (src/main/endou_test.c).
 */

#ifndef SRC_MAIN_ENDOU_TEST_H
#define SRC_MAIN_ENDOU_TEST_H

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern float I2F(int value);

extern char callback_file_name[64];

extern const char not_found_format[];

extern const char read_error_format[];

#define progress_percent 0.009999999776f

extern float transfer_progress;

#endif /* SRC_MAIN_ENDOU_TEST_H */
