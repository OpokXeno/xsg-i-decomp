#include "common.h"

#include "find_native_method.h"

typedef struct ClassCacheEntry ClassCacheEntry;
struct JavaMethod;

extern ClassCacheEntry **classEntryPool;

int strcmp(const char *, const char *);

/* One row of the default_native[] table: a native method's exported name
 * and the address of its native implementation. */

typedef struct NativeMethodEntry {
    const char *name;
    void *function;
} NativeMethodEntry;

extern NativeMethodEntry default_native[];

/*
 * Looks up name in the default_native[] table, which a NULL name terminates,
 * and returns the matching native implementation, or NULL if the table is
 * empty or no entry's name matches.
 */

/*
 * A string constant of a class file's constant pool, as this function reads
 * one: resolveConstants walks the pool's tag bytes, hands every entry tagged
 * 8 to this function and retags it 0x18 once the slot holds the instance
 * built here.
 */

typedef struct StringConstant {
    u8 unmodeled_00[6];
    u16 length;     /* +0x6: the constant's UTF-8 byte count */
    char *bytes;    /* +0x8: interned UTF-8 character buffer */
} StringConstant;

/*
 * The value record a java.lang.String instance refers to.  Only the two
 * words written here are modeled: SCENE_start reads the length and text
 * buffer from +0x4 and +0x8 and passes them to loadConstString. The source
 * constant has the same interned UTF-8 layout as ConstString.
 */

typedef struct StringValue {
    u8 unmodeled_00[4];
    int length;     /* +0x4: the constant's length, widened to a word */
    char *bytes;    /* +0x8: the constant's UTF-8 character buffer */
} StringValue;

/*
 * The java.lang.String instance itself: the object header newObject seeds,
 * then the value record at +0x4 SCENE_start reads.
 */

typedef struct StringInstance {
    ObjectHeader header;
    StringValue *value;  /* +0x4 */
} StringInstance;

JavaClass *findClass(ClassCacheEntry *entry);

ClassCacheEntry *lookupClassEntry(ClassNameKey *name, void *classLoader);

int processClass(JavaClass *clazz, int targetState);

void *classFromSig(const char **cursor, void *classLoader);

static int sizeofDescripter(u8 **cursor);

typedef struct JavaMethod JavaMethod;

typedef struct JavaFieldEntry FieldEntry;

typedef struct DispatchTable DispatchTable;

typedef union ConstantPoolEntry ConstantPoolEntry;

/*
 * The full layout of the VM class record (include/shared.h's SceneClass is
 * the shared view of the same 0x40 bytes).
 */
struct JavaClass {
    ObjectHeader header;
    ClassNameKey *name;
    u16 accessFlags;
    u16 flags;
    void *classLoader;
    JavaClass *superClass;
    ConstantPoolEntry *constants;
    DispatchTable *classPointer;
    FieldEntry *fields;
    JavaMethod *methods;
    JavaClass **interfaces;
    u16 constantCount;
    u16 dispatchSize;
    u16 methodCount;
    u16 interfaceCount;
    u16 fieldCount;
    u16 staticFieldCount;
    u16 declaredInterfaceCount;
    s16 state;
    int instanceSize;
    ElementType *elementClass;
};

static JavaClass _dummyClass;

char *strcpy(char *, const char *);

extern ElementType *classBoolean;

extern ElementType *classByte;

extern ElementType *classChar;

extern ElementType *classShort;

extern ElementType *classInt;

extern ElementType *classLong;

extern ElementType *classFloat;

extern ElementType *classDouble;

extern ElementType *classVoid;

extern ClassNameKey *NAME_Constructor;

extern ClassNameKey *loadConstString(const char *bytes, int length);

extern ClassNameKey *loadConstString2(const char *bytes, int length);

static struct JavaMethod *findSuperMethod(struct JavaMethod *method, JavaClass *classEntry);

static void buildDispatchMethodTable(JavaClass *clazz);

static void allocStaticField(JavaClass *classEntry);

static void resolveInstanceField(JavaClass *classEntry);

static void resolveStaticField(JavaClass *clazz);

