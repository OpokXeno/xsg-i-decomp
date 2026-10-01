#include "common.h"

typedef struct JSClassRecord JSClassRecord;
typedef struct JSLookupEntry JSLookupEntry;
typedef void *(*JSGetPeer)(int index);
typedef void (*JSMethodCallback)(void *arguments);

typedef union JSLookupKey {
    int integer;
    float floating;
    const char *name;
} JSLookupKey;

typedef union JSLookupData {
    int auxiliary;
    JSClassRecord *class_record;
    JSMethodCallback callback;
} JSLookupData;

/* Primitive values, lookup entries and method entries occupy three words.
 * Their second and third words carry different roles for integer constants,
 * named class lookups and native methods. */
struct JSLookupEntry {
    int type;
    JSLookupKey key;
    JSLookupData data;
};
typedef JSLookupEntry JSPrimitive;
typedef JSLookupEntry JSMethodEntry;

struct JSClassRecord {
    const char *name;
    JSGetPeer get_peer;
    int method_count;
    JSMethodEntry methods[8];
};

extern void *RSRC_alloc(int heap, int size, int tag);

extern int numClass;
extern int numPrimitive;
extern void *primitive;
extern void *classes;
extern JSPrimitive js_type_int;
extern JSPrimitive js_type_float;
extern JSPrimitive js_type_string;
extern const char D_004DA590[];
extern char *tokenCurrent;
extern char *tokenLimit;
extern int tokenType;

extern int strcmp(const char *left, const char *right);
extern char *strchr(const char *text, int character);
extern unsigned int strlen(const char *text);
extern char *strcpy(char *destination, const char *source);
extern int JS_callMethod(JSClassRecord *class_record, int token_type);
extern char *STR_tokenGetNext(char *text, const char *delimiters);
extern int STR_tokenGetType(void);
static char *STR_getLine(char *destination, char *source, char *limit);
static char *STR_trim(char *text);
static char *STR_trim2(char *text, int delimiter);

/* Resets the class/primitive tables and allocates their backing storage from
 * the caller's resource heap, sized for the requested primitive and class
 * capacities. */
