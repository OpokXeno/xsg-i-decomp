#include "common.h"
#include "shared.h"
#include "init_class_db.h"

void initClassDB(void)
{
    int i;

    for (i = 7; i >= 0; i--) {
        classDB[i] = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", findClass);

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

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readFields);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readMethods);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readInterfaces);

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