static int resolveConstants(JavaClass *classEntry);

extern JavaClass *getClass(JavaClass *classRef, JavaClass *owner);

extern ClassNameKey *NAME_Init;

extern ClassNameKey *TYPE_Void;

extern SceneVm *initVMThread;

extern SceneMethod *findMethodLocal(JavaClass *clazz, ClassNameKey *name, ClassNameKey *type);

extern void JNI_initThread(SceneVm *thread);

extern void JNI_callMethod(SceneVm *thread, SceneMethod *method, SceneObject *object, int *args);

union ConstantSlot {
    u8 *tags;
    StringConstant *stringConstant;
    StringInstance *stringObject;
};

enum {
    CLASS_STATE_MEMBERS_RESOLVED = 4,
    CLASS_STATE_BEFORE_CONSTANTS = 5,
    CLASS_STATE_CONSTANTS_RESOLVED = 6,
    CLASS_STATE_PREPARED = 8,
    CLASS_STATE_INITIALIZED = 11,
    CLASS_ACCESS_INTERFACE = 0x0200,
    METHOD_ACCESS_STATIC = 0x0008,
    FIELD_FLAG_HAS_CONSTANT_VALUE = 0x4000,
    CONSTANT_POOL_TAG_INTEGER = 3,
    CONSTANT_POOL_TAG_FLOAT = 4,
    CONSTANT_POOL_TAG_LONG = 5,
    CONSTANT_POOL_TAG_DOUBLE = 6,
    CONSTANT_POOL_TAG_STRING = 8,
    CONSTANT_POOL_TAG_RESOLVED_STRING = 0x18
};

struct ClassNameKey {
    u8 unmodeled_00[4];
    u16 hash;
    u8 unmodeled_06[2];
    const char *bytes;
};

struct ClassCacheEntry {
    ClassNameKey *name;
    void *classLoader;
    JavaClass *resolvedClass;
    struct ClassCacheEntry *next;
};

union ConstantPoolEntry {
    u8 *tags;
    void *object;
    int intValue;
    float floatValue;
    u8 byteValue;
    u16 halfValue;
    struct StringConstant *stringConstant;
    struct StringInstance *stringObject;
};

struct JavaFieldEntry {
    u8 unmodeled_00[4];
    u16 flags;
    u8 unmodeled_06[2];
    ElementType *type;
    union { int constantIndex; int size; int value; } payload;
    union { void *address; int offset; int initialValue; } location;
};

struct JavaMethod {
    ClassNameKey *name;
    ClassNameKey *descriptor;
    u8 unmodeled_08[4];
    JavaClass *owner;
    u16 accessFlags;
    s16 dispatchIndex;
    void *nativeFunction;
};

struct DispatchTable {
    JavaClass *clazz;
    JavaMethod *methods[1];
};

static int sizeofDescripterType(u8 **cursor);

void *findNativeMethod(const char *name)
{
    NativeMethodEntry *entry = default_native;

    if (entry->name != 0) {
        do {
            if (strcmp(entry->name, name) == 0) {
                return entry->function;
            }
            entry++;
        } while (entry->name != 0);
    }
    return 0;
}

void resolveNativeMethod(JavaMethod *method)
{
    char name[512];
    char *arrays[8];
    char *cursor;
    int arrayCount;
    void *function;
    char letter;
    int i;

    arrayCount = 0;
    __builtin_strcpy(name, "Java_");
    cursor = name + 5;
    strcpy(cursor, method->owner->name->bytes);
    letter = *cursor;
    if (letter != 0 && letter != ';') {
        do {
            if (letter == '/') {
                *cursor = '_';
            }
            cursor++;
            letter = *cursor;
        } while (letter != 0 && letter != ';');
    }
    *cursor++ = '_';
    strcpy(cursor, method->name->bytes);
    letter = *cursor;
    while (letter != 0) {
        if (letter == '/' || letter == '(' || letter == ')' || letter == ';') {
            *cursor = '_';
        }
        cursor++;
        letter = *cursor;
    }
    *cursor++ = '_';
    strcpy(cursor, method->descriptor->bytes);
    letter = *cursor;
    if (letter != 0) {
        do {
            if (letter == '[') {
                arrays[arrayCount++] = cursor;
                *cursor++ = 'a';
            } else {
                if (letter == '/' || letter == '(' || letter == ';') {
                    *cursor = '_';
                } else if (letter == ')') {
                    *cursor = 0;
                    break;
                }
                cursor++;
            }
            letter = *cursor;
        } while (letter != 0);
    }
    function = findNativeMethod(name);
    if (function == 0 && arrayCount > 0) {
        for (i = 0; i < arrayCount; i++) {
            cursor = arrays[i];
            *cursor = '_';
        }
        function = findNativeMethod(name);
    }
    if (function != 0) {
        method->nativeFunction = function;
    }
}

