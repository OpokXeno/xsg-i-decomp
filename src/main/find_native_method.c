#include "common.h"
#include "main/init_vm.h"
#include "main/scene_1.h"
#include "find_native_method.h"
#include "main/find_native_method.h"
#include "main/jni.h"
#include "main/string_utf_get_hash.h"

typedef struct ClassCacheEntry ClassCacheEntry;

/* One row of the default_native[] table: a native method's exported name
 * and the address of its native implementation. */
typedef struct NativeMethodEntry {
    const char *name;
    void *function;
} NativeMethodEntry;

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


struct ClassCacheEntry {
    SceneString *name;
    void *classLoader;
    JavaClass *resolvedClass;
    struct ClassCacheEntry *next;
};

extern ClassCacheEntry **classEntryPool;
extern NativeMethodEntry default_native[];
extern JavaClass *classByte;
extern JavaClass *classVoid;
int strcmp(const char *, const char *);
JavaClass *findClass(ClassCacheEntry *entry);
ClassCacheEntry *lookupClassEntry(SceneString *name, void *classLoader);
int processClass(JavaClass *clazz, int targetState);
void *classFromSig(const char **cursor, void *classLoader);
static int sizeofDescripter(u8 **cursor);
char *strcpy(char *, const char *);
static struct JavaMethod *findSuperMethod(struct JavaMethod *method, JavaClass *classEntry);
static void buildDispatchMethodTable(JavaClass *clazz);
static void allocStaticField(JavaClass *classEntry);
static void resolveInstanceField(JavaClass *classEntry);
static void resolveStaticField(JavaClass *clazz);
static int resolveConstants(JavaClass *classEntry);
extern JavaClass *getClass(JavaClass *classRef, JavaClass *owner);
extern JavaMethod *findMethodLocal(JavaClass *clazz, SceneString *name, SceneString *type);
extern void JNI_initThread(SceneVm *thread);
static int sizeofDescripterType(u8 **cursor);
static JavaClass _dummyClass;

/*
 * Looks up name in the default_native[] table, which a NULL name terminates,
 * and returns the matching native implementation, or NULL if the table is
 * empty or no entry's name matches.
 */
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

/*
 * Builds the JNI name Java_<class>_<method>_<descriptor> for method and looks it up
 * in default_native[]. When that fails and the descriptor has array arguments, it
 * retries with the array marks replaced by '_'. Sets method->native when found.
 */
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
        method->native = function;
    }
}

/* Returns a java.lang.String instance that refers to the constant's length and UTF-8 buffer. */
StringInstance *Const2JavaString(StringConstant *constant)
{
    StringInstance *string;
    StringValue *value;
    int length;
    char *bytes;

    value = xmalloc(sizeof(StringValue), VM_HEAP_OBJECT);
    length = constant->length;
    bytes = constant->bytes;
    value->length = length;
    value->bytes = bytes;
    string = newObject(classString);
    string->value = value;
    return string;
}

/* Allocates an instance of clazz's instance size and sets its class pointer. */
void *newObject(JavaClass *clazz)
{
    ObjectHeader *instance = xmalloc(clazz->instanceSize, VM_HEAP_OBJECT);

    instance->classPointer = clazz->classPointer;
    return instance;
}

/* Allocates an empty JavaClass record. */
void *newClass(void)
{
    JavaClass *clazz;

    clazz = xmalloc(sizeof(JavaClass), VM_HEAP_OBJECT);
    clazz->instanceSize = 0;
    clazz->staticFieldCount = 0;
    clazz->classPointer = (void *)*(int *)&classClass->classPointer;
    return clazz;
}

/* Allocates an array of length elements: primitive elements use their size, references 4 bytes. */
void *newArray(JavaClass *elementType, int length)
{
    ArrayHeader *array;
    void *classPointer;

    if (elementType->flags & VM_CLASS_FLAG_PRIMITIVE) {
        array = xmalloc(elementType->elementSize * length + sizeof(ArrayHeader), VM_HEAP_OBJECT);
    } else {
        array = xmalloc(length * 4 + sizeof(ArrayHeader), VM_HEAP_OBJECT);
    }
    classPointer = lookupArray(elementType)->classPointer;
    array->length = length;
    array->classPointer = classPointer;
    array->data = array + 1;
    return array;
}

