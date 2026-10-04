#include "common.h"
#include "shared.h"

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

extern int vprintf(const char *format, va_list args);
static char dstr[0x100];
const char D_004CCA40[] = "\033[1;36mMDMSG:\033[m %s\n";
const char D_004CCA58[] = "\033[1;35mMDMSG:\033[m %s\n";

void MOutputDebugString(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    sprintf(dstr, D_004CCA40, format);
    vprintf(dstr, args);
    va_end(args);
}

void MOutputDebugStringWarn(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    sprintf(dstr, D_004CCA58, format);
    vprintf(dstr, args);
    va_end(args);
}