StringInstance *Const2JavaString(StringConstant *constant)
{
    StringInstance *string;
    StringValue *value;
    int length;
    char *bytes;

    value = xmalloc(sizeof(StringValue), 0xE);
    length = constant->length;
    bytes = constant->bytes;
    value->length = length;
    value->bytes = bytes;
    string = newObject(classString);
    string->value = value;
    return string;
}

void *newObject(SceneClass *scene_class)
{
    ObjectHeader *instance = xmalloc(scene_class->instance_size, 0xE);

    instance->classPointer = scene_class->instance_class_ref;
    return instance;
}

void *newClass(void)
{
    JavaClass *clazz;

    clazz = xmalloc(0x40, 0xE);
    clazz->instanceSize = 0;
    clazz->staticFieldCount = 0;
    clazz->classPointer = (void *)*(int *)&classClass->classPointer;
    return clazz;
}

void *newArray(ElementType *elementType, int length)
{
    ArrayHeader *array;
    void *classPointer;

    if (elementType->flags & 0x100) {
        array = xmalloc(elementType->elementSize * length + 0xC, 0xE);
    } else {
        array = xmalloc(length * 4 + 0xC, 0xE);
    }
    classPointer = lookupArray(elementType)->classPointer;
    array->length = length;
    array->classPointer = classPointer;
    array->data = (void *)(array + 1);
    return array;
}

int processClass(JavaClass *clazz, int targetState)
{
    JavaClass *super;
    u16 interfaceCount;
    int i;
    SceneMethod *init;
    int initializedState;
    int preparedState;

    preparedState = CLASS_STATE_PREPARED;
    if (clazz->state >= targetState) {
        return 1;
    }
    if (clazz->state < CLASS_STATE_MEMBERS_RESOLVED &&
        targetState >= CLASS_STATE_MEMBERS_RESOLVED) {
        allocStaticField(clazz);
        if (clazz->superClass != 0) {
            clazz->superClass = getClass(clazz->superClass, clazz);
            if (clazz->superClass != 0) {
                clazz->instanceSize = clazz->superClass->instanceSize;
            }
        }
        super = clazz->superClass;
        interfaceCount = clazz->declaredInterfaceCount;
        if (super != 0 && super != classObject) {
            if (clazz->accessFlags & CLASS_ACCESS_INTERFACE) {
                interfaceCount++;
            }
            interfaceCount += super->interfaceCount;
        }
        for (i = 0; i < clazz->declaredInterfaceCount; i++) {
            JavaClass *interface = getClass(clazz->interfaces[i], clazz);

            clazz->interfaces[i] = interface;
            if (interface != 0) {
                interfaceCount += interface->interfaceCount;
            }
        }
        clazz->interfaceCount = interfaceCount;
        clazz->header.classPointer = classClass->classPointer;
        resolveInstanceField(clazz);
        resolveStaticField(clazz);
        if (!(clazz->accessFlags & CLASS_ACCESS_INTERFACE)) {
            buildDispatchMethodTable(clazz);
        }
        clazz->state = CLASS_STATE_MEMBERS_RESOLVED;
    }
    if (clazz->state < CLASS_STATE_BEFORE_CONSTANTS &&
        targetState >= CLASS_STATE_BEFORE_CONSTANTS) {
        clazz->state = CLASS_STATE_BEFORE_CONSTANTS;
    }
    if (clazz->state < CLASS_STATE_CONSTANTS_RESOLVED &&
        targetState >= CLASS_STATE_CONSTANTS_RESOLVED) {
        resolveConstants(clazz);
        clazz->state = CLASS_STATE_CONSTANTS_RESOLVED;
    }
    initializedState = CLASS_STATE_INITIALIZED;
    if (clazz->state < preparedState && targetState >= CLASS_STATE_PREPARED) {
        clazz->state = CLASS_STATE_PREPARED;
    }
    if (clazz->state < initializedState && targetState >= CLASS_STATE_INITIALIZED) {
        clazz->state = initializedState;
        init = findMethodLocal(clazz, NAME_Init, TYPE_Void);
        if (init != 0) {
            JNI_initThread(initVMThread);
            JNI_callMethod(initVMThread, init, 0, 0);
        }
    }
    return 1;
}

