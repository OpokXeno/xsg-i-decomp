#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/sef_print", tracePrint);

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
