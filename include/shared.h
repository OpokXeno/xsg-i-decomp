#ifndef INCLUDE_SHARED_H
#define INCLUDE_SHARED_H

typedef unsigned char u8;

typedef unsigned int u32;

struct request {
    u8 mode;
    u8 data;
};

typedef unsigned short u16;

typedef unsigned long long u64;

typedef struct XglPacket XglPacket;

/*
 * classJava_xeno_Chr/classJava_xeno_Unit/JavaField/lookupClassField are
 * already canonical in include/xeno/math/{types,functions}.h. They cannot
 * be reused via #include here: that header pulls in math's types.h, whose
 * generated content independently redefines struct StudioCamera/request/
 * topArg and enum TOP_REQ, already defined by include/xeno/core/types.h
 * (both domains harvested main-00201350/main-00244d20) -- a hard
 * redefinition conflict unrelated to this function. Declared verbatim
 * identical to the accepted math spelling instead (header_divergence:
 * cross-domain header generation conflict, not a differing local view).
 */
typedef struct JavaField {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    int offset;        /* +16: read at 0x2f9438/0x2f94ac/... (lw v1,16(v0)) */
} JavaField;

/* JavaField.flags: the field's type is still a signature, not a resolved class. */
#define JAVA_FIELD_UNRESOLVED 0x8000
#define JAVA_FIELD_FLAGS_MASK 0x7fff /* all flags except JAVA_FIELD_UNRESOLVED */

typedef unsigned char SceneByte;

typedef struct JavaClass JavaClass;

typedef struct SceneString SceneString;

/* A VM object reference (the JNI jobject): a pointer to an object header
 * whose first word is its class. The original type aliased `void *`: the
 * sequence stores of ACT_createChr/ACT_createNPC (near_dir) and the JThread
 * object/script argument slots schedule only under void*'s alias set, not
 * under a char-pointer one. Field slots are reached by byte offset
 * (field->location.offset), so SceneByte views remain for that. */
typedef void *SceneObject;

typedef struct {
    unsigned char status;
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
} XglClock;

typedef struct ObjectTask ObjectTask;

typedef signed short s16;

typedef struct RgHeap RgHeap;

typedef struct {
    u8 pad_00[0x28];
    u16 half_28;
    u16 half_2a;
    unsigned char unmodeled_2c[4];
    u16 half_30;
    u16 mappedPressed;
    u16 mappedRepeat;
} PadPrefix;

/* Controller button masks (libpad layout) of the button and direction halfwords
   of PadPrefix/XglPadRecord. */
#define PAD_L2       0x0001
#define PAD_R2       0x0002
#define PAD_L1       0x0004
#define PAD_R1       0x0008
#define PAD_TRIANGLE 0x0010
#define PAD_CIRCLE   0x0020
#define PAD_CROSS    0x0040
#define PAD_SQUARE   0x0080
#define PAD_SELECT   0x0100
#define PAD_L3       0x0200
#define PAD_R3       0x0400
#define PAD_START    0x0800
#define PAD_UP       0x1000
#define PAD_RIGHT    0x2000
#define PAD_DOWN     0x4000
#define PAD_LEFT     0x8000

/* EE scratchpad RAM. */
#define SCRATCHPAD_BASE 0x70000000

/* Held SELECT with START newly pressed leaves a test screen (the pad snapshot
 * packs held halfword +40, then newly pressed halfword +42). */
#define PAD_EXIT_COMBO 0x08000100ULL

/* PS2 sceOpen flags. */
#define SCE_RDONLY 1
#define SCE_WRONLY 2
#define SCE_RDWR 3

/* Height Get_Height / UnduGet / UnduCheck return, and a LadderStep rung holds,
 * when there is no ground. */
#define GROUND_HEIGHT_NONE (-1000.0f)

/* Number of MapUnit[] slots (map_2.h's MapUnit table). */
#define MAP_UNIT_COUNT 64

/* End of the 32 MiB EE RAM (scene archives load right below it). */
#define EE_RAM_END 0x02000000