static JavaMethod *findSuperMethod(JavaMethod *method, JavaClass *classEntry)
{
    JavaClass *superClass = classEntry->superClass;

    if (superClass != 0) {
        do {
            int remaining = superClass->methodCount;
            JavaMethod *candidate = superClass->methods;

            remaining--;
            if (remaining >= 0) {
                do {
                    if (candidate->name == method->name &&
                        candidate->descriptor == method->descriptor) {
                        return candidate;
                    }
                    remaining--;
                } while (remaining >= 0);
            }
            superClass = superClass->superClass;
        } while (superClass != 0);
    }
    return 0;
}

static void buildDispatchMethodTable(JavaClass *clazz)
{
    int i;
    JavaMethod *method;
    JavaMethod *overridden;
    DispatchTable *table;
    JavaMethod **slots;
    JavaMethod **inherited;
    int inheritedCount;
    int slot;
    ClassNameKey **constructorName = &NAME_Constructor;

    if (clazz->superClass != 0) {
        clazz->dispatchSize = clazz->superClass->dispatchSize;
    } else {
        clazz->dispatchSize = 0;
    }
    method = clazz->methods;
    for (i = clazz->methodCount; --i >= 0;) {
        if ((method->accessFlags & METHOD_ACCESS_STATIC) ||
            method->name == *constructorName) {
            method->dispatchIndex = -1;
        } else {
            overridden = findSuperMethod(method, clazz);
            if (overridden != 0) {
                method->dispatchIndex = overridden->dispatchIndex;
            } else {
                method->dispatchIndex = clazz->dispatchSize++;
            }
        }
    }
    table = xmalloc(clazz->dispatchSize * 4 + 8, 15);
    table->clazz = clazz;
    slots = table->methods;
    clazz->classPointer = table;
    if (clazz->superClass != 0) {
        inherited = clazz->superClass->classPointer->methods;
        inheritedCount = clazz->superClass->dispatchSize;
        for (slot = 0; slot < inheritedCount; slot++) {
            slots[slot] = inherited[slot];
        }
    }
    method = clazz->methods;
    for (i = clazz->methodCount; --i >= 0;) {
        if (method->dispatchIndex >= 0) {
            slots[method->dispatchIndex] = method;
        }
    }
}

