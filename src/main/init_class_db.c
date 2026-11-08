#include "common.h"
#include "shared.h"
#include "init_class_db.h"
#include "main/init_vm.h"
#include "main/find_native_method.h"
#include "main/scene_1.h"
#include "main/data_buffer.h"

typedef struct ClassFileMemberHeader {
    u16 access_flags;
    u16 name_index;
    u16 descriptor_index;
} ClassFileMemberHeader;

typedef struct ClassFilePath {
    unsigned char unmodeled_00[6];
    unsigned short length;
    const char *name;
} ClassFilePath;

typedef struct ClassFileEntry {
    ClassFilePath *path;
} ClassFileEntry;

typedef struct ClassFile {
    unsigned char unmodeled_00[8];
    unsigned char *bytes;
    int length;
} ClassFile;

const char classFileExtension[];
extern void *xmalloc(int size, int type);
SceneField *addField(JavaClass *class_info, const ClassFileMemberHeader *field_info);
JavaMethod *addMethod(JavaClass *class_info, ClassFileMemberHeader *method_info);
extern char *strncpy(char *dest, const char *src, unsigned int count);
extern char *strcat(char *dest, const char *src);
extern ClassFile *PDB_findFile(int volume, const char *name);

/* getClassFromSignature resolves both reference classes and primitive type records. */
extern JavaClass *getClassFromSignature(const char *signature, void *class_loader);

void setFieldValue(SceneField *field, unsigned int value);

ConstString **constStringTable;

int classDB[VM_CLASS_DB_SLOTS];

#ifndef MAIN_INIT_CLASS_DB_PRIVATE_H
#define MAIN_INIT_CLASS_DB_PRIVATE_H

#include "shared.h"
#include "main/string_utf_get_hash.h"
#include "main/init_class_db.h"

/* This entry point is defined with this prototype by string_utf_get_hash.c. */
extern SceneString *loadConstString(const char *bytes, int length);

#endif

void initClassDB(void)
{
    int i;

    for (i = VM_CLASS_DB_SLOTS - 1; i >= 0; i--) {
        classDB[i] = 0;
    }
}

/* Finds the class file named by entry in the class-path volumes and reads it. Returns 0 when no volume has it. */
JavaClass *findClass(ClassFileEntry *entry)
{
    char filename[256];
    DataBuffer buffer;
    int volume;
    int index;

    strncpy(filename, entry->path->name, entry->path->length);
    filename[entry->path->length] = '\0';
    strcat(filename, classFileExtension);

    for (index = 0; index < VM_CLASS_DB_SLOTS; index++) {
        volume = classDB[index];
        if (volume != 0) {
            ClassFile *file = PDB_findFile(volume, filename);
            if (file != 0) {
                JavaClass *class_info = newClass();
                DataBuffer_init(&buffer, file->bytes, file->length, 1);
                readClass(&buffer, class_info, 0);
                return class_info;
            }
        }
    }
    return 0;
}

/* Parses a class file from buffer into class_info. Returns class_info, or 0 when the magic number is wrong. */
JavaClass *readClass(DataBuffer *buffer, JavaClass *class_info,
                     void *class_loader)
{
    u32 access_flags;
    u32 this_class_index;
    u32 super_class_index;

    if (DataBuffer_getUIntAt(buffer) != JVM_CLASS_MAGIC) {
        return 0;
    }
    DataBuffer_getUShortAt(buffer); /* minor_version, unused */
    DataBuffer_getUShortAt(buffer); /* major_version, unused */
    readConstantPool(buffer, class_info);
    access_flags = DataBuffer_getUShortAt(buffer);
    this_class_index = DataBuffer_getUShortAt(buffer);
    super_class_index = DataBuffer_getUShortAt(buffer);
    setupClass(class_info, this_class_index, super_class_index, access_flags,
               class_loader);
    readInterfaces(buffer, class_info);
    readFields(buffer, class_info);
    readMethods(buffer, class_info);
    readAttributes(buffer, class_info, 0);
    return class_info;
}

