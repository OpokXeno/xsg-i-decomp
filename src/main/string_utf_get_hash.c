#include "common.h"
#include "string_utf_get_hash.h"
#include "main/string_utf_get_hash.h"

static int constStringCount = 0;

/* Returns the Java-style hash of the `length` bytes at `bytes`. */
int StringUtf_getHash(const char *bytes, int length)
{
    const unsigned char *current = (const unsigned char *)bytes;
    const unsigned char *end = current + length;
    int hash = 0;
    /* The byte is loaded into its own local before the multiply, which keeps the
       original order of the byte load and the accumulator shift. */
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

/* Allocates a string object with room for `length` bytes, a header and a terminator. */
void *StringUtf_create(int length)
{
    return xmalloc(length + VM_CONST_STRING_HEADER_SIZE + 1, VM_HEAP_STRING);
}

/* Returns the table entry equal to the `length` bytes at `bytes`, or 0 when there is none. */
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

/* Drops the leading entries of each bucket whose addresses are at or above `heap_boundary`. */
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

/*
 * Returns the interned string for the bytes, adding an entry when it is new.  The
 * table is created on first use.  The bytes are not copied; a negative length
 * means NUL-terminated.  Returns 0 for length 0.
 */
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

/* Like loadConstString, but a new entry gets its own copy of the bytes. */
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