static void resolveStaticField(JavaClass *clazz)
{
    FieldEntry *field;
    ConstantPoolEntry *address;
    ConstantPoolEntry *constants;
    int i;

    i = clazz->staticFieldCount;
    field = clazz->fields;
    constants = clazz->constants;
    for (; --i >= 0; field++) {
        int index;

        if (!(field->flags & FIELD_FLAG_HAS_CONSTANT_VALUE)) {
            continue;
        }
        index = field->payload.constantIndex;
        address = field->location.address;
        switch (constants[0].tags[index]) {
        case CONSTANT_POOL_TAG_INTEGER:
            if (field->type == classBoolean || field->type == classByte) {
                address->byteValue = constants[index].byteValue;
                field->payload.constantIndex = classByte->elementSize;
            } else {
                ElementType *shortType = classShort;

                if (field->type == classChar || field->type == shortType) {
                    address->halfValue = constants[index].halfValue;
                    field->payload.constantIndex = shortType->elementSize;
                } else {
                    address->intValue = constants[index].intValue;
                    field->payload.constantIndex = classInt->elementSize;
                }
            }
            break;
        case CONSTANT_POOL_TAG_FLOAT:
            address->floatValue = constants[index].floatValue;
            field->payload.constantIndex = classFloat->elementSize;
            break;
        case CONSTANT_POOL_TAG_LONG:
        case CONSTANT_POOL_TAG_DOUBLE:
            address[0].intValue = constants[index].intValue;
            address[1].intValue = constants[index + 1].intValue;
            field->payload.constantIndex = classLong->elementSize;
            break;
        case CONSTANT_POOL_TAG_STRING:
            constants[0].tags[index] = CONSTANT_POOL_TAG_RESOLVED_STRING;
            constants[index].object = Const2JavaString(constants[index].object);
            /* fall through: the slot now holds the String instance */
        case CONSTANT_POOL_TAG_RESOLVED_STRING:
            address->object = constants[index].object;
            field->payload.constantIndex = 4;
            break;
        }
    }
}

static void resolveInstanceField(JavaClass *classEntry)
{
    int instanceSize = classEntry->instanceSize;
    int remaining = classEntry->fieldCount - classEntry->staticFieldCount;
    FieldEntry *field = classEntry->fields + classEntry->staticFieldCount;

    if (instanceSize == 0) {
        instanceSize = 4;
    }
    while (--remaining >= 0) {
        int alignedSize = ((unsigned int)field->payload.size + 3U) / 4U * 4U;

        instanceSize = ((instanceSize + alignedSize - 1) / alignedSize) * alignedSize;
        field->location.offset = instanceSize;
        instanceSize += alignedSize;
        field++;
    }
    classEntry->instanceSize = instanceSize;
}

static int resolveConstants(JavaClass *classEntry)
{
    int index = 0;
    int constantCount = classEntry->constantCount;
    ConstantPoolEntry *constantPool = classEntry->constants;

    if (constantCount != 0) {
        u8 stringConstantTag = CONSTANT_POOL_TAG_STRING;
        u8 resolvedStringTag = CONSTANT_POOL_TAG_RESOLVED_STRING;
        ConstantPoolEntry *constant = constantPool;

        do {
            u8 *tags = constantPool[0].tags;
            u8 *tag = &tags[index];

            index++;
            if (*tag == stringConstantTag) {
                StringInstance *string;

                *tag = resolvedStringTag;
                string = Const2JavaString(constant->stringConstant);
                constant->stringObject = string;
                constantCount = classEntry->constantCount;
            }
            constant++;
        } while (index < constantCount);
    }
    return 1;
}

JavaClass *loadArray(ClassNameKey *name, void *classLoader)
{
    void *elementClass = getClassFromSignature(name->bytes + 1, classLoader);

    if (elementClass != 0) {
        return lookupArray(elementClass);
    }
    return 0;
}

JavaClass *loadClass(ClassNameKey *classKey, void *classLoader)
{
    ClassCacheEntry *entry;
    void *resolved;
    JavaClass *result;

    entry = lookupClassEntry(classKey, classLoader);
    resolved = entry->resolvedClass;
    if (resolved == 0) {
        if (classLoader == 0) {
            resolved = findClass(entry);
        }
        if (resolved != 0) {
            entry->resolvedClass = resolved;
        }
    }
    if (resolved == 0) {
        result = 0;
    } else {
        result = (processClass(resolved, 0xB) == 0) ? 0 : resolved;
    }
    return result;
}

void loadStaticClass(void *class_slot, const char *name)
{
    JavaClass **slot = class_slot;
    ClassCacheEntry *entry = lookupClassEntry(loadConstString(name, -1), 0);

    if (entry->resolvedClass == 0) {
        JavaClass *resolved = findClass(entry);

        if (resolved == 0) {
            *slot = 0;
            return;
        }
        entry->resolvedClass = resolved;
        *slot = resolved;
    }
    processClass(entry->resolvedClass, CLASS_STATE_INITIALIZED);
}