/* Reads a Code attribute into method: records the bytecode position and length, skips the exception table, then reads the nested attributes. */
void addCode(DataBuffer *buffer, JavaClass *class_info, JavaMethod *method,
             u32 attributeLength)
{
    u16 max_stack;
    u16 max_locals;
    u16 exception_table_count;
    u32 code_length;

    max_stack = DataBuffer_getUShortAt(buffer);
    max_locals = DataBuffer_getUShortAt(buffer);
    code_length = DataBuffer_getUIntAt(buffer);
    if (code_length != 0) {
        method->code = buffer->position;
        DataBuffer_seek(buffer, code_length);
    } else {
        method->code = 0;
    }
    exception_table_count = DataBuffer_getUShortAt(buffer);
    method->maxStack = max_stack;
    method->maxLocals = max_locals;
    method->codeLength = (u16) code_length;
    DataBuffer_seek(buffer, exception_table_count * 8);
    readAttributes(buffer, class_info, method);
}

/*
 * Adds one field to class_info->fields and resolves its type. Static fields fill
 * the array from the front, instance fields from the back.
 */
SceneField *addField(JavaClass *class_info,
                     const ClassFileMemberHeader *field_info)
{
    SceneField *field;
    SceneField *fields;
    ConstantPoolEntry descriptor_value;
    ConstantPoolEntry *constant_pool;
    SceneString *descriptor;
    JavaClass *resolved_type;
    const char *signature;
    u16 field_index;

    constant_pool = class_info->constants;
    if ((field_info->access_flags & 8) != 0) {
        field_index = class_info->staticFieldCount++;
    } else {
        class_info->fieldCount--;
        field_index = class_info->fieldCount;
    }

    fields = class_info->fields;
    field = &fields[field_index];
    field->name = constant_pool[field_info->name_index].object;
    field->flags = field_info->access_flags;
    descriptor_value = constant_pool[field_info->descriptor_index];
    descriptor = descriptor_value.object;
    signature = descriptor->bytes;
    if ((signature[0] == 'L') || (signature[0] == '[')) {
        field->type.descriptor = descriptor;
        field->payload.size = 4;
        field->flags |= VM_FIELD_FLAG_UNRESOLVED_TYPE;
    } else {
        resolved_type = getClassFromSignature(signature, 0);
        field->type.resolved = resolved_type;
        field->payload.size = resolved_type->elementSize;
    }
    return field;
}

/* Adds one method to class_info->methods, taking its name and descriptor from the constant pool, and computes its parameter and return sizes. */
JavaMethod *addMethod(JavaClass *class_info,
                      ClassFileMemberHeader *method_info)
{
    ConstantPoolEntry *constant_pool;
    JavaMethod *method;
    JavaMethod *methods;
    u16 method_index;
    u16 access_flags;
    u16 name_index;
    u16 descriptor_index;
    void *descriptor_object;
    signed char return_type[16];

    method_index = class_info->methodCount;
    methods = class_info->methods;
    method = &methods[method_index];
    class_info->methodCount = method_index + 1;
    constant_pool = class_info->constants;
    access_flags = method_info->access_flags;
    method->accessFlags = access_flags;
    name_index = method_info->name_index;
    descriptor_index = method_info->descriptor_index;
    method->name = constant_pool[name_index].object;
    descriptor_object = constant_pool[descriptor_index].object;
    method->owner = class_info;
    method->descriptor = descriptor_object;
    method->dispatchIndex = 0xffff;
    method->maxStack = 0;
    method->maxLocals = 0;
    if (method->name == NAME_Constructor) {
        method->accessFlags |= VM_ACC_CONSTRUCTOR;
    }
    methodDescripter((u8 *)method->descriptor->bytes,
                     &method->parameterSize, &method->returnSize,
                     return_type);
    return method;
}

/* Reads the field table and adds each field to class_info. */
static void readFields(DataBuffer *buffer, JavaClass *class_info)
{
    u16 field_count;
    u16 field_index;
    ClassFileMemberHeader field_info;

    field_count = DataBuffer_getUShortAt(buffer);
    class_info->staticFieldCount = 0;
    class_info->fieldCount = field_count;
    if (field_count != 0) {
        class_info->fields = xmalloc(field_count * sizeof(SceneField), VM_HEAP_FIELDS);
        field_index = 0;
    } else {
        class_info->fields = 0;
        return;
    }
    while (field_index < field_count) {
        SceneField *field;

        field_info.access_flags = DataBuffer_getUShortAt(buffer);
        field_info.name_index = DataBuffer_getUShortAt(buffer);
        field_info.descriptor_index = DataBuffer_getUShortAt(buffer);
        field = addField(class_info, &field_info);
        readAttributes(buffer, class_info, field);
        field_index++;
    }
    class_info->fieldCount = field_count;
}

