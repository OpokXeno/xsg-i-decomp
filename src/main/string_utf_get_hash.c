#include "common.h"
#include "string_utf_get_hash.h"

INCLUDE_ASM("asm/main/nonmatchings/string_utf_get_hash", StringUtf_getHash);

INCLUDE_ASM("asm/main/nonmatchings/string_utf_get_hash", StringUtf_create);

INCLUDE_ASM("asm/main/nonmatchings/string_utf_get_hash", findConstString);

void reloadConstString(void *heap_boundary)
{
    ConstString **bucket = constStringTable;
    int remaining = 511;

    do {
        ConstString *entry = *bucket;

        if (entry != 0) {
            ConstString *current = entry;

            if ((unsigned int)current >= (unsigned int)heap_boundary) {
                do {
                current = current->next;
                } while (current != 0 &&
                         (unsigned int)current >= (unsigned int)heap_boundary);
            }
            *bucket = current;
        }

        remaining--;
        bucket++;
    } while (remaining >= 0);
}

INCLUDE_ASM("asm/main/nonmatchings/string_utf_get_hash", loadConstString);

INCLUDE_ASM("asm/main/nonmatchings/string_utf_get_hash", loadConstString2);
