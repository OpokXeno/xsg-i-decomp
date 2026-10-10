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
    float floating;
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

JSPrimitive *primitive;

int numClass;

JSClassRecord *classes;

int numPrimitive;

static JSPrimitive js_type_int = {3, {0}, {0}};

static JSPrimitive js_type_float = {4, {0}, {0}};

static JSPrimitive js_type_string = {5, {0}, {0}};

extern char D_004DA590[];
char D_004DA590[8] = ".[](),";

static char *tokenCurrent;

static char *tokenLimit;

static int tokenType;

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

extern char D_004DA598[];

extern char D_004DA5A0[];

extern double atof(const char *text);

union JSArgumentValue {
    int integer;
    float floating;
    char *string;
    void *object;
};

struct JSCallArguments {
    int count;
    int unmodeled_04;
    int type[8];
    union JSArgumentValue value[8];
};

extern int atoi(const char *text);

extern int JS_checkArgs(struct JSCallArguments *arguments,
                        int argument_count, char **argument_names);

void JS_init(int heap, int primitiveCapacity, int classCapacity) {
    numClass = 0;
    numPrimitive = 0;
    primitive = RSRC_alloc(heap, primitiveCapacity * 0xC, 0);
    classes = RSRC_alloc(heap, classCapacity * 0x6C, 0);
}

JSClassRecord *JS_loadClass(const char *class_name) {
    int *class_count = &numClass;
    int *primitive_count = &numPrimitive;
    JSClassRecord **class_table = &classes;
    JSPrimitive **primitive_table = &primitive;
    int class_index = *class_count;
    int primitive_index = *primitive_count;
    JSClassRecord *class_record = &(*class_table)[class_index];
    JSPrimitive *primitive_entry = &(*primitive_table)[primitive_index];

    *class_count = class_index + 1;
    *primitive_count = primitive_index + 1;

    class_record->name = class_name;
    primitive_entry->type = 7;
    primitive_entry->key = (JSLookupKey){ .name = class_name };
    primitive_entry->data.class_record = class_record;
    return class_record;
}

int JS_loadConstInteger(int value, int auxiliary) {
    JSPrimitive *primitive_table = primitive;
    JSPrimitive *primitive_entry = &primitive_table[numPrimitive++];

    primitive_entry->type = 1;
    primitive_entry->key.integer = value;
    primitive_entry->data.auxiliary = auxiliary;
    return 0;
}

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

int JS_callMethod(JSClassRecord *class_record, int token_type) {
    char object_name[128];
    struct JSCallArguments call_arguments;
    char *argument_names[8];
    char **argument_slots;
    int peer_index = -1;
    JSLookupEntry *entry = 0;
    char *token;
    int argument_count;
    int index;
    int parsed_token_type;

    if (token_type == '[') {
        token = STR_tokenGetNext(0, D_004DA590);
        parsed_token_type = STR_tokenGetType();
        strcpy(object_name, token);
        token = STR_trim(object_name);
        entry = JS_findObject(token, primitive, numPrimitive);
        if (entry != 0) {
            if (parsed_token_type == ']') {
                if (entry->type == 3) {
                    peer_index = atoi(token);
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        } else {
            return 0;
        }
        STR_tokenGetNext(0, D_004DA590);
        token_type = STR_tokenGetType();
    }
    if (token_type != '.') {
        return 0;
    }
    argument_count = 0;
    argument_slots = argument_names;
    for (index = 7; index >= 0; index--) {
        argument_slots[index] = 0;
    }
    token = STR_tokenGetNext(0, D_004DA590);
    parsed_token_type = STR_tokenGetType();
    strcpy(object_name, token);
    token = STR_trim(object_name);
    if (class_record != 0) {
        entry = JS_findObject(token, class_record->methods,
                              class_record->method_count);
    }
    if (parsed_token_type == '(') {
        do {
            token = STR_tokenGetNext(0, D_004DA598);
            parsed_token_type = STR_tokenGetType();
            if (token == 0) {
                break;
            }
            if (token[0] != '\0') {
                argument_slots[argument_count] = token;
                argument_count++;
            }
        } while (parsed_token_type != ')');
    }
    call_arguments.type[7] = (int)class_record->get_peer(peer_index);
    JS_checkArgs(&call_arguments, argument_count, argument_slots);
    if (entry->data.callback != 0) {
        entry->data.callback(&call_arguments);
    }
    return 1;
}

int JS_checkArgs(struct JSCallArguments *call_arguments, int argument_count,
                 char **argument_names) {
    int index;
    int *argument_types = call_arguments->type;
    union JSArgumentValue *argument_values = call_arguments->value;
    char **argument_slots = argument_names;
    int count = argument_count;
    int token_type;
    int peer_index;
    char object_name[128];
    char *argument;
    char *original_argument;
    JSLookupEntry *entry;
    JSClassRecord *class_record;

    index = 0;
    if (count > 0) {
        do {
            JSPrimitive *primitive_table;
            int *primitive_count;
            original_argument = STR_trim(argument_slots[index]);
            primitive_table = primitive;
            primitive_count = &numPrimitive;
            argument_slots[index] = original_argument;
            argument = original_argument;
            entry = JS_findObject(argument, primitive_table, *primitive_count);
            if (entry != 0) {
                switch (entry->type) {
                case 1:
                    argument_types[index] = 3;
                    argument_values[index].integer = entry->data.auxiliary;
                    break;
                case 2:
                    argument_types[index] = 4;
                    argument_values[index].floating = entry->data.floating;
                    break;
                case 3:
                    argument_types[index] = 3;
                    argument_values[index].integer = atoi(argument);
                    break;
                case 4:
                    argument_types[index] = 4;
                    argument_values[index].floating = (float)atof(argument);
                    break;
                case 5:
                    argument_types[index] = 5;
                    argument_values[index].string = STR_trim2(argument, '"');
                    break;
                default:
                    argument_types[index] = 0;
                    argument_values[index].string = argument;
                    break;
                }
            } else {
                argument = STR_tokenGetNext(argument, D_004DA5A0);
                token_type = STR_tokenGetType();
                entry = JS_findObject(argument, primitive, numPrimitive);
                if (entry != 0) {
                    if (entry->type == 7) {
                        class_record = entry->data.class_record;
                        peer_index = -1;
                        if (token_type == '[') {
                            argument = STR_tokenGetNext(0, D_004DA5A0);
                            token_type = STR_tokenGetType();
                            strcpy(object_name, argument);
                            argument = STR_trim(object_name);
                            entry = JS_findObject(argument, primitive,
                                                  numPrimitive);
                            if (entry != 0 && token_type == ']') {
                                if (entry->type == 3) {
                                    peer_index = atoi(argument);
                                }
                            } else {
                                argument_types[index] = 0;
                                argument_values[index].integer = 0;
                            }
                        }
                        argument_types[index] = 6;
                        argument_values[index].object =
                            class_record->get_peer(peer_index);
                    }
                } else {
                    argument_types[index] = 0;
                    argument_values[index].string = argument;
                }
            }
            index++;
        } while (index < count);
    }
    if (argument_count - 1 > 0) {
        index = argument_count - 1;
        do {
            index--;
        } while (index != 0);
    }
    call_arguments->count = count;
    return 0;
}

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
