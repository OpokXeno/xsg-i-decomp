#include "common.h"
#include "shared.h"

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

extern int vprintf(const char *format, va_list args);
extern char dstr[0x100];
extern const char D_004CCA40[];
extern const char D_004CCA58[];

void MOutputDebugString(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    sprintf(dstr, D_004CCA40, format);
    vprintf(dstr, args);
    va_end(args);
}

INCLUDE_ASM("asm/main/nonmatchings/m_output_debug_string", MOutputDebugStringWarn);
