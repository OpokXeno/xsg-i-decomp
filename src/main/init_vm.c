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

/* JVM opcodes the interpreter implements (JVM specification numbering). */
enum {
    OP_NOP = 0x00,
    OP_ACONST_NULL = 0x01,
    OP_ICONST_M1 = 0x02,
    OP_ICONST_0 = 0x03,
    OP_ICONST_1 = 0x04,
    OP_ICONST_2 = 0x05,
    OP_ICONST_3 = 0x06,
    OP_ICONST_4 = 0x07,
    OP_ICONST_5 = 0x08,
    OP_FCONST_0 = 0x0B,
    OP_FCONST_1 = 0x0C,
    OP_FCONST_2 = 0x0D,
    OP_BIPUSH = 0x10,
    OP_SIPUSH = 0x11,
    OP_LDC = 0x12,
    OP_LDC_W = 0x13,
    OP_ILOAD = 0x15,
    OP_FLOAD = 0x17,
    OP_ALOAD = 0x19,
    OP_ILOAD_0 = 0x1A,
    OP_ILOAD_1 = 0x1B,
    OP_ILOAD_2 = 0x1C,
    OP_ILOAD_3 = 0x1D,
    OP_FLOAD_0 = 0x22,
    OP_FLOAD_1 = 0x23,
    OP_FLOAD_2 = 0x24,
    OP_FLOAD_3 = 0x25,
    OP_ALOAD_0 = 0x2A,
    OP_ALOAD_1 = 0x2B,
    OP_ALOAD_2 = 0x2C,
    OP_ALOAD_3 = 0x2D,
    OP_IALOAD = 0x2E,
    OP_FALOAD = 0x30,
    OP_AALOAD = 0x32,
    OP_BALOAD = 0x33,
    OP_CALOAD = 0x34,
    OP_SALOAD = 0x35,
    OP_ISTORE = 0x36,
    OP_FSTORE = 0x38,
    OP_ASTORE = 0x3A,
    OP_ISTORE_0 = 0x3B,
    OP_ISTORE_1 = 0x3C,
    OP_ISTORE_2 = 0x3D,
    OP_ISTORE_3 = 0x3E,
    OP_FSTORE_0 = 0x43,
    OP_FSTORE_1 = 0x44,
    OP_FSTORE_2 = 0x45,
    OP_FSTORE_3 = 0x46,
    OP_ASTORE_0 = 0x4B,
    OP_ASTORE_1 = 0x4C,
    OP_ASTORE_2 = 0x4D,
    OP_ASTORE_3 = 0x4E,
    OP_IASTORE = 0x4F,
    OP_FASTORE = 0x51,
    OP_AASTORE = 0x53,
    OP_BASTORE = 0x54,
    OP_CASTORE = 0x55,
    OP_SASTORE = 0x56,
    OP_POP = 0x57,
    OP_POP2 = 0x58,
    OP_DUP = 0x59,
    OP_DUP_X1 = 0x5A,
    OP_DUP_X2 = 0x5B,
    OP_DUP2 = 0x5C,
    OP_DUP2_X1 = 0x5D,
    OP_DUP2_X2 = 0x5E,
    OP_SWAP = 0x5F,
    OP_IADD = 0x60,
    OP_FADD = 0x62,
    OP_ISUB = 0x64,
    OP_FSUB = 0x66,
    OP_IMUL = 0x68,
    OP_FMUL = 0x6A,
    OP_IDIV = 0x6C,
    OP_FDIV = 0x6E,
    OP_IREM = 0x70,
    OP_FREM = 0x72,
    OP_INEG = 0x74,
    OP_FNEG = 0x76,
    OP_ISHL = 0x78,
    OP_ISHR = 0x7A,
    OP_IUSHR = 0x7C,
    OP_IAND = 0x7E,
    OP_IOR = 0x80,
    OP_IXOR = 0x82,
    OP_IINC = 0x84,
    OP_I2F = 0x86,
    OP_F2I = 0x8B,
    OP_I2C = 0x92,
    OP_I2S = 0x93,
    OP_FCMPL = 0x95,
    OP_FCMPG = 0x96,
    OP_IFEQ = 0x99,
    OP_IFNE = 0x9A,
    OP_IFLT = 0x9B,
    OP_IFGE = 0x9C,
    OP_IFGT = 0x9D,
    OP_IFLE = 0x9E,
    OP_IF_ICMPEQ = 0x9F,
    OP_IF_ICMPNE = 0xA0,
    OP_IF_ICMPLT = 0xA1,
    OP_IF_ICMPGE = 0xA2,
    OP_IF_ICMPGT = 0xA3,
    OP_IF_ICMPLE = 0xA4,
    OP_IF_ACMPEQ = 0xA5,
    OP_IF_ACMPNE = 0xA6,
    OP_GOTO = 0xA7,
    OP_JSR = 0xA8,
    OP_RET = 0xA9,
    OP_TABLESWITCH = 0xAA,
    OP_LOOKUPSWITCH = 0xAB,
    OP_IRETURN = 0xAC,
    OP_FRETURN = 0xAE,
    OP_ARETURN = 0xB0,
    OP_RETURN = 0xB1,
    OP_GETSTATIC = 0xB2,
    OP_PUTSTATIC = 0xB3,
    OP_GETFIELD = 0xB4,
    OP_PUTFIELD = 0xB5,
    OP_INVOKEVIRTUAL = 0xB6,
    OP_INVOKESPECIAL = 0xB7,
    OP_INVOKESTATIC = 0xB8,
    OP_INVOKEINTERFACE = 0xB9,
    OP_NEW = 0xBB,
    OP_NEWARRAY = 0xBC,
    OP_ANEWARRAY = 0xBD,
    OP_ARRAYLENGTH = 0xBE,
    OP_CHECKCAST = 0xC0,
    OP_INSTANCEOF = 0xC1,
    OP_WIDE = 0xC4,
    OP_IFNULL = 0xC6,
    OP_IFNONNULL = 0xC7,
    OP_GOTO_W = 0xC8,
    OP_JSR_W = 0xC9
};