/* "FPK" in the low three bytes of FpkFcvHeader.magic (little-endian). */
#define FPK_MAGIC 0x004B5046
#define FPK_MAGIC_MASK 0x00FFFFFF

/* Footstep sound ids Sound_FootStep (set_motion.c) detects: a walk or run
 * stride has two footfalls; act_3.c's foot_tbl holds two contact frames per gait. */
#define FOOT_STEP_WALK_A 0x20001
#define FOOT_STEP_WALK_B 0x20002
#define FOOT_STEP_RUN_A 0x20003
#define FOOT_STEP_RUN_B 0x20004

/* EUC-JP: first byte of a two-byte character (0xA0 and up). */
#define EUC_FIRST_MULTIBYTE_LEAD 160

/* Entries of ItemBoxTbl (create_evt_item_get_task.c), indexed by itemNo. */
#define ITEM_BOX_COUNT 601

/* MenuWork.flags (window_tex_load.h): the menu state changed this frame. */
#define MENU_FLAG_ENTER 0x01

/* Menu sound effects (xglSoundEffectNormalID / xglSoundEffectNormalDirect ids). */
#define SE_DECIDE 1
#define SE_CANCEL 2
#define SE_CURSOR 3
#define SE_BUZZER 5

/*
 * The helper ABI uses a four-float vector record.  Its pinned implementation
 * loads all four storage slots but computes the length from xyz only, so w is
 * storage for this call rather than an input to the distance test.
 */
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

/*
 * The same vector with the quadword alignment of the SDK's sceVu0FVECTOR
 * (libvu0.h: aligned(16)); whole-vector copies of it are two ld/sd
 * doublewords, while plain Vector4 copies use ldl/ldr. User 2026-11-01.
 */
typedef Vector4 AlignedVector4 __attribute__((aligned(16)));

/*
 * The same vector with doubleword alignment: objects the original keeps only
 * 8-aligned (e.g. pooled local initializers at 8 mod 16) that are still
 * copied as two ld/sd doublewords, without a union. User 2026-11-02.
 */
typedef Vector4 DoublewordAlignedVector4 __attribute__((aligned(8)));

/*
 * A vector the original stores only 8-aligned (its objects sit at 8 mod 16)
 * but still copies as two ld/sd doublewords: the vector shares storage with
 * its two doubleword words. The u64 member keeps the 8-byte alignment in
 * arrays and through pointers too. User 2026-11-01.
 */
typedef union DoublewordVector4 {
    Vector4 vector;
    u64 words[2];
} DoublewordVector4;

typedef float Matrix4[4][4];

typedef struct XglTaskPrefix XglTaskPrefix;

typedef struct XglTaskScheduler XglTaskScheduler;

/*
 * Only the link/callback prefix is evidenced by this allocation.  The
 * complete task records are allocated with a 0x80-byte stride, and callers
 * write task-specific payload beyond this 0x10-byte prefix.  This declaration
 * must not be read as the complete task layout or allocation size.
 */
struct XglTaskPrefix {
    XglTaskScheduler *scheduler;
    XglTaskPrefix *next;
    XglTaskPrefix *previous;
    int (*callback)(XglTaskPrefix *task);
};

struct XglTaskScheduler {
    XglTaskPrefix *free_tasks;
    XglTaskPrefix *active_head;
    XglTaskPrefix *active_tail;
    XglTaskPrefix *next_to_visit;
};

typedef struct StudioCamera {
    u32 active;
    u32 state;
    u8 unmodeled_08[0x18];
    float nearClip;
    float farClip;
    u8 unmodeled_28[0x48];
    Vector4 screenOffset;
    Vector4 screenScale;
    u8 unmodeled_90[4];
    float fov;
    u8 unmodeled_98[8];
    DoublewordVector4 rotation;
    Vector4 angleVelocity;
    u8 unmodeled_c0[0x10];
    DoublewordVector4 position;
    u8 unmodeled_e0[0x40];
    DoublewordVector4 chasePosition;
    u8 unmodeled_130[0x40];
    Matrix4 cameraToWorldMatrix;
    Matrix4 occlusionTransform; /* 0x1B0: read by face_point.c's setup_occlusion/check_occlusion */
    u8 unmodeled_1f0[0x280];
    Matrix4 viewMatrix;
    u8 unmodeled_4b0[0x40];
    Matrix4 clipMatrices[2]; /* 0x4F0: the pair _ModelCalcClipInit loads into vf12..vf19 */
    u8 unmodeled_570[0x80];
} StudioCamera;

