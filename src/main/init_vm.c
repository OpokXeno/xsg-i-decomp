#include "common.h"
#include "init_vm.h"

/*
 * initBaseClasses and initPrimitiveTypes are file-local (LOCAL in the
 * original symbol table); initPrimitiveTypes stays asm, so both are
 * declared static before their use below.
 */
static void initBaseClasses(void);
static void initPrimitiveTypes(void);

void initVM(void) {
    int i;

    xheap_init(1, 0, 0);
    classEntryPool = 0;
    constStringTable = 0;
    NAME_Init = loadConstString(D_004CD0C0, -1);
    NAME_Constructor = loadConstString(D_004DBFE0, -1);
    TYPE_Chr_talk = loadConstString(D_004DBFE8, -1);
    TYPE_Stage_entered = loadConstString(D_004DBFF0, -1);
    TYPE_Void = loadConstString(D_004DBFE8, -1);
    ATTR_SourceFile = loadConstString(D_004CD0D0, -1);
    ATTR_InnerClasses = loadConstString(D_004CD0E0, -1);
    ATTR_Code = loadConstString(D_004DBFF8, -1);
    ATTR_Exceptions = loadConstString(D_004CD0F0, -1);
    ATTR_LineNumberTable = loadConstString(D_004CD100, -1);
    ATTR_ConstantValue = loadConstString(D_004CD110, -1);
    ATTR_LocalVariableTable = loadConstString(D_004CD120, -1);
    jthreadResetFunc = 0;
    jthreadTop = 0;
    initVMThread = JNI_createThread(0, 8, 0x40);
    initBaseClasses();
    for (i = 3; i >= 0; i--) {
        XTK_peerGroup[i] = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_vm", virtualMachine);

static void initBaseClasses(void) {
    initPrimitiveTypes();
    loadStaticClass(&classObject, D_004CD628);
    loadStaticClass(&classStringBuffer, D_004CD640);
    loadStaticClass(&classString, D_004CD658);
}

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