/* Constant pool tags ldc/ldc_w read (constants[0] points at the tag table). */
enum {
    VM_CONSTANT_INTEGER = 3,
    VM_CONSTANT_FLOAT = 4,
    VM_CONSTANT_STRING = 24
};

#define VM_ACC_STATIC 0x8
#define VM_ACC_NATIVE 0x100

typedef union VMSlot {
    int i;
    float f;
    void *ref;
    unsigned char b;
    short s;
    unsigned short c;
} VMSlot;

typedef struct VMSlots {
    short top;
    unsigned short size;
    VMSlot *base;
} VMSlots;

typedef struct VMMethodP VMMethod;

typedef struct VMFrame {
    unsigned short pc;
    unsigned short nextPc;
    VMMethod *callee;
    VMSlots locals;
    VMSlots stack;
} VMFrame;

typedef struct VMThreadP {
    unsigned char unmodeled_00[0x24];
    unsigned int flags;
    VMSlot *stack;
    VMFrame *frames;
    unsigned char unmodeled_30[8];
    unsigned short stack_offset;
    short stack_limit;
    short resume_frames;
    short frame_depth;
} VMThread;

typedef void (*VMNativeFunc)(VMThread *thread, VMSlot *arguments, VMSlot *result);

typedef struct VMClassP {
    unsigned char unmodeled_00[0x14];
    VMSlot *constants;
} VMOwner;

struct VMMethodP {
    unsigned char unmodeled_00[8];
    short arg_count;
    unsigned short result_size;
    VMOwner *owner;
    unsigned short access;
    unsigned char unmodeled_12[2];
    union {
        unsigned char *code;       /* +0x14: bytecode */
        VMNativeFunc native;       /* +0x14: native implementation */
    };
    unsigned short max_stack;
    unsigned short max_locals;
    unsigned short code_length;
};

typedef struct VMArray {
    unsigned char unmodeled_00[4];
    int length;
    void *data;
} VMArray;

typedef struct VMField {
    unsigned char unmodeled_00[4];
    unsigned short flags;
    unsigned char unmodeled_06[2];
    VMClass *type;
    unsigned char unmodeled_0c[4];
    union {
        int *intAddress;           /* static field storage, by field type */
        float *floatAddress;
        unsigned char *byteAddress;
        short *shortAddress;
        unsigned int offset;       /* instance field byte offset */
    };
} VMField;

typedef struct VMFieldRef {
    unsigned char unmodeled_00[4];
    VMField *field;
    unsigned char unmodeled_08[8];
} VMFieldRef;


extern void resolveNativeMethod(VMMethod *method);
extern void JNI_callMethod(VMThread *thread, VMMethod *method, VMSlot *arguments, VMSlot *result);
extern void JNI_threadException();
extern int getField(int index, VMOwner *owner, int isStatic, VMFieldRef *ref);
extern VMMethod *getMethodSignatureClass(int index, VMOwner *owner, int mode);
extern VMClass *getClass(int index, VMOwner *owner);
extern void *newObject(VMClass *vmClass);
extern void *newArray(VMClass *elementClass, int length);
extern int JNI_isInstanceOf(void *object, VMClass *vmClass);
extern VMClass *primitiveClassTable[16];

/* The operand stack slot `n` places above (n > 0) or below (n < 0) the top. */
#define STACK(n) (stack->base[stack->top + (n)])

