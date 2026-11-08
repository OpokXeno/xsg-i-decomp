#include "common.h"

typedef struct JSClassRecord JSClassRecord;

/* JSLookupEntry.type: what a primitive, class or method entry stands for. */
enum {
    JS_ENTRY_CONSTANT_INTEGER = 1,
    JS_ENTRY_CONSTANT_FLOAT = 2,
    JS_ENTRY_INTEGER = 3,
    JS_ENTRY_FLOAT = 4,
    JS_ENTRY_STRING = 5,
    JS_ENTRY_CLASS = 7,
    JS_ENTRY_METHOD = 8
};

/* JSCallArguments.type[]: the kind of each argument handed to a native method. */
enum {
    JS_ARGUMENT_INTEGER = 3,
    JS_ARGUMENT_FLOAT = 4,
    JS_ARGUMENT_STRING = 5,
    JS_ARGUMENT_OBJECT = 6
};

#define JS_PRIMITIVE_SIZE 0xC
#define JS_CLASS_RECORD_SIZE 0x6C

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

typedef void (*JSNativeMethod)(void);

/*
 * The script-class descriptor. JS_classAddMethod reads and increments
 * method_count through the +0x8 word and appends each entry (12 bytes)
 * to a method table that starts at +0xC. Only the two fields
 * JS_classSetup touches are modeled; the rest is unmodeled.
 */
typedef struct JSClass {
    unsigned char unmodeled_00[4];
    JSNativeMethod get_peer; /* +0x4 */
    int method_count;        /* +0x8 */
} JSClass;

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

extern char jsTokenDelimiters[];
extern char jsCallSeparators[];
extern char jsArgumentDelimiters[];

extern void *RSRC_alloc(int heap, int size, int tag);
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
extern double atof(const char *text);
extern int atoi(const char *text);

extern int JS_checkArgs(struct JSCallArguments *arguments,
                        int argument_count, char **argument_names);

static JSPrimitive js_type_int = {JS_ENTRY_INTEGER, {0}, {0}};

static JSPrimitive js_type_float = {JS_ENTRY_FLOAT, {0}, {0}};

static JSPrimitive js_type_string = {JS_ENTRY_STRING, {0}, {0}};

char jsTokenDelimiters[8] = ".[](),";

char jsCallSeparators[8] = ",()";

char jsArgumentDelimiters[8] = ".[]";

JSPrimitive *primitive;

int numClass;

JSClassRecord *classes;

int numPrimitive;

static char *tokenCurrent;

static char *tokenLimit;

static int tokenType;

/* Resets the class/primitive tables and allocates their backing storage from
 * the caller's resource heap, sized for the requested primitive and class
 * capacities. */
void JS_init(int heap, int primitiveCapacity, int classCapacity) {
    numClass = 0;
    numPrimitive = 0;
    primitive = RSRC_alloc(heap, primitiveCapacity * JS_PRIMITIVE_SIZE, 0);
    classes = RSRC_alloc(heap, classCapacity * JS_CLASS_RECORD_SIZE, 0);
}

/* Appends a class record and its class primitive entry, and returns the record. */
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
    primitive_entry->type = JS_ENTRY_CLASS;
    primitive_entry->key = (JSLookupKey){ .name = class_name };
    primitive_entry->data.class_record = class_record;
    return class_record;
}

int JS_loadConstInteger(int value, int auxiliary) {
    JSPrimitive *primitive_table = primitive;
    JSPrimitive *primitive_entry = &primitive_table[numPrimitive++];

    primitive_entry->type = JS_ENTRY_CONSTANT_INTEGER;
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

    method_entry->type = JS_ENTRY_METHOD;
    method_entry->key.name = method_name;
    method_entry->data.callback = callback;
}

/*
 * Finds name among the entry keys. Failing that, a numeric literal resolves
 * to the float or integer entry (float when it contains a '.'), and a
 * leading quote to the string entry. Returns 0 when nothing matches.
 */
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

