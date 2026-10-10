#include "common.h"

#include "shared.h"

#include "init_class_db.h"

int classDB[8];

extern void *xmalloc(int size, int type);

typedef struct ClassFileMemberHeader {
    u16 access_flags;
    u16 name_index;
    u16 descriptor_index;
} ClassFileMemberHeader;

ClassFieldRecord *addField(ClassDescriptor *class_info, const ClassFileMemberHeader *field_info);

SceneMethod *addMethod(ClassDescriptor *class_info, ClassFileMemberHeader *method_info);

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

const char D_004DBFD0[];

ConstString **constStringTable;

extern char *strncpy(char *dest, const char *src, unsigned int count);

extern char *strcat(char *dest, const char *src);

extern ClassFile *PDB_findFile(int volume, const char *name);

extern void DataBuffer_init(DataBuffer *buffer, unsigned char *bytes,
                            int length, int big_endian);

#include "main/init_vm.h"

#include "main/find_native_method.h"

#include "main/scene_1.h"

/* classFromSig resolves both reference classes and primitive type records. */

extern void *getClassFromSignature(const char *signature, void *class_loader);

extern void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize,
                            signed char *returnType);

extern SceneString *NAME_Constructor;

extern SceneString *ATTR_Code;

extern SceneString *ATTR_Exceptions;

extern SceneString *ATTR_LineNumberTable;

extern SceneString *ATTR_ConstantValue;

void setFieldValue(SceneField *field, unsigned int value);

void initClassDB(void)
{
    int i;

    for (i = 7; i >= 0; i--) {
        classDB[i] = 0;
    }
}

ClassDescriptor *findClass(ClassFileEntry *entry)
{
    char filename[256];
    DataBuffer buffer;
    int volume;
    int index;

    strncpy(filename, entry->path->name, entry->path->length);
    filename[entry->path->length] = '\0';
    strcat(filename, D_004DBFD0);

    for (index = 0; index < 8; index++) {
        volume = classDB[index];
        if (volume != 0) {
            ClassFile *file = PDB_findFile(volume, filename);
            if (file != 0) {
                ClassDescriptor *class_info = newClass();
                DataBuffer_init(&buffer, file->bytes, file->length, 1);
                readClass(&buffer, class_info, 0);
                return class_info;
            }
        }
    }
    return 0;
}