void reloadClassEntry(void *heapBoundary)
{
    ClassCacheEntry **bucket = classEntryPool;
    int bucketsRemaining = 511;

    do {
        ClassCacheEntry *entry = *bucket;

        if (entry != 0) {
            ClassCacheEntry *current = entry;

            if ((unsigned int)current >= (unsigned int)heapBoundary) {
                do {
                    current = current->next;
                } while (current != 0 &&
                         (unsigned int)current >= (unsigned int)heapBoundary);
            }
            *bucket = current;
        }
        bucket++;
        bucketsRemaining--;
    } while (bucketsRemaining >= 0);
}

ClassCacheEntry *lookupClassEntry(ClassNameKey *name, void *classLoader)
{
    ClassCacheEntry **pool = classEntryPool;
    ClassCacheEntry **slot;
    ClassCacheEntry *entry;
    unsigned int bucket;

    if (pool == 0) {
        int remaining = 511;

        pool = xmalloc(0x800, 2);
        classEntryPool = pool;
        slot = pool + 511;
        do {
            remaining--;
            *slot = 0;
            slot--;
        } while (remaining >= 0);
    }
    bucket = name->hash & 0x1FF;
    entry = pool[bucket];
    while (entry != 0 && (name != entry->name || classLoader != entry->classLoader)) {
        entry = entry->next;
    }
    if (entry != 0) {
        return entry;
    }
    entry = xmalloc(0x10, 2);
    entry->name = name;
    entry->classLoader = classLoader;
    entry->next = classEntryPool[bucket];
    entry->resolvedClass = 0;
    classEntryPool[bucket] = entry;
    return entry;
}

JavaClass *lookupArray(ElementType *elementType)
{
    extern const char D_004DC050[];
    extern const char D_004DC058[];
    extern const char D_004DC060[];
    char name[1024];
    ClassCacheEntry *entry;
    ClassNameKey *key;

    if (elementType->flags & 0x100) {
        sprintf(name, D_004DC050, elementType->typeCode);
    } else {
        const char *typeName = elementType->name->bytes;

        if (typeName[0] == '[') {
            sprintf(name, D_004DC058, typeName);
        } else {
            sprintf(name, D_004DC060, typeName);
        }
    }
    key = loadConstString2(name, -1);
    entry = lookupClassEntry(key, elementType->classLoader);
    if (entry->resolvedClass == 0) {
        JavaClass *clazz = newClass();
        void *classLoader = elementType->classLoader;
        JavaClass *parentClass = classObject;

        clazz->accessFlags = 0x411;
        entry->resolvedClass = clazz;
        clazz->name = key;
        clazz->classLoader = classLoader;
        clazz->state = 1;
        clazz->superClass = parentClass;
        buildDispatchMethodTable(clazz);
        clazz->elementClass = elementType;
    }
    return entry->resolvedClass;
}

void *classFromSig(const char **cursor, void *classLoader)
{
    const char *start;
    const char *end;

    switch (*(*cursor)++) {
    case 'V':
        return classVoid;
    case 'Z':
        return classBoolean;
    case 'B':
        return classByte;
    case 'C':
        return classChar;
    case 'S':
        return classShort;
    case 'I':
        return classInt;
    case 'J':
        return classLong;
    case 'F':
        return classFloat;
    case 'D':
        return classDouble;
    case '[':
        return lookupArray(classFromSig(cursor, classLoader));
    case 'L':
        start = *cursor;
        end = start;
        while (*end != 0 && *end != ';') {
            end++;
        }
        *cursor = end;
        if (*end != 0) {
            *cursor = end + 1;
        }
        return loadClass(loadConstString(start, end - start), classLoader);
    }
    return 0;
}

SceneClass *getClassFromSignature(const char *signature, void *classLoader)
{
    const char *cursor = signature;

    return classFromSig(&cursor, classLoader);
}