typedef struct SceneField SceneField;

typedef struct JavaMethod JavaMethod;

typedef struct DispatchTable DispatchTable;

typedef union ConstantPoolEntry ConstantPoolEntry;

/* include/main/jni.h: the VM thread and its operand slots. */
struct JThread;
union VMSlot;

typedef struct SceneStringStorage SceneStringStorage;

/*
 * The word every heap object (instances, strings, arrays, class records)
 * starts with: the dispatch table of its class. newObject stores the class's
 * classPointer here; JNI_isInstanceOf, SCENE_start/SCENE_cleanup and the
 * script observers read the class back as header->classPointer->clazz.
 */
typedef struct ObjectHeader {
    DispatchTable *classPointer; /* +0x0 */
} ObjectHeader;

/*
 * The VM's own text record: a length and the bytes it counts.
 *
 * SCENE_start reads both of them out of the storage a java.lang.String hands
 * it and passes them straight to loadConstString (lw 4 and lw 8 at
 * 0x0025a7f4/0x0025a7f8), whose second parameter is the byte count -1 stands
 * for elsewhere.  Word 0 is read by nothing and stays a byte range.
 */
struct SceneStringStorage {
    unsigned char unmodeled_00[4];  /* +0x00 */
    int length;                     /* +0x04 */
    const char *bytes;              /* +0x08 */
};

/*
 * A java.lang.String instance: the object header, then the text record the
 * string's characters live in (lw 4 at 0x0025a7ec).
 *
 * The same object is the interned name key of classes, methods, fields and
 * type descriptors (loadConstString): +0x04 holds the key's 16-bit hash
 * (ClassCacheEntry bucket index, lookupClassEntry) and +0x08 the key's byte
 * pointer, which DB_evtMonitor prints as "%s.%s" for a class name and a
 * method name and SCENE_instance passes to getClassFromSignature for a field
 * descriptor. The two views read +0x04 as different types, so it is a union.
 */
struct SceneString {
    ObjectHeader header;          /* +0x00 */
    union {
        struct SceneStringStorage *storage;  /* +0x04: the VM's text record */
        u16 hash;                            /* +0x04: the interned key's hash */
    };
    const char *bytes;            /* +0x08: the interned key's name bytes */
};

/* One 20-byte field entry of JavaClass.fields: static fields first. */
struct SceneField {
    SceneString *name; /* +0: the key lookupClassField compares per entry */
    unsigned short flags;
    unsigned char unmodeled_06[2];
    /* +8: the type descriptor while flags bit 0x8000 is set, then the class
       getClassFromSignature resolved it to. */
    union {
        SceneString *descriptor;
        JavaClass *resolved;
    } type;
    union { int constant_index; int size; int value; } payload; /* +12 */
    union { void *address; int offset; int initial_value; } location; /* +16 */
};

/*
 * One 32-byte method entry of JavaClass.methods (JVM class file method_info
 * as addMethod/addCode build it, virtualMachine runs it): name and
 * descriptor, the argument slot count without the receiver and the result
 * slot count methodDescripter computes, the declaring class (its constant
 * pool resolves the method's operands), access flags, the dispatch table
 * slot buildDispatchMethodTable assigns (-1: static or constructor), the
 * bytecode or, for an ACC_NATIVE method, the native implementation
 * resolveNativeMethod finds, max_stack/max_locals and a 16-bit code length
 * holding the low half of the classfile's 32-bit attribute length.
 */