/*
 * Advances clazz through the loading states up to targetState: members, constants,
 * prepared, and finally initialized, which runs the class's <init> method.
 */
int processClass(JavaClass *clazz, int targetState)
{
    JavaClass *super;
    u16 interfaceCount;
    int i;
    JavaMethod *init;
    int initializedState;
    int preparedState;

    preparedState = CLASS_STATE_PREPARED;
    if (clazz->state >= targetState) {
        return 1;
    }
    /* Members: resolve the superclass, interfaces, fields and dispatch table. */
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
            if (clazz->accessFlags & JVM_ACC_INTERFACE) {
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
        if (!(clazz->accessFlags & JVM_ACC_INTERFACE)) {
            buildDispatchMethodTable(clazz);
        }
        clazz->state = CLASS_STATE_MEMBERS_RESOLVED;
    }
    /* Constants: move through the pre-constants state, then resolve the pool. */
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
    /* Initialization: run the <init> method with no arguments once. */
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

/* Returns the superclass method with the same name and descriptor as method, searching up the chain, or 0. */
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

/*
 * Assigns dispatch indices to clazz's methods: an override reuses the superclass
 * index, static methods and constructors get -1. Then builds the dispatch table.
 */
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
    SceneString **constructorName = &NAME_Constructor;

    if (clazz->superClass != 0) {
        clazz->dispatchSize = clazz->superClass->dispatchSize;
    } else {
        clazz->dispatchSize = 0;
    }
    method = clazz->methods;
    for (i = clazz->methodCount; --i >= 0;) {
        if ((method->accessFlags & JVM_ACC_STATIC) ||
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
    table = xmalloc(clazz->dispatchSize * 4 + 8, VM_HEAP_DISPATCH_TABLE);
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

/* Stores the constant values of static fields into their storage; string constants are interned first. */
static void resolveStaticField(JavaClass *clazz)
{
    SceneField *field;
    ConstantPoolEntry *address;
    ConstantPoolEntry *constants;
    int i;

    i = clazz->staticFieldCount;
    field = clazz->fields;
    constants = clazz->constants;
    for (; --i >= 0; field++) {
        int index;

        if (!(field->flags & VM_FIELD_FLAG_HAS_CONSTANT_VALUE)) {
            continue;
        }
        index = field->payload.constant_index;
        address = field->location.address;
        switch (constants[0].tags[index]) {
        case JVM_CONSTANT_INTEGER:
            if (field->type.resolved == classBoolean || field->type.resolved == classByte) {
                address->byteValue = constants[index].byteValue;
                field->payload.constant_index = classByte->elementSize;
            } else {
                JavaClass *shortType = classShort;

                if (field->type.resolved == classChar || field->type.resolved == shortType) {
                    address->halfValue = constants[index].halfValue;
                    field->payload.constant_index = shortType->elementSize;
                } else {
                    address->intValue = constants[index].intValue;
                    field->payload.constant_index = classInt->elementSize;
                }
            }
            break;
        case JVM_CONSTANT_FLOAT:
            address->floatValue = constants[index].floatValue;
            field->payload.constant_index = classFloat->elementSize;
            break;
        case JVM_CONSTANT_LONG:
        case JVM_CONSTANT_DOUBLE:
            address[0].intValue = constants[index].intValue;
            address[1].intValue = constants[index + 1].intValue;
            field->payload.constant_index = classLong->elementSize;
            break;
        case JVM_CONSTANT_STRING:
            constants[0].tags[index] = VM_CONSTANT_RESOLVED_STRING;
            constants[index].object = Const2JavaString(constants[index].object);
            /* fall through: the slot now holds the String instance */
        case VM_CONSTANT_RESOLVED_STRING:
            address->object = constants[index].object;
            field->payload.constant_index = 4;
            break;
        }
    }
}

/* Lays out the instance fields after the superclass's size, each aligned to its rounded size. */
static void resolveInstanceField(JavaClass *classEntry)
{
    int instanceSize = classEntry->instanceSize;
    int remaining = classEntry->fieldCount - classEntry->staticFieldCount;
    SceneField *field = classEntry->fields + classEntry->staticFieldCount;

    if (instanceSize == 0) {
        instanceSize = 4;
    }
    while (--remaining >= 0) {
        int alignedSize = (field->payload.size + 3U) / 4U * 4U;

        instanceSize = ((instanceSize + alignedSize - 1) / alignedSize) * alignedSize;
        field->location.offset = instanceSize;
        instanceSize += alignedSize;
        field++;
    }
    classEntry->instanceSize = instanceSize;
}

/* Replaces each string constant of the pool with a java.lang.String instance and retags it as resolved. */
static int resolveConstants(JavaClass *classEntry)
{
    int index = 0;
    int constantCount = classEntry->constantCount;
    ConstantPoolEntry *constantPool = classEntry->constants;

    if (constantCount != 0) {
        u8 stringConstantTag = JVM_CONSTANT_STRING;
        u8 resolvedStringTag = VM_CONSTANT_RESOLVED_STRING;
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

/* Returns the array class whose element type is named by name, a descriptor without its leading '['. */
JavaClass *loadArray(SceneString *name, void *classLoader)
{
    void *elementClass = getClassFromSignature(name->bytes + 1, classLoader);

    if (elementClass != 0) {
        return lookupArray(elementClass);
    }
    return 0;
}

/* Returns the class for classKey, loading it through the class cache when needed, or 0 when it cannot be found. */
JavaClass *loadClass(SceneString *classKey, void *classLoader)
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
        result = (processClass(resolved, CLASS_STATE_INITIALIZED) == 0) ? 0 : resolved;
    }
    return result;
}

/* Loads the class named name into *class_slot and initializes it; the slot is set to 0 when the class is not found. */
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

/* Unlinks the leading cache entries of each bucket that lie at or above heapBoundary. */
void reloadClassEntry(void *heapBoundary)
{
    ClassCacheEntry **bucket = classEntryPool;
    int bucketsRemaining = VM_HASH_LAST_BUCKET;

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

/* Returns the cache entry for name and classLoader, creating the lookup table and the entry when missing. */
ClassCacheEntry *lookupClassEntry(SceneString *name, void *classLoader)
{
    ClassCacheEntry **pool = classEntryPool;
    ClassCacheEntry **slot;
    ClassCacheEntry *entry;
    unsigned int bucket;

    if (pool == 0) {
        int remaining = VM_HASH_LAST_BUCKET;

        pool = xmalloc(VM_HASH_BUCKETS * sizeof(ClassCacheEntry *), VM_HEAP_LOOKUP_TABLE);
        classEntryPool = pool;
        slot = pool + VM_HASH_LAST_BUCKET;
        do {
            remaining--;
            *slot = 0;
            slot--;
        } while (remaining >= 0);
    }
    bucket = name->hash & VM_HASH_MASK;
    entry = pool[bucket];
    while (entry != 0 && (name != entry->name || classLoader != entry->classLoader)) {
        entry = entry->next;
    }
    if (entry != 0) {
        return entry;
    }
    entry = xmalloc(sizeof(ClassCacheEntry), VM_HEAP_LOOKUP_TABLE);
    entry->name = name;
    entry->classLoader = classLoader;
    entry->next = classEntryPool[bucket];
    entry->resolvedClass = 0;
    classEntryPool[bucket] = entry;
    return entry;
}

/* Returns the array class for elementType, creating and caching it on first use. */
JavaClass *lookupArray(JavaClass *elementType)
{
    extern const char primitiveArrayDescriptorFormat[];
    extern const char nestedArrayDescriptorFormat[];
    extern const char objectArrayDescriptorFormat[];
    char name[1024];
    ClassCacheEntry *entry;
    SceneString *key;

    if (elementType->flags & VM_CLASS_FLAG_PRIMITIVE) {
        sprintf(name, primitiveArrayDescriptorFormat, elementType->typeCode);
    } else {
        const char *typeName = elementType->name->bytes;

        if (typeName[0] == '[') {
            sprintf(name, nestedArrayDescriptorFormat, typeName);
        } else {
            sprintf(name, objectArrayDescriptorFormat, typeName);
        }
    }
    key = loadConstString2(name, -1);
    entry = lookupClassEntry(key, elementType->classLoader);
    if (entry->resolvedClass == 0) {
        JavaClass *clazz = newClass();
        void *classLoader = elementType->classLoader;
        JavaClass *parentClass = classObject;

        clazz->accessFlags = JVM_ACC_PUBLIC | JVM_ACC_FINAL | JVM_ACC_ABSTRACT;
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

/* Reads one type from *cursor, advances past it and returns its class. */
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

JavaClass *getClassFromSignature(const char *signature, void *classLoader)
{
    const char *cursor = signature;

    return classFromSig(&cursor, classLoader);
}

/* Lays out the static fields in one allocated block and copies each field's initial value into it. */
static void allocStaticField(JavaClass *classEntry)
{
    int remaining = classEntry->staticFieldCount;

    if (remaining != 0) {
        SceneField *field = classEntry->fields;
        int storageOffset = 0;

        while (--remaining >= 0) {
            int alignedSize = (field->payload.size + 3U) / 4U * 4U;

            storageOffset += alignedSize;
            field->payload.size = ((storageOffset - 1) / alignedSize) * alignedSize;
            field++;
        }
        {
            unsigned char *storage = xmalloc(storageOffset, VM_HEAP_FIELDS);

            field = classEntry->fields;
            remaining = classEntry->staticFieldCount;
            while (--remaining >= 0) {
                int initialValue = field->location.initial_value;

                storageOffset = field->payload.size;
                field->payload.value = initialValue;
                field->location.address = storage + storageOffset;
                field++;
            }
        }
    }
}

/* Computes the parameter size, return type and return size of a method descriptor. */
void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType)
{
    u8 *cursor = descriptor;

    *paramSize = sizeofDescripter(&cursor);
    *returnType = (s8)*cursor;
    *returnSize = sizeofDescripter(&cursor);
}

/* Sums the slot sizes of consecutive types from cursor, stopping at ')' or the end of the descriptor. */
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

/*
 * Array assignability: strips matching array dimensions, then compares the
 * element types. The recursive instanceOf call handles object element types.
 */
static inline u8 arrayInstanceOf(JavaClass *source, JavaClass *target)
{
    while (source->name->bytes[0] == '[' && target->name->bytes[0] == '[') {
        source = source->elementClass;
        target = target->elementClass;
    }
    if (source->name->bytes[0] == '[') {
        return 0;
    }
    if (source->flags & VM_CLASS_FLAG_PRIMITIVE) {
        return source == target;
    }
    if (target->name->bytes[0] == '[') {
        return source == classObject;
    }
    if (target->flags & VM_CLASS_FLAG_PRIMITIVE) {
        return 0;
    }
    return instanceOf(source, target);
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

/* Returns 1 when an instance of from can be used as to: same class, array assignable, interface or superclass. */
int instanceOf(JavaClass *from, JavaClass *to)
{
    if (from == to) {
        return 1;
    }
    if (to->name->bytes[0] == '[') {
        return arrayInstanceOf(from, to);
    }
    if (to->accessFlags & JVM_ACC_INTERFACE) {
        return implementsInterface(from, to);
    }
    return inheritsFrom(from, to);
}

/*
 * Returns the slot size of the type at *cursor: 0 for '(' and void, 2 for J and D,
 * 1 for other types, or -1 at the end of the descriptor. Advances past the type.
 */
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char primitiveArrayDescriptorFormat[8] = "[%c";

const char nestedArrayDescriptorFormat[8] = "[%s";

const char objectArrayDescriptorFormat[8] = "[L%s;";
