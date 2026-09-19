#include "common.h"
#include "init_vm.h"

INCLUDE_ASM("asm/main/nonmatchings/init_vm", initVM);

INCLUDE_ASM("asm/main/nonmatchings/init_vm", virtualMachine);

INCLUDE_ASM("asm/main/nonmatchings/init_vm", initBaseClasses);

INCLUDE_ASM("asm/main/nonmatchings/init_vm", initPrimitiveTypes);

static void initWrapperClass(VMClass **class_slot, const char *name,
                              signed char type_code, int element_size)
{
    VMClass *new_class = newClass();
    SceneString *interned_name;
    VMClass *stored_class;

    *class_slot = new_class;
    new_class->type_flags = 0x100;

    interned_name = loadConstString(name, -1);
    stored_class = *class_slot;
    stored_class->type_code = type_code;
    stored_class->name = interned_name;
    (*class_slot)->element_size = element_size;
}