struct JavaMethod {
    SceneString *name;              /* +0x00 */
    SceneString *descriptor;        /* +0x04 */
    s16 parameterSize;              /* +0x08 */
    s16 returnSize;                 /* +0x0a */
    JavaClass *owner;               /* +0x0c */
    u16 accessFlags;                /* +0x10 */
    s16 dispatchIndex;              /* +0x12 */
    union {
        unsigned char *code;        /* +0x14: bytecode */
        void (*native)(struct JThread *thread, union VMSlot *arguments,
                       union VMSlot *result); /* +0x14: ACC_NATIVE body */
    };
    u16 maxStack;                   /* +0x18 */
    u16 maxLocals;                  /* +0x1a */
    u16 codeLength;                 /* +0x1c */
    u16 unmodeled_1e;
};

/*
 * A class's virtual method table (buildDispatchMethodTable): the class it
 * belongs to, then one method per dispatchIndex. Every object's header
 * points at its class's table.
 */
struct DispatchTable {
    JavaClass *clazz;               /* +0x0 */
    JavaMethod *methods[1];         /* +0x4 */
};

/*
 * One class constant pool slot. Entry 0 holds the tag byte table; every
 * other entry holds its constant's value, its resolved object (tags 0x17
 * class, 0x18 string) or, before resolution, the classfile's index pair.
 */
union ConstantPoolEntry {
    u8 *tags;
    void *object;
    int intValue;
    float floatValue;
    u8 byteValue;
    u16 halfValue;
    struct StringConstant *stringConstant;
    struct StringInstance *stringObject;
    struct {
        u16 classIndex;
        u16 nameAndTypeIndex;
    } memberRef;
    struct {
        u16 nameIndex;
        u16 typeIndex;
    } nameAndType;
};

/*
 * The VM class record (0x40 bytes, newClass), shared by the class loader,
 * the interpreter and every native that reads an object's class.
 */
struct JavaClass {
    ObjectHeader header;            /* +0x00 */
    SceneString *name;              /* +0x04 */
    u16 accessFlags;                /* +0x08 */
    u16 flags;                      /* +0x0a: bit 0x100 primitive class */
    void *classLoader;              /* +0x0c */
    /* +0x10: the classfile's super_class constant pool index (setupClass)
       until processClass resolves it; findMethod walks the resolved chain. */
    union {
        JavaClass *superClass;
        u32 superClassIndex;
    };
    ConstantPoolEntry *constants;   /* +0x14 */
    DispatchTable *classPointer;    /* +0x18: the header word of an instance */
    SceneField *fields;             /* +0x1c */
    JavaMethod *methods;            /* +0x20 */
    JavaClass **interfaces;         /* +0x24 */
    u16 constantCount;              /* +0x28 */
    u16 dispatchSize;               /* +0x2a */
    u16 methodCount;                /* +0x2c */
    u16 interfaceCount;             /* +0x2e */
    u16 fieldCount;                 /* +0x30 */
    u16 staticFieldCount;           /* +0x32 */
    u16 declaredInterfaceCount;     /* +0x34 */
    s16 state;                      /* +0x36 */
    /* +0x38: a primitive class (flags bit 0x100) has no instances; its
       descriptor character and element byte size live in these bytes
       (initWrapperClass). */
    union {
        int instanceSize;
        struct {
            unsigned char unmodeled_38;
            signed char typeCode;
            unsigned char elementSize;
        };
    };
    JavaClass *elementClass;        /* +0x3c */
};

typedef struct SceneType SceneType;

typedef void SceneVm;

typedef struct TskObject TskObject;

typedef void (*TskObjectWorker)(TskObject *task, void *data);

struct TskObject {
    XglTaskPrefix base;
    unsigned char state;
    TskObjectWorker worker;
    void *data;
};

typedef signed long long PartySkillPoints;

typedef struct PartySkillLevelArray {
    unsigned char level_by_character[8];
} PartySkillLevelArray;

typedef struct SchedulerState SchedulerState;

typedef void (*ObjectTaskCallback)(ObjectTask *task);

struct ObjectTask {
    XglTaskPrefix task; /* the scheduler header xglTaskEntryNext maintains. */
    void *work; /* objRemove passes this slot to objWorkFree. */
};

/* Only an opaque handle crosses this bounded source slice. */
typedef struct RgSimpleDB RgSimpleDB;