/* Reads the method table and adds each method to class_info. */
static void readMethods(DataBuffer *buffer, JavaClass *class_info)
{
    u16 method_count;
    u16 method_index;
    ClassFileMemberHeader method_info;

    method_count = DataBuffer_getUShortAt(buffer);
    class_info->methodCount = 0;
    if (method_count != 0) {
        class_info->methods = xmalloc(method_count * sizeof(JavaMethod), VM_HEAP_METHODS);
        method_index = 0;
    } else {
        class_info->methods = 0;
        return;
    }
    while (method_index < method_count) {
        JavaMethod *method;

        method_info.access_flags = DataBuffer_getUShortAt(buffer);
        method_info.name_index = DataBuffer_getUShortAt(buffer);
        method_info.descriptor_index = DataBuffer_getUShortAt(buffer);
        method = addMethod(class_info, &method_info);
        readAttributes(buffer, class_info, method);
        method_index++;
    }
}

/* Reads the interface table into class_info->interfaces. */
static void readInterfaces(DataBuffer *buffer, JavaClass *class_info)
{
    u16 interface_count;
    u16 interface_index;

    interface_count = DataBuffer_getUShortAt(buffer);
    class_info->declaredInterfaceCount = interface_count;
    if (interface_count != 0) {
        class_info->interfaces = xmalloc(interface_count * sizeof(JavaClass *), VM_HEAP_INTERFACES);
        interface_index = 0;
    } else {
        class_info->interfaces = 0;
        return;
    }
    while (interface_index < interface_count) {
        /* The slot holds the interface's constant pool index until
           processClass resolves it to the class. */
        u32 class_index = DataBuffer_getUShortAt(buffer);

        class_info->interfaces[interface_index] = (JavaClass *)class_index;
        interface_index++;
    }
}

/* Reads an attribute table for attribute_target. Code is parsed, ConstantValue sets the field value, and other attributes are skipped. */
static void readAttributes(DataBuffer *buffer, JavaClass *class_info,
                           void *attribute_target)
{
    ConstantPoolEntry tag_vector;
    ConstantPoolEntry constant_pool_value;
    ConstantPoolEntry *constant_pool;
    unsigned short attribute_count;
    unsigned short attribute_index;
    unsigned short name_index;
    unsigned int attribute_length;
    void *attribute_name;

    attribute_count = DataBuffer_getUShortAt(buffer);
    if (attribute_count == 0) {
        return;
    }

    constant_pool = class_info->constants;
    tag_vector = constant_pool[0];
    for (attribute_index = 0; attribute_index < attribute_count;
         attribute_index++) {
        name_index = DataBuffer_getUShortAt(buffer);
        attribute_length = DataBuffer_getUIntAt(buffer);
        switch (tag_vector.tags[name_index]) {
        case JVM_CONSTANT_UTF8:
            constant_pool_value = constant_pool[name_index];
            attribute_name = constant_pool_value.object;
            if (attribute_name == ATTR_Code) {
                addCode(buffer, class_info, (JavaMethod *)attribute_target,
                        attribute_length);
            } else if ((attribute_name == ATTR_Exceptions) ||
                       (attribute_name == ATTR_LineNumberTable)) {
                DataBuffer_seek(buffer, attribute_length);
            } else if (attribute_name == ATTR_ConstantValue) {
                setFieldValue((SceneField *)attribute_target,
                              DataBuffer_getUShortAt(buffer));
            } else {
                DataBuffer_seek(buffer, attribute_length);
            }
            break;
        default:
            DataBuffer_seek(buffer, attribute_length);
            break;
        }
    }
}