void JS_init(int heap, int primitiveCapacity, int classCapacity) {
    numClass = 0;
    numPrimitive = 0;
    primitive = RSRC_alloc(heap, primitiveCapacity * 0xC, 0);
    classes = RSRC_alloc(heap, classCapacity * 0x6C, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_loadClass);

int JS_loadConstInteger(int value, int auxiliary) {
    JSPrimitive *primitive_table = primitive;
    JSPrimitive *primitive_entry = &primitive_table[numPrimitive++];

    primitive_entry->type = 1;
    primitive_entry->key.integer = value;
    primitive_entry->data.auxiliary = auxiliary;
    return 0;
}

typedef void (*JSNativeMethod)(void);

/*
 * The script-class descriptor JS_loadClass hands back. JS_classAddMethod
 * (still assembler in this TU, 0x0026b260) proves the layout: it reads and
 * increments method_count through this same pointer's +0x8 word, then
 * multiplies the pre-increment count by 12 to place the next entry, so the
 * method table it appends to starts at +0xC. Only the two fields
 * JS_classSetup itself touches are modeled; the rest is unmodeled.
 */
typedef struct JSClass {
    unsigned char unmodeled_00[4];
    JSNativeMethod get_peer; /* +0x4 */
    int method_count;        /* +0x8 */
} JSClass;

/* Configures a freshly loaded script class: installs its native
 * getPeer/getHandle callback and resets its method table. */
void JS_classSetup(JSClass *js_class, JSNativeMethod get_peer) {
    js_class->get_peer = get_peer;
    js_class->method_count = 0;
}

void JS_classAddMethod(JSClassRecord *js_class, const char *method_name,
                       JSMethodCallback callback) {
    JSMethodEntry *method_entry = &js_class->methods[js_class->method_count++];

    method_entry->type = 8;
    method_entry->key.name = method_name;
    method_entry->data.callback = callback;
}

JSLookupEntry *JS_findObject(const char *name, JSLookupEntry *entries,
                             int entry_count) {
    int index;
    char first;
    char next;

    for (index = 0; index < entry_count; index++, entries++) {
        if (entries->key.name != 0 && strcmp(entries->key.name, name) == 0) {
            return entries;
        }
    }

    first = name[0];
    next = name[1];
    if ((unsigned int)(first - '0') < 10u ||
        (first == '-' && (unsigned int)(next - '0') < 10u)) {
        if (strchr(name, '.') != 0) {
            return &js_type_float;
        }
        return &js_type_int;
    }

    if (first == '"') {
        return &js_type_string;
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_callMethod);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_checkArgs);

void JS_exec(char *source, char *limit) {
    char line[1024];
    char object_name[512];
    char *token;
    int token_type;
    JSLookupEntry *entry;

    for (;;) {
        source = STR_getLine(line, source, limit);
        if (source == 0) {
            break;
        }

        token = STR_tokenGetNext(line, D_004DA590);
        token_type = STR_tokenGetType();
        while (token != 0) {
            strcpy(object_name, token);
            entry = JS_findObject(STR_trim(object_name), primitive,
                                  numPrimitive);
            if (entry != 0 && entry->type == 7 &&
                entry->data.class_record != 0) {
                JS_callMethod(entry->data.class_record, token_type);
            }

            token = STR_tokenGetNext(0, D_004DA590);
            token_type = STR_tokenGetType();
        }
    }
}

extern int tokenType;

int STR_tokenGetType(void) {
    return tokenType;
}

char *STR_tokenGetNext(char *text, const char *delimiters) {
    char *token_start;
    char *cursor;
    char *end;
    int delimiter_count;
    int delimiter_index;
    unsigned char character;
    int matched_delimiter = 0;

    if (text != 0) {
        tokenCurrent = text;
        tokenLimit = &text[strlen(text)];
    }
    delimiter_count = delimiters != 0 ? strlen(delimiters) : 0;

    cursor = tokenCurrent;
    end = tokenLimit;
    token_start = cursor;
    while (cursor < end) {
        character = (unsigned char)cursor[0];
        if (character == '"') {
            cursor++;
            for (;;) {
                character = (unsigned char)cursor[0];
                cursor++;
                if (character == '"' || character == 0) {
                    break;
                }
            }
            continue;
        } else if ((unsigned int)(character - '0') < 10U ||
                   (character == '-' &&
                    (unsigned char)((unsigned char)cursor[1] + 208) < 10)) {
            cursor++;
            if (character == '0' && (unsigned char)cursor[0] == 'x') {
                cursor++;
            }

            do {
                character = (unsigned char)*cursor++;
            } while ((unsigned int)(character - 'A') < 6U ||
                     (unsigned int)(character - 'a') < 6U ||
                     (unsigned int)(character - '0') < 10U ||
                     character == '-' || character == '.');

            cursor--;
        }

        if (character == 0) {
            break;
        }

        matched_delimiter = -1;
        for (delimiter_index = 0; delimiter_index < delimiter_count;
             delimiter_index++) {
            if (character != (unsigned char)delimiters[delimiter_index]) {
                continue;
            }
            tokenType = character;
            matched_delimiter = delimiter_index;
            cursor[0] = 0;
            break;
        }

        if (matched_delimiter >= 0) {
            break;
        }
        cursor++;
    }

    if (cursor >= end) {
        if (matched_delimiter >= 0) {
            return 0;
        }
        if (token_start == cursor) {
            return 0;
        }
        tokenCurrent = cursor;
        return token_start;
    }
    tokenCurrent = cursor + 1;
    return token_start;
}

/* Copy a line, omitting comments beginning with // or #. */
static char *STR_getLine(char *destination, char *source, char *limit) {
    int character;

    while ((character = (signed char)*source) < 0x21 && character != 0) {
        source++;
    }

    if (source < limit) {
        character = (signed char)*source++;
        if (character != ';') {
            do {
                if (character == '/' && *source == '/') {
                    for (;;) {
                        character = (signed char)*source++;
                        if (character >= 128) {
                            source++;
                        } else if (character == '\n' || character == 0) {
                            break;
                        }
                    }
                } else if (character == '#') {
                    for (;;) {
                        character = (signed char)*source++;
                        if (character >= 128) {
                            source++;
                        } else if (character == '\n' || character == 0) {
                            break;
                        }
                    }
                } else {
                    *destination++ = character;
                }

                if (source >= limit) {
                    break;
                }
                character = (signed char)*source++;
            } while (character != ';');
        }
    }

    source = source < limit ? source : 0;
    *destination = 0;
    return source;
}

static char *STR_trim(char *text) {
    int index;

    while (text[0] < 0x21 && text[0] != 0) {
        text++;
    }

    index = strlen(text) - 1;
    if (index >= 0) {
        do {
            if (text[index] < 0x21) {
                text[index] = 0;
            }
            index--;
        } while (index >= 0);
    }
    return text;
}

static char *STR_trim2(char *text, int delimiter) {
    int index;

    while (text[0] == delimiter && delimiter != 0) {
        text++;
    }

    index = strlen(text) - 1;
    if (index >= 0) {
        do {
            if (text[index] == delimiter) {
                text[index] = 0;
            }
            index--;
        } while (index >= 0);
    }
    return text;
}