typedef struct RgChar RgChar;
typedef struct RgCharMgr RgCharMgr;

struct stat;

typedef struct StreamXssBuffer StreamXssBuffer;

typedef struct StreamRing StreamRing;

typedef unsigned char byte;

typedef struct ConstString ConstString;

typedef unsigned char DataBufferByte;

typedef unsigned int DataBufferWord;

typedef struct DataBuffer DataBuffer;

typedef DataBufferWord (*DataBufferReader)(DataBuffer *, int);

typedef struct __sFILE FILE;

typedef struct RgFileSys RgFileSys;

typedef struct RgFileSysData RgFileSysData;

struct RgFileSysData {
    void *data;
    unsigned int size;
    unsigned int mode;
    RgFileSys *owner;
    unsigned int ref_count;
    unsigned int allocated;
    RgFileSysData *dupOf;
    char name[0x60 - 0x1C];
};

typedef struct {
    float x;
    float y;
    float z;
    float w;
} HomogeneousVector;

/* The containing actor's complete historical layout is not established here.
 * These routines use the individually evidenced actor offsets in candidate.c. */
typedef struct SequenceActor SequenceActor;

typedef float f32;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} HermiteVector;

typedef float RgMatrix[16];

/* The complete geometry definition is outside this allocation.  These two
 * matrix members are the fields directly evidenced by the allocated accessors
 * at byte offsets 0x20 and 0x60. */
typedef struct RgGeom RgGeom;

/* Only the pointer identity and the two observed vector offsets are known. */
typedef struct RgGeomPoint RgGeomPoint;

typedef float RgVector[4];

/*
 * The vertex color XrgPaint2DColor copies into a 2D paint request as one
 * quadword (the default is 0x7F in all four channels); RgFontColor is this
 * type. Integer components, never floats.
 */
typedef struct XrgPaint2DRGBA {
    int r;
    int g;
    int b;
    int a;
} XrgPaint2DRGBA;

typedef struct RgStatus RgStatus;

extern int WakeupThread(int thread_id);

extern void sceVif1PkRef(XglPacket *packet, const void *environment,
                         int count, int mode, int offset, int flags);

extern char *strcat(char *destination, const char *source);

/* A 128-bit quadword (one lq/sq), as the VIF1 packet writers take it. */
typedef unsigned int Quadword __attribute__((mode(TI)));

extern void sceVif1PkInit(XglPacket *packet, unsigned int base);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAlign(XglPacket *packet, int align, int size);
extern void sceVif1PkAddCode(XglPacket *packet, unsigned int code);
extern int sceVif1PkSize(XglPacket *packet);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenGifTag(XglPacket *packet, Quadword tag);
extern void sceVif1PkCloseGifTag(XglPacket *packet);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void sceVif1PkAddGsAD(XglPacket *packet, int reg, u64 value);
extern void sceVif1PkAddGsPacked(XglPacket *packet, Quadword value);

extern int sprintf(char *destination, const char *format, ...);

int write(int file_descriptor, char *buffer, int length);

int read(int file_descriptor, char *buffer, int length);

int open();

int close(int fd);

int stat(const char *path, struct stat *status);

extern u8 *buffer;

extern float defaultOffset[];

extern float sinf(float angle);

extern void *memset(void *destination, int value, unsigned int count);

extern void *memcpy(void *destination, const void *source, unsigned int count);
extern unsigned int strlen(const char *string);
extern char *strcpy(char *destination, const char *source);
extern char *strncpy(char *destination, const char *source, unsigned int count);
extern int strcmp(const char *left, const char *right);
extern int strncmp(const char *left, const char *right, unsigned int count);
extern int strcasecmp(const char *left, const char *right);
extern int atoi(const char *string);
extern double atof(const char *string);
extern int rand(void);
extern float cosf(float angle);
extern float atan2f(float y, float x);
extern float floorf(float value);
extern double atan2(double y, double x);

extern void XrgInvMatrix(RgMatrix destination, const RgMatrix source);

#include "umbrella/public.h"

#endif /* INCLUDE_SHARED_H */