static void allocStaticField(JavaClass *classEntry)
{
    int remaining = classEntry->staticFieldCount;

    if (remaining != 0) {
        FieldEntry *field = classEntry->fields;
        int storageOffset = 0;

        while (--remaining >= 0) {
            int alignedSize = ((unsigned int)field->payload.size + 3U) / 4U * 4U;

            storageOffset += alignedSize;
            field->payload.size = ((storageOffset - 1) / alignedSize) * alignedSize;
            field++;
        }
        {
            unsigned char *storage = xmalloc(storageOffset, 10);

            field = classEntry->fields;
            remaining = classEntry->staticFieldCount;
            while (--remaining >= 0) {
                int initialValue = field->location.initialValue;

                storageOffset = field->payload.size;
                field->payload.value = initialValue;
                field->location.address = storage + storageOffset;
                field++;
            }
        }
    }
}

void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType)
{
    u8 *cursor = descriptor;

    *paramSize = (s16)sizeofDescripter(&cursor);
    *returnType = (s8)*cursor;
    *returnSize = (s16)sizeofDescripter(&cursor);
}

static int sizeofDescripter(u8 **cursor)
{
    int size = 0;
    int typeSize;

    while (1) {
        typeSize = sizeofDescripterType(cursor);
        if (typeSize < 0) {
            return size;
        }
        size += typeSize;
    }
}

int instanceOf(SceneClass *fromClass, SceneClass *toClass);

/*
 * Array assignability: strips matching array dimensions, then compares the
 * element types. The recursive instanceOf call handles object element types.
 */
static inline u8 arrayInstanceOf(JavaClass *source, JavaClass *target)
{
    while (source->name->bytes[0] == '[' && target->name->bytes[0] == '[') {
        source = (JavaClass *)source->elementClass;
        target = (JavaClass *)target->elementClass;
    }
    if (source->name->bytes[0] == '[') {
        return 0;
    }
    if (source->flags & 0x100) {
        return source == target;
    }
    if (target->name->bytes[0] == '[') {
        return source == classObject;
    }
    if (target->flags & 0x100) {
        return 0;
    }
    return instanceOf((SceneClass *)source, (SceneClass *)target);
}

static inline int implementsInterface(JavaClass *from, JavaClass *to)
{
    int i;
    int interfaceCount = from->interfaceCount;

    for (i = 0; i < interfaceCount; i++) {
        if (from->interfaces[i] == to) {
            return 1;
        }
    }
    return 0;
}

static inline int inheritsFrom(JavaClass *from, JavaClass *to)
{
    JavaClass *superClass;

    for (superClass = from->superClass; superClass != 0; superClass = superClass->superClass) {
        if (superClass == to) {
            return 1;
        }
    }
    return 0;
}

int instanceOf(SceneClass *fromClass, SceneClass *toClass)
{
    JavaClass *from = (JavaClass *)fromClass;
    JavaClass *to = (JavaClass *)toClass;

    if (from == to) {
        return 1;
    }
    if (to->name->bytes[0] == '[') {
        return arrayInstanceOf(from, to);
    }
    if (to->accessFlags & 0x200) {
        return implementsInterface(from, to);
    }
    return inheritsFrom(from, to);
}

static int sizeofDescripterType(u8 **cursor)
{
    u8 *descriptorByte = *cursor;
    int size;

    switch ((char)*descriptorByte) {
    case '(':
    case 'V':
        size = 0;
        break;
    case '[':
        size = 1;
        if ((char)*descriptorByte == '[') {
            do {
                descriptorByte++;
            } while ((char)*descriptorByte == '[');
        }
        if ((char)*descriptorByte == 'L') {
            do {
                descriptorByte++;
            } while ((char)*descriptorByte != ';');
        }
        break;
    case 'L':
        size = 1;
        while ((char)*descriptorByte != ';') {
            descriptorByte++;
        }
        break;
    case 'D':
    case 'J':
        size = 2;
        break;
    case 'B':
    case 'C':
    case 'F':
    case 'I':
    case 'S':
    case 'Z':
        size = 1;
        break;
    case 0:
    default:
        size = -1;
        break;
    }
    *cursor = descriptorByte + 1;
    return size;
}