ClassDescriptor *readClass(DataBuffer *buffer, ClassDescriptor *class_info,
                           u32 class_loader)
{
    u32 access_flags;
    u32 this_class_index;
    u32 super_class_index;

    if (DataBuffer_getUIntAt(buffer) != 0xCAFEBABE) {
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

void addCode(DataBuffer *buffer, ClassDescriptor *class_info, SceneMethod *method,
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
    method->max_stack = max_stack;
    method->max_locals = max_locals;
    method->code_length = (u16) code_length;
    DataBuffer_seek(buffer, exception_table_count * 8);
    readAttributes(buffer, class_info, method);
}

ClassFieldRecord *addField(ClassDescriptor *class_info,
                           const ClassFileMemberHeader *field_info)
{
    ClassFieldRecord *field;
    ClassFieldRecord *fields;
    ClassConstantPoolValue descriptor_value;
    ClassConstantPoolValue *constant_pool;
    SceneTypeDescriptor *descriptor;
    ClassSignatureResult *resolved_type;
    const char *signature;
    u16 field_index;

    constant_pool = class_info->constant_pool;
    if ((field_info->access_flags & 8) != 0) {
        field_index = class_info->static_field_count++;
    } else {
        class_info->field_count--;
        field_index = class_info->field_count;
    }

    fields = class_info->fields;
    field = &fields[field_index];
    field->name = constant_pool[field_info->name_index].object;
    field->flags = field_info->access_flags;
    descriptor_value = constant_pool[field_info->descriptor_index];
    descriptor = descriptor_value.object;
    signature = descriptor->signature;
    if ((signature[0] == 'L') || (signature[0] == '[')) {
        field->type_or_descriptor = descriptor;
        field->field_size = 4;
        field->flags |= 0x8000;
    } else {
        resolved_type = getClassFromSignature(signature, 0);
        field->type_or_descriptor = resolved_type;
        field->field_size = resolved_type->field_size;
    }
    return field;
}

SceneMethod *addMethod(ClassDescriptor *class_info,
                       ClassFileMemberHeader *method_info)
{
    ClassConstantPoolValue *constant_pool;
    SceneMethod *method;
    SceneMethod *methods;
    u16 method_index;
    u16 access_flags;
    u16 name_index;
    u16 descriptor_index;
    void *descriptor_object;
    signed char return_type[16];

    method_index = class_info->method_count;
    methods = class_info->methods;
    method = &methods[method_index];
    class_info->method_count = method_index + 1;
    constant_pool = class_info->constant_pool;
    access_flags = method_info->access_flags;
    method->access_flags = access_flags;
    name_index = method_info->name_index;
    descriptor_index = method_info->descriptor_index;
    method->name = constant_pool[name_index].object;
    descriptor_object = constant_pool[descriptor_index].object;
    method->declaring_class = class_info;
    method->descriptor = descriptor_object;
    method->state = 0xffff;
    method->max_stack = 0;
    method->max_locals = 0;
    if (method->name == NAME_Constructor) {
        method->access_flags |= 0x800;
    }
    methodDescripter((u8 *)method->descriptor->signature,
                     &method->parameter_size, &method->return_size,
                     return_type);
    return method;
}

static void readFields(DataBuffer *buffer, ClassDescriptor *class_info)
{
    u16 field_count;
    u16 field_index;
    ClassFileMemberHeader field_info;

    field_count = DataBuffer_getUShortAt(buffer);
    class_info->static_field_count = 0;
    class_info->field_count = field_count;
    if (field_count != 0) {
        class_info->fields = xmalloc(field_count * 20, 10);
        field_index = 0;
    } else {
        class_info->fields = 0;
        return;
    }
    while (field_index < field_count) {
        ClassFieldRecord *field;

        field_info.access_flags = DataBuffer_getUShortAt(buffer);
        field_info.name_index = DataBuffer_getUShortAt(buffer);
        field_info.descriptor_index = DataBuffer_getUShortAt(buffer);
        field = addField(class_info, &field_info);
        readAttributes(buffer, class_info, field);
        field_index++;
    }
    class_info->field_count = field_count;
}

static void readMethods(DataBuffer *buffer, ClassDescriptor *class_info)
{
    u16 method_count;
    u16 method_index;
    ClassFileMemberHeader method_info;

    method_count = DataBuffer_getUShortAt(buffer);
    class_info->method_count = 0;
    if (method_count != 0) {
        class_info->methods = xmalloc(method_count * 32, 11);
        method_index = 0;
    } else {
        class_info->methods = 0;
        return;
    }
    while (method_index < method_count) {
        SceneMethod *method;

        method_info.access_flags = DataBuffer_getUShortAt(buffer);
        method_info.name_index = DataBuffer_getUShortAt(buffer);
        method_info.descriptor_index = DataBuffer_getUShortAt(buffer);
        method = addMethod(class_info, &method_info);
        readAttributes(buffer, class_info, method);
        method_index++;
    }
}

static void readInterfaces(DataBuffer *buffer, ClassDescriptor *class_info)
{
    u16 interface_count;
    u16 interface_index;

    interface_count = DataBuffer_getUShortAt(buffer);
    class_info->interface_count = interface_count;
    if (interface_count != 0) {
        class_info->interfaces = xmalloc(interface_count * 4, 12);
        interface_index = 0;
    } else {
        class_info->interfaces = 0;
        return;
    }
    while (interface_index < interface_count) {
        ((u32 *)class_info->interfaces)[interface_index] =
            DataBuffer_getUShortAt(buffer);
        interface_index++;
    }
}

static void readAttributes(DataBuffer *buffer, ClassDescriptor *class_info,
                           void *attribute_target)
{
    ClassConstantPoolValue tag_vector;
    ClassConstantPoolValue constant_pool_value;
    ClassConstantPoolValue *constant_pool;
    unsigned short attribute_count;
    unsigned short attribute_index;
    unsigned short name_index;
    unsigned int attribute_length;
    void *attribute_name;

    attribute_count = DataBuffer_getUShortAt(buffer);
    if (attribute_count == 0) {
        return;
    }

    constant_pool = class_info->constant_pool;
    tag_vector = constant_pool[0];
    for (attribute_index = 0; attribute_index < attribute_count;
         attribute_index++) {
        name_index = DataBuffer_getUShortAt(buffer);
        attribute_length = DataBuffer_getUIntAt(buffer);
        switch (tag_vector.tags[name_index]) {
        case 1:
            constant_pool_value = constant_pool[name_index];
            attribute_name = constant_pool_value.object;
            if (attribute_name == ATTR_Code) {
                addCode(buffer, class_info, (SceneMethod *)attribute_target,
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

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readConstantPool);

void setFieldValue(SceneField *field, unsigned int value)
{
    field->instance_offset = value;
    field->flags |= 0x4000;
}

void setupClass(ClassDescriptor *class_info, u32 this_class_index,
                u32 super_class_index, u32 access_flags,
                u32 class_loader)
{
    ConstantPoolWord *constant_pool = class_info->constant_pool;
    access_flags = (u16)access_flags;

    if (constant_pool != 0 &&
        ((u8 *)constant_pool[0])[this_class_index] == 7) {
        void *class_name = (void *)constant_pool[this_class_index];

        class_info->super_class_index = super_class_index;
        class_info->class_name = class_name;
        class_info->access_flags = (u16)access_flags;
        class_info->class_loader = class_loader;
        class_info->instance_size = 0;
        class_info->processing_state = 0;
    }
}

const char D_004DBFD0[] = ".class";
ConstString **constStringTable = 0;


