#include "common.h"
#include "string_utf_get_hash.h"
#include "main/string_utf_get_hash.h"

static int constStringCount = 0;

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
            hash = hash * JAVA_HASH_MULTIPLIER + byte;
        } while (current < end);
    }

    return hash;
}

void *StringUtf_create(int length)
{
    return xmalloc(length + VM_CONST_STRING_HEADER_SIZE + 1, VM_HEAP_STRING);
}

ConstString *findConstString(const char *bytes, int length)
{
    int bucket_index = StringUtf_getHash(bytes, length) & VM_HASH_MASK;
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
    int remaining = VM_HASH_LAST_BUCKET;

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
        int remaining = VM_HASH_LAST_BUCKET;

        constStringTable = xmalloc(VM_HASH_BUCKETS * sizeof(ConstString *), VM_HEAP_LOOKUP_TABLE);
        bucket = &constStringTable[VM_HASH_LAST_BUCKET];
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

    hash = StringUtf_getHash(bytes, length) & VM_HASH_MASK;
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

    entry = xmalloc(VM_CONST_STRING_HEADER_SIZE, VM_HEAP_STRING);
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

SceneString *loadConstString2(const char *bytes, int length)
{
    unsigned int bucket_index;
    ConstString *entry;

    if (constStringTable == 0) {
        ConstString **bucket;
        int remaining = VM_HASH_LAST_BUCKET;

        constStringTable = xmalloc(VM_HASH_BUCKETS * sizeof(ConstString *), VM_HEAP_LOOKUP_TABLE);
        bucket = &constStringTable[VM_HASH_LAST_BUCKET];
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

    bucket_index = StringUtf_getHash(bytes, length) & VM_HASH_MASK;
    entry = constStringTable[bucket_index];
    while (entry != 0) {
        if (entry->length == length &&
            memcmp(entry->bytes, bytes, length) == 0)
            break;
        entry = entry->next;
    }
    if (entry != 0)
        return (SceneString *)entry;

    entry = xmalloc(length + VM_CONST_STRING_HEADER_SIZE + 1, VM_HEAP_STRING);
    entry->bytes = entry->data;
    memcpy(entry->bytes, bytes, length);
    entry->bytes[length] = '\0';
    entry->length = length;

    {
        ConstString *previous_head = constStringTable[bucket_index];

        entry->hash = bucket_index;
        constStringCount++;
        entry->next = previous_head;
        constStringTable[bucket_index] = entry;
    }

    return (SceneString *)entry;
}