/*
 * Parses one method call from the token stream: an optional object index,
 * the method name and its arguments. Invokes the method's callback and
 * returns 1, or returns 0 when the call does not match the syntax.
 */
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

    /* Optional object index before the method name: name[index]. */
    if (token_type == '[') {
        token = STR_tokenGetNext(0, jsTokenDelimiters);
        parsed_token_type = STR_tokenGetType();
        strcpy(object_name, token);
        token = STR_trim(object_name);
        entry = JS_findObject(token, primitive, numPrimitive);
        if (entry != 0) {
            if (parsed_token_type == ']') {
                if (entry->type == JS_ENTRY_INTEGER) {
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
        STR_tokenGetNext(0, jsTokenDelimiters);
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
    token = STR_tokenGetNext(0, jsTokenDelimiters);
    parsed_token_type = STR_tokenGetType();
    strcpy(object_name, token);
    token = STR_trim(object_name);
    if (class_record != 0) {
        entry = JS_findObject(token, class_record->methods,
                              class_record->method_count);
    }
    /* Collect the comma-separated arguments inside the parentheses. */
    if (parsed_token_type == '(') {
        do {
            token = STR_tokenGetNext(0, jsCallSeparators);
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

/*
 * Classifies each argument string (type and value into call_arguments) as a
 * primitive, a class with an optional index, or a plain string. Stores
 * argument_count in call_arguments->count.
 */
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
                case JS_ENTRY_CONSTANT_INTEGER:
                    argument_types[index] = JS_ARGUMENT_INTEGER;
                    argument_values[index].integer = entry->data.auxiliary;
                    break;
                case JS_ENTRY_CONSTANT_FLOAT:
                    argument_types[index] = JS_ARGUMENT_FLOAT;
                    argument_values[index].floating = entry->data.floating;
                    break;
                case JS_ENTRY_INTEGER:
                    argument_types[index] = JS_ARGUMENT_INTEGER;
                    argument_values[index].integer = atoi(argument);
                    break;
                case JS_ENTRY_FLOAT:
                    argument_types[index] = JS_ARGUMENT_FLOAT;
                    argument_values[index].floating = (float)atof(argument);
                    break;
                case JS_ENTRY_STRING:
                    argument_types[index] = JS_ARGUMENT_STRING;
                    argument_values[index].string = STR_trim2(argument, '"');
                    break;
                default:
                    argument_types[index] = 0;
                    argument_values[index].string = argument;
                    break;
                }
            } else {
                /* Not a primitive: try a class name, else keep the raw text. */
                argument = STR_tokenGetNext(argument, jsArgumentDelimiters);
                token_type = STR_tokenGetType();
                entry = JS_findObject(argument, primitive, numPrimitive);
                if (entry != 0) {
                    if (entry->type == JS_ENTRY_CLASS) {
                        class_record = entry->data.class_record;
                        peer_index = -1;
                        if (token_type == '[') {
                            argument = STR_tokenGetNext(0, jsArgumentDelimiters);
                            token_type = STR_tokenGetType();
                            strcpy(object_name, argument);
                            argument = STR_trim(object_name);
                            entry = JS_findObject(argument, primitive,
                                                  numPrimitive);
                            if (entry != 0 && token_type == ']') {
                                if (entry->type == JS_ENTRY_INTEGER) {
                                    peer_index = atoi(argument);
                                }
                            } else {
                                argument_types[index] = 0;
                                argument_values[index].integer = 0;
                            }
                        }
                        argument_types[index] = JS_ARGUMENT_OBJECT;
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

/* Runs each statement of source up to limit, calling every class method it names. */
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

        token = STR_tokenGetNext(line, jsTokenDelimiters);
        token_type = STR_tokenGetType();
        while (token != 0) {
            strcpy(object_name, token);
            entry = JS_findObject(STR_trim(object_name), primitive,
                                  numPrimitive);
            if (entry != 0 && entry->type == JS_ENTRY_CLASS &&
                entry->data.class_record != 0) {
                JS_callMethod(entry->data.class_record, token_type);
            }

            token = STR_tokenGetNext(0, jsTokenDelimiters);
            token_type = STR_tokenGetType();
        }
    }
}

int STR_tokenGetType(void) {
    return tokenType;
}

/*
 * Returns the next token of text (or of the previous text when text is 0),
 * ending it at one of delimiters. The delimiter that ended it is stored in
 * tokenType. Returns 0 when no token is left.
 */
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
        /* A quoted string is skipped as part of the token. */
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

/*
 * Copies the next ';'-terminated statement from source into destination,
 * dropping '//' and '#' comments. Returns the position after it, or 0 at
 * the end of the buffer.
 */
static char *STR_getLine(char *destination, char *source, char *limit) {
    int character;

    while ((character = *source) < 0x21 && character != 0) {
        source++;
    }

    if (source < limit) {
        character = *source++;
        if (character != ';') {
            do {
                if (character == '/' && *source == '/') {
                    for (;;) {
                        character = *source++;
                        if (character >= 128) {
                            source++;
                        } else if (character == '\n' || character == 0) {
                            break;
                        }
                    }
                } else if (character == '#') {
                    for (;;) {
                        character = *source++;
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
                character = *source++;
            } while (character != ';');
        }
    }

    source = source < limit ? source : 0;
    *destination = 0;
    return source;
}

/* Removes leading and trailing control characters and spaces in place. */
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

/* Removes leading and trailing copies of delimiter in place. */
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