/* The address of the operand stack's top slot, for relative access. */
static inline VMSlot *_StackTop(VMSlots *stack)
{
    return &stack->base[stack->top];
}

/* The big-endian 16-bit operand at code[pc + n]. */
#define CODE_U16(n) ((code[frame->pc + (n)] << 8) | code[frame->pc + (n) + 1])

/* The big-endian 32-bit operand at p[0..3] (tableswitch/lookupswitch). */
#define CODE_S32(p) (((p)[0] << 24) | ((p)[1] << 16) | ((p)[2] << 8) | (p)[3])

/* Branch to the 16-bit offset that follows the opcode. */
#define BRANCH() (frame->nextPc = frame->pc + CODE_U16(0) - 1)

void virtualMachine(SceneVm *vm, VMMethod *method, VMSlot *arguments, VMSlot *result)
{
    VMThread *thread = (VMThread *)vm;
    unsigned short access = method->access;
    VMFrame *frame;
    VMSlots *locals;
    VMSlots *stack;
    unsigned char *code;
    unsigned int codeLength;
    unsigned int flags;
    VMMethod *callee;
    VMSlot *args;

    if (access & VM_ACC_NATIVE) {
        if (!(thread->flags & 2)) {
            if (method->native == 0) {
                resolveNativeMethod(method);
            }
            method->native(thread, arguments, result);
            return;
        }
        thread->flags &= ~2;
        return;
    }

    if (!(thread->flags & 2)) {
        int count;
        int i;

        frame = &thread->frames[thread->frame_depth];
        code = method->code;
        codeLength = method->code_length;
        locals = &frame->locals;
        stack = &frame->stack;
        args = &thread->stack[(short)thread->stack_offset];
        thread->stack_offset += method->max_locals + method->max_stack + 1;
        if ((short)thread->stack_offset >= thread->stack_limit) {
            JNI_threadException(thread, method);
        }
        locals->top = 0;
        locals->base = args;
        locals->size = method->max_locals;
        stack->base = args + method->max_locals;
        stack->top = 0;
        stack->size = method->max_stack;
        frame->pc = 0;
        if (arguments != 0) {
            count = method->arg_count;
            if (!(access & VM_ACC_STATIC)) {
                count++;
            }
            for (i = 0; i < count; i++) {
                locals->base[i] = arguments[i];
            }
        }
    } else {
        /* Resume the invoke a native call suspended. */
        frame = &thread->frames[thread->frame_depth - thread->resume_frames];
        thread->resume_frames--;
        code = method->code;
        codeLength = method->code_length;
        locals = &frame->locals;
        stack = &frame->stack;
        switch (code[frame->pc - 1]) {
        case OP_INVOKESTATIC:
            callee = frame->callee;
            JNI_callMethod(thread, callee, &STACK(1 - callee->arg_count), &STACK(1 - callee->arg_count));
            if (!(thread->flags & 5)) {
                thread->frame_depth--;
                stack->top = stack->top - callee->arg_count;
                stack->top = stack->top + callee->result_size;
                frame->nextPc = frame->pc + 2;
            }
            break;
        case OP_INVOKEVIRTUAL:
            callee = frame->callee;
            JNI_callMethod(thread, callee, &STACK(-callee->arg_count), &STACK(-callee->arg_count));
            if (!(thread->flags & 5)) {
                thread->frame_depth--;
                stack->top = stack->top - callee->arg_count - 1;
                stack->top = stack->top + callee->result_size;
                frame->nextPc = frame->pc + 2;
            }
            break;
        case OP_INVOKESPECIAL:
            callee = frame->callee;
            JNI_callMethod(thread, callee, &STACK(-callee->arg_count), &STACK(-callee->arg_count));
            if (!(thread->flags & 5)) {
                thread->frame_depth--;
                stack->top = stack->top - callee->arg_count - 1;
                frame->nextPc = frame->pc + 2;
            }
            break;
        case OP_INVOKEINTERFACE:
            callee = frame->callee;
            JNI_callMethod(thread, callee, &STACK(-callee->arg_count), &STACK(-callee->arg_count));
            if (!(thread->flags & 5)) {
                thread->frame_depth--;
                stack->top = stack->top - callee->arg_count - 1;
                stack->top = stack->top + callee->result_size;
                frame->nextPc = frame->pc + 4;
            }
            break;
        }
        if (thread->flags & 7) {
            return;
        }
        frame->pc = frame->nextPc;
    }

    while (frame->pc < codeLength) {
        unsigned short pc;

        frame->nextPc = frame->pc + 1;
        pc = frame->pc++;
        switch (code[pc]) {
        case OP_NOP:
            break;
        case OP_ACONST_NULL:
            stack->top++;
            STACK(0).i = 0;
            break;
        case OP_ICONST_M1:
        case OP_ICONST_0:
        case OP_ICONST_1:
        case OP_ICONST_2:
        case OP_ICONST_3:
        case OP_ICONST_4:
        case OP_ICONST_5:
            stack->top++;
            STACK(0).i = code[frame->pc - 1] - OP_ICONST_0;
            break;
        case OP_FCONST_0:
        case OP_FCONST_1:
        case OP_FCONST_2:
            stack->top++;
            STACK(0).f = code[frame->pc - 1] - OP_FCONST_0;
            break;
        case OP_BIPUSH:
            stack->top++;
            STACK(0).i = (signed char)code[frame->pc];
            frame->nextPc = frame->pc + 1;
            break;
        case OP_SIPUSH:
            stack->top++;
            STACK(0).i = CODE_U16(0);
            frame->nextPc = frame->pc + 2;
            break;
        case OP_LDC: {
            unsigned char index = code[frame->pc];
            VMSlot *constants = method->owner->constants;

            stack->top++;
            switch (((unsigned char *)constants[0].ref)[index]) {
            case VM_CONSTANT_STRING:
                STACK(0).i = constants[index].i;
                break;
            case VM_CONSTANT_FLOAT:
                STACK(0).f = constants[index].f;
                break;
            case VM_CONSTANT_INTEGER:
                STACK(0).i = constants[index].i;
                break;
            }
            frame->nextPc = frame->pc + 1;
            break;
        }
        case OP_LDC_W: {
            unsigned int index = CODE_U16(0);
            VMSlot *constants = method->owner->constants;

            stack->top++;
            switch (((unsigned char *)constants[0].ref)[index]) {
            case VM_CONSTANT_STRING:
                STACK(0).i = constants[index].i;
                break;
            case VM_CONSTANT_INTEGER:
                STACK(0).f = constants[index].f;
                break;
            case VM_CONSTANT_FLOAT:
                STACK(0).f = constants[index].f;
                break;
            }
            frame->nextPc = frame->pc + 2;
            break;
        }
        case OP_ILOAD:
        case OP_FLOAD:
        case OP_ALOAD: {
            unsigned char index = code[frame->pc];

            stack->top++;
            STACK(0) = locals->base[index];
            frame->nextPc = frame->pc + 1;
            break;
        }
        case OP_ILOAD_0:
        case OP_ILOAD_1:
        case OP_ILOAD_2:
        case OP_ILOAD_3:
            stack->top++;
            STACK(0) = locals->base[code[frame->pc - 1] - OP_ILOAD_0];
            break;
        case OP_FLOAD_0:
        case OP_FLOAD_1:
        case OP_FLOAD_2:
        case OP_FLOAD_3:
            stack->top++;
            STACK(0) = locals->base[code[frame->pc - 1] - OP_FLOAD_0];
            break;
        case OP_ALOAD_0:
        case OP_ALOAD_1:
        case OP_ALOAD_2:
        case OP_ALOAD_3:
            stack->top++;
            STACK(0) = locals->base[code[frame->pc - 1] - OP_ALOAD_0];
            break;
        case OP_IALOAD:
        case OP_AALOAD: {
            VMSlot *sp = &stack->base[stack->top];
            VMArray *array = sp[-1].ref;
            int index = sp[0].i;

            stack->top--;
            STACK(0).i = ((int *)array->data)[index];
            break;
        }
        case OP_FALOAD: {
            VMSlot *sp = &stack->base[stack->top];
            VMArray *array = sp[-1].ref;
            int index = sp[0].i;

            stack->top--;
            STACK(0).f = ((float *)array->data)[index];
            break;
        }
        case OP_BALOAD: {
            VMSlot *sp = &stack->base[stack->top];
            VMArray *array = sp[-1].ref;
            int index = sp[0].i;

            stack->top--;
            STACK(0).i = ((signed char *)array->data)[index];
            break;
        }
        case OP_CALOAD: {
            VMArray *array = STACK(-1).ref;
            int index = STACK(0).i;

            stack->top--;
            STACK(0).i = ((unsigned short *)array->data)[index];
            break;
        }
        case OP_SALOAD: {
            VMSlot *sp = &stack->base[stack->top];
            VMArray *array = sp[-1].ref;
            int index = sp[0].i;

            stack->top--;
            STACK(0).i = ((short *)array->data)[index];
            break;
        }
        case OP_ISTORE:
        case OP_FSTORE:
        case OP_ASTORE:
            locals->base[code[frame->pc]] = STACK(0);
            stack->top--;
            frame->nextPc = frame->pc + 1;
            break;
        case OP_ISTORE_0:
        case OP_ISTORE_1:
        case OP_ISTORE_2:
        case OP_ISTORE_3:
            locals->base[code[frame->pc - 1] - OP_ISTORE_0] = STACK(0);
            stack->top--;
            break;
        case OP_FSTORE_0:
        case OP_FSTORE_1:
        case OP_FSTORE_2:
        case OP_FSTORE_3:
            locals->base[code[frame->pc - 1] - OP_FSTORE_0] = STACK(0);
            stack->top--;
            break;
        case OP_ASTORE_0:
        case OP_ASTORE_1:
        case OP_ASTORE_2:
        case OP_ASTORE_3:
            locals->base[code[frame->pc - 1] - OP_ASTORE_0] = STACK(0);
            stack->top--;
            break;
        case OP_IASTORE: {
            VMSlot *sp = &stack->base[stack->top];

            ((int *)((VMArray *)sp[-2].ref)->data)[sp[-1].i] = sp[0].i;
            stack->top -= 3;
            break;
        }
        case OP_FASTORE: {
            VMSlot *sp = &stack->base[stack->top];

            ((float *)((VMArray *)sp[-2].ref)->data)[sp[-1].i] = sp[0].f;
            stack->top -= 3;
            break;
        }
        case OP_AASTORE: {
            VMSlot *sp = &stack->base[stack->top];

            ((void **)((VMArray *)sp[-2].ref)->data)[sp[-1].i] = sp[0].ref;
            stack->top -= 3;
            break;
        }
        case OP_BASTORE: {
            VMSlot *sp = &stack->base[stack->top];

            ((unsigned char *)((VMArray *)sp[-2].ref)->data)[sp[-1].i] = sp[0].b;
            stack->top -= 3;
            break;
        }
        case OP_CASTORE:
        case OP_SASTORE: {
            VMSlot *sp = &stack->base[stack->top];

            ((unsigned short *)((VMArray *)sp[-2].ref)->data)[sp[-1].i] = sp[0].s;
            stack->top -= 3;
            break;
        }
        case OP_POP:
            stack->top--;
            break;
        case OP_POP2:
            stack->top -= 2;
            break;
        case OP_DUP:
            stack->top++;
            STACK(0) = STACK(-1);
            break;
        case OP_DUP_X1:
            stack->top++;
            STACK(0) = STACK(-1);
            _StackTop(stack)[-1] = _StackTop(stack)[-2];
            _StackTop(stack)[-2] = _StackTop(stack)[0];
            break;
        case OP_DUP_X2:
            stack->top++;
            STACK(0) = STACK(-1);
            _StackTop(stack)[-1] = _StackTop(stack)[-2];
            _StackTop(stack)[-2] = _StackTop(stack)[-3];
            _StackTop(stack)[-3] = _StackTop(stack)[0];
            break;
        case OP_DUP2:
            stack->top += 2;
            STACK(0) = STACK(-2);
            _StackTop(stack)[-1] = _StackTop(stack)[-3];
            break;
        case OP_DUP2_X1:
            stack->top += 2;
            STACK(0) = STACK(-2);
            _StackTop(stack)[-1] = _StackTop(stack)[-3];
            _StackTop(stack)[-2] = _StackTop(stack)[-4];
            _StackTop(stack)[-3] = _StackTop(stack)[0];
            _StackTop(stack)[-4] = _StackTop(stack)[-1];
            break;
        case OP_DUP2_X2:
            stack->top += 2;
            STACK(0) = STACK(-2);
            _StackTop(stack)[-1] = _StackTop(stack)[-3];
            _StackTop(stack)[-2] = _StackTop(stack)[-4];
            _StackTop(stack)[-3] = _StackTop(stack)[-5];
            _StackTop(stack)[-4] = _StackTop(stack)[0];
            _StackTop(stack)[-5] = _StackTop(stack)[-1];
            break;
        case OP_SWAP: {
            VMSlot top = STACK(0);

            STACK(0) = STACK(-1);
            _StackTop(stack)[-1] = top;
            break;
        }
        case OP_IADD: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i + right->i;
            stack->top--;
            break;
        }
        case OP_FADD: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->f = left->f + right->f;
            stack->top--;
            break;
        }
        case OP_ISUB: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i - right->i;
            stack->top--;
            break;
        }
        case OP_FSUB: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->f = left->f - right->f;
            stack->top--;
            break;
        }
        case OP_IMUL: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i * right->i;
            stack->top--;
            break;
        }
        case OP_FMUL: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->f = left->f * right->f;
            stack->top--;
            break;
        }
        case OP_IDIV: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i / right->i;
            stack->top--;
            break;
        }
        case OP_FDIV: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->f = left->f / right->f;
            stack->top--;
            break;
        }
        case OP_IREM: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i % right->i;
            stack->top--;
            break;
        }
        case OP_FREM: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->f = (int)left->f % (int)right->f;
            stack->top--;
            break;
        }
        case OP_INEG:
            STACK(0).i = -STACK(0).i;
            break;
        case OP_FNEG:
            STACK(0).f = -STACK(0).f;
            break;
        case OP_ISHL: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i << (right->i & 0x1F);
            stack->top--;
            break;
        }
        case OP_ISHR: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i >> (right->i & 0x1F);
            stack->top--;
            break;
        }
        case OP_IUSHR: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = (unsigned int)left->i >> (right->i & 0x1F);
            stack->top--;
            break;
        }
        case OP_IAND: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i & right->i;
            stack->top--;
            break;
        }
        case OP_IOR: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i | right->i;
            stack->top--;
            break;
        }
        case OP_IXOR: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            left->i = left->i ^ right->i;
            stack->top--;
            break;
        }
        case OP_IINC:
            locals->base[code[frame->pc]].i += (signed char)code[frame->pc + 1];
            frame->nextPc = frame->pc + 2;
            break;
        case OP_I2F:
            STACK(0).f = STACK(0).i;
            break;
        case OP_F2I:
            STACK(0).i = STACK(0).f;
            break;
        case OP_I2C:
            STACK(0).i = STACK(0).c;
            break;
        case OP_I2S:
            STACK(0).i = STACK(0).s;
            break;
        case OP_FCMPL: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            if (right->f < left->f) {
                left->i = 1;
            } else if (left->f == right->f) {
                left->i = 0;
            } else {
                left->i = -1;
            }
            stack->top--;
            break;
        }
        case OP_FCMPG: {
            VMSlot *right = &stack->base[stack->top];
            VMSlot *left = right - 1;

            if (right->f < left->f) {
                left->i = 1;
            } else if (left->f == right->f) {
                left->i = 0;
            } else {
                left->i = -1;
            }
            stack->top--;
            break;
        }
        case OP_IFEQ:
            if (STACK(0).i == 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IFNE:
            if (STACK(0).i != 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IFLT:
            if (STACK(0).i < 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IFGE:
            if (STACK(0).i >= 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IFGT:
            if (STACK(0).i > 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IFLE:
            if (STACK(0).i <= 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IF_ICMPEQ: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i == sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ICMPNE: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i != sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ICMPLT: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i < sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ICMPGT: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i > sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ICMPGE: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i >= sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ICMPLE: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i <= sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ACMPEQ: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i == sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_IF_ACMPNE: {
            VMSlot *sp = &stack->base[stack->top];

            if (sp[-1].i != sp[0].i) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top -= 2;
            break;
        }
        case OP_GOTO:
            BRANCH();
            break;
        case OP_JSR:
            stack->top++;
            STACK(0).i = frame->pc + 2;
            BRANCH();
            break;
        case OP_RET:
            frame->nextPc = locals->base[code[frame->pc]].i;
            break;
        case OP_TABLESWITCH: {
            unsigned char *table;
            int defaultOffset;
            int low;
            int high;
            int key;

            frame->pc = (frame->pc + 3) & ~3;
            table = &code[frame->pc];
            defaultOffset = CODE_S32(&table[0]);
            low = CODE_S32(&table[4]);
            high = CODE_S32(&table[8]);
            frame->pc += 12;
            key = STACK(0).i;
            stack->top--;
            if (key < low || key > high) {
                frame->nextPc = frame->nextPc + defaultOffset - 1;
            } else {
                frame->pc += (key - low) * 4;
                frame->nextPc = frame->nextPc + CODE_U16(2) - 1;
            }
            break;
        }
        case OP_LOOKUPSWITCH: {
            unsigned char *table;
            int defaultOffset;
            int pairs;
            int key;
            int i = 0;

            frame->pc = (frame->pc + 3) & ~3;
            table = &code[frame->pc];
            defaultOffset = CODE_S32(&table[0]);
            pairs = CODE_S32(&table[4]);
            frame->pc += 8;
            key = STACK(0).i;
            stack->top--;
            if (pairs > 0) {
                do {
                    unsigned char *pair = &code[frame->pc];
                    int match = CODE_S32(pair);

                    if (key == match) {
                        frame->nextPc = frame->nextPc + ((pair[6] << 8) | pair[7]) - 1;
                        break;
                    }
                    if (key < match) {
                        frame->nextPc = frame->nextPc + defaultOffset - 1;
                        break;
                    }
                    i++;
                    frame->pc += 8;
                } while (i < pairs);
            }
            if (i == pairs) {
                frame->nextPc = frame->nextPc + defaultOffset - 1;
            }
            break;
        }
        case OP_IRETURN:
        case OP_FRETURN:
        case OP_ARETURN:
            if (thread->frame_depth == 0) {
                thread->flags |= 8;
            } else {
                thread->flags |= 0x80;
            }
            *result = STACK(0);
            break;
        case OP_RETURN:
            if (thread->frame_depth == 0) {
                thread->flags |= 8;
            } else {
                thread->flags |= 0x80;
            }
            break;
        case OP_GETSTATIC: {
            VMFieldRef ref;

            if (getField(CODE_U16(0), method->owner, 1, &ref) == 1) {
                if (!(ref.field->type->type_flags & 0x100) || (ref.field->flags & 0x8000)) {
                    stack->top++;
                    STACK(0).i = *ref.field->intAddress;
                } else {
                    switch (ref.field->type->type_code) {
                    case 'I':
                        stack->top++;
                        STACK(0).i = *ref.field->intAddress;
                        break;
                    case 'F':
                        stack->top++;
                        STACK(0).f = *ref.field->floatAddress;
                        break;
                    case 'B':
                    case 'Z':
                        stack->top++;
                        STACK(0).b = *ref.field->byteAddress;
                        STACK(0).i = STACK(0).b;
                        break;
                    case 'C':
                    case 'S':
                        stack->top++;
                        STACK(0).s = *ref.field->shortAddress;
                        STACK(0).i = STACK(0).s;
                        break;
                    }
                }
            }
            frame->nextPc = frame->pc + 2;
            break;
        }
        case OP_PUTSTATIC: {
            VMFieldRef ref;

            if (getField(CODE_U16(0), method->owner, 1, &ref) == 1) {
                if (!(ref.field->type->type_flags & 0x100) || (ref.field->flags & 0x8000)) {
                    *ref.field->intAddress = STACK(0).i;
                    stack->top--;
                } else {
                    switch (ref.field->type->type_code) {
                    case 'I':
                        *ref.field->intAddress = STACK(0).i;
                        stack->top--;
                        break;
                    case 'F':
                        *ref.field->floatAddress = STACK(0).f;
                        stack->top--;
                        break;
                    case 'B':
                    case 'Z':
                        *ref.field->byteAddress = STACK(0).b;
                        stack->top--;
                        break;
                    case 'C':
                    case 'S':
                        *ref.field->shortAddress = STACK(0).s;
                        stack->top--;
                        break;
                    }
                }
            }
            frame->nextPc = frame->pc + 2;
            break;
        }
        case OP_GETFIELD: {
            VMFieldRef ref;

            if (getField(CODE_U16(0), method->owner, 0, &ref) == 1) {
                if (!(ref.field->type->type_flags & 0x100) || (ref.field->flags & 0x8000)) {
                    STACK(0).i = *(int *)((char *)STACK(0).ref + ref.field->offset);
                } else {
                    switch (ref.field->type->type_code) {
                    case 'I':
                        STACK(0).i = *(int *)((char *)STACK(0).ref + ref.field->offset);
                        break;
                    case 'F':
                        STACK(0).f = *(float *)((char *)STACK(0).ref + ref.field->offset);
                        break;
                    case 'B':
                    case 'Z':
                        STACK(0).b = *(unsigned char *)((char *)STACK(0).ref + ref.field->offset);
                        STACK(0).i = STACK(0).b;
                        break;
                    case 'C':
                    case 'S':
                        STACK(0).s = *(short *)((char *)STACK(0).ref + ref.field->offset);
                        STACK(0).i = STACK(0).s;
                        break;
                    }
                }
            }
            frame->nextPc = frame->pc + 2;
            break;
        }
        case OP_PUTFIELD: {
            VMFieldRef ref;

            if (getField(CODE_U16(0), method->owner, 0, &ref) == 1) {
                if (!(ref.field->type->type_flags & 0x100) || (ref.field->flags & 0x8000)) {
                    *(int *)((char *)_StackTop(stack)[-1].ref + ref.field->offset) = _StackTop(stack)[0].i;
                    stack->top -= 2;
                } else {
                    switch (ref.field->type->type_code) {
                    case 'B':
                    case 'C':
                    case 'F':
                    case 'I':
                    case 'S':
                    case 'Z':
                        *(int *)((char *)_StackTop(stack)[-1].ref + ref.field->offset) = _StackTop(stack)[0].i;
                        stack->top -= 2;
                        break;
                    case 'D':
                    case 'J':
                        stack->top -= 3;
                        break;
                    }
                }
            }
            frame->nextPc = frame->pc + 2;
            break;
        }
        case OP_INVOKEVIRTUAL:
            callee = getMethodSignatureClass(CODE_U16(0), method->owner, 0);
            frame->callee = callee;
            thread->frame_depth++;
            JNI_callMethod(thread, callee, &STACK(-callee->arg_count), &STACK(-callee->arg_count));
            if (thread->flags & 5) {
                return;
            }
            thread->frame_depth--;
            stack->top = stack->top - callee->arg_count - 1;
            stack->top = stack->top + callee->result_size;
            frame->nextPc = frame->pc + 2;
            break;
        case OP_INVOKESPECIAL:
            callee = getMethodSignatureClass(CODE_U16(0), method->owner, 1);
            frame->callee = callee;
            thread->frame_depth++;
            JNI_callMethod(thread, callee, &STACK(-callee->arg_count), &STACK(-callee->arg_count));
            if (thread->flags & 5) {
                return;
            }
            thread->frame_depth--;
            stack->top = stack->top - callee->arg_count - 1;
            frame->nextPc = frame->pc + 2;
            break;
        case OP_INVOKESTATIC:
            callee = getMethodSignatureClass(CODE_U16(0), method->owner, 0);
            frame->callee = callee;
            thread->frame_depth++;
            JNI_callMethod(thread, callee, &STACK(1 - callee->arg_count), &STACK(1 - callee->arg_count));
            if (thread->flags & 5) {
                return;
            }
            thread->frame_depth--;
            stack->top = stack->top - callee->arg_count;
            stack->top = stack->top + callee->result_size;
            frame->nextPc = frame->pc + 2;
            break;
        case OP_INVOKEINTERFACE:
            callee = getMethodSignatureClass(CODE_U16(0), method->owner, 1);
            frame->callee = callee;
            thread->frame_depth++;
            JNI_callMethod(thread, callee, &STACK(-callee->arg_count), &STACK(-callee->arg_count));
            if (thread->flags & 5) {
                return;
            }
            thread->frame_depth--;
            stack->top = stack->top - callee->arg_count - 1;
            stack->top = stack->top + callee->result_size;
            frame->nextPc = frame->pc + 4;
            break;
        case OP_NEW:
            stack->top++;
            STACK(0).ref = newObject(getClass(CODE_U16(0), method->owner));
            frame->nextPc = frame->pc + 2;
            break;
        case OP_NEWARRAY:
            STACK(0).ref = newArray(primitiveClassTable[code[frame->pc]], STACK(0).i);
            frame->nextPc = frame->pc + 1;
            break;
        case OP_ANEWARRAY:
            STACK(0).ref = newArray(getClass(CODE_U16(0), method->owner), STACK(0).i);
            frame->nextPc = frame->pc + 2;
            break;
        case OP_ARRAYLENGTH:
            STACK(0).i = ((VMArray *)STACK(0).ref)->length;
            break;
        case OP_CHECKCAST:
            frame->nextPc = frame->pc + 2;
            break;
        case OP_INSTANCEOF:
            STACK(0).i = JNI_isInstanceOf(STACK(0).ref, getClass(CODE_U16(0), method->owner));
            break;
        case OP_WIDE: {
            unsigned int index;

            /* Dispatches on code[pc - 1], the wide opcode itself, not the widened one. */
            switch (code[frame->pc - 1]) {
            case OP_ILOAD:
            case OP_FLOAD:
            case OP_ALOAD:
                index = CODE_U16(0);
                stack->top++;
                STACK(0) = locals->base[index];
                frame->nextPc = frame->pc + 2;
                break;
            case OP_ISTORE:
            case OP_FSTORE:
            case OP_ASTORE:
                index = CODE_U16(0);
                locals->base[index] = STACK(0);
                stack->top--;
                frame->nextPc = frame->pc + 2;
                break;
            case OP_RET:
                frame->nextPc = locals->base[CODE_U16(0)].i;
                break;
            case OP_IINC:
                index = CODE_U16(0);
                locals->base[index].i += (short)CODE_U16(2);
                frame->nextPc = frame->pc + 4;
                break;
            }
            break;
        }
        case OP_IFNULL:
            if (STACK(0).ref == 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_IFNONNULL:
            if (STACK(0).ref != 0) {
                BRANCH();
            } else {
                frame->nextPc = frame->pc + 2;
            }
            stack->top--;
            break;
        case OP_GOTO_W:
            frame->nextPc = frame->pc + CODE_U16(2) - 1;
            break;
        case OP_JSR_W:
            stack->top++;
            STACK(0).i = frame->pc + 4;
            frame->nextPc = frame->pc + CODE_U16(2) - 1;
            break;
        }
        flags = thread->flags;
        if (flags & 5) {
            return;
        }
        if (flags & 0x88) {
            thread->flags = flags & ~0x80;
            break;
        }
        frame->pc = frame->nextPc;
    }

    thread->stack_offset = thread->stack_offset - (method->max_locals + method->max_stack) - 1;
    if ((short)thread->stack_offset < 0) {
        JNI_threadException(thread, method);
    }
    frame->pc = 0;
    frame->nextPc = 0;
}

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
