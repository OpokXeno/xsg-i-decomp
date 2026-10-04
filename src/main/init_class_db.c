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

SceneField *addField(ClassDescriptor *class_info, ClassFileMemberHeader *field_info);
SceneMethod *addMethod(ClassDescriptor *class_info, u16 method_info[3]);

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

extern const char D_004DBFD0[];
extern ConstString **constStringTable;
extern char *strncpy(char *dest, const char *src, unsigned int count);
extern char *strcat(char *dest, const char *src);
extern ClassFile *PDB_findFile(int volume, const char *name);
extern ClassDescriptor *newClass(void);
extern void DataBuffer_init(DataBuffer *buffer, unsigned char *bytes,
                            int length, int big_endian);

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

/*
 * Reads a Java class file header from `buffer` into `class_info`: the
 * magic 0xCAFEBABE, the unused minor/major version words, the constant
 * pool, access_flags/this_class/super_class, then the interfaces, fields,
 * methods and attributes that follow (JVM class file format).
 */
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

void addCode(DataBuffer *buffer, ClassDescriptor *class_info, SceneMethod *method)
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
    readAttributes(buffer, class_info, (int) method);
}

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", addField);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", addMethod);

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
        SceneField *field;

        field_info.access_flags = DataBuffer_getUShortAt(buffer);
        field_info.name_index = DataBuffer_getUShortAt(buffer);
        field_info.descriptor_index = DataBuffer_getUShortAt(buffer);
        field = addField(class_info, &field_info);
        readAttributes(buffer, class_info, (int)field);
        field_index++;
    }
    class_info->field_count = field_count;
}

static void readMethods(DataBuffer *buffer, ClassDescriptor *class_info)
{
    u16 method_count;
    u16 method_index;
    u16 method_info[3];

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

        method_info[0] = DataBuffer_getUShortAt(buffer);
        method_info[1] = DataBuffer_getUShortAt(buffer);
        method_info[2] = DataBuffer_getUShortAt(buffer);
        method = addMethod(class_info, method_info);
        readAttributes(buffer, class_info, (int)method);
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

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readAttributes);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readConstantPool);

/*
 * Stores a field's resolved constant/instance value and marks it resolved
 * (SceneField.flags bit 0x4000), the way readAttributes does after loading a
 * field's ConstantValue attribute.
 */
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