/* Reads the constant pool into class_info->constants; the tag bytes are stored after the entries. */
static void readConstantPool(DataBuffer *buffer, JavaClass *class_info)
{
    unsigned int count;
    unsigned short index;
    ConstantPoolEntry *constant_pool;
    int is_non_member_tag;
    unsigned char *tags;
    int tag;

    count = DataBuffer_getUShortAt(buffer);
    if (count == 0) {
        class_info->constants = 0;
        class_info->constantCount = 0;
        return;
    }

    constant_pool = xmalloc(count * sizeof(*constant_pool) + count, VM_HEAP_OBJECT);
    class_info->constantCount = count;
    class_info->constants = constant_pool;
    /* The tag bytes follow the four-byte entries in the same allocation.
     * Entry zero holds the tag-vector pointer for the later loader stages. */
    tags = (unsigned char *)&constant_pool[count];
    constant_pool[0].tags = tags;
    tags[0] = 0;

    /* Read each entry: its tag byte, then the payload that tag defines. */
    for (index = 1; index < count;) {
        unsigned char *tag_slot;
        unsigned int current_tag = DataBuffer_getUByteAt(buffer);

        tag_slot = &tags[index];
        *tag_slot = current_tag;
        switch (current_tag) {
        case JVM_CONSTANT_UTF8: {
            unsigned int length = DataBuffer_getUShortAt(buffer);
            const char *bytes = (const char *)buffer->position;

            constant_pool[index].object = loadConstString(bytes, length - 1);
            DataBuffer_seek(buffer, length);
            break;
        }
        case JVM_CONSTANT_CLASS:
        case JVM_CONSTANT_STRING:
            constant_pool[index].halfValue = DataBuffer_getUShortAt(buffer);
            break;
        case JVM_CONSTANT_FIELDREF:
        case JVM_CONSTANT_METHODREF:
        case JVM_CONSTANT_INTERFACE_METHODREF:
        case JVM_CONSTANT_NAME_AND_TYPE: {
            unsigned int first_index = DataBuffer_getUShortAt(buffer);
            unsigned int second_index = DataBuffer_getUShortAt(buffer);

            constant_pool[index].intValue =(second_index << 16) | first_index;
            break;
        }
        case JVM_CONSTANT_INTEGER:
        case JVM_CONSTANT_FLOAT:
            constant_pool[index].intValue =DataBuffer_getUIntAt(buffer);
            break;
        case JVM_CONSTANT_LONG: {
            /* Long entries reserve the following pool slot. */
            unsigned int high_word = DataBuffer_getUIntAt(buffer);
            unsigned int low_word = DataBuffer_getUIntAt(buffer);

            constant_pool[index].intValue =high_word;
            index++;
            tags[index] = 0;
            constant_pool[index].intValue =low_word;
            break;
        }
        case JVM_CONSTANT_DOUBLE: {
            /* Double entries use the same two-slot representation. */
            unsigned int high_word = DataBuffer_getUIntAt(buffer);
            unsigned int low_word = DataBuffer_getUIntAt(buffer);

            constant_pool[index].intValue =high_word;
            index++;
            tags[index] = 0;
            constant_pool[index].intValue =low_word;
            break;
        }
        default:
            break;
        }
        index++;
    }

    /* Give class and string entries the object of their UTF8 name; entries without one lose their tag. */
    for (index = 1; index < count; index++) {
        tag = tags[index];
        is_non_member_tag = tag < JVM_CONSTANT_FIELDREF;
        if (is_non_member_tag && tag >= JVM_CONSTANT_CLASS) {
            unsigned short reference_index = constant_pool[index].halfValue;

            if (tags[reference_index] == JVM_CONSTANT_UTF8) {
                constant_pool[index].object = constant_pool[reference_index].object;
            } else {
                tags[index] = 0;
            }
        }
    }
}

void setFieldValue(SceneField *field, unsigned int value)
{
    field->location.offset = value;
    field->flags |= VM_FIELD_FLAG_HAS_CONSTANT_VALUE;
}

/* Fills in the name, flags, loader and super class index when this_class names a class constant. */
void setupClass(JavaClass *class_info, u32 this_class_index,
                u32 super_class_index, u32 access_flags,
                void *class_loader)
{
    ConstantPoolEntry *constant_pool = class_info->constants;
    access_flags = (u16)access_flags;

    if (constant_pool != 0 &&
        constant_pool[0].tags[this_class_index] == JVM_CONSTANT_CLASS) {
        SceneString *class_name = constant_pool[this_class_index].object;

        class_info->superClassIndex = super_class_index;
        class_info->name = class_name;
        class_info->accessFlags = (u16)access_flags;
        class_info->classLoader = class_loader;
        class_info->state = 0;
        class_info->instanceSize = 0;
    }
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char classFileExtension[] = ".class";

ConstString **constStringTable = 0;
