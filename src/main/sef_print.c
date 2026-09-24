#include "common.h"

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

extern int printf(const char *format, ...);
extern int vsprintf(char *buffer, const char *format, va_list args);
extern unsigned char D_004DBB78[];

void tracePrint(const char *format, ...) {
    va_list args;
    char buffer[0x100];

    va_start(args, format);
    vsprintf(buffer, format, args);
    printf(D_004DBB78, buffer);
    va_end(args);
}

/*
 * These exported debug entry points contain no instructions beyond their
 * return sequence.  Their callers are not present in the bounded resident
 * call search, so the historical argument lists remain unresolved; the
 * unspecified C parameter lists preserve that uncertainty without inventing
 * a prototype.  The original symbol spellings are retained.
 */
void sefPrintVector()
{
}

void sefPrintMatrix()
{
}
