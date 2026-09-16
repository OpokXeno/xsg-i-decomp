#include "common.h"
#include "string_utf_get_hash.h"

int StringUtf_getHash(const char *bytes, int length)
{
    const unsigned char *current = (const unsigned char *)bytes;
    const unsigned char *end = current + length;
    int hash = 0;
    /* Separate load keeps the fetched byte in its own register before the
       multiply: gcc 2.96 schedules the accumulator shift ahead of the byte
       load when the byte is read directly in the expression (form01, this
       attempt, differs 73.125% at StringUtf_getHash); the separate local
       reproduces the original lbu-before-sll order (CP-0222, form04,
       attempt-be432885387a). */
    unsigned char byte;

    if (current < end) {
        do {
            byte = *current;
            current++;
            hash = hash * 31 + byte;
        } while (current < end);
    }

    return hash;
}

void *StringUtf_create(int length)
{
    return xmalloc(length + 13, 20);
}

ConstString *findConstString(const char *bytes, int length)
{
    int bucket_index = StringUtf_getHash(bytes, length) & 0x1ff;
    ConstString *entry = constStringTable[bucket_index];

    while (entry != 0) {
        if (entry->length == length) {
            if (memcmp(entry->bytes, bytes, length) == 0)
                break;
        }
        entry = entry->next;
    }

    return entry;
}

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

SceneString *loadConstString(const char *bytes, int length)
{
    int hash;
    int count;
    ConstString *entry;

    if (constStringTable == 0) {
        ConstString **bucket;
        int remaining = 511;

        constStringTable = xmalloc(2048, 2);
        bucket = &constStringTable[511];
        do {
            *bucket = 0;
            bucket--;
            remaining--;
        } while (remaining >= 0);
    }

    if (length == 0)
        return 0;
    if (length < 0)
        length = strlen(bytes);

    hash = StringUtf_getHash(bytes, length) & 0x1ff;
    entry = constStringTable[hash];
    while (entry != 0) {
        if (entry->length == length &&
            memcmp(entry->bytes, bytes, length) == 0)
            break;
        entry = entry->next;
    }
    if (entry != 0) {
        return (SceneString *)entry;
    }

    entry = xmalloc(12, 20);
    entry->next = constStringTable[hash];

    count = constStringCount;
    count++;
    entry->hash = hash;
    entry->bytes = (char *)bytes;
    entry->length = length;
    constStringTable[hash] = entry;
    constStringCount = count;
    return (SceneString *)entry;
}

INCLUDE_ASM("asm/main/nonmatchings/string_utf_get_hash", loadConstString2);
