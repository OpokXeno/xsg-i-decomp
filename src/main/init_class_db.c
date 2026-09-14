#include "common.h"
#include "shared.h"
#include "init_class_db.h"

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", initClassDB);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", findClass);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readClass);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", addCode);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", addField);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", addMethod);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readFields);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readMethods);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readInterfaces);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readAttributes);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", readConstantPool);

INCLUDE_ASM("asm/main/nonmatchings/init_class_db", setFieldValue);

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
